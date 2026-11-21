/*******************************************************************************
* @file  rsi_hal_mcu_ioports.c
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
 * @file       rsi_hal_mcu_ioports.c
 * @version    1.0
 * @date       2018-MAR-01
 *
 *
 *
 * @brief Functions to control IO pins of the microcontroller
 *  
 * @section Description
 * This file contains API to control different pins of the microcontroller 
 * which interface with the module and other components related to the module. 
 *
 */


/**
 * Includes
 */
#include "rsi_global.h"
#include "rsi_hal.h"
#if (LINUX_VERSION_CODE == KERNEL_VERSION(2,6,18))
#include <asm/arch/gpio.h>
#else
#include <linux/gpio.h>
#endif

/**
 * Global Variales
 */



/*====================================================*/
/**
 * @fn          void rsi_module_reset(uint8 tf)
 * @brief       Sets or clears the reset pin on the wifi module
 * @param[in]   uint8 tf, true or false, true sets reset, false clears reset
 * @param[out]  none
 * @return      none
 * @description This HAL API is used to set or clear the active-low reset pin of the Wi-Fi module.
 */
void rsi_module_reset(uint8 tf)
{  
  if (tf == RSI_TRUE) {
  }
  else {
  }


}

/*====================================================*/
/**
 * @fn           void rsi_module_power(uint8 tf)
 * @brief        Turns on or off the power to the wifi module
 * @param[in]    uint8 tf, true or false, true turns on power, false turns off power
 * @param[out]   none
 * @return       none 
 * @description This HAL API is used to turn on or off the power to the Wi-Fi module.
 */
void rsi_module_power(uint8 tf)
{  
  if (tf == RSI_TRUE) {
  }
  else {
  }

 }

/*===========================================================*/
/**
 * @fn          int16 rsi_module_power_cycle(void)
 * @brief       MODULE POWER ON, Power cycles the module
 * @param[in]   none
 * @param[out]  none
 * @return      errCode
 *             -1 = Error
 *              0  = OK
 * @description  This API is used to power cycle the module. 
 * This API is valid only if there is a power gate, external to the module, 
 * which is controlling the power to the module using a GPIO signal of the MCU.
 */
int16 rsi_module_power_cycle(void)
{
   return 0;
}

/*===========================================================*/
/**
 * @fn          void config_gpio_output(uint8 value)
 * @brief       Configures gpio pin in output mode,with a value
 * @param[in]   uint8 value, value to be configures
 * @param[out]  none
 * @return      errCode
 *             -1 = Error
 *              0  = OK
 * @description This API is used to configure host gpio pin in output mode. 
 */
void config_gpio_output(uint8 value)
{
#ifdef SAM9_G45
  at91_set_gpio_output(SPI_WAKEUP_REQ_GPIO, value);
#endif
  
}

/*===========================================================*/
/**
 * @fn           void config_gpio_input(void)
 * @brief        Configures gpio pin in input mode
 * @param[in]    none
 * @param[out]   none
 * @return       errCode
 *               -1 = Error
 *                0  = OK
 * @description This API is used to configure host gpio pin in input mode. 
 */
void config_gpio_input(void)
{
#ifdef SAM9_G45
  at91_set_gpio_input(SPI_WAKEUP_STAT_GPIO, 0);
#endif
}

/*===========================================================*/
/**
 * @fn          uint8 get_gpio_value()
 * @brief       Gets the gpio value
 * @param[in]   none
 * @param[out]  none
 * @return      errCode
 *              -1 = Error
 *              0  = OK
 * @description This API is used to gets the host gpio value. 
 */
uint8 get_gpio_value()
{
#ifdef SAM9_G45
    return at91_get_gpio_value(SPI_WAKEUP_STAT_GPIO) ;
#else
    return 0;
#endif
}

uint8 get_spi_intr_gpio_value()
{
#ifdef SAM9_G45
    return at91_get_gpio_value(SPI_INTR_GPIO_PIN) ;
#else
    return 1;
#endif
}
