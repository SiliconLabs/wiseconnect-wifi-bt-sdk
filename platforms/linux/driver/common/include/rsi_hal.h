/*******************************************************************************
* @file  rsi_hal.h
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
 * @file       rsi_hal.h
 * @version    2.2.0.0
 * @date       2011-May-30
 *
 *
 *
 *
 * @section Description
 * This file contains the function definition prototypes for HAL
 *
 *
 */


#ifndef _RSIHAL_H_
#define _RSIHAL_H_


/**
 * INCLUDES
 */
#include "rsi_global.h"

#define SPI_INTR_GPIO_PIN 17
/**
 * DEFINES
 */
#define RSI_SPI_SEND_BYTE(A)   // to send 1 byte over SPI interface
#define RSI_SPI_SEND_4BYTE(A)  // to send 4 bytes over SPI interface
#define RSI_SPI_READ_BYTE(B)   // to receive 1 byte over SPI interface
#define RSI_SPI_READ_4BYTE(B)  // to receive 4 byte over SPI interface
/**
 * Function Prototypes
 */
void  rsi_irq_start(void);
void  rsi_irq_enable(void);
void  rsi_irq_disable(void);
void  rsi_irq_clear_pending(void);
uint8 rsi_irq_status(void);
void  rsi_module_power(uint8 tf);
int16 rsi_spi_send(uint8 *ptrBuf, uint16 bufLen,uint8 *valBuf,uint8 mode);
int16 rsi_spi_recv(uint8 *ptrBuf, uint16 bufLen,uint8 mode);
void  rsi_delay_ms(uint16 delay);
void  rsi_delay_us(uint16 delay);
#endif
