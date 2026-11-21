/*******************************************************************************
* @file  rsi_hal_mcu_sdio.c
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
 * @file rsi_hal_mcu_sdio.c
 * @author 
 * @version 1.0
 *
 *
 * @section DESCRIPTION
 *
 * The file contains Generic HAL changes for SDIO.
 */

#include "rsi_sdio.h"
#include "rsi_nic.h"
#include <linux/version.h>
#include <linux/mmc/card.h>
#include "rsi_linux.h"
#include "rsi_api.h"
#include "rsi_hal.h"



extern struct net_device *glbl_net_device;
#define align_address(a) ((unsigned long)(a) & ~0x7)






/**===========================================================================================================
 * @fn        static  int rsi_cmd52writebyte(struct mmc_card *card, unsigned int address, unsigned char byte)
 * @brief     This function issues cmd52 byte write onto the card.
 * @param     Pointer to the mmc_card.  
 * @param     address to write.  
 * @param     data to write.  
 * @return    the write status 
 */
static  int rsi_cmd52writebyte(struct mmc_card *card, unsigned int address, unsigned char byte)
{
	struct mmc_command ioCmd;
	unsigned long      arg;

	memset(&ioCmd,0,sizeof(ioCmd));
	SDIO_SET_CMD52_WRITE_ARG(arg,0,address,byte);
	ioCmd.opcode = SD_IO_RW_DIRECT;
	ioCmd.arg = arg;
	ioCmd.flags = MMC_RSP_R5 | MMC_CMD_AC;

	return mmc_wait_for_cmd(card->host, &ioCmd, 0);
}/* End <rsi_cmd52writebyte> */ 








/**==========================================================================================================
 * @fn        static int rsi_cmd52readbyte(struct mmc_card *card, unsigned int address, unsigned char *byte)
 * @brief     This function issues cmd52 byte read onto the card.
 * @param     Pointer to the mmc_card.  
 * @param     address to read from.  
 * @param     variable to store read value.  
 * @return    the read status 
 */
static int rsi_cmd52readbyte(struct mmc_card *card, unsigned int address, unsigned char *byte)
{
  struct mmc_command ioCmd;
  unsigned long   arg;
  int err;

  memset(&ioCmd,0,sizeof(ioCmd));
  SDIO_SET_CMD52_READ_ARG(arg,0,address);
  ioCmd.opcode = SD_IO_RW_DIRECT;
  ioCmd.arg = arg;
  ioCmd.flags = MMC_RSP_R5 | MMC_CMD_AC;

  err = mmc_wait_for_cmd(card->host, &ioCmd, 0);

  if ((!err) && (byte)) 
  {
    *byte =  ioCmd.resp[0] & 0xFF;
  }

  return err;
}/* End <rsi_cmd52readbyte> */ 











/**===========================================================================
 * @fn        int rsi_issue_sdiocommand(struct sdio_func *func, uint32 opcode, uint32 arg, uint32 flags, uint32 *resp)  
 * @brief     This function issues sdio commands.
 * @param     Pointer to the sdio_func.  
 * @param     opcode value.  
 * @param     arguments to pass.  
 * @param     flags.  
 * @param     pointer to store response.  
 * @return    the command status
 */
static int rsi_issue_sdiocommand(struct sdio_func *func, uint32 opcode, uint32 arg, uint32 flags, uint32 *resp)
{
  struct mmc_command cmd;
  int err;
  struct mmc_host *host;

  host = func->card->host;

  memset(&cmd, 0, sizeof(struct mmc_command)); 
  cmd.opcode = opcode;
  cmd.arg = arg;
  cmd.flags = flags;
  err = mmc_wait_for_cmd(host, &cmd, 3);

  if ((!err) && (resp)) 
  {
    *resp = cmd.resp[0];
  }

  return err;
}/* End <rsi_issue_sdiocommand> */ 









/**===========================================================================
 * @fn        int  __devinit rsi_enable_interface(PVOID ptr)
 * @brief     This function enables the SDcard interface.
 * @param     Pointer to SDcard interface.  
 * @return    On success SD_SUCCESS is returned or SD_ERROR_NODEV on error. 
 */
#if KERNEL_VERSION_BTWN_2_6_(18, 22)
int  __devinit rsi_enable_interface(PVOID ptr)
#elif KERNEL_VERSION_GREATER_THAN_EQUALS_2_6_(26)
int  rsi_enable_interface(PVOID ptr)
#endif
{
#if KERNEL_VERSION_BTWN_2_6_(18, 22)
  PSDDEVICE pDevice = (PSDDEVICE)ptr;
  SDCONFIG_FUNC_ENABLE_DISABLE_DATA fData;
  fData.EnableFlags = SDCONFIG_ENABLE_FUNC;
  fData.TimeOut     = 500;
  return SDLIB_IssueConfig(pDevice,
      SDCONFIG_FUNC_ENABLE_DISABLE,
      &fData,
      sizeof(fData));
#elif KERNEL_VERSION_GREATER_THAN_2_6_(26)
  int32 ret;
  struct sdio_func *pfunction = (struct sdio_func *)ptr; 

  /* Wait for 100 ms timeout to enable the function */
  pfunction->enable_timeout = 100;
  ret = sdio_enable_func(pfunction);
  return ret;
#endif
}/* End <rsi_enable_interface> */ 










/**===========================================================================
 * @fn        VOID rsi_reset_card(struct sdio_func *pfunction)
 * @brief     This function resets and re-initializes the card.
 * @param     Pointer to sdio_func.  
 * @VOID 
 */
VOID rsi_reset_card(struct sdio_func *pfunction)
{
  int ret = 0;
  uint8 val =0;
  uint8 status;

#if 0
  //! Taking the backup values of CCCR registers
  rsi_cccr_register_backup(pfunction);
  /* Reset RS9116 chip */
  ret = rsi_cmd52writebyte(pfunction->card, SDIO_CCCR_ABORT, (1 << 3)); 
  /* Card will not send any response as it is getting reset immediately
   * Hence expect a timeout status from host controller */
  if (ret != -ETIMEDOUT)
  {

  }
#endif
  /* Wait for few milli seconds to get rid of residue charges if any */
  msleep(2);
  /* Initialize the SDIO card */
  do
  {
    int err;
    struct mmc_card *card = pfunction->card;
    struct mmc_host *host = card->host;
    uint8 cmd52_resp;
    uint32 clock;
    uint32 resp, i;
    uint16 rca;
    int bit = fls(host->ocr_avail) - 1;
    /* emulate the mmc_power_up(...) */
    host->ios.vdd = bit;
    host->ios.clock = host->f_min;
    host->ios.chip_select = MMC_CS_DONTCARE;
    host->ios.bus_mode = MMC_BUSMODE_OPENDRAIN;
    host->ios.power_mode = MMC_POWER_UP;
    host->ios.bus_width = MMC_BUS_WIDTH_1;
    host->ios.timing = MMC_TIMING_LEGACY;
    host->ops->set_ios(host, &host->ios);
    /*
     * This delay should be sufficient to allow the power supply
     * to reach the minimum voltage.
     */
    msleep(2);
    host->ios.clock = host->f_min;
    //host->ios.clock = 4000;
    host->ios.power_mode = MMC_POWER_ON;
    host->ops->set_ios(host, &host->ios);
    /*
     * This delay must be at least 74 clock sizes, or 1 ms, or the
     * time required to reach a stable voltage.
     */
    msleep(2);
    /* Issue CMD0. Goto idle state */
    host->ios.chip_select = MMC_CS_HIGH;
    host->ops->set_ios(host, &host->ios);
    msleep(2);
    err = rsi_issue_sdiocommand(pfunction, MMC_GO_IDLE_STATE, 0, (MMC_RSP_NONE | MMC_CMD_BC), NULL);
    host->ios.chip_select = MMC_CS_DONTCARE;
    host->ops->set_ios(host, &host->ios);
    msleep(2);
    host->use_spi_crc = 0;

    if (err) 
    {
      RSI_DEBUG(RSI_ZONE_SDIO_DBG, "%s: CMD0 failed : %d \n",__func__,err);
      break;
    }        

#if LINUX_VERSION_CODE <= KERNEL_VERSION(3,12,74)
    if (!host->ocr) 
#else
      if (!card->ocr)
#endif
      {
        /* Issue CMD5, arg = 0 */
        err = rsi_issue_sdiocommand(pfunction, SD_IO_SEND_OP_COND, 0, (MMC_RSP_R4 | MMC_CMD_BCR), &resp);
        if (err) 
        {
          RSI_DEBUG(RSI_ZONE_SDIO_DBG, (TEXT("%s: CMD5 failed : %d \n"),__func__,err));
          break;
        }
#if LINUX_VERSION_CODE <= KERNEL_VERSION(3,12,74)
        err = rsi_issue_sdiocommand(pfunction, SD_IO_SEND_OP_COND, 0, (MMC_RSP_R4 | MMC_CMD_BCR), &resp);
        host->ocr = resp;
#else
        card->ocr = resp;
#endif
      }
    //msleep(100);
    /* Issue CMD5, arg = ocr. Wait till card is ready  */
    {
#if LINUX_VERSION_CODE <= KERNEL_VERSION(3,12,74)
      err = rsi_issue_sdiocommand(pfunction, SD_IO_SEND_OP_COND, 0, (MMC_RSP_R4 | MMC_CMD_BCR), &resp);
      msleep(100);

      err = rsi_issue_sdiocommand(pfunction, SD_IO_SEND_OP_COND, host->ocr, (MMC_RSP_R4 | MMC_CMD_BCR), &resp);
#else
      err = rsi_issue_sdiocommand(pfunction, SD_IO_SEND_OP_COND, card->ocr, (MMC_RSP_R4 | MMC_CMD_BCR), &resp);
#endif
      if (err) 
      {
        RSI_DEBUG(RSI_ZONE_SDIO_DBG, (TEXT("%s: CMD5 failed : %d \n"),__func__,err));
        break;
      }
      if (resp & MMC_CARD_BUSY) 
      {
        break;
      }
    /* Adding some delay for the card to stable  */
     //msleep(10);
     msleep(2);
    }

#if LINUX_VERSION_CODE <= KERNEL_VERSION(3,12,74)
    host->ocr = resp;
#else
    card->ocr = resp;
#endif


    /* Issue CMD3, get RCA */
    err = rsi_issue_sdiocommand(pfunction, SD_SEND_RELATIVE_ADDR, 0, MMC_RSP_R6 | MMC_CMD_BCR, &resp);
    if (err) 
    {
      RSI_DEBUG(RSI_ZONE_SDIO_DBG, (TEXT("%s: CMD3 failed : %d \n"),__func__,err));
      break;
    }
    rca = resp >> 16;
    host->ios.bus_mode = MMC_BUSMODE_PUSHPULL;
    host->ops->set_ios(host, &host->ios);

    /* Issue CMD7, select card  */
    err = rsi_issue_sdiocommand(pfunction, MMC_SELECT_CARD, (rca << 16), MMC_RSP_R1 | MMC_CMD_AC, NULL);
    if (err) 
    {
      RSI_DEBUG(RSI_ZONE_SDIO_DBG, (TEXT("%s: CMD7 failed : %d \n"),__func__,err));
      break;
    }

    /* Enable high speed */
    if (card->host->caps & MMC_CAP_SD_HIGHSPEED) 
    {
      RSI_DEBUG(RSI_ZONE_SDIO_DBG, (TEXT("%s: Set high speed mode\n"),__func__));
      err = rsi_cmd52readbyte(card, SDIO_CCCR_SPEED, &cmd52_resp);
      if (err) 
      {
        RSI_DEBUG(RSI_ZONE_SDIO_DBG, (TEXT("%s: CMD52 read to CCCR speed register failed  : %d \n"),__func__,err));
#if LINUX_VERSION_CODE <= KERNEL_VERSION(3,15,10)
        card->state &= ~MMC_STATE_HIGHSPEED;
#endif
        /* no need to break */
      } 
      else 
      {
        /* CMD52 write to CCCR speed register  */
        err = rsi_cmd52writebyte(card, SDIO_CCCR_SPEED, (cmd52_resp | SDIO_SPEED_EHS));
        if (err) 
        {
          RSI_DEBUG(RSI_ZONE_SDIO_DBG, (TEXT("%s: CMD52 write to CCCR speed register failed  : %d \n"),__func__,err));
          break;
        }
#if (LINUX_VERSION_CODE < KERNEL_VERSION(3,16,0))
        mmc_card_set_highspeed(card);
#endif
        host->ios.timing = MMC_TIMING_SD_HS;
        host->ops->set_ios(host, &host->ios);
      }
    }
    /* Set clock */
#if (LINUX_VERSION_CODE < KERNEL_VERSION(3,16,0))
    if (mmc_card_highspeed(card)) 
#else
      if (mmc_card_hs(card))
#endif
      {
        /*!setting the clock speed */
        clock = SDIO_CLOCK;
      } 
      else 
      {
        clock = card->cis.max_dtr;
      }

    if (clock > host->f_max) 
    {
      clock = host->f_max;
    }
    host->ios.clock = clock;
    host->ops->set_ios(host, &host->ios);

    if (card->host->caps & MMC_CAP_4_BIT_DATA) 
    {
      /* CMD52: Set bus width & disable card detect resistor */
      err = rsi_cmd52writebyte(card, SDIO_CCCR_IF, SDIO_BUS_CD_DISABLE | SDIO_BUS_WIDTH_4BIT);
      if (err) 
      {
        RSI_DEBUG(RSI_ZONE_SDIO_DBG, (TEXT("%s: CMD52 to set bus mode failed : %d \n"),__func__,err));
        break;
      }
      host->ios.bus_width = MMC_BUS_WIDTH_4;
      host->ops->set_ios(host, &host->ios);
    }
  } while (0);

  /* Writing back the CCCR Register values after chip got reset */
  //rsi_write_cccr_registers(pfunction);
  msleep(2);
  return;
}/* End <rsi_reset_card> */ 










/**===========================================================================
 * @fn        int rsi_disable_interface(void *ptr)
 * @brief     This function disables the card interface.
 * @param     Pointer to SDcard interface.  
 * @return    On success SD_SUCCESS is returned or SD_ERROR_NODEV on error. 
 */
#if KERNEL_VERSION_BTWN_2_6_(18, 22)
int  __devexit rsi_disable_interface(void *ptr)
#elif KERNEL_VERSION_GREATER_THAN_EQUALS_2_6_(26)
int    rsi_disable_interface(void *ptr)
#endif
{
#if KERNEL_VERSION_BTWN_2_6_(18, 22)
  PSDDEVICE pDevice = (PSDDEVICE)ptr; 
  SDCONFIG_FUNC_ENABLE_DISABLE_DATA fData;

  fData.EnableFlags = SDCONFIG_DISABLE_FUNC;
  fData.TimeOut     = 500;
  return SDLIB_IssueConfig(pDevice,
      SDCONFIG_FUNC_ENABLE_DISABLE,
      &fData,
      sizeof(fData));
#elif KERNEL_VERSION_GREATER_THAN_2_6_(26)
  struct sdio_func *pfunction = (struct sdio_func*)ptr;
  int ret = 0;

  ret = sdio_disable_func(pfunction);
  if (ret)
  {
    RSI_DEBUG(RSI_ZONE_SDIO_DBG, (TEXT("Failed to diable sdio func : %d \n"),ret));
  }
  return ret;
#endif
}/* End <rsi_disable_interface> */









/**========================================================================================================
 * @fn        int32 rsi_request_interrupt_handler(void *ptr,sdio_irq_handler_t interrupt_handler, PVOID pContext)
 * @brief     This function registers the client driver's interrupt handler
 *            with the bus driver.
 * @param     Pointer to SD card interface.  
 * @param     Pointer to interrupt handler.  
 * @return    VOID. 
 */
#if KERNEL_VERSION_BTWN_2_6_(18, 22)
int32 __devinit rsi_request_interrupt_handler(void *ptr,
    sdio_irq_handler_t  interrupt_handler,
    PVOID pContext
    )
#elif KERNEL_VERSION_GREATER_THAN_EQUALS_2_6_(26)
int32 rsi_request_interrupt_handler(void *ptr,
		                             sdio_irq_handler_t    interrupt_handler,
                                      PVOID pContext
                                      )
#endif
{
#if KERNEL_VERSION_BTWN_2_6_(18, 22)
	PSDDEVICE pDevice = (PSDDEVICE)ptr;
	int32 status;

	SDDEVICE_SET_IRQ_HANDLER(pDevice, interrupt_handler, pContext);
	RSI_DEBUG(RSI_ZONE_INIT,
	             (TEXT("%s: Unmasking IRQ \n"), __func__));
	status = SDLIB_IssueConfig(pDevice, SDCONFIG_FUNC_UNMASK_IRQ, NULL, 0);
	if (!SDIO_SUCCESS((status))) 
	{
		RSI_DEBUG(RSI_ZONE_ERROR,
		             (TEXT( "%s: failed to unmask IRQ %d\n"), __func__, status));
	}
#elif KERNEL_VERSION_GREATER_THAN_EQUALS_2_6_(26)
	struct sdio_func *pfunction = (struct sdio_func*)ptr;
	int32         status;
	status = sdio_claim_irq(pfunction,(sdio_irq_handler_t *)interrupt_handler);
	if (status != 0)
	{
		printk(" failed to unmask IRQ \n");
	}
#endif
	return status;
}/* End <rsi_request_interrupt_handler> */









/**===========================================================================
 * @fn         VOID rsi_sdio_interrupt_handler(struct sdio_func *function)
 * @brief      This function registers the sdio driver's interrupt handler
 * @param      Pointer to interrupt handler.  
 * @return     VOID. 
 */

#if KERNEL_VERSION_BTWN_2_6_(18, 22)
VOID rsi_sdio_interrupt_handler(PVOID pContext)
#elif KERNEL_VERSION_GREATER_THAN_2_6_(26)
VOID rsi_sdio_interrupt_handler(struct sdio_func *function)
#endif
{

	PRSI_ADAPTER adapter ;
#if KERNEL_VERSION_BTWN_2_6_(18, 22)
	adapter = (PRSI_ADAPTER)pContext;
#elif KERNEL_VERSION_GREATER_THAN_2_6_(26)
	adapter = rsi_getcontext(function);
#endif
#if KERNEL_VERSION_GREATER_THAN_2_6_(26)
	adapter->irq_registered = current;
#endif
	rsi_interrupt_handler(adapter);
#if KERNEL_VERSION_GREATER_THAN_2_6_(26)
	adapter->irq_registered = NULL;
#endif
}/* End <rsi_sdio_interrupt_handler> */











/**===========================================================================
 * @fn        int16 rsi_init_host_interface(PRSI_ADAPTER Adapter)
 * @brief     This function does the actual initialization of SDBUS slave registers. 
 * @param     Pointer to the driver adapter structure. 
 * @return    on success 0 and  on failure -1. 
 */
int16 rsi_init_host_interface(PRSI_ADAPTER Adapter)
{
  UINT8 byte;
  int8 status;
  RSI_DEBUG(RSI_ZONE_INIT,(TEXT("+ ganges_init_sdio_slave_regs\n")));

  /* initialize Next read delay */
  if (Adapter->next_read_delay)
  {
    RSI_DEBUG(RSI_ZONE_INIT,
        (TEXT("init_sdio_slave_regs: Initialzing SDIO_NXT_RD_DELAY2\n")));
    byte = Adapter->next_read_delay;

    status = write_register(Adapter,
        0,
        SDIO_NXT_RD_DELAY2,
        &byte);
    if (status)
    {
      RSI_DEBUG(RSI_ZONE_INIT,
          (TEXT("init_sdio_slave_regs: fail to write SDIO_NXT_RD_DELAY2\n")));
      return -1;
    }
  }

  if (Adapter->sdio_high_speed_enable)
  {
#define SDIO_REG_HIGH_SPEED      0x13
    RSI_DEBUG(RSI_ZONE_INIT,
        (TEXT("init_sdio_slave_regs: Enabling SDIO High speed\n")));
    byte = 0x3;

    /* Enabling SDIO high speed */
    status =write_register(Adapter,
        0,
        SDIO_REG_HIGH_SPEED,
        &byte);
    if (status)
    {
      RSI_DEBUG(RSI_ZONE_ERROR,
          (TEXT("init_sdio_slave_regs: fail to enable SDIO high speed\n")));
      return -1;
    }

  }

  /* This tells SDIO FIFO when to start read to host */
  RSI_DEBUG(RSI_ZONE_INIT,
      (TEXT("init_sdio_slave_regs: Initialzing SDIO read start level\n")));
  byte = 0x24;

  status = write_register(Adapter,
      0,
      SDIO_READ_START_LVL,
      &byte);
  if (status)
  {
    RSI_DEBUG(RSI_ZONE_ERROR,
        (TEXT("init_sdio_slave_regs: fail to write SDIO_READ_START_LVL\n")));
    return -1;
  }

  /* Change these parameters to load firmware */
  /* This tells SDIO FIFO when to start read to host */
  RSI_DEBUG(RSI_ZONE_INIT,
      (TEXT("init_sdio_slave_regs: Initialzing FIFO ctrl registers\n")));
  byte = (128-32);

  status = write_register(Adapter,
      0,
      SDIO_READ_FIFO_CTL,
      &byte);
  if (status)
  {
    RSI_DEBUG(RSI_ZONE_ERROR,
        (TEXT("init_sdio_slave_regs: fail to write SDIO_READ_FIFO_CTL\n")));
    return -1;
  }

  /* This tells SDIO FIFO when to start read to host */
  byte = 32;
  status = write_register(Adapter,
      0,
      SDIO_WRITE_FIFO_CTL,
      &byte);
  if (status)
  {
    RSI_DEBUG(RSI_ZONE_ERROR,
        (TEXT("init_sdio_slave_regs: fail to write SDIO_WRITE_FIFO_CTL\n")));
    return -1;
  }

  RSI_DEBUG(RSI_ZONE_INIT,(TEXT("- ganges_init_sdio_slave_regs\n")));
  return 0;
}/* End <rsi_init_host_interface> */









/**===========================================================================
 * @fn        VOID rsi_sdio_disconnect ( struct sdio_func *pfunction)
 * @brief     This function performs the reverse of the probe function..
 * @param     Pointer to sdio_func structure.  
 * @param     Pointer to sdio_device_id structure.  
 * @return    VOID. 
 */
#if KERNEL_VERSION_BTWN_2_6_(18, 22)
VOID __devexit rsi_sdio_disconnect ( PSDFUNCTION pfunction, PSDDEVICE   pDevice)
#elif KERNEL_VERSION_GREATER_THAN_EQUALS_2_6_(26)
VOID rsi_sdio_disconnect ( struct sdio_func *pfunction)
#endif
{
  int status;
  uint32 ii;
#if KERNEL_VERSION_BTWN_2_6_(18, 22)
  struct net_device *dev = pfunction->pContext;
#elif KERNEL_VERSION_GREATER_THAN_2_6_(26)
  struct net_device *dev = rsi_getcontext(pfunction);
#endif
  PRSI_ADAPTER adapter   = rsi_getpriv(glbl_net_device);
  adapter->FSM_STATE  = 0;
  RSI_DEBUG(RSI_ZONE_INFO,"Killing thread \n");
  adapter->halt_flag = 1;
  rsi_sdio_claim_host(pfunction);
  if(adapter->irq_registered != NULL)
  {	
    rsi_sdio_release_irq(pfunction);
  }
  rsi_sdio_release_host(pfunction);
  adapter->stop_card_writing = 2;

  rsi_Kill_Thread(adapter);
  rsi_Delete_Event(&adapter->PwrSaveEvent);
  rsi_Delete_Event(&adapter->Event);


  flush_workqueue(adapter->workqueue);
  destroy_workqueue(adapter->workqueue);
  RSI_DEBUG(RSI_ZONE_INFO,"Purge queue \n");
  for (ii = 0; ii < 4; ii++)
  {
    rsi_queue_purge(&adapter->list[ii]);
  }
  rsi_queue_purge(&adapter->rx_q);
  rsi_sdio_claim_host(pfunction);
  /* Resetting the sdio card to make it ready for the next run */
  rsi_reset_card(pfunction);
  sdio_disable_func(pfunction);
  /* Release host */
  rsi_sdio_release_host(pfunction);
  unregister_netdev(adapter->net_device0);
  free_netdev(adapter->net_device0);
  kfree(adapter->DataRcvPacket);  
  return;
}/* End <rsi_disconnect> */










/**===========================================================================
 * @fn        int  __devinit rsi_setblocklength(PRSI_ADAPTER adapter, uint32 length)
 * @brief     This function sets the host block length.
 * @param     Pointer to Driver adapter structure.  
 * @param     Block lenght to be set.  
 * @return    On success SD_SUCCESS is returned or negative error code on failure. 
 */
#if KERNEL_VERSION_BTWN_2_6_(18, 22)
int  __devinit rsi_setblocklength(PRSI_ADAPTER adapter, uint32 length)
{
  PSDDEVICE    pDevice    = adapter->pDevice;
  int32        blklength;

  RSI_DEBUG(rsi_ZONE_INIT,(TEXT( "%s: Setting the block length\n"), __func__));

  SDLIB_SetFunctionBlockSize(pDevice,256);
  blklength = SDDEVICE_GET_OPER_BLOCK_LEN(pDevice);

  RSI_DEBUG(rsi_ZONE_INFO,(TEXT( "%s: Operational blk length is %d\n", __func__),
        blklength));
  return int_SUCCESS; 
} 
#elif KERNEL_VERSION_GREATER_THAN_EQUALS_2_6_(26)
int        rsi_setblocklength(PRSI_ADAPTER adapter, uint32 length)
{
  int32  status;
  status = sdio_set_block_size(adapter->pfunction, length);
  adapter->pfunction->max_blksize = 256;
  return status;
} /* End <rsi_setblocklength> */
#endif








/**===========================================================================
 * @fn        VOID rsi_sdio_release_host(PSDFUNCTION pFunction)
 * @brief     This function releases a bus after a certain SDIO function.
 * @param     Pointer to sdio_func structure.  
 * @return    VOID. 
 */
#if KERNEL_VERSION_BTWN_2_6_(18, 22)
VOID rsi_sdio_release_host(PSDFUNCTION pFunction)
#elif KERNEL_VERSION_GREATER_THAN_EQUALS_2_6_(26)
VOID rsi_sdio_release_host(struct sdio_func *pfunction)
#endif
{
#if KERNEL_VERSION_BTWN_2_6_(18, 22)
  /*No code here*/
#elif KERNEL_VERSION_GREATER_THAN_EQUALS_2_6_(26)
  sdio_release_host(pfunction);
#endif
  return;
}/* End <rsi_sdio_release_host> */








/**===========================================================================
 * @fn        int  VOID rsi_setcontext(PSDFUNCTION pFunction, void *adapter)
 * @brief     This function sets the private data of the card function as 
 *            our n/w interface.
 * @param     Pointer to sdio_func structure. 
 * @param     Pointer to our network interface 
 * @return    VOID. 
 */
#if KERNEL_VERSION_BTWN_2_6_(18, 22)
VOID rsi_setcontext(PSDFUNCTION pFunction, void *adapter)
#elif KERNEL_VERSION_GREATER_THAN_EQUALS_2_6_(26)
VOID rsi_setcontext(struct sdio_func *pfunction, void *adapter)
#endif
{
#if KERNEL_VERSION_BTWN_2_6_(18, 22)
  /*No code for 2.6.18*/
#elif KERNEL_VERSION_GREATER_THAN_EQUALS_2_6_(26)
  sdio_set_drvdata(pfunction, adapter);
#endif
  return;
}/* End <rsi_setcontext> */









/**===========================================================================
 * @fn        void* rsi_getcontext(struct sdio_func *pfunction)
 * @brief     This function gets the private data of the cards function.
 * @param     Pointer to sdio_func structure.  
 * @return    Pointer to a our network interface. 
 */
#if KERNEL_VERSION_BTWN_2_6_(18, 22)
struct net_device * rsi_getcontext(PSDFUNCTION pfunction)
#elif KERNEL_VERSION_GREATER_THAN_EQUALS_2_6_(26)
void* rsi_getcontext(struct sdio_func *pfunction)
#endif
{
#if KERNEL_VERSION_BTWN_2_6_(18, 22)
  return NULL;
#elif KERNEL_VERSION_GREATER_THAN_2_6_(26)
  void *dev;
  dev = sdio_get_drvdata(pfunction);
  return dev;
#endif
}/* End <rsi_getcontext> */









/**===========================================================================
 * @fn        int32 rsi_sdio_release_irq(PSDFUNCTION pFunction)
 * @brief     This function releases the irq registered with the card.
 * @param     Pointer to sdio_func structure.  
 * @return    0 on sucess else a negative number with specific failure. 
 */
#if KERNEL_VERSION_BTWN_2_6_(18, 22)
int32 rsi_sdio_release_irq(PSDFUNCTION pFunction)
#elif KERNEL_VERSION_GREATER_THAN_EQUALS_2_6_(26)
int32 rsi_sdio_release_irq(struct sdio_func *pfunction)
#endif
{
#if KERNEL_VERSION_BTWN_2_6_(18, 22)
  return 0;
#elif KERNEL_VERSION_GREATER_THAN_2_6_(26)
  return sdio_release_irq(pfunction);
#endif
}/* End <rsi_sdio_release_irq> */







/**===========================================================================
 * @fn        int32 deregister_sdio_irq(PRSI_ADAPTER adapter)
 * @brief     This function deregisters the irq with the card.
 * @param     Pointer to sdio_func structure.  
 * @return    0 on sucess else a negative number with specific failure. 
 */
int32 deregister_sdio_irq(PRSI_ADAPTER adapter)
{
#if KERNEL_VERSION_GREATER_THAN_2_6_(26)
  int32 s;

  rsi_sdio_claim_host(adapter->pfunction);
  s = rsi_sdio_release_irq(adapter->pfunction);
  if (s) {
    RSI_DEBUG(RSI_ZONE_SDIO_DBG,
        (TEXT("%s: Unable to Deregister irq"), __func__));
  }	
  rsi_sdio_release_host(adapter->pfunction);

  return s;
#endif
}/* End <deregister_sdio_irq> */








/**===========================================================================
 * @fn         int rsi_setupcard(PRSI_ADAPTER adapter)
 * @brief      This function queries and sets the card's features.
 * @param      Pointer to Driver adapter structure.  
 * @return     On success int_SUCCESS else int_FAILURE is returned.
 */
int rsi_setupcard(PRSI_ADAPTER adapter)
{
  int Status = 0;

  /* Setting sdio clock frequency to 50MHz */
#ifdef FPGA_VALIDATION
  rsi_setclock(adapter,6000);
#else
  rsi_setclock(adapter,50000);
#endif  
  if(Status!=0)
  {
    RSI_DEBUG(RSI_ZONE_INFO,
        (TEXT("%s: Unsuccessful at setting clk"), __func__));
    return Status;
  }
#if KERNEL_VERSION_BTWN_2_6_(18, 22) 
  adapter->CardCapability = SDDEVICE_GET_SDIO_CARD_CAPS(adapter->pDevice);
  RSI_DEBUG(RSI_ZONE_INFO,(TEXT("%s: Card Cap: %0x\n"), __func__,
        adapter->CardCapability));
  RSI_DEBUG(RSI_ZONE_INFO,(TEXT("%s: Common CIS Ptr:  %0x\n"), __func__,
        SDDEVICE_GET_SDIO_COMMON_CISPTR(adapter->pDevice)));
  RSI_DEBUG(RSI_ZONE_INFO,(TEXT("%s: Funcn CIS Ptr:  %0x\n"), __func__,
        SDDEVICE_GET_SDIO_FUNC_CISPTR(adapter->pDevice)));
  RSI_DEBUG(RSI_ZONE_INFO,(TEXT("%s: CSA Ptr:  %0x\n"), __func__,
        SDDEVICE_GET_SDIO_FUNC_CSAPTR(adapter->pDevice)));
#endif
  adapter->TransmitBlockSize = 256;
  adapter->ReceiveBlockSize  = 256;
  Status = rsi_setblocklength(adapter, adapter->TransmitBlockSize);

  if(Status != 0)
  {
    RSI_DEBUG(RSI_ZONE_INFO,
        (TEXT("%s: Unable to set block length\n"), __func__));
    return Status;
  }

  return Status;  
}/* End <rsi_setupcard> */





/**===========================================================================
 * @fn        int rsi_setclock(PRSI_ADAPTER adapter, uint32 Freq)
 * @brief     This function sets the clock frequency
 * @param     adapter   Pointer To rsi_ADAPTER sturct
 * @param     Frequency   Clock frequency
 */
int rsi_setclock(PRSI_ADAPTER adapter, uint32 Freq)
{
#if KERNEL_VERSION_BTWN_2_6_(18,22)
  SDCONFIG_BUS_MODE_DATA  busSettings;
#ifdef rsi_WITHOUT_HARDWARE
  {
    return int_SUCCESS;
  }
#endif
  adapter->os_intf_ops->rsi_memset(&busSettings, 0, sizeof(SDCONFIG_BUS_MODE_DATA));
  busSettings.BusModeFlags = SDDEVICE_GET_BUSMODE_FLAGS(adapter->pDevice);

  RSI_DEBUG(rsi_ZONE_INIT,
      (TEXT("%s: Forcing SDIO clock to %dMHz\n"), __func__,
       busSettings.ClockRate));

  busSettings.ClockRate = Freq * 1000;
  return SDLIB_IssueConfig(adapter->pDevice, SDCONFIG_BUS_MODE_CTRL,
      &busSettings, sizeof(SDCONFIG_BUS_MODE_DATA));
#else
  uint32 clock;
  struct mmc_host *host = adapter->pfunction->card->host;

  clock = Freq * 1000;
  if (clock > host->f_max) 
  {
    clock = host->f_max;
  }
  host->ios.clock = clock;
  host->ops->set_ios(host, &host->ios);
  return 0;
#endif
}/* End <rsi_setclock> */





/**===========================================================================
 * @fn        int read_register(PRSI_ADAPTER adapter, uint32 Addr,uint8 fun_num,uint8 *data)
 * @brief     This function reads one byte of information from a register.
 * @param     Pointer to Driver adapter structure.  
 * @param     Function Number.  
 * @param     Address of the register.  
 * @param     Pointer to the data that stores the data read.  
 * @return    On success int_SUCCESS else int_FAILURE. 
 */
int read_register(PRSI_ADAPTER adapter, uint32 Addr,uint8 fun_num,uint8 *data)
{

  int status;
#if KERNEL_VERSION_BTWN_2_6_(18, 22)
  return SDLIB_IssueCMD52(adapter->pDevice,
      fun_num,
      Addr,
      data,
      1,
      rsi_FALSE  
      );

#elif KERNEL_VERSION_GREATER_THAN_2_6_(26)

  if(rsi_likely(adapter->irq_registered != current))
  {
    rsi_sdio_claim_host(adapter->pfunction);
  }
  if (fun_num == 0)
  {
    *data = sdio_f0_readb(adapter->pfunction, Addr, &status);
  }
  else
  {
    *data = sdio_readb(adapter->pfunction, Addr, &status);
  } /* End if <condition> */

  if (rsi_likely(adapter->irq_registered != current))
  {
    rsi_sdio_release_host(adapter->pfunction);
  }
  if (status)
  {
    return -1;
  }
  else
  {
    return 0;
  }/* End if <condition> */
#endif
}/* End <read_register> */






/**===========================================================================
 * @fn        int write_register(PRSI_ADAPTER adapter,uint8 reg_dmn,uint32 Addr,uint8 *data)
 * @brief     This function writes one byte of information into a register.
 * @param     Pointer to Driver adapter structure.  
 * @param     Function Number.  
 * @param     Address of the register.  
 * @param     Pointer to the data tha has to be written.  
 * @return    On success int_SUCCESS else int_FAILURE. 
 */
int write_register(PRSI_ADAPTER adapter,uint8 reg_dmn,uint32 Addr,uint8 *data)
{
#if KERNEL_VERSION_BTWN_2_6_(18, 22)
  return SDLIB_IssueCMD52(adapter->pDevice,
      reg_dmn,
      Addr,
      data,
      1,
      rsi_TRUE  
      );
#elif KERNEL_VERSION_GREATER_THAN_2_6_(26)
  int status=0;

  if (rsi_likely(adapter->irq_registered != current))
  {
    rsi_sdio_claim_host(adapter->pfunction);
  }

  if (reg_dmn == 0)
  {

    sdio_f0_writeb(adapter->pfunction, *data, Addr, &status);
  }
  else
  {
    sdio_writeb(adapter->pfunction, *data, Addr, &status);
  } /* End if <condition> */
  if (rsi_likely(adapter->irq_registered != current))
  { 
    rsi_sdio_release_host(adapter->pfunction);
  }
  if (status)
  {
    return -1;
  }
  else
  {
    return 0;
  } /* End if <condition> */
#endif
}/* End <write_register> */







/**==========================================================================================
 * @fn        int read_register_multiple(PRSI_ADAPTER adapter,uint32 Addr,uint32 Count,uint8 *data)
 * @brief     This function read multiple bytes of information from the SD card.
 * @param     Pointer to Driver adapter structure.  
 * @param     Function Number.  
 * @param     Address of the register.  
 * @param     Length of the data to be read.  
 * @param     Pointer to the read data.  
 * @return    On success int_SUCCESS else int_FAILURE. 
 */
int read_register_multiple(PRSI_ADAPTER adapter, 

                                     uint32 Addr,
                                     uint32 Count,
                                     uint8 *data )
{
  uint32      status;
#if KERNEL_VERSION_BTWN_2_6_(18, 22)  
  PSDREQUEST  pReq       = NULL;
  PSDDEVICE   pDevice ; 
  uint32      num_blocks = Count / 256;
#endif
  uint32 ii;
#if KERNEL_VERSION_BTWN_2_6_(18, 22)
  pDevice = adapter->pDevice;

  if (Count % 256 != 0)
  {
    ++num_blocks;
  }

  pReq = SDDeviceAllocRequest(pDevice);
  if (pReq == NULL)
  {
    RSI_DEBUG(rsi_ZONE_ERROR,
        (TEXT("%s: Request Allocation Failed\n"), __func__));
    return int_NO_RESOURCES;
  }

  /* If Length Less Than 256 Then Byte Mode */
  if (Count < 256)
  {
    SDIO_SET_CMD53_ARG(pReq->Argument,
        CMD53_READ,
        1,
        CMD53_BYTE_BASIS,
        CMD53_FIXED_ADDRESS,
        Addr,
        Count);
    pReq->pDataBuffer = data;
    pReq->Command     = CMD53;
    pReq->Flags       = SDREQ_FLAGS_RESP_SDIO_R5 | SDREQ_FLAGS_DATA_TRANS;
    pReq->BlockLen    = Count;
    pReq->BlockCount  = 1;    
  }
  else  /* Block Mode */
  {
    SDIO_SET_CMD53_ARG(pReq->Argument,
        CMD53_READ,
        1,
        CMD53_BLOCK_BASIS,
        CMD53_FIXED_ADDRESS,
        Addr,
        num_blocks);
    pReq->pDataBuffer = data;
    pReq->Command     = CMD53;
    pReq->Flags       = SDREQ_FLAGS_RESP_SDIO_R5 | SDREQ_FLAGS_DATA_TRANS;
    pReq->BlockLen    = 256; /*adapter->ReceiveBlockSize;*/
    pReq->BlockCount  = num_blocks;    
  } /* End if <condition> */
  status = SDDEVICE_CALL_REQUEST_FUNC(pDevice, pReq);
  if (!status)
  {
    RSI_DEBUG(rsi_ZONE_ERROR,
        (TEXT("%s: Synch Cmd53 read failed\n"), __func__));
  }
  else
  {
    /*
     * RSI_DEBUG(rsi_ZONE_ERROR,
     * "rsi_read_register_multiple: Synch Cmd53 read Success\n");
     * */
  } /* End if <condition> */

  SDDeviceFreeRequest(pDevice,pReq);               
  return status;
#elif KERNEL_VERSION_GREATER_THAN_2_6_(26)

  if (rsi_likely(adapter->irq_registered != current))
  {
    RSI_DEBUG(RSI_ZONE_INFO,
        (TEXT("%s: Calling sdio_claim_host here\n"), __func__));
    rsi_sdio_claim_host(adapter->pfunction);
  }

  status =  sdio_readsb(adapter->pfunction, data, Addr, Count);
  if (rsi_likely(adapter->irq_registered != current))
  {
    rsi_sdio_release_host(adapter->pfunction);
  }
  if (status != 0)
  {
    RSI_DEBUG(RSI_ZONE_ERROR,
        (TEXT("%s: Synch Cmd53 read failed\n"), __func__));
  }
  return status;
#endif

}/* End <read_register_multiple> */  






/**============================================================================================
 * @fn        int write_register_multiple(PRSI_ADAPTER adapter,uint32 Addr,uint8 *data,uint32 Count)
 * @brief     This function writes multiple bytes of information to the SD card.
 * @param     Pointer to Driver adapter structure.  
 * @param     Function Number.  
 * @param     Address of the register.  
 * @param     Length of the data.  
 * @param     Pointer to the data that has to be written.  
 * @return    On success int_SUCCESS else int_FAILURE. 
 */
int write_register_multiple(PRSI_ADAPTER adapter,
                                      uint32 Addr,
                                      uint8 *data,
                                      uint32 Count
                                     )
{
  int32   status = 0;
#if KERNEL_VERSION_BTWN_2_6_(18, 22)  
  PSDDEVICE    pDevice;
  PSDREQUEST   pReq       = NULL;
  uint32       num_blocks = Count / 256;
#endif
#if KERNEL_VERSION_BTWN_2_6_(18, 22)
  pDevice    = adapter->pDevice;
  if (Count % 256 != 0)
  {
    ++num_blocks;
  }

  pReq = SDDeviceAllocRequest(pDevice);
  if (pReq == NULL)
  {
    RSI_DEBUG(rsi_ZONE_ERROR,
        (TEXT("%s: Resource Allocation Failed\n"), __func__));
    return int_NO_RESOURCES;
  }

  /* If Length Less Than 256 Then Byte Mode */
  if (Count < 256)
  {
    SDIO_SET_CMD53_ARG(pReq->Argument,
        CMD53_WRITE,
        1,
        CMD53_BYTE_BASIS,
        CMD53_FIXED_ADDRESS,
        Addr,
        Count);
    pReq->pDataBuffer = &data[0];
    pReq->Command     = CMD53;
    pReq->Flags       = SDREQ_FLAGS_RESP_SDIO_R5 |
      SDREQ_FLAGS_DATA_TRANS   |
      SDREQ_FLAGS_DATA_WRITE;
    pReq->BlockCount = 1;    
    pReq->BlockLen   = Count;

  }
  else /* Block Mode */
  {
    SDIO_SET_CMD53_ARG(pReq->Argument,
        CMD53_WRITE,
        1,
        CMD53_BLOCK_BASIS,
        CMD53_FIXED_ADDRESS,
        Addr,
        num_blocks);
    pReq->pDataBuffer = &data[0];
    pReq->Command     = CMD53;
    pReq->Flags       = SDREQ_FLAGS_RESP_SDIO_R5 |
      SDREQ_FLAGS_DATA_TRANS   |
      SDREQ_FLAGS_DATA_WRITE;
    pReq->BlockCount = num_blocks;    
    pReq->BlockLen   = 256; /* adapter->TransmitBlockSize; */
  } /* End if <condition>i */
  do
  {
    status = SDDEVICE_CALL_REQUEST_FUNC(pDevice, pReq);
    if (!(status))
    {
      RSI_DEBUG(rsi_ZONE_ERROR,
          (TEXT("%s: Synch Cmd53 write failed %d\n"), __func__, 
           status));
    } 
  } while (!(status));
  SDDeviceFreeRequest(pDevice,pReq);               

  return status;

#elif KERNEL_VERSION_GREATER_THAN_2_6_(26)
  if(0)
  {
    return -1;	
  }
  if(rsi_likely(adapter->irq_registered != current))
  {
    rsi_sdio_claim_host(adapter->pfunction);
  }
  status = sdio_writesb(adapter->pfunction,Addr,data,Count);
  if(status !=0)
  {
  }

  if(rsi_likely(adapter->irq_registered != current))
  {
    rsi_sdio_release_host(adapter->pfunction);
  }
  if (status != 0)
  {
	  printk("%s: Synch Cmd53 write failed %d\n", __func__, status);
  }
  return status;
#endif
} /* End <write_register_multiple> */






/**===========================================================================
 * @fn         int rsi_open_sdbus_handle(PRSI_ADAPTER adapter)
 * @brief      Dummy for linux
 * @Params     Pointer to Driver adapter structure. 
 * Return      None
 */
 int rsi_open_sdbus_handle(PRSI_ADAPTER adapter)
{
  /**Dummy for Linux*/
  return 0;
} /* End <rsi_open_sdbus_handle> */





/**===========================================================================
 * @fn        int remove()
 * @brief     Dummy for linux sdio
 * @Params    None 
 * @Return    Value None
 */
int remove()
{
  /**Dummy for Linux*/
  //FIXME: Kill all the VAP'S
  return 0;
} /* End <remove> */





/**===========================================================================
 * @fn        void claim_device(PRSI_ADAPTER adapter)
 * @brief     This function claims the device.
 * @param     Pointer to driver adapter structure.  
 * @return    VOID. 
 */
void claim_device(PRSI_ADAPTER adapter)
{
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,26))
  struct sdio_func *pfunc = adapter->pfunction;
  sdio_claim_host(pfunc);
  adapter->irq_registered = current;
#endif
} /* End <claim_device> */







/**===========================================================================
 * @fn        void release_device(PRSI_ADAPTER adapter)
 * @brief     This function releases the device.
 * @param     Pointer to driver adapter structure.  
 * @return    VOID. 
 */
void release_device(PRSI_ADAPTER adapter)
{
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,26))
  struct sdio_func *pfunc = adapter->pfunction;
  adapter->irq_registered = NULL;
  sdio_release_host(pfunc);
#endif
} /* End <release_device> */











/**===========================================================================
 * @fn        int32 rsi_sdio_device_interrupt_status(PRSI_ADAPTER Adapter,uint8 fun_num,uint32 address,uint8 *data)
 * @brief     This function registers the device interrupt status.
 * @param     Pointer to Driver adapter structure.  
 * @param     Function Number.  
 * @param     Address of the register.  
 * @param     Pointer to the data that stores the data read.  
 * @return    On success int_SUCCESS else int_FAILURE. 
 */
int32 rsi_sdio_device_interrupt_status(PRSI_ADAPTER Adapter,uint8 fun_num,uint32 address,uint8 *data)
{
#if  KERNEL_VERSION_BTWN_2_6_(18, 22)
  return SDLIB_IssueCMD52(Adapter->pDevice,
      fun_num,
      address,
      data,
      1,
      TRUE
      );
#elif  KERNEL_VERSION_GREATER_THAN_2_6_(26)
  int status=0;
  if (rsi_likely(Adapter->irq_registered != current))
  {
    rsi_sdio_claim_host(Adapter->pfunction);
  }

  if (fun_num == 0)
  {
    *data = sdio_f0_readb(Adapter->pfunction,
        address,
        &status);
  }
  else
  {
    *data =  sdio_readb(Adapter->pfunction,
        address,
        &status);
  } /* End if <condition> */
  if (likely(Adapter->irq_registered != current))
  {
    sdio_release_host(Adapter->pfunction);
  }
  if (status)
  {
    return -1;
  }
  else
  {
    return 0;
  } /* End if <condition> */
#endif
} /* End <rsi_sdio_device_interrupt_status> */







/**===========================================================================
 * @fn        VOID rsi_sdio_claim_host(struct sdio_func *pfunction)
 * @brief     This function exclusively claims a bus before a certain SDIO function.
 * @param     Pointer to sdio_func structure.  
 * @return    VOID. 
 */
#if KERNEL_VERSION_BTWN_2_6_(18, 22)
VOID rsi_sdio_claim_host(PSDFUNCTION pFunction)
#elif KERNEL_VERSION_GREATER_THAN_2_6_(26)
VOID rsi_sdio_claim_host(struct sdio_func *pfunction)
#endif
{
#if KERNEL_VERSION_BTWN_2_6_(18, 22)
  /*No code here*/
#elif KERNEL_VERSION_GREATER_THAN_2_6_(26)
  sdio_claim_host(pfunction);
#endif
  return;
}/* End <rsi_sdio_claim_host> */


static uint8 process_lmac_aggr_pkt(PRSI_ADAPTER adapter, uint32 pkt_len)
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
		payload_len = length + FRAME_DESC_SZ;
		skb = dev_alloc_skb(payload_len);
		skb_put(skb, payload_len);
		memcpy(skb->data, frame_desc_addr + offset, skb->len);
		rsi_queue_tail(&adapter->rx_q, skb);
		index += actual_length;
		pkt_len -= actual_length;
	} while ( pkt_len > 0);
	queue_work(adapter->workqueue, &adapter->defer_work);
	return 0;
}

/**===========================================================================
 * @fn        int32 rsi_sdio_frame_read(PRSI_ADAPTER Adapter)
 * @brief     This function read frames from the SD card.
 * @param     Pointer to driver adapter structure.  
 * @param     Pointer to received packet.  
 * @param     Pointer to length of the received packet.  
 * @return    0 if success else -1. 
 */
int32 rsi_sdio_frame_read(PRSI_ADAPTER Adapter)
{
  int32 status;
  uint32 q_no;
  uint32 pktLen;
  uint16 type;
  uint8 regval  = 0;
  uint8 num_blocks;
  uint32 rcv_pkt_len;
  uint8 *rx_data = NULL;
  status = read_register(Adapter,SDIO_RX_NUM_BLOCKS_REG,0,&regval);
  if(status != 0)
  {
    RSI_DEBUG(RSI_ZONE_SDIO_DBG,"\nReading number of blocks failed");
    return -1;
  }
  num_blocks = regval & 0x1F;
  if(num_blocks == 0)
  {
    RSI_DEBUG(RSI_ZONE_SDIO_DBG,"\n number of blocks received is zero");
    return -1;
  }
  rcv_pkt_len = 256 * num_blocks;
  rx_data = Adapter->DataRcvPacket;
  status = host_intf_read_pkt(Adapter,rx_data, rcv_pkt_len);
  if(status != 0 )
  {
    RSI_DEBUG(RSI_ZONE_SDIO_DBG,"\n EEROR in reading packet");
    return -1;

  }
  process_lmac_aggr_pkt(Adapter, rcv_pkt_len);
  return 0;
}/* End <rsi_sdio_frame_read> */







/**===========================================================================
 * @fn        int16 sdio_read_buffer_full()
 * @brief     This function checks the buffer full conditions.
 * @param     None.  
 * @return    0 if success else -1. 
 */
int16 sdio_read_buffer_full()
{
  uint32 READ_BUFFER_ADDRESS = 0;//FIXME
  uint8 temp;
  PRSI_ADAPTER Adapter =  rsi_getpriv(glbl_net_device);
  read_register(Adapter,SDIO_FUN1_INT_REG,0,&temp);
  if(temp & SDIO_BUFFER_FULL)
  {
    Adapter->BufferFull = true;
    return -1;
  }
  Adapter->BufferFull = false;
  return 0;
}/* End <sdio_read_buffer_full> */







/**===========================================================================
 * @fn        INT32 ack_interrupt(PRSI_ADAPTER Adapter,uint8 INT_BIT)
 * @brief     This function acks the interrupt received.
 * @param     Pointer to the driver adapter structure. 
 * @param     Interrupt bit to write into register. 
 * @return    VOID. 
 */
INT32 ack_interrupt(PRSI_ADAPTER Adapter,uint8 INT_BIT)
{
  int32 status;
  uint8 reg_dmn =1;
  status = write_register(Adapter,reg_dmn,SDIO_FUN1_INTR_CLR_REG|SD_REQUEST_MASTER,&INT_BIT);  
  if(status !=0)
  {
    return status;
  }
  return status;
}/* End <ack_interrupt> */











/**===========================================================================
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
  status= host_intf_write_pkt(Adapter,data_ptr,payload_len,(data_ptr[1] >> 4));//FIXME queue no
#else
  status= host_intf_write_pkt(Adapter,data_ptr_act ,payload_len+216+16,(data_ptr[1] >> 4));//FIXME queue no
#endif
  if(status != 0)
  {
    status = -1;
  }   

  if(data_ptr[2]== RSI_RSP_SOFT_RESET)
  {
    rsi_reinit_card(Adapter->pfunction);
  }
  return status;
}/* End <rsi_execute_cmd> */









/**===========================================================================
 * @fn        int host_intf_read_pkt(PRSI_ADAPTER adapter,uint8 *pkt,uint32 Len)
 * @brief     This function reads the packet from the SD card.
 * @param     Pointer to the driver's private data structure
 * @param     Pointer to the packet data  read from the the device
 * @param     Length of the data to be read from the device.  
 * @return    0 if success else a negative number. 
 */
int host_intf_read_pkt(PRSI_ADAPTER adapter,uint8 *pkt,uint32 Len)
{
  int Status  = 0;
  if (!Len)
  {
    printk("%s: Pkt size is zero\n",__func__);
    return Status;
  }
  /*Reading the actual data*/
  Status = read_register_multiple(adapter,Len,Len,(uint8 *)pkt);

  if (Status != 0)
  {
    printk("%s: Failed to read frame from the card: %d\n",__func__,
          Status);
    return Status;
  }
  return Status;
}/* End <host_intf_read_pkt> */










/**===========================================================================
 * @fn        int host_intf_write_pkt(PRSI_ADAPTER adapter,uint8 *pkt,uint32 Len, uint8 queueno)
 * @brief     This function writes the packet to the device.
 * @param     Pointer to the driver's private data structure
 * @param     Pointer to the data to be written on to the device
 * @param     Length of the data to be written on to the device.  
 * @return    0 if success else a negative number. 
 */
int host_intf_write_pkt(PRSI_ADAPTER adapter,uint8 *pkt,uint32 Len, uint8 queueno)
{
  uint32 block_size    = adapter->TransmitBlockSize;
  uint32 num_blocks,Address,Length;
#ifndef RSI_SDIO_MULTI_BLOCK_SUPPORT
  uint32 ii;
#endif
  int32 status = 0;

  if( !Len  && (queueno == WLAN_TX_D_Q ))
  {
    return -1;
  } /* End if <condition> */
  num_blocks = Len/block_size;
  if (Len % block_size)
  {
    num_blocks++;
  }

  if (num_blocks < 2)
  {
    num_blocks = 2;
  }

  Address = num_blocks * block_size | (queueno << 12);
  Length  = num_blocks * block_size;

#ifdef  RSI_SDIO_MULTI_BLOCK_SUPPORT
  status = write_register_multiple(adapter,
      Address,
      (uint8 *)pkt,
      Length);
  if (status != 0)
  {
	  printk(" Unable to write onto the card \n");
    //FIXME//rsi_abort_handler(adapter);
    return status;
  } /* End if <condition> */
#else
  /* Non multi block read */
  for(ii = 0; ii < num_blocks; ii++)
  {
    if(ii==0)
    {
      status = write_register_multiple(adapter,
          Address,
          (uint8 *)(pkt + (ii*block_size)),
          block_size);
    }
    else
    {
      status = write_register_multiple(adapter,
          (num_blocks*block_size),
          (uint8 *)(pkt + (ii*block_size)),
          block_size);
    } /* End if <condition> */
    if(status != 0)
    {

      //FIXME//rsi_abort_handler(adapter);
      return status;
    } /* End if <condition> */
  } /* End for loop */
#endif

  return status;
}/* End <host_intf_write_pkt> */









/**===========================================================================
 * @fn        int16 rsi_sdio_master_access_msword(uint16 ms_word)
 * @brief     This function set the AHB master access MS word in the SDIO slave registers.
 * @param     Pointer to the driver adapter structure. 
 * @param     ms word need to be initialized.
 * @return    0 if success else a negative number. 
 */
int16 rsi_sdio_master_access_msword(uint16 ms_word)
{
  PRSI_ADAPTER Adapter =  rsi_getpriv(glbl_net_device);
  unsigned char byte;
  uint8 reg_dmn;
  int32 status=RSI_STATUS_SUCCESS;
  reg_dmn = 0; //TA domain
  /* Initialize master address MS word register with given value*/
  byte=(unsigned char*)(ms_word&0x00FF);
//  RSI_DEBUG(RSI_ZONE_INFO,
//      (TEXT("%s: MASTER_ACCESS_MSBYTE:0x%x\n"), __func__,byte));
  status = write_register(Adapter, reg_dmn ,SDIO_MASTER_ACCESS_MSBYTE, &byte);
  if(status != RSI_STATUS_SUCCESS)
  {
    RSI_DEBUG(RSI_ZONE_ERROR,
        (TEXT("%s: fail to access MASTER_ACCESS_MSBYTE\n"), __func__));
    return -1;
  }
  byte=(unsigned char)(ms_word >>8);
//  RSI_DEBUG(RSI_ZONE_INFO,
//      (TEXT("%s:MASTER_ACCESS_LSBYTE:0x%x\n"), __func__,byte));
  status =write_register(Adapter, reg_dmn ,SDIO_MASTER_ACCESS_LSBYTE, &byte);
  if(status != RSI_STATUS_SUCCESS)
  {
    RSI_DEBUG(RSI_ZONE_ERROR,
        (TEXT("%s: fail to access MASTER_ACCESS_LSBYTE\n"), __func__));
    return -1;
  }
  return RSI_STATUS_SUCCESS;
}/* End <rsi_sdio_master_access_msword> */









/**===========================================================================
 * @fn          int16 rsi_mem_rd(uint32 reg_address,uint16 len,uint32 *value)
 * @brief       Reads a register specified by reg_address & stores in value
 * @param       uint32       reg_address:  Address of the register to read
 * @param       uint16       len:          Number of bytes to read. (def: 2 since we have 16 bit regs)
 * @param       uint16*      value:        Variable in which the read value is stored
 * @return      0 if success else a negative number. 
 */
int16 rsi_mem_rd(uint32 reg_address,uint16 len,uint32 *value)
{
  uint32 *data = NULL;
  uint16 ms_addr = 0;
  uint32 align[2] = {};
  uint32 addr_on_bus;
  PRSI_ADAPTER Adapter =  rsi_getpriv(glbl_net_device);
  data = (uint32 *)align_address(&align[1]);
    ms_addr = (reg_address >> 16);
    if (rsi_sdio_master_access_msword(ms_addr)!= RSI_STATUS_SUCCESS)
    {
      RSI_DEBUG(RSI_ZONE_ERROR,
          (TEXT("%s: Unable to set ms word to common reg\n"), __func__));
      return -1;              
    }
    reg_address = reg_address & 0xFFFF;

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
  if(read_register_multiple( Adapter,addr_on_bus | SD_REQUEST_MASTER,4, (uint8 *)data )!= RSI_STATUS_SUCCESS)
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








/**===========================================================================
 * @fn          int16 rsi_mem_wr(uint32 reg_address,uint16 len,uint32 *value)
 * @brief       Writes the given data to the specified register address in the WiFi Module
 * @param       uint32 reg_address  Address of the register to read
 * @param       uint16 len  Number of bytes to read. (def: 2 since we have 16 bit regs)
 * @param       uint16* value Variable in which the read value is stored
 * @return      0 if success else a negative number. 
 *
 */
int16 rsi_mem_wr(unsigned long reg_address,uint16 len,uint32 *value)
{
  uint16 ms_addr = 0;
  PRSI_ADAPTER Adapter =  rsi_getpriv(glbl_net_device);
  unsigned long data1[2];
  unsigned long *data_alligned;
  data_alligned =(unsigned long *)align_address(&data1[1]); 
  if(len == 2) {
    *data_alligned  = ((*value << 16) |(*value & 0xFFFF));
  }
  else if(len == 1) {
    uint32 temp_data;
    temp_data = (*value & 0xFF);
    *data_alligned = ((temp_data << 24) | (temp_data << 16) | (temp_data << 8) | (temp_data));
  }
  else {
    *data_alligned = *value;
  }

  len = 4;

    ms_addr = (reg_address >> 16);
    if(rsi_sdio_master_access_msword(ms_addr)!= RSI_STATUS_SUCCESS)

    {
      RSI_DEBUG(RSI_ZONE_ERROR,
          (TEXT("%s: Unable to set ms word to common reg\n"), __func__));
      return -1;              
    }
    reg_address = reg_address & 0xFFFF;
  if(write_register_multiple(Adapter,
        reg_address |
        SD_REQUEST_MASTER,
        (uint8 *)data_alligned,
        len)!= RSI_STATUS_SUCCESS)
  {
    RSI_DEBUG(RSI_ZONE_ERROR,
        (TEXT("%s: Unable to do AHB reg write\n"), __func__));
    return -1;
  }
  return RSI_STATUS_SUCCESS;
}/* End <rsi_mem_wr> */












/**===========================================================================
 * @fn        VOID rsi_write_cccr_registers(struct sdio_func *pfunction)
 * @brief     This function takes the backup of the CCCR registers.
 * @param     Pointer to sdio_func.  
 * @return    VOID 
 */
VOID rsi_cccr_register_backup(struct sdio_func *pfunction)
{
  int ret = 0;
  uint8 val =0;
  PRSI_ADAPTER Adapter   = rsi_getpriv(glbl_net_device);
  Adapter->io_enable =read_register(Adapter, SDIO_CCCR_IOEx,0, &val);
  Adapter->io_ready =read_register(Adapter, SDIO_CCCR_IORx,0, &val);
  Adapter->int_enable =read_register(Adapter, SDIO_CCCR_IENx,0, &val);
  Adapter->bus_interface_control =read_register(Adapter, SDIO_CCCR_IF,0, &val);
}/* End <rsi_cccr_register_backup> */







/**===========================================================================
 * @fn        int  __devinit rsi_enable_interface(PVOID ptr)
 * @brief     This function takes the writes the backup values of the CCCR registers.
 * @param     Pointer to sdio_func.  
 * @return    VOID 
 */
VOID rsi_write_cccr_registers(struct sdio_func *pfunction)
{
  int ret = 0;
  uint8 val =0;
  PRSI_ADAPTER adapter   = rsi_getpriv(glbl_net_device);
  ret = rsi_cmd52writebyte(pfunction->card, SDIO_CCCR_IOEx,adapter->io_enable);
  msleep(2); 
  ret = rsi_cmd52writebyte(pfunction->card, SDIO_CCCR_IORx,adapter->io_ready);
  msleep(2); 
  ret = rsi_cmd52writebyte(pfunction->card, SDIO_CCCR_IENx, adapter->int_enable);
  msleep(2); 
  ret = rsi_cmd52writebyte(pfunction->card, SDIO_CCCR_IF, adapter->bus_interface_control);
  msleep(2); 

}/* End <rsi_write_cccr_registers> */










/**===========================================================================
 * @fn        VOID rsi_reinit_card(struct sdio_func *pfunction)
 * @brief     This function reinitialises the card
 * @param     Pointer to sdio_func.  
 * @return    0 if success else a negative number. 
 */
VOID rsi_reinit_card(struct sdio_func *pfunction)
{
  int ret = 0;
  uint8 val =0;
  uint8 status;
  PRSI_ADAPTER Adapter   = rsi_getpriv(glbl_net_device);
#if KERNEL_VERSION_BTWN_2_6_(18, 22)

#elif KERNEL_VERSION_GREATER_THAN_EQUALS_2_6_(26)
  rsi_sdio_claim_host(Adapter->pfunction);
  /* Resetting the sdio card to make it ready for the next run */
  rsi_reset_card(Adapter->pfunction);
  status = rsi_setupcard(Adapter);
  if (status != 0)
  {
    RSI_DEBUG(RSI_ZONE_ERROR,(TEXT("%s:Failed to setup card\n"), __func__));
    kfree((uint8 *)Adapter->DataRcvPacket[0]);
    rsi_sdio_release_host(Adapter->pfunction);
    goto out;
  }

#if LINUX_VERSION_CODE == KERNEL_VERSION(2,6,18)
  Adapter->sdio_high_speed_enable = 1;
#endif
  if(rsi_init_host_interface(Adapter)!= 0)
  {
    RSI_DEBUG(RSI_ZONE_ERROR,(TEXT("%s:Failed to init slave regs\n"), __func__));
    rsi_sdio_release_host(Adapter->pfunction);
    goto out;
  }
  /* Release host */
  rsi_sdio_release_host(Adapter->pfunction);
#endif
  return status;
out :
  status = -1;
  return status;
}/* End <rsi_reinit_card> */


