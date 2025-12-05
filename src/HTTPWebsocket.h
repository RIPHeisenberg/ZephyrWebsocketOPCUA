/*
 * Copyright (c) 2023, Emna Rekik
 * Copyright (c) 2024, Nordic Semiconductor
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef HTTPWEBSOCKET_H
#define HTTPWEBSOCKET_H

#include <zephyr/kernel.h>
#include <zephyr/net/http/server.h>
#include <zephyr/net/http/service.h>
#include <zephyr/net/net_if.h>

/**
 * @brief Initialize the HTTP/WebSocket server
 *
 * Sets up all HTTP resources, services, and starts the server.
 */
void httpwebsocket_init(void);

/**
 * @brief Start the HTTP server
 *
 * @return 0 on success, negative error code on failure
 */
int httpwebsocket_start(void);

/**
 * @brief Stop the HTTP server
 *
 * @return 0 on success, negative error code on failure
 */
int httpwebsocket_stop(void);

/**
 * @brief Initialize network interface
 */
void httpwebsocket_interface_init(void);

/**
 * @brief Start DHCP client
 */
void httpwebsocket_start_dhcp(void);

/**
 * @brief Configure static IP and stop DHCP
 */
void httpwebsocket_set_static_ip(void);

/**
 * @brief Switch to static IP configuration
 */
void httpwebsocket_static_ip_mode(void);

#endif /* HTTPWEBSOCKET_H */
