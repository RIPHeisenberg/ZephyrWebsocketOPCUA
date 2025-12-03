/*
 * Copyright (c) 2025
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "HTTPServer.h"
#include <stdio.h>
#include <string.h>
#include <zephyr/kernel.h>
#include <zephyr/net/net_if.h>
#include <zephyr/net/net_mgmt.h>
#include <zephyr/net/dhcpv4.h>
#include <zephyr/net/tls_credentials.h>
#include <zephyr/net/http/server.h>
#include <zephyr/net/http/service.h>
#include <zephyr/net/net_ip.h>
#include <zephyr/net/socket.h>

#include <zephyr/logging/log.h>
LOG_MODULE_REGISTER(http_server_module, LOG_LEVEL_DBG);

/* Include certificates if HTTPS is enabled */
#if defined(CONFIG_NET_SAMPLE_HTTPS_SERVICE)
#include "certificate.h"
#endif

static struct net_mgmt_event_callback mgmt_cb;
static struct k_sem dhcp_sem;
static bool dhcp_configured = false;
static struct http_server_config server_config;

/**
 * @brief Network management event handler
 */
static void net_mgmt_event_handler(struct net_mgmt_event_callback *cb,
				  uint32_t mgmt_event,
				  struct net_if *iface)
{
	switch (mgmt_event) {
	case NET_EVENT_IPV4_DHCP_BOUND:
		LOG_INF("IPv4 DHCP bound");
		dhcp_configured = true;
		k_sem_give(&dhcp_sem);
		break;
	case NET_EVENT_IPV4_ADDR_ADD:
		LOG_INF("IPv4 address added to interface");
		break;
	case NET_EVENT_IPV4_ADDR_DEL:
		LOG_INF("IPv4 address removed from interface");
		dhcp_configured = false;
		break;
	case NET_EVENT_IPV4_DHCP_START:
		LOG_INF("IPv4 DHCP started");
		break;
	case NET_EVENT_IPV4_DHCP_STOP:
		LOG_INF("IPv4 DHCP stopped");
		break;
	default:
		break;
	}
}

/**
 * @brief Configure network interface with static IP
 */
static int configure_static_ip(struct net_if *iface, const struct http_server_config *config)
{
	struct in_addr addr;
	struct in_addr netmask;
	struct in_addr gw;
	int ret;

	if (!config->ipv4_addr) {
		LOG_ERR("Static IP address is required");
		return -EINVAL;
	}

	/* Parse and set IPv4 address */
	ret = inet_pton(AF_INET, config->ipv4_addr, &addr);
	if (ret != 1) {
		LOG_ERR("Invalid IPv4 address: %s", config->ipv4_addr);
		return -EINVAL;
	}

	ret = net_if_ipv4_addr_add(iface, &addr, NET_ADDR_MANUAL, 0);
	if (ret < 0) {
		LOG_ERR("Failed to add IPv4 address: %d", ret);
		return ret;
	}

	/* Set netmask if provided */
	if (config->ipv4_netmask) {
		ret = inet_pton(AF_INET, config->ipv4_netmask, &netmask);
		if (ret == 1) {
			net_if_ipv4_set_netmask(iface, &netmask);
		} else {
			LOG_WRN("Invalid netmask, using default");
		}
	}

	/* Set gateway if provided */
	if (config->ipv4_gw) {
		ret = inet_pton(AF_INET, config->ipv4_gw, &gw);
		if (ret == 1) {
			net_if_ipv4_set_gw(iface, &gw);
		} else {
			LOG_WRN("Invalid gateway address");
		}
	}

	LOG_INF("Static IP configuration applied: %s", config->ipv4_addr);
	return 0;
}

/**
 * @brief Configure network interface with DHCP
 */
static int configure_dhcp(struct net_if *iface)
{
	int ret;

	/* Initialize DHCP semaphore */
	k_sem_init(&dhcp_sem, 0, 1);

	/* Register network management event callback */
	net_mgmt_init_event_callback(&mgmt_cb, net_mgmt_event_handler,
				     NET_EVENT_IPV4_DHCP_BOUND |
				     NET_EVENT_IPV4_ADDR_ADD |
				     NET_EVENT_IPV4_ADDR_DEL |
				     NET_EVENT_IPV4_DHCP_START |
				     NET_EVENT_IPV4_DHCP_STOP);

	net_mgmt_add_event_callback(&mgmt_cb);

	/* Start DHCP client */
	ret = net_dhcpv4_start(iface);
	if (ret < 0) {
		LOG_ERR("Failed to start DHCPv4 client: %d", ret);
		net_mgmt_del_event_callback(&mgmt_cb);
		return ret;
	}

	LOG_INF("DHCP client started, waiting for IP address...");

	/* Wait for DHCP to complete (timeout: 30 seconds) */
	ret = k_sem_take(&dhcp_sem, K_SECONDS(30));
	if (ret < 0) {
		LOG_ERR("DHCP timeout");
		net_dhcpv4_stop(iface);
		net_mgmt_del_event_callback(&mgmt_cb);
		return -ETIMEDOUT;
	}

	if (!dhcp_configured) {
		LOG_ERR("DHCP configuration failed");
		net_mgmt_del_event_callback(&mgmt_cb);
		return -ENONET;
	}

	LOG_INF("DHCP configuration completed successfully");
	return 0;
}

int http_server_setup_tls(void)
{
#if defined(CONFIG_NET_SAMPLE_HTTPS_SERVICE)
#if defined(CONFIG_NET_SOCKETS_SOCKOPT_TLS)
	int err;

	err = tls_credential_add(HTTP_SERVER_CERTIFICATE_TAG,
				 TLS_CREDENTIAL_SERVER_CERTIFICATE,
				 server_certificate,
				 sizeof(server_certificate));
	if (err < 0) {
		LOG_ERR("Failed to register public certificate: %d", err);
		return err;
	}

	err = tls_credential_add(HTTP_SERVER_CERTIFICATE_TAG,
				 TLS_CREDENTIAL_PRIVATE_KEY,
				 private_key, sizeof(private_key));
	if (err < 0) {
		LOG_ERR("Failed to register private key: %d", err);
		return err;
	}

#if defined(CONFIG_MBEDTLS_KEY_EXCHANGE_PSK_ENABLED)
	err = tls_credential_add(PSK_TAG,
				 TLS_CREDENTIAL_PSK,
				 psk,
				 sizeof(psk));
	if (err < 0) {
		LOG_ERR("Failed to register PSK: %d", err);
		return err;
	}

	err = tls_credential_add(PSK_TAG,
				 TLS_CREDENTIAL_PSK_ID,
				 psk_id,
				 sizeof(psk_id) - 1);
	if (err < 0) {
		LOG_ERR("Failed to register PSK ID: %d", err);
		return err;
	}
#endif /* defined(CONFIG_MBEDTLS_KEY_EXCHANGE_PSK_ENABLED) */

	LOG_INF("TLS credentials configured successfully");
#endif /* defined(CONFIG_NET_SOCKETS_SOCKOPT_TLS) */
#else
	LOG_INF("HTTPS service not enabled, skipping TLS setup");
#endif /* defined(CONFIG_NET_SAMPLE_HTTPS_SERVICE) */
	return 0;
}

int http_server_init_and_start(const struct http_server_config *config)
{
	struct net_if *iface;
	int ret;

	if (!config) {
		LOG_ERR("Invalid configuration");
		return -EINVAL;
	}

	/* Store configuration */
	memcpy(&server_config, config, sizeof(struct http_server_config));

	/* Get default network interface */
	iface = net_if_get_default();
	if (!iface) {
		LOG_ERR("No network interface available");
		return -ENODEV;
	}

	LOG_INF("Configuring network interface...");

	/* Configure IP address */
	if (config->use_dhcp) {
		ret = configure_dhcp(iface);
		if (ret < 0) {
			LOG_ERR("DHCP configuration failed: %d", ret);
			return ret;
		}
	} else {
		ret = configure_static_ip(iface, config);
		if (ret < 0) {
			LOG_ERR("Static IP configuration failed: %d", ret);
			return ret;
		}
	}

	/* Setup TLS if HTTPS is enabled */
	if (config->enable_https) {
		ret = http_server_setup_tls();
		if (ret < 0) {
			LOG_ERR("TLS setup failed: %d", ret);
			return ret;
		}
	}

	/* Start HTTP server */
	LOG_INF("Starting HTTP server...");
	ret = http_server_start();
	if (ret < 0) {
		LOG_ERR("Failed to start HTTP server: %d", ret);
		return ret;
	}

	LOG_INF("HTTP server started successfully");

	/* Print IP information */
	char ip_str[16];
	ret = http_server_get_ip_info(ip_str, sizeof(ip_str));
	if (ret == 0) {
		LOG_INF("HTTP server accessible at:");
		if (config->enable_http) {
			LOG_INF("  HTTP:  http://%s:%d", ip_str, config->http_port);
		}
		if (config->enable_https) {
			LOG_INF("  HTTPS: https://%s:%d", ip_str, config->https_port);
		}
	}

	return 0;
}

int http_server_stop_service(void)
{
	int ret;

	LOG_INF("Stopping HTTP server...");

	ret = http_server_stop();
	if (ret < 0) {
		LOG_ERR("Failed to stop HTTP server: %d", ret);
		return ret;
	}

	/* Stop DHCP if it was used */
	if (server_config.use_dhcp && dhcp_configured) {
		struct net_if *iface = net_if_get_default();
		if (iface) {
			net_dhcpv4_stop(iface);
			net_mgmt_del_event_callback(&mgmt_cb);
			dhcp_configured = false;
		}
	}

	LOG_INF("HTTP server stopped successfully");
	return 0;
}

int http_server_get_ip_info(char *ipv4_addr, size_t ipv4_addr_len)
{
	struct net_if *iface;
	struct net_if_ipv4 *ipv4;

	if (!ipv4_addr || ipv4_addr_len < 16) {
		return -EINVAL;
	}

	iface = net_if_get_default();
	if (!iface) {
		return -ENODEV;
	}

	ipv4 = iface->config.ip.ipv4;
	if (!ipv4 || ipv4->unicast[0].addr_type == NET_ADDR_OVERRIDABLE) {
		return -ENONET;
	}

	net_addr_ntop(AF_INET, &ipv4->unicast[0].address.in_addr, 
		      ipv4_addr, ipv4_addr_len);

	return 0;
}