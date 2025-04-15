#ifndef BLE_CONFIG_H
#define BLE_CONFIG_H

//! defines

#define ADV_ENABLED_DEFAULT        1
#define SCAN_ENABLED_DEFAULT       1
#define POWER_SAVE_ENABLED_DEFAULT 0

#define BD_ADDR_LEN 6

#define RSI_REM_DEV_ADDR_LEN 18

#define RSI_REM_DEV_NAME_LEN 31

#define BT_TRUE  1
#define BT_FLASE 0

/* MACRO defined so we dont have an overlap of BT / BLE local_name_set . */

#define RSI_BT_LOCAL_NAME (void *)"BLE_AE_APP"
/***********************************************************************************************************************************************/
/***********************************************************************************************************************************************/

/***********************************************************************************************************************************************/
/***********************************************************************************************************************************************/
#define RSI_BLE_APP_GATT_TEST       (void *)"BLE_AE_COEX_DEMO" //! local device name
#define RSI_BLE_MAX_NBR_PERIPHERALS 1
#define RSI_BLE_MAX_NBR_CENTRALS    1
#define TOTAL_CONNECTIONS           RSI_BLE_MAX_NBR_PERIPHERALS + RSI_BLE_MAX_NBR_CENTRALS
#define REM_DEV_ADDR                "00:00:00:00:00:00"
#define RSI_BLE_SET_RAND_ADDR       "CC:44:33:44:55:CC"

#define RSI_BLE_GATT_ASYNC_ENABLE               1
#define RSI_BLE_INDICATE_CONFIRMATION_FROM_HOST 1
#define RSI_BLE_MTU_EXCHANGE_FROM_HOST          1
#define MAX_MTU_SIZE                            240
#define UPDATE_CONN_PARAMETERS                  0 //! To update connection parameters of remote central connection
#define MITM_ENABLE                             1
#define RSI_BLE_MAX_CHAR_DESCRIPTORS            5
#define RSI_MAX_PROFILE_CNT                     8
#define SMP_ENABLE                              1
#define SIG_VUL                                 0
#define RESOLVE_ENABLE                          0
#define LOCAL_MTU_SIZE                          232

/** Note: When BLE_DYNAMIC_BUFF_CONFIGURATION_BASED_ON_INTERVAL Macro is enabled,
 *   After connection update completion, Tx buffers for Each Remote Central connection will be allocated as follows..
 *   if Connection interval <= 15msec then 3 buffers will be allocated.
 *   If Connection interval <= 45msec then 5 buffers will be allocated.
 *   If Connection interval > 45msec then 7 buffers will be allocated.
 */

#define BLE_DYNAMIC_BUFF_CONFIGURATION_BASED_ON_INTERVAL 0

#define RSI_BLE_PWR_INX 30

#define CONN_BY_ADDR   1
#define CONN_BY_NAME   2
#define CONNECT_OPTION CONN_BY_NAME // CONN_BY_ADDR

#if (CONNECT_OPTION == CONN_BY_NAME)
/***********************************************************************************************************************************************/
//! Remote Device Name to connect
/***********************************************************************************************************************************************/
#define RSI_REMOTE_DEVICE_NAME1 "AEdevice1" //"Hotspot"//"Skv"//"Hotspot"
#define RSI_REMOTE_DEVICE_NAME2 "AEdevice2"
#define RSI_REMOTE_DEVICE_NAME3 "AEdevice3"
#else

#define RSI_BLE_DEV_ADDR_TYPE \
  LE_PUBLIC_ADDRESS //! Address type of the device to connect - LE_PUBLIC_ADDRESS / LE_RANDOM_ADDRESS
/***********************************************************************************************************************************************/
//! Address of the devices to connect
/***********************************************************************************************************************************************/
#define RSI_BLE_DEV_1_ADDR "00:1A:7D:DA:71:16"
#define RSI_BLE_DEV_2_ADDR "00:1A:7D:DA:71:74"
#define RSI_BLE_DEV_3_ADDR "00:1A:7D:DA:73:13"
#endif

#define PERIPHERAL_FOUND         1
#define PERIPHERAL_CONNECTED     2
#define PERIPHERAL_NOT_CONNECTED 3

/***********************************************************************************************************************************************/
//! BLE AE event properties
/***********************************************************************************************************************************************/

#define BLE_CONNECTABLE_ADV        (1 << 0)
#define BLE_SCANNABLE_ADV          (1 << 1)
#define BLE_LOW_DUTY_DIR_CONN_ADV  (1 << 2)
#define BLE_HIGH_DUTY_DIR_CONN_ADV (1 << 3)
#define BLE_LEGACY_ADV             (1 << 4)
#define BLE_ANONYMOUS_ADV          (1 << 5)
#define BLE_TX_WR_ADV              (1 << 6)

/***********************************************************************************************************************************************/
//! BLE connection specific configurations
/***********************************************************************************************************************************************/
#if !RUN_TIME_CONFIG_ENABLE
/*=================Central1 configurations=====================*/
//! configure below macro to enable secure connection
#define SMP_ENABLE_C1 1
// Add remote device to acceptlist
#define ADD_TO_ACCEPTLIST_C1 0
//! configure below macro to discover remote profiles
#define PROFILE_QUERY_C1 1
//! configure below macro to perform data transfer
#define DATA_TRANSFER_C1 1
//! configure below macros to select type of data transfer
//!  set below macro to receive 'gatt notifications' from remote device
#define RX_NOTIFICATIONS_FROM_C1 1
//! set below macro to receive 'gatt indications' from remote device
#define RX_INDICATIONS_FROM_C1 0
//! set below macro to Transmit 'gatt notifications' to remote device
#define TX_NOTIFICATIONS_TO_C1 1
//! set below macro to Transmit 'gatt write with response' to remote device
#define TX_WRITES_TO_C1 0
//! set below macro to Transmit 'gatt write without response' to remote device
#define TX_WRITES_NO_RESP_TO_C1 0
//! set below macro to Transmit 'gatt indications' to remote device
#define TX_INDICATIONS_TO_C1 0
//! Configure below macro to select data length extension ON/OFF
#define DLE_ON_C1 0
#if DLE_ON_C1
#define DLE_BUFFER_MODE_C1      1
#define DLE_BUFFER_COUNT_C1     2
#define RSI_BLE_MAX_DATA_LEN_C1 230
#else
#define DLE_BUFFER_MODE_C1      0
#define DLE_BUFFER_COUNT_C1     2
#define RSI_BLE_MAX_DATA_LEN_C1 20 //! max data length
#endif
//! Configure below macros to select connection paramaters while data transfer
#define CONN_INTERVAL_C1            36 //! for conn interval of 45ms
#define CONN_LATENCY_C1             0
#define CONN_SUPERVISION_TIMEOUT_C1 800

/*=================Central2 configurations=====================*/
//! configure below macro to enable secure connection
#define SMP_ENABLE_C2 0
// Add remote device to acceptlist
#define ADD_TO_ACCEPTLIST_C2 0
//! configure below macro to discover remote profiles
#define PROFILE_QUERY_C2 1
//! configure below macro to perform data transfer
#define DATA_TRANSFER_C2 1
//! configure below macros to select type of data transfer
//!  set below macro to receive 'gatt notifications' from remote device
#define RX_NOTIFICATIONS_FROM_C2 0
//! set below macro to receive 'gatt indications' from remote device
#define RX_INDICATIONS_FROM_C2 0
//! set below macro to Transmit 'gatt notifications' to remote device
#define TX_NOTIFICATIONS_TO_C2 1
//! set below macro to Transmit 'gatt write with response' to remote device
#define TX_WRITES_TO_C2 0
//! set below macro to Transmit 'gatt write without response' to remote device
#define TX_WRITES_NO_RESP_TO_C2 0
//! set below macro to Transmit 'gatt indications' to remote device
#define TX_INDICATIONS_TO_C2 0

//! Configure below macro to select data length extension ON/OFF
#define DLE_ON_C2 0
#if DLE_ON_C2
#define DLE_BUFFER_MODE_C2      1
#define DLE_BUFFER_COUNT_C2     2
#define RSI_BLE_MAX_DATA_LEN_C2 230
#else
#define DLE_BUFFER_MODE_C2      0
#define DLE_BUFFER_COUNT_C2     2
#define RSI_BLE_MAX_DATA_LEN_C2 20 //! max data length
#endif
//! Configure below macros to select connection paramaters while data transfer
#define CONN_INTERVAL_C2            400 //! for conn interval of 500ms
#define CONN_LATENCY_C2             0
#define CONN_SUPERVISION_TIMEOUT_C2 800

/*=================Peripheral1 configurations=====================*/
//! configure below macro to enable secure connection
#define SMP_ENABLE_P1 0
// Add remote device to acceptlist
#define ADD_TO_ACCEPTLIST_P1 0
//! configure below macro to discover remote profiles
#define PROFILE_QUERY_P1 1
//! configure below macro to perform data transfer
#define DATA_TRANSFER_P1 1
//! configure below macros to select type of data transfer
//!  set below macro to receive 'gatt notifications' from remote device
#define RX_NOTIFICATIONS_FROM_P1 1
//! set below macro to receive 'gatt indications' from remote device
#define RX_INDICATIONS_FROM_P1 0
//! set below macro to Transmit 'gatt notifications' to remote device
#define TX_NOTIFICATIONS_TO_P1 1
//! set below macro to Transmit 'gatt write with response' to remote device
#define TX_WRITES_TO_P1 0
//! set below macro to Transmit 'gatt write without response' to remote device
#define TX_WRITES_NO_RESP_TO_P1 0
//! set below macro to Transmit 'gatt indications' to remote device
#define TX_INDICATIONS_TO_P1 0

//! Configure below macro to select data length extension ON/OFF
#define DLE_ON_P1 0
#if DLE_ON_P1
#define DLE_BUFFER_MODE_P1      1
#define DLE_BUFFER_COUNT_P1     2
#define RSI_BLE_MAX_DATA_LEN_P1 230
#else
#define DLE_BUFFER_MODE_P1      0
#define DLE_BUFFER_COUNT_P1     2
#define RSI_BLE_MAX_DATA_LEN_P1 20 //! max data length
#endif
//! Configure below macros to select connection paramaters while data transfer
#define CONN_INTERVAL_P1            240 //! for conn interval of 300ms
#define CONN_LATENCY_P1             0
#define CONN_SUPERVISION_TIMEOUT_P1 400

/*=================Peripheral2 configurations=====================*/
//! configure below macro to enable secure connection
#define SMP_ENABLE_P2 1
// Add remote device to acceptlist
#define ADD_TO_ACCEPTLIST_P2 0
//! configure below macro to discover remote profiles
#define PROFILE_QUERY_P2 1
//! configure below macro to perform data transfer
#define DATA_TRANSFER_P2 1
//! configure below macros to select type of data transfer
//!  set below macro to receive 'gatt notifications' from remote device
#define RX_NOTIFICATIONS_FROM_P2 1
//! set below macro to receive 'gatt indications' from remote device
#define RX_INDICATIONS_FROM_P2 0
//! set below macro to Transmit 'gatt notifications' to remote device
#define TX_NOTIFICATIONS_TO_P2 1
//! set below macro to Transmit 'gatt write with response' to remote device
#define TX_WRITES_TO_P2 0
//! set below macro to Transmit 'gatt write without response' to remote device
#define TX_WRITES_NO_RESP_TO_P2 0
//! set below macro to Transmit 'gatt indications' to remote device
#define TX_INDICATIONS_TO_P2 0
//! Configure below macro to select data length extension ON/OFF
#define DLE_ON_P2 0
#if DLE_ON_P2
#define DLE_BUFFER_MODE_P2      1
#define DLE_BUFFER_COUNT_P2     2
#define RSI_BLE_MAX_DATA_LEN_P2 230
#else
#define DLE_BUFFER_MODE_P2      0
#define DLE_BUFFER_COUNT_P2     2
#define RSI_BLE_MAX_DATA_LEN_P2 20 //! max data length
#endif
//! Configure below macros to select connection paramaters while data transfer
#define CONN_INTERVAL_P2            240 //! for conn interval of 300ms
#define CONN_LATENCY_P2             0
#define CONN_SUPERVISION_TIMEOUT_P2 400

/*=================Peripheral3 configurations=====================*/
//! configure below macro to enable secure connection
#define SMP_ENABLE_P3 1
// Add remote device to acceptlist
#define ADD_TO_ACCEPTLIST_P3 0
//! configure below macro to discover remote profiles
#define PROFILE_QUERY_P3 1
//! configure below macro to perform data transfer
#define DATA_TRANSFER_P3 1
//! configure below macros to select type of data transfer
//!  set below macro to receive 'gatt notifications' from remote device
#define RX_NOTIFICATIONS_FROM_P3 1
//! set below macro to receive 'gatt indications' from remote device
#define RX_INDICATIONS_FROM_P3 0
//! set below macro to Transmit 'gatt notifications' to remote device
#define TX_NOTIFICATIONS_TO_P3 1
//! set below macro to Transmit 'gatt write with response' to remote device
#define TX_WRITES_TO_P3 0
//! set below macro to Transmit 'gatt write without response' to remote device
#define TX_WRITES_NO_RESP_TO_P3 0
//! set below macro to Transmit 'gatt indications' to remote device
#define TX_INDICATIONS_TO_P3 0
//! Configure below macro to select data length extension ON/OFF
#define DLE_ON_P3 0
#if DLE_ON_P3
#define DLE_BUFFER_MODE_P3      1
#define DLE_BUFFER_COUNT_P3     2
#define RSI_BLE_MAX_DATA_LEN_P3 230
#else
#define DLE_BUFFER_MODE_P3      0
#define DLE_BUFFER_COUNT_P3     2
#define RSI_BLE_MAX_DATA_LEN_P3 20 //! max data length
#endif
//! Configure below macros to select connection paramaters while data transfer
#define CONN_INTERVAL_P3            240 //! for conn interval of 300ms
#define CONN_LATENCY_P3             0
#define CONN_SUPERVISION_TIMEOUT_P3 400
#endif

/***********************************************************************************************************************************************/
//! BLE extended advertising sets specific configurations
/***********************************************************************************************************************************************/

#define PERIODIC_ADV_EN 0
#define ADV_SET1_EN     1
#define ADV_SET2        1

//!Extended Advertise Data types
#define BLE_AE_ADV_DATA_TYPE          0x01
#define BLE_AE_PERIODIC_ADV_DATA_TYPE 0x02
#define BLE_AE_SCAN_RSP_DATA_TYPE     0x03

#define BLE_AE_ADV_DATA_LEN 200
#define BLE_AE_ADV_DATA     "AE_PERIPHERAL_DATA_1"
#define RSI_BLE_AE_ADV_DATA                                                        \
  "AE_PERIPHERAL_DATA_1_This_is_the_advertisement_data_for_the_peripheral_device_" \
  "which_includes_information_about_its_capabilities_and_features_for_bluetooth."

/*=================AE Set 1 configurations=====================*/
#define BLE_AE_ADV_HNDL_SET_1          0x00
#define BLE_AE_ADV_INT_MIN_SET_1       0x664
#define BLE_AE_ADV_INT_MAX_SET_1       0x664
#define BLE_AE_ADV_CHANNEL_MAP_SET_1   0x07
#define BLE_AE_ADV_FILTER_POLICY_SET_1 0x00
#define BLE_AE_ADV_TX_PWR_SET_1        0x00
#define BLE_AE_PRIMARY_ADV_PHY_SET_1   0x01
#define BLE_AE_SEC_ADV_MAX_SKIP_SET_1  0x00
#define BLE_AE_SECONDARY_ADV_PHY_SET_1 0x01
#define BLE_AE_ADV_SID_SET_1           0x00
#define BLE_AE_SCAN_REQ_NOTIF_EN_SET_1 0x01
#define BLE_AE_ADV_EVNT_PROP_SET_1     (BLE_CONNECTABLE_ADV) //| BLE_LEGACY_ADV | BLE_SCANNABLE_ADV)
#define BLE_AE_ADV_DUR_SET_1           0x00
#define BLE_AE_ADV_MAX_AE_EVENTS_SET_1 0x00

/*=================AE Set 2 configurations=====================*/
#define BLE_AE_ADV_HNDL_SET_2          0x01
#define BLE_AE_ADV_INT_MIN_SET_2       0x664
#define BLE_AE_ADV_INT_MAX_SET_2       0x664
#define BLE_AE_ADV_CHANNEL_MAP_SET_2   0x07
#define BLE_AE_ADV_FILTER_POLICY_SET_2 0x00
#define BLE_AE_ADV_TX_PWR_SET_2        0x06
#define BLE_AE_PRIMARY_ADV_PHY_SET_2   0x03
#define BLE_AE_SEC_ADV_MAX_SKIP_SET_2  0x00
#define BLE_AE_SECONDARY_ADV_PHY_SET_2 0x03
#define BLE_AE_ADV_SID_SET_2           0x01
#define BLE_AE_SCAN_REQ_NOTIF_EN_SET_2 0x01
#define BLE_AE_ADV_EVNT_PROP_SET_2     BLE_CONNECTABLE_ADV
#define BLE_AE_ADV_DUR_SET_2           0x00
#define BLE_AE_ADV_MAX_AE_EVENTS_SET_2 0x00

/*=================Periodic advertising configurations=====================*/
#define BLE_AE_PER_ADV_EN      1
#define BLE_AE_PER_ADV_INT_MIN 0x90
#define BLE_AE_PER_ADV_INT_MAX 0x190
#define BLE_AE_PER_ADV_PROP    0x40

/*=================Extended scanning configurations=====================*/
#define BLE_AE_SCAN_FILTER_TYPE       SCAN_FILTER_TYPE_ALL
#define PRI_PHY_BLE_AE_SCAN_TYPE      SCAN_TYPE_ACTIVE
#define SEC_PHY_BLE_AE_SCAN_TYPE      SCAN_TYPE_ACTIVE
#define PRI_PHY_LE_AE_SCAN_INTERVAL   0x14A0
#define PRI_PHY_LE_AE_SCAN_WINDOW     0x14A0
#define SEC_PHY_LE_AE_SCAN_INTERVAL   0x14A0
#define SEC_PHY_LE_AE_SCAN_WINDOW     0x14A0
#define BLE_AE_SCAN_ENABLE_FILTER_DUP 0x00
#define BLE_AE_SCAN_DUR               0x00
#define BLE_AE_SCAN_PERIOD            0x00

#define PHY_1M       BIT(0)
#define PHY_2M       BIT(1)
#define PHY_LE_CODED BIT(2)

#define RSI_BLE_CLIENT_WRITE_SERVICE_UUID         0x180D //! Heart Rate service uuid
#define RSI_BLE_CLIENT_WRITE_CHAR_UUID            0x2A39 //! Heart Rate control Point
#define RSI_BLE_CLIENT_WRITE_NO_RESP_SERVICE_UUID 0x1802 //! Immediate Alert service uuid
#define RSI_BLE_CLIENT_WRITE_NO_RESP_CHAR_UUID    0x2A06 //! Alert level char uuid
#define RSI_BLE_CLIENT_INIDCATIONS_SERVICE_UUID   0x1809 //! Health thermometer Alert service uuid
#define RSI_BLE_CLIENT_INIDCATIONS_CHAR_UUID      0x2A1C //! Temperature measurement
#define RSI_BLE_CLIENT_NOTIFICATIONS_SERVICE_UUID 0x180D //! Heart Rate service uuid
#define RSI_BLE_CLIENT_NOTIFICATIONS_CHAR_UUID    0x2A37 //! Heart Rate measurement

#if RESOLVE_ENABLE
#define RRIVACY_ENABLE 0
#endif

#define SET_SMP_CONFIGURATION 0x01
#if SET_SMP_CONFIGURATION
#define LOCAL_OOB_DATA_FLAG_NOT_PRESENT (0x00)

#define AUTH_REQ_BONDING_BITS   ((1 << 0))
#define AUTH_REQ_MITM_BIT       (1 << 2)
#define AUTH_REQ_SC_BIT         (1 << 3)
#define AUTH_REQ_CT2_BIT        (1 << 5)
#define AUTH_REQ_BITS           (AUTH_REQ_BONDING_BITS | AUTH_REQ_MITM_BIT | AUTH_REQ_SC_BIT | AUTH_REQ_CT2_BIT)
#define MAXIMUM_ENC_KEY_SIZE_16 (16)

// BLUETOOTH SPECIFICATION Version 5.0 | Vol 3, Part H 3.6.1 Key Distribution and Generation
#define ENC_KEY_DIST  (1 << 0)
#define ID_KEY_DIST   (1 << 1)
#define SIGN_KEY_DIST (1 << 2)
#define LINK_KEY_DIST (0 << 3)
#if RESOLVE_ENABLE
#define RESPONDER_KEYS_TO_DIST (ENC_KEY_DIST | SIGN_KEY_DIST | ID_KEY_DIST)
#else
#define RESPONDER_KEYS_TO_DIST (ENC_KEY_DIST | SIGN_KEY_DIST | ID_KEY_DIST | LINK_KEY_DIST)
#endif
#if RESOLVE_ENABLE
#define INITIATOR_KEYS_TO_DIST (ID_KEY_DIST | ENC_KEY_DIST | SIGN_KEY_DIST)
#else
#define INITIATOR_KEYS_TO_DIST (ENC_KEY_DIST | SIGN_KEY_DIST | ID_KEY_DIST | LINK_KEY_DIST)
#endif
#endif

#define PERIPHERAL1 0
#define PERIPHERAL2 1
#define PERIPHERAL3 2
#define CENTRAL1    RSI_BLE_MAX_NBR_PERIPHERALS
#define CENTRAL2    RSI_BLE_MAX_NBR_PERIPHERALS + 1

//! Notify status
#define NOTIFY_DISABLE 0x00
#define NOTIFY_ENABLE  0x01
// write status
#define WRITE_ENABLE  0x01
#define WRITE_DISABLE 0x00
//! Indication status
#define INDICATION_DISABLE 0x00
#define INDICATION_ENABLE  0x02
#define BIT64(a)           ((long long int)1 << a)
//! error codes
#define RSI_ERROR_IN_BUFFER_ALLOCATION   0x4e65
#define RSI_END_OF_PROFILE_QUERY         0x4a0a
#define RSI_TO_CHECK_ALL_CMD_IN_PROGRESS 0

/***********************************************************************************************************************************************/
//! Characteristic Presenatation Format Fields
/***********************************************************************************************************************************************/
#define RSI_BLE_UINT8_FORMAT          0x04
#define RSI_BLE_EXPONENT              0x00
#define RSI_BLE_PERCENTAGE_UNITS_UUID 0x27AD
#define RSI_BLE_NAME_SPACE            0x01
#define RSI_BLE_DESCRIPTION           0x010B

/***********************************************************************************************************************************************/
//! Notification related macros
/***********************************************************************************************************************************************/
#define HEART_RATE_PROFILE_UUID                0x180D     //! Heart rate profile UUID
#define HEART_RATE_MEASUREMENT                 0x2A37     //! Heart rate measurement
#define APPLE_NOTIFICATION_CENTER_SERVICE_UUID 0x7905F431 //! Apple notification center service UUID
#define APPLE_NOTIFICATION_SOURCE_UUID         0x9FBF120D //! Apple Notification source uuid

#define ENABLE_NOTIFICATION_SERVICE HEART_RATE_PROFILE_UUID

#if ENABLE_NOTIFICATION_SERVICE == HEART_RATE_PROFILE_UUID
#define ENABLE_NOTIFICATION_UUID HEART_RATE_MEASUREMENT
#elif ENABLE_NOTIFICATION_SERVICE == APPLE_NOTIFICATION_CENTER_SERVICE_UUID
#define ENABLE_NOTIFICATION_UUID APPLE_NOTIFICATION_SOURCE_UUID
#endif

//! BLE characteristic custom service uuid
#define RSI_BLE_CUSTOM_SERVICE_UUID 0x1AA1
#define RSI_BLE_CUSTOM_LEVEL_UUID   0x1BB1

/***********************************************************************************************************************************************/
//! BLE client characteristic service and attribute uuid.
/***********************************************************************************************************************************************/
#define RSI_BLE_NEW_CLIENT_SERVICE_UUID 0x180D //! immediate alert service uuid
#define RSI_BLE_CLIENT_ATTRIBUTE_1_UUID 0x2A37 //! Alert level characteristic

#define BLE_ATT_REC_SIZE 500
#define NO_OF_VAL_ATT    5 //! Attribute value count

/***********************************************************************************************************************************************/
//! Advertising command parameters
/***********************************************************************************************************************************************/
#define RSI_BLE_ADV_INT_MIN 0x152 // 0x06a8
#define RSI_BLE_ADV_INT_MAX 0x152 // 0x06a8

/***********************************************************************************************************************************************/
//! Connection parameters
/***********************************************************************************************************************************************/
#define INITIATING_PHYS (PHY_1M | PHY_2M | PHY_LE_CODED)

#define LE_SCAN_INTERVAL 0x0035
#define LE_SCAN_WINDOW   0x0017

#define CONNECTION_INTERVAL_MIN 0x00A0 //0x00C8
#define CONNECTION_INTERVAL_MAX 0x00A0 //0x00C8

#define SUPERVISION_TIMEOUT 0x07D0 //0x0C80

#define LE_SCAN_INTERVAL_CONN 0x0050
#define LE_SCAN_WINDOW_CONN   0x0050

#define CONNECTION_EVENT_LEN_MIN 0x0000
#define CONNECTION_EVENT_LEN_MAX 0x0000

/***********************************************************************************************************************************************/
//! Connection parameters for SiWx917 as peripheral to remote device as central connection
/***********************************************************************************************************************************************/
#define BLE_MAX_SUPPORETED_SUPERVISION_TIMEOUT 0x0C80
#define P2C_CONNECTION_INTERVAL_MIN            0x00C8
#define P2C_CONNECTION_INTERVAL_MAX            0x00C8

#define P2C_CONNECTION_LATENCY  0x0000
#define P2C_SUPERVISION_TIMEOUT MIN(BLE_MAX_SUPPORETED_SUPERVISION_TIMEOUT, (4 * P2C_CONNECTION_INTERVAL_MAX))

/***********************************************************************************************************************************************/
//! Connection parameters for SiWx917 as central to remote device as peripheral connection
/***********************************************************************************************************************************************/
#define C2P12_CONNECTION_INTERVAL_MIN 0x00C8
#define C2P12_CONNECTION_INTERVAL_MAX 0x00C8

#define C2P12_CONNECTION_LATENCY  0x0000
#define C2P12_SUPERVISION_TIMEOUT MIN(BLE_MAX_SUPPORETED_SUPERVISION_TIMEOUT, (4 * C2P12_CONNECTION_INTERVAL_MAX))

/***********************************************************************************************************************************************/
//! Connection parameters for SiWx917 as central to remote device as peripheral connection
/***********************************************************************************************************************************************/
#define C2P34_CONNECTION_INTERVAL_MIN 0x0190
#define C2P34_CONNECTION_INTERVAL_MAX 0x0190

#define C2P34_CONNECTION_LATENCY  0x0000
#define C2P34_SUPERVISION_TIMEOUT MIN(BLE_MAX_SUPPORETED_SUPERVISION_TIMEOUT, (4 * C2P34_CONNECTION_INTERVAL_MAX))

/***********************************************************************************************************************************************/
//! Connection parameters for SiWx917 as central to remote device as peripheral connection
/***********************************************************************************************************************************************/
#define C2P56_CONNECTION_INTERVAL_MIN 0x0320
#define C2P56_CONNECTION_INTERVAL_MAX 0x0320

#define C2P56_CONNECTION_LATENCY  0x0000
#define C2P56_SUPERVISION_TIMEOUT MIN(BLE_MAX_SUPPORETED_SUPERVISION_TIMEOUT, (4 * C2P56_CONNECTION_INTERVAL_MAX))

/***********************************************************************************************************************************************/

#define BLE_OUTPUT_POWER_FRONT_END_LOSS \
  0 /* This define value is 2 incase of EVK on screen verification, 0 at Antenna (used by wearables). Used in dBm to index conversion, per role Tx power  */

/**
 * Bit positions for BLE state bitmap i.e rsi_ble_states_bitmap
 */
#define RSI_SCAN_STATE 1
#define RSI_ADV_STATE  1

#endif

// enable this, to send writes without responses
#define SEND_W_W_R                   0
#define SEND_NOTIFICATION            0
#define RSI_BLE_MAX_CHAR_DESCRIPTORS 5

#define CENTRAL_ROLE    2
#define PERIPHERAL_ROLE 1

#define NO_PERIPHERAL_FOUND      0
#define PERIPHERAL_FOUND         1
#define PERIPHERAL_CONNECTED     2
#define PERIPHERAL_NOT_CONNECTED 3
#define PROTOCOL_SEL             "protocol <"
#define BLE_CONF                 "ble_conf <"
#define BLE_REMOTE_NAME          "ble_remote_names <"
#define BLE_REMOTE_ADDR          "ble_remote_addr <"
#define PERIPHERAL1_CONF         "peripheral1 <"
#define PERIPHERAL2_CONF         "peripheral2 <"
#define PERIPHERAL3_CONF         "peripheral3 <"
#define CENTRAL1_CONF            "central1 <"
#define CENTRAL2_CONF            "central2 <"
#define BT_CONFIG                "bt_conf <"
#define LINE1                    1
#define LINE2                    2
#define LINE3                    3
#define LINE4                    4
#define LINE5                    5
#define LINE6                    6
#define LINE7                    7
#define LINE8                    8
#define LINE9                    9
#define LINE10                   10
//#define DEMO_RING_BUFFER_SIZE 1000 // 300

/*=======================================================================*/
//! Power save command parameters
/*=======================================================================*/
//! set handshake type of power mode
#ifdef RSI_M4_INTERFACE
#define RSI_HAND_SHAKE_TYPE M4_BASED
#else
#define RSI_HAND_SHAKE_TYPE GPIO_BASED
#endif

/*=======================================================================*/
// Extended Advertising Opermode parameters
/*=======================================================================*/
#define RSI_BLE_ENABLE_ADV_EXTN 1 // disabled by default
#define RSI_BLE_AE_MAX_ADV_SETS 2 // default number of Advertising sets in extended advertising (Max value = 15)

#ifdef FW_LOGGING_ENABLE
// Firmware Logging Parameters
/* ================================================================================== */
//! Enable or Disable firmware logging (Enable = 1; Disable = 0)
#define FW_LOG_ENABLE 1
//! Set TSF Granularity for firmware logging in micro seconds
#define FW_TSF_GRANULARITY_US 1
//! Log level for COMMON component in firmware
#define COMMON_LOG_LEVEL FW_LOG_INFO
//! Log level for CM_PM component in firmware
#define CM_PM_LOG_LEVEL FW_LOG_INFO
//! Log level for WLAN_LMAC component in firmware
#define WLAN_LMAC_LOG_LEVEL FW_LOG_INFO
//! Log level for WLAN_UMAC component in firmware
#define WLAN_UMAC_LOG_LEVEL FW_LOG_INFO
//! Log level for WLAN NETWORK STACK component in firmware
#define WLAN_NETSTACK_LOG_LEVEL FW_LOG_INFO
//! Log level for BT BLE CONTROL component in firmware
#define BT_BLE_CTRL_LOG_LEVEL FW_LOG_INFO
//! Log level for BT BLE STACK component in firmware
#define BT_BLE_STACK_LOG_LEVEL FW_LOG_TRACE
//! Min Value = 2048 bytes; Max Value = 4096 bytes; Value should be in multiples of 512 bytes
#define FW_LOG_BUFFER_SIZE 2048
#define FW_LOG_QUEUE_SIZE  2
#endif

#include "rsi_ble_common_config.h"