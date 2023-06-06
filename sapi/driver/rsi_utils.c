/*******************************************************************************
* @file  rsi_utils.c
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

/*
  Include files
 */
#include "rsi_driver.h"
#include <stdio.h>
/*
  Global defines
 */
/** @addtogroup DRIVER12
* @{
*/
/*=============================================================================*/
/**
 * @fn           void rsi_uint16_to_2bytes(uint8_t *dBuf, uint16_t val)
 * @brief        Convert uint16 to two byte array. 
 * @param[in]    dBuf - Pointer to buffer to put the data in 
 * @param[in]    val  - Data to convert 
 * @return       void 
 */
void rsi_uint16_to_2bytes(uint8_t *dBuf, uint16_t val)
{
#ifdef ROM_WIRELESS
  ROMAPI_WL->rsi_uint16_to_2bytes(global_cb_p, dBuf, val);
#else
  api_wl->rsi_uint16_to_2bytes(global_cb_p, dBuf, val);
#endif
}

/*=============================================================================*/
/**
 * @fn              void rsi_uint32_to_4bytes(uint8_t *dBuf, uint32_t val)
 * @brief           Convert uint32 to four byte array.
 * @param[in]       dBuf - Pointer to buffer to put the data in  
 * @param[in]       val  - Data to convert  
 * @return          void  
 */

void rsi_uint32_to_4bytes(uint8_t *dBuf, uint32_t val)
{
#ifdef ROM_WIRELESS
  ROMAPI_WL->rsi_uint32_to_4bytes(global_cb_p, dBuf, val);
#else
  api_wl->rsi_uint32_to_4bytes(global_cb_p, dBuf, val);
#endif
}

/*=============================================================================*/
/**
 * @fn          uint16_t rsi_bytes2R_to_uint16(uint8_t *dBuf)    
 * @brief       Convert a 2 byte array to uint16, first byte in array is LSB.     
 * @param[in]   dBuf - Pointer to a buffer to get the data from       
 * @return      Converted 16 bit data  
 */
uint16_t rsi_bytes2R_to_uint16(uint8_t *dBuf)
{
#ifdef ROM_WIRELESS
  return ROMAPI_WL->rsi_bytes2R_to_uint16(global_cb_p, dBuf);
#else
  return api_wl->rsi_bytes2R_to_uint16(global_cb_p, dBuf);
#endif
}

/*=============================================================================*/
/**
 * @fn           uint32_t rsi_bytes4R_to_uint32(uint8_t *dBuf)
 * @brief        Convert a 4 byte array to uint32, first byte in array is LSB.
 * @param[in]    dBuf - Pointer to a buffer to get the data from  
 * @return       Converted 32 bit data
 */

uint32_t rsi_bytes4R_to_uint32(uint8_t *dBuf)
{
#ifdef ROM_WIRELESS
  return ROMAPI_WL->rsi_bytes4R_to_uint32(global_cb_p, dBuf);
#else
  return api_wl->rsi_bytes4R_to_uint32(global_cb_p, dBuf);
#endif
}
/*==============================================*/
/**
 * @fn         int8_t rsi_ascii_hex2num(int8_t ascii_hex_in)
 * @brief      ASCII to HEX conversion. 
 * @param[in]  ascii_hex_in - ASCII HEX input 
 * @return     HEX number  
 */

int8_t rsi_ascii_hex2num(int8_t ascii_hex_in)
{
  if ((ascii_hex_in >= '0') && (ascii_hex_in <= '9'))
    return (ascii_hex_in - '0');
  if ((ascii_hex_in >= 'A') && (ascii_hex_in <= 'F'))
    return (ascii_hex_in - 'A' + 10);
  if ((ascii_hex_in >= 'a') && (ascii_hex_in <= 'f'))
    return (ascii_hex_in - 'a' + 10);

  return RSI_SUCCESS;
}
/*=============================================================================*/
/**
 * @fn          int8 rsi_char_hex2dec(int8_t *cBuf)
 * @brief       Convert given ASCII HEX notation to decimal notation (used for MAC address). 
 * @param[in]   cBuf - ASCII HEX notation string 
 * @return      Integer Value
 */

int8_t rsi_char_hex2dec(int8_t *cBuf)
{
  int8_t k = 0;
  uint8_t i;
  for (i = 0; i < strlen((char *)cBuf); i++) {
    k = ((k * 16) + rsi_ascii_hex2num(cBuf[i]));
  }
  return k;
}

/*=============================================================================*/
/**
 * @fn             uint8_t *rsi_ascii_dev_address_to_6bytes_rev(uint8_t *hex_addr, int8_t *ascii_mac_address)
 * @brief          Convert notation MAC address to a 6-byte HEX address.
 * @param[in]      asciiMacFormatAddress - Source address to convert, must be a null terminated string.  
 * @param[out]     hex_addr              - Converted HEX address.   
 * @return         HEX address 
 */

uint8_t *rsi_ascii_dev_address_to_6bytes_rev(uint8_t *hex_addr, int8_t *ascii_mac_address)
{
  uint8_t i;       // loop counter
  uint8_t cBufPos; // which char in the ASCII representation
  uint8_t byteNum; // which byte in the 32Bithex_address
  int8_t cBuf[6];  // temporary buffer

  byteNum = 5;
  cBufPos = 0;
  for (i = 0; i < strlen((char *)ascii_mac_address); i++) {
    // this will take care of the first 5 octets
    if (ascii_mac_address[i] == ':') {                                 // we are at the end of the address octet
      cBuf[cBufPos]       = 0;                                         // terminate the string
      cBufPos             = 0;                                         // reset for the next char
      hex_addr[byteNum--] = (uint8_t)rsi_char_hex2dec((int8_t *)cBuf); // convert the strint to an integer
    } else {
      cBuf[cBufPos++] = ascii_mac_address[i];
    }
  }
  // handle the last octet						// we are at the end of the string with no .
  cBuf[cBufPos]     = 0x00;                                      // terminate the string
  hex_addr[byteNum] = (uint8_t)rsi_char_hex2dec((int8_t *)cBuf); // convert the strint to an integer

  return hex_addr;
}

/*=============================================================================*/
/**
 * @fn          int8_t hex_to_ascii(uint8_t hex_num)
 * @brief       Hex to ascii conversion.
 * @param[in]   hex_num - HEX number
 * @return      Ascii value for given HEX value	
 */

int8_t hex_to_ascii(uint8_t hex_num)
{
  uint8_t ascii = 0;

  switch (hex_num & 0x0F) {
    case 0:
    case 1:
    case 2:
    case 3:
    case 4:
    case 5:
    case 6:
    case 7:
    case 8:
    case 9:
      ascii = (hex_num & 0x0F) + '0';
      return ascii;

    case 0xa:
    case 0xb:
    case 0xc:
    case 0xd:
    case 0xe:
    case 0xf:
      ascii = (hex_num & 0x0F) - 10 + 'A';
      return ascii;
      //	break;
    default:
      break;
  }

  return ascii;
}

/*=============================================================================*/
/**
 * @fn              int8_t *rsi_6byte_dev_address_to_ascii(uint8_t *ascii_mac_address, uint8_t *hex_addr)
 * @brief           Convert given 6-byte HEX address to ASCII Mac address.
 * @param[in]       hex_addr              - HEX address input. 
 * @param[out]      asciiMacFormatAddress - Converted ASCII mac address is returned here. 
 * @return          Converted ASCII mac address 
 */

uint8_t *rsi_6byte_dev_address_to_ascii(uint8_t *ascii_mac_address, uint8_t *hex_addr)
{
  int8_t i;        // loop counter
  uint8_t cBufPos; // which char in the ASCII representation

  //byteNum = 5;
  cBufPos = 0;
  for (i = 5; i >= 0; i--) {
    ascii_mac_address[cBufPos++] = hex_to_ascii(hex_addr[i] >> 4);
    ascii_mac_address[cBufPos++] = hex_to_ascii(hex_addr[i]);
    if (i != 0) {
      ascii_mac_address[cBufPos++] = ':';
    }
  }
  return ascii_mac_address;
}

/*==============================================*/
/**
 * @fn          uint8_t lmac_crc8_c(uint8_t crc8_din, uint8_t crc8_state, uint8_t end) 
 * @brief       Calculate CRC for a given byte and accumulate CRC.
 * @param[in]   crc8_din   -  CRC byte input  
 * @param[in]   crc8_state - accumulated CRC  
 * @param[in]   end        - last byte CRC  
 * @return      CRC value 
 *                 
 */
///@private
uint8_t lmac_crc8_c(uint8_t crc8_din, uint8_t crc8_state, uint8_t end)
{
  uint8_t din[8];
  uint8_t state[8];
  uint8_t state_c[8];
  uint8_t crc8_out;

  din[0] = ((crc8_din & BIT(7)) >> 7);
  din[1] = ((crc8_din & BIT(6)) >> 6);
  din[2] = ((crc8_din & BIT(5)) >> 5);
  din[3] = ((crc8_din & BIT(4)) >> 4);
  din[4] = ((crc8_din & BIT(3)) >> 3);
  din[5] = ((crc8_din & BIT(2)) >> 2);
  din[6] = ((crc8_din & BIT(1)) >> 1);
  din[7] = ((crc8_din & BIT(0)) >> 0);

  state[0] = ((crc8_state & BIT(0)) >> 0);
  state[1] = ((crc8_state & BIT(1)) >> 1);
  state[2] = ((crc8_state & BIT(2)) >> 2);
  state[3] = ((crc8_state & BIT(3)) >> 3);
  state[4] = ((crc8_state & BIT(4)) >> 4);
  state[5] = ((crc8_state & BIT(5)) >> 5);
  state[6] = ((crc8_state & BIT(6)) >> 6);
  state[7] = ((crc8_state & BIT(7)) >> 7);

  state_c[7] = (state[7] ^ din[7]) ^ (state[6] ^ din[6]) ^ (state[5] ^ din[5]);

  state_c[6] = (state[6] ^ din[6]) ^ (state[5] ^ din[5]) ^ (state[4] ^ din[4]);

  state_c[5] = (state[5] ^ din[5]) ^ (state[4] ^ din[4]) ^ (state[3] ^ din[3]);

  state_c[4] = (state[4] ^ din[4]) ^ (state[3] ^ din[3]) ^ (state[2] ^ din[2]);

  state_c[3] = (state[1] ^ din[1]) ^ (state[2] ^ din[2]) ^ (state[3] ^ din[3]) ^ (state[7] ^ din[7]);

  state_c[2] = (state[0] ^ din[0]) ^ (state[1] ^ din[1]) ^ (state[2] ^ din[2]) ^ (state[6] ^ din[6]);

  state_c[1] = (state[0] ^ din[0]) ^ (state[1] ^ din[1]) ^ (state[6] ^ din[6]);

  state_c[0] = (state[0] ^ din[0]) ^ (state[7] ^ din[7]) ^ (state[6] ^ din[6]);
  if (!end) {
    crc8_out = ((state_c[0] & BIT(0)) << 0) | ((state_c[1] & BIT(0)) << 1) | ((state_c[2] & BIT(0)) << 2)
               | ((state_c[3] & BIT(0)) << 3) | ((state_c[4] & BIT(0)) << 4) | ((state_c[5] & BIT(0)) << 5)
               | ((state_c[6] & BIT(0)) << 6) | ((state_c[7] & BIT(0)) << 7);
  } else {
    crc8_out = ((state_c[7] & BIT(0)) << 0) | ((state_c[6] & BIT(0)) << 1) | ((state_c[5] & BIT(0)) << 2)
               | ((state_c[4] & BIT(0)) << 3) | ((state_c[3] & BIT(0)) << 4) | ((state_c[2] & BIT(0)) << 5);

    crc8_out = ~crc8_out;
    crc8_out &= 0x3f;
  }
  return (crc8_out);
}

/*==============================================*/
/**
 * @fn         uint8_t multicast_mac_hash(uint8_t *mac)
 * @brief      Calculate 6-bit hash value for given MAC address 
 * @param[in]  mac - pointer to MAC address    
 * @return     6-bit Hash value
 *                 
 */
/// @private
uint8_t multicast_mac_hash(uint8_t *mac)
{
  uint8_t i, crc = 0xff;
  for (i = 0; i < 6; i++) {
    crc = lmac_crc8_c(mac[i], crc, ((i == 5) ? 1 : 0));
  }
  return (crc);
}

/*=========================================================================*/
/**
 * @fn          uint8_t convert_lower_case_to_upper_case(uint8_t lwrcase)
 * @brief       Convert the given lower-case character to upper case. 
 * @param[in]   lwrcase - Lower case character to convert   
 * @return      Converted Upper case character  
 */

uint8_t convert_lower_case_to_upper_case(uint8_t lwrcase)
{
  uint8_t digit = (lwrcase >= 'a' && lwrcase <= 'f') ? (lwrcase - 0x20) : lwrcase;
  return (digit >= 'A' && digit <= 'F') ? digit - 0x37 : digit - '0';
}

/*=========================================================================*/
/**
 * @fn          void string2array(uint8_t *dst, uint8_t *src, uint32_t length)
 * @brief       Convert the given string to array. 
 * @param[in]   dst    -  Pointer to destination array  
 * @param[in]   src    - Pointer to source string  
 * @param[in]   length - Length of the string 
 * @return      Void   
 */

void string2array(uint8_t *dst, uint8_t *src, uint32_t length)
{
  uint32_t i = 0, j = 0;
  for (i = 0, j = 0; i < (length * 2) && j < length; i += 2, j++)
    if (src[i] && src[i + 1]) {
      dst[j] = ((uint16_t)convert_lower_case_to_upper_case(src[i])) * 16;
      dst[j] += convert_lower_case_to_upper_case(src[i + 1]);
    } else {
      dst[j] = 0;
    }
}
/*=========================================================================*/
/**
 * @fn         uint8_t *rsi_itoa(uint32_t val, uint8_t *str)
 * @brief      Convert integer value into null-terminated string and stores it. 
 * @param[in]  val - Value to be converted to a string  
 * @param[in]  str - Array in memory where to store the resulting null-terminated string  
 * @return     Null-terminated string  
 */

uint8_t *rsi_itoa(uint32_t val, uint8_t *str)
{
  int16_t ii = 0, jj = 0;
  uint8_t tmp[10];
  if (val == 0) {
    // if value is zero then handling
    str[jj] = '0';
    jj++;
    str[jj] = '\0';
    return str;
  }

  while (val) {
    tmp[ii] = '0' + (val % 10);
    val /= 10;
    ii++;
  }

  for (jj = 0, ii--; ii >= 0; ii--, jj++) {
    str[jj] = tmp[ii];
  }
  str[jj] = '\0';

  return str;
}

/*=========================================================================*/
/**
 * @fn          int32_t rsi_atoi(const int8_t *str)
 * @brief       Convert string to an integer. 
 * @param[in]   str - String to be converted to integer 
 * @return      Converted Integer  
 */
int32_t rsi_atoi(const int8_t *str)
{
  int32_t res              = 0;
  int32_t i                = 0;
  uint32_t negative_number = 0;

  if (str[i] == '-') {
    negative_number = 1;
    i++;
  }
  for (; (str[i] >= '0') && (str[i] <= '9'); ++i)
    res = res * 10 + str[i] - '0';

  if (negative_number) {
    res *= -1;
  }
  return res;
}

/*=============================================================================*/
/**
 * @fn         int8_t asciihex_2_num(int8_t ascii_hex_in)
 * @brief      ASCII to hex conversion.
 * @param[in]  ascii_hex_in - ASCII hex input  
 * @return     HEX number  
 */
int8_t asciihex_2_num(int8_t ascii_hex_in)
{
  if ((ascii_hex_in >= '0') && (ascii_hex_in <= '9'))
    return (ascii_hex_in - '0');
  if ((ascii_hex_in >= 'A') && (ascii_hex_in <= 'F'))
    return (ascii_hex_in - 'A' + 10);
  if ((ascii_hex_in >= 'a') && (ascii_hex_in <= 'f'))
    return (ascii_hex_in - 'a' + 10);

  return RSI_SUCCESS;
}

/*=============================================================================*/
/**
 * @fn          int8_t rsi_charhex_2_dec(int8_t *cBuf)
 * @brief       Convert given ASCII HEX notation to decimal notation (used for mac address). 
 * @param[in]   cBuf - ASCII HEX notation string. 
 * @return      Integer value    
 */
int8_t rsi_charhex_2_dec(int8_t *cBuf)
{
  int8_t k = 0;
  uint8_t i;
  for (i = 0; i < strlen((char *)cBuf); i++) {
    k = ((k * 16) + asciihex_2_num(cBuf[i]));
  }
  return k;
}

/*=============================================================================*/
/**
 * @fn              void rsi_ascii_mac_address_to_6bytes(uint8_t *hexAddr, int8_t *asciiMacAddress)
 * @brief           Convert ASCII notation MAC address to a 6-byte HEX address. 
 * @param[in]       asciiMacFormatAddress - Source address to convert, must be a null terminated string.  
 * @param[out]      hexAddr               - Converted HEX address is returned here.
 * @return          Void 
 */
void rsi_ascii_mac_address_to_6bytes(uint8_t *hexAddr, int8_t *asciiMacAddress)
{
  uint8_t i;       // loop counter
  uint8_t cBufPos; // which char in the ASCII representation
  uint8_t byteNum; // which byte in the 32BitHexAddress
  int8_t cBuf[6];  // temporary buffer

  byteNum = 0;
  cBufPos = 0;
  for (i = 0; i < strlen((char *)asciiMacAddress); i++) {
    // this will take care of the first 5 octets
    if (asciiMacAddress[i] == ':') {                                   // we are at the end of the address octet
      cBuf[cBufPos]      = 0;                                          // terminate the string
      cBufPos            = 0;                                          // reset for the next char
      hexAddr[byteNum++] = (uint8_t)rsi_charhex_2_dec((int8_t *)cBuf); // convert the strint to an integer
    } else {
      cBuf[cBufPos++] = asciiMacAddress[i];
    }
  }
  // handle the last octet                  // we are at the end of the string with no .
  cBuf[cBufPos]    = 0x00;                                       // terminate the string
  hexAddr[byteNum] = (uint8_t)rsi_charhex_2_dec((int8_t *)cBuf); // convert the strint to an integer
}

/*=============================================================================*/
/**
 * @fn            void rsi_ascii_dot_address_to_4bytes(uint8_t *hexAddr, int8_t *asciiDotAddress)
 * @brief         Convert ASCII notation network address to 4-byte HEX address. 
 * @param[in]     asciiDotAddress - Source address to convert, must be a null terminated string.
 * @param[out]    hexAddr         - Output value is passed back in the 4-byte HEX Address.
 * @return        Void  
 */
void rsi_ascii_dot_address_to_4bytes(uint8_t *hexAddr, int8_t *asciiDotAddress)
{
  uint8_t i;
  // loop counter
  uint8_t cBufPos;
  // which char in the ASCII representation
  uint8_t byteNum;
  // which byte in the 32BitHexAddress
  int8_t cBuf[4];
  // character buffer

  byteNum = 0;
  cBufPos = 0;
  for (i = 0; i < strlen((char *)asciiDotAddress); i++) {
    // this will take care of the first 3 octets
    if (asciiDotAddress[i] == '.') {
      // we are at the end of the address octet
      cBuf[cBufPos] = 0;
      // terminate the string
      cBufPos = 0;
      // reset for the next char
      hexAddr[byteNum++] = (uint8_t)rsi_atoi(cBuf);
      // convert the strint to an integer
    } else {
      cBuf[cBufPos++] = asciiDotAddress[i];
    }
  }
  // handle the last octet
  // we are at the end of the string with no .
  cBuf[cBufPos] = 0x00;
  // terminate the string
  // convert the strint to an integer
  hexAddr[byteNum] = (uint8_t)rsi_atoi(cBuf);
}
/*=============================================================================*/
/**
 * @fn         uint64_t ip_to_reverse_hex(char *ip)
 * @brief      Convert IP address to reverse HEX format.  
 * @param[in]  ip - IP address to convert. 
 * @return     IP address in reverse HEX format  
 */
uint64_t ip_to_reverse_hex(char *ip)
{
  uint32_t ip1, ip2, ip3, ip4;
  uint64_t ip_hex;
  uint32_t status;

  status = sscanf(ip, "%lu.%lu.%lu.%lu", &ip1, &ip2, &ip3, &ip4);
  if (status != 4) {
    return 0x00000000; // Problem if we actually pass 0.0.0.0
  }

  ip_hex = (uint64_t)ip1;
  ip_hex |= (uint64_t)(ip2 << 8);
  ip_hex |= (uint64_t)(ip3 << 16);
  ip_hex |= (uint64_t)(ip4 << 24);

  return ip_hex;
}
/*=============================================================================*/
/**
 * @fn         uint32_t rsi_ntohl(uint32_t a)
 * @brief      Converts the unsigned integer from network byte order to host byte order.
 * @param[in]  a - Unsigned integer to convert.
 * @return     Unsigned integer in host byte order
 */
// network to host long
uint32_t rsi_ntohl(uint32_t a)
{
  return ((((a)&0xff000000) >> 24) | (((a)&0x00ff0000) >> 8) | (((a)&0x0000ff00) << 8) | (((a)&0x000000ff) << 24));
}
/*=============================================================================*/
/**
 * @fn         uint8_t ascii_to_dec(uint8_t *num)
 * @brief      Convert ASCII to Decimal Value.  
 * @param[in]  num - ASCII input
 * @return     dec_val - value after conversion 
 */
uint8_t ascii_to_dec(uint8_t *num)
{
  uint8_t dec_val = 0, i = 0;
  while (num[i] != '\0') {
    dec_val = dec_val * 10 + (num[i] - '0');
    i++;
  }
  return dec_val;
}

#if defined(FLOAT_PRINT_BYPASS)
/*=============================================================================*/
/**
 * @fn         void rsi_reverse(char *str, int len)
 * @brief      Reverses the given string.
 * @param[in]  str - string to be reversed
 * @param[in]  len - the length of the given string
 * @return     void
 */

void rsi_reverse(char *str, int len)
{
  int i = 0, j = len - 1, temp;
  while (i < j) {
    temp   = str[i];
    str[i] = str[j];
    str[j] = temp;
    i++;
    j--;
  }
}

/*=============================================================================*/
/**
 * @fn         int rsi_intToStr(int x, char str[], int d)
 * @brief      converts the integer into a string.
 * @param[in]  x - given interger to be made into a string
 * @param[in]  str - the string that stores the final string corresponding to the integer
 * @param[in]  d - the number of digits required in the output, should be less than 6. If d is more than the number of digits in x, then 0s are added at the beginning.
 * @return     the lenght of the string
 */
int rsi_intToStr(int x, char str[], int d)
{
  int i = 0;
  while (x) {
    str[i++] = (x % 10) + '0';
    x        = x / 10;
  }

  // If number of digits required is more, then
  // add 0s at the beginning
  while (i < d)
    str[i++] = '0';

  rsi_reverse(str, i);
  str[i] = '\0';
  return i;
}

/*=============================================================================*/
/**
 * @fn         int rsi_intToStr(int x, char str[], int d)
 * @brief      Converts a floating-point/double number to a string.
 * @param[in]  res - the string in which the result must be stored
 * @param[in]  n - the float number that needs to be converted into a string
 * @param[in]  afterpoint - the number of points after the decimal that needs to be printed, should be less than 6.
 * @return     void
 */
void rsi_ftoa(char *res, float n, int afterpoint)
{
  float fpart;
  // Extract integer part
  int ipart = (int)n;

  // Extract floating part
  fpart = n - (float)ipart;

  // convert integer part to string
  int i = rsi_intToStr(ipart, res, 0);

  // check for display option after point
  if (afterpoint != 0) {
    res[i]   = '.'; // add dot
    int mult = 1;
    for (int i = 0; i < afterpoint; i++)
      mult *= 10;
    fpart = fpart * mult; // supports 6 digits for now.

    rsi_intToStr((int)fpart, res + i + 1, afterpoint);
  }
}
#endif

/** @} */
