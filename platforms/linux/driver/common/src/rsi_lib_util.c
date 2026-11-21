/*******************************************************************************
* @file  rsi_lib_util.c
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
 * @file       rsi_lib_util.c
 * @version    2.7
 * @date       2012-Sep-26
 *
 *
 *
 * @brief UTIL: Generic function utils such as bytes2Touint16 which are not tied to anything.
 *
 * @section Description
 * This file implements misc utilities/functions.
 *
 */


/**
 * Include files
 */
#include "rsi_global.h"


/**
 * Global defines
 */


/*=============================================================================*/
/**
 * @fn              void rsi_uint16_to_2bytes(uint8 *dBuf, uint16 val)
 * @brief           Convert uint16 to two byte array
 * @param[in]       uint8 *dBuf,pointer to buffer to put the data in
 * @param[in]       uint16 data to convert
 * @param[out]      none
 * @return          none
 */
void rsi_uint16_to_2bytes(uint8 *dBuf, uint16 val)
{
#ifdef RSI_LITTLE_ENDIAN
  dBuf[0] = val & 0x00ff;
  dBuf[1] = (val >> 8) & 0x00ff;
#else
  dBuf[1] = val & 0x00ff;
  dBuf[0] = (val >> 8) & 0x00ff;
#endif
}

/*=============================================================================*/
/**
 * @fn              void rsi_uint32_to_4bytes(uint8 *dBuf, uint32 val)
 * @brief           Convert uint32 to four byte array
 * @param[in]       uint8 *dBuf,pointer to the buffer to put the data in
 * @param[in]       uint32 data to convert
 * @param[out]      none
 * @return          none
 */
void rsi_uint32_to_4bytes(uint8 *dBuf, uint32 val)
{
#ifdef RSI_LITTLE_ENDIAN
  dBuf[0] = val & 0x000000ff;
  dBuf[1] = (val >> 8) & 0x000000ff;
  dBuf[2] = (val >> 16) & 0x000000ff;
  dBuf[3] = (val >> 24) & 0x000000ff;
#else
  dBuf[3] = val & 0x000000ff;
  dBuf[2] = (val >> 8) & 0x000000ff;
  dBuf[1] = (val >> 16) & 0x000000ff;
  dBuf[0] = (val >> 24) & 0x000000ff;
#endif
}

/*=============================================================================*/
/**
 * @fn              uint16 rsi_bytes2R_to_uint16(uint8 *dBuf)
 * @brief           Convert a 2 byte array to uint16, first byte in array is LSB
 * @param[in]       uint8 *dBuf,pointer to a buffer to get the data from
 * @param[out]      none
 * @return          uint16, converted data
 */
uint16 rsi_bytes2R_to_uint16(uint8 *dBuf)
{
  uint16        val;    
#ifdef RSI_LITTLE_ENDIAN
  val = dBuf[1];
  val <<= 8;
  val |= dBuf[0] & 0x000000ff;
#else
  val = dBuf[0];
  val <<= 8;
  val |= dBuf[1] & 0x000000ff;
#endif
  return val;
}

/*=============================================================================*/
/**
 * @fn           uint32 rsi_bytes4R_to_uint32(uint8 *dBuf)
 * @brief        Convert a 4 byte array to uint32, first byte in array is LSB
 * @param[in]    uint8 *dBuf,pointer to buffer to get the data from
 * @param[out]   none
 * @return       uint32, converted data
 */
uint32 rsi_bytes4R_to_uint32(uint8 *dBuf)
{
   uint32            val;            //! the 32-bit value to return

#ifdef RSI_LITTLE_ENDIAN
   val = dBuf[3];
   val <<= 8;
   val |= dBuf[2] & 0x000000ff;
   val <<= 8;
   val |= dBuf[1] & 0x000000ff;
   val <<= 8;
   val |= dBuf[0] & 0x000000ff;
#else
   val = dBuf[0];
   val <<= 8;
   val |= dBuf[1] & 0x000000ff;
   val <<= 8;
   val |= dBuf[2] & 0x000000ff;
   val <<= 8;
   val |= dBuf[3] & 0x000000ff;
#endif

   return val;
}


