/*
 * Copyright (c) 2023, Emna Rekik
 * Copyright (c) 2024, Nordic Semiconductor
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdio.h>
#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>

#include "HTTPWebsocket.h"

LOG_MODULE_REGISTER(main, LOG_LEVEL_DBG);

int main(void)
{
    LOG_INF("Starting HTTP WebSocket Server Application");

    // Initialize the HTTP WebSocket server
    httpwebsocket_init();

    // Start the HTTP server
    int ret = httpwebsocket_start();
    if (ret < 0) {
        LOG_ERR("Failed to start HTTP WebSocket server: %d", ret);
        return ret;
    }

    LOG_INF("HTTP WebSocket Server started successfully");

    return 0;
}
