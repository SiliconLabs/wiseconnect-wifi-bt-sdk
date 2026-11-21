/*HEADER**********************************************************************
Copyright (c)
All rights reserved
This software embodies materials and concepts that are confidential to Silicon Labs
and is made available solely pursuant to the terms of a written license
agreement with Silicon Labs

Project name : WiSeConnect
Module name  : LINUX-SPI-driver
File Name    : rsi_linux.h

File Description:
        Operating system specific definitions can be found here
List of functions:

Author :

Rev History:
Sl By date change details
---------------------------------------------------------
01.                   created
---------------------------------------------------------

*END*************************************************************************/

#ifndef __RSI_LINUX_H_
#define __RSI_LINUX_H_
#include "rsi_linux_common.h"
#ifdef RSI_SPI_INTERFACE
#include "rsi_spi.h"
#endif
#ifdef RSI_USB_INTERFACE
#include "../../usb/include/rsi_usb.h"
#endif
#ifdef RSI_UART_INTERFACE
#include "rsi_uart.h"
#endif
#ifdef RSI_SDIO_INTERFACE
#include "rsi_sdio.h"
#endif

#include <linux/skbuff.h>
#include <linux/netdevice.h>
#include <linux/etherdevice.h>
#include <linux/if.h>
#include <linux/wireless.h>
#include <linux/wait.h>
#include <linux/vmalloc.h>
#include <linux/version.h>

#define BIT(n) (1 << (n))
#define TEXT(arg...) arg
#define RSI_DEBUG(zone, fmt, arg...) if(zone & rsi_zone_enabled) printk(fmt, ##arg)
#define RSI_ZONE_INFO           BIT(0)
#define RSI_ZONE_ERROR          BIT(1)
#define RSI_ZONE_INIT           BIT(2)
#define RSI_ZONE_OID            BIT(3)
#define RSI_ZONE_MGMT_SEND      BIT(4)
#define RSI_ZONE_MGMT_RCV       BIT(5)
#define RSI_ZONE_DATA_SEND      BIT(6)
#define RSI_ZONE_DATA_RCV       BIT(7)
#define RSI_ZONE_FSM            BIT(8)
#define RSI_ZONE_ISR            BIT(9)
#define RSI_ZONE_MGMT_DUMP      BIT(10)
#define RSI_ZONE_DATA_DUMP      BIT(11)
#define RSI_ZONE_PARSER         BIT(12)
#define RSI_ZONE_SPI_DBG        BIT(13)
#ifdef RS9116
#define RSI_ZONE_RS9116_DBG     BIT(14)
#endif
#define RSI_ZONE_SDIO_DBG       BIT(15)


#define rsi_carrier_on(dev)        netif_carrier_on(dev)
#define rsi_carrier_off(dev)       netif_carrier_off(dev)
#define rsi_vmalloc(temp)          vmalloc(temp)
#define rsi_vfree(ptr)             vfree(ptr)

#define rsi_queue_init(a)          skb_queue_head_init(a)
#define rsi_queue_purge(a)         skb_queue_purge(a)
#define rsi_queue_tail(a,b)        skb_queue_tail(a,b)
#define rsi_queue_head(a,b)        skb_queue_head(a,b)
#define rsi_queue_len(a)           skb_queue_len(a)
#define rsi_dequeue(a)             skb_dequeue(a)
#define rsi_skb_put(a,b)           skb_put(a,b)
#define rsi_kfree_skb(a)           dev_kfree_skb(a)

#define rsi_spinlock_init(a)       spin_lock_init(a)
#define rsi_lock_bh(a)             spin_lock_bh(a)
#define rsi_unlock_bh(a)           spin_unlock_bh(a)

#define rsi_memcpy(a,b,c)          memcpy(a,b,c)
#define rsi_memset(a,b,c)          memset(a,b,c)
#define rsi_memcmp(a,b,c)          memcmp(a,b,c)
#define rsi_strcpy(a,b)            strcpy(a,b)
#define rsi_strcpy(a,b)            strcpy(a,b)
#define rsi_strncpy(a,b,c)         strncpy(a,b,c)
#define rsi_equal_string(a,b)      strcmp(a,b)
#define rsi_simple_strtol(a,b,c)   simple_strtol(a,b,c)

#define rsi_Signal_Pending()       signal_pending(current)
#define rsi_netif_rx(a)            netif_rx_ni(a)
#define rsi_netif_start_queue(a)   netif_start_queue(a)
#define rsi_netif_queue_stopped(a) netif_queue_stopped(a)
#define rsi_netif_stop_queue(a)    netif_stop_queue(a)
#define rsi_eth_type_trans(a,b)    eth_type_trans(a,b)

#define rsi_schedule()             schedule()
#define rsi_sprintf                sprintf

#define rsi_down_interruptible(a)  down_interruptible(a)
#define rsi_up_sem(a)              up(a)
#define rsi_down_sem(a)            down(a)
#ifdef init_MUTEX
#define rsi_init_mutex(a)          init_MUTEX(a)
#else
#define rsi_sema_init(a,b)         sema_init(a,b)
#endif

#define rsi_mem_free(ptr)          kfree(ptr)
#define rsi_mem_alloc(a,b)         kmalloc(a,b)

#define rsi_copy_to_user(a,b,c)    copy_to_user(a,b,c)
#define rsi_copy_from_user(a,b,c)  copy_from_user(a,b,c)

#define rsi_nla_data(a)                 nla_data(a)
#if LINUX_VERSION_CODE == KERNEL_VERSION(2,6,18)
#define rsi_genlmsg_new(a,b)            nlmsg_new(a)
#else
#define rsi_genlmsg_new(a,b)            genlmsg_new(a,b)
#endif
#if LINUX_VERSION_CODE == KERNEL_VERSION(2,6,18)
#define rsi_genlmsg_put(a,b,c,d,e,f,g,h) genlmsg_put(a,b,c,d,e,f,g,h)
#else
#define rsi_genlmsg_put(a,b,c,d,e,f)    genlmsg_put(a,b,c,d,e,f)
#endif
#define rsi_nla_put(a,b,c,d)            nla_put(a,b,c,d)
#define rsi_genlmsg_end(a,b)            genlmsg_end(a,b)
#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,32)
#define rsi_genlmsg_unicast(a,b,c)      genlmsg_unicast(a,b,c)
#else
#define rsi_genlmsg_unicast(a,b)        genlmsg_unicast(a,b)
#endif
#define rsi_genl_register_ops(a,b)      genl_register_ops(a,b)
#define rsi_genl_unregister_ops(a,b)    genl_unregister_ops(a,b)
#define rsi_genl_register_family(a)     genl_register_family(a)
#define rsi_genl_unregister_family(a)   genl_unregister_family(a)
//#define RSI_GENL_ID_GENERATE            GENL_ID_GENERATE   //! GENL_ID_GENERATE  was removed from kernel 4.10, which prevents compilation of netsniff-ng.

#define EVENT_WAIT_FOREVER       (0x00)

#if LINUX_VERSION_CODE <= KERNEL_VERSION(3, 6, 11)
# define get_portid(_info) (_info)->snd_pid
#else
# define get_portid(_info) (_info)->snd_portid
#endif

#define BIN_FILE 0
#define HEX_FILE 1

#define RSI_THREAD_NAME_LEN      15
#define RSI_THREAD_PRIORITY      0
#define RSI_INT_THREAD_PRIORITY  0

#if LINUX_VERSION_CODE > KERNEL_VERSION(2,5,0)
typedef  struct work_struct         RSI_WORK_QUEUE;
#define  rsi_Init_Work_Queue(a,b)  INIT_WORK(a,b)
#define  rsi_Schedule_Work(a)      schedule_work(a)
#else
typedef  struct tq_struct           RSI_WORK_QUEUE;
#define  rsi_Init_Work_Queue(a,b,c)  INIT_TQUEUE(a,b,c)
#define  rsi_Schedule_Work(a)        schedule_task(a)
#endif
#define RSI_THREAD_NAME_LEN 15

#define RSI_ASSERT(exp)                                                     \
    do {                                                                    \
        if (!(exp)) {                                                       \
            RSI_DEBUG(RSI_ZONE_ERROR,"Assertion Failed! %s, %s(), %s:%d\n", \
                    #exp, __FUNCTION__, __FILE__, __LINE__);                \
        }                                                                   \
    } while (0)

typedef struct semaphore RSI_SEMAPHORE, *PRSI_SEMAPHORE;

static inline void rsi_skb_set_tail_pointer(struct sk_buff *skb, INT32 offset)
{
  skb->tail = skb->data + offset;
}

/*Function prototypes*/
/* net device */
struct net_device* rsi_allocdev(INT32 sizeof_priv, UINT8 vap_id);
INT32  rsi_registerdev( struct net_device *dev );
VOID   rsi_unregisterdev(struct net_device *dev);
struct net_device_stats *rsi_get_stats( struct net_device *dev);
INT32  rsi_open(struct net_device *dev);
INT32  rsi_stop(struct net_device *dev);
struct net_device* rsi_netdevice_op(UINT8 vap_id);

INT32  rsi_xmit(struct sk_buff *skb, struct net_device *dev);
INT32 rsi_transmit_thread (PVOID pContext);
INT32  rsi_indicate_packet(PRSI_ADAPTER Adapter, UINT8 *DataRcvPacket,UINT32 pktLen, UINT8 sta_id);

struct sk_buff *rsi_alloc_skb(UINT32 len);
VOID   rsi_kfree_skb(struct sk_buff *skb);
VOID   rsi_dump(INT32 zone, PVOID vdata, INT32   len);
INT32  rsi_Kill_Thread( PRSI_ADAPTER Adapter);
INT32  rsi_Kill_Rx_Thread(struct rsi_thread *thread);
INT32 rsi_Init_Thread ( PRSI_ADAPTER Adapter);
INT32 rsi_Init_Event(RSI_EVENT *pEvent);
INT32 rsi_Delete_Event (RSI_EVENT *pEvent);
INT32  rsi_Wait_Event(RSI_EVENT *pEvent,UINT32 timeOut);
VOID rsi_Set_Event(RSI_EVENT *pEvent);
INT32 rsi_Reset_Event(RSI_EVENT *pEvent);
PRSI_ADAPTER rsi_getpriv(struct net_device *glbl_net_device);
int rsi_create_kthread( PRSI_ADAPTER Adapter,struct rsi_thread *thread,void *func_ptr,u8 *name);


/* netlink */
INT32 rsi_read_req_from_userspace(struct sk_buff *skb_2, struct genl_info *info);
#ifdef RSI_UART_INTERFACE
INT32 rsi_send_rsp_to_userspace(PRSI_ADAPTER Adapter, UINT8 *rspBuff, UINT8 to_uart);
#else
INT32 rsi_send_rsp_to_userspace(PRSI_ADAPTER Adapter, UINT8 *rspBuff);
#endif
INT32 rsi_register_genl(void);
INT32 rsi_unregister_genl(void);
#endif //__RSI_LINUX_H_
