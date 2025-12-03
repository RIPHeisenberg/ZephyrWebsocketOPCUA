/* Networking DHCPv4 client */

/*
 * Copyright (c) 2017 ARM Ltd.
 * Copyright (c) 2016 Intel Corporation.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <zephyr/logging/log.h>
LOG_MODULE_REGISTER(net_dhcpv4_client_sample, LOG_LEVEL_DBG);

#include <zephyr/kernel.h>
#include <zephyr/linker/sections.h>
#include <errno.h>
#include <stdio.h>
#include <stdint.h>

#include <zephyr/net/net_if.h>
#include <zephyr/net/net_core.h>
#include <zephyr/net/net_context.h>
#include <zephyr/net/net_mgmt.h>
#include <zephyr/net/dhcpv4.h>

#include "DHCPClient.h"

typedef struct {
    tcp_info_t tcpInfo;
    bool isValide;
} tcp_info_dhcp_t;

tcp_info_dhcp_t TCPInfoDHCP = {
    .isValide = false
};

#define DHCP_OPTION_NTP (42)

static uint8_t ntp_server[4];

static struct net_mgmt_event_callback mgmt_cb;

static struct net_dhcpv4_option_callback dhcp_cb;

static void start_dhcpv4_client(struct net_if *iface, void *user_data)
{
    ARG_UNUSED(user_data);
    LOG_INF("Start on %s: index=%d", net_if_get_device(iface)->name,
            net_if_get_by_iface(iface));
    net_dhcpv4_start(iface);
}

static void handler(struct net_mgmt_event_callback *cb,
                    uint32_t mgmt_event,
                    struct net_if *iface) {
    int i = 0;
    printk("DHCPHandler called\n");

    if (mgmt_event != NET_EVENT_IPV4_ADDR_ADD) {
        printk("No IPV4 found\n");
        return;
    }

    for (i = 0; i < NET_IF_MAX_IPV4_ADDR; i++) {
        char buf[NET_IPV4_ADDR_LEN];

        //struct net_if_addr *unicast = (net_if_addr*)&iface->config.ip.ipv4->unicast[i];
        if (iface->config.ip.ipv4->unicast[i].ipv4.addr_type != NET_ADDR_DHCP) {
            printk("Net adress not DHCP\n");
            continue;
        }

        printk("\n    Address[%d]: %s\n", net_if_get_by_iface(iface),
                net_addr_ntop(AF_INET,
                    &iface->config.ip.ipv4->unicast[i].ipv4.address.in_addr,
                    buf, sizeof(buf)));
        printk("    Subnet[%d]: %s\n", net_if_get_by_iface(iface),
                net_addr_ntop(AF_INET,
                    &iface->config.ip.ipv4->unicast[i].netmask,
                    buf, sizeof(buf)));
        printk("    Router[%d]: %s\n", net_if_get_by_iface(iface),
                net_addr_ntop(AF_INET,
                    &iface->config.ip.ipv4->gw,
                    buf, sizeof(buf)));
        printk("Lease time[%d]: %u seconds\n", net_if_get_by_iface(iface),
                iface->config.dhcpv4.lease_time);

        /* Store network information in tcpInfo structure */
        memcpy(TCPInfoDHCP.tcpInfo.tcp_4_info, &iface->config.ip.ipv4->unicast[i].ipv4.address.in_addr.s4_addr, 4);
        memcpy(TCPInfoDHCP.tcpInfo.tcp_4_info_router, &iface->config.ip.ipv4->gw.s4_addr, 4);
        memcpy(TCPInfoDHCP.tcpInfo.tcp_4_info_netmask, &iface->config.ip.ipv4->unicast[i].netmask.s4_addr, 4);
        TCPInfoDHCP.tcpInfo.lease_time = iface->config.dhcpv4.lease_time;
        TCPInfoDHCP.isValide = true;

        printk("Scan finished\n");
        net_dhcpv4_stop(iface);
    }
}


static void option_handler(struct net_dhcpv4_option_callback *cb,
                           size_t length,
                           enum net_dhcpv4_msg_type msg_type,
                           struct net_if *iface) {
    char buf[NET_IPV4_ADDR_LEN];

    LOG_INF("DHCP Option %d: %s", cb->option,
            net_addr_ntop(AF_INET, cb->data, buf, sizeof(buf)));
}

bool DHCP_Client_TCP_infos_get(tcp_info_t *tcp_info) 
{
    if (tcp_info == NULL) {
        return false;
    }

    if (!TCPInfoDHCP.isValide) {
        return false;
    }

    *tcp_info = TCPInfoDHCP.tcpInfo;
    return true;
}


int DHCP_Client_init(void) 
{
    LOG_INF("Run dhcpv4 client");

    net_mgmt_init_event_callback(&mgmt_cb, handler,
                                 NET_EVENT_IPV4_ADDR_ADD);
    net_mgmt_add_event_callback(&mgmt_cb);

    net_dhcpv4_init_option_callback(&dhcp_cb, option_handler,
                                    DHCP_OPTION_NTP, ntp_server,
                                    sizeof(ntp_server));

    net_dhcpv4_add_option_callback(&dhcp_cb);

    net_if_foreach(start_dhcpv4_client, NULL);
    return 0;
}
