/*******************************************************************************
* @file  rsi_91x_spi.c
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
 * @file rsi_91x_spi.c
 * @author 
 * @version 1.0
 *
 *
 * @section DESCRIPTION
 *
 * The file contains Generic HAL changes for SPI.
 */

#include <linux/module.h>
#include <linux/of.h>
#include <linux/of_device.h>
#include <linux/spi/spi.h>
#include <linux/spi/spidev.h>
#include <linux/uaccess.h>
#include <linux/gpio.h>

#include "rsi_linux.h"
//#include "rsi_spi.h"
extern struct net_device *glbl_net_device;
bool already_probed;
#define align_address(a) ((unsigned long)(a) & ~0x7)

/**
 * rsi_spi_ack_intr() - This function acks the interrupt received.
 * @adapter: Pointer to the adapter structure.
 * @int_bit: Interrupt bit to write into register.
 *
 * Return: None.
 */
void rsi_spi_ack_intr(PRSI_ADAPTER adapter, u8 int_bit)
{
	int status;

	status = rsi_spi_master_reg_write(adapter,
					 RSI_SPI_ACK_INTR_REG,
					 int_bit, 4);
	if (status)
		RSI_DEBUG(RSI_ZONE_ERROR, "%s: Unable to send ack\n", __func__);
}



static uint8 process_spi_lmac_aggr_pkt(PRSI_ADAPTER adapter, uint32 pkt_len)
{
  uint8 *frame_desc_addr = NULL;
  uint16 actual_length = 0;
  uint16 offset        = 0;
  uint32 payload_len = 0;
  uint32 index;
  uint32 length        = 0;
  struct sk_buff *skb = NULL;
  index = 0;

  do {
		  frame_desc_addr = &adapter->DataRcvPacket[index];

		  actual_length = cpu_to_le16(*(uint16 *)&frame_desc_addr[0]);
		  offset        = cpu_to_le16(*(uint16 *)&frame_desc_addr[2]);
		  length = (cpu_to_le16(*(uint16 *)&frame_desc_addr[offset]) & 0x0fff);
		  payload_len = length + 16;
		  skb = dev_alloc_skb(payload_len);
		  skb_put(skb, payload_len);
		  memcpy(skb->data, frame_desc_addr + offset, skb->len);
		  rsi_queue_tail(&adapter->rx_q, skb);
		  index += actual_length;
		  pkt_len -= actual_length;
  } while ( pkt_len > 0);
  rsi_Set_Event(&adapter->rx_thread.event);
  return 0;
}

#define FLASH_SIZE_ADDR               0x04000016

int16 rsi_mem_wr(unsigned long reg_address,uint16 len,uint32 *value)
{
  PRSI_ADAPTER Adapter =  rsi_getpriv(glbl_net_device);
  if(rsi_spi_read_write_reg_multiple(Adapter,AHB_MASTER_WRITE,
        reg_address,
        (uint8 *)value,
	len)!= RSI_STATUS_SUCCESS)
  {
    RSI_DEBUG(RSI_ZONE_ERROR,
        (TEXT("%s: Unable to do AHB reg write\n"), __func__));
    return -1;
  }
  return RSI_STATUS_SUCCESS;
}/* End <rsi_mem_wr> */


int16 rsi_mem_rd(uint32 reg_address,uint16 len,uint32 *value)
{
  uint32 *data = NULL;
  uint16 ms_addr = 0;
  uint32 align[2] = {};
  uint32 addr_on_bus;
  PRSI_ADAPTER Adapter =  rsi_getpriv(glbl_net_device);
  data = (uint32 *)align_address(&align[1]);

  addr_on_bus = (reg_address & 0xFF000000);
  if ((addr_on_bus == (FLASH_SIZE_ADDR & 0xFF000000)) || /* This is for flash access*/
      (addr_on_bus == 0x0)) {/* This check is for Internal RAM access */
    addr_on_bus = (reg_address & ~(0x3));
  }
  else {
    /* This is for accessing peripherals on APB(AMBA- Peripheral Bus) bus of the device */
    addr_on_bus = reg_address;
  }
  /* Bringing TA out of reset */
  if(rsi_spi_read_write_reg_multiple( Adapter,AHB_MASTER_READ,addr_on_bus, (uint8 *)data ,4)!= RSI_STATUS_SUCCESS)
  {
    RSI_DEBUG(RSI_ZONE_ERROR,
        (TEXT("%s: AHB register read failed\n"), __func__));
    return -1;

  }
  if(len == 2) {
    if((reg_address & 0x3) == 0) {
      *value = *data;
    }
    else {
      *value  = ((*data >> 16));
    }
    *value = (*value & 0xFFFF);
  }
  else if(len == 1) {
    if((reg_address & 0x3) == 0) {
      *value = *data;
    } else if((reg_address & 0x3) == 1) {
      *value = (*data >> 8);
    } else if((reg_address & 0x3) == 2) {
      *value = (*data >> 16);
    } else {
      *value = (*data >> 24);
    }
    *value = (*value & 0xFF);
  }
  else { /*len is 4 */
    *value = *data;
  }

  return RSI_STATUS_SUCCESS;
}/* End <rsi_mem_rd> */




#define READ_LEN_FROM_CARD_REG 0x20

/*===========================================================================
 * @fn        int32 rsi_spi_frame_read(PRSI_ADAPTER Adapter)
 * @brief     This function read frames from the SD card.
 * @param     Pointer to driver adapter structure.
 * @return    0 if success else -1.
 */
int32 rsi_spi_frame_read(PRSI_ADAPTER Adapter)
{
	int32 status;
	uint32 q_no;
	uint32 pktLen;
	uint16 type;
	u16 regval  = 0;
	uint8 num_blocks;
	uint32 rcv_pkt_len;
	uint8 *rx_data = NULL;

	status = rsi_spi_read_write_reg_multiple(Adapter, SPI_INTERNAL_READ, 0x20, (u8 *)&regval, 2);
	if(status != 0)
	{
		RSI_DEBUG(RSI_ZONE_SDIO_DBG,"\nReading number of blocks failed");
		return -1;
	}
	rcv_pkt_len = regval & 0xFFF;
	rx_data = Adapter->DataRcvPacket;
	status = rsi_spi_host_intf_read_pkt(Adapter,rx_data, rcv_pkt_len);
	if(status != 0 )
	{
		RSI_DEBUG(RSI_ZONE_SDIO_DBG,"\n EEROR in reading packet");
		return -1;

	}
	process_spi_lmac_aggr_pkt(Adapter, rcv_pkt_len);
	return 0;
}/* End <rsi_sdio_frame_read> */



/**
 * rsi_spi_interrupt_handler() - Identifies the type of interrupt and serves
 *				accordingly.
 */
void rsi_spi_interrupt_handler( PRSI_ADAPTER Adapter)
{
	int status;
	enum spi_interrupt_type isr_type;
	u16 isr_status = 0;

	do {
		mutex_lock(&Adapter->rx_lock);
		status = rsi_spi_read_write_reg_multiple(Adapter, SPI_INTERNAL_READ, 0, (u8 *)&isr_status, 2);
		if (status) {
			RSI_DEBUG(RSI_ZONE_ERROR,
					"%s: Failed to Read intr Status Reg\n",
					__func__);
			mutex_unlock(&Adapter->rx_lock);
			return;
		}

		Adapter->interrupt_status = isr_status;
		isr_status &= 0xE;
		if (isr_status == 0) {
			rsi_Set_Event(&Adapter->Event);
			mutex_unlock(&Adapter->rx_lock);
			return;
		}

		do {
			RSI_GET_SPI_INTERRUPT_TYPE(isr_status, isr_type);

			switch (isr_type) {
				case FIRMWARE_ASSERT_IND:
					Adapter->FSM_STATE = FSM_CARD_NOT_READY;
					RSI_DEBUG(RSI_ZONE_ERROR,
							"%s: ==> FIRMWARE Assert <==\n",
							__func__);
					//	dump_stack();
					break;
				case MSDU_PACKET_PENDING:
					RSI_DEBUG(RSI_ZONE_ISR, "Pkt pending interrupt\n");
					status = rsi_spi_frame_read(Adapter);
					if (status) {
						RSI_DEBUG(RSI_ZONE_ERROR,
								"%s: Failed to read pkt\n",
								__func__);
						mutex_unlock(&Adapter->rx_lock);
						return;
					}
					break;
				default:
					rsi_spi_ack_intr(Adapter, isr_status);
					isr_status = 0;
					RSI_DEBUG(RSI_ZONE_ISR,
							"Unknown Interrupt %x\n",
							isr_status);
					break;
			}
			isr_status ^= BIT(isr_type - 1);
		} while (isr_status);
		mutex_unlock(&Adapter->rx_lock);
	} while (1);
}


/*===========================================================================
 * @fn          INT16 rsi_execute_cmd(UINT8 *descparam, UINT8 *payload, UINT16 payload_len)
 * @brief       Common function for all the commands.
 * @param       uint8 *descparam, pointer to the frame descriptor parameter structure
 * @param       uint8 *payload, pointer to the command payload parameter structure
 * @param        uint16 payload_len, size of the payload for the command
 * @return      errCode
 *              0  = SUCCESS
 */
INT16 rsi_execute_cmd(UINT8 *descparam, UINT8 *payload, UINT16 payload_len)
{
	UINT16 status;
	UINT8 *data_ptr =NULL;
#ifdef CHIP_917
	UINT8 *data_ptr_act =NULL;
	uint32 i ;
#endif
	UINT8 transfer[2000];
	uint16 num_blocks;
	PRSI_ADAPTER Adapter =  rsi_getpriv(glbl_net_device);

#ifndef CHIP_917
	data_ptr =(UINT8 *)(((UINT32)transfer) + 236);
#else
	data_ptr_act = &transfer[0];
	rsi_memcpy(data_ptr_act,descparam,4);
	data_ptr = &transfer[216];
#endif
	rsi_memcpy(data_ptr,descparam,16);
	//rsi_memcpy(data_ptr,descparam,payload_len);
	if (payload_len)
	{
		memcpy((data_ptr+16),payload, payload_len);
	}
#ifndef CHIP_917
	num_blocks = payload_len + 16 ;//FIXME descriptor length
#else
	num_blocks = payload_len + 216 ;//FIXME descriptor length
#endif

#ifndef CHIP_917
	status= rsi_spi_host_intf_write_pkt(Adapter,data_ptr,payload_len+16);//FIXME queue no
#else
	status= rsi_spi_host_intf_write_pkt(Adapter,data_ptr_act ,payload_len+216+16);//FIXME queue no
#endif
	if(status != 0)
	{
		status = -1;
	}

	return status;
}/* End <rsi_execute_cmd> */


/**===========================================================================
 * @fn        int16 spi_read_buffer_full(PRSI_ADAPTER Adapter)
 * @brief     This function checks the buffer full conditions.
 * @param     None.
 * @return    0 if success else -1.
 */
int16 spi_read_buffer_full(PRSI_ADAPTER Adapter)
{
   uint32 READ_BUFFER_ADDRESS = 0;//FIXME
   uint8 temp;
   rsi_spi_read_register(Adapter,0x22,&temp);
   if(temp & BUFFER_FULL)
   {
     Adapter->BufferFull = true;
     return -1;
   }
   Adapter->BufferFull = false;
   return 0;
 }/* End <sdio_read_buffer_full> */


/**
 * rsi_spi_rx_thread() - Thread to collect receive packets. It is in waiting
 *			state until any RX packet arrives.
 */

void rsi_spi_rx_thread(PRSI_ADAPTER Adapter)
{
	struct sk_buff *skb;
	int status;
	bool done = false;

	do {
		status = rsi_Wait_Event(&Adapter->rx_thread.event,
				EVENT_WAIT_FOREVER);
		if (status < 0)
			break;

		if (atomic_read(&Adapter->rx_thread.thread_done))
			break;
		rsi_Reset_Event(&Adapter->rx_thread.event);
		rsi_spi_interrupt_handler(Adapter);


		while(rsi_queue_len(&Adapter->rx_q)) {
			skb = rsi_dequeue(&Adapter->rx_q);
			if(!skb) {
				printk("Skb is NULL\n");
				break;
			}
			status = deploy_pkt_to_assets(Adapter, skb);
			if (status) {
				printk("Failed to send pkt to userspace\n");
				return;
			}
			if (atomic_read(&Adapter->rx_thread.thread_done)) {
				done = true;
				break;
			}
		}
		rsi_Set_Event(&Adapter->spi_rx_thread_done_event);
		if (done)
			break;
	} while (1);

	RSI_DEBUG(RSI_ZONE_INFO, "%s: Terminated SPI RX thread\n", __func__);
	skb_queue_purge(&Adapter->rx_q);
	atomic_inc(&Adapter->rx_thread.thread_done);
	complete_and_exit(&Adapter->rx_thread.completion, 0);
}


static void rsi_spi_gspi_init(PRSI_ADAPTER adapter)
{
	unsigned long gspi_ctrl_reg0_val;

	/*
	 * Programming gspi frequency = soc_frequency / 2.
	 * Warning : ULP seemed to be not working
	 * well at high frequencies. Modify accordingly.
	 */
	gspi_ctrl_reg0_val = 0x4;
	/* csb_setup_time [5:4] */
	gspi_ctrl_reg0_val |= 0x10;
	/* csb_hold_time [7:6] */
	gspi_ctrl_reg0_val |= 0x40;
	/* csb_high_time [9:8] */
	gspi_ctrl_reg0_val |= 0x100;
	/* spi_mode [10] */
	gspi_ctrl_reg0_val |= 0x000;
	/* clock_phase [11] */
	gspi_ctrl_reg0_val |= 0x000;
	/* Initializing GSPI for ULP read/writes */
	rsi_spi_master_reg_write(adapter,
				  GSPI_CTRL_REG0,
				  gspi_ctrl_reg0_val,
				  2);
}

static int spi_ulp_read_write( PRSI_ADAPTER adapter, u16 addr, u16 *data,
		u16 len_in_bits)
{
	if (rsi_spi_master_reg_write(adapter, GSPI_DATA_REG1,
				((addr << 6) | (data[1] & 0x3f)), 2) < 0)
		goto fail;
	if (rsi_spi_master_reg_write(adapter, GSPI_DATA_REG0,
				cpu_to_le16(*(u16 *)&data[0]), 2) < 0)
		goto fail;
	rsi_spi_gspi_init(adapter);
	if (rsi_spi_master_reg_write(adapter, GSPI_CTRL_REG1,
				((len_in_bits - 1) | GSPI_TRIG), 2) < 0)
		goto fail;
	msleep(10);
	return RSI_SUCCESS;
fail:
	return RSI_FAIL;
}

/**
 * rsi_reset_card() - Resets the card upon disconnect.
 */
static void rsi_reset_card( PRSI_ADAPTER adapter)
{
	u16 temp[4] = {0};

	if (rsi_spi_master_reg_write(adapter,
				SWBL_REGOUT, FW_WDT_DISABLE_REQ, 2) < 0) {
		RSI_DEBUG(RSI_ZONE_ERROR, "%s:%d FW WDT Disabling failed..\n",
				__func__, __LINE__);
		goto fail;
	}

#define TA_HOLD_REG 0x22000844
	rsi_spi_master_reg_write(adapter, TA_HOLD_REG, 0xE, 4);
	msleep(100);

	*(u32 *)temp = 2;
	if (spi_ulp_read_write(adapter, WATCH_DOG_TIMER_1, &temp[0],
							32) < 0)
			goto fail;
	*(u32 *)temp = 0;
	if (spi_ulp_read_write(adapter, WATCH_DOG_TIMER_2, temp,
							32) < 0)
			goto fail;
	*(u32 *)temp = 50;
	if (spi_ulp_read_write(adapter, WATCH_DOG_DELAY_TIMER_1, temp,
							32) < 0)
			goto fail;
	*(u32 *)temp = 0;
	if (spi_ulp_read_write(adapter, WATCH_DOG_DELAY_TIMER_2, temp,
							32) < 0)
			goto fail;
	*(u32 *)temp = ((0xaa000) | RESTART_WDT | BYPASS_ULP_ON_WDT);
	if (spi_ulp_read_write(adapter, WATCH_DOG_TIMER_ENABLE, temp,
							32) < 0)
			goto fail;

	return;
fail:
	RSI_DEBUG(RSI_ZONE_ERROR, "Reset card failed\n");
}



/*===========================================================================
 * @fn        VOID rsi_spi_disconnect ( struct spi_device *spidev)
 * @brief     This function performs the reverse of the probe function..
 * @param     Pointer to spi_device structure.
 * @return    int.
 */
int rsi_spi_disconnect(struct spi_device *spi_dev)
{
	int status;
	uint32 ii;
	PRSI_ADAPTER adapter   = rsi_getpriv(glbl_net_device);
	if (adapter == NULL){
		RSI_DEBUG(RSI_ZONE_ERROR,"rsi_spi_disconnect:Adapter is NULL \n");
	}
#ifdef RSI_SPI_INTERFACE
	if(adapter->irq_registered)
	{
		free_irq(gpio_to_irq(SPI_INTR_GPIO),(void *)spi_dev);
	}
#endif
	adapter->FSM_STATE  = 0;
	RSI_DEBUG(RSI_ZONE_INFO,"Killing thread \n");

	rsi_Delete_Event(&adapter->Event);
	rsi_Kill_Thread(adapter);
	rsi_Delete_Event(&adapter->rx_thread.event);
	rsi_Kill_Rx_Thread(&adapter->rx_thread);


	RSI_DEBUG(RSI_ZONE_INFO,"Purge queue \n");
	for (ii = 0; ii < 4; ii++)
	{
		rsi_queue_purge(&adapter->list[ii]);
	}
	rsi_queue_purge(&adapter->rx_q);
	rsi_reset_card(adapter);
	unregister_netdev(adapter->net_device0);
	free_netdev(adapter->net_device0);
	kfree(adapter->DataRcvPacket);
	return;
}/* End <rsi_disconnect> */
