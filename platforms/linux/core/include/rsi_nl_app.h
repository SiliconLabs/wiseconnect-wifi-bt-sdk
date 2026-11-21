/*******************************************************************************
* @file  rsi_nl_app.h
* @brief 
*******************************************************************************
* # License
* <b>Copyright 2020 Silicon Laboratories Inc. www.silabs.com</b>
*******************************************************************************
*
* The licensor of this software is Silicon Laboratories Inc. Your use of this
* software is governed by the terms of Silicon Labs Master Software License
* Agreement (MSLA) available at
* www.silabs.com/about-us/legal/master-software-license-agreement. This
* software is distributed to you in Source Code format and is governed by the
* sections of the MSLA applicable to Source Code.
*
******************************************************************************/
/**
 * @file     rsi_nl_app.h
 * @version  3.6
 * @date     2013-May-16
 *
 *
 *
 * @brief HEADER  
 *
 * @section Description
 * 
 *
 */

#ifndef __RSI_GNUSER_H_
#define __RSI_GNUSER_H_

#include <stdint.h>

#include "rsi_linux_app_init.h"
#include <errno.h>
#include <sys/socket.h>
#include <linux/genetlink.h>

/*
 * Generic macros for dealing with netlink sockets. Might be duplicated
 * elsewhere. It is recommended that commercial grade applications use
 * libnl or libnetlink and use the interfaces provided by the library
 */
#define GENLMSG_DATA(glh) ((void *)(NLMSG_DATA(glh) + GENL_HDRLEN))
#define GENLMSG_PAYLOAD(glh) (NLMSG_PAYLOAD(glh, 0) - GENL_HDRLEN)
#define NLA_DATA(na) ((void *)((char*)(na) + NLA_HDRLEN))
#define MAX_RCV_SIZE  1600
#define RSI_NL_HEAD_SIZE         (sizeof(struct nlmsghdr) + sizeof(struct genlmsghdr) + sizeof(struct nlattr))
#define RSI_STATUS_OFFSET         12
#define RSI_TWOBYTE_STATUS_OFFSET 12
#define RSI_RSP_TYPE_OFFSET       2
#define GET_SEND_LENGTH(a) ((uint16_t)(*(uint32_t *)(a)))
/* User to Kernel Update Types */
enum {
  MODULE_POWER_CYCLE               = 0x01,
  UPDATE_JOIN_DONE                 = 0x02,
  PS_CONTINUE                      = 0x03,
  WKP_FROM_HOST                    = 0x04,
  UPDATE_CONCURRENT_AP_JOIN_DONE   = 0x05,
};



/* type defines */
typedef struct {
  struct nlmsghdr n;
  struct genlmsghdr g;
} rsi_nlPkt_t;

/* Function prototypes */
uint8_t *rsi_alloc_and_init_cmdbuff(uint8_t *Desc, uint8_t *payload, uint16_t payload_size);

#endif //__RSI_GNUSER_H_
