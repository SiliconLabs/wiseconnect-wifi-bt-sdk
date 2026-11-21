/*******************************************************************************
* @file  rsi_linux_app_init.h
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
 * @file     rsi_linux_app_init.h
 * @version  0.1
 * @date     04 July 2018
 *
 *
 *
 * @brief LINUX PLATFORM HEADER: Function prototypes and defines for USB, SPI, USB-CDC
 *
 * @section Description
 * This file contains the function definition prototypes, defines used in USB, SPI, USB_CDC and related functions
 *
 *
 */
#include <pthread.h>
#include <linux/types.h>                   
#ifndef _RSI_LINUX_APP_INIT_H_
#define _RSI_LINUX_APP_INIT_H_

#ifndef RSI_WITH_OS
#define rsi_free(ptr)    free(ptr)
#define rsi_malloc(ptr)  malloc(ptr)
#endif

#define RSI_FRAME_DESC_LEN                  16     //@ Length of the frame descriptor, for both read and write
#define RSI_BYTES_3                         3
#define REG_READ                          0xD1
#define REG_WRITE                         0xD2
#define PONG_WRITE                        0xD4
#define PING_WRITE                        0xD5

#define SIOCDEVPRIVATE 		0x89F0	/* to 89FF */

#define RSI_WSC_PRIV_IOCTLS      SIOCDEVPRIVATE
#define OID_WSC_GET_STATUS       RSI_WSC_PRIV_IOCTLS + 0x1
#define OID_WSC_BOOT_READ        RSI_WSC_PRIV_IOCTLS + 0x2
#define OID_WSC_BOOT_WRITE       RSI_WSC_PRIV_IOCTLS + 0x3
#define OID_WSC_BOOT_PING_WRITE  RSI_WSC_PRIV_IOCTLS + 0x4
#define OID_WSC_BOOT_PONG_WRITE  RSI_WSC_PRIV_IOCTLS + 0x5
#define OID_MASTER_READ          RSI_WSC_PRIV_IOCTLS + 0x8

#define HOST_INTERACT_REG_VALID           (0xA0 << 8)
#define HOST_INTERACT_REG_VALID_READ      (0xAB << 8)

#define BOOTUP_OPTIONS_CHECKSUM_FAIL      0xF2
#define BOOTUP_OPTIONS_LAST_CONFIG_NOT_SAVED 0xF1

#define RSI_RESET_LOOP_COUNTER(X)              X = 0;
#define RSI_LOOP_COUNT_SELECT_OPTION           0xFFFF
#define RSI_CHECK_LOOP_COUNTER(X, Y)           { if(X >= Y)\
  return -1;}

#define RSI_WHILE_LOOP(X, Y)                   while((X++) < (uint32_t)Y)
#define CHECKSUM_SUCCESS                  0xAA
#define CHECKSUM_FAILURE                  0xCC
#define CHECKSUM_INVALID_ADDRESS          0x4C

#define RSI_NL_APP_MAX_PAYLOAD_SIZE			  1600 + 20	 // 1600 maximum data payload size ; 20 is for to avoid unwanted portion
#define RSI_NL_APP_RXPKT_HEAD_ROOM         16 + 4    //16 Actual length;



#define RSI_MODULE_IP_ADDRESS          "192.168.0.12"        //@ IP Address of the WiFi Module

#define SET_WLAN0_IP system("ifconfig rsi_wlan0 "RSI_MODULE_IP_ADDRESS)

//const uint8_t rsi_sleep_ack[RSI_FRAME_DESC_LEN]      =    {0x00, 0x40, 0xDE, 0x00, 0x00, 0x00, 0x00, 0x00,0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};


//@ Debug Print Levels
#define RSI_DEBUG_LVL       			0x00ff
//@ These bit values may be ored to all different combinations of debug printing
#define RSI_PL0					0xffff
#define RSI_PL1					0x0001
#define RSI_PL2					0x0002
#define RSI_PL3					0x0004
#define RSI_PL4					0x0008
#define RSI_PL5					0x0010
#define RSI_PL6					0x0020
#define RSI_PL7					0x0040
#define RSI_PL8					0x0080
#define RSI_PL9					0x0100
#define RSI_PL10				0x0200
#define RSI_PL11				0x0400
#define RSI_PL12				0x0800
#define RSI_PL13				0x1000
#define RSI_PL14				0x2000
#define RSI_PL15				0x4000
#define RSI_PL16				0x8000

#define RSI_DPRINT(lvl, fmt, args...)              if (lvl & RSI_DEBUG_LVL) printf(fmt, ##args)




extern const uint8_t            rsi_frameCmdUpdateInfo[RSI_BYTES_3];

#if (defined (RSI_USB_INTERFACE) || defined (RSI_SDIO_INTERFACE))

//! host descriptor structure
typedef struct rsi_frame_desc_s {
  //! Data frame body length. Bits 14:12=queue, 000 for data, Bits 11:0 are the length
  uint8_t   frame_len_queue_no[2];            
  //! Frame type
  uint8_t   frame_type;                
  //! Unused , set to 0x00
  uint8_t   reserved[9];                       
  //! Management frame descriptor response status, 0x00=success, else error
  uint8_t   status;
  uint8_t   reserved1[3];
} rsi_frame_desc_t;

#endif


typedef struct pkt_struct_s
{
  //! next packet pointer 
  struct pkt_struct_s *next;

  //! host descriptor 
  uint8_t desc[16];

  //! payload 
  uint8_t *data;
}pkt_struct_t;


typedef struct
{
  pkt_struct_t *head;                  //! queue head 
  pkt_struct_t *tail;                  //! queue tail
  volatile uint16_t pending_pkt_count;            //! pending packets in the queue
}pkt_queue_t;


typedef struct rsi_linux_driver_cb_s{
  int32_t                nl_sd;          //! netlink socket descriptor 
  int32_t                ioctl_sd;       //! socket descriptor of ioctl 
  int32_t                family_id;      //! family id 
  uint8_t                rsi_glbl_genl_nl_hdr[20];
  uint8_t                mac_addr[6];
  uint32_t               num_rcvd_packets;
  pthread_mutex_t        mutex1;
  pkt_queue_t          rcv_queue;
}rsi_linux_driver_cb_t;


typedef union {
  struct {
    uint8_t   dataFrmLenAndQueue[2];                
    //@ Data frame body length. Bits 14:12=queue, 010 for data, Bits 11:0 are the length
    uint8_t    padding[14];                     //@ Unused, set to 0x00
  } frameDscDataSnd;
  struct {
    uint8_t   mgmtFrmLenAndQueue[2];            
    //@ Data frame body length. Bits 14:12=queue, 000 for data, Bits 11:0 are the length
    uint8_t   mgmtRespType;                
    //@ Management frame descriptor response status, 0x00=success, else error
    uint8_t   padding[9];                       //@ Unused , set to 0x00
    uint8_t   mgmtFrmDscRspStatus;
    uint8_t   padding1[3];
  } frameDscMgmtRsp;
  uint8_t   uFrmDscBuf[16]; //RSI_FRAME_DESC_LEN];       //@ byte format for spi interface, 16 bytes
} rsi_uFrameDsc;





struct  iw_point
{
  void    *pointer;     //@ Pointer to the data  (in user space) 
  __u16    length;      //@ number of fields or size in bytes 
  __u16     flags;      //@ Optional params 
};

#define	IFNAMSIZ	16

/* ------------------------ IOCTL REQUEST ------------------------ */
/*
 * This structure defines the payload of an ioctl, and is used
 * below.
 *
 * Note that this structure should fit on the memory footprint
 * of iwreq (which is the same as ifreq), which mean a max size of
 * 16 octets = 128 bits. Warning, pointers might be 64 bits wide...
 * You should check this when increasing the structures defined
 * above in this file...
 */
union   iwreq_data
{
  //! Config - generic 
  char            name[IFNAMSIZ];

  //! Name of the protocol/provider...
  struct iw_point data;           //! Other large parameters 
};

/*
 * The structure to exchange data for ioctl.
 * This structure is the same as 'struct ifreq', but (re)defined for
 * convenience...
 * Do I need to remind you about structure size (32 octets) ?
 */

struct  iwreq
{
  union
  {
    char    ifrn_name[IFNAMSIZ];    //! if name, e.g. "wlan0" 
  } ifr_ifrn;

  //! Data part (defined just above) 
  union   iwreq_data      u;
};



/* Function prototypes */

int16_t rsi_execute_cmd(uint8_t *,uint8_t *,uint16_t );
int16_t rsi_register_interrupt_irq(void);
extern void * RecvThreadBody(void * );

extern rsi_linux_driver_cb_t rsi_linux_driver_app_cb;

#endif
