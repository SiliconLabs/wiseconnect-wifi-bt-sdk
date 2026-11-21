/*******************************************************************************
* @file  rsi_bootup_config.c
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
 * @file     rsi_bootup_config.c
 * @version  1.0
 * @date     2018-July-04
 *
 *
 *
 * @brief BOOTUP: Bootup configuration file
 *
 * @section Description
 * This file contains the Bootup functionality.
 * @functions Defined:
 * rsi_waitfor_boardready
 * rsi_select_option
 * rsi_upgrade_fw
 */

/**
 * Includes */
#include "rsi_driver.h"
#include "rsi_linux_app_init.h"
rsi_linux_driver_cb_t rsi_linux_driver_app_cb;


/*==============================================*/
/**
 * @fn          int16 rsi_boot_insn(uint8 type, uint16 *data)
 * @brief       Sends boot instructions to WiFi module
 * @param[in]   uint8 type, type of the insruction to perform
 * @param[in]   uint32 *data, pointer to data which is to be read/write
 * @param[out]  none
 * @return      errCode
 *              < 0  = Command issued failure/Invalid command
 *                0  = SUCCESS
 *              > 0  = Read value
 * @section description
 * This API is used to send boot instructions to WiFi module.
 */


int16_t rsi_boot_insn(uint8_t type, uint16_t *data)
{
  rsi_linux_driver_cb_t *driver_cbPtr = &rsi_linux_driver_app_cb;

  int16_t            retval = 0;
  uint32_t           size = 0;

  switch (type)
  {
    case REG_READ:
    {
      size = 4;
      retval = rsi_ioctl_send_req(driver_cbPtr->ioctl_sd,(uint8_t *)data, size, OID_WSC_BOOT_READ);
    }
    break;

    case REG_WRITE:
    {

      size = 4;
      retval = rsi_ioctl_send_req(driver_cbPtr->ioctl_sd,(uint8_t *)data, size, OID_WSC_BOOT_WRITE);
    }
    break;
    case PING_WRITE:
    {
      size = (4 * 1024);
      retval = rsi_ioctl_send_req(driver_cbPtr->ioctl_sd,(uint8_t *)data, size, OID_WSC_BOOT_PING_WRITE);
    }
    break;

    case PONG_WRITE:
    {
      size = (4 * 1024);
      retval = rsi_ioctl_send_req(driver_cbPtr->ioctl_sd,(uint8_t *)data, size, OID_WSC_BOOT_PONG_WRITE);
    }
    break;
  }

  return retval;
}


/*==============================================*/
/**
 * @fn          int16 rsi_waitfor_boardready(void)
 * @brief       Waits to receive board ready from WiFi module
 * @param[in]   none
 * @param[out]  none
 * @return      errCode
 *              0  = SUCCESS
 *              < 0 = Failure
 *              -3 = Board ready not received
 *              -4 = Bootup options last configuration not saved
 *              -5 = Bootup options checksum failed
 *              -6 = Bootloader version mismatch
 * @section description
 * This API is used to check board ready from WiFi module.
 */
int16_t rsi_waitfor_boardready(void)
{
  int16_t retval = 0;
  uint16_t read_value = 0;

  retval = rsi_boot_insn(REG_READ, &read_value);

  if(retval < 0)
  {
    return retval;
  }
	if(read_value == 0)
	{
		 return RSI_ERROR_IN_OS_OPERATION;
	}
  if ((read_value & 0xFF00) == (HOST_INTERACT_REG_VALID_READ & 0xFF00))
  {
    if((read_value & 0xFF) == BOOTUP_OPTIONS_LAST_CONFIG_NOT_SAVED)
    {
#ifdef RSI_DEBUG_PRINT
      RSI_DPRINT(RSI_PL3,"BOOTUP OPTIOINS LAST CONFIGURATION NOT SAVED\n");
#endif
      return RSI_ERROR_BOOTUP_OPTIONS_NOT_SAVED;
    }
    else if((read_value & 0xFF) == BOOTUP_OPTIONS_CHECKSUM_FAIL)
    {
#ifdef RSI_DEBUG_PRINT
      RSI_DPRINT(RSI_PL3,"BOOTUP OPTIONS CHECKSUM FAIL\n");
#endif
      return RSI_ERROR_BOOTUP_OPTIONS_CHECKSUM_FAIL;
    }
#if BOOTLOADER_VERSION_CHECK
		else if((read_value & 0xFF) == BOOTLOADER_VERSION)
    {
#ifdef RSI_DEBUG_PRINT
      RSI_DPRINT(RSI_PL3,"BOOTLOADER VERSION CORRECT\n");
#endif
    }
    else
    {
#ifdef RSI_DEBUG_PRINT
      RSI_DPRINT(RSI_PL3,"BOOTLOADER VERSION NOT MATCHING\n");
#endif

      return RSI_ERROR_BOOTLOADER_VERSION_NOT_MATCHING;
    }
#endif

#ifdef RSI_DEBUG_PRINT
    RSI_DPRINT(RSI_PL3,"RECIEVED BOARD READY\n");
#endif
		return RSI_SUCCESS;
  }

#ifdef RSI_DEBUG_PRINT
      RSI_DPRINT(RSI_PL3,"WAITING FOR BOARD READY\n");
#endif
  return RSI_ERROR_WAITING_FOR_BOARD_READY;
}

/*==============================================*/
/**
 * @fn          int16 rsi_select_option(uint8 cmd)
 * @brief       Sends cmd to select option to load or update configuration
 * @param[in]   uint8 cmd, type of configuration to be saved
 * @param[out]  none
 * @return      errCode
                < 0 = Command issue failed
 *              0  = SUCCESS
 * @section description
 * This API is used to send firmware load request to WiFi module or update default configurations.
 */
int16_t rsi_select_option(uint8_t cmd)
{
  uint16_t   boot_cmd = 0;
  int16_t    retval = 0;
  uint16_t   read_value = 0;
  uint8_t    image_number  = 0;
  volatile int32_t  loop_counter = 0;

  boot_cmd = HOST_INTERACT_REG_VALID | cmd;
  if(cmd == CHECK_NWP_INTEGRITY)
  {
    boot_cmd &= 0xF0FF;
    boot_cmd |= (image_number << 8);
  }
  retval   = rsi_boot_insn(REG_WRITE, &boot_cmd);
  if(retval < 0)
  {
    return retval;
  }

  if((cmd != LOAD_NWP_FW) && (cmd != LOAD_DEFAULT_NWP_FW_ACTIVE_LOW) && (cmd !=  RSI_JUMP_TO_PC))
  {
    RSI_RESET_LOOP_COUNTER(loop_counter);
    RSI_WHILE_LOOP(loop_counter, RSI_LOOP_COUNT_SELECT_OPTION)
    {
      retval = rsi_boot_insn(REG_READ, &read_value);
      if(retval < 0)
      {
        return retval;
      }
      if(cmd == CHECK_NWP_INTEGRITY)
      {
        if((read_value & 0xFF) == CHECKSUM_SUCCESS)
        {
#ifdef RSI_DEBUG_PRINT
          RSI_DPRINT(RSI_PL3,"CHECKSUM SUCCESS\n");
#endif
        }
        else if(read_value == CHECKSUM_FAILURE)
        {
#ifdef RSI_DEBUG_PRINT
          RSI_DPRINT(RSI_PL3,"CHECKSUM FAIL\n");
#endif
        }
        else if(read_value == CHECKSUM_INVALID_ADDRESS)
        {
#ifdef RSI_DEBUG_PRINT
          RSI_DPRINT(RSI_PL3,"Invalid Address \n");
#endif
        }
      }
      if (read_value == (HOST_INTERACT_REG_VALID | cmd))
      {
        break;
      }
    }
    RSI_CHECK_LOOP_COUNTER(loop_counter, RSI_LOOP_COUNT_SELECT_OPTION);
  }
  return retval;
}







/*==============================================*/
/**
 * @fn             int32 rsi_ioctl_send_req(int32 sockfd,
 *                                          uint8 *buf,
 *                                          int32 buf_len,
 *                                          int32 req_type)
 * @brief          update the info to kernel
 * @param[in]      int32 sockfd, socket descriptor
 * @param[in]      uint8 *buf, pointer to the buffer
 * @param[in]      int32 buf_len, length of the buffer
 * @param[in]      int32 req_type, request type
 * @param[out]     none
 * @return         errCode
 *                 0  = SUCCESS
 *                 else Failure
 * @section description
 * This function is used to send some info which is required
 * by the kernel netdevice driver.
 */


int32_t rsi_ioctl_send_req(int32_t sockfd, uint8_t *buf, int32_t buf_len, int32_t req_type)
{


  struct iwreq iwr;
  int32_t val;


#ifdef RSI_DEBUG_PRINT
  RSI_DPRINT(RSI_PL14,"\nrsi_ioctl_send_req:\n");
#endif
  memset(&iwr, 0, sizeof(iwr));
  strncpy(iwr.ifr_ifrn.ifrn_name, "rsi_wlan0", IFNAMSIZ);
  //strncpy(iwr.ifr_name, "rsi_wlan0", IFNAMSIZ);
  iwr.u.data.pointer= buf;
  iwr.u.data.length= buf_len;

  while((val=ioctl(sockfd, req_type, &iwr))<0)
  {
#ifdef RSI_DEBUG_PRINT
    RSI_DPRINT(RSI_PL0,"unable to issue ioctl for %lx request %ld reason %d\n",req_type,val,(errno));
#endif
    return val;
  }
  return 0;
}
/*==============================================*/
/**
 * @fn          int32_t rsi_get_ram_dump(uint32_t addr,uint16_t length, uint8_t *buffer)
 * @brief       This API is used to get ram dump
 * @param[in]   uint32_t addr
 * @param[in]   uint16_t length
 * @param[out]  uint8_t  buffer
 * @param[out]  none
 * @return      errCode
 *                0  = SUCCESS
 *              > 0  = Failure
 * @section description
 * This API is used to get ram dump.
 */
int32_t rsi_get_ram_dump(uint32_t addr,uint16_t length, uint8_t *buffer)
{
  rsi_linux_driver_cb_t *driver_cbPtr = &rsi_linux_driver_app_cb;
  int16_t    retval;

  if(buffer == NULL)
   {
     return -1;
   }

  if(!length || length > 4096 )
  {
    return -2;
  }

  buffer[0]= addr & 0xFF;
  buffer[1]= (addr >> 8) & 0xFF;
  buffer[2]= (addr >> 16) & 0xFF;
  buffer[3]= (addr >> 24) & 0xFF;
  retval = rsi_ioctl_send_req(driver_cbPtr->ioctl_sd,buffer, length, OID_MASTER_READ);

  return retval;
}
