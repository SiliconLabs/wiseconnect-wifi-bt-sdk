/*******************************************************************************
* @file  rsi_ble_long_read.c
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
/*************************************************************************
 *
 */

/*================================================================================
 * @brief : This file contains example application for BLE Long Read
 * @section Description :
 * This application demonstrates how a GATT client device accesses a GATT server
 * device for long read, means when user wants to read more than MTU (minimum of
 * local and remote devices MTU's) size of data.
 * Silabs module acts as a GATT client/server (based on user configuration) and
 * explains reads/writes.
 =================================================================================*/

/**
 * Include files
 * */

//! BLE include files to refer BLE APIs
#include <rsi_ble_apis.h>
#include <rsi_ble_config.h>
#include <rsi_common_apis.h>
#include <rsi_bt_common.h>

#include <stdio.h>
#include <string.h>

#ifdef RSI_M4_INTERFACE
#include "rsi_board.h"
#include "rsi_rtc.h"
#include "rsi_driver.h"
#include "rsi_chip.h"
#include "rsi_wisemcu_hardware_setup.h"
#include "rsi_m4.h"
#include "rsi_ps_ram_func.h"
#include "rsi_ds_timer.h"
#endif
#include "rsi_driver.h"
//! Memory length for driver
#define BT_GLOBAL_BUFF_LEN 15000

//! Memory to initialize driver
uint8_t global_buf[BT_GLOBAL_BUFF_LEN];

#define BLE_ATT_REC_SIZE 500
#define NO_OF_VAL_ATT    5

#define SERVER 0
#define CLIENT 1

#define GATT_ROLE SERVER

//! BLE attribute service types uuid values
#define RSI_BLE_CHAR_SERV_UUID   0x2803
#define RSI_BLE_CLIENT_CHAR_UUID 0x2902

//! BLE characteristic service uuid
#define RSI_BLE_NEW_SERVICE_UUID 0xAABB
#define RSI_BLE_ATTRIBUTE_1_UUID 0x1AA1
//! local device name

#define RSI_BLE_APP_GATT_TEST "LONG_READ_TEST"

//! MTU size for the link
#define RSI_BLE_MTU_SIZE 100

//! immediate alert service uuid
#define RSI_BLE_NEW_CLIENT_SERVICE_UUID 0x180F
//! Alert level characteristic
#define RSI_BLE_CLIENT_ATTRIBUTE_1_UUID 0x2A19

#define RSI_BLE_MAX_DATA_LEN 20

//! attribute properties
#define RSI_BLE_ATT_PROPERTY_READ   0x02
#define RSI_BLE_ATT_PROPERTY_WRITE  0x08
#define RSI_BLE_ATT_PROPERTY_NOTIFY 0x10

//! Configuration bitmap for attributes
#define RSI_BLE_ATT_MAINTAIN_IN_HOST BIT(0)
#define RSI_BLE_ATT_SECURITY_ENABLE  BIT(1)

#define RSI_BLE_ATT_CONFIG_BITMAP (RSI_BLE_ATT_MAINTAIN_IN_HOST)

//! application event list
#define RSI_APP_EVENT_ADV_REPORT              0x00
#define RSI_BLE_CONN_EVENT                    0x01
#define RSI_BLE_DISCONN_EVENT                 0x02
#define RSI_BLE_GATT_WRITE_EVENT              0x03
#define RSI_BLE_READ_REQ_EVENT                0x04
#define RSI_BLE_MTU_EVENT                     0x05
#define RSI_BLE_GATT_PROFILE_RESP_EVENT       0x06
#define RSI_BLE_GATT_CHAR_SERVICES_RESP_EVENT 0x07

//! Address type of the device to connect
#define RSI_BLE_DEV_ADDR_TYPE LE_PUBLIC_ADDRESS

//! Address of the device to connect
#define RSI_BLE_DEV_ADDR "00:23:A7:80:70:B9"

//! Remote Device Name to connect
#define RSI_REMOTE_DEVICE_NAME "SILABS_DEV"

typedef struct rsi_ble_att_list_s {
  uuid_t char_uuid;
  uint16_t handle;
  uint16_t len;
  uint16_t max_value_len;
  uint8_t char_val_prop;
  void *value;
} rsi_ble_att_list_t;

typedef struct rsi_ble_s {
  uint8_t DATA[BLE_ATT_REC_SIZE];
  uint16_t DATA_ix;
  uint16_t att_rec_list_count;
  rsi_ble_att_list_t att_rec_list[NO_OF_VAL_ATT];
} rsi_ble_t;

rsi_ble_t att_list;

uint16_t mtu_size;

#ifdef RSI_WITH_OS
//! BLE task stack size
#define RSI_BT_TASK_STACK_SIZE 1000

//! BT task priority
#define RSI_BT_TASK_PRIORITY 1

//! Number of packet to send or receive
#define NUMBER_OF_PACKETS 1000

//! Wireless driver task priority
#define RSI_DRIVER_TASK_PRIORITY 2

//! Wireless driver task stack size
#define RSI_DRIVER_TASK_STACK_SIZE 3000

void rsi_wireless_driver_task(void);

#endif
#define ENABLE_POWER_SAVE 0 //! Set to 1 for powersave mode

#if ENABLE_POWER_SAVE
//! Power Save Profile Mode
#define PSP_MODE RSI_SLEEP_MODE_2
//! Power Save Profile type
#define PSP_TYPE RSI_MAX_PSP
#endif
//! global parameters list
static uint32_t ble_app_event_map;
static uint8_t remote_addr_type = 0;
static uint8_t remote_name[31];
static uint8_t remote_dev_addr[18]   = { 0 };
static uint8_t remote_dev_bd_addr[6] = { 0 };
static uint8_t device_found          = 0;
static rsi_ble_event_conn_status_t conn_event_to_app;
static rsi_ble_event_disconnect_t disconn_event_to_app;
static rsi_ble_event_write_t app_ble_write_event;
static uint16_t rsi_ble_att1_val_hndl;
static rsi_ble_read_req_t app_ble_read_event;
static rsi_ble_event_mtu_t app_ble_mtu_event;
#if (GATT_ROLE == CLIENT)
static profile_descriptors_t rsi_ble_service;
static rsi_ble_resp_char_services_t char_servs;
#endif
uint8_t str_remote_address[18] = { '\0' };
rsi_semaphore_handle_t ble_main_task_sem;
/**
 * @fn         rsi_ble_only_Trigger_M4_Sleep
 * @brief      Keeps the M4 In the Sleep
 * @param[in]  none
 * @return    none.
 * @section description
 * This function is used to trigger sleep in the M4 and in the case of the retention submitting the buffer valid
 * to the TA for the rx packets.
 */
#if M4_POWERSAVE_ENABLE
void rsi_ble_only_Trigger_M4_Sleep(void)
{
  /* Configure Wakeup-Source */
  RSI_PS_SetWkpSources(WIRELESS_BASED_WAKEUP);
  /* sets the priority of an Wireless wakeup interrupt. */
  NVIC_SetPriority(WIRELESS_WAKEUP_IRQHandler, WIRELESS_WAKEUP_IRQ_PRI);
  NVIC_EnableIRQ(WIRELESS_WAKEUP_IRQHandler);

#ifndef FLASH_BASED_EXECUTION_ENABLE
  /* LDOSOC Default Mode needs to be disabled */
  RSI_PS_LdoSocDefaultModeDisable();

  /* bypass_ldorf_ctrl needs to be enabled */
  RSI_PS_BypassLdoRfEnable();

  RSI_PS_FlashLdoDisable();

  /* Configure RAM Usage and Retention Size */
  RSI_WISEMCU_ConfigRamRetention(WISEMCU_48KB_RAM_IN_USE, WISEMCU_RETAIN_DEFAULT_RAM_DURING_SLEEP);

  /* Trigger M4 Sleep */
  RSI_WISEMCU_TriggerSleep(SLEEP_WITH_RETENTION,
                           DISABLE_LF_MODE,
                           0,
                           (uint32_t)RSI_PS_RestoreCpuContext,
                           0,
                           RSI_WAKEUP_WITH_RETENTION_WO_ULPSS_RAM);

#else

#ifdef COMMON_FLASH_EN
  M4SS_P2P_INTR_SET_REG &= ~BIT(3);
#endif
  /* Configure RAM Usage and Retention Size */
  RSI_WISEMCU_ConfigRamRetention(WISEMCU_192KB_RAM_IN_USE, WISEMCU_RETAIN_DEFAULT_RAM_DURING_SLEEP);

  RSI_WISEMCU_TriggerSleep(SLEEP_WITH_RETENTION,
                           DISABLE_LF_MODE,
                           WKP_RAM_USAGE_LOCATION,
                           (uint32_t)RSI_PS_RestoreCpuContext,
                           IVT_OFFSET_ADDR,
                           RSI_WAKEUP_FROM_FLASH_MODE);
#endif
#ifdef RSI_WITH_OS
  /*  Setup the systick timer */
  vPortSetupTimerInterrupt();
#endif
#ifdef DEBUG_UART
  fpuInit();
  DEBUGINIT();
#endif
}
#endif
#if 0 // This function is not used, To avoid compilation warning this funtion is added in #if 0
/*==============================================*/  
/**
 * @fn         rsi_ble_fill_128bit_uuid  
 * @brief      this function is used to form 128bit uuid of apple device from 32 bit uuid.
 * @param[in]  none.
 * @return     int32_t
 *             0  =  success
 *             !0 = failure
 * @section description
 * This function is used at application to create new service.
 */
static void rsi_ble_fill_128bit_uuid(uint32_t uuid_32bit, uuid_t *serv_uuid)
{
  uint8_t ix;
  serv_uuid->size = 16;
  rsi_uint32_to_4bytes((uint8_t *)&serv_uuid->val.val128.data1, uuid_32bit);
  rsi_uint16_to_2bytes((uint8_t *)&serv_uuid->val.val128.data2, 0x00);
  rsi_uint16_to_2bytes((uint8_t *)&serv_uuid->val.val128.data3, 0x1000);
  rsi_uint16_to_2bytes(&serv_uuid->val.val128.data4[0], 0x8000);
  for (ix = 0; ix < 6; ix++) {
    serv_uuid->val.val128.data4[2] = 0x26;
    serv_uuid->val.val128.data4[3] = 0x00;
    serv_uuid->val.val128.data4[4] = 0x91;
    serv_uuid->val.val128.data4[5] = 0x52;
    serv_uuid->val.val128.data4[6] = 0x76;
    serv_uuid->val.val128.data4[7] = 0xBB;
  }

  return;
}
#endif
void print_data_pkt(unsigned char *input_buf, uint16_t buf_len)
{
  uint16_t ix;

  LOG_PRINT("buf_len: %d\r\n", buf_len);
  for (ix = 0; ix < buf_len; ix++) {
    if (ix % 16 == 0) {
      LOG_PRINT("\r\n");
    } else if (ix % 8 == 0) {
      LOG_PRINT("\t\t");
    }
    LOG_PRINT(" 0x%02X ", input_buf[ix]);
  }
  LOG_PRINT("\r\n");

  return;
}
/*==============================================*/
/**
 * @fn         rsi_ble_add_char_serv_att
 * @brief      this function is used to add characteristic service attribute..
 * @param[in]  serv_handler, service handler.
 * @param[in]  handle, characteristic service attribute handle.
 * @param[in]  val_prop, characteristic value property.
 * @param[in]  att_val_handle, characteristic value handle
 * @param[in]  att_val_uuid, characteristic value uuid
 * @return     none.
 * @section description
 * This function is used at application to add characteristic attribute
 */
static void rsi_ble_add_char_serv_att(void *serv_handler,
                                      uint16_t handle,
                                      uint8_t val_prop,
                                      uint16_t att_val_handle,
                                      uuid_t att_val_uuid)
{
  rsi_ble_req_add_att_t new_att = { 0 };

  //! preparing the attribute service structure
  new_att.serv_handler       = serv_handler;
  new_att.handle             = handle;
  new_att.att_uuid.size      = 2;
  new_att.att_uuid.val.val16 = RSI_BLE_CHAR_SERV_UUID;
  new_att.property           = RSI_BLE_ATT_PROPERTY_READ;

  //! preparing the characteristic attribute value
  new_att.data_len = 6;
  new_att.data[0]  = val_prop;
  rsi_uint16_to_2bytes(&new_att.data[2], att_val_handle);
  rsi_uint16_to_2bytes(&new_att.data[4], att_val_uuid.val.val16);

  //! Add attribute to the service
  rsi_ble_add_attribute(&new_att);

  return;
}
/*==============================================*/
/**
 * @fn         rsi_gatt_add_attribute_to_list
 * @brief      This function is used to store characteristic service attribute.
 * @param[in]  p_val, pointer to homekit structure
 * @param[in]  handle, characteristic service attribute handle.
 * @param[in]  data_len, characteristic value length
 * @param[in]  data, characteristic value pointer
 * @param[in]  uuid, characteristic value uuid
 * @return     none.
 * @section description
 * This function is used to store all attribute records  
 */
void rsi_gatt_add_attribute_to_list(rsi_ble_t *p_val,
                                    uint16_t handle,
                                    uint16_t data_len,
                                    uint8_t *data,
                                    uuid_t uuid,
                                    uint8_t char_prop)
{
  if ((p_val->DATA_ix + data_len) >= BLE_ATT_REC_SIZE) {
    LOG_PRINT("no data memory for att rec values");
    return;
  }

  p_val->att_rec_list[p_val->att_rec_list_count].char_uuid     = uuid;
  p_val->att_rec_list[p_val->att_rec_list_count].handle        = handle;
  p_val->att_rec_list[p_val->att_rec_list_count].len           = data_len;
  p_val->att_rec_list[p_val->att_rec_list_count].max_value_len = data_len;
  p_val->att_rec_list[p_val->att_rec_list_count].char_val_prop = char_prop;
  memcpy(p_val->DATA + p_val->DATA_ix, data, data_len);
  p_val->att_rec_list[p_val->att_rec_list_count].value = p_val->DATA + p_val->DATA_ix;
  p_val->att_rec_list_count++;
  p_val->DATA_ix += p_val->att_rec_list[p_val->att_rec_list_count].max_value_len;

  return;
}
/*==============================================*/
/**
 * @fn         rsi_gatt_get_attribute_from_list
 * @brief      This function is used to retrieve attribute from list based on handle.
 * @param[in]  p_val, pointer to characteristic structure
 * @param[in]  handle, characteristic service attribute handle.
 * @return     pointer to the attribute
 * @section description
 * This function is used to store all attribute records
 */
rsi_ble_att_list_t *rsi_gatt_get_attribute_from_list(rsi_ble_t *p_val, uint16_t handle)
{
  uint16_t i;
  for (i = 0; i < p_val->att_rec_list_count; i++) {
    if (p_val->att_rec_list[i].handle == handle) {
      return &(p_val->att_rec_list[i]);
    }
  }
  return 0;
}
/*==============================================*/
/**
 * @fn         rsi_ble_add_char_val_att
 * @brief      this function is used to add characteristic value attribute.
 * @param[in]  serv_handler, new service handler.
 * @param[in]  handle, characteristic value attribute handle.
 * @param[in]  att_type_uuid, attribute uuid value.
 * @param[in]  val_prop, characteristic value property.
 * @param[in]  data, characteristic value data pointer.
 * @param[in]  data_len, characteristic value length.
 * @return     none.
 * @section description
 * This function is used at application to create new service.
 */

static void rsi_ble_add_char_val_att(void *serv_handler,
                                     uint16_t handle,
                                     uuid_t att_type_uuid,
                                     uint8_t val_prop,
                                     uint8_t *data,
                                     uint8_t data_len,
                                     uint8_t config_bitmap)
{
  rsi_ble_req_add_att_t new_att = { 0 };

  //! preparing the attributes
  new_att.serv_handler  = serv_handler;
  new_att.handle        = handle;
  new_att.config_bitmap = config_bitmap;
  memcpy(&new_att.att_uuid, &att_type_uuid, sizeof(uuid_t));
  new_att.property = val_prop;

  //! preparing the attribute value
  if (data != NULL)
    memcpy(new_att.data, data, data_len);

  new_att.data_len = data_len;
  //! add attribute to the service
  rsi_ble_add_attribute(&new_att);

  if (((config_bitmap & RSI_BLE_ATT_MAINTAIN_IN_HOST) == 1) || (data_len > 20)) {
    if (data != NULL)
      rsi_gatt_add_attribute_to_list(&att_list, handle, data_len, data, att_type_uuid, val_prop);
  }

  //! check the attribute property with notification
  if (val_prop & RSI_BLE_ATT_PROPERTY_NOTIFY) {
    //! if notification property supports then we need to add client characteristic service.

    //! preparing the client characteristic attribute & values
    memset(&new_att, 0, sizeof(rsi_ble_req_add_att_t));
    new_att.serv_handler       = serv_handler;
    new_att.handle             = handle + 1;
    new_att.att_uuid.size      = 2;
    new_att.att_uuid.val.val16 = RSI_BLE_CLIENT_CHAR_UUID;
    new_att.property           = RSI_BLE_ATT_PROPERTY_READ | RSI_BLE_ATT_PROPERTY_WRITE;
    new_att.data_len           = 2;

    //! add attribute to the service
    rsi_ble_add_attribute(&new_att);
  }

  return;
}

/*==============================================*/
/**
 * @fn         rsi_ble_add_simple_chat_serv
 * @brief      this function is used to add new servcie i.e.., simple chat service.
 * @param[in]  none.
 * @return     int32_t
 *             0  =  success
 *             !0 = failure
 * @section description
 * This function is used at application to create new service.
 */

static uint32_t rsi_ble_add_simple_chat_serv(void)
{
  uuid_t new_uuid                       = { 0 };
  rsi_ble_resp_add_serv_t new_serv_resp = { 0 };
  uint8_t data1[100]                    = { 1, 0 };

  //! adding new service
  new_uuid.size      = 2;
  new_uuid.val.val16 = RSI_BLE_NEW_SERVICE_UUID;
  rsi_ble_add_service(new_uuid, &new_serv_resp);

  //! adding characteristic service attribute to the service
  new_uuid.size      = 2;
  new_uuid.val.val16 = RSI_BLE_ATTRIBUTE_1_UUID;
  rsi_ble_add_char_serv_att(new_serv_resp.serv_handler,
                            new_serv_resp.start_handle + 1,
                            RSI_BLE_ATT_PROPERTY_WRITE | RSI_BLE_ATT_PROPERTY_READ | RSI_BLE_ATT_PROPERTY_NOTIFY,
                            new_serv_resp.start_handle + 2,
                            new_uuid);

  //! adding characteristic value attribute to the service
  rsi_ble_att1_val_hndl = new_serv_resp.start_handle + 2;
  new_uuid.size         = 2;
  new_uuid.val.val16    = RSI_BLE_ATTRIBUTE_1_UUID;
  rsi_ble_add_char_val_att(new_serv_resp.serv_handler,
                           new_serv_resp.start_handle + 2,
                           new_uuid,
                           RSI_BLE_ATT_PROPERTY_WRITE | RSI_BLE_ATT_PROPERTY_READ | RSI_BLE_ATT_PROPERTY_NOTIFY,
                           data1,
                           sizeof(data1),
                           RSI_BLE_ATT_CONFIG_BITMAP);

  return 0;
}

/*==============================================*/
/**
 * @fn         rsi_ble_app_init_events
 * @brief      initializes the event parameter.
 * @param[in]  none.
 * @return     none.
 * @section description
 * This function is used during BLE initialization.
 */
static void rsi_ble_app_init_events()
{
  ble_app_event_map = 0;
  return;
}

/*==============================================*/
/**
 * @fn         rsi_ble_app_set_event
 * @brief      set the specific event.
 * @param[in]  event_num, specific event number.
 * @return     none.
 * @section description
 * This function is used to set/raise the specific event.
 */
static void rsi_ble_app_set_event(uint32_t event_num)
{
  if (event_num < 32) {
    ble_app_event_map |= BIT(event_num);
  }
  rsi_semaphore_post(&ble_main_task_sem);
  return;
}

/*==============================================*/
/**
 * @fn         rsi_ble_app_clear_event
 * @brief      clears the specific event.
 * @param[in]  event_num, specific event number.
 * @return     none.
 * @section description
 * This function is used to clear the specific event.
 */
static void rsi_ble_app_clear_event(uint32_t event_num)
{
  if (event_num < 32) {
    ble_app_event_map &= ~BIT(event_num);
  }
  return;
}

/*==============================================*/
/**
 * @fn         rsi_ble_app_get_event
 * @brief      returns the first set event based on priority
 * @param[in]  none.
 * @return     int32_t
 *             > 0  = event number
 *             -1   = not received any event
 * @section description
 * This function returns the highest priority event among all the set events
 */
static int32_t rsi_ble_app_get_event(void)
{
  uint32_t ix;

  for (ix = 0; ix < 32; ix++) {
    if (ble_app_event_map & (1 << ix)) {
      return ix;
    }
  }

  return (-1);
}
/*==============================================*/
/**
 * @fn         rsi_ble_simple_central_on_adv_report_event
 * @brief      invoked when advertise report event is received
 * @param[in]  adv_report, pointer to the received advertising report
 * @return     none.
 * @section description
 * This callback function updates the scanned remote devices list
 */
void rsi_ble_simple_central_on_adv_report_event(rsi_ble_event_adv_report_t *adv_report)
{

  if (device_found == 1) {
    return;
  }

  memset(remote_name, 0, 31);
  BT_LE_ADPacketExtract(remote_name, adv_report->adv_data, adv_report->adv_data_len);

  remote_addr_type = adv_report->dev_addr_type;
  rsi_6byte_dev_address_to_ascii(remote_dev_addr, (uint8_t *)adv_report->dev_addr);
  memcpy(remote_dev_bd_addr, (uint8_t *)adv_report->dev_addr, 6);
  if (((device_found == 0) && ((strcmp((const char *)remote_name, (const char *)RSI_REMOTE_DEVICE_NAME)) == 0))
      || (((remote_addr_type == RSI_BLE_DEV_ADDR_TYPE)
           && ((strcmp((const char *)remote_dev_addr, (const char *)RSI_BLE_DEV_ADDR) == 0))))) {
    device_found = 1;
    rsi_ble_app_set_event(RSI_APP_EVENT_ADV_REPORT);
  }
}

/*==============================================*/
/**
 * @fn         rsi_ble_on_connect_event
 * @brief      invoked when connection complete event is received
 * @param[out] resp_conn, connected remote device information
 * @return     none.
 * @section description
 * This callback function indicates the status of the connection
 */
static void rsi_ble_on_connect_event(rsi_ble_event_conn_status_t *resp_conn)
{
  memcpy(&conn_event_to_app, resp_conn, sizeof(rsi_ble_event_conn_status_t));
  rsi_ble_app_set_event(RSI_BLE_CONN_EVENT);
}

/*==============================================*/
/**
 * @fn         rsi_ble_on_disconnect_event
 * @brief      invoked when disconnection event is received
 * @param[in]  resp_disconnect, disconnected remote device information
 * @param[in]  reason, reason for disconnection.
 * @return     none.
 * @section description
 * This callback function indicates disconnected device information and status
 */
static void rsi_ble_on_disconnect_event(rsi_ble_event_disconnect_t *resp_disconnect, uint16_t reason)
{
  UNUSED_PARAMETER(reason); //This statement is added only to resolve compilation warning, value is unchanged
  memcpy(&disconn_event_to_app, resp_disconnect, sizeof(rsi_ble_event_disconnect_t));
  rsi_ble_app_set_event(RSI_BLE_DISCONN_EVENT);
}

/*==============================================*/
/**
 * @fn         rsi_ble_on_enhance_conn_status_event
 * @brief      invoked when enhanced connection complete event is received
 * @param[out] resp_conn, connected remote device information
 * @return     none.
 * @section description
 * This callback function indicates the status of the connection
 */
void rsi_ble_on_enhance_conn_status_event(rsi_ble_event_enhance_conn_status_t *resp_enh_conn)
{
  conn_event_to_app.dev_addr_type = resp_enh_conn->dev_addr_type;
  memcpy(conn_event_to_app.dev_addr, resp_enh_conn->dev_addr, RSI_DEV_ADDR_LEN);
  conn_event_to_app.status = resp_enh_conn->status;
  rsi_ble_app_set_event(RSI_BLE_CONN_EVENT);
}

/*==============================================*/
/**
 * @fn         rsi_ble_profile
 * @brief      invoked when the specific service details are received for
 *             supported specific services.
 * @param[out] p_ble_resp_profile, specific profile details
 * @return     none
 * @section description
 * This is a callback function
 */
static void rsi_ble_profile(uint16_t resp_status, profile_descriptors_t *rsi_ble_resp_profile)
{
  UNUSED_PARAMETER(resp_status); //This statement is added only to resolve compilation warning, value is unchanged
  UNUSED_PARAMETER(
    rsi_ble_resp_profile); //This statement is added only to resolve compilation warning, value is unchanged
  rsi_ble_app_set_event(RSI_BLE_GATT_PROFILE_RESP_EVENT);
  return;
}
/*==============================================*/
/**
 * @fn         rsi_ble_char_services
 * @brief      invoked when response is received for characteristic services details
 * @param[out] p_ble_resp_char_services, list of characteristic services.
 * @return     none
 * @section description
 */
static void rsi_ble_char_services(uint16_t resp_status, rsi_ble_resp_char_services_t *rsi_ble_resp_char_services)
{
  UNUSED_PARAMETER(
    rsi_ble_resp_char_services); //This statement is added only to resolve compilation warning, value is unchanged
  UNUSED_PARAMETER(resp_status); //This statement is added only to resolve compilation warning, value is unchanged
  rsi_ble_app_set_event(RSI_BLE_GATT_CHAR_SERVICES_RESP_EVENT);
  return;
}

/*==============================================*/
/**
 * @fn         rsi_ble_on_gatt_write_event
 * @brief      its invoked when write/notify/indication events are received.
 * @param[in]  event_id, it indicates write/notification event id.
 * @param[in]  rsi_ble_write, write event parameters.
 * @return     none.
 * @section description
 * This callback function is invoked when write/notify/indication events are received
 */
static void rsi_ble_on_gatt_write_event(uint16_t event_id, rsi_ble_event_write_t *rsi_ble_write)
{
  UNUSED_PARAMETER(event_id); //This statement is added only to resolve compilation warning, value is unchanged
  memcpy(&app_ble_write_event, rsi_ble_write, sizeof(rsi_ble_event_write_t));
  rsi_ble_app_set_event(RSI_BLE_GATT_WRITE_EVENT);
}

/*==============================================*/
/**
 * @fn         rsi_ble_on_read_req_event
 * @brief      its invoked when write/notify/indication events are received.
 * @param[in]  event_id, it indicates write/notification event id.
 * @param[in]  rsi_ble_write, write event parameters.
 * @return     none.
 * @section description
 * This callback function is invoked when write/notify/indication events are received
 */
static void rsi_ble_on_read_req_event(uint16_t event_id, rsi_ble_read_req_t *rsi_ble_read_req)
{
  UNUSED_PARAMETER(event_id); //This statement is added only to resolve compilation warning, value is unchanged
  memcpy(&app_ble_read_event, rsi_ble_read_req, sizeof(rsi_ble_read_req_t));
  rsi_ble_app_set_event(RSI_BLE_READ_REQ_EVENT);
}

/*==============================================*/
/**
 * @fn         rsi_ble_on_mtu_event
 * @brief      its invoked when write/notify/indication events are received.
 * @param[in]  event_id, it indicates write/notification event id.
 * @param[in]  rsi_ble_write, write event parameters.
 * @return     none.
 * @section description
 * This callback function is invoked when write/notify/indication events are received
 */
static void rsi_ble_on_mtu_event(rsi_ble_event_mtu_t *rsi_ble_mtu)
{
  memcpy(&app_ble_mtu_event, rsi_ble_mtu, sizeof(rsi_ble_event_mtu_t));
  mtu_size = (uint16_t)app_ble_mtu_event.mtu_size;
  rsi_ble_app_set_event(RSI_BLE_MTU_EVENT);
}

/*==============================================*/
/**
 * @fn         rsi_ble_simple_gatt_test
 * @brief      this is the application of ble GATT client api's test cases.
 * @param[in]  none.
 * @return     none.
 * @section description
 * This function is used at application.
 */
int32_t rsi_ble_simple_gatt_test(void)
{
  int32_t status  = 0;
  uint8_t adv[31] = { 2, 1, 6 };
  int32_t event_id;
  int8_t data[20]       = { 0 };
  uint8_t fmversion[20] = { 0 };
#if (GATT_ROLE == CLIENT)
  int8_t client_data[100] = { 1, 2, 3, 4, 5, 6, 7, 8, 9, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 0,
                              1, 2, 3, 4, 5, 6, 7, 8, 9, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 0 };
  uint8_t ix;
  uuid_t service_uuid;
#endif
  rsi_ble_resp_local_att_value_t local_att_val;

  uint8_t read_data[100] = { 2 };

#ifdef RSI_WITH_OS
  rsi_task_handle_t driver_task_handle = NULL;
#endif

#ifndef RSI_WITH_OS
  //! Driver initialization
  status = rsi_driver_init(global_buf, BT_GLOBAL_BUFF_LEN);
  if ((status < 0) || (status > BT_GLOBAL_BUFF_LEN)) {
    return status;
  }

  //! SiLabs module intialisation
  status = rsi_device_init(LOAD_NWP_FW);
  if (status != RSI_SUCCESS) {
    LOG_PRINT("\r\nDevice Initialization Failed, Error Code : 0x%lX\r\n", status);
    return status;
  } else {
    LOG_PRINT("\r\nDevice Initialization Success\r\n");
  }
#endif
#ifdef RSI_WITH_OS
#ifndef RSI_M4_INTERFACE

  //! SiLabs module initialization
  status = rsi_device_init(LOAD_NWP_FW);
  if (status != RSI_SUCCESS) {
    LOG_PRINT("\r\nDevice Initialization Failed, Error Code : 0x%lX\r\n", status);
    return status;
  } else {
    LOG_PRINT("\r\nDevice Initialization Success\r\n");
  }
#endif
  //! Task created for Driver task
  rsi_task_create((rsi_task_function_t)rsi_wireless_driver_task,
                  (uint8_t *)"driver_task",
                  RSI_DRIVER_TASK_STACK_SIZE,
                  NULL,
                  RSI_DRIVER_TASK_PRIORITY,
                  &driver_task_handle);
#endif
  //! WC initialization
  status = rsi_wireless_init(0, RSI_OPERMODE_WLAN_BLE);
  if (status != RSI_SUCCESS) {
    LOG_PRINT("\r\nWireless Initialization Failed, Error Code : 0x%lX\r\n", status);
    return status;
  } else {
    LOG_PRINT("\r\nWireless Initialization Success\r\n");
  }

  //! Firmware version Prints
  status = rsi_get_fw_version(fmversion, sizeof(fmversion));
  if (status != RSI_SUCCESS) {
    LOG_PRINT("\r\nFirmware version Failed, Error Code : 0x%lX\r\n", status);
  } else {
    LOG_PRINT("\nfirmware_version = %s", fmversion);
  }
#if M4_POWERSAVE_ENABLE

  //RSI_PS_FlashLdoEnable();
  /* MCU Hardware Configuration for Low-Power Applications */
  RSI_WISEMCU_HardwareSetup();
  LOG_PRINT("\r\nRSI_WISEMCU_HardwareSetup Success\r\n");

#endif
#if (GATT_ROLE == SERVER)
  rsi_ble_add_simple_chat_serv();
#endif

  //! registering the GAP call back functions
  rsi_ble_gap_register_callbacks(rsi_ble_simple_central_on_adv_report_event,
                                 rsi_ble_on_connect_event,
                                 rsi_ble_on_disconnect_event,
                                 NULL,
                                 NULL,
                                 NULL,
                                 rsi_ble_on_enhance_conn_status_event,
                                 NULL,
                                 NULL,
                                 NULL);

  //! registering the GATT call back functions
  rsi_ble_gatt_register_callbacks(NULL,
                                  rsi_ble_profile,
                                  rsi_ble_char_services,
                                  NULL,
                                  NULL,
                                  NULL,
                                  NULL,
                                  rsi_ble_on_gatt_write_event,
                                  NULL,
                                  NULL,
                                  rsi_ble_on_read_req_event,
                                  rsi_ble_on_mtu_event,
                                  NULL,
                                  NULL,
                                  NULL,
                                  NULL,
                                  NULL,
                                  NULL,
                                  NULL,
                                  NULL,
                                  NULL,
                                  NULL,
                                  NULL);

  rsi_semaphore_create(&ble_main_task_sem, 0);
  //!  initializing the application events map
  rsi_ble_app_init_events();

  //! Set local name
  rsi_bt_set_local_name((uint8_t *)RSI_BLE_APP_GATT_TEST);

#if (GATT_ROLE == SERVER)
  //! prepare advertise data //local/device name
  adv[3] = strlen(RSI_BLE_APP_GATT_TEST) + 1;
  adv[4] = 9;
  strcpy((char *)&adv[5], RSI_BLE_APP_GATT_TEST);

  //! set advertise data
  rsi_ble_set_advertise_data(adv, strlen(RSI_BLE_APP_GATT_TEST) + 5);

  //! set device in advertising mode.
  rsi_ble_start_advertising();
#endif

#if (GATT_ROLE == CLIENT)
  //! start scanning
  status = rsi_ble_start_scanning();
  LOG_PRINT("start scanning\n");
  if (status != RSI_SUCCESS) {
    LOG_PRINT("start_scanning status: 0x%X\r\n", status);
    return status;
  }
#endif
#if ENABLE_POWER_SAVE

  LOG_PRINT("\r\n Initiate module in to power save \r\n");
  //! enable wlan radio
  status = rsi_wlan_radio_init();
  if (status != RSI_SUCCESS) {
    LOG_PRINT("\n radio init failed \n");
  }

  //! initiating power save in BLE mode
  status = rsi_bt_power_save_profile(PSP_MODE, PSP_TYPE);
  if (status != RSI_SUCCESS) {
    LOG_PRINT("\r\n Failed to initiate power save in BLE mode \r\n");
    return status;
  }

  //! initiating power save in wlan mode
  status = rsi_wlan_power_save_profile(PSP_MODE, PSP_TYPE);
  if (status != RSI_SUCCESS) {
    LOG_PRINT("\r\n Failed to initiate power save in WLAN mode \r\n");
    return status;
  }

  LOG_PRINT("\r\n Module is in power save \r\n");
#endif
#if M4_POWERSAVE_ENABLE
  P2P_STATUS_REG &= ~M4_wakeup_TA;
  // LOG_PRINT("\n RSI_BLE_REQ_PWRMODE\n ");
#endif
  //! waiting for events from controller.
  while (1) {
#ifndef RSI_WITH_OS
    rsi_wireless_driver_task();
#endif

    //! checking for events list
    event_id = rsi_ble_app_get_event();
    if (event_id == -1) {
#if M4_POWERSAVE_ENABLE
      //! if events are not received loop will be continued.
      if ((!(P2P_STATUS_REG & TA_wakeup_M4)) && (!rsi_driver_cb->scheduler_cb.event_map)) {
        P2P_STATUS_REG &= ~M4_wakeup_TA;
        rsi_ble_only_Trigger_M4_Sleep();
      }
#else
      {
        rsi_semaphore_wait(&ble_main_task_sem, 0);
      }
#endif
      continue;
    }

    switch (event_id) {
#if (GATT_ROLE == CLIENT)
      case RSI_APP_EVENT_ADV_REPORT: {
        //! advertise report event.
        //! clear the advertise report event.
        rsi_ble_app_clear_event(RSI_APP_EVENT_ADV_REPORT);
        LOG_PRINT("\r\nIn Advertising Event\r\n");
        status = rsi_ble_connect(remote_addr_type, (int8_t *)remote_dev_bd_addr);
        if (status != RSI_SUCCESS) {
          LOG_PRINT("connect status: 0x%X\r\n", status);
        }

      } break;

#endif
      case RSI_BLE_CONN_EVENT: {
        //! event invokes when connection was completed

        //! clear the served event
        rsi_ble_app_clear_event(RSI_BLE_CONN_EVENT);
        rsi_6byte_dev_address_to_ascii(str_remote_address, conn_event_to_app.dev_addr);
        LOG_PRINT("\r\n Module connected to address : %s \r\n", str_remote_address);

#if (GATT_ROLE == CLIENT)
        service_uuid.size      = 2;
        service_uuid.val.val16 = RSI_BLE_NEW_CLIENT_SERVICE_UUID;
retry:
        status = rsi_ble_get_profile(conn_event_to_app.dev_addr, service_uuid, &rsi_ble_service);
        if (status != 0)
          goto retry;
#endif
#if (GATT_ROLE == SERVER)
        status = rsi_ble_mtu_exchange_event((uint8_t *)conn_event_to_app.dev_addr, RSI_BLE_MTU_SIZE);
        if (status != RSI_SUCCESS) {
          LOG_PRINT("MTU CMD status: 0x%lX\r\n", status);
        }
#endif

      } break;

      case RSI_BLE_DISCONN_EVENT: {
        //! event invokes when disconnection was completed

        //! clear the served event
        rsi_ble_app_clear_event(RSI_BLE_DISCONN_EVENT);
        LOG_PRINT("\r\nModule got Disconnected\r\n");
#if (GATT_ROLE == SERVER)
adv:
        //! set device in advertising mode.
        status = rsi_ble_start_advertising();
        if (status != RSI_SUCCESS) {
          goto adv;
        }
#endif
#if (GATT_ROLE == CLIENT)
        //! start scanning
        device_found = 0;
        LOG_PRINT("\r\nStart scanning \n");
        status = rsi_ble_start_scanning();
        if (status != RSI_SUCCESS) {
          LOG_PRINT("start_scanning status: 0x%X\r\n", status);
        }
#endif
      } break;
      case RSI_BLE_GATT_WRITE_EVENT: {
        LOG_PRINT("\r\nIn BLE GATT write event\r\n");
        //! event invokes when write/notification events received

        //! clear the served event
        rsi_ble_app_clear_event(RSI_BLE_GATT_WRITE_EVENT);

        //TO DO: send ERR or write response
        if ((*(uint16_t *)app_ble_write_event.handle) == rsi_ble_att1_val_hndl) {
          rsi_ble_att_list_t *attribute = NULL;
          uint8_t opcode = 0x12, err = 0x00;
          attribute = rsi_gatt_get_attribute_from_list(&att_list, (*(uint16_t *)(app_ble_write_event.handle)));

          //! Check if value has write properties
          if ((attribute != NULL) && (attribute->value != NULL)) {
            if (!(attribute->char_val_prop & 0x08)) //! If no write property, send error response
            {
              err = 0x03; //! Error - Write not permitted
            }
          } else {
            //!Error = No such handle exists
            err = 0x01;
          }

          //! Update the value based6 on the offset and length of the value
          if ((err == 0) && ((app_ble_write_event.length) <= attribute->max_value_len)) {
            memset(attribute->value, 0, attribute->max_value_len);

            //! Check if value exists for the handle. If so, maximum length of the value.
            memcpy(attribute->value, app_ble_write_event.att_value, app_ble_write_event.length);

            //! Update value length
            attribute->len = app_ble_write_event.length;

            LOG_PRINT("\r\n received data from remote device: %s \n", (char *)attribute->value);

            //! Send gatt write response
            rsi_ble_gatt_write_response(conn_event_to_app.dev_addr, 0);
          } else {
            //! Error : 0x07 - Invalid request,  0x0D - Invalid attribute value length
            err = 0x07;
          }

          if (err) {
            //! Send error response
            rsi_ble_att_error_response(conn_event_to_app.dev_addr,
                                       *(uint16_t *)app_ble_write_event.handle,
                                       opcode,
                                       err);
          }
        }
        //! prepare the data to set as local attribute value.
        memset(data, 0, sizeof(data));
        memcpy(data, app_ble_write_event.att_value, app_ble_write_event.length);

        //! set the local attribute value.
        rsi_ble_notify_value(conn_event_to_app.dev_addr, rsi_ble_att1_val_hndl, RSI_BLE_MAX_DATA_LEN, (uint8_t *)data);
      } break;
      case RSI_BLE_READ_REQ_EVENT: {
        LOG_PRINT("\r\nIn BLE read request event\r\n");
        //! event invokes when write/notification events received

        //! clear the served event
        rsi_ble_app_clear_event(RSI_BLE_READ_REQ_EVENT);

        rsi_ble_gatt_read_response(conn_event_to_app.dev_addr,
                                   0,
                                   app_ble_read_event.handle,
                                   0,
                                   RSI_MIN(mtu_size - 2, sizeof(read_data)),
                                   read_data);
      } break;

      case RSI_BLE_GATT_PROFILE_RESP_EVENT: {
        LOG_PRINT("\r\nIn BLE GATT profile response event\r\n");
        //! event invokes when get profile response received

        //! clear the served event
        rsi_ble_app_clear_event(RSI_BLE_GATT_PROFILE_RESP_EVENT);

#if (GATT_ROLE == CLIENT)
        //! get characteristics of the immediate alert servcie
        //rsi_6byte_dev_address_to_ascii ((int8_t *)remote_dev_addr, (uint8_t *)conn_event_to_app.dev_addr);
        rsi_ble_get_char_services(conn_event_to_app.dev_addr,
                                  *(uint16_t *)rsi_ble_service.start_handle,
                                  *(uint16_t *)rsi_ble_service.end_handle,
                                  &char_servs);
#endif
      } break;

      case RSI_BLE_GATT_CHAR_SERVICES_RESP_EVENT: {
        LOG_PRINT("\r\nIn BLE GATT characteristics of services response event\r\n");
        //! event invokes when get characteristics of the service response received

        //! clear the served event
        rsi_ble_app_clear_event(RSI_BLE_GATT_CHAR_SERVICES_RESP_EVENT);

#if (GATT_ROLE == CLIENT)
        //! verifying the immediate alert characteristic
        for (ix = 0; ix < char_servs.num_of_services; ix++) {
          LOG_PRINT("Character services of  profile : ");
          LOG_PRINT(" uuid: 0x%04x\r\n", char_servs.char_services[ix].char_data.char_uuid.val.val16);
          if (char_servs.char_services[ix].char_data.char_uuid.val.val16 == RSI_BLE_CLIENT_ATTRIBUTE_1_UUID) {
            rsi_ble_att1_val_hndl = char_servs.char_services[ix].char_data.char_handle;

            rsi_ble_set_att_cmd(conn_event_to_app.dev_addr,
                                rsi_ble_att1_val_hndl,
                                RSI_MIN(mtu_size - 3, 100),
                                (uint8_t *)&client_data);
            //! set the event to calculate RSSI value
#ifndef RSI_SAMPLE_HAL
            last_time = rsi_hal_gettickcount();
#endif
            break;
          }
        }
#endif
      } break;

      case RSI_BLE_MTU_EVENT: {
        LOG_PRINT("\r\nIn MTU event\r\n");
        //! event invokes when write/notification events received

        //! clear the served event
        rsi_ble_app_clear_event(RSI_BLE_MTU_EVENT);

      } break;
      default: {
      }
    }
  }

  // return 0; // // This statement is commented to avoid compilation warning
}

/*==============================================*/
/**
 * @fn         main_loop
 * @brief      Schedules the Driver task.
 * @param[in]  none.
 * @return     none.
 * @section description
 * This function schedules the Driver task.
 */
void main_loop(void)
{
  while (1) {
    rsi_wireless_driver_task();
  }
}

/*==============================================*/
/**
 * @fn         main
 * @brief      main function
 * @param[in]  none.
 * @return     none.
 * @section description
 * This is the main function to call Application
 */
int main(void)
{
  int32_t status;
#ifdef RSI_WITH_OS
  rsi_task_handle_t bt_task_handle = NULL;
#endif
#ifndef RSI_WITH_OS
  //! Call WLAN BLE application
  rsi_ble_simple_gatt_test();

  //! Application main loop
  main_loop();

  return 0;
#endif

#ifdef RSI_WITH_OS
  //! Driver initialization
  status = rsi_driver_init(global_buf, BT_GLOBAL_BUFF_LEN);
  if ((status < 0) || (status > BT_GLOBAL_BUFF_LEN)) {
    return status;
  }
#ifdef RSI_M4_INTERFACE
  // Silicon labs module initialization
  status = rsi_device_init(LOAD_NWP_FW);
  if (status != RSI_SUCCESS) {
    LOG_PRINT("\r\nDevice Initialization Failed, Error Code : 0x%lX\r\n", status);
    return status;
  }
#endif

  //! OS case
  //! Task created for BLE task
  rsi_task_create((rsi_task_function_t)(int32_t)rsi_ble_simple_gatt_test,
                  (uint8_t *)"ble_task",
                  RSI_BT_TASK_STACK_SIZE,
                  NULL,
                  RSI_BT_TASK_PRIORITY,
                  &bt_task_handle);

  //! OS TAsk Start the scheduler
  rsi_start_os_scheduler();

  return 0;
#endif
}
