/*******************************************************************************
* @file  rsi_net_device.c
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
 * @file             rsi_net_device.c
 * @version          1.6
 * @date             2013-June-12
 *
 *
 *
 * @brief This contains all the LINUX network device specific code
 *
 * @section Description
 * This file contains the following functions.
 *    rsi_allocdev
 *    rsi_registerdev
 *    rsi_unregisterdev
 *    rsi_get_stats
 *    rsi_open
 *    rsi_stop
 *    rsi_netdevice_op
 *    rsi_spi_module_init
 *    rsi_spi_module_exit
 *
 */

#include <net/genetlink.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/delay.h>
#include <linux/device.h>
#include <linux/workqueue.h>
#include <linux/skbuff.h>
#include <linux/version.h>
#if LINUX_VERSION_CODE == KERNEL_VERSION(2,6,18)
#include <asm/semaphore.h>
#else
#include <linux/semaphore.h>
#endif
#include "rsi_linux.h"
#include "rsi_nic.h"
#include "rsi_global.h"
#include "rsi_api.h"
#ifdef RSI_SPI_INTERFACE
#include <linux/spi/spi.h>
#include <linux/irq.h>
#include <linux/spi/spidev.h>
#if (LINUX_VERSION_CODE == KERNEL_VERSION(2,6,18))
#include <linux/irqreturn.h>
#include <linux/hardirq.h>
#include <asm/arch/gpio.h>
#else
#include <linux/gpio.h>
#endif
#include "rsi_hal.h"
#endif
#ifdef RSI_USB_INTERFACE
#include <linux/usb.h>
#include "rsi_usb.h"
#endif
#ifdef RSI_SDIO_INTERFACE
#include "rsi_sdio.h"
#endif

#include "rsi_linux.h"
#include "rsi_nic.h"
#include "rsi_global.h"
#include "rsi_api.h"

/* globals */
struct net_device *glbl_net_device = NULL;
PRSI_ADAPTER Adapter = NULL;
#ifdef RSI_UART_INTERFACE
#if RSI_TCP_IP_BYPASS
extern int ack_flag;
#endif
#endif

#ifdef ENABLE_WMM_FEATURE 
void dbg_test_values(PRSI_ADAPTER adapter);
#endif

VOID rsi_interrupt_handler (PRSI_ADAPTER Adapter );
VOID deferred_rx_packet_parser( struct work_struct *work );

#ifdef RSI_SPI_INTERFACE
bool Already_probed;
int rsi_probe(struct spi_device *spi_device);
void rsi_spi_rx_thread(PRSI_ADAPTER adapter);
int rsi_linux_handle_interrupt(unsigned int irq, void *spi_device);
static irqreturn_t spi_irq_thread_handler(unsigned int irq, void *spi_device);
static struct of_device_id spidev_dt_ids[] = {
   {.compatible = "spi_bcm2835"},
   {},
};

struct spi_driver rsi_driver = {
  .driver = {
	  .name   = "spidev",
	  .bus    = &spi_bus_type,
	  .owner  = THIS_MODULE,
	  .of_match_table = of_match_ptr(spidev_dt_ids),
  },
  .probe  = rsi_probe,
  .remove = rsi_spi_disconnect,
};

struct spi_device *spi_dev;
irqreturn_t rsi_ssp_interrupt (
  INT32 irq,
  PVOID dev_instance,
  struct pt_regs *regs
);
#endif

#ifdef RSI_USB_INTERFACE

static struct usb_driver rsi_usb_driver = {
  .name           = "OBE-USB",
  .probe          = rsi_usb_probe,
  .disconnect     = rsi_usb_disconnect,
  .id_table       = rsi_usb_IdTable,
};
MODULE_DEVICE_TABLE (usb, rsi_usb_IdTable);

extern  RSI_STATUS rsi_submit_rx_urb(PRSI_ADAPTER Adapter,int endpoint);
VOID usb_rx_packet_parser( struct work_struct *work );

#endif


#ifdef RSI_SDIO_INTERFACE
#if KERNEL_VERSION_BTWN_2_6_(18, 22)
static SD_PNP_INFO rsi_IdTable[] __devinitdata =
{
	{
		.SDIO_ManufacturerID   = 0x101,
		.SDIO_ManufacturerCode = 0x041b,
		.SDIO_FunctionNo       = 1,
		.SDIO_FunctionClass    = 0
	},
	{
		.SDIO_ManufacturerID   = 0x201,
		.SDIO_ManufacturerCode = 0x041b,
		.SDIO_FunctionNo       = 1,
		.SDIO_FunctionClass    = 0
	},
	{
		.SDIO_ManufacturerID   = 0x301,
		.SDIO_ManufacturerCode = 0x041b,
		.SDIO_FunctionNo       = 1,
		.SDIO_FunctionClass    = 0
	},
	{
		.SDIO_ManufacturerID   = 0x100,
		.SDIO_ManufacturerCode = 0x0303,
		.SDIO_FunctionNo       = 1,
		.SDIO_FunctionClass    = 0
	},
	{}
};

static SDFUNCTION rsi_driver =
{
	.pName       = "rsi-SDIO WLAN",
	.Version     = CT_SDIO_STACK_VERSION_CODE, /* FIXME */
	.MaxDevices  = 1,
	.NumDevices  = 0,
	.pIds        = rsi_IdTable,
	.pProbe      = (PVOID)rsi_sdio_probe,
	.pRemove     = rsi_sdio_disconnect,
	.pSuspend    = NULL,
	.pResume     = NULL,
	.pWake       = NULL,
	/* .Function.pContext    = &rsi_SDIO,  */
};

#elif KERNEL_VERSION_GREATER_THAN_EQUALS_2_6_(26)
static const struct sdio_device_id rsi_IdTable[] =
{
	{ SDIO_DEVICE(0x303, 0x100) },
	{ SDIO_DEVICE(0x041B, 0x0301) },
	{ SDIO_DEVICE(0x041B, 0x0201) },
	{ SDIO_DEVICE(0x041B, 0x9330) },
	{ SDIO_DEVICE(0x041B, 0x9116) },
	{ SDIO_DEVICE(0x041B, 0x9117) },
	{ SDIO_DEVICE(0x041B, 0x917) },
	{ /* Blank */},
};

static struct sdio_driver rsi_driver =
{
	.name       = "rsi-SDIO WLAN",
	.id_table   = rsi_IdTable,
	.probe      = rsi_sdio_probe,
	.remove     = rsi_sdio_disconnect,
};
#endif

#endif

/*==============================================*/
/**
 * @fn          VOID rsi_deinit_interface(PVOID pContext)
 * @brief       Interrupt handler
 * @param[in]   pContext, Pointer to our adapter
 * @param[out]  none
 * @return      none
 * @section description:
 * This function de-intializes the bus driver
 */
VOID rsi_deinit_interface(PVOID pContext)
{
#ifdef RSI_DEBUG_PRINT
  RSI_DEBUG(RSI_ZONE_SPI_DBG,"\nrsi_deinit_interface:\n");
#endif
  PRSI_ADAPTER Adapter = (PRSI_ADAPTER)pContext;
#ifdef RSI_SPI_INTERFACE
  if(Adapter->irq_registered)
  {
    free_irq(SPI_INTR_GPIO_PIN,Adapter->net_device0);
  }
#endif
  return;
}




/* statics */

/*==============================================*/
/**
 * @fn          struct net_device* rsi_allocdev(
 *                             INT32 sizeof_priv)
 * @brief       Allocate & initializes the network device
 * @param[in]   sizeof_priv, size of the priv area to be allocated
 * @param[out]  none
 * @return      Pointer to the network device structure is returned
 * @section description
 * Allocate & initialize the network device.This function
 * allocates memory for the network device & initializes it
 * with ethernet generic values
 */
struct net_device*
rsi_allocdev
(
  INT32 sizeof_priv, uint8 vap_id
)
{
#ifdef RSI_DEBUG_PRINT
  RSI_DEBUG(RSI_ZONE_SPI_DBG,"\nrsi_allocdev:\n");
#endif
  struct net_device *dev = NULL;
  if(vap_id == 0)
  {
#ifndef ENABLE_WMM_FEATURE
#if LINUX_VERSION_CODE < KERNEL_VERSION(3,17,0)
   dev = alloc_netdev(sizeof_priv,"rsi_wlan0",ether_setup);
#else
   dev = alloc_netdev(sizeof_priv,"rsi_wlan0", NET_NAME_UNKNOWN, ether_setup);
#endif
#ifdef RSI_DEBUG_PRINT
   RSI_DEBUG(RSI_ZONE_INIT,"rsi_allocdev: device allocated for rsi_wlan0 %p\n", dev);
#endif
#else //ENABLE_WMM_FEATURE
#if (LINUX_VERSION_CODE < KERNEL_VERSION(2,6,38))
	dev = alloc_etherdev_mq(sizeof_priv, 4);
#elif (LINUX_VERSION_CODE < KERNEL_VERSION(3,17,0))
	dev = alloc_netdev_mqs(sizeof_priv,"rsi_wlan0",
					ether_setup, 4, 1);
#else
	dev = alloc_netdev_mqs(sizeof_priv,"rsi_wlan0",NET_NAME_UNKNOWN,
					ether_setup, 4, 1);
#endif	
#endif //ENABLE_WMM_FEATURE
  }
  else if (vap_id == 1)
  {

   //! AP interface in concurrent mode
#if LINUX_VERSION_CODE < KERNEL_VERSION(3,17,0)
   dev = alloc_netdev(sizeof_priv,"rsi_wlan1",ether_setup);
#else
   dev = alloc_netdev(sizeof_priv,"rsi_wlan1", NET_NAME_UNKNOWN, ether_setup);
#endif
#ifdef RSI_DEBUG_PRINT
   RSI_DEBUG(RSI_ZONE_INIT,"rsi_allocdev: device allocated for rsi_wlan1\n");
#endif
  }
  return dev;
}

/*==============================================*/
/**
 * @fn          INT32 rsi_registerdev(struct net_device *dev)
 * @brief       Register the network device
 * @param[in]   dev, pointer to our network device structure
 * @param[out]  none
 * @return      On success 0 is returned else a negative value signifying
 *              failure
 * @section description
 * This function is used to register the network device.
 */
INT32
rsi_registerdev
(
  struct net_device *dev
)
{
#ifdef RSI_DEBUG_PRINT
  RSI_DEBUG(RSI_ZONE_SPI_DBG,"\nrsi_registerdev:\n");
#endif
  return register_netdev(dev);
}

/*==============================================*/
/**
 * @fn          INT32 rsi_unregisterdev(struct net_device *dev)
 * @brief       Unregister the network device
 * @param[in]   dev, pointer to our network device structure
 * @param[out]  none
 * @return      On success 0 is returned else a negative value signifying
 *              failure
 * @section description
 * This function is used to unregisters the network device
 * & returns it back to the kernel
 */
VOID
rsi_unregisterdev
(
  struct net_device *dev
)
{
#ifdef RSI_DEBUG_PRINT
  RSI_DEBUG(RSI_ZONE_SPI_DBG,"\nrsi_unregisterdev:\n");
#endif
  unregister_netdev(dev);
  free_netdev(dev);
  return;
}

/*==============================================*/
/**
 * @fn          struct net_device_stats *
 *              rsi_get_stats(struct net_device *dev)
 * @brief       Gives the stats info
 * @param[in]   dev, pointer to our network device structure
 * @param[out]  none
 * @return      Pointer to the net_device_stats structure
 * @section description
 * This function gives the statistical information regarding
 * the interface
 */
struct net_device_stats*
rsi_get_stats
(
  struct net_device *dev
)
{
#ifdef RSI_DEBUG_PRINT
  RSI_DEBUG(RSI_ZONE_SPI_DBG,"\nrsi_get_stats:\n");
#endif
  PRSI_ADAPTER Adapter = rsi_getpriv(dev);
  return &Adapter->stats;
}

/*==============================================*/
/**
 * @fn          PRSI_ADAPTER rsi_getpriv(struct net_device *dev)
 * @brief       Get the pointer for the adapter
 * @param[in]   dev, pointer to our network device structure
 * @param[out]  none
 * @return      Pointer to the adapter On success 0, negative value signifying
 *              failure
 * @section description
 * This function get the pointer for the adapter
 */
PRSI_ADAPTER rsi_getpriv
(
  struct net_device *dev
)
{
  PRSI_NET_DEV rsi_net_device = netdev_priv(dev); 
  return rsi_net_device->rsi_adapter_ptr;
}

/*==============================================*/
/**
 * @fn          PRSI_NET_DEV rsi_getdevpriv(struct net_device *dev)
 * @brief       Get the pointer for the adapter
 * @param[in]   dev, pointer to our network device structure
 * @param[out]  none
 * @return      Pointer to the adapter On success 0, negative value signifying
 *              failure
 * @section description
 * This function get the pointer for the adapter
 */
PRSI_NET_DEV rsi_getdevpriv
(
  struct net_device *dev
)
{
  PRSI_NET_DEV rsi_net_device = netdev_priv(dev); 
  return rsi_net_device;
}

/*==============================================*/
/**
 * @fn          INT32 rsi_ioctl(struct net_device *dev,
 *                              struct ifreq *ifr,
 *                              INT32 cmd)
 * @brief       Gives the stats info
 * @param[in]   dev, pointer to our network device structure
 * @param[in]   ifr, pointer to ifr structure
 * @param[in]   cmd, type of command or request
 * @param[out]  none
 * @return      Pointer to the net_device_stats structure
 * @section description
 * This function is registered to driver and will be called when
 * ioctl issued from user space.
 */
INT32 rsi_ioctl
(
  struct net_device *dev,
  struct ifreq *ifr,
  INT32 cmd
)
{
#ifdef RSI_DEBUG_PRINT
  RSI_DEBUG(RSI_ZONE_SPI_DBG,"\nrsi_ioctl:\n");
#endif
  PRSI_ADAPTER Adapter = rsi_getpriv(dev);
  struct iwreq *wrq =(struct iwreq *)ifr;
  int16 retval = 0;
  uint8 local_buffer[4096];
  uint16 data = 0;
  uint32 addr,len,i,len_dwords;
  uint8 get_status = 0;

  switch(cmd)
  {
    case OID_WSC_GET_STATUS:
      {
        rsi_down_interruptible(&Adapter->int_check_sem);
#ifdef RSI_SPI_INTERFACE
/*
        if(Adapter->power_save_enable)
        {
#if (RSI_POWER_MODE == RSI_POWER_MODE_2)||(RSI_POWER_MODE == RSI_POWER_MODE_8)
#if (RSI_WMM_PS_SUPPORT && RSI_WMM_PS_TYPE)
          retval = (int16)rsi_wait4wakeup();
#else
          retval = (int16)rsi_req_wakeup();
#endif
#endif
        }
        if(retval == 0)
          retval = rsi_checkBufferFullIrq(&get_status);
*/
#if (RSI_POWER_MODE == RSI_POWER_MODE_2)||(RSI_POWER_MODE == RSI_POWER_MODE_8)
        config_gpio_output(0);
#endif
#endif
        rsi_up_sem(&Adapter->int_check_sem);

        if(retval && copy_to_user(wrq->u.data.pointer,&retval,sizeof(UINT8)))
        {
          RSI_DEBUG(RSI_ZONE_SPI_DBG,"rsi_ioctl : Failed to perform operation\n");
          return -EFAULT;
        }
        else if(copy_to_user(wrq->u.data.pointer,&get_status,sizeof(UINT8)))
        {
          RSI_DEBUG(RSI_ZONE_SPI_DBG,"rsi_ioctl : Failed to perform operation\n");
          return -EFAULT;
        }
      }
      break;
#ifndef RSI_UART_INTERFACE
    case OID_WSC_BOOT_READ:
      {
        rsi_down_interruptible(&Adapter->int_check_sem);
        retval = rsi_boot_insn(REG_READ, &data);
        rsi_up_sem(&Adapter->int_check_sem);
        if(retval < 0)
        {
          return -EFAULT;
        }
        if(copy_to_user(wrq->u.data.pointer, &data, sizeof(uint16)))
        {
          RSI_DEBUG(RSI_ZONE_SPI_DBG,"rsi_ioctl : Failed to perform operation\n");
          return -EFAULT;
        }
      }
      break;
    case OID_WSC_BOOT_WRITE:
      {
        rsi_down_interruptible(&Adapter->int_check_sem);
        copy_from_user(local_buffer, wrq->u.data.pointer, 2);
        retval = rsi_boot_insn(REG_WRITE, (uint16 *)local_buffer);
        rsi_up_sem(&Adapter->int_check_sem);
        if(retval < 0)
        {
          return -EFAULT;
        }
      }
      break;
    case OID_WSC_BOOT_PING_WRITE:
      {
        rsi_down_interruptible(&Adapter->int_check_sem);
        copy_from_user(local_buffer, wrq->u.data.pointer, 4096);
        retval = rsi_boot_insn(PING_WRITE, (uint16 *)local_buffer);
        rsi_up_sem(&Adapter->int_check_sem);
        if(retval < 0)
        {
          return -EFAULT;
        }
      }
      break;
    case OID_WSC_BOOT_PONG_WRITE:
      {
        rsi_down_interruptible(&Adapter->int_check_sem);
        copy_from_user(local_buffer, wrq->u.data.pointer, 4096);
        retval = rsi_boot_insn(PONG_WRITE, (uint16 *)local_buffer);
        rsi_up_sem(&Adapter->int_check_sem);
        if(retval < 0)
        {
          return -EFAULT;
        }
      }
      break;
#endif
    case OID_WSC_POWER_SAVE_ENABLE:
      {
        uint8 currentPowerMode = 0;
        rsi_down_interruptible(&Adapter->int_check_sem);
        copy_from_user(&currentPowerMode, wrq->u.data.pointer, 1);
        Adapter->power_save_enable = !!currentPowerMode;
        rsi_up_sem(&Adapter->int_check_sem);
      }
      break;
#ifdef RSI_SPI_INTERFACE
/*
    case OID_WSC_WAKEUP:
      {
        rsi_req_wakeup();
      }
      break;
*/
#endif
    case OID_MASTER_READ:
      {
        addr = *(uint32_t *)wrq->u.data.pointer; 
        len = wrq->u.data.length;
        len_dwords = (len & 3)? ((len/4)+1) : (len/4) ;
        memset(local_buffer,0,sizeof(local_buffer));
        rsi_down_interruptible(&Adapter->int_check_sem);
        for(i = 0; i< len_dwords; i++)
        {
          retval = rsi_boot_req(local_buffer+(i*4),addr+(i*4));
        }
        rsi_up_sem(&Adapter->int_check_sem);
        if(retval < 0)
        {
          return -EFAULT;
        }
        if(copy_to_user((wrq->u.data.pointer),local_buffer,len))
        {
          RSI_DEBUG(RSI_ZONE_SPI_DBG,"rsi_ioctl : failed to perform\n");
          return -EFAULT;
        }
      }
      break;
  }
  return retval;
}

/*==============================================*/
/**
 * @fn          INT32 rsi_open(struct net_device *dev)
 * @brief       Opens the interface
 * @param[in]   dev, pointer to our network device structure
 * @param[out]  none
 * @return      On success 0 is returned else a negative value signifying
 *              failure
 * @section description
 * This function opens the interface
 */
INT32
rsi_open
(
  struct net_device *dev
)
{
#ifdef RSI_DEBUG_PRINT
  RSI_DEBUG(RSI_ZONE_SPI_DBG,"\nrsi_open:\n");
#endif
  RSI_DEBUG(RSI_ZONE_INIT,"rsi_open:\n");
  return 0;
}

/*==============================================*/
/**
 * @fn          INT32 rsi_stop(struct net_device *dev)
 * @brief       Stops/brings down the interface
 * @param[in]   dev, pointer to our network device structure
 * @param[out]  none
 * @return      On success 0 is returned else a negative value signifying
 *              failure
 * @section description
 * This function stops/brings down the interface
 */
INT32
rsi_stop
(
  struct net_device *dev
)
{
#ifdef RSI_DEBUG_PRINT
  RSI_DEBUG(RSI_ZONE_SPI_DBG,"\nrsi_stop:\n");
#endif
  RSI_DEBUG(RSI_ZONE_INFO,"rsi_stop\n");
  return 0;
}

#ifdef ENABLE_WMM_FEATURE
/*==================================================*/
#if (LINUX_VERSION_CODE < KERNEL_VERSION(3,13,0))
u16 rsi_select_queue(struct net_device *dev,
					struct sk_buff *skb)
#elif (LINUX_VERSION_CODE < KERNEL_VERSION(3,14,0))
u16 rsi_select_queue(struct net_device *dev,
					struct sk_buff *skb,
					void *accel_priv)
#elif (LINUX_VERSION_CODE < KERNEL_VERSION(4,19,0))
u16 rsi_select_queue(struct net_device *dev,
					struct sk_buff *skb,
					void *accel_priv,
					select_queue_fallback_t fallback)
#elif (LINUX_VERSION_CODE < KERNEL_VERSION(5,2,0))
u16 rsi_select_queue(struct net_device *dev,
					struct sk_buff *skb,
					struct net_device *sb_dev,
					select_queue_fallback_t fallback)
#elif (LINUX_VERSION_CODE >= KERNEL_VERSION(5,2,0))
u16 rsi_select_queue(struct net_device *dev,
					struct sk_buff *skb,
					struct net_device *sb_dev)
#endif
{
#define	ETHERTYPE_IPV6		0x86dd
	u16 tos=0;
	u8  qid=0;
	
	/* Returning default Access Category if TX_q's are not 4, insignificant skb data length and for Multicast packets*/ 
	if ((dev->num_tx_queues < 4) || (skb->len < 6) || (skb->data[0] & 0x1)) {
		skb->priority = 0; 
		//RSI_DEBUG(RSI_ZONE_ERROR,"Invalid skb rcvd\n");
		return qid;
	}
	
	/* Incase of device encap pick the tos from the IP hdr */
	if (cpu_to_le16(*(u16 *)&skb->data[12]) == htons(ETHERTYPE_IPV6)) {
		tos = (uint8_t)(cpu_to_le32(*(u32 *)&skb->data[14]) >> 20);
		tos >>= 5;
	} else {
		tos = skb->data[15] >> 5;
	}
	if(tos > 7)
	{
		//RSI_DEBUG(RSI_ZONE_ERROR,"Unsupported tos:%d making zero\n",tos);
		tos=0;
	}
	qid = TID_TO_WME_AC(tos);	
	
	//RSI_DEBUG(RSI_ZONE_ERROR,"Rcvd packet of tos:%d and assigned qid:%d\n",tos,qid);
	skb->priority = tos;
	return qid;
}

#endif

/*==============================================*/
/**
 * @fn          struct net_device* rsi_netdevice_op(vap_id)
 * @brief       netdevice related operations
 * @param[in]   vap_id device vap id
 * @param[out]  none
 * @return      dev, Pointer to net device structure
 * @section description
 * This function performs all net device related operations like
 * allocating,initializing and registering the netdevice.
 */
struct net_device*
rsi_netdevice_op
(
  UINT8 vap_id
)
{
#ifdef RSI_DEBUG_PRINT
  RSI_DEBUG(RSI_ZONE_SPI_DBG,"\nrsi_netdevice_op:\n");
#endif
  struct net_device *dev;
  PRSI_NET_DEV net_dev;

  /*Allocate & initialize the network device structure*/
  dev = rsi_allocdev(sizeof(RSI_NET_DEV), vap_id);

  if (dev == NULL)
  {
    RSI_DEBUG(RSI_ZONE_ERROR,
              "rsi_netdevice_op: Failure in allocation of net-device\n");
    return dev;
  }

  //! Net device settings init
  net_dev = rsi_getdevpriv(dev);
  net_dev->rsi_adapter_ptr = Adapter;

#if KERNEL_VERSION_LESS_THAN_EQUALS_2_6_(28)
  dev->open                 = rsi_open;
  dev->stop                 = rsi_stop;
  dev->hard_start_xmit      = rsi_xmit;
  dev->get_stats            = rsi_get_stats;
  dev->do_ioctl             = rsi_ioctl;
  dev->hard_header_len      = 30;
#else
  static struct net_device_ops dev_ops =
    {
      .ndo_open               =    rsi_open,
      .ndo_stop               =    rsi_stop,
      .ndo_start_xmit         =    rsi_xmit,
      .ndo_do_ioctl           =    rsi_ioctl,
      .ndo_get_stats          =    rsi_get_stats,
#ifdef ENABLE_WMM_FEATURE
      .ndo_select_queue       =    rsi_select_queue,	
#endif      
    };

  dev->netdev_ops           = &dev_ops;
  dev->hard_header_len      = 30;
#endif
  if (rsi_registerdev(dev) != 0)
  {
    RSI_DEBUG(RSI_ZONE_ERROR,
              "rsi_netdevice_op: Registration of net-device failed for vap id %d\n", vap_id);
    free_netdev(dev);
    return NULL;
  }

  return dev;

}
#ifdef RSI_USB_INTERFACE

/*==============================================*/
/**
 * @fn          find_bulkInAndOutEndpoints (struct usb_interface *interface, RSI_ADAPTER *adapter)
 * @brief       This function initializes the bulk endpoints to the device
 * @param[in]   struct usb_interface *interface, pointer to usb interaface structure
 * @param[in]   RSI_ADAPTER *adapter, pointer to adapter structure
 * @return      0 on success and -1 on failure
 * @section description
 * This function initializes the bulk endpoints to the device
 */
int find_bulkInAndOutEndpoints (struct usb_interface *interface,
    RSI_ADAPTER *adapter)
{
  struct usb_host_interface *iface_desc;
  struct usb_endpoint_descriptor *endpoint;
  int buffer_size;

  int rx_bulk_ep_index = 0;
  int tx_bulk_ep_index = 0;

  int ret_val = -ENOMEM, i, bep_found = 0;


  /* set up the endpoint information */
  /* check out the endpoints */
  /* use only the first bulk-in and bulk-out endpoints */

  if ((interface == NULL) || (adapter == NULL)) {
    return ret_val;
  }

  iface_desc = &(interface->altsetting[0]);

  RSI_DEBUG (RSI_ZONE_INFO,
      ("bNumEndpoints :%#10lx \n", iface_desc->desc.bNumEndpoints));
  for (i = 0; i < iface_desc->desc.bNumEndpoints; ++i) {
    endpoint = &(iface_desc->endpoint[i].desc);

    RSI_DEBUG (RSI_ZONE_INFO, ("IN LOOP :%#10lx \n", bep_found));
#if LINUX_VERSION_CODE == KERNEL_VERSION(2,6,18)
if ((!(adapter->rx_endpoint_address[rx_bulk_ep_index])) && 
        (endpoint->bEndpointAddress & USB_DIR_IN)&&((endpoint->bmAttributes & USB_ENDPOINT_XFERTYPE_MASK) == USB_ENDPOINT_XFER_BULK)) 
#else
    if ((!(adapter->rx_endpoint_address[rx_bulk_ep_index])) && 
        (usb_endpoint_is_bulk_in(endpoint))) 
#endif
		{
      /* we found a bulk in endpoint */
      RSI_DEBUG (RSI_ZONE_INFO, ("IN EP :%#10lx \n", i));
      buffer_size = rsi_bytes2R_to_uint16(&endpoint->wMaxPacketSize);
      //buffer_size = MAX_RX_PKT_SZ;
      adapter->rx_len[rx_bulk_ep_index]= buffer_size;
      adapter->rx_endpoint_address[rx_bulk_ep_index++] = endpoint->bEndpointAddress;
    }
#if LINUX_VERSION_CODE == KERNEL_VERSION(2,6,18)
    if ((!(adapter->tx_endpoint_address[tx_bulk_ep_index])) && 
        (endpoint->bEndpointAddress & USB_DIR_IN)&&((endpoint->bmAttributes & USB_ENDPOINT_XFERTYPE_MASK) == USB_ENDPOINT_XFER_BULK)) 
#else
    if (!adapter->tx_endpoint_address[tx_bulk_ep_index] &&
        (usb_endpoint_is_bulk_out(endpoint))) 
#endif
		{
      RSI_DEBUG (RSI_ZONE_INFO, ("OUT EP :%#10lx \n", i));
      /* we found a bulk out endpoint */
      adapter->tx_endpoint_address[tx_bulk_ep_index] = endpoint->bEndpointAddress;
      /* on some platforms using this kind of buffer alloc
       * call eliminates a dma "bounce buffer".
       *
       * NOTE: you'd normally want i/o buffers that hold
       * more than one packet, so that i/o delays between
       * packets don't hurt throughput.
       */
      buffer_size = rsi_bytes2R_to_uint16(&endpoint->wMaxPacketSize);
      adapter->tx_len[tx_bulk_ep_index++] = buffer_size;
      //adapter->tx_urb->transfer_flags = (URB_NO_TRANSFER_DMA_MAP );
      
    }
  }
   printk("endpoint 1 pkt sizes tx %d, rx %d \n",adapter->tx_len[USB_WLAN],adapter->rx_len[USB_WLAN]);
   if ((adapter->tx_endpoint_address[USB_BT_ZB] && adapter->rx_endpoint_address[USB_BT_ZB])) 
    printk("endpoint 2 pkt sizes tx %d, rx %d \n",adapter->tx_len[USB_BT_ZB],adapter->rx_len[USB_BT_ZB]);
  if (!(adapter->tx_endpoint_address[USB_WLAN] && adapter->rx_endpoint_address[USB_WLAN])) {
    printk("Couldn't find both bulk-in and bulk-out endpoints");
    return ret_val;
  } else {
     if (!(adapter->tx_endpoint_address[USB_BT_ZB] && adapter->rx_endpoint_address[USB_BT_ZB])) 
     {
       printk("Couldn't find both bulk-in and bulk-out endpoints");
       return ret_val;
     }
     else
       RSI_DEBUG (RSI_ZONE_INFO, ("EP INIT SUCESS \n"));
  }

  return 0;
}


/*==============================================*/
/**
 * @fn          RSI_STATUS rsi_usb_disconnect(VOID)
 * @brief       Reverse of probe
 * @param[in]   nstruct usb_interface usb interface
 * @param[out]  none
 * @return      RSI_STATUS_SUCCESS in case of successful initialization
 *              or a negative error code signifying failure
 * @section description
 * This function performs the reverse of the probe function.
 */
void rsi_usb_disconnect(struct usb_interface *intf)
{
  RSI_DEBUG(RSI_ZONE_SPI_DBG,"\nrsi_usb_disconnect:\n");
  RSI_STATUS Status = RSI_STATUS_SUCCESS;
  UINT32 ii;
  if(!intf)
  {
    RSI_DEBUG(RSI_ZONE_INFO,"rsi_usb_disconnect: invalid interface\n");
   return -1;
  }
  RSI_DEBUG(RSI_ZONE_SPI_DBG,"\nrsi_usb_disconnect: interface is correct\n");
  PRSI_ADAPTER Adapter = usb_get_intfdata(intf);
 if(!Adapter)
 {
    RSI_DEBUG(RSI_ZONE_INFO,"rsi_usb_disconnect: invalid adapter\n");
  return -1;
 }
 RSI_DEBUG(RSI_ZONE_SPI_DBG,"\nrsi_usb_disconnect: adapter correct\n");
  RSI_DEBUG(RSI_ZONE_INFO,"rsi_linux_disconnect: Deinitializing\n");

  Adapter->FSM_STATE  = 0;
  rsi_Delete_Event(&Adapter->PwrSaveEvent);
  rsi_Delete_Event(&Adapter->Event);

  RSI_DEBUG(RSI_ZONE_INFO,"Killing thread \n");
  Adapter->halt_flag = 1;

  rsi_Kill_Thread(Adapter);

  RSI_DEBUG(RSI_ZONE_INFO,"Purge queue \n");
  for (ii = 0; ii < 4; ii++)
  {
    rsi_queue_purge(&Adapter->list[ii]);
  }
  
  usb_set_intfdata (Adapter->interface, NULL);
  if(Adapter->rx_usb_urb[USB_WLAN]) {
    usb_kill_urb(Adapter->rx_usb_urb[USB_WLAN]);
  }
 if(Adapter->rx_usb_urb[USB_WLAN]->transfer_buffer)
 {
  kfree(Adapter->rx_usb_urb[USB_WLAN]->transfer_buffer);
 }
  if(Adapter->rx_usb_urb[USB_WLAN])
  {
  usb_free_urb(Adapter->rx_usb_urb[USB_WLAN]);
  }
  if(Adapter->rx_usb_urb[USB_BT_ZB])
  {
    usb_kill_urb(Adapter->rx_usb_urb[USB_BT_ZB]);
  }
  if(Adapter->rx_usb_urb[USB_BT_ZB]->transfer_buffer)
  {
    kfree(Adapter->rx_usb_urb[USB_BT_ZB]->transfer_buffer);
  }
  if(Adapter->rx_usb_urb[USB_BT_ZB])
  {
    usb_free_urb(Adapter->rx_usb_urb[USB_BT_ZB]);
  }
#ifdef RS9116
  if(Adapter->rx_usb_urb[USB_ZB_ONLY])
  {
    usb_kill_urb(Adapter->rx_usb_urb[USB_ZB_ONLY]);
  }
  if(Adapter->rx_usb_urb[USB_ZB_ONLY]->transfer_buffer)
  {
    kfree(Adapter->rx_usb_urb[USB_ZB_ONLY]->transfer_buffer);
  }
  if(Adapter->rx_usb_urb[USB_ZB_ONLY])
  {
    usb_free_urb(Adapter->rx_usb_urb[USB_ZB_ONLY]);
  }
#endif
  /*Disable the interface*/
  RSI_DEBUG(RSI_ZONE_INFO,"Unregister netdev \n");
  rsi_unregisterdev(Adapter->net_device0);
#if RSI_CONCURRENT_MODE
  rsi_unregisterdev(Adapter->net_device1);
#endif
  RSI_DEBUG(RSI_ZONE_INFO,"rsi_linux_disconnect: Disabling the interface\n");
  return Status;
}
#endif



/*==============================================*/
/**
 * @fn          RSI_STATUS rsi_probe(VOID)
 * @brief       All the initializations at load time
 * @param[in]   none
 * @param[out]  none
 * @return      RSI_STATUS_SUCCESS in case of successful initialization
 *              or a negative error code signifying failure
 * @section description
 * This function is called at the module load time.
 * All the initialization work is done here.
 */
#ifdef RSI_SPI_INTERFACE
RSI_STATUS
rsi_probe
(
  struct spi_device *spi_device
)
#endif
#ifdef RSI_USB_INTERFACE
RSI_STATUS
rsi_usb_probe
(
  struct usb_interface *interface,
  const struct usb_device_id *id
)
#endif
#ifdef RSI_UART_INTERFACE
RSI_STATUS
rsi_uart_probe()
#endif
#ifdef RSI_SDIO_INTERFACE
#if  KERNEL_VERSION_BTWN_2_6_(18, 22)
BOOLEAN __devinit rsi_sdio_probe(PSDFUNCTION pfunction, PSDDEVICE   pDevice)
#elif KERNEL_VERSION_GREATER_THAN_EQUALS_2_6_(26)
int   rsi_sdio_probe(struct sdio_func *pfunction, const struct sdio_device_id *id)
#endif
#endif
{
#ifdef RSI_SPI_INTERFACE
  if (Already_probed) {
    printk("rsi_probe: Already probe\n");
    return -EALREADY;
  }
  Already_probed = true;
#endif
#ifdef RSI_DEBUG_PRINT
  RSI_DEBUG(RSI_ZONE_SPI_DBG,"\nrsi_probe:\n");
#endif
  struct net_device *dev = NULL;
#if RSI_CONCURRENT_MODE
  struct net_device *dev_ap = NULL;
#endif  
  RSI_STATUS status = RSI_STATUS_SUCCESS;
  UINT8 ii;
  UINT16 retval = 0;
  RSI_DEBUG(RSI_ZONE_INIT,"rsi_probe: Initialization function called\n");

  //! Freeing Adapter if it is already allocated.
  if(Adapter != NULL)
  {
    RSI_DEBUG(RSI_ZONE_INIT,"rsi_probe: Freed Adapter\n");
    kfree(Adapter);
    Adapter = NULL;
  }

  //! Allocating memory for Adapter from kernel Heap memory pool
  Adapter = (PRSI_ADAPTER)kmalloc(sizeof(RSI_ADAPTER),GFP_ATOMIC);

  if(Adapter == NULL)
  {
    RSI_DEBUG(RSI_ZONE_INIT,"rsi_probe: kmalloc failed\n");
    return RSI_STATUS_FAIL;
  }

  //! Reseting the adapter
  rsi_memset(Adapter, 0, sizeof(RSI_ADAPTER));

  Adapter->DataRcvPacket = kzalloc((MAX_BUFF_SIZE *4), GFP_KERNEL);
  if(Adapter->DataRcvPacket == NULL)
  {
    RSI_DEBUG(RSI_ZONE_INIT,"rsi_probe: kmalloc failed\n");
    return RSI_STATUS_FAIL;
  }

  dev = rsi_netdevice_op(0);
  if (!dev)
  {
    RSI_DEBUG(RSI_ZONE_ERROR,
              "rsi_probe: Failed to perform netdevice operations\n");
    return RSI_STATUS_FAIL;
  }

#if RSI_CONCURRENT_MODE
  dev_ap = rsi_netdevice_op(1);
  if (!dev_ap)
  {
    RSI_DEBUG(RSI_ZONE_ERROR,
              "rsi_probe: Failed to perform netdevice operations for AP1\n");
    return RSI_STATUS_FAIL;
  }
#endif

  RSI_DEBUG(RSI_ZONE_INIT,
            "rsi_probe: Net device operations suceeded\n");

#ifdef ENABLE_WMM_FEATURE  
  dbg_test_values(Adapter);
#endif

#ifdef RSI_SPI_INTERFACE
  Adapter->spi = spi_device;
  spi_dev = spi_device;
  Adapter->dummy_buf = kzalloc(MAX_SPI_PKT_SIZE, GFP_KERNEL);
  sema_init(&Adapter->spi_dev_sem, 1);
#endif
#ifdef RSI_USB_INTERFACE
  Adapter->usbdev = usb_get_dev(interface_to_usbdev(interface));
  Adapter->interface = interface;
#endif



  Adapter->net_device0 = dev;
#if RSI_CONCURRENT_MODE
  Adapter->net_device1 = dev_ap;
#endif
  glbl_net_device = dev;

#ifdef RSI_USB_INTERFACE
if (find_bulkInAndOutEndpoints (interface, Adapter)) {
    goto fail_out;
  }
  usb_set_intfdata (interface, Adapter);

  RSI_DEBUG(RSI_ZONE_INIT,(TEXT("usb_probe: Enabled the interface\n")));

	Adapter->max_rx_len[USB_WLAN] = USB_MAX_PACKET_SIZE;
  Adapter->rx_usb_urb[USB_WLAN] =  usb_alloc_urb(0, GFP_KERNEL);
  Adapter->rx_usb_urb[USB_WLAN]->transfer_buffer = (UINT8 *)kmalloc(Adapter->max_rx_len[USB_WLAN], GFP_ATOMIC);
  if(Adapter->rx_endpoint_address[USB_BT_ZB])
	{
		Adapter->max_rx_len[USB_BT_ZB] = USB_MAX_PACKET_SIZE;
		Adapter->rx_usb_urb[USB_BT_ZB] = usb_alloc_urb(0, GFP_KERNEL);
		Adapter->rx_usb_urb[USB_BT_ZB]->transfer_buffer = (UINT8 *)kmalloc(Adapter->max_rx_len[USB_BT_ZB], GFP_ATOMIC);
	}
#ifdef RS9116
	if(Adapter->rx_endpoint_address[USB_ZB_ONLY])
	{
		Adapter->max_rx_len[USB_ZB_ONLY] = USB_MAX_PACKET_SIZE;
		Adapter->rx_usb_urb[USB_ZB_ONLY] = usb_alloc_urb(0, GFP_KERNEL);
		Adapter->rx_usb_urb[USB_ZB_ONLY]->transfer_buffer = (UINT8 *)kmalloc(Adapter->max_rx_len[USB_ZB_ONLY], GFP_ATOMIC);
	}
#endif
#endif

  for (ii = 0; ii < 4; ii++)
  {
    rsi_queue_init(&Adapter->list[ii]);
  }
  rsi_queue_init(&Adapter->rx_q);
#ifdef ENABLE_WMM_FEATURE
  Adapter->max_q_len[WME_AC_BK] = QUEUE_DEPTH_BK;
  Adapter->max_q_len[WME_AC_BE] = QUEUE_DEPTH_BE;
  Adapter->max_q_len[WME_AC_VI] = QUEUE_DEPTH_VI;
  Adapter->max_q_len[WME_AC_VO] = QUEUE_DEPTH_VO;
#endif
  Adapter->workqueue = create_singlethread_workqueue("gdvr_work");
  if (Adapter->workqueue == NULL)
  {
    RSI_DEBUG(RSI_ZONE_INIT,"Work queue Fail\n");
    goto fail;

  }
#ifdef RSI_SDIO_INTERFACE
	RSI_DEBUG(RSI_ZONE_INIT,"\n SDIO: claiming host");
	rsi_sdio_claim_host(pfunction);
#if KERNEL_VERSION_BTWN_2_6_(18, 22)
	status = rsi_enable_interface(pDevice);

	if (!SDIO_SUCCESS((status)))
	{
		RSI_DEBUG(RSI_ZONE_ERROR,(TEXT
		             ("%s: Failed to enable interface for the kernels b/w 2.6.18 and 2.6.22\n"), __func__));
		rsi_sdio_release_host(pfunction);
		return status; 
	}
#elif KERNEL_VERSION_GREATER_THAN_2_6_(26)
	status = rsi_enable_interface(pfunction);
	if (status != 0)
	{
		RSI_DEBUG(RSI_ZONE_SDIO_DBG,"%s: Failed to enable interface\n", __func__);
		rsi_sdio_release_host(pfunction);
		return status; 
	}
#endif

	rsi_sdio_release_host(pfunction);
#endif

#ifdef RSI_USB_INTERFACE
INIT_WORK((struct work_struct *)&Adapter->usb_rx_work,  &usb_rx_packet_parser);
#else
#ifdef RSI_SDIO_INTERFACE
  /*Enable the SPI interface*/
#if (LINUX_VERSION_CODE == KERNEL_VERSION(2,6,18))
  INIT_WORK(&Adapter->handler,(void *)rsi_interrupt_handler,(void *)&Adapter->handler);
#else
  INIT_WORK((struct work_struct *)&Adapter->defer_work,  &deferred_rx_packet_parser);
#endif
#endif
#endif
  RSI_DEBUG(RSI_ZONE_INIT,"rsi_probe: Enabled the interface\n");

#ifdef RSI_UART_INTERFACE
#if RSI_TCP_IP_BYPASS
 rsi_spinlock_init(&Adapter->uart_data_ack_flag_sem);
#endif
#endif
  rsi_spinlock_init(&Adapter->lockqueue);
#ifdef init_MUTEX
  rsi_init_mutex(&Adapter->int_check_sem);
  rsi_init_mutex(&Adapter->sleep_ack_sem); //default
  rsi_init_mutex(&Adapter->tx_data_sem); //default
#else
  rsi_sema_init(&Adapter->int_check_sem,1);
  rsi_sema_init(&Adapter->sleep_ack_sem,1);
  rsi_sema_init(&Adapter->tx_data_sem,1);
  rsi_sema_init(&Adapter->rx_data_sem,1);
#endif

  /* Requesting thread */
  rsi_Init_Event(&Adapter->Event);
#ifdef RSI_UART_INTERFACE
#if RSI_TCP_IP_BYPASS
  rsi_Init_Event(&Adapter->DATA_ACKEvent);
#endif
#endif
  
  rsi_Init_Event(&Adapter->PwrSaveEvent);
  rsi_Init_Thread(Adapter);
  RSI_DEBUG(RSI_ZONE_INIT,"rsi_probe: Initialized thread & Event\n");
#ifdef RSI_SPI_INTERFACE
  mutex_init(&Adapter->rx_lock);
  rsi_Init_Event(&Adapter->spi_rx_thread_done_event);
  rsi_Init_Event(&Adapter->rx_thread.event);

  status = rsi_create_kthread(Adapter, &Adapter->rx_thread,
		  rsi_spi_rx_thread, "SPI-RX-Thread");
  if (status) {
	  RSI_DEBUG(RSI_ZONE_ERROR, "%s: Unable to init RX-Thread\n", __func__);
	  goto fail;
  }

  if (rsi_init_interface(Adapter,spi_device) < 0 )
  {
	  RSI_DEBUG(RSI_ZONE_ERROR,"rsi_probe: Failed to enable interface\n");
	  goto fail;
  }
  else
  {
	  Adapter->irq_registered = 1;
  }
#endif


#ifdef RSI_SDIO_INTERFACE
#if KERNEL_VERSION_BTWN_2_6_(18, 22)
	Adapter->pDevice    = pDevice;
#elif KERNEL_VERSION_GREATER_THAN_2_6_(26)
	Adapter->pfunction = pfunction;
#endif
	
	rsi_setcontext(pfunction, (void *)Adapter);
	/*Initalizing the hardware*/
	rsi_sdio_claim_host(pfunction);

	status = rsi_setupcard(Adapter);
	if (status != 0)
	{
		RSI_DEBUG(RSI_ZONE_SDIO_DBG,(TEXT("%s:Failed to setup card\n"), __func__));
	 	rsi_sdio_release_host(pfunction);
		goto fail;
	}

    	RSI_DEBUG(RSI_ZONE_SDIO_DBG,(TEXT("%s: Setup card succesfully\n"), __func__));
#if LINUX_VERSION_CODE == KERNEL_VERSION(2,6,18)
 Adapter->sdio_high_speed_enable = 1;
#endif
	rsi_sdio_release_host(pfunction);
	
	if(rsi_init_host_interface(Adapter)!= 0)
	{
		RSI_DEBUG(RSI_ZONE_SDIO_DBG,(TEXT("%s:Failed to init slave regs\n"), __func__));
	 	rsi_sdio_release_host(pfunction);
		goto fail;
	}
	rsi_sdio_claim_host(pfunction);
	status= rsi_request_interrupt_handler(pfunction, rsi_sdio_interrupt_handler, Adapter);
	if (status != 0)
	{
		printk("Failed to unmask IRQ\n");
		rsi_sdio_release_host(pfunction);
		goto fail;
	}
	rsi_sdio_release_host(pfunction);
	Adapter->irq_registered = 1;
#endif

  return status;
  /*Failure in one of the steps will cause the control to be transferred here*/
fail_out:
  for (ii = 0; ii < 4; ii++)
  {
    rsi_queue_purge(&Adapter->list[ii]);
  }
  rsi_queue_purge(&Adapter->rx_q);
  flush_workqueue(Adapter->workqueue);
  destroy_workqueue(Adapter->workqueue);
#ifdef RSI_SDIO_INTERFACE
    rsi_sdio_claim_host(pfunction);
  	rsi_sdio_release_irq(pfunction);

  	/*Disable the interface*/
  #if KERNEL_VERSION_BTWN_2_6_(18, 22)
  	rsi_disable_interface(pDevice);
  #elif KERNEL_VERSION_GREATER_THAN_2_6_(26)
  	rsi_disable_interface(pfunction);
  #endif
  	/* Release the host. It should be called after calling sdio_disable_func() */
  	rsi_sdio_release_host(pfunction);

  	RSI_DEBUG(RSI_ZONE_SDIO_DBG,(TEXT("%s: Failed to initialize...Exiting\n"), __func__));
#if KERNEL_VERSION_BTWN_2_6_(18, 22)
	return 1;
#elif KERNEL_VERSION_GREATER_THAN_EQUALS_2_6_(26)
	return 0;
#endif	
#endif

fail_out1:
  rsi_deinit_interface(Adapter);

fail:
  if (dev)
  {
    /*Unregisters & returns the network device to the kernel*/
    rsi_unregisterdev(dev);
  }

#if RSI_CONCURRENT_MODE
  if (dev_ap)
  {
    /*Unregisters & returns the network device to the kernel*/
    rsi_unregisterdev(dev_ap);
  }
#endif
  
#ifdef RSI_USB_INTERFACE
  usb_set_intfdata (interface, NULL);
#endif

  /*Disable the interface*/

  RSI_DEBUG(RSI_ZONE_ERROR,"rsi_probe: Failure to initialize\n");
  return RSI_STATUS_FAIL;
}

#ifdef RSI_USB_INTERFACE
void usb_rx_packet_parser(struct work_struct *work)
{
#ifdef RSI_DEBUG_PRINT
  RSI_DEBUG(RSI_ZONE_SPI_DBG,"\nusb_rx_packet_parser:\n");
#endif
  PRSI_ADAPTER Adapter = container_of(work,RSI_ADAPTER,usb_rx_work);

  if(Adapter->rx_buf[USB_WLAN])
  {
#ifdef RSI_UART_INTERFACE
    rsi_send_rsp_to_userspace(Adapter, Adapter->rx_buf[USB_WLAN], 0);
#else
    rsi_send_rsp_to_userspace(Adapter, Adapter->rx_buf[USB_WLAN]);
#endif
    Adapter->rx_buf[USB_WLAN] = NULL;
    rsi_submit_rx_urb(Adapter,USB_WLAN);
  }
  if(Adapter->rx_buf[USB_BT_ZB])
   {
#ifdef RSI_UART_INTERFACE
     rsi_send_rsp_to_userspace(Adapter, Adapter->rx_buf[USB_BT_ZB], 0);
#else
     rsi_send_rsp_to_userspace(Adapter, Adapter->rx_buf[USB_BT_ZB]);
#endif
     Adapter->rx_buf[USB_BT_ZB] = NULL;
     rsi_submit_rx_urb(Adapter,USB_BT_ZB);
   }
  return;
}
#endif

uint8 deploy_pkt_to_assets(PRSI_ADAPTER Adapter, struct sk_buff *skb)
{
	uint8 status = 0;
	rsi_down_interruptible(&Adapter->rx_data_sem);
	status = rsi_send_rsp_to_userspace(Adapter, skb->data);
	if (status) {
		printk("Failed to send pkt to userspace\n");
		dev_kfree_skb(skb);
		rsi_up_sem(&Adapter->rx_data_sem);
		return status;
	}
	dev_kfree_skb(skb);
	rsi_up_sem(&Adapter->rx_data_sem);
	return status;
}

/*
 * deferred_rx_packet_parser - Bottom half handler for interrupt
 *
 * @work - packet receive work
 *
 * Receives Data and Management frame from the interrupt handler.
 */
void deferred_rx_packet_parser(struct work_struct *work)
{
	struct sk_buff *skb = NULL;
	uint8 status;
	PRSI_ADAPTER Adapter = container_of(work,RSI_ADAPTER,defer_work);

	while(rsi_queue_len(&Adapter->rx_q)) {
		skb = rsi_dequeue(&Adapter->rx_q);
		if(!skb) {
			printk("Skb is NULL\n");
			return;
		}
		status = deploy_pkt_to_assets(Adapter, skb);
		if (status) {
			printk("Failed to send pkt to userspace\n");
			return;
		}
	}
	return;
}

/*==============================================*/
/**
 * @fn          VOID rsi_interrupt_handler(struct work_struct *work)
 * @brief       Interrupt handler
 * @param[in]   struct work_struct *work, Pointer to work_struct
 *              structure
 * @param[out]  none
 * @return      none
 * @section description:
 * Upon the occurence of an interrupt, the interrupt handler will
 * be called
 */
VOID
rsi_interrupt_handler
(
  PRSI_ADAPTER Adapter
)
{
#ifdef RSI_SDIO_INTERFACE
#ifdef RSI_DEBUG_PRINT
   RSI_DEBUG(RSI_ZONE_SDIO_DBG,"\n SDIO :rsi_interrupt_handler called");
#endif
  uint32 InterruptType = 0;
  INT32 retval;
  uint8 temp;
  UINT8          InterruptStatus = 0;
  uint16  offset;
  uint32 length,status;

#ifdef DRSI_DEBUG_PRINT
  uint8 i=0;
#endif

  uint8 i=0;
 do
 {
   //rsi_up_sem(&Adapter->int_check_sem);
   retval=read_register(Adapter,SDIO_FUN1_INT_REG,0,&InterruptStatus);
    if(retval != 0)
    {
	 //abort_handler();
	//	 rsi_down_sem(&Adapter->int_check_sem);
	 schedule();
    } 
   Adapter->InterruptStatus = InterruptStatus;
    InterruptStatus &= 0xE;
    if(InterruptStatus == 0)
    {
       // rsi_down_sem(&Adapter->int_check_sem);
       return ;
    }
 

    do
     {
       InterruptType = RSI_GET_INTERRUPT_TYPE(InterruptStatus);
       switch (InterruptType)
            {
              case SDIO_BUFFER_FULL:
               {
                    Adapter->BufferFull = 1; 
                    ack_interrupt(Adapter,SD_PKT_BUFF_FULL);
            	    RSI_DEBUG(RSI_ZONE_SDIO_DBG,"BUFFER FULL \n");
            	    return;
               }
               break;

              case SDIO_BUFFER_FREE:
	       {
		       Adapter->BufferFull = 0; 
		       ack_interrupt(Adapter,SD_PKT_BUFF_EMPTY);
		       RSI_DEBUG(RSI_ZONE_SDIO_DBG,"BUFFER EMPTY \n");
		       return;
	       }
                break;

              case FIRMWARE_STATUS:
                {
                     Adapter->FSM_STATE = FSM_CARD_NOT_READY;
             	     RSI_DEBUG(RSI_ZONE_SPI_DBG,"FIRMWARE CARD NOT READY\n");
             	     return;
                }
                break;

              case SDIO_DATA_PENDING:
                {
                  if(rsi_sdio_frame_read(Adapter))
                  {
                    RSI_DEBUG(RSI_ZONE_ERROR,(TEXT(
                        "ganges_interrupt_handler: Unable to recv pkt\n")));
		    return;
                  }
                }
                break;

                default:
                {
                	ack_interrupt(Adapter, InterruptStatus);
                	RSI_DEBUG(RSI_ZONE_SPI_DBG,(TEXT(
                         "ganges_interrupt_handler: No more pending interrupts\n")));
                	return;
                }
            }
           InterruptStatus ^= BIT(InterruptType -1);
      } while (InterruptStatus);
  } while(1);
return;
#endif

  
}

#if (defined RSI_SPI_INTERFACE || defined RSI_UART_INTERFACE || defined RSI_SDIO_INTERFACE) 
/*==============================================*/
/**
 * @fn          RSI_STATUS rsi_linux_disconnect(VOID)
 * @brief       Reverse of probe
 * @param[in]   none
 * @param[out]  none
 * @return      RSI_STATUS_SUCCESS in case of successful initialization
 *              or a negative error code signifying failure
 * @section description
 * This function performs the reverse of the probe function.
 */
RSI_STATUS rsi_linux_disconnect(VOID)
{
#ifdef RSI_DEBUG_PRINT
  RSI_DEBUG(RSI_ZONE_SPI_DBG,"\nrsi_linux_disconnect:\n");
#endif
  RSI_STATUS Status = RSI_STATUS_SUCCESS;
  UINT32 ii;
  PRSI_ADAPTER Adapter   = rsi_getpriv(glbl_net_device);

  RSI_DEBUG(RSI_ZONE_INFO,"rsi_linux_disconnect: Deinitializing\n");


  Adapter->FSM_STATE  = 0;

  rsi_Delete_Event(&Adapter->PwrSaveEvent);
#ifdef RSI_UART_INTERFACE
#if RSI_TCP_IP_BYPASS
  rsi_Delete_Event(&Adapter->DATA_ACKEvent);
#endif
#endif
  
  rsi_Delete_Event(&Adapter->Event);

  RSI_DEBUG(RSI_ZONE_INFO,"Killing thread \n");
  Adapter->halt_flag = 1;

  rsi_Kill_Thread(Adapter);

  RSI_DEBUG(RSI_ZONE_INFO,"Purge queue \n");
  for (ii = 0; ii < 4; ii++)
  {
    rsi_queue_purge(&Adapter->list[ii]);
  }
  rsi_queue_purge(&Adapter->rx_q);
  /*Return the network device to the kernel*/
  RSI_DEBUG(RSI_ZONE_INFO,"Deinit interface \n");
  rsi_deinit_interface(Adapter);
  RSI_DEBUG(RSI_ZONE_INFO,"Unregister netdev \n");
  rsi_unregisterdev(Adapter->net_device0);
#if RSI_CONCURRENT_MODE
  rsi_unregisterdev(Adapter->net_device1);
#endif

  /*Disable the interface*/
  RSI_DEBUG(RSI_ZONE_INFO,"rsi_linux_disconnect: Disabling the interface\n");
  return Status;
}

#ifdef RSI_SPI_INTERFACE
int rsi_linux_handle_interrupt(unsigned int irq, void *spi_device)
{
  PRSI_ADAPTER Adapter =  spi_get_drvdata(spi_device);

  if (Adapter == NULL) {
	  return IRQ_HANDLED;
  }
  rsi_Set_Event(&Adapter->rx_thread.event);
  return IRQ_WAKE_THREAD;
}
static irqreturn_t spi_irq_thread_handler(unsigned int irq, void *spi_device)
{
  PRSI_ADAPTER Adapter =  spi_get_drvdata(spi_device);
  rsi_Wait_Event(&Adapter->spi_rx_thread_done_event, EVENT_WAIT_FOREVER);
  rsi_Reset_Event(&Adapter->spi_rx_thread_done_event);
  return IRQ_HANDLED;
}
#endif

/*==============================================*/
/**
 * @fn          RSI_STATUS rsi_init_interface(PVOID pContext)
 * @brief       Interrupt handler
 * @param[in]   pContext, Pointer to our adapter
 * @param[out]  none
 * @return      Returns RSI_STATUS_SUCCESS or RSI_STATUS_FAIL
 * @section description:
 * This function intializes the bus driver
 */
RSI_STATUS
#ifdef RSI_SPI_INTERFACE
rsi_init_interface( 
PRSI_ADAPTER Adapter, struct spi_device *spi_dev
)
#else
rsi_init_interface
(
  PVOID pContext
)
#endif
{

#ifdef RSI_DEBUG_PRINT
  RSI_DEBUG(RSI_ZONE_SPI_DBG,"\nrsi_init_interface:\n");
#endif
  INT32 ret = 0;
#ifndef RSI_SPI_INTERFACE
  PRSI_ADAPTER Adapter = (PRSI_ADAPTER)pContext;
#endif
  int irq_any_gpio;

#ifdef RSI_SPI_INTERFACE
  spi_set_drvdata(spi_dev, (void *)Adapter);
  spi_dev->max_speed_hz = 20000000;
  spi_setup(spi_dev);
  mdelay(500);
  gpio_request(SPI_INTR_GPIO_PIN, "rsi_int");
  Adapter->spi_irq_num = irq_any_gpio = gpio_to_irq(SPI_INTR_GPIO);
  gpio_direction_input(SPI_INTR_GPIO_PIN);

  ret = request_threaded_irq(irq_any_gpio,
      (irq_handler_t)rsi_linux_handle_interrupt,
      (irq_handler_t) spi_irq_thread_handler,
      IRQF_ONESHOT | IRQF_SHARED | IRQF_TRIGGER_HIGH,
      "rsi_int", (void *)spi_dev);
  if (ret)
  {
    RSI_DEBUG(RSI_ZONE_INIT,"Failed to request interrupt %d\n",ret);
    return (NULL);
  } /* End if <condition> */

  ret = rsi_spi_write_multiple(Adapter,SPI_INITIALIZATION,0,0,0);
  if (ret < 0)
  {
    RSI_DEBUG(RSI_ZONE_INIT,"Failed to write multiple%d\n",ret);
    return -1;
  }
  RSI_DEBUG(RSI_ZONE_INIT,"SPI init successful%d\n",ret);
  printk("SPI init successful\n");

  return RSI_STATUS_SUCCESS;
#endif
#ifdef RSI_SDIO_INTERFACE
        uint32 status;
        rsi_sdio_claim_host(Adapter->pfunction);
#if KERNEL_VERSION_BTWN_2_6_(18, 22)
        status = rsi_request_interrupt_handler(Adapter->pDevice, rsi_sdio_interrupt_handler, Adapter);
#elif KERNEL_VERSION_GREATER_THAN_EQUALS_2_6_(26)
        status= rsi_request_interrupt_handler(Adapter->pfunction, rsi_sdio_interrupt_handler, Adapter);
#endif
       if(status != 0)
        {
		RSI_DEBUG(RSI_ZONE_SDIO_DBG,(TEXT("%s:Failed to register interrupt\n"), __func__));
		rsi_sdio_release_host(Adapter->pfunction);
		return -1;                                                                      		
	}		
  	RSI_DEBUG(RSI_ZONE_SDIO_DBG,(TEXT("%s: Registered Interrupt handler\n"), __func__));

 	 /*Initalizing the hardware*/
         rsi_sdio_release_host(Adapter->pfunction);

  return RSI_STATUS_SUCCESS;


#endif
}
#endif

/*==============================================*/
/**
 * @fn          RSI_STATIC INT32 __init
 *              rsi_module_init(VOID)
 * @brief       module init
 * @param[in]   pContext, Pointer to our adapter
 * @param[out]  none
 * @return      0 in case of success or a negative
 *              value signifying failure
 * @section description:
 * This function is invoked when the module is loaded into the
 * kernel. It registers the client driver.
 */
RSI_STATIC INT32 __init
rsi_module_init
(
  VOID
)
{
#ifdef RSI_DEBUG_PRINT
  RSI_DEBUG(RSI_ZONE_SPI_DBG,"\nrsi_module_init:\n");
#endif
  INT32 rc;

  RSI_STATUS status = RSI_STATUS_SUCCESS;
  RSI_DEBUG(RSI_ZONE_SPI_DBG,"INIT MODULE\n");

#ifdef RSI_UART_INTERFACE
  rc = rsi_uart_probe();
  if (rc != 0)
    goto failure;
#endif
  rc = rsi_register_genl();
  if (rc != 0)
    goto failure;

#ifdef RSI_SPI_INTERFACE
  RSI_DEBUG(RSI_ZONE_SPI_DBG,"SPI REGISTER DRIVER \n");
  spi_register_driver(&rsi_driver);
#endif
#ifdef RSI_USB_INTERFACE
  RSI_DEBUG(RSI_ZONE_SPI_DBG,"USB REGISTER DRIVER \n");
  usb_register(&rsi_usb_driver);
#endif

#ifdef RSI_SDIO_INTERFACE
  RSI_DEBUG(RSI_ZONE_SDIO_DBG,"SDIO REGISTER DRIVER \n");
#if KERNEL_VERSION_BTWN_2_6_(18,22)
  status = SDIOErrorToOSError(SDIO_RegisterFunction(&onebox_driver));
#elif KERNEL_VERSION_GREATER_THAN_2_6_(26)
  status = sdio_register_driver(&rsi_driver);
#endif
  if(status)
  goto failure;
#endif
  return 0;

failure:
  RSI_DEBUG(RSI_ZONE_SPI_DBG,"an error occured while inserting the module\n");
  return -1;
}

/*==============================================*/
/**
 * @fn          RSI_STATIC VOID __exit
 *              rsi_module_exit(VOID)
 * @brief       module exit
 * @param[in]   VOID
 * @param[out]  none
 * @return      none
 * @section description:
 * At the time of removing/unloading the module, this function is
 * called. It unregisters the client driver.
 */
RSI_STATIC VOID __exit
rsi_module_exit
(
  VOID
)
{
#ifdef RSI_DEBUG_PRINT
  RSI_DEBUG(RSI_ZONE_SPI_DBG,"\nrsi_module_exit:\n");
#endif
  INT32 ret;
  RSI_DEBUG(RSI_ZONE_SPI_DBG,"EXIT MODULE\n");
#ifdef RSI_SDIO_INTERFACE
  int32 status; 
#endif
  /*Unregistering the client driver*/
#ifdef RSI_USB_INTERFACE
  if(glbl_net_device)
  {
     PRSI_ADAPTER Adapter = rsi_getpriv(glbl_net_device);
     rsi_usb_disconnect(Adapter->interface); 
  }
  RSI_DEBUG(RSI_ZONE_SPI_DBG,"get adapter\n");
#endif
#ifdef RSI_UART_INTERFACE

	PRSI_ADAPTER Adapter = rsi_getpriv(glbl_net_device);
#if RSI_TCP_IP_BYPASS
   rsi_lock_bh(&Adapter->uart_data_ack_flag_sem);
   ack_flag = 0;
   rsi_unlock_bh(&Adapter->uart_data_ack_flag_sem);
#endif
	rsi_linux_disconnect();
#endif
  ret = rsi_unregister_genl();
  if(ret != 0)
  {
    RSI_DEBUG(RSI_ZONE_SPI_DBG,"genl unregister failed %i\n",ret);
    return;
  }

#ifdef RSI_SPI_INTERFACE
  spi_unregister_driver(&rsi_driver);
#endif
#ifdef RSI_USB_INTERFACE
  usb_deregister(&rsi_usb_driver);
#endif 
#ifdef RSI_SDIO_INTERFACE
#if KERNEL_VERSION_BTWN_2_6_(18, 22)
 status = SDIO_UnregisterFunction(&rsi_driver); 
#elif KERNEL_VERSION_GREATER_THAN_2_6_(26)
 sdio_unregister_driver(&rsi_driver);
#endif
#endif

  
  //! Free the Adapter when driver is removed
  if(Adapter != NULL)
  {
    RSI_DEBUG(RSI_ZONE_SPI_DBG,"Freed Adapter successfully\n");
    kfree(Adapter);
    Adapter = NULL;
  }
  
#ifdef RSI_SDIO_INTERFACE
if(status)
 goto fail;
#endif 

  RSI_DEBUG(RSI_ZONE_SPI_DBG,"Module removed successfully\n");
  return;
  fail:
  RSI_DEBUG(RSI_ZONE_ERROR,"Error in removing the module\n"); 
  return;
}

module_init(rsi_module_init);
module_exit(rsi_module_exit);
MODULE_LICENSE("GPL");

/* $EOF */
/* Log */
