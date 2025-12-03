#ifndef DHCP_CLIENT_H_
#define DHCP_CLIENT_H_

#include <zephyr/net/net_if.h>
#include <zephyr/net/net_core.h>
#include <zephyr/net/net_context.h>
#include <zephyr/net/net_mgmt.h>
#include <zephyr/net/dhcpv4.h>

#include <zephyr/kernel.h>
#include <zephyr/linker/sections.h>
#include <errno.h>
#include <stdio.h>
#include <stdint.h>

/* TCP/IP information structure */
typedef struct {
    uint8_t tcp_4_info[4];        /* IPv4 address */
    uint8_t tcp_4_info_router[4]; /* Gateway/router address */
    uint8_t tcp_4_info_netmask[4]; /* Subnet mask */
    uint32_t lease_time;          /* DHCP lease time */
} tcp_info_t;

bool DHCP_Client_TCP_infos_get(tcp_info_t *tcp_info);
//bool Net_Credentials_read(tcp_info_t *tcp_info);
//bool NET_Credentials_write(tcp_info_t *tcp_info);
int DHCP_Client_init(void);

#endif // DHCP_CLIENT_H_