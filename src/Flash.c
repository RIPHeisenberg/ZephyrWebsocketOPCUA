#include <zephyr/settings/settings.h>
#include <zephyr/storage/flash_map.h>
#include <zephyr/fs/nvs.h>
#include "Flash.h"

#define DHCP_CONFIG_ID 1

#if !DT_NODE_EXISTS(DT_NODELABEL(storage_partition))
#error "storage_partition node not found in devicetree!"
#endif

static struct nvs_fs nvs;

int Flash_SaveNVS(const DHCP_t *cfg)
{
    return nvs_write(&nvs, DHCP_CONFIG_ID, cfg, sizeof(*cfg));
}

int Flash_LoadNVS(DHCP_t *cfg)
{
    int rc = nvs_read(&nvs, DHCP_CONFIG_ID, cfg, sizeof(*cfg));
    if (rc == sizeof(*cfg)) {
        return 0;
    }
    return -ENOENT;
}

int Flash_Init(void)
{
    int rc;

    /* Use the *node label* from DTS: storage_partition */
    nvs.offset       = FIXED_PARTITION_OFFSET(storage_partition);
    nvs.sector_size  = FIXED_PARTITION_SIZE(storage_partition);
    nvs.sector_count = 1;   // whole partition as one NVS area

    rc = nvs_mount(&nvs);
    if (rc) {
        printk("NVS mount failed: %d\n", rc);
        return rc;
    }

    return 0;
}