# HTTPServer Module

This module provides an easy-to-use interface for configuring and starting an HTTP/HTTPS server with flexible network configuration options, including DHCP and static IP support.

## Features

- **DHCP Support**: Automatically obtain IP address from DHCP server
- **Static IP Configuration**: Manually configure IP address, gateway, and netmask
- **HTTP/HTTPS Support**: Enable either or both HTTP and HTTPS services
- **Flexible Port Configuration**: Configure custom ports for HTTP and HTTPS
- **TLS Management**: Automatic TLS certificate setup for HTTPS
- **Network Information**: Get current IP configuration

## Files

- `HTTPServer.h` - Header file with function declarations and structures
- `HTTPServer.c` - Implementation of the HTTP server module
- `HTTPServer_examples.c` - Usage examples

## Configuration Structure

```c
struct http_server_config {
    const char *ipv4_addr;      // IPv4 address (NULL for DHCP)
    const char *ipv4_gw;        // Gateway address (optional)
    const char *ipv4_netmask;   // Subnet mask (optional)
    bool use_dhcp;              // Use DHCP for IP configuration
    uint16_t http_port;         // HTTP port
    uint16_t https_port;        // HTTPS port
    bool enable_http;           // Enable HTTP service
    bool enable_https;          // Enable HTTPS service
};
```

## Basic Usage

### 1. DHCP Configuration (Recommended)

```c
#include "HTTPServer.h"

struct http_server_config config = {
    .ipv4_addr = NULL,          // Use DHCP
    .use_dhcp = true,
    .http_port = 80,
    .https_port = 443,
    .enable_http = true,
    .enable_https = false,
};

int ret = http_server_init_and_start(&config);
if (ret < 0) {
    LOG_ERR("Failed to start server: %d", ret);
}
```

### 2. Static IP Configuration

```c
struct http_server_config config = {
    .ipv4_addr = "192.168.1.100",
    .ipv4_gw = "192.168.1.1", 
    .ipv4_netmask = "255.255.255.0",
    .use_dhcp = false,
    .http_port = 8080,
    .https_port = 8443,
    .enable_http = true,
    .enable_https = true,
};

int ret = http_server_init_and_start(&config);
```

## API Functions

### http_server_init_and_start()
Initializes network configuration and starts the HTTP server.

**Parameters:**
- `config`: Pointer to configuration structure

**Returns:** 0 on success, negative error code on failure

### http_server_stop_service()
Stops the HTTP server and cleans up resources.

**Returns:** 0 on success, negative error code on failure

### http_server_get_ip_info()
Retrieves current IPv4 address information.

**Parameters:**
- `ipv4_addr`: Buffer to store IP address string (min 16 bytes)
- `ipv4_addr_len`: Length of the buffer

**Returns:** 0 on success, negative error code on failure

### http_server_setup_tls()
Sets up TLS credentials for HTTPS (called automatically by init_and_start).

**Returns:** 0 on success, negative error code on failure

## Configuration Requirements

Add the following to your `prj.conf`:

```
# DHCP client support (if using DHCP)
CONFIG_NET_DHCPV4=y
CONFIG_NET_MGMT=y
CONFIG_NET_MGMT_EVENT=y
CONFIG_NET_MGMT_EVENT_INFO=y

# Basic networking (if not already present)
CONFIG_NETWORKING=y
CONFIG_NET_IPV4=y
CONFIG_NET_TCP=y
CONFIG_NET_SOCKETS=y
CONFIG_NET_CONNECTION_MANAGER=y

# HTTP server support
CONFIG_HTTP_SERVER=y

# HTTPS support (if needed)
CONFIG_NET_SOCKETS_SOCKOPT_TLS=y
CONFIG_MBEDTLS=y
```

## Error Handling

The module returns standard Zephyr error codes:
- `-EINVAL`: Invalid configuration parameters
- `-ENODEV`: No network interface available
- `-ETIMEDOUT`: DHCP timeout (30 seconds)
- `-ENONET`: Network configuration failed

## Integration with Main Application

Replace the existing HTTP server setup in your `main.c`:

```c
// Old way:
// setup_tls();
// http_server_start();

// New way:
#include "HTTPServer.h"

int main(void) {
    struct http_server_config config = {
        .use_dhcp = true,
        .http_port = CONFIG_NET_SAMPLE_HTTP_SERVER_SERVICE_PORT,
        .enable_http = true,
        .enable_https = IS_ENABLED(CONFIG_NET_SAMPLE_HTTPS_SERVICE),
    };
    
    int ret = http_server_init_and_start(&config);
    if (ret < 0) {
        LOG_ERR("Server start failed: %d", ret);
        return ret;
    }
    
    return 0;
}
```

## Notes

- DHCP configuration includes a 30-second timeout
- The module automatically handles TLS certificate setup when HTTPS is enabled
- Network management events are logged for debugging
- The server will print accessible URLs once successfully started