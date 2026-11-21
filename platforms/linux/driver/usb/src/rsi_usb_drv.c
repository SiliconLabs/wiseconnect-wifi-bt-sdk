/*******************************************************************************
* @file  rsi_hal_mcu_usb.c
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
 * @file     rsi_hal_mcu_usb.c
 * @version  3.3
 * @date     2013-Jan-28
 *
 *
 *
 *
 */


/**
 * Includes
 */
#include "rsi_linux.h"
#include "rsi_nic.h"
#include "rsi_linux_common.h"
#include <linux/kernel.h>
#include <linux/device.h>
#include <linux/time.h>
#include <linux/usb.h>
#include <linux/interrupt.h>
#include <linux/irq.h>
#include <linux/errno.h>
#include <linux/delay.h>
#include "rsi_global.h"

#define  USB_VENDOR_SECURE_CMD   0x19
#define  USB_OUT_REG_WRITE       0x22
#define  USB_IN_REG_READ         0x11
#define  USB_PING_WRITE          0x33 
#define  USB_PONG_WRITE          0x44
#define  USB_PING_OFFSET         0x55
#define  USB_PONG_OFFSET         0x66
#define  SLEEP_DELAY             20
/*==============================================*/
/**
 * @fn          rsi_rx_done_handler(struct urb *urb)
 * @brief       This function handles the recieved packet from USB interface
 * @param[in]   struct urb *urb, pointer to urb structure
 * @return      none
 * @section description
 * This function handles the recieved packet from USB interface
 */
void rsi_rx_done_handler(struct urb *urb)
{
  int i=0;
  char *src,*dst;
  host_desc_t *host_desc;
  UINT8 queue_no;

  wlan_rx_pkt_t *rx_pkt = (wlan_rx_pkt_t *) urb->transfer_buffer;
  host_desc = (unsigned long)rx_pkt + rsi_bytes2R_to_uint16(&rx_pkt->host_desc_offset);

  char *rx_buf = (char *)host_desc;
  PRSI_ADAPTER Adapter = urb->context;

   if (urb->status) 
  {
    printk("rx_done, status %d, length %d", urb->status, urb->actual_length);
    return;
  }
  if (urb->actual_length == 0)
  {
    printk ("==> ERROR: ZERO LENGTH <==\n");
  }

  queue_no  = rx_buf[1] >> 4;

  if(queue_no == WLAN_MGMT_TYPE || queue_no == WLAN_DATA_TYPE)
  {
    Adapter->rx_buf[0] = rx_buf;
  }
  else
  {
    Adapter->rx_buf[1] = rx_buf;
  }

  queue_work(Adapter->workqueue, &Adapter->usb_rx_work);

  if(!Adapter)
    printk("adapter fail");
}

RSI_STATUS rsi_submit_rx_urb(PRSI_ADAPTER Adapter,int endpoint)
{
  //printk("RX buffer submitted to usb core\n");
  usb_fill_bulk_urb (Adapter->rx_usb_urb[endpoint],
      Adapter->usbdev,
      usb_rcvbulkpipe (Adapter->usbdev, Adapter->rx_endpoint_address[endpoint]),
      Adapter->rx_usb_urb[endpoint]->transfer_buffer, Adapter->max_rx_len[endpoint], rsi_rx_done_handler,
      Adapter);
  if (usb_submit_urb(Adapter->rx_usb_urb[endpoint], GFP_KERNEL)) 
  {
    //printk ("submit(bulk rx_urb)");
  }
}


extern struct net_device *glbl_net_device;
 /*====================================================*/
/**
  * @fn          int16 rsi_execute_cmd(uint8 *descparam,uint8 *payloadparam,uint16 size_param)
  * @brief       Common function for all the commands.
  * @param[in]   uint8 *descparam, pointer to the frame descriptor parameter structure
  * @param[in]   uint8 *payloadparam, pointer to the command payload parameter structure
  * @param[in]   uint16 size_param, size of the payload for the command
  * @return      errCode
  *              0  = SUCCESS
  * @section description
  * This is a common function used to process a command to the Wi-Fi module.
  */
INT16 rsi_execute_cmd(UINT8 *desc, UINT8 *payload, UINT16 payload_len)
{
  UINT16 status;
  UINT8 *data_ptr;
  UINT8 transfer[1800];
  UINT32 actual_len;
  UINT8  queue_no = desc[1] >> 4;
  UINT32 head_room;
  UINT8  ep_index =0;

  PRSI_ADAPTER Adapter =  rsi_getpriv(glbl_net_device);
  if(queue_no == ZB_MGMT_TYPE || queue_no == ZB_DATA_TYPE) 
  {
    head_room = USB_TX_HEAD_ROOM_ZB;
#ifdef RS9116
		ep_index = 2;

#else
		ep_index = 1;
#endif
  }
  else if(queue_no == BT_MGMT_TYPE || queue_no == BT_DATA_TYPE)
  {
    head_room = USB_TX_HEAD_ROOM_BT;
    ep_index = 1;
  }
  else
  {
    head_room = USB_TX_HEAD_ROOM_WLAN;    
  }
  data_ptr =(UINT8 *)(((unsigned long)transfer) + head_room);
  memcpy(data_ptr,desc,16);
  if (payload_len)
  {
    memcpy(data_ptr+16,payload, payload_len);
  }
  usb_bulk_msg (Adapter->usbdev,
      usb_sndbulkpipe (Adapter->usbdev,
        Adapter->tx_endpoint_address[ep_index]),
      (void *) transfer, payload_len + head_room + 16, &actual_len, 0);

  return 0;
}



#if RSI_SECURE_BOOT

/**===========================================================================
 * @fn          RSI_USB_STATUS rsi_secure_reg_rd(uint16 len, uint16 *value)
 * @brief       Reads from command in register
 * @param[in]   uint16       len:          Number of bytes to read. (def: 2 since we have 16 bit regs)
 * @param[out]  uint16*      value:        Variable in which the read value is stored
 * @return      RSI_USB_STATUS:            Indicates the success/failure of operation
 *              0   = SUCCESS
 *              < 0 = Failure
 * @section Description
 * Reads a command in register for bootloader handshake
 *
 */

RSI_USB_STATUS rsi_secure_reg_rd(uint16 len, uint8 *value)
{
  PRSI_ADAPTER Adapter = rsi_getpriv(glbl_net_device);
  /* Temporary Buffer */
  UINT8            temp_buf[10];

  /* Read operation - Status variable */
  RSI_USB_STATUS   status = RSI_USB_STATUS_SUCCESS;

    /*Added the delay */
    udelay(SLEEP_DELAY);
  /* Call kernel API to send 'READ REQUEST' */
  status = usb_control_msg (Adapter->usbdev,
      usb_rcvctrlpipe (Adapter->usbdev, 0),
      USB_VENDOR_SECURE_CMD,
      USB_TYPE_VENDOR | USB_DIR_IN | USB_RECIP_DEVICE,
      USB_IN_REG_READ,
      0,
      (void *) temp_buf, 2, 0);
  *(UINT16 *)value = *(UINT16 *)temp_buf;

  /* Check status returned for success/failure */
  if (status < 0) {
    RSI_DEBUG (RSI_ZONE_INFO, "REG READ FAILED WITH ERROR CODE :%#10lx \n", status);
    return;
  }

  /* TORM: Debug print */
  RSI_DEBUG (RSI_ZONE_INFO, "USB REG READ VALUE :%#10lx \n", *value);

  /* Return success/failure status */
  return status;
}

/**===========================================================================
 * @fn          RSI_USB_STATUS rsi_secure_reg_wr(uint16* value, uint16 len)
 * @brief       Writes the given command in bootloader out register
 * @param[in]   uint16         len:          Number of bytes to read. (def: 2 since we have 16 bit regs)
 * @param[out]  uint16*        value:        Variable in which the read value is stored
 * @return      RSI_USB_STATUS:              Returns Number of bytes written
 *              < 0  = Failure 
 *              > 0  = Number of bytes written
 * @section Description
 * Writes the given data to the specified register address.
 *
 */

RSI_USB_STATUS rsi_secure_reg_wr(uint8 *value,uint16 len)
{
  PRSI_ADAPTER Adapter = rsi_getpriv(glbl_net_device);
  /* Temporary Buffer to store the value in the device expected format */
  uint8            usb_reg_buf[4];

  /* Write Operation Status Variable */
  RSI_USB_STATUS   status = RSI_USB_STATUS_SUCCESS;

  /* Store the lower byte of the 16 bit value in the first byte of temp buffer */
  //*(uint32 *)usb_reg_buf = *(uint32 *)value;
  if(len <= 2)
  {
    memcpy(usb_reg_buf, value, len);
  }
  else
  {
    return -1;
  }


  /* TORM: Debug Print */
  RSI_DEBUG (RSI_ZONE_INFO, "USB REG WRITE VALUE :%#10lx \n", *(UINT32 *)usb_reg_buf);

    /*Added the delay */
    udelay(SLEEP_DELAY);
  /* Call the kernel API for performing the write operation */
  status = usb_control_msg (Adapter->usbdev,
      usb_sndctrlpipe (Adapter->usbdev, 0),
      USB_VENDOR_SECURE_CMD,
      USB_TYPE_VENDOR,
      USB_OUT_REG_WRITE,
      0,
      (void *) usb_reg_buf, len, 0);

  /* Check status variable for success/failure */
  if (status < 0) {
    RSI_DEBUG (RSI_ZONE_INFO, "REG WRITE FAILED WITH ERROR CODE :%#10d \n", status);
  }

  /* Return success/failure status */
  return status;
}

/**===========================================================================
 * @fn          RSI_USB_STATUS rsi_secure_ping_pong_wr( uint32 ping_pong, uint16* value, uint16 len)
 * @brief       Writes the given data to the specified register address in the WiFi Module
 * @param[in]   uint32         ping_pong:    ping or pong buffer write
 * @param[in]   uint16         len:          Number of bytes to read. (def: 2 since we have 16 bit regs)
 * @param[out]  uint8*         value:        pointer contain the buffer of content to be written
 * @return      RSI_USB_STATUS:              Indicates the success/failure of operation
 *              < 0  = Failure 
 *              > 0  = Number of bytes written
 *
 * @section Description
 * Writes the given data in ping or pong buffer
 *
 */

RSI_USB_STATUS rsi_secure_ping_pong_wr(uint32 ping_pong, uint8 *src_addr, uint16 len)
{
  /* Read operation - Status variable */
  RSI_USB_STATUS status = RSI_USB_STATUS_SUCCESS;
  PRSI_ADAPTER Adapter = rsi_getpriv(glbl_net_device);

  /* Variable which stores the actual length of data transfered */
  UINT32 i,j;
  UINT16 buffer_type;

  if(ping_pong)
  {
    buffer_type = USB_PONG_WRITE;
  }
  else
  {
    buffer_type = USB_PING_WRITE;
  }

    /*Added the delay */
    udelay(SLEEP_DELAY);
  //FIXME: check the need of local buffer
    /* Send the data via control_msg */
    status = usb_control_msg(Adapter->usbdev,
        usb_sndctrlpipe (Adapter->usbdev, 0),
        USB_VENDOR_SECURE_CMD, USB_TYPE_VENDOR,
        buffer_type,0,
        (void *) src_addr, len, 0);

    /* If reading is not successful, print error, free memory and return */
    if (status < 0) {
      RSI_DEBUG (RSI_ZONE_INFO, "rsi_reg_wr_multi: Failed with Error Code: %#10d \n", status);
      //kfree(tmp_buffer);
      return status;
    }
  /* TORM: Debug Print */
  RSI_DEBUG (RSI_ZONE_INFO, "rsi_reg_wr_multi: was successful:%#10d \n", status);
  /* Return the operation status */
  return status;
}

/**===========================================================================
 * @fn          RSI_USB_STATUS rsi_secure_ping_pong_wr_offset( uint32 ping_pong, uint16* value, uint16 len)
 * @brief       Writes the given data to the specified register address in the WiFi Module
 * @param[in]   uint32         ping_pong:    ping or pong buffer write
 * @param[in]   uint16         len:          Number of bytes to read. (def: 2 since we have 16 bit regs)
 * @param[out]  uint8*         value:        pointer contain the buffer of content to be written
 * @return      RSI_USB_STATUS:              Indicates the success/failure of operation
 *              < 0  = Failure 
 *              > 0  = Number of bytes written
 *
 * @section Description
 * Writes the given data in ping or pong buffer
 *
 */

RSI_USB_STATUS rsi_secure_ping_pong_wr_offset(uint32 ping_pong, uint8 *src_addr, uint16 len)
{
  /* Read operation - Status variable */
  RSI_USB_STATUS status = RSI_USB_STATUS_SUCCESS;
  PRSI_ADAPTER Adapter = rsi_getpriv(glbl_net_device);

  /* Variable which stores the actual length of data transfered */
  UINT16 offset;
  UINT16 buffer_type;

  if(ping_pong)
  {
    buffer_type = USB_PONG_OFFSET;
  }
  else
  {
    buffer_type = USB_PING_OFFSET;
  }
  for(offset=0; offset<len;offset+=2)
  {
    /*Added the delay */
    udelay(SLEEP_DELAY);
  //FIXME: check the need of local buffer
    /* Send the data via control_msg */
    status = usb_control_msg(Adapter->usbdev,
        usb_sndctrlpipe (Adapter->usbdev, 0),
        USB_VENDOR_SECURE_CMD, USB_TYPE_VENDOR,
        buffer_type,offset,
        (void *) (src_addr+offset), 2, 0);

    /* If reading is not successful, print error, free memory and return */
    if (status < 0) {
      RSI_DEBUG (RSI_ZONE_INFO, "rsi_secure_ping_pong_wr_offset: Failed with Error Code: %#10d \n", status);
      //kfree(tmp_buffer);
      return status;
    }
  }
  /* TORM: Debug Print */
  RSI_DEBUG (RSI_ZONE_INFO, "rsi_secure_ping_pong_wr_offset: was successful:%#10d \n", status);

  /* Return the operation status */
  return status;
}

#endif




/**===========================================================================
 * @fn          RSI_USB_STATUS rsi_mem_rd(uint32 reg_address, uint16 len, uint16 *value)
 * @brief       Reads a register specified by reg_address & stores in value
 * @param[in]   uint32       reg_address:  Address of the register to read
 * @param[in]   uint16       len:          Number of bytes to read. (def: 2 since we have 16 bit regs)
 * @param[out]  uint16*      value:        Variable in which the read value is stored
 * @return      RSI_USB_STATUS:            Indicates the success/failure of operation
 *              0   = SUCCESS
 *              < 0 = Failure
 * @section Description
 * Reads a register specified specified by the address into the given variable.
 *
 */

RSI_USB_STATUS rsi_mem_rd(uint32 reg_address, uint16 len, uint8 *value)
{
  PRSI_ADAPTER Adapter = rsi_getpriv(glbl_net_device);
  /* Temporary Buffer */
  UINT8            temp_buf[10];

  /* Read operation - Status variable */
  RSI_USB_STATUS   status = RSI_USB_STATUS_SUCCESS;
  if (len == 2){
    /*Added the delay */
    udelay(SLEEP_DELAY);
    /* Call kernel API to send 'READ REQUEST' */
    status = usb_control_msg (Adapter->usbdev,
        usb_rcvctrlpipe (Adapter->usbdev, 0),
        USB_VENDOR_REGISTER_READ,
	      USB_TYPE_VENDOR | USB_DIR_IN | USB_RECIP_DEVICE,
        ((reg_address & 0xffff0000) >> 16),
        (reg_address & 0xffff),
        (void *) temp_buf, 2, 0);
    *(UINT32 *)value = *(UINT16 *)temp_buf;
  } else {
    /*Added the delay */
    udelay(SLEEP_DELAY);
    /* Call kernel API to send 'READ REQUEST' */
    status = usb_control_msg (Adapter->usbdev,
        usb_rcvctrlpipe (Adapter->usbdev, 0),
        USB_VENDOR_REGISTER_READ,
	      USB_TYPE_VENDOR | USB_DIR_IN | USB_RECIP_DEVICE,
        ((reg_address & 0xffff0000) >> 16),
        (reg_address & 0xffff),
        (void *) temp_buf, 2, 0);
    reg_address += 2;
    /*Added the delay */
    udelay(SLEEP_DELAY);
    /* Call kernel API to send 'READ REQUEST' */
    status = usb_control_msg (Adapter->usbdev,
        usb_rcvctrlpipe (Adapter->usbdev, 0),
        USB_VENDOR_REGISTER_READ,
	      USB_TYPE_VENDOR | USB_DIR_IN | USB_RECIP_DEVICE,
        ((reg_address & 0xffff0000) >> 16),
        (reg_address & 0xffff),
        (void *)((UINT32)temp_buf + 2), 2, 0);
    *(UINT32 *)value = *(UINT32 *)temp_buf;
  }
  /* Convert the first two bytes (uint8) of the buffer to a 'uint16' */
  //*value = *(uint32*)temp_buf;

  /* Check status returned for success/failure */
  if (status < 0) {
    RSI_DEBUG (RSI_ZONE_INFO, "REG READ FAILED WITH ERROR CODE :%#10lx \n", status);
    return;
  }

  /* TORM: Debug print */
  RSI_DEBUG (RSI_ZONE_INFO, "USB REG READ VALUE :%#10lx \n", *value);

  /* Return success/failure status */
  return status;
}

/**===========================================================================
 * @fn          RSI_USB_STATUS rsi_reg_rd(PRSI_ADAPTER Adapter, uint32 reg_address, uint16* value, uint16 len)
 * @brief       Reads data from the specified reg_address & stores in value
 * @param[in]   PRSI_ADAPTER Adapter:      Pointer to the Adapter structure
 * @param[in]   uint32       reg_address:  Address of the register to read
 * @param[in]   uint16       len:          Number of bytes to read. (def: 2 since we have 16 bit regs)
 * @param[out]  uint16*      value:        Variable in which the read value is stored
 * @return      RSI_USB_STATUS:            Indicates the success/failure of operation
 *                           =  0:         RSI_USB_STATUS_SUCCESS
 *                           < 0 :         RSI_USB_STATUS_FAILURE
 *
 * @section Description
 * Reads data from the specified reg_address & stores in value based on the length
 *
 */

RSI_USB_STATUS rsi_reg_rd_multi(PRSI_ADAPTER  Adapter, uint32 reg_address, uint8 *dest_addr, uint16 len)
{
  /* Read operation - Status variable */
  RSI_USB_STATUS status = RSI_USB_STATUS_SUCCESS;

  /* Variable which stores the actual length of data transfered */
  UINT8 transfer;

  /* Temporary Buffer to read 4K chunks of packets */
  UINT8 *tmp_buffer;

  /* Allocate memory for the temporary buffer */
  tmp_buffer = kzalloc(4096, GFP_KERNEL);

  /* If no memory was allocated, return NoMem error */
  if (!tmp_buffer)
    return -ENOMEM;

  /* While there's data left to be read */
  while (len) {

    /* Select 4K or lesser chunk of data */
    transfer = min_t(int, len, 4096);
    /*Added the delay */
    udelay(SLEEP_DELAY);
    /* Read the data via control msg */
    status = usb_control_msg(Adapter->usbdev,
        usb_rcvctrlpipe (Adapter->usbdev, 0),
        USB_VENDOR_REGISTER_READ,
	                            USB_TYPE_VENDOR | USB_DIR_IN | USB_RECIP_DEVICE,
        ((reg_address & 0xffff0000) >> 16), (reg_address & 0xffff),
        (void *) tmp_buffer, transfer, 0);

    /* Copy new data from the temporary buffer into the destination addr */
    memcpy(dest_addr, tmp_buffer, transfer);

    /* If reading is not successful, print error, free memory and return */
    if (status < 0) {
      RSI_DEBUG (RSI_ZONE_INFO, "rsi_reg_rd_multi: Failed with Error Code: %#10d \n", status);
      kfree(tmp_buffer);
      return status;
    }

    /* Otherwise, adjust offsets to read the next 4K / leftover chunk of data */
    else {
      len -= transfer;
      dest_addr += transfer;
      reg_address += transfer;
    }
  }

  /* TORM: Debug print */
  RSI_DEBUG (RSI_ZONE_INFO, "rsi_reg_rd_multi: was successful:%#10d \n", status);

  /* Free the memory */
  kfree(tmp_buffer);

  /* Return the operation status */
  return status;
}

/**===========================================================================
 * @fn          RSI_USB_STATUS rsi_reg_wr(uint32 reg_address, uint16* value, uint16 len)
 * @brief       Writes the given data to the specified register address in the WiFi Module
 * @param[in]   uint32         reg_address:  Address of the register to read
 * @param[in]   uint16         len:          Number of bytes to read. (def: 2 since we have 16 bit regs)
 * @param[out]  uint16*        value:        Variable in which the read value is stored
 * @return      RSI_USB_STATUS:              Returns Number of bytes written
 *              < 0  = Failure 
 *              > 0  = Number of bytes written
 * @section Description
 * Writes the given data to the specified register address.
 *
 */

RSI_USB_STATUS rsi_mem_wr(unsigned long reg_address, uint16 len, uint8 *value)
{
  PRSI_ADAPTER Adapter = rsi_getpriv(glbl_net_device);
  /* Temporary Buffer to store the value in the device expected format */
  uint8  *usb_reg_buf = NULL;
  uint16 reg_value, index;

  /* Write Operation Status Variable */
  RSI_USB_STATUS   status = RSI_USB_STATUS_SUCCESS;

  /* Store the lower byte of the 16 bit value in the first byte of temp buffer */
  //*(uint32 *)usb_reg_buf = *(uint32 *)value;
  usb_reg_buf = kmalloc(4, GFP_KERNEL);
  if (!usb_reg_buf)
    return -ENOMEM;

  if(len <= 4)
  {
    memcpy(usb_reg_buf, value, len);
  }
  else
  {
    kfree(usb_reg_buf);
    return -1;
  }

 reg_value = ((u16 *)&reg_address)[1] & 0xffff;
 index     = ((u16 *)&reg_address)[0] & 0xffff;

  /* TORM: Debug Print */
  RSI_DEBUG (RSI_ZONE_INFO, "USB REG WRITE VALUE :%#10lx \n",  *(uint32 *)usb_reg_buf);
    /*Added the delay */
    udelay(SLEEP_DELAY);
  /* Call the kernel API for performing the write operation */
  status = usb_control_msg (Adapter->usbdev,
      usb_sndctrlpipe (Adapter->usbdev, 0),
      USB_VENDOR_REGISTER_WRITE,
      USB_TYPE_VENDOR,
      reg_value,
      index,
      (void *)usb_reg_buf, len, 0);

  /* Check status variable for success/failure */
  if (status < 0) {
    RSI_DEBUG (RSI_ZONE_INFO, "REG WRITE FAILED WITH ERROR CODE :%#10d \n", status);
  }
  kfree(usb_reg_buf);
  /* Return success/failure status */
  return status;
}

/**===========================================================================
 * @fn          RSI_USB_STATUS rsi_reg_wr_multi(PRSI_ADAPTER Adapter, uint32 reg_address, uint16* value, uint16 len)
 * @brief       Writes the given data to the specified register address in the WiFi Module
 * @param[in]   PRSI_ADAPTER   Adapter:      Pointer to the Adapter structure
 * @param[in]   uint32         reg_address:  Address of the register to write
 * @param[in]   uint16         len:          Number of bytes to read. (def: 2 since we have 16 bit regs)
 * @param[out]  uint16*        value:        Variable in which the read value is stored
 * @return      RSI_USB_STATUS:              Indicates the success/failure of operation
 *              < 0  = Failure 
 *              > 0  = Number of bytes written
 *
 * @section Description
 * Writes the given data to the specified register address.
 *
 */

RSI_USB_STATUS rsi_reg_wr_multi (PRSI_ADAPTER Adapter, uint32 reg_address, uint8 *src_addr, uint16 len)
{
  /* Read operation - Status variable */
  RSI_USB_STATUS status = RSI_USB_STATUS_SUCCESS;

  /* Variable which stores the actual length of data transfered */
  UINT8 transfer;
  UINT32 i,j;
  /* Temporary Buffer to send 4K chunks of packets */
  UINT8 *tmp_buffer;

  /* Allocate memory for the temporary buffer */
  tmp_buffer = kzalloc(4096, GFP_KERNEL);

  /* If no memory was allocated, return NoMem error */
  if (!tmp_buffer)
    return -ENOMEM;

  /* While there's data left to be read */
  while (len) {

    /* Select 4K or remaining chunk of data */
    transfer = min_t(int, len, 128);

    /* Copy data to be transmitted into the temporary buffer */
    memcpy(tmp_buffer, src_addr, transfer);

    /*Added the delay */
    udelay(SLEEP_DELAY);
    /* Send the data via control_msg */
    status = usb_control_msg(Adapter->usbdev,
        usb_sndctrlpipe (Adapter->usbdev, 0),
        USB_VENDOR_REGISTER_WRITE, USB_TYPE_VENDOR,
        ((reg_address & 0xffff0000) >> 16), (reg_address & 0xffff),
        (void *) tmp_buffer, transfer, 0);

    /* If reading is not successful, print error, free memory and return */
    if (status < 0) {
      RSI_DEBUG (RSI_ZONE_INFO, "rsi_reg_wr_multi: Failed with Error Code: %#10d \n", status);
      kfree(tmp_buffer);
      return status;
    }

    /* Otherwise, adjust offsets to send the next 4K / leftover chunk of data */
    else {
      len -= transfer;
      src_addr += transfer;
      reg_address += transfer;
    }
  }

  /* TORM: Debug Print */
  RSI_DEBUG (RSI_ZONE_INFO, "rsi_reg_wr_multi: was successful:%#10d \n", status);

  /* Free the memory allocated for temp buffer */
  kfree(tmp_buffer);

  /* Return the operation status */
  return status;
}

