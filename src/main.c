/*
 * Copyright (c) 2023, Emna Rekik
 * Copyright (c) 2024, Nordic Semiconductor
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdio.h>
#include <inttypes.h>

#include <zephyr/kernel.h>
#include <zephyr/net/tls_credentials.h>
#include <zephyr/net/http/server.h>
#include <zephyr/net/http/service.h>
#include <zephyr/net/net_ip.h>
#include <zephyr/net/socket.h>
#include "zephyr/device.h"
#include "zephyr/sys/util.h"
#include <zephyr/drivers/led.h>
#include <zephyr/data/json.h>
#include <zephyr/sys/util_macro.h>

#include <zephyr/net/net_if.h>
#include <zephyr/net/dhcpv4.h>
#include <zephyr/logging/log.h>

#include "DHCPClient.h"
#include "ws.h"


LOG_MODULE_REGISTER(net_http_server_sample, LOG_LEVEL_DBG);

static uint8_t index_html_gz[] = {
#include "index.html.gz.inc"
};

static uint8_t main_js_gz[] = {
#include "main.js.gz.inc"
};

static uint8_t WebsocketBuffer[1024];

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

static uint16_t test_http_service_port = CONFIG_NET_SAMPLE_HTTP_SERVER_SERVICE_PORT;

HTTP_SERVICE_DEFINE(test_http_service, NULL, &test_http_service_port, 1,
                    10, NULL, NULL);

HTTP_RESOURCE_DEFINE(index_html_gz_resource, test_http_service, "/",
                     &HTMLResource);

HTTP_RESOURCE_DEFINE(main_js_gz_resource, test_http_service, "/main.js",
                     &MainJSResource);

HTTP_RESOURCE_DEFINE(WSEchoResource, test_http_service, "/ws_echo", &WSEcho);

HTTP_RESOURCE_DEFINE(ws_echo_resource_https, test_https_service, "/ws_echo", &WSEcho);


#if defined(CONFIG_NET_SAMPLE_HTTPS_SERVICE)
#include "certificate.h"

static const sec_tag_t sec_tag_list_verify_none[] = {
    HTTP_SERVER_CERTIFICATE_TAG,
#if defined(CONFIG_MBEDTLS_KEY_EXCHANGE_PSK_ENABLED)
PSK_TAG,
#endif
};

static uint16_t test_https_service_port = CONFIG_NET_SAMPLE_HTTPS_SERVER_SERVICE_PORT;
HTTPS_SERVICE_DEFINE(test_https_service, NULL,
		     &test_https_service_port, 1, 10, NULL, NULL,
                     sec_tag_list_verify_none, sizeof(sec_tag_list_verify_none));

HTTP_RESOURCE_DEFINE(index_html_gz_resource_https, test_https_service, "/",
		     &index_html_gz_resource_detail);

HTTP_RESOURCE_DEFINE(main_js_gz_resource_https, test_https_service, "/main.js",
		     &main_js_gz_resource_detail);


#endif /* CONFIG_NET_SAMPLE_HTTPS_SERVICE */




static struct net_if *iface = NULL;

static void InterfaceInit(void)
{
    iface = net_if_get_default();
}

// Enable DHCP
void StartDHCP(void)
{
    net_dhcpv4_start(iface);
}

// Disable DHCP and optionally switch to static IP
void SetHTTPStatic(void)
{
    printk("Use Static IP\n");
    /* Stop DHCP and drop lease */
    net_dhcpv4_stop(iface);

    /* Optional: configure static IPv4 */
    struct in_addr addr, netmask, gw;

    inet_pton(AF_INET, "192.168.1.100", &addr);
    inet_pton(AF_INET, "255.255.255.0", &netmask);
    inet_pton(AF_INET, "192.168.1.1", &gw);

    net_if_ipv4_addr_add(iface, &addr, NET_ADDR_MANUAL, 0);
    net_if_ipv4_set_netmask_by_addr(iface, &addr, &netmask);
    net_if_ipv4_set_gw(iface, &gw);
}

void HTTPServer_StaticIP(void)
{
    InterfaceInit();
    http_server_stop();
    SetHTTPStatic();
}

int main(void)
{

    //init_usb();
    //HTTPServer_StaticIP();

    http_server_start();

	//init_usb();
    //setup_tls();


    return 0;
}
