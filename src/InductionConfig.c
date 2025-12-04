
#include <zephyr/kernel.h>
#include <zephyr/data/json.h>
#include "InductionConfig.h"

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

bool InductionConfig_json_parser(char *data, uint32_t size)
{

  DHCP_t dhcp;
  uint8_t json_msg[] = "{\"DHCP\":\"on\",\"IP4Address\":\"192.0.2.1\",\"NetMask\":\"255.255.255.0\",\"isEnabled_CAN_1_0\":1,\"isEnabled_CAN_1_1\":0,\"isEnabled_CAN_1_2\":0,\"isEnabled_CAN_1_3\":0,\"isEnabled_CAN_2_0\":0,\"isEnabled_CAN_2_1\":0,\"isEnabled_CAN_2_2\":0,\"isEnabled_CAN_2_3\":0}";

  static const struct json_obj_descr DHCPDescriptor[] = {
      JSON_OBJ_DESCR_PRIM(DHCP_t, DHCP, JSON_TOK_STRING),
      JSON_OBJ_DESCR_PRIM(DHCP_t, IP4Address, JSON_TOK_STRING),
      JSON_OBJ_DESCR_PRIM(DHCP_t, NetMask, JSON_TOK_STRING),
      JSON_OBJ_DESCR_PRIM(DHCP_t, isEnabled_CAN_1_0, JSON_TOK_NUMBER),
      JSON_OBJ_DESCR_PRIM(DHCP_t, isEnabled_CAN_1_1, JSON_TOK_NUMBER),
      JSON_OBJ_DESCR_PRIM(DHCP_t, isEnabled_CAN_1_2, JSON_TOK_NUMBER),
      JSON_OBJ_DESCR_PRIM(DHCP_t, isEnabled_CAN_1_3, JSON_TOK_NUMBER),
      JSON_OBJ_DESCR_PRIM(DHCP_t, isEnabled_CAN_2_0, JSON_TOK_NUMBER),
      JSON_OBJ_DESCR_PRIM(DHCP_t, isEnabled_CAN_2_1, JSON_TOK_NUMBER),
      JSON_OBJ_DESCR_PRIM(DHCP_t, isEnabled_CAN_2_2, JSON_TOK_NUMBER),
      JSON_OBJ_DESCR_PRIM(DHCP_t, isEnabled_CAN_2_3, JSON_TOK_NUMBER),
  };

  int64_t ret = json_obj_parse(data, size, DHCPDescriptor, ARRAY_SIZE(DHCPDescriptor), &dhcp);
  if (ret < 0)
  {
    printk("JSON Parse Error: %d\n", ret);
    return false;
  }
  else
  {
    printk("json_obj_parse return code: %d\n", ret);
    printk("DHCP:                       %s\n", dhcp.DHCP);
    printk("IP4Address                  %s\n", dhcp.IP4Address);
    printk("NetMask:                    %s\n", dhcp.NetMask);
    printk("isEnabled_CAN_1_0           %i\n", dhcp.isEnabled_CAN_1_0);
    printk("isEnabled_CAN_2_3           %i\n", dhcp.isEnabled_CAN_2_3);
  }

  printk("Exit\n");
  return true;
}