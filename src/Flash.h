#ifndef FLASH_H_
#define FLASH_H_

#include "InductionConfig.h"

int Flash_SaveNVS(const DHCP_t *cfg);

int Flash_LoadNVS(DHCP_t *cfg);

int Flash_Init(void);

#endif // FLASH_H_