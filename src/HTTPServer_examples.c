/*
 * HTTPServer Usage Examples
 * 
 * This file demonstrates how to use the HTTPServer module with different
 * network configurations.
 */

#include "HTTPServer.h"
#include <zephyr/logging/log.h>

/* Example 1: Using DHCP (recommended for most cases) */
void example_dhcp_server(void)
{
	struct http_server_config config = {
		.ipv4_addr = NULL,        /* Not used with DHCP */
		.ipv4_gw = NULL,         /* Not used with DHCP */
		.ipv4_netmask = NULL,    /* Not used with DHCP */
		.use_dhcp = true,        /* Enable DHCP */
		.http_port = 80,         /* HTTP port */
		.https_port = 443,       /* HTTPS port */
		.enable_http = true,     /* Enable HTTP service */
		.enable_https = false,   /* Disable HTTPS service */
	};

	int ret = http_server_init_and_start(&config);
	if (ret < 0) {
		printk("Failed to start DHCP HTTP server: %d\n", ret);
	} else {
		printk("DHCP HTTP server started successfully\n");
	}
}

/* Example 2: Using static IP address */
void example_static_ip_server(void)
{
	struct http_server_config config = {
		.ipv4_addr = "192.168.1.100",    /* Static IP address */
		.ipv4_gw = "192.168.1.1",       /* Gateway address */
		.ipv4_netmask = "255.255.255.0", /* Subnet mask */
		.use_dhcp = false,               /* Disable DHCP */
		.http_port = 8080,               /* Custom HTTP port */
		.https_port = 8443,              /* Custom HTTPS port */
		.enable_http = true,             /* Enable HTTP service */
		.enable_https = true,            /* Enable HTTPS service */
	};

	int ret = http_server_init_and_start(&config);
	if (ret < 0) {
		printk("Failed to start static IP HTTP server: %d\n", ret);
	} else {
		printk("Static IP HTTP server started successfully\n");
		
		/* Get and print current IP information */
		char ip_str[16];
		ret = http_server_get_ip_info(ip_str, sizeof(ip_str));
		if (ret == 0) {
			printk("Server IP: %s\n", ip_str);
			printk("HTTP URL:  http://%s:%d\n", ip_str, config.http_port);
			printk("HTTPS URL: https://%s:%d\n", ip_str, config.https_port);
		}
	}
}

/* Example 3: HTTPS-only server with DHCP */
void example_https_only_server(void)
{
	struct http_server_config config = {
		.ipv4_addr = NULL,
		.ipv4_gw = NULL,
		.ipv4_netmask = NULL,
		.use_dhcp = true,         /* Use DHCP */
		.http_port = 80,          /* HTTP port (not used) */
		.https_port = 443,        /* HTTPS port */
		.enable_http = false,     /* Disable HTTP service */
		.enable_https = true,     /* Enable HTTPS service only */
	};

	int ret = http_server_init_and_start(&config);
	if (ret < 0) {
		printk("Failed to start HTTPS-only server: %d\n", ret);
	} else {
		printk("HTTPS-only server started successfully\n");
	}
}

/* Example 4: How to stop the server */
void example_stop_server(void)
{
	int ret = http_server_stop_service();
	if (ret < 0) {
		printk("Failed to stop HTTP server: %d\n", ret);
	} else {
		printk("HTTP server stopped successfully\n");
	}
}