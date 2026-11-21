/*HEADER**********************************************************************
Copyright (c)
All rights reserved
This software embodies materials and concepts that are confidential to Silicon Labs
and is made available solely pursuant to the terms of a written license
agreement with Silicon Labs

Project name : WiSeConnect
Module name  : Linux USB driver
File Name    : rsi_usb.h

File Description:
      USB specific header file

List of functions:

---------------------------------------------------------
*END**************************************************************************/

#ifndef __RSI_USB_H_
#define __RSI_USB_H_

#include "rsi_global.h"
#include "rsi_linux_common.h"
#include "rsi_nic.h"
#include <linux/usb.h>
#include <linux/version.h>
#include <linux/errno.h>
#include <linux/slab.h>

/*=========================================================*/
/* General Defines */
#define BIT(x)  ((uint32)1 << (x))


/*=========================================================*/
/* USB Driver specific defines */
#define PRIV_IOC_FIRST           0x8be0

#define RSI_USB_STATUS_FAILURE   -1
#define RSI_USB_STATUS_SUCCESS   0
#define RSI_USB_STATUS_PENDING   2

#define RSI_USB_STATUS           INT32

#define MY_DEVICE_NAME           "MY USB"
#define RSI_USB_STATIC           static
#define RSI_USB_EXTERN           extern
#define ONEBOX_RCV_BUFFER_LEN    2000
#define RSI_USB_TX_HEAD_ROOM     164
#define DEVINIT                  __devinit
#define DEVEXIT                  __devexit
#define INIT                     __init
#define EXIT                     __exit

#define USB_VENDOR_REGISTER_READ        0x15
#define USB_VENDOR_REGISTER_WRITE       0x16


typedef struct host_desc_dword0_s {
  uint32 length : 12;
  uint32 q_num : 4;
  uint32 frame_type : 8;

#define PWR_SAVE_WAKEUP_IND   BIT(0)
#define TCP_CHECK_SUM_OFFLOAD BIT(1)
#define CONFIRM_NOT_REQUIRED  BIT(2)
#define HOST_BROADCAST_IND    BIT(3)
#define UPDATE_PS_PARAMS      BIT(4)
#define MIMIC_UAPSD_SUPPORT    BIT(5)

  uint32 misc_flags : 8;
} dword0_t;

typedef struct host_desc_dword1_s {
  uint32 xtended_desc_size : 8;
  uint32 byte1 : 8;
  uint32 byte2 : 8;
  uint32 byte3 : 8;
  //uint32 reserved : 16;
} dword1_t;

typedef struct host_desc_dword2_s {
  uint32 byte0 : 8;
  uint32 byte1 : 8;
  uint32 byte2 : 8;
  uint32 byte3 : 8;

} dword2_t;

typedef struct host_desc_dword3_s {
  uint32 token    : 16;
  uint32 queue_id  : 4;
  uint32 tid       : 4;
  uint32 sta_id    : 8;
} dword3_t;

typedef struct host_desc_s {
  dword0_t dword0;
  dword1_t dword1;
  dword2_t dword2;
  dword3_t dword3;
} host_desc_t;
typedef struct rx_scatter_s
{
  //! Address of scatter
  uint32 addr;
  //! valid length of scatter
  uint16 len;
  //! reserved 
  uint16 reserved;
} rx_scatter_t;


typedef struct rx_onair_extended_desc_s
{
  uint16 rssi;
  uint16 reserved;
} rx_onair_extended_desc_t;
//! This structure contains format for rx_pkt
typedef struct wlan_rx_pkt_s
{
  //! length of host transfer (which includes this structure also.)
  uint16 host_xfer_len;
  //! host desc offset in bytes from buffer start.
  uint16 host_desc_offset;
  //! next buffer pointer 
  struct wlan_rx_pkt_s *next;
  //! RX scatters
  struct rx_scatter_s scatter[5];
  //! host queue being used
  uint16 host_q;
  //! no. of segments
  uint16 num_seg;
  //! defragment flags
  uint16 dfragn_flags;
  //! vap id and tid
  uint8 vap_id_tid;
  //! station id
  uint8 sta_id;
  uint16 host_desc[8];
  rx_onair_extended_desc_t rx_onair_extended_desc;
} wlan_rx_pkt_t;
#define HOST_DESC_SIZE 16

/* globals */
/* USB Device Information */
static struct usb_device_id rsi_usb_IdTable[] = {
  { USB_DEVICE(0x0303, 0x0100) },
  { USB_DEVICE(USB_RPINE_VENDER_ID, USB_RPINE_PRODUCT_ID) },  
  { USB_DEVICE(USB_RPINE_VENDER_ID_2, USB_RPINE_PRODUCT_ID) },  
  { }
};

RSI_STATUS rsi_usb_probe
(
  struct usb_interface *interface,
  const struct usb_device_id *id
);
void rsi_usb_disconnect(struct usb_interface *intf);


#endif // __RSI_USB_H_
