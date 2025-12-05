/*
 * Copyright (c) 2023, Emna Rekik
 * Copyright (c) 2024, Nordic Semiconductor
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "HTTPWebsocket.h"
#include "DHCPClient.h"
#include "ws.h"

#include <stdio.h>
#include <zephyr/net/net_ip.h>
#include <zephyr/net/socket.h>
#include <zephyr/net/dhcpv4.h>
#include <zephyr/logging/log.h>

LOG_MODULE_REGISTER(httpwebsocket, LOG_LEVEL_DBG);

// Static web resources
static uint8_t index_html_gz[] = {
#include "index.html.gz.inc"
};

static uint8_t main_js_gz[] = {
#include "main.js.gz.inc"
};

static uint8_t WebsocketBuffer[1024];

// HTTP resource definitions
static struct http_resource_detail_static HTMLResource = {
    .common = {
        .type = HTTP_RESOURCE_TYPE_STATIC,
        .bitmask_of_supported_http_methods = BIT(HTTP_GET),
        .content_encoding = "gzip",
        .content_type = "text/html",
    },
    .static_data = index_html_gz,
    .static_data_len = sizeof(index_html_gz),
};

static struct http_resource_detail_static MainJSResource = {
    .common = {
        .type = HTTP_RESOURCE_TYPE_STATIC,
        .bitmask_of_supported_http_methods = BIT(HTTP_GET),
        .content_encoding = "gzip",
        .content_type = "text/javascript",
    },
    .static_data = main_js_gz,
    .static_data_len = sizeof(main_js_gz),
};

struct http_resource_detail_websocket WSEcho = {
    .common = {
        .type = HTTP_RESOURCE_TYPE_WEBSOCKET,
        // We need HTTP/1.1 Get method for upgrading
        .bitmask_of_supported_http_methods = BIT(HTTP_GET),
    },
    .cb = ws_echo_setup,
    .data_buffer = WebsocketBuffer,
    .data_buffer_len = sizeof(WebsocketBuffer),
    .user_data = NULL, // Fill this for any user specific data
};

// HTTP service configuration
static uint16_t test_http_service_port = CONFIG_NET_SAMPLE_HTTP_SERVER_SERVICE_PORT;

HTTP_SERVICE_DEFINE(test_http_service, NULL, &test_http_service_port, 1,
                    10, NULL, NULL);

HTTP_RESOURCE_DEFINE(index_html_gz_resource, test_http_service, "/",
                     &HTMLResource);

HTTP_RESOURCE_DEFINE(main_js_gz_resource, test_http_service, "/main.js",
                     &MainJSResource);

HTTP_RESOURCE_DEFINE(WSEchoResource, test_http_service, "/ws_echo", &WSEcho);

// Network interface management
static struct net_if *Interface = NULL;

void httpwebsocket_interface_init(void)
{
    Interface = net_if_get_default();
}

void httpwebsocket_start_dhcp(void)
{
    if (Interface) {
        net_dhcpv4_start(Interface);
    }
}

void httpwebsocket_set_static_ip(void)
{
    printk("Use Static IP\n");

    if (!Interface) {
        LOG_ERR("Network interface not initialized");
        return;
    }

    /* Stop DHCP and drop lease */
    net_dhcpv4_stop(Interface);

    /* Optional: configure static IPv4 */
    struct in_addr addr, netmask, gw;

    inet_pton(AF_INET, "192.168.1.100", &addr);
    inet_pton(AF_INET, "255.255.255.0", &netmask);
    inet_pton(AF_INET, "192.168.1.1", &gw);

    net_if_ipv4_addr_add(Interface, &addr, NET_ADDR_MANUAL, 0);
    net_if_ipv4_set_netmask_by_addr(Interface, &addr, &netmask);
    net_if_ipv4_set_gw(Interface, &gw);
}

void httpwebsocket_static_ip_mode(void)
{
    httpwebsocket_interface_init();
    http_server_stop();
    httpwebsocket_set_static_ip();
}

void httpwebsocket_init(void)
{
    httpwebsocket_interface_init();
    LOG_INF("HTTP WebSocket server initialized");
}

int httpwebsocket_start(void)
{
    int ret = http_server_start();
    if (ret < 0) {
        LOG_ERR("Failed to start HTTP server: %d", ret);
        return ret;
    }

    LOG_INF("HTTP WebSocket server started on port %d", test_http_service_port);
    return 0;
}

int httpwebsocket_stop(void)
{
    int ret = http_server_stop();
    if (ret < 0) {
        LOG_ERR("Failed to stop HTTP server: %d", ret);
        return ret;
    }

    LOG_INF("HTTP WebSocket server stopped");
    return 0;
}
