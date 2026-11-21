/*******************************************************************************
* @file  rsi_send_boot_insn.c
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
 * @file       rsi_send_boot_insn.c
 * @version    1.0
 * @date       2014-Apr-13
 *
 *
 *
 * @brief SEND BOOT INSN: send boot instructions to WiFi module 
 *
 * @section Description
 * This file contains boot instructions exchanges with WiFi module.
 *
 *
 */

/**
 * Includes
 */
#include "rsi_global.h"

int16 rsi_secure_ping_pong_wr(uint32 ping_pong, uint8 *src_addr, uint16 size_param);
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
#if (defined(RSI_USB_INTERFACE) || defined(RSI_SPI_INTERFACE) || defined(RSI_SDIO_INTERFACE))
extern int16 rsi_mem_rd(uint32 reg_address, uint16 len, uint8 *value);
extern int16 rsi_mem_wr(unsigned long reg_address, uint16 len, uint8 *value);

int16 rsi_boot_insn(uint8 type, uint16 *data)
{
  int16   retval = 0;
  uint16  local = 0;
  uint32  j = 0;
  uint32  cmd = 0; 
  uint16  read_data = 0;
  volatile int32  loop_counter = 0;
#ifdef RSI_DEBUG_PRINT
  RSI_DPRINT(RSI_PL3,"\nBootInsn\n");
#endif

  switch(type)
  {
    case REG_READ:
#if ((RSI_SECURE_BOOT == ENABLE) && (RSI_INTERFACE == RSI_USB))
      retval = rsi_secure_reg_rd(2,(uint8 *)&read_data);
#else
       retval = rsi_mem_rd(HOST_INTF_REG_OUT,2,(uint8 *)&read_data);
#endif
      *data = read_data;
      break;

    case REG_WRITE:
#if ((RSI_SECURE_BOOT == ENABLE) && (RSI_INTERFACE == RSI_USB))
      retval = rsi_secure_reg_wr((uint8 *)data,2);
#else
      retval = rsi_mem_wr(HOST_INTF_REG_IN,2, (uint8 *)data);
#endif
      break;

    case PING_WRITE:
#if RSI_SECURE_BOOT
#ifndef RSI_OFFSET_BASED
      retval = rsi_secure_ping_pong_wr(0,(uint8 *)data,4096);
#else
      retval = rsi_secure_ping_pong_wr_offset(0,(uint8 *)data,4096);
#endif
      local = 0xab49;
#if (RSI_INTERFACE == RSI_USB)
      retval = rsi_secure_reg_wr((uint8 *)&local,2);
#else
      retval = rsi_mem_wr(HOST_INTF_REG_IN, 2, (uint8 *)&local);
#endif	
#else
      for (j = 0; j<2048; j++){
        retval = rsi_mem_wr(0x18000 + (j*2), 2, (uint8 *)((uint32)data + (j*2)));
        if(retval < 0)
        {
          return retval;
        }
      }

      local = 0xab49;
      retval = rsi_mem_wr(HOST_INTF_REG_IN, 2, (uint8 *)&local);
#endif	
      break;

    case PONG_WRITE:
#if RSI_SECURE_BOOT
#ifndef RSI_OFFSET_BASED
      retval = rsi_secure_ping_pong_wr(1,(uint8 *)data,4096);
#else
      retval = rsi_secure_ping_pong_wr_offset(1,(uint8 *)data,4096);
#endif  
      local = 0xab4f;
#if (RSI_INTERFACE == RSI_USB)
      retval = rsi_secure_reg_wr((uint8 *)&local,2);
#else
      retval = rsi_mem_wr(HOST_INTF_REG_IN, 2, (uint8 *)&local);
#endif

#else
      for (j = 0; j<2048; j++){
        retval = rsi_mem_wr(0x19000 + (j*2), 2 ,(uint8 *)((uint32)data + (j*2)));
        if(retval < 0)
        {
          return retval;
        }
      }
      // Perform the write operation
      local = 0xab4f;
      retval = rsi_mem_wr(HOST_INTF_REG_IN, 2, (uint8 *)&local);
#endif
      // Perform the write operation
      break;

    case BURN_NWP_FW:
      cmd = BURN_NWP_FW | HOST_INTERACT_REG_VALID;
#if ((RSI_SECURE_BOOT == ENABLE) && (RSI_INTERFACE == RSI_USB))
      retval = rsi_secure_reg_wr((uint8 *)&cmd,2);
      if(retval < 0)
      {
        return retval;
      }
#else
      retval = rsi_mem_wr(HOST_INTF_REG_IN, 2, (uint8 *)&cmd);
      if(retval < 0)
      {
        return retval;
      }
#endif
      RSI_RESET_LOOP_COUNTER(loop_counter); 
      RSI_WHILE_LOOP(loop_counter, RSI_LOOP_COUNT_UPGRADE_IMAGE)
      {
#if ((RSI_SECURE_BOOT == ENABLE) && (RSI_INTERFACE == RSI_USB))
      retval = rsi_secure_reg_rd(2,(uint8 *)&read_data);
        if(retval < 0)
        {
          return retval;
        }
#else
        retval = rsi_mem_rd(HOST_INTF_REG_OUT, 2, (uint8 *)&read_data);
        if(retval < 0)
        {
          return retval;
        }
#endif
        if (read_data == (SEND_RPS_FILE | HOST_INTERACT_REG_VALID)){
          break;
        }
      }
      RSI_CHECK_LOOP_COUNTER(loop_counter, RSI_LOOP_COUNT_UPGRADE_IMAGE);
      break;

    case LOAD_NWP_FW:
      cmd = LOAD_NWP_FW | HOST_INTERACT_REG_VALID;
#if ((RSI_SECURE_BOOT == ENABLE) && (RSI_INTERFACE == RSI_USB))
      retval = rsi_secure_reg_wr((uint8 *)&cmd,2);
#else
      retval = rsi_mem_wr(HOST_INTF_REG_IN, 2, (uint8 *)&cmd);
#endif
      break;
    case LOAD_DEFAULT_NWP_FW_ACTIVE_LOW:
      cmd = LOAD_DEFAULT_NWP_FW_ACTIVE_LOW | HOST_INTERACT_REG_VALID;
#if ((RSI_SECURE_BOOT == ENABLE) && (RSI_INTERFACE == RSI_USB))
      retval = rsi_secure_reg_wr((uint8 *)&cmd,2);
#else
      retval = rsi_mem_wr(HOST_INTF_REG_IN, 2, (uint8 *)&cmd);
#endif
      break;
    case RSI_UPGRADE_BL:
      cmd = RSI_UPGRADE_BL | HOST_INTERACT_REG_VALID;
#if ((RSI_SECURE_BOOT == ENABLE) && (RSI_INTERFACE == RSI_USB))
      retval = rsi_secure_reg_wr((uint8 *)&cmd,2);
#else      
      retval = rsi_mem_wr(HOST_INTF_REG_IN, 2, (uint8 *)&cmd);
#endif      
      if(retval < 0)
      {
        return retval;
      }
      RSI_RESET_LOOP_COUNTER(loop_counter);
      RSI_WHILE_LOOP(loop_counter, RSI_LOOP_COUNT_UPGRADE_IMAGE)
      {
#if ((RSI_SECURE_BOOT == ENABLE) && (RSI_INTERFACE == RSI_USB))
      retval = rsi_secure_reg_rd(2,(uint8 *)&read_data);
#else
      retval = rsi_mem_rd(HOST_INTF_REG_OUT,  2, (uint8 *)&read_data);
#endif
        if(retval < 0)
        {
          return retval;
        }
        if (read_data == (SEND_RPS_FILE | HOST_INTERACT_REG_VALID)){
          break;
        }
      }
      RSI_CHECK_LOOP_COUNTER(loop_counter, RSI_LOOP_COUNT_UPGRADE_IMAGE);
      break;
    default:
      retval = -2;
      break;
  }
  return retval;
}
/*==============================================*/
/**
 * @fn          int16_t rsi_boot_req()
 * @brief       Sends boot instructions to WiFi module 
 * @param[in]   uint32 *data 
 * @param[in]   uint32 addr
 * @param[out]  none
 * @return      errCode
 *                0  = SUCCESS
 *              > 0  = Read value
 * @section description 
 * This API is used to send boot instructions to WiFi module.
 */
int16 rsi_boot_req(uint32 *data, uint32 addr)
{
  int16   retval = -1;
  if(data != NULL)
  {
    retval =  rsi_mem_rd(addr,4,(uint32 *)data);
  }
  return retval;
}

#endif
