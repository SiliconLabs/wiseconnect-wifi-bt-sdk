/*******************************************************************************
* @file  rsi_hal_mcu_timer.c
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

#include "rsi_driver.h"

#define RSI_HAL_MS_TO_TICKS		1

static volatile uint32_t _dwTickCount; //systick cout variable

/** @addtogroup HAL
* @{
*/
/*===================================================*/
/**
 * @fn  int32_t rsi_timer_start(uint8_t timer_node,
                        uint8_t mode,
                        uint8_t type,
                        uint32_t duration,
                        void (*rsi_timer_expiry_handler)(void))
 * @brief        Initialize the timer and start the timer
 * @param[in]    timer_node                 - timer node to be configured.
 * @param[in]    mode                       - mode of the timer \n
 *                                            0 - Micro seconds mode \n
 *                                            1 - Milli seconds mode
 * @param[in]    type                       - type of  the timer \n
 *                                            0 - single shot type \n
 *                                            1 - periodic type
 * @param[in]    duration                   - timer duration 
 * @param[in]    rsi_timer_expiry_handler() - call back function to handle timer interrupt
 * @return       0              - Success  \n
 *               Non-Zero Value - Failure
 */

int32_t rsi_timer_start(uint8_t timer_node,
                        uint8_t mode,
                        uint8_t type,
                        uint32_t duration,
                        void (*rsi_timer_expiry_handler)(void))
{

  // Initialise the timer

  // register the call back

  // Start timer

  return 0;
}

/*===================================================*/
/**
 * @fn           int32_t rsi_timer_stop(uint8_t timer_no)
 * @brief        Stop the timer
 * @param[in]    timer_node - timer node to stop
 * @return       0          - Success  \n
 *               Non-Zero Value - Failure
 */

int32_t rsi_timer_stop(uint8_t timer_node)
{

  // Stop the timer

  return 0;
}

/*===================================================*/
/**
 * @fn           uint32_t rsi_timer_read(uint8_t timer_node)
 * @brief        Read the timer
 * @param[in]    timer_node - timer node to read
 * @return       timer value
 */

uint32_t rsi_timer_read(uint8_t timer_node)
{

  volatile uint32_t timer_val = 0;

  // read the timer and return timer value

  return timer_val;
}

/*===================================================*/
/**
 * @fn           void rsi_delay_us(uint32_t delay_us)
 * @brief        Create delay in micro seconds
 * @param[in]    delay_us - timer delay in micro seconds
 * @return       void 
 */

void rsi_delay_us(uint32_t delay_us)
{

  // call the API for delay in micro seconds

  return;
}
/** @} */
#ifdef RSI_M4_INTERFACE

extern void SysTick_Handler(void);

void SysTick_Handler(void)
{
  _dwTickCount++;
}
uint32_t GetTickCount(void)
{
  return _dwTickCount; // gets the tick count from systic ISR
}
#endif

/** @addtogroup HAL
* @{
*/
/*===================================================*/
/**
 * @fn           void rsi_delay_ms(uint32_t delay_ms)
 * @brief        Create delay in milli seconds
 * @param[in]    delay_ms - timer delay in milli seconds
 * @return       void 
 */

void rsi_delay_ms(uint32_t delay_ms)
{
  uint32_t start;
  if (delay_ms == 0)
    return;
  start = rsi_hal_gettickcount();
  do {
  } while (rsi_hal_gettickcount() - start < delay_ms);

  return;
}

/*===================================================*/
/**
 * @fn          uint32_t rsi_hal_gettickcount(void)
 * @brief       Read the timer tick count value in milliseconds
 * @param[in]   void 
 * @return      tick value
 */

uint32_t rsi_hal_gettickcount(void)
{
#ifdef LINUX_PLATFORM
  // Define your API to get the tick count delay in milli seconds from systic ISR and return the resultant value
  struct rsi_timeval tv1;
  gettimeofday(&tv1, NULL);
  return (tv1.tv_sec * 1000 + tv1.tv_usec / 1000);
#endif
}

/*==============================================*/
/**
 * @fn          uint32_t rsi_ms_to_tick(uint32_t timeout_ms)
 * @brief       Converting ms to os ticks
 * @param[in]   timeout_ms - timeout in ms
 * @return      uint32_t
 */
uint32_t rsi_ms_to_tick(uint32_t timeout_ms)
{
  return (timeout_ms * RSI_HAL_MS_TO_TICKS);
}
/** @} */
