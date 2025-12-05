#ifndef INDUCTION_CONFIG_H_
#define INDUCTION_CONFIG_H_

#include <stdbool.h>

typedef struct
{
    const char *DHCP;
    const char *IP4Address;
    const char *NetMask;
    const int *isEnabled_CAN_1_0;
    const int *isEnabled_CAN_1_1;
    const int *isEnabled_CAN_1_2;
    const int *isEnabled_CAN_1_3;
    const int *isEnabled_CAN_2_0;
    const int *isEnabled_CAN_2_1;
    const int *isEnabled_CAN_2_2;
    const int *isEnabled_CAN_2_3;
} DHCP_t;

bool InductionConfig_json_parser(char *data, uint32_t size);


#endif // INDUCITON_CONFIG_H_