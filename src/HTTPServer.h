/*
 * Copyright (c) 2025
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef __HTTP_SERVER_H__
#define __HTTP_SERVER_H__

#include <zephyr/kernel.h>
#include <zephyr/net/net_ip.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief HTTP Server Configuration Structure
 */
struct http_server_config {
	/** IPv4 address to bind to (optional, can be NULL for DHCP) */
	const char *ipv4_addr;
	/** IPv4 gateway address (optional) */
	const char *ipv4_gw;
	/** IPv4 subnet mask (optional) */
	const char *ipv4_netmask;
	/** Use DHCP for IPv4 configuration */
	bool use_dhcp;
	/** HTTP port */
	uint16_t http_port;
	/** HTTPS port */
	uint16_t https_port;
	/** Enable HTTP service */
	bool enable_http;
	/** Enable HTTPS service */
	bool enable_https;
};

/**
 * @brief Initialize and start HTTP server with custom IP configuration
 *
 * This function configures the network interface with either static IP
 * or DHCP, sets up TLS credentials if needed, and starts the HTTP server.
 *
 * @param config Pointer to HTTP server configuration structure
 * @return 0 on success, negative error code on failure
 */
int http_server_init_and_start(const struct http_server_config *config);

/**
 * @brief Stop the HTTP server
 *
 * @return 0 on success, negative error code on failure
 */
int http_server_stop_service(void);

/**
 * @brief Get current network interface information
 *
 * @param ipv4_addr Buffer to store IPv4 address (minimum 16 bytes)
 * @param ipv4_addr_len Length of ipv4_addr buffer
 * @return 0 on success, negative error code on failure
 */
int http_server_get_ip_info(char *ipv4_addr, size_t ipv4_addr_len);

/**
 * @brief Setup TLS credentials for HTTPS
 *
 * @return 0 on success, negative error code on failure
 */
int http_server_setup_tls(void);

#ifdef __cplusplus
}
#endif

#endif /* __HTTP_SERVER_H__ */