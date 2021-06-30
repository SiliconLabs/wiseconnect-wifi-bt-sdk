/*******************************************************************************
* @file  main.c
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

//! Memory length for driver
#define BT_GLOBAL_BUFF_LEN 15000

//! BT Coex mode
#define RSI_BT_BLE_MODE 9

//! Parameter to run forever loop
#define RSI_FOREVER 1

/*=======================================================================*/
//!    Powersave configurations
/*=======================================================================*/
#define ENABLE_POWER_SAVE 0 //! Set to 1 for powersave mode

#if ENABLE_POWER_SAVE
//! Power Save Profile Mode
#define PSP_MODE RSI_SLEEP_MODE_2
//! Power Save Profile type
#define PSP_TYPE RSI_MAX_PSP
#endif

//! Memory to initialize driver
uint8_t global_buf[BT_GLOBAL_BUFF_LEN];

//! Function prototypes
extern void rsi_bt_app_init(void);
extern void rsi_ble_app_init(void);
extern void rsi_bt_app_task(void);
extern void rsi_ble_app_task(void);

void switch_proto_async(uint16_t mode, uint8_t *bt_disabled_status)
{
  UNUSED_PARAMETER(mode); //This statement is added only to resolve compilation warning, value is unchanged
  UNUSED_PARAMETER(
    bt_disabled_status); //This statement is added only to resolve compilation warning, value is unchanged
}

int32_t rsi_bt_ble_app(void)
{
  int32_t status = RSI_SUCCESS;

  //! Driver initialization
  status = rsi_driver_init(global_buf, BT_GLOBAL_BUFF_LEN);
  if ((status < 0) || (status > BT_GLOBAL_BUFF_LEN)) {
    return status;
  }

  //! Redpine module intialisation
  status = rsi_device_init(LOAD_NWP_FW);
  if (status != RSI_SUCCESS) {
    return status;
  }

  //! WiSeConnect initialization
  status = rsi_wireless_init(RSI_WLAN_CLIENT_MODE, RSI_BT_BLE_MODE);
  if (status != RSI_SUCCESS) {
    return status;
  }

  status = rsi_switch_proto(1, NULL);
  if (status != RSI_SUCCESS) {
    //return status;
  }

  //! BT initialization
  rsi_bt_app_init();

  //! BLE initialization
  rsi_ble_app_init();

#if ENABLE_POWER_SAVE
  //! initiating power save in BLE mode
  LOG_PRINT("\r\n Initiate module in to power save \r\n");
  status = rsi_bt_power_save_profile(PSP_MODE, PSP_TYPE);
  if (status != RSI_SUCCESS) {
    return status;
  }
  LOG_PRINT("\r\n Module is in power save \r\n");
#endif

  while (RSI_FOREVER) {
    //! BLE application tasks
    rsi_bt_app_task();

    //! BLE application tasks
    rsi_ble_app_task();

    //! wireless driver tasks
    rsi_wireless_driver_task();
  }
}

//! Forever in wireless driver task
void main_loop()
{
  while (RSI_FOREVER) {
    rsi_wireless_driver_task();
  }
}

//! main funtion definition
int main(void)
{
  int32_t status;

  //! Call BT BLE application
  status = rsi_bt_ble_app();

  //! Application main loop
  main_loop();

  return status;
}
