# WiSeConnect Release Notes

Release notes are provided with each version of the [WiSeConnect&trade; SDK](https://github.com/SiliconLabs/wiseconnect-wifi-bt-sdk).

  - [WiSeConnect&trade; 2.11.3](#wiseconnect-2113-release-notes)
  - [WiSeConnect&trade; 2.11.2](#wiseconnect-2112-release-notes)
  - [WiSeConnect&trade; 2.11.1](#wiseconnect-2111-release-notes)
  - [WiSeConnect&trade; 2.11](#wiseconnect-211-release-notes)
  - [WiSeConnect&trade; 2.10.4](#wiseconnect-2104-release-notes)
  - [WiSeConnect&trade; 2.10.3](#wiseconnect-2103-release-notes)
  - [WiSeConnect&trade; 2.10.2](#wiseconnect-2102-release-notes)
  - [WiSeConnect&trade; 2.10.1](#wiseconnect-2101-release-notes)
  - [WiSeConnect&trade; 2.10](#wiseconnect-210-release-notes)
  - [WiSeConnect&trade; 2.9.2](#wiseconnect-292-release-notes)
  - [WiSeConnect&trade; 2.9.1-Qual](#wiseconnect-291-qual-release-notes)
  - [WiSeConnect&trade; 2.9.1](#wiseconnect-291-release-notes)
  - [WiSeConnect&trade; 2.9.0](#wiseconnect-290-release-notes)
  - [WiSeConnect&trade; 2.8.2](#wiseconnect-282-release-notes)
  - [WiSeConnect&trade; 2.8.1](#wiseconnect-28-release-notes)
  - [WiSeConnect&trade; 2.8](#wiseconnect-28-release-notes)
  - [WiSeConnect&trade; 2.7.3](#wiseconnect-273-release-notes)
  - [WiSeConnect&trade; 2.7.1](#wiseconnect-271-release-notes)
  - [WiSeConnect&trade; 2.7](#wiseconnect-27-release-notes)
  - [WiSeConnect&trade; 2.6.1](#wiseconnect-261-release-notes)
  - [WiSeConnect&trade; 2.6](#wiseconnect-26-release-notes)
  - [WiSeConnect&trade; 2.5.2](#wiseconnect-252-release-notes)
  - [WiSeConnect&trade; 2.5.1.6](#wiseconnect-2516-release-notes)  
  - [WiSeConnect&trade; 2.5.1](#wiseconnect-251-release-notes)  
  - [WiSeConnect&trade; 2.5](#wiseconnect-25-release-notes)
  - [WiSeConnect&trade; 2.4.1.19](#wiseconnect-24119-release-notes)
  - [WiSeConnect&trade; 2.4.1](#wiseconnect-241-release-notes)
  - [WiSeConnect&trade; 2.4](#wiseconnect-24-release-notes)
  - [WiSeConnect&trade; 2.3](https://www.silabs.com/documents/login/release-notes/RS9116W-IoT-Software-2.3-release-notes.pdf)
  - [WiSeConnect&trade; 2.0](https://www.silabs.com/documents/login/release-notes/rs9116w-iot-software-2.0-release-notes.pdf)

# WiSeConnect 2.11.3 Release Notes

Last updated: February 24, 2025

## Highlights
Bug fix

## Release Details
| Item                                                | Details                                |
|-----------------------------------------------------|----------------------------------------|
| API Version (SAPI)                                  | 2.11.3.2 (Build 2)                     |
| Firmware Version                                    | 2.11.3.0.2 (Build 2)                   |
| Package Name                                        | RS9116W.2.11.3.2                       |
| Supported RTOS                                      | FreeRTOS                               |
| Hardware Modules                                    | QMS, B00, C00, CC1, AB0, AB1, AA0, AA1, AC0, AC1 |
| Hardware Chipsets                                   | Chip revision 1.3, Chip revision 1.4, Chip revision 1.5  |
| Operating Modes Supported                           | Wi-Fi STA, Wi-Fi AP, Wi-Fi STA + BLE, Wi-Fi STA + AP     |
| Additional Operating Modes Supported                | Wi-Fi STA + BT, Wi-Fi STA + BT + BLE , BLE (with AE)     |

### Notes
The current release has bug fixes, enhancements, and new features in both 'SAPIs', and 'Firmware'. Therefore, it is recommended to update and use the same version for both 'SAPIs', and 'Firmware'.

## Updates in this Release
RS9116W 2.11.3.2 release consists of two components:
  - Firmware - RS9116 Firmware Binary
  - SAPI Library - SAPI Library runs on Host

This release is meant only for use with designs based on RS9116 Silicon revision 1.4 (RS9116X-xxx-xxx-Bxx), and RS9116 Silicon revision 1.5 (RS9116X-xxx-xxx-Cxx).
Customers using the Silicon revision 1.3 (RS9116X-xxx-xxx-Xxx) parts can upgrade to the latest firmware. However, the power optimization feature macro EXT_FEAT_LOW_POWER_MODE in rsi_wlan_config.h must be disabled for Silicon revision 1.3.


Select the appropriate firmware version for your specific Silicon revision as its a part of your design. Following Silicon revisions are available:
- RS9116W.2.11.3.0.2.rps
  - RS9116X-xxx-xxx-Xxx (Silicon revision 1.3)
  - RS9116X-xxx-xxx-Bxx (Silicon revision 1.4)
- RS916W.2.11.3.0.2.rps
  - RS9116X-xxx-xxx-Cxx (Silicon revision 1.5)
  - RS916WS00ACxxx

## New Features
Following are the new features incorporated with this version:

### Wi-Fi
None

### System
  - Added a support to disable power save during device de-initialization process.

### BT-Classic/BLE
None

### Network Stack
None

### Coexistence
None

## Changes and Fixes
Following is the list of fixes, and changes outlined for this version:

### Wi-Fi
  - Renewed the EAP certificates (wifiuser.pem) for enterprise certificate based authentication. This certificates have Pathlength enabled by default. The wifiuser.pem is from Wi-Fi Alliance validation package for EAP certificate based Wi-Fi connection. This certificate is for validation purposes only, and should not be used for commercial purpose.

### Network Stack
  - Added code support to process maximim Rx read under the macro RSI_PROCESS_MAX_RX_DATA. If the RSI_PROCESS_MAX_RX_DATA is enabled, it might result in low throughput values.

### BT-Classic/BLE
  - Addressed the issue with the 'just works' method to distinguish it from the numeric comparison.
  - Fixed the IOP issue, DUT is unable to establish the connection with Samsung M14. Here mobile is sending sdp_serviceattribute_request, which contains additional data bytes without a continuous state.

### Coexistence
None

### System
  - Fixed warnings when rsi_get_rtc_timer API is used.

### Documentation
  - Updated the WPA/WPA2 Security Mode under at+rsi_scan and at_rsi_join of the AT-PRM and rsi_scan and rsi_join of SAPI PRM.
  - Updated AT and SAPI PRMs for the command rsi_get_rtc_timer.
  - Updated the note on the RSI_PROCESS_MAX_RX_DATA macro.
  - Added a note on the rsi_wlan_ping_async() API and increased the ping timeout value.
  - Updated scan APIs related description in DSC.

## Recommendations and New Options
Following are the list of recommendations, and new options:

### Wi-Fi
  - Enable aggregation using the opermode parameter [feature_bit_map\[2\]](https://docs.silabs.com/rs9116-wiseconnect/2.6/wifibt-wc-sapi-reference/opermode#rsi-feature-bit-map).
  - Disable high MCS rates to improve the robustness of a connection for low throughput applications in congested networks. For instance, turning off `MCS6`, and `MCS7` may help a Smart Lock in a congested wireless environment.
  - To restart the RS9116, the application should call `rsi_driver_deinit()` followed by `rsi_driver_init()`, and `rsi_device_init()`. For OS cases, additionally call `rsi_task_destroy(driver_task_handle)` to delete the driver task before calling `rsi_driver_deinit()`, and create again after `rsi_device_init()` using `rsi_task_create()`.

### Network Stack
  - To ensure graceful handling during asynchronous TCP closures from a peer, enable the opermode parameter [ext_tcp_ip_feature_bit_map\[16\]](https://docs.silabs.com/rs9116-wiseconnect/2.6/wifibt-wc-sapi-reference/opermode#rsi-ext-tcpip-feature-bitmap).
  - To avoid TCP disconnects during a rejoin, set TCP retransmission count to >= `30`.
  - Implemented a new API - `rsi_network_app_protocol_config()` to configure the network application protocols such as, cipher selection for HTTP protocol.

### BT-Classic/BLE
  - Following is the recommended range for the BLE Connection Interval:
    - Powersave (BLE only) - 100 ms to 1.28 s
    - BT Classic + BLE Dual Mode is >= 200 ms
    - Wi-Fi + BLE coex - 30 to 250 ms
  - During connection in BLE, it is not recommended to configure the scan window, and scan interval with the same value.
  - In BT Classic, the recommended Sniff Interval configuration during powersave is limited to 100 ms (\<= 100).
  - In BLE, if a device is acting as central, the scan window (in `set_scan_params()`, and `create_connection()` APIs) must be less than the existing connection interval.
  - In BLE mode, if scanning, and advertising is in progress, and the device subsequently connects, and moves to the central role, scanning, and advertising stops. To further establish connection to another peripheral device or to a central device, the application must initiate advertising, and scanning again.
  - Device powersave must be disabled prior to BT initialization and deinitialization.

### Coexistence
  - For concurrent Wi-Fi + BLE, and while a Wi-Fi connection is active, we recommend setting the ratio of the BLE scan window to BLE scan interval to 1:3 or 1:4.
  - Wi-Fi + BLE Advertising
    - All standard advertising intervals are supported. As Wi-Fi throughput is increased, a slight difference in on-air advertisements compared to configured intervals might be observed.
    - BLE advertising is skipped if the advertising interval collides with Wi-Fi activity.
  - Wi-Fi + BLE scanning
    - All standard scan intervals are supported. For better scan results, we recommend setting the ratio of the BLE scan window to BLE scan interval to 1:3 or 1:4.
    - BLE scan would be stopped for intervals that collide with Wi-Fi activity.
  - Wi-Fi + BLE Central/Peripheral Connections
    - All standard connection intervals are supported.
    - For a stable connection, use optimal connection intervals, and maximum supervision timeout in the presence of Wi-Fi activity.
  - Wi-Fi + BLE Central/Peripheral Data Transfer
    - To achieve higher throughput for both Wi-Fi, and BLE, use medium connection intervals, such as 45 to 80 ms with maximum supervision timeout.
    - Ensure Wi-Fi activity consumes lower intervals.
  - To ensure a stable, and seamless connection, connect Wi-Fi before BT/BLE connections in the following operating modes: Wi-Fi + BT + BLE, Wi-Fi + BT, and Wi-Fi + BLE.
  - For Wi-Fi + BT + BLE, Wi-Fi + BT, and Wi-Fi + BLE operating modes, if BT/BLE must be connected before Wi-Fi, use with a high supervision timeout, a high connection interval for BLE, and a high sniff interval for BT to ensure seamless and stable connection. This configuration ensures that the BT/BLE connection stays stable when Wi-Fi connects/disconnects/rejoins.

### System
  - For User Store configuration and Configuration Save, do not enable powersave or save it as a configuration. If powersave is enabled and saved as a configuration, then on boot up, the RS9116 will boot with the saved configuration. In this scenario, The RS9116 will go to powersave mode without giving any indication to the host.
  - Set the compiler optimization level to `O0` in project settings for IDE (KEIL).
  - Memory configuration must be 384K for BT/BLE and coex operating modes.
    - Usage of low-power flash mode bit (bit 19 in extended customer feature bitmap). Enable this bit for ultra-low-power standby associated scenarios. This results in about 20&micro;A lower Wi-Fi standby associated current consumption.
  - Memory Configuration in SAPI functions: The default memory configuration is 384K.
  - Apply Opermode commands in AT mode correctly. Using the wrong opermode might lead to unspecified behavior.
  - Set the recommended Power Save Profile (PSP) type to Enhanced Max PSP.
  - It is recommended to disable the powersave functionality while updating the firmware.
  - For high throughput applications, powersave operation should be disabled.
  - The application is required to set the real-time clock with the correct timestamp when the feature is enabled before establishing a TLS connection.
  - At least one second timeout is a necessity for socket select and receive. No timeout value under one second is supported.

## Known Issues
Following are the list of known issues:

### Network Stack
  - Reassembly of fragmented MQTT packets is not supported in embedded MQTT. MQTT transactions may fail with fragmented packets received from the broker.
  - Occasionally, during a TLS handshake, ECC curve parameters are generated incorrectly, which results in connection failure with BBD2 error. However, this gets recover in next attempt.
  - During firmware updates, MQTT disconnects when power savings is enabled. During the firmware update, disable powersave and re-enable it on updated completion.
  - All GCM-based cipher suites are implemented in software and have limited performance.
  - The recommended MQTT publish payload is 1 kBytes.
  - If HTTP server functionality is enabled, do not use port 80 for the MQTT client.
  - Randomize the client port if using rapid connect/disconnect of the MQTT session on the same client port with powersave.
  - Secure TLS renegotiation is not supported in the embedded networking stack.
  - In WPA2 enterprise security mode, getting wireless statistics using rsi_wlan_get() API has restrictions.
  - Example application wlan_throughtput_bt_spp_ble_dual_role is unsupported with bare metal.
  - In concurrent mode, there could be a loss of communication on the STA application when, simultaneously, multiple clients are connected to the APUT.
  - IP address conflict detection has limitations in STA + AP concurrent mode, failure is not reported to host.
  - mqtt_client, and mqtt_client_v6 applications does not support SDIO host interface.
  - During firmware update via OTA (TCP/HTTP) methods, if the firmware image received is corrupted, it can result in the module going unresponsive on the next bootup.
  - TLS connection problems may be observed when attempting to revoke public and private keys using existing keys.
  - Observed STA + BT (Connected) test cases not getting throughput results prints in teraterm.
  - Observed inconsistency in TCP and UDP - Rx throughputs in AP alone mode.
  - Observed duplicate IPv6 address is assigned to multiple STAs in AP mode.
  - Ping timeout may be observed with 1 second ping timeout configuration.

### Wi-Fi
  - WPA3 Connection with Hunting and Pecking algorithm takes about three to four seconds.
  - If the station performs a scan in concurrent mode (Wi-Fi STA + AP), stations connected to the AP may be disconnected. Enable AP after the STA connection is completed.
  - With powersave, PTA 3-wire co-existence does not work. However, if a weak pull-up is applied on the GRANT pin, the PTA 3-wire functionality will work while the power is saved. For further information, please refer to Application Notes.
  - EAP security in the aws_device_shadow application is unsupported on the EFR platform due to memory constraints.
  - In STA + AP, If AP started first station scan is supported only in channel in which AP is brought up. If multiple channel scan is requested, AP behaviour is undefined.
  - Auto channel selection with EXT_FEAT_IEEE_80211J feature enabled is not supported in AP operating mode.
  - Testing for traffic differentiation within a single BSS with 802.11n stations is yielding failures during the 11n certification process.

### BT-Classic/BLE
  - Wake on wireless is not supported for BLE.
  - BT-HID might not interoperate with Apple devices.
  - For BLE, dual role scenario issues might occur when connecting as a central when the peripheral is advertising. Central connections can be made after peripheral connections are established and advertising stops.
  - A BLE disconnection would occur if both the peer and RS9116 already have bond information. The RS9116 initiates security as part of BLE secure connections. The work around for this issue is the application should not initiate a security request, if bond information is already present.
  - A2DP music streaming glitches may be observed if a Wi-Fi download is in progress at the same time.
  - RTOS support is not there in ae_peripheral, and ae_central applications.
  - AE connection issue is expected with Samsung mobiles.
  - BT inquiry would not work in temperature range from -40 to 0 C.
  - When DUT is configured as a peripheral with powersave, repeated string of passkey is observed in multiple iterations of reconnection.

### Coexistence
  - In Wi-Fi + BLE mode:
    - BLE may disconnect when the supervision timeout is configured to be less than 16 seconds.
    - WPS does not work.
  - In Wi-Fi + BLE, or BLE only modes:
    - When Wi-Fi disconnects, a BT/BLE reconnection issue is observed.
  - In Wi-Fi + BT mode:
    - When BT SPP is connected, Wi-Fi throughput prints are not displayed in serial terminal applications (like Teraterm) with wlan_bt_throughput_app demo.
  - The wlan_https_bt_a2dp_ble_dual_role and wlan_https_bt_a2dp demos are not supported.
  - In Wi-Fi + BT/BLE CoEx mode, high Wi-Fi broadcast traffic might cause BT/BLE disconnections.
  - In Wi-Fi + BT mode when there is continuous Wi-Fi data, you might observe BT not re-connecting to the remote after disconnection.
  - While downloading a file through a Wi-Fi OTA upgrade, an error code -43 (0xffffffd5) was observed, indicating an error in the WLAN command.

### Interoperability
Wi-Fi client mode might disconnect if an AP does not acknowledge QoS null frames with powersave configured.

### System
  - Chip Revision 1.3 does not support low-power mode optimization using the bit setting defined by EXT_FEAT_LOW_POWER_MODE; setting this bit results in a hang issue.
  - Sometimes the RS9116 enters powersave mode before the configured monitor interval for inactivity when coex is configured.
  - The FTP client application does not work with EFx32/STM32 when an RTOS is used.
  - The HTTP OTA update application does not work with EFR32 and EFM32 when an RTOS is used.
  - Firmware update does not work for the EFM32 platform.
  - SAPI driver with UART host interface is supported only on bare metal.
  - Powersave without RAM retention does not work when the SPI interface is used.
  - MQTT Receive may encounter failures in STA + AP concurrent mode.
  - Standby associated listen interval current consumption is higher than expected with EFM Platform.

## Limitations and Unsupported Features
Following are the list of limitations and unsupported features:

### Wi-Fi/Network Stack
  - AMSDU transmit is not supported.
  - UAPSD is not supported.
  - Fragmentation is not supported.
  - AMSDU's within AMPDU is not supported.
  - Currently, the RS9116 does not support the radio measurement requests feature of CCX V2.
  - 802.11k is not supported.
  - Short GI is not supported.
  - 2.4 and 5 GHz bands does not support 40 MHz bandwidth.
  - 802.11j channels less than 16 are not supported.
  - The USB host interface and USB powersave are not supported.
  - In AT mode, the total MQTT command length (apart from MQTT publish) should not exceed `150` bytes. This includes `at+rsi` (start of command) to (end of command)`\r\n`.
  - Configure `MQTT_VERSION` in `rsi_mqtt_client.h` based on the server configuration; only version 3 and 4 are supported.
  - 3xTLS connections or 1xTCP and 2xTLS connections are supported concurrently in Wi-Fi only mode.
  - TLS curve IDs supports from 15-28. TLS handshake with third party clients depends on the TLS curve.
  - Big endian architecture is not supported.
  - Auto PAC Provisioning in EAP-FAST with TLSv1.2 is not supported.
  - RC4_SHA, DES_CBC3_SHA, and RC4_MD5 ciphers are not supported in enterprise security.
  - SA query procedure not supported in 11W AP mode.
  - If background scan is enabled along with custom feature bit map bit (8) configuration of the DFS channel support, issues may occur in obtaining an IP address for the Wi-Fi STA.
  - In AP mode, IPv6 Router Advertisements are not supported. Although STA connected to the APUT can transfer IPv6 data to the peers by using SLAAC for address configuration.
  - Transparent mode is unsupported in USB-CDC mode.
  - Channel 144 is not supported.
  - IOP issues may be observed with device running DHCPv6 server with Android devices, as latter do not support DHCPv6 process for IPv6 address configuration.
  - WPA3 AP mode is supported with alpha-level quality.
  - WPA3 AP supports only H2E algorithm.
  - WPA3 AP PMKSA is not supported.
  - WPA3 AP transition mode is not supported.
  - AP standalone mode does not support Tx aggregation. Rx aggregation is supported with limited number of BA sessions.
  - In concurrent mode - AP mode, aggregation (Tx/Rx) is not supported.

### BT-Classic/BLE
  - BT A2DP is only supports for RS9116 Silicon revision 1.5.
  - BT sniff mode does not work if BT multiple slaves feature is enabled.
  - For BLE, if the connection is established with a small connection interval (<15 ms), simultaneous roles are not supported (that is Central/Peripheral + Advertising/Scanning).
  - For BT Classic, only one connection is supported at a time.
  - BLE slave latency value is valid up to 32 only.
  - BT-A2DP encoder is not supported in the firmware.
  - BT-A2DP is not supported in AT mode.
  - Maximum supported  AE data length is 200 bytes.
  - Supports only two ADV_EXT sets.
  - Maximum two BLE connections are supported (one central + one peripheral).
  - SMP is not supported in AE applications "ble_ae_central", and "ble_ae_peripheral".
  - GATT transactions, and Data transfer are not supported in AE applications "ble_ae_central", and "ble_ae_peripheral".
  - BLE duty cycling is not supported.
  - BLE does not support User_Config method.
  - In the rsibt_spptxv2 command, if the data length field value exceeds the actual data bytes, the firmware would continuously poll for the data, which results in no response or can cause it to get stuck.

### Coexistence
  - Wi-Fi STA + BT + BLE multiprotocol use cases requires a detailed understanding of use cases and associated configurations. Contact Silicon Labs support for additional details.
  - Wi-Fi AP + BLE, Wi-Fi AP + BT, and Wi-Fi AP + BT + BLE modes are not supported.
  - For AT commands, Wi-Fi + BT + BLE (Opermode 9), BT + BLE (Opermode 8) does not work.
  - In Wi-Fi + BLE mode, if the BLE scan interval and window have the same value, then the Wi-Fi connection might be unsuccessful.
  - Wi-Fi + AE Coexistence:
    - Maximum supported BLE scan interval and window for 100 % duty cycling  is three seconds.
    - Enable powersave to test the WiFi + AE coexistence use cases.
    - Remove the bond information on remote device for every connection if SMP is enabled.
    - Maximum two extended Advertising sets are supported.
    - Maximum two BLE connections are supported (one central + one peripheral).

### System
  - Wake on Wireless support has only been tested for the UART AT command interface.
  - Firmware update via bootloader method requires the user to check the integrity of the RPS file content. A corrupt image may cause the chip to go unresponsive to host commands.

## Notes
### BT-Classic/BLE
 - Acceptlist address should be given in reverse order, that is in big endian format.
 - AE feature validated using revision 1.5 AC1 expansion board.
 - AE feature is not supported in RS9116 revision 1.4.

# WiSeConnect 2.11.2 Release Notes

Last updated: January 17, 2025

## Highlights
Bug fix

## Release Details
| Item                                                | Details                                |
|-----------------------------------------------------|----------------------------------------|
| API Version (SAPI)                                  | 2.11.2.3 (Build 3)                     |
| Firmware Version                                    | 2.11.2.0.3 (Build 3)                   |
| Package Name                                        | RS9116W.2.11.2.3                       |
| Supported RTOS                                      | FreeRTOS                               |
| Hardware Modules                                    | QMS, B00, C00, CC1, AB0, AB1, AA0, AA1, AC0, AC1 |
| Hardware Chipsets                                   | Chip revision 1.3, Chip revision 1.4, Chip revision 1.5  |
| Operating Modes Supported                           | Wi-Fi STA, Wi-Fi AP, Wi-Fi STA + BLE, Wi-Fi STA + AP     |
| Additional Operating Modes Supported                | Wi-Fi STA + BT, Wi-Fi STA + BT + BLE , BLE (with AE)     |

### Notes
The current release has bug fixes, enhancements, and new features in both 'SAPIs', and 'Firmware'. Therefore, it is recommended to update and use the same version for both 'SAPIs', and 'Firmware'.

## Updates in this Release
RS9116W 2.11.2.3 release consists of two components:
  - Firmware - RS9116 Firmware Binary
  - SAPI Library - SAPI Library runs on Host

This release is meant only for use with designs based on RS9116 Silicon revision 1.4 (RS9116X-xxx-xxx-Bxx), and RS9116 Silicon revision 1.5 (RS9116X-xxx-xxx-Cxx).
Customers using the Silicon revision 1.3 (RS9116X-xxx-xxx-Xxx) parts can upgrade to the latest firmware. However, the power optimization feature macro EXT_FEAT_LOW_POWER_MODE in rsi_wlan_config.h must be disabled for Silicon revision 1.3.


Select the appropriate firmware version for your specific Silicon revision as its a part of your design. Following Silicon revisions are available:
- RS9116W.2.11.2.0.3.rps
  - RS9116X-xxx-xxx-Xxx (Silicon revision 1.3)
  - RS9116X-xxx-xxx-Bxx (Silicon revision 1.4)
- RS916W.2.11.2.0.3.rps
  - RS9116X-xxx-xxx-Cxx (Silicon revision 1.5)
  - RS916WS00ACxxx

## New Features
Following new features are implemented with this version:

### Wi-Fi
None

### System
None

### BT-Classic/BLE
The maximum data length for AT SPP transmission has been increased from 200 to 512 bytes.

### Network Stack
None

### Coexistence
None

## Changes and Fixes
The list of fixes, and changes for this version are outlined further:

### Wi-Fi
None

### Network Stack
None

### BT-Classic/BLE
  - Fixed error code -43 (FFFFD5) while downloading the file through BTC-SPP (Serial Port Profile).
  - Fixed Protocol Test Suite (PTS) failures observed during Serial Port Profile (SPP), Logical Link Control and Adaptation Protocol (L2CAP) and Service Discovery Protocol (SDP) testing.

### Coexistence
None

### System
None

### Documentation
Updated the spptxv2 command documentation in BT classic AT PRM.

## Recommendations and New Options
Following are the list of recommendations, and new options:

### Wi-Fi
  - Enable aggregation using the opermode parameter [feature_bit_map\[2\]](https://docs.silabs.com/rs9116-wiseconnect/2.6/wifibt-wc-sapi-reference/opermode#rsi-feature-bit-map).
  - Disable high MCS rates to improve the robustness of a connection for low throughput applications in congested networks. For instance, turning off `MCS6`, and `MCS7` may help a Smart Lock in a congested wireless environment.
  - To restart the RS9116, the application should call `rsi_driver_deinit()` followed by `rsi_driver_init()`, and `rsi_device_init()`. For OS cases, additionally call `rsi_task_destroy(driver_task_handle)` to delete the driver task before calling `rsi_driver_deinit()`, and create again after `rsi_device_init()` using `rsi_task_create()`.

### Network Stack
  - To ensure graceful handling during asynchronous TCP closures from a peer, enable the opermode parameter [ext_tcp_ip_feature_bit_map\[16\]](https://docs.silabs.com/rs9116-wiseconnect/2.6/wifibt-wc-sapi-reference/opermode#rsi-ext-tcpip-feature-bitmap).
  - To avoid TCP disconnects during a rejoin, set TCP retransmission count to >= `30`.
  - Implemented a new API - `rsi_network_app_protocol_config()` to configure the network application protocols such as, cipher selection for HTTP protocol.

### BT-Classic/BLE
  - In BLE, the recommended range of the BLE Connection Interval is as follows:
    - Powersave (BLE only) - 100 ms to 1.28 s
    - BT Classic + BLE Dual Mode is >= 200 ms
    - Wi-Fi + BLE coex - 30 to 250 ms
  - During connection in BLE, it is not recommended to configure the scan window, and scan interval with the same value.
  - In BT Classic, the recommended Sniff Interval configuration during powersave is limited to 100 ms (\<= 100).
  - In BLE, if a device is acting as central, the scan window (in `set_scan_params()`, and `create_connection()` APIs) must be less than the existing connection interval.
  - In BLE mode, if scanning, and advertising is in progress, and the device subsequently connects, and moves to the central role, scanning, and advertising stops. To further establish connection to another peripheral device or to a central device, the application must initiate advertising, and scanning again.
  - Device powersave must be disabled prior to BT initialization and deinitialization.

### Coexistence
  - For concurrent Wi-Fi + BLE, and while a Wi-Fi connection is active, we recommend setting the ratio of the BLE scan window to BLE scan interval to 1:3 or 1:4.
  - Wi-Fi + BLE Advertising
    - All standard advertising intervals are supported. As Wi-Fi throughput is increased, a slight difference in on-air advertisements compared to configured intervals might be observed.
    - BLE advertising is skipped if the advertising interval collides with Wi-Fi activity.
  - Wi-Fi + BLE scanning
    - All standard scan intervals are supported. For better scan results, we recommend setting the ratio of the BLE scan window to BLE scan interval to 1:3 or 1:4.
    - BLE scan would be stopped for intervals that collide with Wi-Fi activity.
  - Wi-Fi + BLE Central/Peripheral Connections
    - All standard connection intervals are supported.
    - For a stable connection, use optimal connection intervals, and maximum supervision timeout in the presence of Wi-Fi activity.
  - Wi-Fi + BLE Central/Peripheral Data Transfer
    - To achieve higher throughput for both Wi-Fi, and BLE, use medium connection intervals, such as 45 to 80 ms with maximum supervision timeout.
    - Ensure Wi-Fi activity consumes lower intervals.
  - To ensure a stable, and seamless connection, connect Wi-Fi before BT/BLE connections in the following operating modes: Wi-Fi + BT + BLE, Wi-Fi + BT, and Wi-Fi + BLE.
  - For Wi-Fi + BT + BLE, Wi-Fi + BT, and Wi-Fi + BLE operating modes, if BT/BLE must be connected before Wi-Fi, use with a high supervision timeout, a high connection interval for BLE, and a high sniff interval for BT to ensure seamless and stable connection. This configuration ensures that the BT/BLE connection stays stable when Wi-Fi connects/disconnects/rejoins.

### System
  - For User Store configuration and Configuration Save, do not enable powersave or save it as a configuration. If powersave is enabled and saved as a configuration, then on boot up, the RS9116 will boot with the saved configuration. In this scenario, the RS9116 will go to powersave mode without giving any indication to the host.
  - Set the compiler optimization level to `O0` in project settings for IDE (KEIL).
  - Memory configuration must be 384K for BT/BLE and coex operating modes.
    - Usage of low-power flash mode bit (bit 19 in extended customer feature bitmap). Enable this bit for ultra-low-power standby associated scenarios. This results in about 20&micro;A lower Wi-Fi standby associated current consumption.
  - Memory Configuration in SAPI functions: The default memory configuration is 384K.
  - Apply Opermode commands in AT mode correctly. Using the wrong opermode might lead to unspecified behavior.
  - Set the recommended Power Save Profile (PSP) type to Enhanced Max PSP.
  - It is recommended to disable the powersave functionality while updating the firmware.
  - For high throughput applications, powersave operation should be disabled.
  - The application is required to set the real-time clock with the correct timestamp when the feature is enabled before establishing a TLS connection.
  - At least one second timeout is a necessity for socket select and receive. No timeout value under one second is supported.

## Known Issues
Following are the list of known issues:

### Network Stack
  - Reassembly of fragmented MQTT packets is not supported in embedded MQTT. MQTT transactions may fail with fragmented packets received from the broker.
  - Occasionally, during a TLS handshake, ECC curve parameters are generated incorrectly, which results in connection failure with BBD2 error. However, this gets recover in next attempt.
  - During firmware updates, MQTT disconnects when power savings is enabled. During the firmware update, disable powersave and re-enable it on updated completion.
  - All GCM-based cipher suites are implemented in software and have limited performance.
  - The recommended MQTT publish payload is 1 kBytes.
  - If HTTP server functionality is enabled, do not use port 80 for the MQTT client.
  - Randomize the client port if using rapid connect/disconnect of the MQTT session on the same client port with powersave.
  - Secure TLS renegotiation is not supported in the embedded networking stack.
  - In WPA2 enterprise security mode, getting wireless statistics using rsi_wlan_get() API has restrictions.
  - Example application wlan_throughtput_bt_spp_ble_dual_role is unsupported with bare metal.
  - In concurrent mode, there could be a loss of communication on the STA application when, simultaneously, multiple clients are connected to the APUT.
  - IP address conflict detection has limitations in STA + AP concurrent mode, failure is not reported to host.
  - mqtt_client, and mqtt_client_v6 applications does not support SDIO host interface.
  - During firmware update via OTA (TCP/HTTP) methods, if the firmware image received is corrupted, it can result in the module going unresponsive on the next bootup.
  - TLS connection problems may be observed when attempting to revoke public and private keys using existing keys.
  - Observed STA + BT (Connected) test cases not getting throughput results prints in teraterm.
  - Observed inconsistency in TCP and UDP - Rx throughputs in AP alone mode.
  - Observed duplicate IPv6 address is assigned to multiple STAs in AP mode.
  - Ping timeout may be observed with 1 second ping timeout configuration.

### Wi-Fi
  - WPA3 Connection with Hunting and Pecking algorithm takes about three to four seconds.
  - If the station performs a scan in concurrent mode (Wi-Fi STA + AP), stations connected to the AP may be disconnected. Enable AP after the STA connection is completed.
  - With powersave, PTA 3-wire co-existence does not work. However, if a weak pull-up is applied on the GRANT pin, the PTA 3-wire functionality will work while the power is saved. For further information, please refer to Application Notes.
  - EAP security in the aws_device_shadow application is unsupported on the EFR platform due to memory constraints.
  - In STA + AP, If AP started first station scan is supported only in channel in which AP is brought up. If multiple channel scan is requested, AP behaviour is undefined.
  - Auto channel selection with EXT_FEAT_IEEE_80211J feature enabled is not supported in AP operating mode.
  - Testing for traffic differentiation within a single BSS with 802.11n stations is yielding failures during the 11n certification process.

### BT-Classic/BLE
  - Wake on wireless is not supported for BLE.
  - BT-HID might not interoperate with Apple devices.
  - For BLE, dual role scenario issues might occur when connecting as a central when the peripheral is advertising. Central connections can be made after peripheral connections are established and advertising stops.
  - A BLE disconnection would occur if both the peer and RS9116 already have bond information. The RS9116 initiates security as part of BLE secure connections. The work around for this issue is the application should not initiate a security request, it bond information is already present.
  - A2DP music streaming glitches may be observed if a Wi-Fi download is in progress at the same time.
  - RTOS support is not there in ae_peripheral, and ae_central applications.
  - AE connection issue is expected with Samsung mobiles.
  - BT inquiry would not work in temperature range from -40 to 0 C.
  - When DUT is configured as a peripheral with powersave, repeated string of passkey is observed in multiple iterations of reconnection.

### Coexistence
  - After powersave is enabled in coex mode, `radio_init()` must be called to turn off powersave.
  - In Wi-Fi + BLE mode:
    - BLE may disconnect when the supervision timeout is configured to be less than 16 seconds.
    - WPS does not work.
  - In Wi-Fi + BLE, or BLE only modes:
    - When Wi-Fi disconnects, a BT/BLE reconnection issue is observed.
  - In Wi-Fi + BT mode:
    - When BT SPP is connected, Wi-Fi throughput prints are not displayed in Teraterm with wlan_bt_throughput_app demo.
  - The wlan_https_bt_a2dp_ble_dual_role and wlan_https_bt_a2dp demos are not supported.
  - In Wi-Fi + BT/BLE CoEx mode, high Wi-Fi broadcast traffic might cause BT/BLE disconnections.
  - In Wi-Fi + BT mode when there is continuous Wi-Fi data, you might observe BT not re-connecting to the remote after disconnection.
  - While downloading a file through a Wi-Fi OTA upgrade, an error code -43 (0xffffffd5) was observed, indicating an error in the WLAN command.

### Interoperability
Wi-Fi client mode might disconnect if an AP does not acknowledge QoS null frames with powersave configured.

### System
  - Chip Revision 1.3 does not support low-power mode optimization using the bit setting defined by EXT_FEAT_LOW_POWER_MODE; setting this bit results in a hang issue.
  - Sometimes the RS9116 enters powersave mode before the configured monitor interval for inactivity when coex is configured.
  - The FTP client application does not work with EFx32/STM32 when an RTOS is used.
  - The HTTP OTA update application does not work with EFR32 and EFM32 when an RTOS is used.
  - Firmware update does not work for the EFM32 platform.
  - SAPI driver with UART host interface is supported only on bare metal.
  - Powersave without RAM retention does not work when the SPI interface is used.
  - MQTT Receive may encounter failures in STA + AP concurrent mode.
  - Standby associated listen interval current consumption is higher than expected with EFM Platform.

## Limitations and Unsupported Features
Following are the list of limitations and unsupported features:

### Wi-Fi/Network Stack
  - AMSDU transmit is not supported.
  - UAPSD is not supported.
  - Fragmentation is not supported.
  - AMSDU's within AMPDU is not supported.
  - Currently, the RS9116 does not support the radio measurement requests feature of CCX V2.
  - 802.11k is not supported.
  - Short GI is not supported.
  - 2.4 and 5 GHz bands does not support 40 MHz bandwidth.
  - 802.11j channels less than 16 are not supported.
  - The USB host interface and USB powersave are not supported.
  - In AT mode, the total MQTT command length (apart from MQTT publish) should not exceed `150` bytes. This includes `at+rsi` (start of command) to (end of command)`\r\n`.
  - Configure `MQTT_VERSION` in `rsi_mqtt_client.h` based on the server configuration; only version 3 and 4 are supported.
  - 3xTLS connections or 1xTCP and 2xTLS connections are supported concurrently in Wi-Fi only mode.
  - TLS curve IDs supports from 15-28. TLS handshake with third party clients depends on the TLS curve.
  - Big endian architecture is not supported.
  - Auto PAC Provisioning in EAP-FAST with TLSv1.2 is not supported.
  - RC4_SHA, DES_CBC3_SHA, and RC4_MD5 ciphers are not supported in enterprise security.
  - SA query procedure not supported in 11W AP mode.
  - If background scan is enabled along with custom feature bit map bit (8) configuration of the DFS channel support, issues may occur in obtaining an IP address for the Wi-Fi STA.
  - In AP mode, IPv6 Router Advertisements are not supported. Although STA connected to the APUT can transfer IPv6 data to the peers by using SLAAC for address configuration.
  - Transparent mode is unsupported in USB-CDC mode.
  - Channel 144 is not supported.
  - IOP issues may be observed with device running DHCPv6 server with Android devices, as latter do not support DHCPv6 process for IPv6 address configuration.
  - WPA3 AP mode is supported with alpha-level quality.
  - WPA3 AP supports only H2E algorithm.
  - WPA3 AP PMKSA is not supported.
  - WPA3 AP transition mode is not supported.
  - AP standalone mode does not support Tx aggregation. Rx aggregation is supported with limited number of BA sessions.
  - In concurrent mode - AP mode, aggregation (Tx/Rx) is not supported.

### BT-Classic/BLE
  - BT A2DP is only supports for RS9116 Silicon revision 1.5.
  - BT sniff mode does not work if BT multiple slaves feature is enabled.
  - For BLE, if the connection is established with a small connection interval (<15 ms), simultaneous roles are not supported (that is Central/Peripheral + Advertising/Scanning).
  - For BT Classic, only one connection is supported at a time.
  - BLE slave latency value is valid up to 32 only.
  - BT-A2DP encoder is not supported in the firmware.
  - BT-A2DP is not supported in AT mode.
  - Maximum supported  AE data length is 200 bytes.
  - Supports only two ADV_EXT sets.
  - Maximum two BLE connections are supported (one central + one peripheral).
  - SMP is not supported in AE applications "ble_ae_central", and "ble_ae_peripheral".
  - GATT transactions, and Data transfer are not supported in AE applications "ble_ae_central", and "ble_ae_peripheral".
  - BLE duty cycling is not supported.
  - BLE does not support User_Config method.
  - In the rsibt_spptxv2 command, if the data length field value exceeds the actual data bytes, the firmware would continuously poll for the data, which results in no response or can cause it to get stuck.

### Coexistence
  - Wi-Fi STA + BT + BLE multiprotocol use cases requires a detailed understanding of use cases and associated configurations. Contact Silicon Labs support for additional details.
  - Wi-Fi AP + BLE, Wi-Fi AP + BT, and Wi-Fi AP + BT + BLE modes are not supported.
  - For AT commands, Wi-Fi + BT + BLE (Opermode 9), BT + BLE (Opermode 8) does not work.
  - In Wi-Fi + BLE mode, if the BLE scan interval and window have the same value, then the Wi-Fi connection might be unsuccessful.
  - Wi-Fi + AE Coexistence:
    - Maximum supported BLE scan interval and window for 100 % duty cycling  is three seconds.
    - Enable powersave to test the WiFi + AE coexistence use cases.
    - Remove the bond information on remote device for every connection if SMP is enabled.
    - Maximum two extended Advertising sets are supported.
    - Maximum two BLE connections are supported (one central + one peripheral).

### System
  - Wake on Wireless support has only been tested for the UART AT command interface.
  - Firmware update via bootloader method requires the user to check the integrity of the RPS file content. A corrupt image may cause the chip to go unresponsive to host commands.

## Notes
### BT-Classic/BLE
 - Acceptlist address should be given in reverse order, that is in big endian format.
 - AE feature validated using revision 1.5 AC1 expansion board.
 - AE feature is not supported in RS9116 revision 1.4.

# WiSeConnect 2.11.1 Release Notes

Last updated: December 11, 2024

## Highlights
Bug fix

## Release Details
| Item                                                | Details                                |
|-----------------------------------------------------|----------------------------------------|
| API Version (SAPI)                                  | 2.11.1.1 (Build 1)                     |
| Firmware Version                                    | 2.11.1.0.1 (Build 1)                   |
| Package Name                                        | RS9116W.2.11.1.1                       |
| Supported RTOS                                      | FreeRTOS                               |
| Hardware Modules                                    | QMS, B00, C00, CC1, AB0, AB1, AA0, AA1, AC0, AC1 |
| Hardware Chipsets                                   | Chip revision 1.3, Chip revision 1.4, Chip revision 1.5  |
| Operating Modes Supported                           | Wi-Fi STA, Wi-Fi AP, Wi-Fi STA + BLE, Wi-Fi STA + AP     |
| Additional Operating Modes Supported                | Wi-Fi STA + BT, Wi-Fi STA + BT + BLE , BLE (With AE)     |

### Notes
The latest releases have bug fixes, enhancements, and new features in both 'SAPIs', and 'Firmware'. Therefore, it is recommended to update, and use 'SAPIs', and 'Firmware' of same version.

## Updates in this Release
RS9116W 2.11.1.1 release consists of two components:
  - Firmware - RS9116 Firmware Binary
  - SAPI Library - SAPI Library runs on Host

This release is meant only for use with designs based on RS9116 Silicon revision 1.4 (RS9116X-xxx-xxx-Bxx), and RS9116 Silicon revision 1.5 (RS9116X-xxx-xxx-Cxx).
Customers using the Silicon revision 1.3 (RS9116X-xxx-xxx-Xxx) parts can upgrade to the latest firmware. However, the power optimization feature macro EXT_FEAT_LOW_POWER_MODE in rsi_wlan_config.h must be disabled for revision 1.3.


Select the appropriate firmware version for your specific Silicon revision as its a part of your design. Following Silicon revisions are available:
- RS9116W.2.11.1.0.1.rps
  - RS9116X-xxx-xxx-Xxx (Silicon revision 1.3)
  - RS9116X-xxx-xxx-Bxx (Silicon revision 1.4)
- RS916W.2.11.1.0.1.rps
  - RS9116X-xxx-xxx-Cxx (Silicon revision 1.5)
  - RS916WS00ACxxx

## New Features
Following new features are implemented with this version:

### Wi-Fi
None

### System
None

### BT-Classic/BLE
None

### Network Stack
None

### Coexistence
None

## Changes and Fixes
The list of fixes, and changes for this version are outlined further:

### Wi-Fi
None

### Network Stack
None

### BT-Classic/BLE
  Resolved the Bluetooth SPP(Serial Port Profile) data transfer issue, ensuring the data transmission from the DUT(Device Under Test) with a payload length of ≤ 127 bytes, while using the 2DH3 packet type.

### Coexistence
None

### System
None

### Documentation
None

## Recommendations and New Options
Following are the list of recommendations, and new options:

### Wi-Fi
  - Enable aggregation using the opermode parameter [feature_bit_map\[2\]](https://docs.silabs.com/rs9116-wiseconnect/2.6/wifibt-wc-sapi-reference/opermode#rsi-feature-bit-map).
  - Disable high MCS rates to improve the robustness of a connection for low throughput applications in congested networks. For instance, turning off `MCS6`, and `MCS7` may help a Smart Lock in a congested wireless environment.
  - To restart the RS9116, the application should call `rsi_driver_deinit()` followed by `rsi_driver_init()`, and `rsi_device_init()`. For OS cases, additionally call `rsi_task_destroy(driver_task_handle)` to delete the driver task before calling `rsi_driver_deinit()`, and create again after `rsi_device_init()` using `rsi_task_create()`.

### Network Stack
  - To ensure graceful handling during asynchronous TCP closures from a peer, enable the opermode parameter [ext_tcp_ip_feature_bit_map\[16\]](https://docs.silabs.com/rs9116-wiseconnect/2.6/wifibt-wc-sapi-reference/opermode#rsi-ext-tcpip-feature-bitmap).
  - To avoid TCP disconnects during a rejoin, set TCP retransmission count to >= `30`.
  - Implemented a new API - `rsi_network_app_protocol_config()` to configure the network application protocols such as cipher selection for HTTP protocol.

### BT-Classic/BLE
  - In BLE, the recommended range of the BLE Connection Interval is as follows:
    - Power Save (BLE only) - 100 ms to 1.28 s
    - BT Classic + BLE Dual Mode is >= 200 ms
    - Wi-Fi + BLE coex - 30 ms to 250 ms
  - During connection in BLE, it is not recommended to configure the scan window, and scan interval with the same value.
  - In BT Classic, the recommended Sniff Interval configuration during powersave is limited to 100 ms (\<= 100).
  - In BLE, if a device is acting as central, the scan window (in `set_scan_params()`, and `create_connection()` APIs) must be less than the existing connection interval.
  - In BLE mode, if scanning, and advertising is in progress, and the device subsequently connects, and moves to the central role, scanning, and advertising stops. To further establish connection to another peripheral device or to a central device, the application must initiate advertising, and scanning again.
  - Device powersave must be disabled prior to BT init, and de-init.

### Coexistence
  - For concurrent Wi-Fi + BLE, and while a Wi-Fi connection is active, we recommend setting the ratio of the BLE scan window to BLE scan interval to 1:3 or 1:4.
  - Wi-Fi + BLE Advertising
    - All standard advertising intervals are supported. As Wi-Fi throughput is increased, a slight difference in on-air advertisements compared to configured intervals might be observed.
    - BLE advertising is skipped if the advertising interval collides with Wi-Fi activity.
  - Wi-Fi + BLE scanning
    - All standard scan intervals are supported. For better scan results, we recommend setting the ratio of the BLE scan window to BLE scan interval to 1:3 or 1:4.
    - BLE scan would be stopped for intervals that collide with Wi-Fi activity.
  - Wi-Fi + BLE Central/Peripheral Connections
    - All standard connection intervals are supported.
    - For a stable connection, use optimal connection intervals, and maximum supervision timeout in the presence of Wi-Fi activity.
  - Wi-Fi + BLE Central/Peripheral Data Transfer
    - To achieve higher throughput for both Wi-Fi, and BLE, use medium connection intervals, such as 45 to 80 ms with maximum supervision timeout.
    - Ensure Wi-Fi activity consumes lower intervals.
  - To ensure a stable, and seamless connection, connect Wi-Fi before BT/BLE connections in the following operating modes: Wi-Fi + BT + BLE, Wi-Fi + BT, and Wi-Fi + BLE.
  - For Wi-Fi + BT + BLE, Wi-Fi + BT, and Wi-Fi + BLE operating modes, if BT/BLE must be connected before Wi-Fi, use with a high supervision timeout, a high connection interval for BLE, and a high sniff interval for BT to ensure seamless and stable connection. This configuration additionally ensures that the BT/BLE connection stays stable when Wi-Fi connects/disconnects/rejoins.

### System
  - For User Store configuration and Configuration Save, do not enable power save or save it as a configuration. If power save is enabled and saved as a configuration, upon boot up, the RS9116 will boot with the saved configuration and will go to powersave without any indication to the host.
  - Set the compiler optimization level to `O0` in project settings for IDE (KEIL).
  - Memory configuration must be 384K for BT/BLE and co-ex operating modes.
    - Usage of low-power flash mode bit (bit 19 in extended customer feature bitmap). Enable this bit for ultra-low-power standby associated scenarios. This results in about 20&micro;A lower Wi-Fi standby associated current consumption.
  - Memory Configuration in SAPI functions: The default memory configuration is 384K.
  - Apply Opermode commands in AT mode correctly. Using the wrong opermode may lead to unspecified behavior.
  - Set the recommended Power Save Profile (PSP) type to Enhanced Max PSP.
  - It is recommended to disable the power save functionality while updating firmware.
  - For high throughput applications, powersave operation should be disabled.
  - The application is required to set the real-time clock with the correct timestamp when the feature is enabled before establishing a TLS connection.
  - At least one second timeout is a necessity for socket select and receive. No timeout value under one second is supported.

## Known Issues
Following are the list of known issues:

### Network Stack
  - Reassembly of fragmented MQTT packets is not supported in embedded MQTT. MQTT transactions may fail with fragmented packets received from the broker.
  - Occasionally, during a TLS handshake, ECC curve parameters are generated incorrectly, which results in connection failure with BBD2 error. However, this gets recover in next attempt.
  - During firmware updates, MQTT disconnects when power savings is enabled. During the firmware update, disable power save and re-enable it on updated completion.
  - All GCM-based cipher suites are implemented in software and have limited performance.
  - The recommended MQTT publish payload is 1 kBytes.
  - If HTTP server functionality is enabled, do not use port 80 for the MQTT client.
  - Randomize the client port if using rapid connect/disconnect of the MQTT session on the same client port with powersave.
  - Secure TLS renegotiation is not supported in the embedded networking stack.
  - In WPA2 enterprise security mode, getting wireless statistics using rsi_wlan_get() API has restrictions.
  - Example application wlan_throughtput_bt_spp_ble_dual_role is unsupported with bare metal.
  - In concurrent mode, there could be a loss of communication on the STA application when, simultaneously, multiple clients are connected to the APUT.
  - IP address conflict detection has limitations in STA + AP concurrent mode, failure is not reported to host.
  - mqtt_client, and mqtt_client_v6 applications does not support SDIO host interface.
  - During firmware update via OTA (TCP/HTTP) methods, if the firmware image received is corrupted, it can result in the module going unresponsive on the next bootup.
  - TLS connection problems may be observed when attempting to revoke public and private keys using existing keys.
  - Observed STA + BT (Connected) test cases not getting throughput results prints in teraterm.
  - Observed inconsistency in TCP and UDP - Rx throughputs in AP alone mode.
  - Observed duplicate IPv6 address is assigned to multiple STAs in AP mode.
  - Ping timeout may be observed with 1 second ping timeout configuration.

### Wi-Fi
  - WPA3 Connection with Hunting and Pecking algorithm takes about three to four seconds.
  - If the station performs a scan in concurrent mode (Wi-Fi STA + AP), stations connected to the AP may be disconnected. Enable AP after the STA connection is completed.
  - With power save, PTA 3-wire co-existence does not work. However, if a weak pull-up is applied on the GRANT pin, the PTA 3-wire functionality will work while the power is saved. For further information, please refer to Application Notes.
  - EAP security in the aws_device_shadow application is unsupported on the EFR platform due to memory constraints.
  - In STA + AP, If AP started first station scan is supported only in channel in which AP is brought up. If multiple channel scan is requested, AP behaviour is undefined.
  - Auto channel selection with EXT_FEAT_IEEE_80211J feature enabled is not supported in Access point operating mode.
  - Testing for traffic differentiation within a single BSS with 802.11n stations is yielding failures during the 11n certification process.

### BT-Classic/BLE
  - Wake on wireless is not supported for BLE.
  - BT-HID may not interoperate with Apple devices.
  - For BLE, dual role scenario issues may occur when connecting as a central when the peripheral is advertising. Central connections can be made after peripheral connections are established and advertising stops.
  - A BLE disconnection would occur if both the peer and RS9116 already have bond info. And the RS9116 initiates security as part of BLE secure connections. To work around this issue, the application should not initiate a security request if, it already has bond info.
  - A2DP music streaming glitches may be observed if a Wi-Fi download is in progress at the same time.
  - RTOS support is not there in ae_peripheral, and ae_central applications.
  - AE connection issue is expected with Samsung mobiles.
  - BT inquiry would not work in temperature range from -40 to 0 C.
  - When DUT is configured as a peripheral with powersave, repeated string of passkey is observed in multiple iterations of reconnection.

### Coexistence
  - After power save is enabled in co-ex mode, `radio_init()` must be called to turn off power save.
  - In Wi-Fi + BLE mode:
    - BLE may disconnect when the supervision timeout is configured to be less than 16 seconds.
    - WPS does not work.
  - In Wi-Fi + BLE, or BLE only modes:
    - When Wi-Fi disconnects, a BT/BLE reconnection issue is observed.
  - In Wi-Fi + BT mode:
    - When BT SPP is connected, Wi-Fi throughput prints are not displayed in Teraterm with wlan_bt_throughput_app demo.
  - The wlan_https_bt_a2dp_ble_dual_role and wlan_https_bt_a2dp demos are not supported.
  - In Wi-Fi + BT/BLE Co-Ex mode, high Wi-Fi broadcast traffic might cause BT/BLE disconnections.
  - In Wi-Fi + BT mode when there is continuous Wi-Fi data, you might observe BT not re-connecting to the remote after disconnection.

### Interoperability
Disconnection may occur in Wi-Fi client mode if an AP does not acknowledge QoS null frames with power save configured.

### System
  - Chip Revision 1.3 doesn't support low-power mode optimization using the bit setting defined by EXT_FEAT_LOW_POWER_MODE; setting this results in a hang issue.
  - Sometimes the RS9116 enters power save mode before the configured monitor interval for inactivity when coex is configured.
  - The FTP client application does not work with EFx32/STM32 when an RTOS is used.
  - The HTTP OTA update application does not work with EFR32 and EFM32 when an RTOS is used.
  - Firmware update does not work for the EFM32 platform.
  - SAPI driver with UART host interface is supported only on bare metal.
  - Powersave without RAM retention does not work when the SPI interface is used.
  - MQTT Receive may encounter failures in STA + AP concurrent mode.
  - Standby associated listen interval current consumption is higher than expected with EFM Platform.

## Limitations and Unsupported Features
Following are the list of limitations and unsupported features:

### Wi-Fi/Network Stack
  - AMSDU transmit is not supported.
  - UAPSD is not supported.
  - Fragmentation is not supported.
  - AMSDU's within AMPDU is not supported.
  - Currently, the RS9116 does not support the radio measurement requests feature of CCX V2.
  - 802.11k is not supported.
  - Short GI is not supported.
  - 2.4 and 5 GHz bands does not support 40 MHz bandwidth.
  - 802.11j channels less than 16 are not supported.
  - The USB host interface and USB power save are not supported.
  - In AT mode, the total MQTT command length (apart from MQTT publish) should not exceed `150` bytes. This includes `at+rsi` (start of command) to (end of command)`\r\n`.
  - Configure `MQTT_VERSION` in `rsi_mqtt_client.h` based on the server configuration; only version 3 and 4 are supported.
  - 3xTLS connections or 1xTCP and 2xTLS connections are supported concurrently in Wi-Fi only mode.
  - TLS curve IDs supports from 15-28. TLS handshake with third party clients depends on the TLS curve.
  - Big endian architecture is not supported.
  - Auto PAC Provisioning in EAP-FAST with TLSv1.2 is not supported.
  - RC4_SHA, DES_CBC3_SHA, and RC4_MD5 ciphers are not supported in enterprise security.
  - SA query procedure not supported in 11W AP mode.
  - If background scan is enabled along with custom feature bit map bit (8) configuration of the DFS channel support, issues may occur in obtaining an IP address for the Wi-Fi STA.
  - In AP mode, IPv6 Router Advertisements are not supported. Although STA connected to the APUT can transfer IPv6 data to the peers by using SLAAC for address configuration.
  - Transparent mode is unsupported in USB-CDC mode.
  - Channel 144 is not supported.
  - IOP issues may be observed with device running DHCPv6 server with Android devices, as latter do not support DHCPv6 process for IPv6 address configuration.
  - WPA3 AP mode is supported with alpha-level quality.
  - WPA3 AP supports only H2E algorithm.
  - WPA3 AP PMKSA is not supported.
  - WPA3 AP transition mode is not supported.
  - AP standalone mode does not support Tx aggregation. Rx aggregation is supported with limited number of BA sessions.
  - In concurrent mode - AP mode, aggregation (Tx/Rx) is not supported.

### BT-Classic/BLE
  - BT A2DP is only supports for RS9116 Silicon revision 1.5.
  - BT sniff mode does not work if BT multiple slaves feature is enabled.
  - For BLE, if the connection is established with a small connection interval (<15 ms), simultaneous roles are not supported (that is Central/Peripheral + Advertising/Scanning).
  - For BT Classic, only one connection is supported at a time.
  - BLE slave latency value is valid up to 32 only.
  - BT-A2DP encoder is not supported in the firmware.
  - BT-A2DP is not supported in AT mode.
  - Maximum supported  AE data length is 200 bytes.
  - Supports only two ADV_EXT sets.
  - Maximum two BLE connections are supported (one central + one peripheral).
  - SMP is not supported in AE applications "ble_ae_central", and "ble_ae_peripheral".
  - GATT transactions, and Data transfer are not supported in AE applications "ble_ae_central", and "ble_ae_peripheral".
  - BLE duty cycling is not supported.
  - BLE does not support User_Config method.
  - In the rsibt_spptxv2 command, if the data length field value exceeds the actual data bytes, the firmware would continuously poll for the data, which results in no response or can cause it to get stuck.

### Coexistence
  - Wi-Fi STA + BT + BLE multiprotocol use cases requires a detailed understanding of use cases and associated configurations. Contact Silicon Labs support for additional details.
  - Wi-Fi AP + BLE, Wi-Fi AP + BT, and Wi-Fi AP + BT + BLE modes are not supported.
  - For AT commands, Wi-Fi + BT + BLE (Opermode 9), BT + BLE (Opermode 8) does not work.
  - In Wi-Fi + BLE mode, if the BLE scan interval and window have the same value, then the Wi-Fi connection might be unsuccessful.
  - Wi-Fi + AE Coexistence:
    - Maximum supported BLE scan interval and window for 100 % duty cycling  is three seconds.
    - Enable power save to test the WiFi + AE coexistence use cases.
    - Remove the bond information on remote device for every connection if SMP is enabled.
    - Maximum two extended Advertising sets are supported.
    - Maximum two BLE connections are supported (one central + one peripheral).

### System
  - Wake on Wireless support has only been tested for the UART AT command interface.
  - Firmware update via bootloader method requires the user to check the integrity of the RPS file content. A corrupted image may cause the chip to go unresponsive to host commands.

## Notes
### BT-Classic/BLE
 - Acceptlist address should be given in reverse order, that is in big endian format.
 - AE feature validated using revision 1.5 AC1 expansion board.
 - AE feature is not supported in RS9116 revision 1.4.

# WiSeConnect 2.11 Release Notes

Last updated: November 29, 2024

## Highlights

  - New features
  - Bug fix

## Release Details
| Item                                                | Details                                |
|-----------------------------------------------------|----------------------------------------|
| API Version (SAPI)                                  | 2.11.0.6 (Build 6)                     |
| Firmware Version                                    | 2.11.0.0.6 (Build 6)                   |
| Package Name                                        | RS9116W.2.11.0.6                       |
| Supported RTOS                                      | FreeRTOS                               |
| Hardware Modules                                    | QMS, B00, C00, CC1, AB0, AB1, AA0, AA1, AC0, AC1 |
| Hardware Chipsets                                   | Chip revision 1.3, Chip revision 1.4, Chip revision 1.5   |
| Operating Modes Supported                           | Wi-Fi STA, Wi-Fi AP, Wi-Fi STA + BLE, Wi-Fi STA + AP     |
| Additional Operating Modes Supported (Beta Quality) | Wi-Fi STA + BT, Wi-Fi STA + BT + BLE , BLE(With AE)     |

### Notes
The latest releases have bug fixes, enhancements, and new features in both 'SAPIs', and 'Firmware'. Therefore, it is recommended to update, and use 'SAPIs', and 'Firmware' of same version.

## Updates in this Release
RS9116W 2.11.0.6 release consists of two components:
  - Firmware - RS9116 Firmware Binary
  - SAPI Library - SAPI Library runs on Host

This release is meant only for use with designs based on RS9116 Silicon revision 1.4 (RS9116X-xxx-xxx-Bxx), and RS9116 Silicon revision 1.5 (RS9116X-xxx-xxx-Cxx).
Customers using the Silicon revision 1.3 (RS9116X-xxx-xxx-Xxx) parts can upgrade to the latest firmware. However, the power optimization feature macro EXT_FEAT_LOW_POWER_MODE in rsi_wlan_config.h must be disabled for revision 1.3.


Select the appropriate firmware version for your specific Silicon revision as its a part of your design. Following Silicon revisions are available:
- RS9116W.2.11.0.0.6.rps
  - RS9116X-xxx-xxx-Xxx (Silicon revision 1.3)
  - RS9116X-xxx-xxx-Bxx (Silicon revision 1.4)
- RS916W.2.11.0.0.6.rps
  - RS9116X-xxx-xxx-Cxx (Silicon revision 1.5)
  - RS916WS00ACxxx

## New Features
Following new features are implemented with this version:

### Wi-Fi
Enhanced the keep-alive logic to reset the timer upon unicast data transmission between the module (STA) and the Access Point (AP).

### System
None

### BT-Classic/BLE
None

### Network Stack
Added feature support for Server Name Indication (SNI), and ciphers selection in AT mode.

### Coexistence
Added support for Wi-Fi coexistence with Advertising Extensions.

## Changes and Fixes
The list of fixes, and changes for this version are outlined further:

### Wi-Fi
Fixed a "Memory Allocation Failed" issue during the Get Configuration process.

### Network Stack
  - Fixed an issue where calling the rsi_calib_read API after the wireless_init API resulted in error code 0x21.
  - Fixed an issue with the select system call to ensure proper functionality in both synchronous, and asynchronous modes.
  - Fixed an issue in the "Store Server IPv6" AT command, which now throws an error for invalid IPv6 addresses.

### BT-Classic/BLE
  - The continuous BTC SPP connection and disconnection issue between modules has been fixed.
  - Fixed BTC L2CAP Echo issues.
  - Fixed BTC disconnection issue occurred due to the dynamic power control in the EDR mode.

### Coexistence
None

### System
Removed TODO, and FIXME strings from the release package source code owned by Silicon Labs.

### Documentation
  - Updated the documentation on rejoin parameter in SAPI reference.
  - Corrected Transmit Power level in Join command description.
  - Added documentation for at+rsi_httpota6 command in Wi-Fi AT PRM.
  - Updated the structure information in SAPI PRM under the rsi_wlan_get() api.
  - Added documentation for at+rsi_dnsget6, and at+rsi_dnsserver6 commands in Wi-Fi AT PRM.
  - Updated the WPA3 documentation, and WPA3 Transition security modes in the scan_info structure of SAPI PRM.
  - Added a note in Wi-Fi AT PRM for switching from Binary mode to AT command mode.

## Recommendations and New Options
Following are the list of recommendations, and new options:

### Wi-Fi
  - Enable aggregation using the opermode parameter [feature_bit_map\[2\]](https://docs.silabs.com/rs9116-wiseconnect/2.6/wifibt-wc-sapi-reference/opermode#rsi-feature-bit-map).
  - Disable high MCS rates to improve the robustness of a connection for low throughput applications in congested networks. For instance, turning off `MCS6`, and `MCS7` may help a Smart Lock in a congested wireless environment.
  - To restart the RS9116, the application should call `rsi_driver_deinit()` followed by `rsi_driver_init()`, and `rsi_device_init()`. For OS cases, additionally call `rsi_task_destroy(driver_task_handle)` to delete the driver task before calling `rsi_driver_deinit()`, and create again after `rsi_device_init()` using `rsi_task_create()`.

### Network Stack
  - To ensure graceful handling during asynchronous TCP closures from a peer, enable the opermode parameter [ext_tcp_ip_feature_bit_map\[16\]](https://docs.silabs.com/rs9116-wiseconnect/2.6/wifibt-wc-sapi-reference/opermode#rsi-ext-tcpip-feature-bitmap).
  - To avoid TCP disconnects during a rejoin, set TCP retransmission count to >= `30`.
  - Implemented a new API - `rsi_network_app_protocol_config()` to configure the network application protocols such as cipher selection for HTTP protocol.

### BT-Classic/BLE
  - In BLE, the recommended range of the BLE Connection Interval is as follows:
    - Power Save (BLE only) - 100 ms to 1.28 s
    - BT Classic + BLE Dual Mode is >= 200 ms
    - Wi-Fi + BLE coex - 30 ms to 250 ms
  - During connection in BLE, it is not recommended to configure the scan window, and scan interval with the same value.
  - In BT Classic, the recommended Sniff Interval configuration during powersave is limited to 100 ms (\<= 100).
  - In BLE, if a device is acting as central, the scan window (in `set_scan_params()`, and `create_connection()` APIs) must be less than the existing connection interval.
  - In BLE mode, if scanning, and advertising is in progress, and the device subsequently connects, and moves to the central role, scanning, and advertising stops. To further establish connection to another peripheral device or to a central device, the application must initiate advertising, and scanning again.
  - Device powersave must be disabled prior to BT init, and de-init.

### Coexistence
  - For concurrent Wi-Fi + BLE, and while a Wi-Fi connection is active, we recommend setting the ratio of the BLE scan window to BLE scan interval to 1:3 or 1:4.
  - Wi-Fi + BLE Advertising
    - All standard advertising intervals are supported. As Wi-Fi throughput is increased, a slight difference in on-air advertisements compared to configured intervals might be observed.
    - BLE advertising is skipped if the advertising interval collides with Wi-Fi activity.
  - Wi-Fi + BLE scanning
    - All standard scan intervals are supported. For better scan results, we recommend setting the ratio of the BLE scan window to BLE scan interval to 1:3 or 1:4.
    - BLE scan would be stopped for intervals that collide with Wi-Fi activity.
  - Wi-Fi + BLE Central/Peripheral Connections
    - All standard connection intervals are supported.
    - For a stable connection, use optimal connection intervals, and maximum supervision timeout in the presence of Wi-Fi activity.
  - Wi-Fi + BLE Central/Peripheral Data Transfer
    - To achieve higher throughput for both Wi-Fi, and BLE, use medium connection intervals, such as 45 to 80 ms with maximum supervision timeout.
    - Ensure Wi-Fi activity consumes lower intervals.
  - To ensure a stable, and seamless connection, connect Wi-Fi before BT/BLE connections in the following operating modes: Wi-Fi + BT + BLE, Wi-Fi + BT, and Wi-Fi + BLE.
  - For Wi-Fi + BT + BLE, Wi-Fi + BT, and Wi-Fi + BLE operating modes, if BT/BLE must be connected before Wi-Fi, use with a high supervision timeout, a high connection interval for BLE, and a high sniff interval for BT to ensure seamless and stable connection. This configuration additionally ensures that the BT/BLE connection stays stable when Wi-Fi connects/disconnects/rejoins.

### System
  - For User Store configuration and Configuration Save, do not enable power save or save it as a configuration. If power save is enabled and saved as a configuration, upon boot up, the RS9116 will boot with the saved configuration and will go to powersave without any indication to the host.
  - Set the compiler optimization level to `O0` in project settings for IDE (KEIL).
  - Memory configuration must be 384K for BT/BLE and co-ex operating modes.
    - Usage of low-power flash mode bit (bit 19 in extended customer feature bitmap). Enable this bit for ultra-low-power standby associated scenarios. This results in about 20&micro;A lower Wi-Fi standby associated current consumption.
  - Memory Configuration in SAPI functions: The default memory configuration is 384K.
  - Apply Opermode commands in AT mode correctly. Using the wrong opermode may lead to unspecified behavior.
  - Set the recommended Power Save Profile (PSP) type to Enhanced Max PSP.
  - It is recommended to disable the power save functionality while updating firmware.
  - For high throughput applications, powersave operation should be disabled.
  - The application is required to set the real-time clock with the correct timestamp when the feature is enabled before establishing a TLS connection.
  - At least one second timeout is a necessity for socket select and receive. No timeout value under one second is supported.

## Known Issues
Following are the list of known issues:

### Network Stack
  - Reassembly of fragmented MQTT packets is not supported in embedded MQTT. MQTT transactions may fail with fragmented packets received from the broker.
  - Occasionally, during a TLS handshake, ECC curve parameters are generated incorrectly, which results in connection failure with BBD2 error. However, this gets recover in next attempt.
  - During firmware updates, MQTT disconnects when power savings is enabled. During the firmware update, disable power save and re-enable it on updated completion.
  - All GCM-based cipher suites are implemented in software and have limited performance.
  - The recommended MQTT publish payload is 1 kBytes.
  - If HTTP server functionality is enabled, do not use port 80 for the MQTT client.
  - Randomize the client port if using rapid connect/disconnect of the MQTT session on the same client port with powersave.
  - Secure TLS renegotiation is not supported in the embedded networking stack.
  - In WPA2 enterprise security mode, getting wireless statistics using rsi_wlan_get() API has restrictions.
  - Example application wlan_throughtput_bt_spp_ble_dual_role is unsupported with bare metal.
  - In concurrent mode, there could be a loss of communication on the STA application when, simultaneously, multiple clients are connected to the APUT.
  - IP address conflict detection has limitations in STA + AP concurrent mode, failure is not reported to host.
  - mqtt_client, and mqtt_client_v6 applications does not support SDIO host interface.
  - During firmware update via OTA (TCP/HTTP) methods, if the firmware image received is corrupted, it can result in the module going unresponsive on the next bootup.
  - TLS connection problems may be observed when attempting to revoke public and private keys using existing keys.
  - Observed STA + BT (Connected) test cases not getting throughput results prints in teraterm.
  - Observed inconsistency in TCP and UDP - Rx throughputs in AP alone mode.
  - Observed duplicate IPv6 address is assigned to multiple STAs in AP mode.
  - Ping timeout may be observed with 1 second ping timeout configuration.

### Wi-Fi
  - WPA3 Connection with Hunting and Pecking algorithm takes about three to four seconds.
  - If the station performs a scan in concurrent mode (Wi-Fi STA + AP), stations connected to the AP may be disconnected. Enable AP after the STA connection is completed.
  - Issue observed with WPA2 Enterprise Connectivity using Microsoft RADIUS Server.
  - With power save, PTA 3-wire co-existence does not work. However, if a weak pull-up is applied on the GRANT pin, the PTA 3-wire functionality will work while the power is saved. For further information, please refer to Application Notes.
  - EAP security in the aws_device_shadow application is unsupported on the EFR platform due to memory constraints.
  - In STA + AP, If AP started first station scan is supported only in channel in which AP is brought up. If multiple channel scan is requested, AP behaviour is undefined.
  - Auto channel selection with EXT_FEAT_IEEE_80211J feature enabled is not supported in Access point operating mode.
  - Testing for traffic differentiation within a single BSS with 802.11n stations is yielding failures during the 11n certification process.

### BT-Classic/BLE
  - Wake on wireless is not supported for BLE.
  - BT-HID may not interoperate with Apple devices.
  - For BLE, dual role scenario issues may occur when connecting as a central when the peripheral is advertising. Central connections can be made after peripheral connections are established and advertising stops.
  - A BLE disconnection would occur if both the peer and RS9116 already have bond info. And the RS9116 initiates security as part of BLE secure connections. To work around this issue, the application should not initiate a security request if, it already has bond info.
  - A2DP music streaming glitches may be observed if a Wi-Fi download is in progress at the same time.
  - RTOS support is not there in ae_peripheral, and ae_central applications.
  - AE connection issue is expected with Samsung mobiles.
  - BT inquiry would not work in temperature range from -40 to 0 C.
  - When DUT is configured as a peripheral with powersave, repeated string of passkey is observed in multiple iterations of reconnection.

### Coexistence
  - After power save is enabled in co-ex mode, `radio_init()` must be called to turn off power save.
  - In Wi-Fi + BLE mode:
    - BLE may disconnect when the supervision timeout is configured to be less than 16 seconds.
    - WPS does not work.
  - In Wi-Fi + BLE, or BLE only modes:
    - When Wi-Fi disconnects, a BT/BLE reconnection issue is observed.
  - In Wi-Fi + BT mode:
    - When BT SPP is connected, Wi-Fi throughput prints are not displayed in Teraterm with wlan_bt_throughput_app demo.
  - The wlan_https_bt_a2dp_ble_dual_role and wlan_https_bt_a2dp demos are not supported.
  - In Wi-Fi + BT/BLE Co-Ex mode, high Wi-Fi broadcast traffic might cause BT/BLE disconnections.
  - In Wi-Fi + BT mode when there is continuous Wi-Fi data, you might observe BT not re-connecting to the remote after disconnection.

### Interoperability
Disconnection may occur in Wi-Fi client mode if an AP does not acknowledge QoS null frames with power save configured.

### System
  - Chip Revision 1.3 doesn't support low-power mode optimization using the bit setting defined by EXT_FEAT_LOW_POWER_MODE; setting this results in a hang issue.
  - Sometimes the RS9116 enters power save mode before the configured monitor interval for inactivity when co-ex is configured.
  - The FTP client application does not work with EFx32/STM32 when an RTOS is used.
  - The HTTP OTA update application does not work with EFR32 and EFM32 when an RTOS is used.
  - Firmware update does not work for the EFM32 platform.
  - SAPI driver with UART host interface is supported only on bare metal.
  - Powersave without RAM retention does not work when the SPI interface is used.
  - MQTT Receive may encounter failures in STA + AP concurrent mode.
  - Standby associated listen interval current consumption is higher than expected with EFM Platform.

## Limitations and Unsupported Features
Following are the list of limitations and unsupported features:

### Wi-Fi/Network Stack
  - AMSDU transmit is not supported.
  - UAPSD is not supported.
  - Fragmentation is not supported.
  - AMSDU's within AMPDU is not supported.
  - Currently, the RS9116 does not support the radio measurement requests feature of CCX V2.
  - 802.11k is not supported.
  - Short GI is not supported.
  - 2.4 and 5 GHz bands does not support 40 MHz bandwidth.
  - 802.11j channels less than 16 are not supported.
  - The USB host interface and USB power save are not supported.
  - In AT mode, the total MQTT command length (apart from MQTT publish) should not exceed `150` bytes. This includes `at+rsi` (start of command) to (end of command)`\r\n`.
  - Configure `MQTT_VERSION` in `rsi_mqtt_client.h` based on the server configuration; only version 3 and 4 are supported.
  - 3xTLS connections or 1xTCP and 2xTLS connections are supported concurrently in Wi-Fi only mode.
  - TLS curve IDs supports from 15-28. TLS handshake with third party clients depends on the TLS curve.
  - Big endian architecture is not supported.
  - Auto PAC Provisioning in EAP-FAST with TLSv1.2 is not supported.
  - RC4_SHA, DES_CBC3_SHA, and RC4_MD5 ciphers are not supported in enterprise security.
  - SA query procedure not supported in 11W AP mode.
  - If background scan is enabled along with custom feature bit map bit (8) configuration of the DFS channel support, issues may occur in obtaining an IP address for the Wi-Fi STA.
  - In AP mode, IPv6 Router Advertisements are not supported. Although STA connected to the APUT can transfer IPv6 data to the peers by using SLAAC for address configuration.
  - Transparent mode is unsupported in USB-CDC mode.
  - Channel 144 is not supported.
  - IOP issues may be observed with device running DHCPv6 server with Android devices, as latter do not support DHCPv6 process for IPv6 address configuration.
  - WPA3 AP mode is supported with alpha-level quality.
  - WPA3 AP supports only H2E algorithm.
  - WPA3 AP PMKSA is not supported.
  - WPA3 AP transition mode is not supported.
  - AP standalone mode does not support Tx aggregation. Rx aggregation is supported with limited number of BA sessions.
  - In concurrent mode - AP mode, aggregation (Tx/Rx) is not supported.

### BT-Classic/BLE
  - BT A2DP is only supports for RS9116 Silicon revision 1.5.
  - BT sniff mode does not work if BT multiple slaves feature is enabled.
  - For BLE, if the connection is established with a small connection interval (<15 ms), simultaneous roles are not supported (that is Central/Peripheral + Advertising/Scanning).
  - For BT Classic, only one connection is supported at a time.
  - BLE slave latency value is valid up to 32 only.
  - BT-A2DP encoder is not supported in the firmware.
  - BT-A2DP is not supported in AT mode.
  - Maximum supported  AE data length is 200 bytes.
  - Supports only two ADV_EXT sets.
  - Maximum two BLE connections are supported (one central + one peripheral).
  - SMP is not supported in AE applications "ble_ae_central", and "ble_ae_peripheral".
  - GATT transactions, and Data transfer are not supported in AE applications "ble_ae_central", and "ble_ae_peripheral".
  - BLE duty cycling is not supported.
  - BLE does not support User_Config method.
  - In the rsibt_spptxv2 command, if the data length field value exceeds the actual data bytes, the firmware would continuously poll for the data, which results in no response or can cause it to get stuck.

### Coexistence
  - Wi-Fi STA + BT + BLE multiprotocol use cases requires a detailed understanding of use cases and associated configurations. Contact Silicon Labs support for additional details.
  - Wi-Fi AP + BLE, Wi-Fi AP + BT, and Wi-Fi AP + BT + BLE modes are not supported.
  - For AT commands, Wi-Fi + BT + BLE (Opermode 9), BT + BLE (Opermode 8) does not work.
  - In Wi-Fi + BLE mode, if the BLE scan interval and window have the same value, then the Wi-Fi connection might be unsuccessful.
  - Wi-Fi + AE Coexistence:
    - Maximum supported BLE scan interval and window for 100 % duty cycling  is three seconds.
    - Enable power save to test the WiFi + AE coexistence use cases.
    - Remove the bond information on remote device for every connection if SMP is enabled.
    - Maximum two extended Advertising sets are supported.
    - Maximum two BLE connections are supported (one central + one peripheral).

### System
  - Wake on Wireless support has only been tested for the UART AT command interface.
  - Firmware update via bootloader method requires the user to check the integrity of the RPS file content. A corrupted image may cause the chip to go unresponsive to host commands.

## Notes
### BT-Classic/BLE
 - Acceptlist address should be given in reverse order, that is in big endian format.
 - AE feature validated using revision 1.5 AC1 expansion board.
 - AE feature is not supported in RS9116 revision 1.4.

# WiSeConnect 2.10.4 Release Notes

Last updated: October 14, 2024

## Highlights

  - Bug fix

## Release Details
| Item                                                | Details                                |
|-----------------------------------------------------|----------------------------------------|
| API Version (SAPI)                                  | 2.10.4.1 (Build 1)                     |
| Firmware Version                                    | 2.10.4.0.1 (Build 1)                   |
| Package Name                                        | RS9116W.2.10.4.1                       |
| Supported RTOS                                      | FreeRTOS                               |
| Hardware Modules                                    | QMS, B00, C00, CC1, AB0, AB1, AA0, AA1, AC0, AC1 |
| Hardware Chipsets                                   | Chip revision 1.3, Chip revision 1.4, Chip revision 1.5   |
| Operating Modes Supported                           | Wi-Fi STA, Wi-Fi AP, Wi-Fi STA+BLE, Wi-Fi STA+AP     |
| Additional Operating Modes Supported (Beta Quality) | Wi-Fi STA+BT, Wi-Fi STA+BT+BLE , BLE(With AE)     |

### Notes
  - The latest release have bug fixes, enhancements, and new features in both 'SAPIs', and 'Firmware'. Therefore, it is recommended to update, and use 'SAPIs', and 'Firmware' of same version.

## Updates in this Release
RS9116W 2.10.4.1 release consists of two components, which are as following:
  - Firmware - RS9116 Firmware Binary
  - SAPI Library - SAPI Library runs on Host

This release is meant only for use with designs based on RS9116 Silicon revision1.4 (RS9116X-xxx-xxx-Bxx) and RS9116 Silicon revision1.5 (RS9116X-xxx-xxx-Cxx).
Customers using the RS9116X-xxx-xxx-Xxx parts (Silicon rev 1.3) can upgrade to the latest firmware. However, the power optimization feature macro EXT_FEAT_LOW_POWER_MODE in rsi_wlan_config.h must be disabled for revision 1.3.


Choose the firmware to match the Silicon revision, which is a part of your design. Following Silicon revisions are available:
- RS9116W.2.10.4.0.1.rps
  - RS9116X-xxx-xxx-Xxx (Silicon revision1.3)
  - RS9116X-xxx-xxx-Bxx (Silicon revision1.4)
- RS916W.2.10.4.0.1.rps
  - RS9116X-xxx-xxx-Cxx (Silicon revision1.5)
  - RS916WS00ACxxx

## New Features
Following are the list of new features implemented:

### Wi-Fi
None

### System
None

### BT-Classic/BLE
  - Added a new command rsibt_spptxv2 for SPP transmission that does not require byte stuffing of the data.

### Network Stack
None

## Changes and Fixes
The list of fixes and changes for this version are outlined further:

### Wi-Fi
None

### Network Stack
None

### BT-Classic/BLE
None

### Coexistence
None

### System
None

### Documentation
None

## Recommendations and New Options
Following are the list of recommendations and new options:

### Wi-Fi
  - Enable aggregation using the opermode parameter [feature_bit_map\[2\]](https://docs.silabs.com/rs9116-wiseconnect/2.6/wifibt-wc-sapi-reference/opermode#rsi-feature-bit-map).
  - Disable high MCS rates to improve the robustness of a connection for low throughput applications in congested networks. For instance, turning off `MCS6` and `MCS7` may help a Smart Lock in a congested wireless environment.
  - To restart the RS9116, the application should call `rsi_driver_deinit()` followed by `rsi_driver_init()` and `rsi_device_init()`. For OS cases, additionally call `rsi_task_destroy(driver_task_handle)` to delete the driver task before calling `rsi_driver_deinit()` and create again after `rsi_device_init()` using `rsi_task_create()`.

### Network Stack
  - To ensure graceful handling during asynchronous TCP closures from a peer, enable the opermode parameter [ext_tcp_ip_feature_bit_map\[16\]](https://docs.silabs.com/rs9116-wiseconnect/2.6/wifibt-wc-sapi-reference/opermode#rsi-ext-tcpip-feature-bitmap).
  - To avoid TCP disconnects during a rejoin, set TCP retransmission count to >= `30`.
  - Implemented a new API - `rsi_network_app_protocol_config()` to configure the network application protocols such as cipher selection for HTTP protocol.

### BT-Classic/BLE
  - In BLE, the recommended range of the BLEConnection Interval is as follows:
    - Power Save (BLE only) - 100 ms to 1.28 s
    - BT Classic + BLE Dual Mode is >= 200 ms
    - Wi-Fi + BLE coex - 30 ms to 250 ms
  - In BLE during connection, it is not recommended to configure the scan window, and scan interval with the same value.
  - In BT Classic, the recommended Sniff Interval configuration during powersave is limited to 100 ms (\<= 100).
  - In BLE, if a device is acting as central, the scan window (in `set_scan_params()`, and `create_connection()` APIs) must be less than the existing Connection Interval.
  - In BLE mode, if scanning and advertising is in progress, and the device subsequently connects and moves to the central role, scanning and advertising stops. To further establish connection to another peripheral device or to a central device, the application must initiate advertising and scanning again.
  - Device powersave must be disabled prior to BT init and de-init.

### Coexistence
  - For concurrent Wi-Fi + BLE, and while a Wi-Fi connection is active, we recommend setting the ratio of the BLE scan window to BLE scan interval to 1:3 or 1:4.
  - Wi-Fi + BLE Advertising
    - All standard advertising intervals are supported. As Wi-Fi throughput is increased, a slight difference in on-air advertisements compared to configured intervals might be observed.
    - BLE advertising is skipped if the advertising interval collides with Wi-Fi activity.
  - Wi-Fi + BLE scanning
    - All standard scan intervals are supported. For better scan results, we recommend setting the ratio of the BLE scan window to BLE scan interval to 1:3 or 1:4.
    - BLE scan would be stopped for intervals that collide with Wi-Fi activity.
  - Wi-Fi + BLE Central/Peripheral Connections
    - All standard connection intervals are supported.
    - For a stable connection, use optimal connection intervals and maximum supervision timeout in the presence of Wi-Fi activity.
  - Wi-Fi + BLE Central/Peripheral Data Transfer
    - To achieve higher throughput for both Wi-Fi and BLE, use medium connection intervals, such as 45 to 80 ms with maximum supervision timeout.
    - Ensure Wi-Fi activity consumes lower intervals.
  - To ensure a stable and seamless connection, connect Wi-Fi before BT/BLE connections in the following operating modes: Wi-Fi + BT + BLE, Wi-Fi + BT, and Wi-Fi + BLE.
  - For Wi-Fi + BT + BLE, Wi-Fi + BT, and Wi-Fi + BLE operating modes, if BT/BLE must be connected before Wi-Fi, use with a high supervision timeout, a high connection interval for BLE, and a high sniff interval for BT to ensure seamless and stable connection. This configuration additionally ensures that the BT/BLE connection stays stable when Wi-Fi connects/disconnects/rejoins.

### System
  - For User Store configuration and Configuration Save, do not enable power save or save it as a configuration. If power save is enabled and saved as a configuration, upon boot up, the RS9116 will boot with the saved configuration and will go to powersave without any indication to the host.
  - Set the compiler optimization level to `O0` in project settings for IDE (KEIL).
  - Memory configuration must be 384K for BT/BLE and co-ex operating modes.
    - Usage of low-power flash mode bit (bit 19 in extended customer feature bitmap). Enable this bit for ultra-low-power standby associated scenarios. This results in about 20&micro;A lower Wi-Fi standby associated current consumption.
  - Memory Configuration in SAPI functions: The default memory configuration is 384K.
  - Apply Opermode commands in AT mode correctly. Using the wrong opermode may lead to unspecified behavior.
  - Set the recommended Power Save Profile (PSP) type to Enhanced Max PSP.
  - It is recommended to disable the power save functionality while updating firmware.
  - For high throughput applications, powersave operation should be disabled.
  - The application is required to set the real-time clock with the correct timestamp when the feature is enabled before establishing a TLS connection.
  - At least one second timeout is a necessity for socket select and receive. No timeout value under one second is supported.

## Known Issues
Following are the list of known issues:

### Network Stack
  - Reassembly of fragmented MQTT packets is not supported in embedded MQTT. MQTT transactions may fail with fragmented packets received from the broker.
  - Sometimes during a TLS handshake, ECC curve parameters may be incorrectly generated resulting in connection failure with BBD2 error. However, this recovers in the next attempt.
  - MQTT disconnects during firmware update when power save is enabled. Disable power save during firmware update and re-enable on update completion.
  - All GCM-based cipher suites are implemented in software and have limited performance.
  - The recommended MQTT publish payload is 1 kBytes.
  - If HTTP server functionality is enabled, do not use port 80 for the MQTT client.
  - Randomize the client port if using rapid connect/disconnect of the MQTT session on the same client port with powersave.
  - Secure TLS renegotiation is not supported in the embedded networking stack.
  - In WPA2 enterprise security mode, getting wireless statistics using rsi_wlan_get() API has restrictions.
  - Example application wlan_throughtput_bt_spp_ble_dual_role is unsupported with bare metal.
  - In concurrent mode, there could be a loss of communication on the STA application when, simultaneously, multiple clients are connected to the APUT.
  - IP address conflict detection has limitations in STA + AP concurrent mode, failure is not reported to host.
  - mqtt_client and mqtt_client_v6 applications do not support SDIO host interface.
  - During firmware update via OTA (TCP/HTTP) methods, if the firmware image received is corrupted, it can result in the module going unresponsive on the next bootup.
  - TLS connection problems may be observed when attempting to revoke public and private keys using existing keys.
  - Observed STA + BT (Connected) test cases not getting throughput results prints in teraterm.
  - Observed inconsistency in TCP & UDP - Rx throughputs in AP alone mode.
  - Observed duplicate IPv6 address is assigned to multiple STAs in AP mode.

### Wi-Fi
  - WPA3 Connection with Hunting and Pecking algorithm takes about 3-4 seconds.
  - If the station performs a scan in concurrent mode (Wi-Fi STA + AP), stations connected to the AP may be disconnected. Enable AP after the STA connection is completed.
  - Issue observed with WPA2 Enterprise Connectivity using Microsoft RADIUS Server.
  - With power save, PTA 3-wire co-existence does not work. However, if a weak pull-up is applied on the GRANT pin, the PTA 3-wire functionality will work while the power is saved. For further information, please refer to Application Notes.
  - EAP security in the aws_device_shadow application is unsupported on the EFR platform due to memory constraints.
  - In STA+AP, If AP started first station scan is supported only in channel in which AP is brought up. If multiple channel scan is requested, AP behaviour is undefined.
  - Auto channel selection with EXT_FEAT_IEEE_80211J feature enabled is not supported in Access point operating mode.
  - Testing for traffic differentiation within a single BSS with 802.11n stations is yielding failures during the 11n certification process.

### BT-Classic/BLE
  - Wake on wireless is not supported for BLE.
  - BT-HID may not interoperate with Apple devices.
  - In Wi-Fi + BT/BLE Co-Ex mode, high Wi-Fi broadcast traffic might cause BT/BLE disconnections.
  - In Wi-Fi + BT mode when there is continuous Wi-Fi data, you might observe BT not re-connecting to the remote after disconnection.
  - For BLE, dual role scenario issues may occur when connecting as a central when the peripheral is advertising. Central connections can be made after peripheral connections are established and advertising stops.
  - A BLE disconnection will occur if both the peer and RS9116 already have bond info and the RS9116 initiates security as part of BLE secure connections. To workaround this issue, the application should not initiate a security request if it already has bond info.
  - A2DP music streaming glitches may be observed if a Wi-Fi download is in progress at the same time.
  - RTOS support is not there in ae_peripheral, and ae_central applications.
  - AE connection issue is expected with Samsung mobiles.
  - BT inquiry would not work in temperature range from -40 to 0 C.

### Coexistence
  - After power save is enabled in co-ex mode, `radio_init()` must be called to turn off power save.
  - In Wi-Fi + BLE mode:
    - BLE may disconnect when the supervision timeout is configured to be less than 16 seconds.
    - WPS does not work.
  - In Wi-Fi + BLE, or BLE only modes:
    - When Wi-Fi disconnects, a BT/BLE reconnection issue is observed.
  - In Wi-Fi + BT mode:
    - When BT SPP is connected, WLAN throughput prints are not displayed in Teraterm with wlan_bt_throughput_app demo.
  - The wlan_https_bt_a2dp_ble_dual_role and wlan_https_bt_a2dp demos are not supported.

### Interoperability
  - Disconnection may occur in Wi-Fi client mode if an AP does not acknowledge QoS null frames with power save configured.

### System
  - Chip Revision 1.3 doesn't support low-power mode optimization using the bit setting defined by EXT_FEAT_LOW_POWER_MODE; setting this results in a hang issue.
  - Sometimes the RS9116 enters power save mode before the configured monitor interval for inactivity when co-ex is configured.
  - The FTP client application does not work with EFx32/STM32 when an RTOS is used.
  - The HTTP OTA update application does not work with EFR32 and EFM32 when an RTOS is used.
  - Firmware update does not work for the EFM32 platform.
  - SAPI driver with UART host interface is supported only on bare metal.
  - Powersave without RAM retention does not work when the SPI interface is used.
  - MQTT Receive may encounter failures in STA + AP concurrent mode.
  - Standby associated listen interval current consumption is higher than expected with EFM Platform.

## Limitations and Unsupported Features
Following are the list of limitations and unsupported features:

### Wi-Fi/Network Stack
  - AMSDU transmit is not supported.
  - UAPSD is not supported.
  - Fragmentation is not supported.
  - AMSDU's within AMPDU is not supported.
  - Currently, the RS9116 does not support the radio measurement requests feature of CCX V2.
  - 802.11k is not supported.
  - Short GI is not supported.
  - 2.4 and 5 GHz bands does not support 40 MHz bandwidth.
  - 802.11j channels less than 16 are not supported.
  - The USB host interface and USB power save are not supported.
  - In AT mode, the total MQTT command length (apart from MQTT publish) should not exceed `150` bytes. This includes `at+rsi` (start of command) to (end of command)`\r\n`.
  - Configure `MQTT_VERSION` in `rsi_mqtt_client.h` based on the server configuration; only version 3 and 4 are supported.
  - 3xTLS connections or 1xTCP and 2xTLS connections are supported concurrently in Wi-Fi only mode.
  - TLS curve IDs supports from 15-28. TLS handshake with third party clients depends on the TLS curve.
  - Big endian architecture is not supported.
  - Auto PAC Provisioning in EAP-FAST with TLSv1.2 is not supported.
  - RC4_SHA, DES_CBC3_SHA, and RC4_MD5 ciphers are not supported in enterprise security.
  - SA query procedure not supported in 11W AP mode.
  - If background scan is enabled along with custom feature bit map bit (8) configuration of the DFS channel support, issues may occur in obtaining an IP address for the Wi-Fi STA.
  - In AP mode, IPv6 Router Advertisements are not supported. Although STA connected to the APUT can transfer IPv6 data to the peers by using SLAAC for address configuration.
  - Transparent mode is unsupported in USB-CDC mode.
  - Channel 144 is not supported.
  - IOP issues may be observed with device running DHCPv6 server with Android devices, as latter do not support DHCPv6 process for IPv6 address configuration.
  - WPA3 AP mode is supported with alpha-level quality.
  - WPA3 AP supports only H2E algorithm.
  - WPA3 AP PMKSA is not supported.
  - WPA3 AP transition mode is not supported.
  - AP standalone mode does not support Tx aggregation. Rx aggregation is supported with limited number of BA sessions.
  - In concurrent mode - AP mode, aggregation (Tx/Rx) is not supported.

### BT-Classic/BLE
  - BT A2DP is only supported for RS9116 Silicon revision 1.5.
  - BT sniff mode does not work if BT multiple slaves feature is enabled.
  - For BLE, if the connection is established with a small connection interval (<15 ms), simultaneous roles are not supported (that is Central/Peripheral + Advertising/Scanning).
  - For BT Classic, only one connection is supported at any time.
  - BLE slave latency value is valid up to 32 only.
  - BT-A2DP encoder is not supported in the firmware.
  - BT-A2DP is not supported in AT mode.
  - Maximum supported  AE data length is 200 bytes.
  - Supports only two ADV_EXT sets.
  - Supports only one BLE connection with AE.
  - SMP is not supported in AE applications "ble_ae_central", and "ble_ae_peripheral".
  - GATT transactions, and Data transfer are not supported in AE applications "ble_ae_central", and "ble_ae_peripheral".
  - BLE duty cycling is not supported.
  - BLE does not support User_Config method.
  - In the rsibt_spptxv2 command, if the data length field value exceeds the actual data bytes, the firmware will continuously poll for the data, resulting in no response or causing it to get stuck.

### Coexistence
  - Wi-Fi STA + BT + BLE multiprotocol use cases require a detailed understanding of use cases and associated configurations. Contact Silicon Labs support for additional details.
  - Wi-Fi AP + BLE, Wi-Fi AP + BT and Wi-Fi AP + BT + BLE modes are not supported.
  - For AT commands, Wi-Fi + BT + BLE(Opermode 9), BT + BLE(Opermode 8) does not work.
  - In Wi-Fi + BLE mode, if the BLE scan interval and window have the same value, then the Wi-Fi connection may be unsuccessful.
  - Advertising Extention feature is not supported in Coexistence.

### System
  - Wake on Wireless support has only been tested for the UART AT command interface.
  - Firmware update via bootloader method requires the user to check the integrity of the RPS file content. A corrupted image may cause the chip to go unresponsive to host commands.

## Notes
### BT-Classic/BLE
 - Acceptlist address should be given in reverse order, that is in big endian format.
 - AE feature validated using revision 1.5 AC1 expansion board.
 - AE feature is not supported in RS9116 revision 1.4.
 - Advertising Extentions is Beta quality feature.

# WiSeConnect 2.10.3 Release Notes

Last updated: September 19, 2024

## Highlights

  - Bug fix

## Release Details
| Item                                                | Details                                |
|-----------------------------------------------------|----------------------------------------|
| API Version (SAPI)                                  | 2.10.3.2 (Build 2)                     |
| Firmware Version                                    | 2.10.3.0.2 (Build 2)                   |
| Package Name                                        | RS9116W.2.10.3.2                       |
| Supported RTOS                                      | FreeRTOS                               |
| Hardware Modules                                    | QMS, B00, C00, CC1, AB0, AB1, AA0, AA1, AC0, AC1 |
| Hardware Chipsets                                   | Chip revision 1.3, Chip revision 1.4, Chip revision 1.5   |
| Operating Modes Supported                           | Wi-Fi STA, Wi-Fi AP, Wi-Fi STA+BLE, Wi-Fi STA+AP     |
| Additional Operating Modes Supported (Beta Quality) | Wi-Fi STA+BT, Wi-Fi STA+BT+BLE , BLE(With AE)     |

### Notes
  - The latest releases have bug fixes, enhancements, and new features in both 'SAPIs', and 'Firmware'. Therefore, it is recommended to update, and use 'SAPIs', and 'Firmware' of same version.

## Updates in this Release
RS9116W 2.10.3.2 release consists of two components, which are as following:
  - Firmware - RS9116 Firmware Binary
  - SAPI Library - SAPI Library runs on Host

This release is meant only for use with designs based on RS9116 Silicon revision1.4 (RS9116X-xxx-xxx-Bxx) and RS9116 Silicon revision1.5 (RS9116X-xxx-xxx-Cxx).
Customers using the RS9116X-xxx-xxx-Xxx parts (Silicon rev 1.3) can upgrade to the latest firmware. However, the power optimization feature macro EXT_FEAT_LOW_POWER_MODE in rsi_wlan_config.h must be disabled for revision 1.3.


Choose the firmware to match the Silicon revision, which is a part of your design. Following Silicon revisions are available:
- RS9116W.2.10.3.0.2.rps
  - RS9116X-xxx-xxx-Xxx (Silicon revision1.3)
  - RS9116X-xxx-xxx-Bxx (Silicon revision1.4)
- RS916W.2.10.3.0.2.rps
  - RS9116X-xxx-xxx-Cxx (Silicon revision1.5)
  - RS916WS00ACxxx

## New Features
Following are the list of new features implemented:

### Wi-Fi
  - None

### System
  - Added support for the SPI clock frequency to switch to high only, when the STM32 high-speed mode is enabled.

### BT-Classic/BLE
  - None

### Network Stack
  - Added RTOS support for HTTP client application.

## Changes and Fixes
The list of fixes and changes for this version are outlined further:

### Wi-Fi
  - Added the macro for WPA-WPA2 mixed security mode.
  - Fixed an issue where hidden SSID is not working in open security mode.

### Network Stack
  - Updated the AWS certificate as per the AWS guidelines.
  - Fixed security vulnerabilities (CVE-2023-48315, CVE-2023-48316, CVE-2023-48691 and CVE-2023-48692) in NetX Duo.
  - Fixed an issue where the device would hang when the TLS socket connection was reset due to a loss of connectivity with the peer.
  - Fixed an issue where DNS query was causing the 0xBBA3 error.
  - Fixed an issue where rsi_send_large_data_async() API was failing to send data in congested environment.
  - Combined CA certificate (Starfield Class 2 and Starfield G2) added in the WiSeConnect SDK.

### BT-Classic/BLE
  - None

### Coexistence
  - Added rsi_send_feature_frame() API call in the application to fix BLE disconnection issue when the host MCU clock frequency is high.

### System
  - None

### Documentation
  - Added rsi_ap_stop command documentation to AT PRM.
  - Updated the send_timeout() API, timeout_type argument with proper macros that should be used.
  - Updated the hyperlink for wlan_ble_power_save() example.
  - Unsupported features are removed from SAPI PRM and AT PRM.
  - Updated Azure IoT hubs root CA to DigiCert Global root G2.
  - Pre-Condition is updated for rsi_wlan_power_save_profile(), and rsi_wlan_power_save_with_listen_interval() APIs.
  - rsi_auto_config_rsp_handler() API documentation is updated.

## Recommendations and New Options
Following are the list of recommendations and new options:

### Wi-Fi
  - Enable aggregation using the opermode parameter [feature_bit_map\[2\]](https://docs.silabs.com/rs9116-wiseconnect/2.6/wifibt-wc-sapi-reference/opermode#rsi-feature-bit-map).
  - Disable high MCS rates to improve the robustness of a connection for low throughput applications in congested networks. For instance, turning off `MCS6` and `MCS7` may help a Smart Lock in a congested wireless environment.
  - To restart the RS9116, the application should call `rsi_driver_deinit()` followed by `rsi_driver_init()` and `rsi_device_init()`. For OS cases, additionally call `rsi_task_destroy(driver_task_handle)` to delete the driver task before calling `rsi_driver_deinit()` and create again after `rsi_device_init()` using `rsi_task_create()`.

### Network Stack
  - To ensure graceful handling during asynchronous TCP closures from a peer, enable the opermode parameter [ext_tcp_ip_feature_bit_map\[16\]](https://docs.silabs.com/rs9116-wiseconnect/2.6/wifibt-wc-sapi-reference/opermode#rsi-ext-tcpip-feature-bitmap).
  - To avoid TCP disconnects during a rejoin, set TCP retransmission count to >= `30`.
  - Implemented a new API - `rsi_network_app_protocol_config()` to configure the network application protocols such as cipher selection for HTTP protocol.

### BT-Classic/BLE
  - In BLE, the recommended range of the BLEConnection Interval is as follows:
    - Power Save (BLE only) - 100 ms to 1.28 s
    - BT Classic + BLE Dual Mode is >= 200 ms
    - Wi-Fi + BLE coex - 30 ms to 250 ms
  - In BLE during connection, it is not recommended to configure the scan window, and scan interval with the same value.
  - In BT Classic, the recommended Sniff Interval configuration during powersave is limited to 100 ms (\<= 100).
  - In BLE, if a device is acting as central, the scan window (in `set_scan_params()`, and `create_connection()` APIs) must be less than the existing Connection Interval.
  - In BLE mode, if scanning and advertising is in progress, and the device subsequently connects and moves to the central role, scanning and advertising stops. To further establish connection to another peripheral device or to a central device, the application must initiate advertising and scanning again.
  - Device powersave must be disabled prior to BT init and de-init.

### Coexistence
  - For concurrent Wi-Fi + BLE, and while a Wi-Fi connection is active, we recommend setting the ratio of the BLE scan window to BLE scan interval to 1:3 or 1:4.
  - Wi-Fi + BLE Advertising
    - All standard advertising intervals are supported. As Wi-Fi throughput is increased, a slight difference in on-air advertisements compared to configured intervals might be observed.
    - BLE advertising is skipped if the advertising interval collides with Wi-Fi activity.
  - Wi-Fi + BLE scanning
    - All standard scan intervals are supported. For better scan results, we recommend setting the ratio of the BLE scan window to BLE scan interval to 1:3 or 1:4.
    - BLE scan would be stopped for intervals that collide with Wi-Fi activity.
  - Wi-Fi + BLE Central/Peripheral Connections
    - All standard connection intervals are supported.
    - For a stable connection, use optimal connection intervals and maximum supervision timeout in the presence of Wi-Fi activity.
  - Wi-Fi + BLE Central/Peripheral Data Transfer
    - To achieve higher throughput for both Wi-Fi and BLE, use medium connection intervals, such as 45 to 80 ms with maximum supervision timeout.
    - Ensure Wi-Fi activity consumes lower intervals.
  - To ensure a stable and seamless connection, connect Wi-Fi before BT/BLE connections in the following operating modes: Wi-Fi + BT + BLE, Wi-Fi + BT, and Wi-Fi + BLE.
  - For Wi-Fi + BT + BLE, Wi-Fi + BT, and Wi-Fi + BLE operating modes, if BT/BLE must be connected before Wi-Fi, use with a high supervision timeout, a high connection interval for BLE, and a high sniff interval for BT to ensure seamless and stable connection. This configuration additionally ensures that the BT/BLE connection stays stable when Wi-Fi connects/disconnects/rejoins.

### System
  - For User Store configuration and Configuration Save, do not enable power save or save it as a configuration. If power save is enabled and saved as a configuration, upon boot up, the RS9116 will boot with the saved configuration and will go to powersave without any indication to the host.
  - Set the compiler optimization level to `O0` in project settings for IDE (KEIL).
  - Memory configuration must be 384K for BT/BLE and co-ex operating modes.
    - Usage of low-power flash mode bit (bit 19 in extended customer feature bitmap). Enable this bit for ultra-low-power standby associated scenarios. This results in about 20&micro;A lower Wi-Fi standby associated current consumption.
  - Memory Configuration in SAPI functions: The default memory configuration is 384K.
  - Apply Opermode commands in AT mode correctly. Using the wrong opermode may lead to unspecified behavior.
  - Set the recommended Power Save Profile (PSP) type to Enhanced Max PSP.
  - It is recommended to disable the power save functionality while updating firmware.
  - For high throughput applications, powersave operation should be disabled.
  - The application is required to set the real-time clock with the correct timestamp when the feature is enabled before establishing a TLS connection.
  - At least one second timeout is a necessity for socket select and receive. No timeout value under one second is supported.

## Known Issues
Following are the list of known issues:

### Network Stack
  - Reassembly of fragmented MQTT packets is not supported in embedded MQTT. MQTT transactions may fail with fragmented packets received from the broker.
  - Sometimes during a TLS handshake, ECC curve parameters may be incorrectly generated resulting in connection failure with BBD2 error. However, this recovers in the next attempt.
  - MQTT disconnects during firmware update when power save is enabled. Disable power save during firmware update and re-enable on update completion.
  - All GCM-based cipher suites are implemented in software and have limited performance.
  - The recommended MQTT publish payload is 1 kBytes.
  - If HTTP server functionality is enabled, do not use port 80 for the MQTT client.
  - Randomize the client port if using rapid connect/disconnect of the MQTT session on the same client port with powersave.
  - Secure TLS renegotiation is not supported in the embedded networking stack.
  - In WPA2 enterprise security mode, getting wireless statistics using rsi_wlan_get() API has restrictions.
  - Example application wlan_throughtput_bt_spp_ble_dual_role is unsupported with bare metal.
  - In concurrent mode, there could be a loss of communication on the STA application when, simultaneously, multiple clients are connected to the APUT.
  - IP address conflict detection has limitations in STA + AP concurrent mode, failure is not reported to host.
  - mqtt_client and mqtt_client_v6 applications do not support SDIO host interface.
  - During firmware update via OTA (TCP/HTTP) methods, if the firmware image received is corrupted, it can result in the module going unresponsive on the next bootup.
  - TLS connection problems may be observed when attempting to revoke public and private keys using existing keys.
  - Observed STA + BT (Connected) test cases not getting throughput results prints in teraterm.
  - Observed inconsistency in TCP & UDP - Rx throughputs in AP alone mode.
  - Observed duplicate IPv6 address is assigned to multiple STAs in AP mode.

### Wi-Fi
  - WPA3 Connection with Hunting and Pecking algorithm takes about 3-4 seconds.
  - If the station performs a scan in concurrent mode (Wi-Fi STA + AP), stations connected to the AP may be disconnected. Enable AP after the STA connection is completed.
  - Issue observed with WPA2 Enterprise Connectivity using Microsoft RADIUS Server.
  - With power save, PTA 3-wire co-existence does not work. However, if a weak pull-up is applied on the GRANT pin, the PTA 3-wire functionality will work while the power is saved. For further information, please refer to Application Notes.
  - EAP security in the aws_device_shadow application is unsupported on the EFR platform due to memory constraints.
  - In STA+AP, If AP started first station scan is supported only in channel in which AP is brought up. If multiple channel scan is requested, AP behaviour is undefined.
  - Auto channel selection with EXT_FEAT_IEEE_80211J feature enabled is not supported in Access point operating mode.
  - Testing for traffic differentiation within a single BSS with 802.11n stations is yielding failures during the 11n certification process.

### BT-Classic/BLE
  - Wake on wireless is not supported for BLE.
  - BT-HID may not interoperate with Apple devices.
  - In Wi-Fi + BT/BLE Co-Ex mode, high Wi-Fi broadcast traffic might cause BT/BLE disconnections.
  - In Wi-Fi + BT mode when there is continuous Wi-Fi data, you might observe BT not re-connecting to the remote after disconnection.
  - For BLE, dual role scenario issues may occur when connecting as a central when the peripheral is advertising. Central connections can be made after peripheral connections are established and advertising stops.
  - A BLE disconnection will occur if both the peer and RS9116 already have bond info and the RS9116 initiates security as part of BLE secure connections. To workaround this issue, the application should not initiate a security request if it already has bond info.
  - A2DP music streaming glitches may be observed if a Wi-Fi download is in progress at the same time.
  - RTOS support is not there in ae_peripheral, and ae_central applications.
  - AE connection issue is expected with Samsung mobiles.
  - BT inquiry would not work in temperature range from -40 to 0 C.

### Coexistence
  - After power save is enabled in co-ex mode, `radio_init()` must be called to turn off power save.
  - In Wi-Fi + BLE mode:
    - BLE may disconnect when the supervision timeout is configured to be less than 16 seconds.
    - WPS does not work.
  - In Wi-Fi + BLE, or BLE only modes:
    - When Wi-Fi disconnects, a BT/BLE reconnection issue is observed.
  - In Wi-Fi + BT mode:
    - When BT SPP is connected, WLAN throughput prints are not displayed in Teraterm with wlan_bt_throughput_app demo.
  - The wlan_https_bt_a2dp_ble_dual_role and wlan_https_bt_a2dp demos are not supported.

### Interoperability
  - Disconnection may occur in Wi-Fi client mode if an AP does not acknowledge QoS null frames with power save configured.

### System
  - Chip Revision 1.3 doesn't support low-power mode optimization using the bit setting defined by EXT_FEAT_LOW_POWER_MODE; setting this results in a hang issue.
  - Sometimes the RS9116 enters power save mode before the configured monitor interval for inactivity when co-ex is configured.
  - The FTP client application does not work with EFx32/STM32 when an RTOS is used.
  - The HTTP OTA update application does not work with EFR32 and EFM32 when an RTOS is used.
  - Firmware update does not work for the EFM32 platform.
  - SAPI driver with UART host interface is supported only on bare metal.
  - Powersave without RAM retention does not work when the SPI interface is used.
  - MQTT Receive may encounter failures in STA + AP concurrent mode.
  - Standby associated listen interval current consumption is higher than expected with EFM Platform.

## Limitations and Unsupported Features
Following are the list of limitations and unsupported features:

### Wi-Fi/Network Stack
  - AMSDU transmit is not supported.
  - UAPSD is not supported.
  - Fragmentation is not supported.
  - AMSDU's within AMPDU is not supported.
  - Currently, the RS9116 does not support the radio measurement requests feature of CCX V2.
  - 802.11k is not supported.
  - Short GI is not supported.
  - 2.4 and 5 GHz bands does not support 40 MHz bandwidth.
  - 802.11j channels less than 16 are not supported.
  - The USB host interface and USB power save are not supported.
  - In AT mode, the total MQTT command length (apart from MQTT publish) should not exceed `150` bytes. This includes `at+rsi` (start of command) to (end of command)`\r\n`.
  - Configure `MQTT_VERSION` in `rsi_mqtt_client.h` based on the server configuration; only version 3 and 4 are supported.
  - 3xTLS connections or 1xTCP and 2xTLS connections are supported concurrently in Wi-Fi only mode.
  - TLS curve IDs supports from 15-28. TLS handshake with third party clients depends on the TLS curve.
  - Big endian architecture is not supported.
  - Auto PAC Provisioning in EAP-FAST with TLSv1.2 is not supported.
  - RC4_SHA, DES_CBC3_SHA, and RC4_MD5 ciphers are not supported in enterprise security.
  - SA query procedure not supported in 11W AP mode.
  - If background scan is enabled along with custom feature bit map bit (8) configuration of the DFS channel support, issues may occur in obtaining an IP address for the Wi-Fi STA.
  - In AP mode, IPv6 Router Advertisements are not supported. Although STA connected to the APUT can transfer IPv6 data to the peers by using SLAAC for address configuration.
  - Transparent mode is unsupported in USB-CDC mode.
  - Channel 144 is not supported.
  - IOP issues may be observed with device running DHCPv6 server with Android devices, as latter do not support DHCPv6 process for IPv6 address configuration.
  - WPA3 AP mode is supported with alpha-level quality.
  - WPA3 AP supports only H2E algorithm.
  - WPA3 AP PMKSA is not supported.
  - WPA3 AP transition mode is not supported.
  - AP standalone mode does not support Tx aggregation. Rx aggregation is supported with limited number of BA sessions.
  - In concurrent mode - AP mode, aggregation (Tx/Rx) is not supported.

### BT-Classic/BLE
  - BT A2DP is only supported for RS9116 Silicon revision 1.5.
  - BT sniff mode does not work if BT multiple slaves feature is enabled.
  - For BLE, if the connection is established with a small connection interval (<15 ms), simultaneous roles are not supported (that is Central/Peripheral + Advertising/Scanning).
  - For BT Classic, only one connection is supported at any time.
  - BLE slave latency value is valid up to 32 only.
  - BT-A2DP encoder is not supported in the firmware.
  - BT-A2DP is not supported in AT mode.
  - Maximum supported  AE data length is 200 bytes.
  - Supports only two ADV_EXT sets.
  - Supports only one BLE connection with AE.
  - SMP is not supported in AE applications "ble_ae_central", and "ble_ae_peripheral".
  - GATT transactions, and Data transfer are not supported in AE applications "ble_ae_central", and "ble_ae_peripheral".
  - BLE duty cycling is not supported.
  - BLE does not support User_Config method.

### Coexistence
  - Wi-Fi STA + BT + BLE multiprotocol use cases require a detailed understanding of use cases and associated configurations. Contact Silicon Labs support for additional details.
  - Wi-Fi AP + BLE, Wi-Fi AP + BT and Wi-Fi AP + BT + BLE modes are not supported.
  - For AT commands, Wi-Fi + BT + BLE(Opermode 9), BT + BLE(Opermode 8) does not work.
  - In Wi-Fi + BLE mode, if the BLE scan interval and window have the same value, then the Wi-Fi connection may be unsuccessful.
  - Advertising Extention feature is not supported in Coexistence.

### System
  - Wake on Wireless support has only been tested for the UART AT command interface.
  - Firmware update via bootloader method requires the user to check the integrity of the RPS file content. A corrupted image may cause the chip to go unresponsive to host commands.

## Notes
### BT-Classic/BLE
 - Acceptlist address should be given in reverse order, that is in big endian format.
 - AE feature validated using revision 1.5 AC1 expansion board.
 - AE feature is not supported in RS9116 revision 1.4.
 - Advertising Extentions is Beta quality feature.

# WiSeConnect 2.10.2 Release Notes

Last updated: September 3, 2024

## Highlights

  - Bug fix

## Release Details
| Item                                                | Details                                |
|-----------------------------------------------------|----------------------------------------|
| API Version (SAPI)                                  | 2.10.2.3 (Build 3)                     |
| Firmware Version                                    | 2.10.2.0.3 (Build 3)                   |
| Package Name                                        | RS9116W.2.10.2.3                       |
| Supported RTOS                                      | FreeRTOS                               |
| Hardware Modules                                    | QMS, B00, C00, CC1, AB0, AB1, AA0, AA1, AC0, AC1 |
| Hardware Chipsets                                   | Chip Revision 1.3, Chip Revision 1.4, Chip Revision 1.5   |
| Operating Modes Supported                           | Wi-Fi STA, Wi-Fi AP, Wi-Fi STA+BLE, Wi-Fi STA+AP     |
| Additional Operating Modes Supported (Beta Quality) | Wi-Fi STA+BT, Wi-Fi STA+BT+BLE , BLE(With AE)     |

### Notes
  - The latest releases will have bug-fixes, enhancements, and new features in both 'SAPIs' and 'Firmware'. Hence, it is recommended to update and use 'SAPIs' and 'Firmware' of same version.

## Updates in this Release
RS9116W 2.10.2.3 release consists of two components, as follows:
  - Firmware - RS9116 Firmware Binary
  - SAPI Library - SAPI Library runs on Host

This release is meant only for use with designs based on RS9116 Silicon rev 1.4 (RS9116X-xxx-xxx-Bxx) and RS9116 Silicon rev 1.5 (RS9116X-xxx-xxx-Cxx).
Customers using the RS9116X-xxx-xxx-Xxx parts (Silicon rev 1.3) can also upgrade to the latest firmware, however, the power optimization feature macro `EXT_FEAT_LOW_POWER_MODE` in `rsi_wlan_config.h` must be disabled for revision 1.3.


Choose the firmware to match the silicon revision used in your design, as follows:
- RS9116W.2.10.2.0.3.rps
  - RS9116X-xxx-xxx-Xxx (Silicon rev 1.3)
  - RS9116X-xxx-xxx-Bxx (Silicon rev 1.4)
- RS916W.2.10.2.0.3.rps
  - RS9116X-xxx-xxx-Cxx (Silicon rev 1.5)
  - RS916WS00ACxxx

## New Features
### Wi-Fi
  - None

### System
  - None

### BT-Classic/BLE
  - None

### Network Stack
  - None

## Changes and Fixes
### Wi-Fi
  - None

### Network Stack
  - Resolved an issue where the device would hang when the TLS socket connection was reset due to a loss of connectivity with the peer.
  
### BT-Classic/BLE
  - None
  
### Coexistence
  - None

### System
  - None
  
### Documentation
  - None
  
## Recommendations and New Options

### Wi-Fi
  - Enable aggregation using the opermode parameter [feature_bit_map\[2\]](https://docs.silabs.com/rs9116-wiseconnect/2.6/wifibt-wc-sapi-reference/opermode#rsi-feature-bit-map)
  - To make a connection more robust for low throughput applications in busy networks, disable high MCS rates. For example, in a congested wireless envionment, a Smart Lock may benefit by disabling `MCS6` and `MCS7`.
  - To restart the RS9116, the application should call `rsi_driver_deinit()` followed by `rsi_driver_init()` and `rsi_device_init()`. For OS cases, additionally call `rsi_task_destroy(driver_task_handle)` to delete the driver task before calling `rsi_driver_deinit()` and create again after `rsi_device_init()` using `rsi_task_create()`.

### Network Stack
  - To ensure graceful handling during asynchronous TCP closures from a peer, enable the opermode parameter [ext_tcp_ip_feature_bit_map\[16\]](https://docs.silabs.com/rs9116-wiseconnect/2.6/wifibt-wc-sapi-reference/opermode#rsi-ext-tcpip-feature-bitmap)
  - To avoid TCP disconnects during a rejoin, set TCP retransmission count to >= `30`
  - Implemented a new API - `rsi_network_app_protocol_config()` to configure the network application protocols such as cipher selection for HTTP protocol.

### BT-Classic/BLE
  - In BLE, the recommended range of the BLEConnection Interval is as follows:
    - Power Save (BLE Only) - 100 ms to 1.28 s
    - BT Classic + BLE Dual Mode is >= 200 ms
    - Wi-Fi + BLE coex - 30 ms to 250 ms
  - In BLE during Connection, the configuration of Scan Interval and Scan Window with the same value is not recommended
  - In BT Classic, the recommended Sniff Interval configuration during powersave is limited to 100 ms (\<= 100).
  - In BLE, if a device is acting as Central, the scan window (in `set_scan_params()` and `create_connection()` APIs) must be less than the existing Connection Interval.
  - In BLE mode, if scanning and advertising is in progress, and the device subsequently connects and moves to the central role, scanning and advertising stops. To further establish connection to another peripheral device or to a central device, the application must initiate advertising and scanning again.
  - Device powersave must be disabled prior to BT init and de-init.

### Coexistence
  - For concurrent Wi-Fi + BLE, and while a Wi-Fi connection is active, we recommend setting the ratio of the BLE scan window to BLE scan interval to 1:3 or 1:4.
  - Wi-Fi + BLE Advertising
    - All standard advertising intervals are supported. As Wi-Fi throughput is increased, a slight difference in on-air advertisements compared to configured intervals may be observed.
    - BLE advertising is skipped if the advertising interval collides with Wi-Fi activity.
  - Wi-Fi + BLE scanning
    - All standard scan intervals are supported. For better scan results, we recommend setting the ratio of the BLE scan window to BLE scan interval to 1:3 or 1:4.
    - BLE scanning will be stopped for intervals that collide with Wi-Fi activity.
  - Wi-Fi + BLE Central/Peripheral Connections
    - All standard connection intervals are supported.
    - For a stable connection, use optimal connection intervals and max supervision timeout in the presence of Wi-Fi activity.
  - Wi-Fi + BLE Central/Peripheral Data Transfer
    - To achieve higher throughput for both Wi-Fi and BLE, use medium connection intervals, such as 45 to 80 ms with maximum supervision timeout.
    - Ensure Wi-Fi activity consumes lower intervals.
  - For Wi-Fi + BT + BLE, Wi-Fi + BT, Wi-Fi + BLE operating modes, connect Wi-Fi before BT/BLE connections to ensure seamless, stable connection for all 3 protocols.
  - For Wi-Fi + BT + BLE, Wi-Fi + BT, Wi-Fi + BLE operating modes, if BT/BLE needs to be connected before a Wi-Fi connection, use with high supervision timeout and high connection interval for BLE and high sniff interval for BT, to ensure seamless, stable connection. This configuration also ensures a stable BT/BLE connection when Wi-Fi connects/disconnects/rejoins.

### System
  - For User Store configuration and Configuration Save, do not enable power save or save it as a configuration. If power save is enabled and saved as a configuration, upon boot up, the RS9116 will boot with the saved configuration and will go to powersave without any indication to the host.
  - Set the compiler optimization level to `O0` in project settings for IDE (KEIL).
  - Memory configuration must be 384K for BT/BLE and co-ex operating modes
    - Usage of low power flash mode bit (bit 19 in extended customer feature bitmap). Enable this bit for ultra low power standby associated scenarios. This results in about 20&micro;A lower Wi-Fi standby associated current consumption.
  - Memory Configuration in SAPI functions: The default memory configuration is 384K.
  - Apply Opermode commands in AT mode correctly. Using the wrong opermode may lead to unspecified behavior.
  - Set the recommended Power Save Profile (PSP) type to Enhanced Max PSP.
  - During firmware updates, powersave operation should be disabled.
  - For high throughput applications, powersave operation should be disabled.
  - The application must set the real-time clock with correct timestamp when the feature is enabled before establishing a TLS connection.
  - The socket select and socket receive timeout must be at least 1 second. A timeout value under 1 second is not supported.

## Known Issues

### Network Stack
  - Reassembly of fragmented MQTT packets is not supported in embedded MQTT. MQTT transactions may fail with fragemented packets received from the broker.
  - Sometimes during a TLS handshake, ECC curve parameters may be incorrectly generated resulting in connection failure with BBD2 error. However, this recovers in the next attempt.
  - MQTT disconnects during firmware update when power save is enabled. Disable power save during firmware update and re-enable on update completion.
  - All GCM-based cipher suites are implemented in software and have limited performance.
  - The recommended MQTT publish payload is 1 kBytes.
  - If HTTP server functionality is enabled, do not use port 80 for the MQTT client.
  - Randomize the client port if using rapid connect/disconnect of the MQTT session on the same client port with powersave.
  - Secure TLS renegotiation is not supported in the embedded networking stack.
  - In WPA2 enterprise security mode getting wireless statistics using rsi_wlan_get() API has restrictions.
  - Example application wlan_throughtput_bt_spp_ble_dual_role is unsupported with bare metal.
  - In concurrent mode, there could be a loss of communication on the STA application, when simultaneously multiple clients are connected to the APUT.
  - IP address conflict detection has limitations in STA + AP concurrent mode, failure is not reported to host.
  - mqtt_client and mqtt_client_v6 applications don't support SDIO host interface
  - During firmware update via OTA (TCP/HTTP) methods, if the firmware image received is corrupted it can result in module going unresponsive on the next bootup.
  - TLS connection problems may be observed when attempting to revoke public and private keys using existing keys.
  - Observed connection failure (error_code 0x0008) with WPA2-Enterprise Security with pathlength enabled certificate.
  - Observed STA+BT(Connected) test cases not getting throughput results prints in teraterm.
  - Observed inconsistency in TCP & UDP - Rx throughputs in AP alone mode.
  - Observed duplicate IPv6 address is assigned to multiple STAs in AP mode.

### Wi-Fi
  - WPA3 Connection with Hunting and Pecking algorithm takes about 3-4 seconds.
  - If the station performs a scan in concurrent mode (Wi-Fi STA+AP), stations connected to the AP may be disconnected. Enable AP after the STA connection is completed.
  - Issue observed with WPA2 Enterprise Connectivity using Microsoft RADIUS Server.
  - PTA 3-wire coexistence does not work with power save. However, if a weak pull up is applied to the GRANT pin, PTA 3-wire functionality will work with power save. Details can be found in the application notes.
  - EAP security in the aws_device_shadow application is unsupported on the EFR platform due to memory constraints.
  - In STA+AP, If AP started first station scan is supported only in channel in which AP is brought up. If multiple channel scan is requested, AP behaviour is undefined.
  - Auto channel selection with EXT_FEAT_IEEE_80211J feature enabled is not supported in Access point operating mode.
  - Testing for traffic differentiation within a single BSS with 802.11n stations is yielding failures during the 11n certification process.

### BT-Classic/BLE
  - Wake on wireless is not supported for BLE.
  - BT-HID may not interoperate with Apple devices.
  - In Wi-Fi + BT/BLE Co-Ex mode, high Wi-Fi broadcast traffic might cause BT/BLE disconnections.
  - In Wi-Fi + BT mode when there is continuous Wi-Fi data, you might observe BT not re-connecting to the remote after disconnection.
  - For BLE, dual role scenario issues may occur when connecting as a central when the peripheral is advertising. Central connections can be made after peripheral connections are established and advertising stops.
  - A BLE disconnection will occur if both the peer and RS9116 already have bond info and the RS9116 initiates security as part of BLE secure connections. To workaround this issue, the application should not initiate a security request if it already has bond info.
  - A2DP music streaming glitches may be observed if a Wi-Fi download is in progress at the same time.
  - RTOS support is not there in ae_peripheral & ae_central applications.
  - AE connection issue is expected with Samsung mobiles.
  - BT inquiry will not work in temperatures ranging from -40C to 0C.

### Coexistence
  - After power save is enabled in co-ex mode, `radio_init()` must be called to turn off power save.
  - In Wi-Fi + BLE mode:
    - BLE may disconnect when the supervision timeout is configured to be less than 16 seconds.
    - WPS does not work.
  - In Wi-Fi + BLE / BLE only modes:
    - When Wi-Fi disconnects, a BT/BLE reconnection issue is observed.
  - In Wi-Fi + BT mode:
    - When BT SPP is connected, WLAN throughput prints are not displayed in Teraterm with wlan_bt_throughput_app demo
  - The wlan_https_bt_a2dp_ble_dual_role and wlan_https_bt_a2dp demos are not supported.

### Interoperability
  - Disconnection may occur in Wi-Fi client mode if an AP does not acknowledge QoS null frames with power save configured.

### System
  - Chip Revision 1.3 doesn't support Low power mode optimization using the bit setting defined by EXT_FEAT_LOW_POWER_MODE, setting this results in a hang issue.
  - Sometimes the RS9116 enters power save mode before the configured monitor interval for inactivity when co-ex is configured.
  - The FTP client application does not work with EFx32/STM32 when an RTOS is used.
  - The HTTP OTA update application does not work with EFR32 and EFM32 when an RTOS is used.
  - Firmware update does not work for the EFM32 platform.
  - SAPI driver with UART host interface is supported only on bare metal.
  - Powersave without RAM retention does not work when the SPI interface is used.
  - MQTT Receive may encounter failures in STA + AP concurrent mode.
  - Standby associated listen interval current consumption is higher than expected with EFM Platform.

## Limitations and Unsupported Features

### Wi-Fi/Network Stack
  - AMSDU transmit is not supported.
  - UAPSD is not supported.
  - Fragmentation is not supported.
  - AMSDU's within AMPDU is not supported.
  - Currently, the RS9116 does not support the radio measurement requests feature of CCX V2
  - 802.11k is not supported
  - Short GI is not supported
  - 40 MHz bandwidth for both the 2.4 & 5 GHz bands is not supported.
  - 802.11j channels less than 16 are not supported.
  - The USB host interface and USB power save are not supported.
  - In AT mode, the total MQTT command length (apart from MQTT publish) should not exceed `150` bytes. This includes `at+rsi` (start of command) to (end of command)`\r\n`.
  - Configure `MQTT_VERSION` in `rsi_mqtt_client.h` based on the server configuration; only version 3 and 4 are supported.
  - 3xTLS connections or 1xTCP and 2xTLS connection, connections are supported concurrently in Wi-Fi only mode.
  - TLS curve IDs supported are 15-28. TLS handshake with 3rd party clients depends on the TLS curve.
  - Big endian architecture is not supported.
  - Auto PAC Provisioning in EAP-FAST with TLSv1.2 is not supported.
  - RC4_SHA, DES_CBC3_SHA, and RC4_MD5 ciphers are not supported in enterprise security
  - SA query procedure not supported in 11W AP mode.
  - If background scan is enabled along with custom feature bit map bit (8) configuration of the DFS channel support, issues may occur in obtaining an IP address for the Wi-Fi STA.
  - In AP mode, IPv6 Router Advertisements are not supported. Although STA connected to the APUT can transfer IPv6 data to the peers by using SLAAC for address configuration.
  - Transparent mode is unsupported in USB-CDC mode.
  - Channel 144 is not supported.
  - IOP issues may be observed with device running DHCPv6 server with Android devices, as latter don't support DHCPv6 process for IPv6 address configuration.
  - WPA3 AP mode is supported with alpha-level quality
  - WPA3 AP supports only H2E algorithm.
  - WPA3 AP PMKSA is not supported.
  - WPA3 AP transition mode is not supported.
  - AP standalone mode does not support Tx aggregation. Rx aggregation is supported with limited number of BA sessions.
  - In concurrent mode - AP mode, aggregation (Tx/Rx) is not supported.

### BT-Classic/BLE
  - BT A2DP is only supported for RS9116 silicon revision 1.5.
  - BT sniff mode does not work if BT multiple slaves feature is enabled.
  - For BLE, if the connection is established with a small connection interval (<15 ms), simultaneous roles are not supported (i.e., Central/Peripheral + Advertising/Scanning).
  - For BT Classic, only 1 connection is supported at any time.
  - BLE slave latency value is valid up to 32 only.
  - BT-A2DP encoder is not supported in the firmware.
  - BT-A2DP is not supported in AT mode.
  - Maximum supported  AE data length is 200 bytes.
  - Supports only two ADV_EXT sets.
  - Supports only one BLE connection with AE.
  - SMP is not supported in AE applications "ble_ae_central" and "ble_ae_peripheral".
  - GATT transactions & Data transfer are not supported in AE applications  "ble_ae_central" and "ble_ae_peripheral".
  - BLE duty cycling is not supported.
  - User_config method is not supported for BLE.

### Coexistence
  - Wi-Fi STA + BT + BLE multiprotocol use cases require a detailed understanding of use cases and associated configurations. Contact Silicon Labs support for additional details.
  - Wi-Fi AP + BLE, Wi-Fi AP + BT & Wi-Fi AP + BT + BLE modes are not supported.
  - For AT commands, Wi-Fi+BT+BLE( Opermode 9 ), BT+BLE(Opermode 8 ) do not work.
  - In Wi-Fi + BLE mode, if the BLE scan interval and window have the same value, then the Wi-Fi connection may be unsuccessful.
  - Advertising Extention feature is not supported in Coexistence.

### System
  - Wake on Wireless support has only been tested for the UART AT command interface.
  - Firmware update via bootloader method requires the user to check the integrity of the RPS file content. A corrupted image may cause the chip to go unresponsive to host commands.

## Notes
### BT-Classic/BLE
 - Acceptlist address should be given in reverse order, i.e in big endian format.
 - AE feature validated using rev 1.5 AC1 expansion board.
 - AE feature is not supported in 9116 rev 1.4.
 - Advertising Extentions is Beta quality feature.

# WiSeConnect 2.10.1 Release Notes

Last updated: August 9, 2024

## Highlights

  - New features
  - Bug fixes

## Release Details
| Item                                                | Details                                |
|-----------------------------------------------------|----------------------------------------|
| Release date                                        | August 16, 2024                        |
| API Version (SAPI)                                  | 2.10.1.3 (Build 3)                     |
| Firmware Version                                    | 2.10.1.0.3 (Build 3)                   |
| Package Name                                        | RS9116W.2.10.1.3                       |
| Supported RTOS                                      | FreeRTOS                               |
| Hardware Modules                                    | QMS, B00, C00, CC1, AB0, AB1, AA0, AA1, AC0, AC1 |
| Hardware Chipsets                                   | Chip Revision 1.3, Chip Revision 1.4, Chip Revision 1.5   |
| Operating Modes Supported                           | Wi-Fi STA, Wi-Fi AP, Wi-Fi STA+BLE, Wi-Fi STA+AP     |
| Additional Operating Modes Supported (Beta Quality) | Wi-Fi STA+BT, Wi-Fi STA+BT+BLE , BLE(With AE)     |

### Notes
  - The latest releases will have bug-fixes, enhancements, and new features in both 'SAPIs' and 'Firmware'. Hence, it is recommended to update and use 'SAPIs' and 'Firmware' of same version.

## Updates in this Release
RS9116W 2.10.1.3 release consists of two components, as follows:
  - Firmware - RS9116 Firmware Binary
  - SAPI Library - SAPI Library runs on Host

This release is meant only for use with designs based on RS9116 Silicon rev 1.4 (RS9116X-xxx-xxx-Bxx) and RS9116 Silicon rev 1.5 (RS9116X-xxx-xxx-Cxx).
Customers using the RS9116X-xxx-xxx-Xxx parts (Silicon rev 1.3) can also upgrade to the latest firmware, however, the power optimization feature macro `EXT_FEAT_LOW_POWER_MODE` in `rsi_wlan_config.h` must be disabled for revision 1.3.


Choose the firmware to match the silicon revision used in your design, as follows:
- RS9116W.2.10.1.0.3.rps
  - RS9116X-xxx-xxx-Xxx (Silicon rev 1.3)
  - RS9116X-xxx-xxx-Bxx (Silicon rev 1.4)
- RS916W.2.10.1.0.3.rps
  - RS9116X-xxx-xxx-Cxx (Silicon rev 1.5)
  - RS916WS00ACxxx

## New Features
### Wi-Fi
  - None

### System
  - None

### BT-Classic/BLE
  - None

### Network Stack
  - None

## Changes and Fixes
### Wi-Fi
  - Fixed the issue of multicast packets being dropped after GTK renewal by properly handling packet numbers post GTK-renewal.
  - Fixed instability issues with the AP start and stop feature.

### Network Stack
  - Added checks to validate the SSL certificate and decline the connection if it fails.
  - The store server credentials command is allowed after the init command is processed, enabling user to save the credentials even before IP is configured on the device.
  - Updated units for MQTT keepalive and socket timeout.
  - Fixed issues with MDNS functionality when the received MDNS message is split into multiple UDP packets.
  - Fixed FTP passive mode error "500 PASV : command not understood".
  - Fixed the issue with setting up FTP mode selection by using rsi_ftp_mode_set API.
  - Fixed instability with FTP connect and disconnect APIs, when FTP connect API fails with errors.
  - The HTTP Server and HTTP Server bypass feature have been removed.
  - Enhanced the stability of the MQTT client application (embedded_mqtt) example by using a random source port during multiple iterations of connection/disconnection.
  - Added the feature FEAT_LONG_ICMP_PACKET to receive long ICMP packets (up to 1472 bytes long).

### BT-Classic/BLE
  - Fixed SD card access issue with bt_a2dp_source_with_avrcp and wlan_https_bt_a2dp demo's.
  - Fixed hang issue when AE ADV enabled and AE Scan start and stop commands are given repeatedly.
  - Fixed firmware timeout issue observed in the dual role when the connect cancel command is sent.
  - Fixed the advertising reports issue with AE scan observed with larger scan interval and scan window.

### Coexistence
  - None

### System
  - Fixed an issue where the GPIO pins were not reset correctly when both the WOW feature and the Auto trigger feature were enabled.
  - Added workaround to recover the device from "Valid firmware not present" state.

### Documentation
  - Added error code for Corrupted RPS encountered during firmware update in SAPI.
  - Added documentation on new API - rsi_set_sni_emb_socket.
  - Updated the setup image for the UART interface with the correct host board.
  - In AT PRM, added detailed documentation of VAP ID usage in AP mode for sockets.
  - Updated information on cipher suites in the SAPI documentation.
  - Updated information on cipher suites in the AT PRM documentation.
  - Updated the at+rsi_httpget URL parameter.
  - Updated the documentation for the rsi_network_app_protocol_config API.
  - Updated SSL/TLS Version Documentation.
  - Updated the at+rsi_trans_mode_param command is only valid in case of UART interface.
  - Updated the reserved_4 parameter value as 2 bytes under the at+rsi_usercfg command.

## Recommendations and New Options

### Wi-Fi
  - Enable aggregation using the opermode parameter [feature_bit_map\[2\]](https://docs.silabs.com/rs9116-wiseconnect/2.6/wifibt-wc-sapi-reference/opermode#rsi-feature-bit-map).
  - To make a connection more robust for low throughput applications in busy networks, disable high MCS rates. For example, in a congested wireless envionment, a Smart Lock may benefit by disabling `MCS6` and `MCS7`.
  - To restart the RS9116, the application should call `rsi_driver_deinit()` followed by `rsi_driver_init()` and `rsi_device_init()`. For OS cases, additionally call `rsi_task_destroy(driver_task_handle)` to delete the driver task before calling `rsi_driver_deinit()` and create again after `rsi_device_init()` using `rsi_task_create()`.

### Network Stack
  - To ensure graceful handling during asynchronous TCP closures from a peer, enable the opermode parameter [ext_tcp_ip_feature_bit_map\[16\]](https://docs.silabs.com/rs9116-wiseconnect/2.6/wifibt-wc-sapi-reference/opermode#rsi-ext-tcpip-feature-bitmap).
  - To avoid TCP disconnects during a rejoin, set TCP retransmission count to >= `30`.
  - Implemented a new API - `rsi_network_app_protocol_config()` to configure the network application protocols such as cipher selection for HTTP protocol.

### BT-Classic/BLE
  - In BLE, the recommended range of the BLEConnection Interval is as follows:
    - Power Save (BLE Only) - 100 ms to 1.28 s
    - BT Classic + BLE Dual Mode is >= 200 ms
    - Wi-Fi + BLE coex - 30 ms to 250 ms
  - In BLE during Connection, the configuration of Scan Interval and Scan Window with the same value is not recommended.
  - In BT Classic, the recommended Sniff Interval configuration during powersave is limited to 100 ms (\<= 100).
  - In BLE, if a device is acting as Central, the scan window (in `set_scan_params()` and `create_connection()` APIs) must be less than the existing Connection Interval.
  - In BLE mode, if scanning and advertising is in progress, and the device subsequently connects and moves to the central role, scanning and advertising stops. To further establish connection to another peripheral device or to a central device, the application must initiate advertising and scanning again.
  - Device powersave must be disabled prior to BT init and de-init.

### Coexistence
  - For concurrent Wi-Fi + BLE, and while a Wi-Fi connection is active, we recommend setting the ratio of the BLE scan window to BLE scan interval to 1:3 or 1:4.
  - Wi-Fi + BLE Advertising
    - All standard advertising intervals are supported. As Wi-Fi throughput is increased, a slight difference in on-air advertisements compared to configured intervals may be observed.
    - BLE advertising is skipped if the advertising interval collides with Wi-Fi activity.
  - Wi-Fi + BLE scanning
    - All standard scan intervals are supported. For better scan results, we recommend setting the ratio of the BLE scan window to BLE scan interval to 1:3 or 1:4.
    - BLE scanning will be stopped for intervals that collide with Wi-Fi activity.
  - Wi-Fi + BLE Central/Peripheral Connections
    - All standard connection intervals are supported.
    - For a stable connection, use optimal connection intervals and max supervision timeout in the presence of Wi-Fi activity.
  - Wi-Fi + BLE Central/Peripheral Data Transfer
    - To achieve higher throughput for both Wi-Fi and BLE, use medium connection intervals, such as 45 to 80 ms with maximum supervision timeout.
    - Ensure Wi-Fi activity consumes lower intervals.
  - For Wi-Fi + BT + BLE, Wi-Fi + BT, Wi-Fi + BLE operating modes, connect Wi-Fi before BT/BLE connections to ensure seamless, stable connection for all three protocols.
  - For Wi-Fi + BT + BLE, Wi-Fi + BT, Wi-Fi + BLE operating modes, if BT/BLE needs to be connected before a Wi-Fi connection, use with high supervision timeout and high connection interval for BLE and high sniff interval for BT, to ensure seamless, stable connection. This configuration also ensures a stable BT/BLE connection when Wi-Fi connects/disconnects/rejoins.

### System
  - For User Store configuration and Configuration Save, do not enable power save or save it as a configuration. If power save is enabled and saved as a configuration, upon boot up, the RS9116 will boot with the saved configuration and will go to powersave without any indication to the host.
  - Set the compiler optimization level to `O0` in project settings for IDE (KEIL).
  - Memory configuration must be 384K for BT/BLE and co-ex operating modes.
    - Usage of low-power flash mode bit (bit 19 in extended customer feature bitmap). Enable this bit for ultra-low-power standby associated scenarios. This results in about 20&micro;A lower Wi-Fi standby associated current consumption.
  - Memory Configuration in SAPI functions: The default memory configuration is 384K.
  - Apply Opermode commands in AT mode correctly. Using the wrong opermode may lead to unspecified behavior.
  - Set the recommended Power Save Profile (PSP) type to Enhanced Max PSP.
  - During firmware updates, powersave operation should be disabled.
  - For high throughput applications, powersave operation should be disabled.
  - The application must set the real-time clock with correct timestamp when the feature is enabled before establishing a TLS connection.
  - The socket select and socket receive timeout must be at least 1 second. A timeout value under 1 second is not supported.

## Known Issues

### Network Stack
  - Reassembly of fragmented MQTT packets is not supported in embedded MQTT. MQTT transactions may fail with fragmented packets received from the broker.
  - Sometimes during a TLS handshake, ECC curve parameters may be incorrectly generated resulting in connection failure with BBD2 error. However, this recovers in the next attempt.
  - MQTT disconnects during firmware update when power save is enabled. Disable power save during firmware update and re-enable on update completion.
  - All GCM-based cipher suites are implemented in software and have limited performance.
  - The recommended MQTT publish payload is 1 kBytes.
  - If HTTP server functionality is enabled, do not use port 80 for the MQTT client.
  - Randomize the client port if using rapid connect/disconnect of the MQTT session on the same client port with powersave.
  - Secure TLS renegotiation is not supported in the embedded networking stack.
  - In WPA2 enterprise security mode, getting wireless statistics using rsi_wlan_get() API has restrictions.
  - Example application wlan_throughtput_bt_spp_ble_dual_role is unsupported with bare metal.
  - In concurrent mode, there could be a loss of communication on the STA application when, simultaneously, multiple clients are connected to the APUT.
  - IP address conflict detection has limitations in STA + AP concurrent mode, failure is not reported to host.
  - mqtt_client and mqtt_client_v6 applications do not support SDIO host interface.
  - During firmware update via OTA (TCP/HTTP) methods, if the firmware image received is corrupted, it can result in the module going unresponsive on the next bootup.
  - TLS connection problems may be observed when attempting to revoke public and private keys using existing keys.
  - Observed connection failure (error_code 0x0008) with WPA2-Enterprise Security with pathlength enabled certificate.
  - Observed STA+BT(Connected) test cases not getting throughput results prints in teraterm.
  - Observed inconsistency in TCP & UDP - Rx throughputs in AP alone mode.
  - Observed duplicate IPv6 address is assigned to multiple STAs in AP mode.

### Wi-Fi
  - WPA3 Connection with Hunting and Pecking algorithm takes about 3-4 seconds.
  - If the station performs a scan in concurrent mode (Wi-Fi STA+AP), stations connected to the AP may be disconnected. Enable AP after the STA connection is completed.
  - Issue observed with WPA2 Enterprise Connectivity using Microsoft RADIUS Server.
  - PTA 3-wire coexistence does not work with power save. However, if a weak pull up is applied to the GRANT pin, PTA 3-wire functionality will work with power save. Details can be found in the application notes.
  - EAP security in the aws_device_shadow application is unsupported on the EFR platform due to memory constraints.
  - In STA+AP, If AP started first station scan is supported only in channel in which AP is brought up. If multiple channel scan is requested, AP behaviour is undefined.
  - Auto channel selection with EXT_FEAT_IEEE_80211J feature enabled is not supported in Access point operating mode.
  - Testing for traffic differentiation within a single BSS with 802.11n stations is yielding failures during the 11n certification process.

### BT-Classic/BLE
  - Wake on wireless is not supported for BLE.
  - BT-HID may not interoperate with Apple devices.
  - In Wi-Fi + BT/BLE Co-Ex mode, high Wi-Fi broadcast traffic might cause BT/BLE disconnections.
  - In Wi-Fi + BT mode when there is continuous Wi-Fi data, you might observe BT not re-connecting to the remote after disconnection.
  - For BLE, dual role scenario issues may occur when connecting as a central when the peripheral is advertising. Central connections can be made after peripheral connections are established and advertising stops.
  - A BLE disconnection will occur if both the peer and RS9116 already have bond info and the RS9116 initiates security as part of BLE secure connections. To workaround this issue, the application should not initiate a security request if it already has bond info.
  - A2DP music streaming glitches may be observed if a Wi-Fi download is in progress at the same time.
  - RTOS support is not there in ae_peripheral and ae_central applications.
  - AE connection issue is expected with Samsung mobiles.
  - BT inquiry will not work in temperatures ranging from -40 C to 0 C.

### Coexistence
  - After power save is enabled in co-ex mode, `radio_init()` must be called to turn off power save.
  - In Wi-Fi + BLE mode:
    - BLE may disconnect when the supervision timeout is configured to be less than 16 seconds.
    - WPS does not work.
  - In Wi-Fi + BLE / BLE only modes:
    - When Wi-Fi disconnects, a BT/BLE reconnection issue is observed.
  - In Wi-Fi + BT mode:
    - When BT SPP is connected, WLAN throughput prints are not displayed in Teraterm with wlan_bt_throughput_app demo.
  - The wlan_https_bt_a2dp_ble_dual_role and wlan_https_bt_a2dp demos are not supported.

### Interoperability
  - Disconnection may occur in Wi-Fi client mode if an AP does not acknowledge QoS null frames with power save configured.

### System
  - Chip Revision 1.3 doesn't support low-power mode optimization using the bit setting defined by EXT_FEAT_LOW_POWER_MODE; setting this results in a hang issue.
  - Sometimes the RS9116 enters power save mode before the configured monitor interval for inactivity when co-ex is configured.
  - The FTP client application does not work with EFx32/STM32 when an RTOS is used.
  - The HTTP OTA update application does not work with EFR32 and EFM32 when an RTOS is used.
  - Firmware update does not work for the EFM32 platform.
  - SAPI driver with UART host interface is supported only on bare metal.
  - Powersave without RAM retention does not work when the SPI interface is used.
  - MQTT Receive may encounter failures in STA + AP concurrent mode.
  - Standby associated listen interval current consumption is higher than expected with EFM Platform.

## Limitations and Unsupported Features

### Wi-Fi/Network Stack
  - AMSDU transmit is not supported.
  - UAPSD is not supported.
  - Fragmentation is not supported.
  - AMSDU's within AMPDU is not supported.
  - Currently, the RS9116 does not support the radio measurement requests feature of CCX V2.
  - 802.11k is not supported.
  - Short GI is not supported.
  - 40 MHz bandwidth for both the 2.4 and 5 GHz bands is not supported.
  - 802.11j channels less than 16 are not supported.
  - The USB host interface and USB power save are not supported.
  - In AT mode, the total MQTT command length (apart from MQTT publish) should not exceed `150` bytes. This includes `at+rsi` (start of command) to (end of command)`\r\n`.
  - Configure `MQTT_VERSION` in `rsi_mqtt_client.h` based on the server configuration; only version 3 and 4 are supported.
  - 3xTLS connections or 1xTCP and 2xTLS connections are supported concurrently in Wi-Fi only mode.
  - TLS curve IDs supported are 15-28. TLS handshake with 3rd party clients depends on the TLS curve.
  - Big endian architecture is not supported.
  - Auto PAC Provisioning in EAP-FAST with TLSv1.2 is not supported.
  - RC4_SHA, DES_CBC3_SHA, and RC4_MD5 ciphers are not supported in enterprise security.
  - SA query procedure not supported in 11W AP mode.
  - If background scan is enabled along with custom feature bit map bit (8) configuration of the DFS channel support, issues may occur in obtaining an IP address for the Wi-Fi STA.
  - In AP mode, IPv6 Router Advertisements are not supported. Although STA connected to the APUT can transfer IPv6 data to the peers by using SLAAC for address configuration.
  - Transparent mode is unsupported in USB-CDC mode.
  - Channel 144 is not supported.
  - IOP issues may be observed with device running DHCPv6 server with Android devices, as latter do not support DHCPv6 process for IPv6 address configuration.
  - WPA3 AP mode is supported with alpha-level quality.
  - WPA3 AP supports only H2E algorithm.
  - WPA3 AP PMKSA is not supported.
  - WPA3 AP transition mode is not supported.
  - AP standalone mode does not support Tx aggregation. Rx aggregation is supported with limited number of BA sessions.
  - In concurrent mode - AP mode, aggregation (Tx/Rx) is not supported.

### BT-Classic/BLE
  - BT A2DP is only supported for RS9116 silicon revision 1.5.
  - BT sniff mode does not work if BT multiple slaves feature is enabled.
  - For BLE, if the connection is established with a small connection interval (<15 ms), simultaneous roles are not supported (i.e., Central/Peripheral + Advertising/Scanning).
  - For BT Classic, only one connection is supported at any time.
  - BLE slave latency value is valid up to 32 only.
  - BT-A2DP encoder is not supported in the firmware.
  - BT-A2DP is not supported in AT mode.
  - Maximum supported  AE data length is 200 bytes.
  - Supports only two ADV_EXT sets.
  - Supports only one BLE connection with AE.
  - SMP is not supported in AE applications "ble_ae_central" and "ble_ae_peripheral".
  - GATT transactions and Data transfer are not supported in AE applications  "ble_ae_central" and "ble_ae_peripheral".
  - BLE duty cycling is not supported.
  - User_config method is not supported for BLE.

### Coexistence
  - Wi-Fi STA + BT + BLE multiprotocol use cases require a detailed understanding of use cases and associated configurations. Contact Silicon Labs support for additional details.
  - Wi-Fi AP + BLE, Wi-Fi AP + BT & Wi-Fi AP + BT + BLE modes are not supported.
  - For AT commands, Wi-Fi+BT+BLE( Opermode 9 ), BT+BLE(Opermode 8 ) do not work.
  - In Wi-Fi + BLE mode, if the BLE scan interval and window have the same value, then the Wi-Fi connection may be unsuccessful.
  - Advertising Extention feature is not supported in Coexistence.

### System
  - Wake on Wireless support has only been tested for the UART AT command interface.
  - Firmware update via bootloader method requires the user to check the integrity of the RPS file content. A corrupted image may cause the chip to go unresponsive to host commands.

## Notes
### BT-Classic/BLE
 - Acceptlist address should be given in reverse order, i.e., in big endian format.
 - AE feature validated using rev 1.5 AC1 expansion board.
 - AE feature is not supported in 9116 rev 1.4.
 - Advertising Extentions is Beta quality feature.

# WiSeConnect 2.10 Release Notes

Last updated: June 10, 2024

## Highlights

  - New features
  - Bug fixes

## Release Details
| Item                                                | Details                                |
|-----------------------------------------------------|----------------------------------------|
| Release date                                        | June 14, 2024                          |
| API Version (SAPI)                                  | 2.10.0.6 (Build 6)                     |
| Firmware Version                                    | 2.10.0.0.6 (Build 6)                   |
| Package Name                                        | RS9116W.2.10.0.6                       |
| Supported RTOS                                      | FreeRTOS                               |
| Hardware Modules                                    | QMS, B00, C00, CC1, AB0, AB1, AA0, AA1, AC0, AC1 |
| Hardware Chipsets                                   | Chip Revision 1.3, Chip Revision 1.4, Chip Revision 1.5   |
| Operating Modes Supported                           | Wi-Fi STA, Wi-Fi AP, Wi-Fi STA+BLE, Wi-Fi STA+AP     |
| Additional Operating Modes Supported (Beta Quality) | Wi-Fi STA+BT, Wi-Fi STA+BT+BLE , BLE(With AE)     |

### Notes
  - The latest releases will have bug-fixes, enhancements, and new features in both 'SAPIs' and 'Firmware'. Hence, it is recommended to update and use 'SAPIs' and 'Firmware' of same version.

## Updates in this Release
RS9116W 2.10.0.6 release consists of two components, as follows:
  - Firmware - RS9116 Firmware Binary
  - SAPI Library - SAPI Library runs on Host

This release is meant only for use with designs based on RS9116 Silicon rev 1.4 (RS9116X-xxx-xxx-Bxx) and RS9116 Silicon rev 1.5 (RS9116X-xxx-xxx-Cxx).
Customers using the RS9116X-xxx-xxx-Xxx parts (Silicon rev 1.3) can also upgrade to the latest firmware, however, the power optimization feature macro `EXT_FEAT_LOW_POWER_MODE` in `rsi_wlan_config.h` must be disabled for revision 1.3.


Choose the firmware to match the silicon revision used in your design, as follows:
- RS9116W.2.10.0.0.6.rps
  - RS9116X-xxx-xxx-Xxx (Silicon rev 1.3)
  - RS9116X-xxx-xxx-Bxx (Silicon rev 1.4)
- RS916W.2.10.0.0.6.rps
  - RS9116X-xxx-xxx-Cxx (Silicon rev 1.5)
  - RS916WS00ACxxx

## New Features
### Wi-Fi
  - Added support for UNII-3 band in Europe region for RS9116 silicon revision 1.4 module (M7DB)

### System
  - Added the timeout handler in ulp_wakeup_init to exit harware failure gracefully.
  - Added support for UART host interface for EFR32 platform in sapi. http_client_v6 is updated as a reference example application to support UART host interface.

### BT-Classic/BLE
  - None

### Network Stack
  - None

## Changes and Fixes
### Wi-Fi
  - Renamed the variable from psk to pmk in rsi_wlan_info
  - In concurrent mode STA-RX aggregation is supported.

### Network Stack
  - Resolved issues with IPv6 address validation for store server credentials command in AT mode.
  - Added backward compatibility support for embedded MQTT feature in current SDK release. The SDK version 2.10.x or older is compatible with firmware version 2.9.x and 2.8.x series for this feature.

### BT-Classic/BLE
  - AT+RSIBT_readresp command and AT+RSIBT_sendindicate command can be used to send data up to MTU size.
  - Fixed BLE 2Mbps transmission issue with HP chain.
  - Address the L2CAP reassembling of fragmented packets received from the remote device when the continuous packet length is more than the start packet length on the BTC Link.

### Coexistence
  - None

### System
  - Added a fix in sapi to recover the bootloader from a stuck-up with invalid firmware status.
  - Fixed inconsistencies with AP stop request and response frame types.

### Documentation
  - Fixed the syntax error observed in the release note of 2.8.1.
  - Added notes for UNII-3 band support in AT and SAPI PRMs
  - Corrected documentation for the maximum number of available TLS sockets and the number of available TCP and UDP sockets when TLS is enabled.
  - Enhanced documentation for SSL_NEW_CIPHERS for ciphers selection.

## Recommendations and New Options

### Wi-Fi
  - Enable aggregation using the opermode parameter [feature_bit_map\[2\]](https://docs.silabs.com/rs9116-wiseconnect/2.6/wifibt-wc-sapi-reference/opermode#rsi-feature-bit-map)
  - To make a connection more robust for low throughput applications in busy networks, disable high MCS rates. For example, in a congested wireless envionment, a Smart Lock may benefit by disabling `MCS6` and `MCS7`.
  - To restart the RS9116, the application should call `rsi_driver_deinit()` followed by `rsi_driver_init()` and `rsi_device_init()`. For OS cases, additionally call `rsi_task_destroy(driver_task_handle)` to delete the driver task before calling `rsi_driver_deinit()` and create again after `rsi_device_init()` using `rsi_task_create()`.

### Network Stack
  - To ensure graceful handling during asynchronous TCP closures from a peer, enable the opermode parameter [ext_tcp_ip_feature_bit_map\[16\]](https://docs.silabs.com/rs9116-wiseconnect/2.6/wifibt-wc-sapi-reference/opermode#rsi-ext-tcpip-feature-bitmap)
  - To avoid TCP disconnects during a rejoin, set TCP retransmission count to >= `30`

### BT-Classic/BLE
  - In BLE, the recommended range of the BLEConnection Interval is as follows:
    - Power Save (BLE Only) - 100 ms to 1.28 s
    - BT Classic + BLE Dual Mode is >= 200 ms
    - Wi-Fi + BLE coex - 30 ms to 250 ms
  - In BLE during Connection, the configuration of Scan Interval and Scan Window with the same value is not recommended
  - In BT Classic, the recommended Sniff Interval configuration during powersave is limited to 100 ms (\<= 100).
  - In BLE, if a device is acting as Central, the scan window (in `set_scan_params()` and `create_connection()` APIs) must be less than the existing Connection Interval.
  - In BLE mode, if scanning and advertising is in progress, and the device subsequently connects and moves to the central role, scanning and advertising stops. To further establish connection to another peripheral device or to a central device, the application must initiate advertising and scanning again.
  - Device powersave must be disabled prior to BT init and de-init.

### Coexistence
  - For concurrent Wi-Fi + BLE, and while a Wi-Fi connection is active, we recommend setting the ratio of the BLE scan window to BLE scan interval to 1:3 or 1:4.
  - Wi-Fi + BLE Advertising
    - All standard advertising intervals are supported. As Wi-Fi throughput is increased, a slight difference in on-air advertisements compared to configured intervals may be observed.
    - BLE advertising is skipped if the advertising interval collides with Wi-Fi activity.
  - Wi-Fi + BLE scanning
    - All standard scan intervals are supported. For better scan results, we recommend setting the ratio of the BLE scan window to BLE scan interval to 1:3 or 1:4.
    - BLE scanning will be stopped for intervals that collide with Wi-Fi activity.
  - Wi-Fi + BLE Central/Peripheral Connections
    - All standard connection intervals are supported.
    - For a stable connection, use optimal connection intervals and max supervision timeout in the presence of Wi-Fi activity.
  - Wi-Fi + BLE Central/Peripheral Data Transfer
    - To achieve higher throughput for both Wi-Fi and BLE, use medium connection intervals, such as 45 to 80 ms with maximum supervision timeout.
    - Ensure Wi-Fi activity consumes lower intervals.
  - For Wi-Fi + BT + BLE, Wi-Fi + BT, Wi-Fi + BLE operating modes, connect Wi-Fi before BT/BLE connections to ensure seamless, stable connection for all 3 protocols.
  - For Wi-Fi + BT + BLE, Wi-Fi + BT, Wi-Fi + BLE operating modes, if BT/BLE needs to be connected before a Wi-Fi connection, use with high supervision timeout and high connection interval for BLE and high sniff interval for BT, to ensure seamless, stable connection. This configuration also ensures a stable BT/BLE connection when Wi-Fi connects/disconnects/rejoins.

### System
  - For User Store configuration and Configuration Save, do not enable power save or save it as a configuration. If power save is enabled and saved as a configuration, upon boot up, the RS9116 will boot with the saved configuration and will go to powersave without any indication to the host.
  - Set the compiler optimization level to `O0` in project settings for IDE (KEIL).
  - Memory configuration must be 384K for BT/BLE and co-ex operating modes
    - Usage of low power flash mode bit (bit 19 in extended customer feature bitmap). Enable this bit for ultra low power standby associated scenarios. This results in about 20&micro;A lower Wi-Fi standby associated current consumption.
  - Memory Configuration in SAPI functions: The default memory configuration is 384K.
  - Apply Opermode commands in AT mode correctly. Using the wrong opermode may lead to unspecified behavior.
  - Set the recommended Power Save Profile (PSP) type to Enhanced Max PSP.
  - During firmware updates, powersave operation should be disabled.
  - For high throughput applications, powersave operation should be disabled.
  - The application must set the real-time clock with correct timestamp when the feature is enabled before establishing a TLS connection.
  - The socket select and socket receive timeout must be at least 1 second. A timeout value under 1 second is not supported.

## Known Issues

### Network Stack
  - Reassembly of fragmented MQTT packets is not supported in embedded MQTT. MQTT transactions may fail with fragemented packets received from the broker.
  - Sometimes during a TLS handshake, ECC curve parameters may be incorrectly generated resulting in connection failure with BBD2 error. However, this recovers in the next attempt.
  - MQTT disconnects during firmware update when power save is enabled. Disable power save during firmware update and re-enable on update completion.
  - All GCM-based cipher suites are implemented in software and have limited performance.
  - The recommended MQTT publish payload is 1 kBytes.
  - If HTTP server functionality is enabled, do not use port 80 for the MQTT client.
  - Randomize the client port if using rapid connect/disconnect of the MQTT session on the same client port with powersave.
  - Secure TLS renegotiation is not supported in the embedded networking stack.
  - In WPA2 enterprise security mode getting wireless statistics using rsi_wlan_get() API has restrictions.
  - Example application wlan_throughtput_bt_spp_ble_dual_role is unsupported with bare metal.
  - In concurrent mode, there could be a loss of communication on the STA application, when simultaneously multiple clients are connected to the APUT.
  - IP address conflict detection has limitations in STA + AP concurrent mode, failure is not reported to host.
  - mqtt_client and mqtt_client_v6 applications don't support SDIO host interface
  - During firmware update via OTA (TCP/HTTP) methods, if the firmware image received is corrupted it can result in module going unresponsive on the next bootup.
  - TLS connection problems may be observed when attempting to revoke public and private keys using existing keys.

### Wi-Fi
  - WPA3 Connection with Hunting and Pecking algorithm takes about 3-4 seconds.
  - If the station performs a scan in concurrent mode (Wi-Fi STA+AP), stations connected to the AP may be disconnected. Enable AP after the STA connection is completed.
  - Issue observed with WPA2 Enterprise Connectivity using Microsoft RADIUS Server.
  - PTA 3-wire coexistence does not work with power save. However, if a weak pull up is applied to the GRANT pin, PTA 3-wire functionality will work with power save. Details can be found in the application notes.
  - EAP security in the aws_device_shadow application is unsupported on the EFR platform due to memory constraints.
  - In STA+AP, If AP started first station scan is supported only in channel in which AP is brought up. If multiple channel scan is requested, AP behaviour is undefined.
  - Auto channel selection with EXT_FEAT_IEEE_80211J feature enabled is not supported in Access point operating mode.
  - Testing for traffic differentiation within a single BSS with 802.11n stations is yielding failures during the 11n certification process.

### BT-Classic/BLE
  - Wake on wireless is not supported for BLE.
  - BT-HID may not interoperate with Apple devices.
  - In Wi-Fi + BT/BLE Co-Ex mode, high Wi-Fi broadcast traffic might cause BT/BLE disconnections.
  - In Wi-Fi + BT mode when there is continuous Wi-Fi data, you might observe BT not re-connecting to the remote after disconnection.
  - For BLE, dual role scenario issues may occur when connecting as a central when the peripheral is advertising. Central connections can be made after peripheral connections are established and advertising stops.
  - A BLE disconnection will occur if both the peer and RS9116 already have bond info and the RS9116 initiates security as part of BLE secure connections. To workaround this issue, the application should not initiate a security request if it already has bond info.
  - A2DP music streaming glitches may be observed if a Wi-Fi download is in progress at the same time.
  - RTOS support is not there in ae_peripheral & ae_central applications.
  - AE connection issue is expected with Samsung mobiles.
  - BT inquiry will not work in temperatures ranging from -40C to 0C.
  - The bt_a2dp_source_with_avrcp demo is not supported.

### Coexistence
  - After power save is enabled in co-ex mode, `radio_init()` must be called to turn off power save.
  - In Wi-Fi + BLE mode:
    - BLE may disconnect when the supervision timeout is configured to be less than 16 seconds.
    - WPS does not work.
  - In Wi-Fi + BLE / BLE only modes:
    - When Wi-Fi disconnects, a BT/BLE reconnection issue is observed.
  - In Wi-Fi + BT mode:
    - When BT SPP is connected, WLAN throughput prints are not displayed in Teraterm with wlan_bt_throughput_app demo
  - The wlan_https_bt_a2dp_ble_dual_role and wlan_https_bt_a2dp demos are not supported.

### Interoperability
  - Disconnection may occur in Wi-Fi client mode if an AP does not acknowledge QoS null frames with power save configured.

### System
  - Chip Revision 1.3 doesn't support Low power mode optimization using the bit setting defined by EXT_FEAT_LOW_POWER_MODE, setting this results in a hang issue.
  - Sometimes the RS9116 enters power save mode before the configured monitor interval for inactivity when co-ex is configured.
  - The FTP client application does not work with EFx32/STM32 when an RTOS is used.
  - The HTTP OTA update application does not work with EFR32 and EFM32 when an RTOS is used.
  - Firmware update does not work for the EFM32 platform.
  - SAPI driver with UART host interface is supported only on bare metal.
  - Powersave without RAM retention does not work when the SPI interface is used.
  - MQTT Receive may encounter failures in STA + AP concurrent mode.
  - Standby associated listen interval current consumption is higher than expected with EFM Platform.

## Limitations and Unsupported Features

### Wi-Fi/Network Stack
  - AMSDU transmit is not supported.
  - Fragmentation is not supported.
  - AMSDU's within AMPDU is not supported.
  - Currently, the RS9116 does not support the radio measurement requests feature of CCX V2
  - 802.11k is not supported
  - Short GI is not supported
  - 40 MHz bandwidth for both the 2.4 & 5 GHz bands is not supported.
  - 802.11j channels less than 16 are not supported.
  - The USB host interface and USB power save are not supported.
  - In AT mode, the total MQTT command length (apart from MQTT publish) should not exceed `150` bytes. This includes `at+rsi` (start of command) to (end of command)`\r\n`.
  - Configure `MQTT_VERSION` in `rsi_mqtt_client.h` based on the server configuration; only version 3 and 4 are supported.
  - 3xTLS connections or 1xTCP and 2xTLS connection, connections are supported concurrently in Wi-Fi only mode.
  - TLS curve IDs supported are 15-28. TLS handshake with 3rd party clients depends on the TLS curve.
  - Big endian architecture is not supported.
  - Auto PAC Provisioning in EAP-FAST with TLSv1.2 is not supported.
  - RC4_SHA, DES_CBC3_SHA, and RC4_MD5 ciphers are not supported in enterprise security
  - SA query procedure not supported in 11W AP mode.
  - If background scan is enabled along with custom feature bit map bit (8) configuration of the DFS channel support, issues may occur in obtaining an IP address for the Wi-Fi STA.
  - In AP mode, IPv6 Router Advertisements are not supported. Although STA connected to the APUT can transfer IPv6 data to the peers by using SLAAC for address configuration.
  - Transparent mode is unsupported in USB-CDC mode.
  - Channel 144 is not supported.
  - IOP issues may be observed with device running DHCPv6 server with Android devices, as latter don't support DHCPv6 process for IPv6 address configuration.
  - WPA3 AP mode is supported with alpha-level quality
  - WPA3 AP supports only H2E algorithm.
  - WPA3 AP PMKSA is not supported.
  - WPA3 AP transition mode is not supported.
  - AP standalone mode does not support Tx aggregation. Rx aggregation is supported with limited number of BA sessions.
  - In concurrent mode - AP mode, aggregation (Tx/Rx) is not supported.

### BT-Classic/BLE
  - BT A2DP is only supported for RS9116 silicon revision 1.5.
  - BT sniff mode does not work if BT multiple slaves feature is enabled.
  - For BLE, if the connection is established with a small connection interval (<15 ms), simultaneous roles are not supported (i.e., Central/Peripheral + Advertising/Scanning).
  - For BT Classic, only 1 connection is supported at any time.
  - BLE slave latency value is valid up to 32 only.
  - BT-A2DP encoder is not supported in the firmware.
  - BT-A2DP is not supported in AT mode.
  - Maximum supported  AE data length is 200 bytes.
  - Supports only two ADV_EXT sets.
  - Supports only one BLE connection with AE.
  - SMP is not supported in AE applications "ble_ae_central" and "ble_ae_peripheral".
  - GATT transactions & Data transfer are not supported in AE applications  "ble_ae_central" and "ble_ae_peripheral".
  - BLE duty cycling is not supported.
  - User_config method is not supported for BLE.

### Coexistence
  - Wi-Fi STA + BT + BLE multiprotocol use cases require a detailed understanding of use cases and associated configurations. Contact Silicon Labs support for additional details.
  - Wi-Fi AP + BLE, Wi-Fi AP + BT & Wi-Fi AP + BT + BLE modes are not supported.
  - For AT commands, Wi-Fi+BT+BLE( Opermode 9 ), BT+BLE(Opermode 8 ) do not work.
  - In Wi-Fi + BLE mode, if the BLE scan interval and window have the same value, then the Wi-Fi connection may be unsuccessful.
  - Advertising Extention feature is not supported in Coexistence.

### System
  - Wake on Wireless support has only been tested for the UART AT command interface.
  - Firmware update via bootloader method requires the user to check the integrity of the RPS file content. A corrupted image may cause the chip to go unresponsive to host commands.

## Notes
### BT-Classic/BLE
 - Acceptlist address should be given in reverse order, i.e in big endian format.
 - AE feature validated using rev 1.5 AC1 expansion board.
 - AE feature is not supported in 9116 rev 1.4.
 - Advertising Extentions is Beta quality feature.

# WiSeConnect 2.9.2 Release Notes

Last updated: April 23, 2024

## Highlights

  - Bug fixes

## Release Details
| Item                                                | Details                                |
|-----------------------------------------------------|----------------------------------------|
| Release date                                        | April 30, 2024                         |
| API Version (SAPI)                                  | 2.9.2.12 (Build 12)                    |
| Firmware Version                                    | 2.9.2.0.12 (Build 12)                  |
| Package Name                                        | RS9116W.2.9.2.12                       |
| Supported RTOS                                      | FreeRTOS                               |
| Hardware Modules                                    | QMS, B00, C00, CC1, AB0, AB1, AA0, AA1, AC0, AC1 |
| Hardware Chipsets                                   | Chip Revision 1.3, Chip Revision 1.4, Chip Revision 1.5   |
| Operating Modes Supported                           | Wi-Fi STA, Wi-Fi AP, Wi-Fi STA+BLE, Wi-Fi STA+AP     |
| Additional Operating Modes Supported (Beta Quality) | Wi-Fi STA+BT, Wi-Fi STA+BT+BLE , BLE(With AE)     |

## Updates in this Release
RS9116W 2.9.2.12 release consists of two components, as follows:
  - Firmware - RS9116 Firmware Binary
  - SAPI Library - SAPI Library runs on Host

This release is meant only for use with designs based on RS9116 Silicon rev 1.4 (RS9116X-xxx-xxx-Bxx) and RS9116 Silicon rev 1.5 (RS9116X-xxx-xxx-Cxx).
Customers using the RS9116X-xxx-xxx-Xxx parts (Silicon rev 1.3) can also upgrade to the latest firmware, however, the power optimization feature macro `EXT_FEAT_LOW_POWER_MODE` in `rsi_wlan_config.h` must be disabled for revision 1.3.


Choose the firmware to match the silicon revision used in your design, as follows:
- RS9116W.2.9.2.0.12.rps
  - RS9116X-xxx-xxx-Xxx (Silicon rev 1.3)
  - RS9116X-xxx-xxx-Bxx (Silicon rev 1.4)
- RS916W.2.9.2.0.12.rps
  - RS9116X-xxx-xxx-Cxx (Silicon rev 1.5)
  - RS916WS00ACxxx

## New Features
### Wi-Fi
  - None

### System
  - None

### BT-Classic/BLE
  - Added Auto join feature for BLE using CFG save command.

### Network Stack
  - None

## Changes and Fixes
### Wi-Fi
  - Addressed the issue of module not entering into deep sleep after AP stop API call.
  - Added a comma delimeter for each byte in the user gain table payload for user understanding.
  - Added support to increase the maximum EAP CA certificate size to 12K and the maximum combined EAP certificate size to 20K.

### Network Stack
  - Fixed compilation warnings in the SDK.
  - Enhanced SDK to avoid function names and keyword conflicts with the standard library.
  - Resolved IOP issue with a few HTTP servers that send an additional two bytes of \r\n at the end of data overshooting the content length specified.
  - Resolved instability issues with the HTTP PUT method when called multiple times in a loop.
  - Resolved instability issues with access_point_v6 example application.

### BT-Classic/BLE
  - Addressed the L2CAP reassembly issue for fragmented packets received from remote devices on the BTC link.

### Coexistence
  - None

### System
  - Addressed the issue, the module is not going into the sleep, added a macro, and updated the wake and sleep indicator pins.

### Documentation
  - Corrected documentation for the return values of rsi_send_large_data_sync() API.

## Recommendations and New Options

### Wi-Fi
  - Enable aggregation using the opermode parameter [feature_bit_map\[2\]](https://docs.silabs.com/rs9116-wiseconnect/2.6/wifibt-wc-sapi-reference/opermode#rsi-feature-bit-map)
  - To make a connection more robust for low throughput applications in busy networks, disable high MCS rates. For example, in a congested wireless envionment, a Smart Lock may benefit by disabling `MCS6` and `MCS7`.
  - To restart the RS9116, the application should call `rsi_driver_deinit()` followed by `rsi_driver_init()` and `rsi_device_init()`. For OS cases, additionally call `rsi_task_destroy(driver_task_handle)` to delete the driver task before calling `rsi_driver_deinit()` and create again after `rsi_device_init()` using `rsi_task_create()`.

### Network Stack
  - To ensure graceful handling during asynchronous TCP closures from a peer, enable the opermode parameter [ext_tcp_ip_feature_bit_map\[16\]](https://docs.silabs.com/rs9116-wiseconnect/2.6/wifibt-wc-sapi-reference/opermode#rsi-ext-tcpip-feature-bitmap)
  - To avoid TCP disconnects during a rejoin, set TCP retransmission count to >= `30`

### BT-Classic/BLE
  - In BLE, the recommended range of the BLEConnection Interval is as follows:
    - Power Save (BLE Only) - 100 ms to 1.28 s
    - BT Classic + BLE Dual Mode is >= 200 ms
    - Wi-Fi + BLE coex - 30 ms to 250 ms
  - In BLE during Connection, the configuration of Scan Interval and Scan Window with the same value is not recommended
  - In BT Classic, the recommended Sniff Interval configuration during powersave is limited to 100 ms (\<= 100).
  - In BLE, if a device is acting as Central, the scan window (in `set_scan_params()` and `create_connection()` APIs) must be less than the existing Connection Interval.
  - In BLE mode, if scanning and advertising is in progress, and the device subsequently connects and moves to the central role, scanning and advertising stops. To further establish connection to another peripheral device or to a central device, the application must initiate advertising and scanning again.
  - Device powersave must be disabled prior to BT init and de-init.

### Coexistence
  - For concurrent Wi-Fi + BLE, and while a Wi-Fi connection is active, we recommend setting the ratio of the BLE scan window to BLE scan interval to 1:3 or 1:4.
  - Wi-Fi + BLE Advertising
    - All standard advertising intervals are supported. As Wi-Fi throughput is increased, a slight difference in on-air advertisements compared to configured intervals may be observed.
    - BLE advertising is skipped if the advertising interval collides with Wi-Fi activity.
  - Wi-Fi + BLE scanning
    - All standard scan intervals are supported. For better scan results, we recommend setting the ratio of the BLE scan window to BLE scan interval to 1:3 or 1:4.
    - BLE scanning will be stopped for intervals that collide with Wi-Fi activity.
  - Wi-Fi + BLE Central/Peripheral Connections
    - All standard connection intervals are supported.
    - For a stable connection, use optimal connection intervals and max supervision timeout in the presence of Wi-Fi activity.
  - Wi-Fi + BLE Central/Peripheral Data Transfer
    - To achieve higher throughput for both Wi-Fi and BLE, use medium connection intervals, such as 45 to 80 ms with maximum supervision timeout.
    - Ensure Wi-Fi activity consumes lower intervals.
  - For Wi-Fi + BT + BLE, Wi-Fi + BT, Wi-Fi + BLE operating modes, connect Wi-Fi before BT/BLE connections to ensure seamless, stable connection for all 3 protocols.
  - For Wi-Fi + BT + BLE, Wi-Fi + BT, Wi-Fi + BLE operating modes, if BT/BLE needs to be connected before a Wi-Fi connection, use with high supervision timeout and high connection interval for BLE and high sniff interval for BT, to ensure seamless, stable connection. This configuration also ensures a stable BT/BLE connection when Wi-Fi connects/disconnects/rejoins.

### System
  - For User Store configuration and Configuration Save, do not enable power save or save it as a configuration. If power save is enabled and saved as a configuration, upon boot up, the RS9116 will boot with the saved configuration and will go to powersave without any indication to the host.
  - Set the compiler optimization level to `O0` in project settings for IDE (KEIL).
  - Memory configuration must be 384K for BT/BLE and co-ex operating modes
    - Usage of low power flash mode bit (bit 19 in extended customer feature bitmap). Enable this bit for ultra low power standby associated scenarios. This results in about 20&micro;A lower Wi-Fi standby associated current consumption.
  - Memory Configuration in SAPI functions: The default memory configuration is 384K.
  - Apply Opermode commands in AT mode correctly. Using the wrong opermode may lead to unspecified behavior.
  - Set the recommended Power Save Profile (PSP) type to Enhanced Max PSP.
  - During firmware updates, powersave operation should be disabled.
  - For high throughput applications, powersave operation should be disabled.
  - The application must set the real-time clock with correct timestamp when the feature is enabled before establishing a TLS connection.
  - The socket select and socket receive timeout must be at least 1 second. A timeout value under 1 second is not supported.

## Known Issues

### Network Stack
  - Reassembly of fragmented MQTT packets is not supported in embedded MQTT. MQTT transactions may fail with fragemented packets received from the broker.
  - Sometimes during a TLS handshake, ECC curve parameters may be incorrectly generated resulting in connection failure with BBD2 error. However, this recovers in the next attempt.
  - MQTT disconnects during firmware update when power save is enabled. Disable power save during firmware update and re-enable on update completion.
  - All GCM-based cipher suites are implemented in software and have limited performance.
  - The recommended MQTT publish payload is 1 kBytes.
  - If HTTP server functionality is enabled, do not use port 80 for the MQTT client.
  - Randomize the client port if using rapid connect/disconnect of the MQTT session on the same client port with powersave.
  - Secure TLS renegotiation is not supported in the embedded networking stack.
  - In WPA2 enterprise security mode getting wireless statistics using rsi_wlan_get() API has restrictions.
  - Example application wlan_throughtput_bt_spp_ble_dual_role is unsupported with bare metal.
  - In concurrent mode, there could be a loss of communication on the STA application, when simultaneously multiple clients are connected to the APUT.
  - IP address conflict detection has limitations in STA + AP concurrent mode, failure is not reported to host.
  - There may be instability with the MQTT connection when MQTT Keep alive period is configured as 0.
  - mqtt_client and mqtt_client_v6 applications don't support SDIO host interface
  - During firmware update via OTA (TCP/HTTP) methods, if the firmware image received is corrupted it can result in module going unresponsive on the next bootup.
  - Embedded MQTT feature using the SDK version 2.8.2 or older is incompatible with firmware version 2.9.0.x, 2.9.1.x, and 2.9.2.x. User is required to update to 2.9.x SDK to work with 2.9.0.x, 2.9.1.x, and 2.9.2.x firmware.

### Wi-Fi
  - WPA3 Connection with Hunting and Pecking algorithm takes about 3-4 seconds.
  - If the station performs a scan in concurrent mode (Wi-Fi STA+AP), stations connected to the AP may be disconnected. Enable AP after the STA connection is completed.
  - Issue observed with WPA2 Enterprise Connectivity using Microsoft RADIUS Server.
  - PTA 3-wire coexistence does not work with power save. However, if a weak pull up is applied to the GRANT pin, PTA 3-wire functionality will work with power save. Details can be found in the application notes.
  - EAP security in the aws_device_shadow application is unsupported on the EFR platform due to memory constraints.
  - In STA+AP, If AP started first station scan is supported only in channel in which AP is brought up. If multiple channel scan is requested, AP behaviour is undefined.
  - Auto channel selection with EXT_FEAT_IEEE_80211J feature enabled is not supported in Access point operating mode.

### BT-Classic/BLE
  - Wake on wireless is not supported for BLE.
  - BT-HID may not interoperate with Apple devices.
  - In Wi-Fi + BT/BLE Co-Ex mode, high Wi-Fi broadcast traffic might cause BT/BLE disconnections.
  - In Wi-Fi + BT mode when there is continuous Wi-Fi data, you might observe BT not re-connecting to the remote after disconnection.
  - For BLE, dual role scenario issues may occur when connecting as a central when the peripheral is advertising. Central connections can be made after peripheral connections are established and advertising stops.
  - A BLE disconnection will occur if both the peer and RS9116 already have bond info and the RS9116 initiates security as part of BLE secure connections. To workaround this issue, the application should not initiate a security request if it already has bond info.
  - A2DP music streaming glitches may be observed if a Wi-Fi download is in progress at the same time.
  - RTOS support is not there in ae_peripheral & ae_central applications.
  - AE connection issue is expected with Samsung mobiles.
  - BT inquiry will not work in temperatures ranging from -40C to 0C.

### Coexistence
  - After power save is enabled in co-ex mode, `radio_init()` must be called to turn off power save.
  - In Wi-Fi + BLE mode:
    - BLE may disconnect when the supervision timeout is configured to be less than 16 seconds.
    - WPS does not work.
  - In Wi-Fi + BLE / BLE only modes:
    - When Wi-Fi disconnects, a BT/BLE reconnection issue is observed.
  - In Wi-Fi + BT mode:
    - When BT SPP is connected, WLAN throughput prints are not displayed in Teraterm with wlan_bt_throughput_app demo

### Interoperability
  - Disconnection may occur in Wi-Fi client mode if an AP does not acknowledge QoS null frames with power save configured.

### System
  - Chip Revision 1.3 doesn't support Low power mode optimization using the bit setting defined by EXT_FEAT_LOW_POWER_MODE, setting this results in a hang issue.
  - Sometimes the RS9116 enters power save mode before the configured monitor interval for inactivity when co-ex is configured.
  - The FTP client application does not work with EFx32/STM32 when an RTOS is used.
  - The HTTP OTA update application does not work with EFR32 and EFM32 when an RTOS is used.
  - Firmware update does not work for the EFM32 platform.
  - SAPI driver with UART host interface is supported only on bare metal.
  - Powersave without RAM retention does not work when the SPI interface is used.
  - MQTT Receive may encounter failures in STA + AP concurrent mode.
  - Standby associated listen interval current consumption is higher than expected with EFM Platform.

## Limitations and Unsupported Features

### Wi-Fi/Network Stack
  - AMSDU transmit is not supported.
  - Fragmentation is not supported.
  - AMSDU's within AMPDU is not supported.
  - Currently, the RS9116 does not support the radio measurement requests feature of CCX V2
  - 802.11k is not supported
  - Short GI is not supported
  - 40 MHz bandwidth for both the 2.4 & 5 GHz bands is not supported.
  - 802.11j channels less than 16 are not supported.
  - The USB host interface and USB power save are not supported.
  - In AT mode, the total MQTT command length (apart from MQTT publish) should not exceed `150` bytes. This includes `at+rsi` (start of command) to (end of command)`\r\n`.
  - Configure `MQTT_VERSION` in `rsi_mqtt_client.h` based on the server configuration; only version 3 and 4 are supported.
  - 3xTLS connections or 1xTCP and 2xTLS connection, connections are supported concurrently in Wi-Fi only mode.
  - TLS curve IDs supported are 15-28. TLS handshake with 3rd party clients depends on the TLS curve.
  - Big endian architecture is not supported.
  - Auto PAC Provisioning in EAP-FAST with TLSv1.2 is not supported.
  - RC4_SHA, DES_CBC3_SHA, and RC4_MD5 ciphers are not supported in enterprise security
  - SA query procedure not supported in 11W AP mode.
  - If background scan is enabled along with custom feature bit map bit (8) configuration of the DFS channel support, issues may occur in obtaining an IP address for the Wi-Fi STA.
  - In AP mode, IPv6 Router Advertisements are not supported. Although STA connected to the APUT can transfer IPv6 data to the peers by using SLAAC for address configuration.
  - Transparent mode is unsupported in USB-CDC mode.
  - Channel 144 is not supported.
  - IOP issues may be observed with device running DHCPv6 server with Android devices, as latter don't support DHCPv6 process for IPv6 address configuration.
  - WPA3 AP PMKSA is not supported.
  - WPA3 AP mode is supported with alpha-level quality
  - WPA3 AP transition mode is not supported.

### BT-Classic/BLE
  - BT A2DP is only supported for RS9116 silicon revision 1.5.
  - BT sniff mode does not work if BT multiple slaves feature is enabled.
  - For BLE, if the connection is established with a small connection interval (<15 ms), simultaneous roles are not supported (i.e., Central/Peripheral + Advertising/Scanning).
  - For BT Classic, only 1 connection is supported at any time.
  - BLE slave latency value is valid up to 32 only.
  - BT-A2DP encoder is not supported in the firmware.
  - BT-A2DP is not supported in AT mode.
  - Maximum supported  AE data length is 200 bytes.
  - Supports only two ADV_EXT sets.
  - Supports only one BLE connection with AE.
  - SMP is not supported in AE applications "ble_ae_central" and "ble_ae_peripheral".
  - GATT transactions & Data transfer are not supported in AE applications  "ble_ae_central" and "ble_ae_peripheral".
  - BLE duty cycling is not supported.
  - User_config method is not supported for BLE.

### Coexistence
  - Wi-Fi STA + BT + BLE multiprotocol use cases require a detailed understanding of use cases and associated configurations. Contact Silicon Labs support for additional details.
  - Wi-Fi AP + BLE, Wi-Fi AP + BT & Wi-Fi AP + BT + BLE modes are not supported.
  - For AT commands, Wi-Fi+BT+BLE( Opermode 9 ), BT+BLE(Opermode 8 ) do not work.
  - In Wi-Fi + BLE mode, if the BLE scan interval and window have the same value, then the Wi-Fi connection may be unsuccessful.
  - Advertising Extention feature is not supported in Coexistence.

### System
  - Wake on Wireless support has only been tested for the UART AT command interface.
  - Firmware update via bootloader method requires the user to check the integrity of the RPS file content. A corrupted image may cause the chip to go unresponsive to host commands.

## Notes
### BT-Classic/BLE
 - Acceptlist address should be given in reverse order, i.e in big endian format.
 - AE feature validated using rev 1.5 AC1 expansion board.
 - AE feature is not supported in 9116 rev 1.4.
 - Advertising Extentions is Beta quality feature.

# WiSeConnect 2.9.1-Qual Release Notes

Last updated: March 21, 2024

## Highlights

  - New features
  - Bug fixes

## Release Details
| Item                                                | Details                                |
|-----------------------------------------------------|----------------------------------------|
| Release date                                        | March 22, 2024                         |
| API Version (SAPI)                                  | 2.9.1.18 (Build 18)                    |
| Firmware Version                                    | 2.9.1.0.18 (Build 18)                  |
| Package Name                                        | RS9116W.2.9.1.18                       |
| Supported RTOS                                      | FreeRTOS                               |
| Hardware Modules                                    | QMS, B00, C00, CC1, AB0, AB1, AA0, AA1, AC0, AC1 |
| Hardware Chipsets                                   | Chip Revision 1.3, Chip Revision 1.4, Chip Revision 1.5   |
| Operating Modes Supported                           | Wi-Fi STA, Wi-Fi AP, Wi-Fi STA+BLE, Wi-Fi STA+AP     |
| Additional Operating Modes Supported (Beta Quality) | Wi-Fi STA+BT, Wi-Fi STA+BT+BLE , BLE(With AE)     |

## Updates in this Release
RS9116W 2.9.1.18 release consists of two components, as follows:
  - Firmware - RS9116 Firmware Binary
  - SAPI Library - SAPI Library runs on Host

This release is meant only for use with designs based on RS9116 Silicon rev 1.4 (RS9116X-xxx-xxx-Bxx) and RS9116 Silicon rev 1.5 (RS9116X-xxx-xxx-Cxx).
Customers using the RS9116X-xxx-xxx-Xxx parts (Silicon rev 1.3) can also upgrade to the latest firmware, however, the power optimization feature macro `EXT_FEAT_LOW_POWER_MODE` in `rsi_wlan_config.h` must be disabled for revision 1.3.


Choose the firmware to match the silicon revision used in your design, as follows:
- RS9116W.2.9.1.0.18.rps
  - RS9116X-xxx-xxx-Xxx (Silicon rev 1.3)
  - RS9116X-xxx-xxx-Bxx (Silicon rev 1.4)
- RS916W.2.9.1.0.18.rps
  - RS9116X-xxx-xxx-Cxx (Silicon rev 1.5)
  - RS916WS00ACxxx

## New Features
### Wi-Fi
  - None

### System
  - None

### BT-Classic/BLE
  - None

### Network Stack
  - None

## Changes and Fixes
### Wi-Fi
  - None

### Network Stack
  - None

### BT-Classic/BLE
  - 9116 is now BLE.5.4 (+AE) qualified.

### Coexistence
  - None

### System
  - None

### Documentation
  - None

## Recommendations and New Options

### Wi-Fi
  - To ensure graceful handling during asynchronous TCP closures from a peer, enable the opermode parameter [ext_tcp_ip_feature_bit_map\[16\]](https://docs.silabs.com/rs9116-wiseconnect/2.6/wifibt-wc-sapi-reference/opermode#rsi-ext-tcpip-feature-bitmap)
  - Enable aggregation using the opermode parameter [feature_bit_map\[2\]](https://docs.silabs.com/rs9116-wiseconnect/2.6/wifibt-wc-sapi-reference/opermode#rsi-feature-bit-map)
  - To avoid TCP disconnects during a rejoin, set TCP retransmission count to >= `30`
  - To make a connection more robust for low throughput applications in busy networks, disable high MCS rates. For example, in a congested wireless envionment, a Smart Lock may benefit by disabling `MCS6` and `MCS7`.
  - To restart the RS9116, the application should call `rsi_driver_deinit()` followed by `rsi_driver_init()` and `rsi_device_init()`. For OS cases, additionally call `rsi_task_destroy(driver_task_handle)` to delete the driver task before calling `rsi_driver_deinit()` and create again after `rsi_device_init()` using `rsi_task_create()`.

### BT-Classic/BLE
  - In BLE, the recommended range of the BLEConnection Interval is as follows:
    - Power Save (BLE Only) - 100 ms to 1.28 s
    - BT Classic + BLE Dual Mode is >= 200 ms
    - Wi-Fi + BLE coex - 30 ms to 250 ms
  - In BLE during Connection, the configuration of Scan Interval and Scan Window with the same value is not recommended
  - In BT Classic, the recommended Sniff Interval configuration during powersave is limited to 100 ms (\<= 100).
  - In BLE, if a device is acting as Central, the scan window (in `set_scan_params()` and `create_connection()` APIs) must be less than the existing Connection Interval.
  - In BLE mode, if scanning and advertising is in progress, and the device subsequently connects and moves to the central role, scanning and advertising stops. To further establish connection to another peripheral device or to a central device, the application must initiate advertising and scanning again.
  - Device powersave must be disabled prior to BT init and de-init.

### Coexistence
  - For concurrent Wi-Fi + BLE, and while a Wi-Fi connection is active, we recommend setting the ratio of the BLE scan window to BLE scan interval to 1:3 or 1:4.
  - Wi-Fi + BLE Advertising
    - All standard advertising intervals are supported. As Wi-Fi throughput is increased, a slight difference in on-air advertisements compared to configured intervals may be observed.
    - BLE advertising is skipped if the advertising interval collides with Wi-Fi activity.
  - Wi-Fi + BLE scanning
    - All standard scan intervals are supported. For better scan results, we recommend setting the ratio of the BLE scan window to BLE scan interval to 1:3 or 1:4.
    - BLE scanning will be stopped for intervals that collide with Wi-Fi activity.
  - Wi-Fi + BLE Central/Peripheral Connections
    - All standard connection intervals are supported.
    - For a stable connection, use optimal connection intervals and max supervision timeout in the presence of Wi-Fi activity.
  - Wi-Fi + BLE Central/Peripheral Data Transfer
    - To achieve higher throughput for both Wi-Fi and BLE, use medium connection intervals, such as 45 to 80 ms with maximum supervision timeout.
    - Ensure Wi-Fi activity consumes lower intervals.
  - For Wi-Fi + BT + BLE, Wi-Fi + BT, Wi-Fi + BLE operating modes, connect Wi-Fi before BT/BLE connections to ensure seamless, stable connection for all 3 protocols.
  - For Wi-Fi + BT + BLE, Wi-Fi + BT, Wi-Fi + BLE operating modes, if BT/BLE needs to be connected before a Wi-Fi connection, use with high supervision timeout and high connection interval for BLE and high sniff interval for BT, to ensure seamless, stable connection. This configuration also ensures a stable BT/BLE connection when Wi-Fi connects/disconnects/rejoins.

### System
  - For User Store configuration and Configuration Save, do not enable power save or save it as a configuration. If power save is enabled and saved as a configuration, upon boot up, the RS9116 will boot with the saved configuration and will go to powersave without any indication to the host.
  - Set the compiler optimization level to `O0` in project settings for IDE (KEIL).
  - Memory configuration must be 384K for BT/BLE and co-ex operating modes
    - Usage of low power flash mode bit (bit 19 in extended customer feature bitmap). Enable this bit for ultra low power standby associated scenarios. This results in about 20&micro;A lower Wi-Fi standby associated current consumption.
  - Memory Configuration in SAPI functions: The default memory configuration is 384K.
  - Apply Opermode commands in AT mode correctly. Using the wrong opermode may lead to unspecified behavior.
  - Set the recommended Power Save Profile (PSP) type to Enhanced Max PSP.
  - During firmware updates, powersave operation should be disabled.
  - For high throughput applications, powersave operation should be disabled.
  - The application must set the real-time clock with correct timestamp when the feature is enabled before establishing a TLS connection.
  - The socket select and socket receive timeout must be at least 1 second. A timeout value under 1 second is not supported.

## Known Issues

### Network Stack
  - Reassembly of fragmented MQTT packets is not supported in embedded MQTT. MQTT transactions may fail with fragemented packets received from the broker.
  - Sometimes during a TLS handshake, ECC curve parameters may be incorrectly generated resulting in connection failure with BBD2 error. However, this recovers in the next attempt.
  - MQTT disconnects during firmware update when power save is enabled. Disable power save during firmware update and re-enable on update completion.
  - All GCM-based cipher suites are implemented in software and have limited performance.
  - The recommended MQTT publish payload is 1 kBytes.
  - If HTTP server functionality is enabled, do not use port 80 for the MQTT client.
  - Randomize the client port if using rapid connect/disconnect of the MQTT session on the same client port with powersave.
  - Secure TLS renegotiation is not supported in the embedded networking stack.
  - In WPA2 enterprise security mode getting wireless statistics using rsi_wlan_get() API has restrictions.
  - Example application wlan_throughtput_bt_spp_ble_dual_role is unsupported with bare metal.
  - In concurrent mode, there could be a loss of communication on the STA application, when simultaneously multiple clients are connected to the APUT.
  - IP address conflict detection has limitations in STA + AP concurrent mode, failure is not reported to host.
  - There may be instability with the MQTT connection when MQTT Keep alive period is configured as 0.
  - mqtt_client and mqtt_client_v6 applications don't support SDIO host interface
  - During firmware update via OTA (TCP/HTTP) methods, if the firmware image received is corrupted it can result in module going unresponsive on the next bootup.

### Wi-Fi
  - WPA3 Connection with Hunting and Pecking algorithm takes about 3-4 seconds.
  - If the station performs a scan in concurrent mode (Wi-Fi STA+AP), stations connected to the AP may be disconnected. Enable AP after the STA connection is completed.
  - Issue observed with WPA2 Enterprise Connectivity using Microsoft RADIUS Server.
  - PTA 3-wire coexistence does not work with power save. However, if a weak pull up is applied to the GRANT pin, PTA 3-wire functionality will work with power save. Details can be found in the application notes.
  - EAP security in the aws_device_shadow application is unsupported on the EFR platform due to memory constraints.
  - In STA+AP, If AP started first station scan is supported only in channel in which AP is brought up. If multiple channel scan is requested, AP behaviour is undefined.
  - Auto channel selection with EXT_FEAT_IEEE_80211J feature enabled is not supported in Access point operating mode.
  - Module fails to enter into deep sleep mode after AP stop.

### BT-Classic/BLE
  - Wake on wireless is not supported for BLE.
  - BT-HID may not interoperate with Apple devices.
  - In Wi-Fi + BT/BLE Co-Ex mode, high Wi-Fi broadcast traffic might cause BT/BLE disconnections.
  - In Wi-Fi + BT mode when there is continuous Wi-Fi data, you might observe BT not re-connecting to the remote after disconnection.
  - For BLE, dual role scenario issues may occur when connecting as a central when the peripheral is advertising. Central connections can be made after peripheral connections are established and advertising stops.
  - A BLE disconnection will occur if both the peer and RS9116 already have bond info and the RS9116 initiates security as part of BLE secure connections. To workaround this issue, the application should not initiate a security request if it already has bond info.
  - A2DP music streaming glitches may be observed if a Wi-Fi download is in progress at the same time.
  - RTOS support is not there in ae_peripheral & ae_central applications.
  - AE connection issue is expected with Samsung mobiles.
  - BT inquiry will not work in temperatures ranging from -40C to 0C.

### Coexistence
  - After power save is enabled in co-ex mode, `radio_init()` must be called to turn off power save.
  - In Wi-Fi + BLE mode:
    - BLE may disconnect when the supervision timeout is configured to be less than 16 seconds.
    - WPS does not work.
  - In Wi-Fi + BLE / BLE only modes:
    - When Wi-Fi disconnects, a BT/BLE reconnection issue is observed.
  - In Wi-Fi + BT mode:
    - When BT SPP is connected, WLAN throughput prints are not displayed in Teraterm with wlan_bt_throughput_app demo

### Interoperability
  - Disconnection may occur in Wi-Fi client mode if an AP does not acknowledge QoS null frames with power save configured.

### System
  - Chip Revision 1.3 doesn't support Low power mode optimization using the bit setting defined by EXT_FEAT_LOW_POWER_MODE, setting this results in a hang issue.
  - Sometimes the RS9116 enters power save mode before the configured monitor interval for inactivity when co-ex is configured.
  - The FTP client application does not work with EFx32/STM32 when an RTOS is used.
  - The HTTP OTA update application does not work with EFR32 and EFM32 when an RTOS is used.
  - Firmware update does not work for the EFM32 platform.
  - SAPI driver with UART host interface is supported only on bare metal.
  - Powersave without RAM retention does not work when the SPI interface is used.
  - MQTT Receive may encounter failures in STA + AP concurrent mode.
  - Standby associated listen interval current consumption is higher than expected with EFM Platform.

## Limitations and Unsupported Features

### Wi-Fi/Network Stack
  - AMSDU transmit is not supported.
  - Fragmentation is not supported.
  - AMSDU's within AMPDU is not supported.
  - Currently, the RS9116 does not support the radio measurement requests feature of CCX V2
  - 802.11k is not supported
  - Short GI is not supported
  - 40 MHz bandwidth for both the 2.4 & 5 GHz bands is not supported.
  - 802.11j channels less than 16 are not supported.
  - The USB host interface and USB power save are not supported.
  - In AT mode, the total MQTT command length (apart from MQTT publish) should not exceed `150` bytes. This includes `at+rsi` (start of command) to (end of command)`\r\n`.
  - Configure `MQTT_VERSION` in `rsi_mqtt_client.h` based on the server configuration; only version 3 and 4 are supported.
  - 3xTLS connections or 1xTCP and 2xTLS connection, connections are supported concurrently in Wi-Fi only mode.
  - TLS curve IDs supported are 15-28. TLS handshake with 3rd party clients depends on the TLS curve.
  - Big endian architecture is not supported.
  - Auto PAC Provisioning in EAP-FAST with TLSv1.2 is not supported.
  - RC4_SHA, DES_CBC3_SHA, and RC4_MD5 ciphers are not supported in enterprise security
  - SA query procedure not supported in 11W AP mode.
  - If background scan is enabled along with custom feature bit map bit (8) configuration of the DFS channel support, issues may occur in obtaining an IP address for the Wi-Fi STA.
  - In AP mode, IPv6 Router Advertisements are not supported. Although STA connected to the APUT can transfer IPv6 data to the peers by using SLAAC for address configuration.
  - Transparent mode is unsupported in USB-CDC mode.
  - Channel 144 is not supported.
  - IOP issues may be observed with device running DHCPv6 server with Android devices, as latter don't support DHCPv6 process for IPv6 address configuration.
  - WPA3 AP PMKSA is not supported.
  - WPA3 AP mode is supported with alpha-level quality
  - WPA3 AP transition mode is not supported.

### BT-Classic/BLE
  - BT A2DP is only supported for RS9116 silicon revision 1.5.
  - BT sniff mode does not work if BT multiple slaves feature is enabled.
  - For BLE, if the connection is established with a small connection interval (<15 ms), simultaneous roles are not supported (i.e., Central/Peripheral + Advertising/Scanning).
  - For BT Classic, only 1 connection is supported at any time.
  - BLE slave latency value is valid up to 32 only.
  - BT-A2DP encoder is not supported in the firmware.
  - BT-A2DP is not supported in AT mode.
  - Maximum supported  AE data length is 200 bytes.
  - Supports only two ADV_EXT sets.
  - Supports only one BLE connection with AE.
  - SMP is not supported in AE applications "ble_ae_central" and "ble_ae_peripheral".
  - GATT transactions & Data transfer are not supported in AE applications  "ble_ae_central" and "ble_ae_peripheral".
  - BLE duty cycling is not supported.

### Coexistence
  - Wi-Fi STA + BT + BLE multiprotocol use cases require a detailed understanding of use cases and associated configurations. Contact Silicon Labs support for additional details.
  - Wi-Fi AP + BLE, Wi-Fi AP + BT & Wi-Fi AP + BT + BLE modes are not supported.
  - For AT commands, Wi-Fi+BT+BLE( Opermode 9 ), BT+BLE(Opermode 8 ) do not work.
  - In Wi-Fi + BLE mode, if the BLE scan interval and window have the same value, then the Wi-Fi connection may be unsuccessful.
  - Advertising Extention feature is not supported in Coexistence.

### System
  - Wake on Wireless support has only been tested for the UART AT command interface.
  - Firmware update via bootloader method requires the user to check the integrity of the RPS file content. A corrupted image may cause the chip to go unresponsive to host commands.

## Notes
### BT-Classic/BLE
 - Acceptlist address should be given in reverse order, i.e in big endian format.
 - AE feature validated using 1.5Rev AC1 expansion board.
 - AE feature is not supported in 9116 1.4Rev.
 - Advertising Extentions is Beta quality feature.

# WiSeConnect 2.9.1 Release Notes

Last updated: February 28, 2024

## Highlights

  - New features
  - Bug fixes

## Release Details
| Item                                                | Details                                |
|-----------------------------------------------------|----------------------------------------|
| Release date                                        | March 8, 2024                          |
| API Version (SAPI)                                  | 2.9.1 (Build 17)                       |
| Firmware Version                                    | 2.9.1.0.17 (Build 17)                  |
| Package Name                                        | RS9116W.2.9.1.17                       |
| Supported RTOS                                      | FreeRTOS                               |
| Hardware Modules                                    | QMS, B00, C00, CC1, AB0, AB1, AA0, AA1, AC0, AC1 |
| Hardware Chipsets                                   | Chip Revision 1.3, Chip Revision 1.4, Chip Revision 1.5   |
| Operating Modes Supported                           | Wi-Fi STA, Wi-Fi AP, Wi-Fi STA+BLE, Wi-Fi STA+AP     |
| Additional Operating Modes Supported (Beta Quality) | Wi-Fi STA+BT, Wi-Fi STA+BT+BLE , BLE(With AE)     |

## Updates in this Release
RS9116W 2.9.1 release consists of two components, as follows:
  - Firmware - RS9116 Firmware Binary
  - SAPI Library - SAPI Library runs on Host

This release is meant only for use with designs based on RS9116 Silicon rev 1.4 (RS9116X-xxx-xxx-Bxx) and RS9116 Silicon rev 1.5 (RS9116X-xxx-xxx-Cxx).
Customers using the RS9116X-xxx-xxx-Xxx parts (Silicon rev 1.3) can also upgrade to the latest firmware, however, the power optimization feature macro `EXT_FEAT_LOW_POWER_MODE` in `rsi_wlan_config.h` must be disabled for revision 1.3.


Choose the firmware to match the silicon revision used in your design, as follows:
- RS9116W.2.9.1.0.17.rps
  - RS9116X-xxx-xxx-Xxx (Silicon rev 1.3)
  - RS9116X-xxx-xxx-Bxx (Silicon rev 1.4)
- RS916W.2.9.1.0.17.rps
  - RS9116X-xxx-xxx-Cxx (Silicon rev 1.5)
  - RS916WS00ACxxx

## New Features
### Wi-Fi
  - None

### System
  - None

### BT-Classic/BLE
  - None

### Network Stack
  - Added support in AT mode, to store the IP address obtained from `rsi_dns` command, to be used later for `rsi_tcp` and `rsi_transmode_params` commands without the host requiring to pass the IP address. This feature is supported only on Silicon rev 1.4.

## Changes and Fixes
### Wi-Fi
  - Added support to turn off BGScan from Host/application.
  - Added support for korea region in STA, AP and PER operating modes.
  - Removed 40 MHz channel bandwidth support from HT capabilities.
  - Corrected the region codes for korea and singapore.
  - Added a fix to address AUTO-JOIN failure when firmware upgrade.
  - Fixed AP disconnection observed on some parts due to HP PLL losing lock.

### Network Stack
  - Added a fix to support port numbers less than 1024 in AT mode.
  - Added support to handle public and private key validation for EAP-TLS connection
  - Added support to configure MQTT keepalive retries from the host. The firmware will report disconnection only after all the retries have exhausted.
  - Enhanced the EAP-TLS handshake to handle the pathlen field also.
  - Handled proper socket-related error codes in case of MQTT failures.

### BT-Classic/BLE
  - Resolved wlan_throughput_bt_spp_ble_dual_role demo BLE disconnection issue with powersave.
  - Addressed the reassembly issue for fragmented packets received from remote devices.
  - Fixed an issue in which BLE was unable to connect to the peer device when attempting to connect and disconnect in a loop.
  - Removed unused variable warnings in SDK
  - AT+RSIBT_readresp command can be used to send 239 bytes of data.

### Coexistence
  - None

### System
  - Resolved data stuck issue in message-based power save case by fixing the race around condition in tx event handler.

### Documentation
  - The string Redpine is removed properly and replaced with Silicon Labs wherever necessary.

## Recommendations and New Options

### Wi-Fi
  - To ensure graceful handling during asynchronous TCP closures from a peer, enable the opermode parameter [ext_tcp_ip_feature_bit_map\[16\]](https://docs.silabs.com/rs9116-wiseconnect/2.6/wifibt-wc-sapi-reference/opermode#rsi-ext-tcpip-feature-bitmap)
  - Enable aggregation using the opermode parameter [feature_bit_map\[2\]](https://docs.silabs.com/rs9116-wiseconnect/2.6/wifibt-wc-sapi-reference/opermode#rsi-feature-bit-map)
  - To avoid TCP disconnects during a rejoin, set TCP retransmission count to >= `30`
  - To make a connection more robust for low throughput applications in busy networks, disable high MCS rates. For example, in a congested wireless envionment, a Smart Lock may benefit by disabling `MCS6` and `MCS7`.
  - To restart the RS9116, the application should call `rsi_driver_deinit()` followed by `rsi_driver_init()` and `rsi_device_init()`. For OS cases, additionally call `rsi_task_destroy(driver_task_handle)` to delete the driver task before calling `rsi_driver_deinit()` and create again after `rsi_device_init()` using `rsi_task_create()`.

### BT-Classic/BLE
  - In BLE, the recommended range of the BLEConnection Interval is as follows:
    - Power Save (BLE Only) - 100 ms to 1.28 s
    - BT Classic + BLE Dual Mode is >= 200 ms
    - Wi-Fi + BLE coex - 30 ms to 250 ms
  - In BLE during Connection, the configuration of Scan Interval and Scan Window with the same value is not recommended
  - In BT Classic, the recommended Sniff Interval configuration during powersave is limited to 100 ms (\<= 100).
  - In BLE, if a device is acting as Central, the scan window (in `set_scan_params()` and `create_connection()` APIs) must be less than the existing Connection Interval.
  - In BLE mode, if scanning and advertising is in progress, and the device subsequently connects and moves to the central role, scanning and advertising stops. To further establish connection to another peripheral device or to a central device, the application must initiate advertising and scanning again.
  - Device powersave must be disabled prior to BT init and de-init.

### Coexistence
  - For concurrent Wi-Fi + BLE, and while a Wi-Fi connection is active, we recommend setting the ratio of the BLE scan window to BLE scan interval to 1:3 or 1:4.
  - Wi-Fi + BLE Advertising
    - All standard advertising intervals are supported. As Wi-Fi throughput is increased, a slight difference in on-air advertisements compared to configured intervals may be observed.
    - BLE advertising is skipped if the advertising interval collides with Wi-Fi activity.
  - Wi-Fi + BLE scanning
    - All standard scan intervals are supported. For better scan results, we recommend setting the ratio of the BLE scan window to BLE scan interval to 1:3 or 1:4.
    - BLE scanning will be stopped for intervals that collide with Wi-Fi activity.
  - Wi-Fi + BLE Central/Peripheral Connections
    - All standard connection intervals are supported.
    - For a stable connection, use optimal connection intervals and max supervision timeout in the presence of Wi-Fi activity.
  - Wi-Fi + BLE Central/Peripheral Data Transfer
    - To achieve higher throughput for both Wi-Fi and BLE, use medium connection intervals, such as 45 to 80 ms with maximum supervision timeout.
    - Ensure Wi-Fi activity consumes lower intervals.
  - For Wi-Fi + BT + BLE, Wi-Fi + BT, Wi-Fi + BLE operating modes, connect Wi-Fi before BT/BLE connections to ensure seamless, stable connection for all 3 protocols.
  - For Wi-Fi + BT + BLE, Wi-Fi + BT, Wi-Fi + BLE operating modes, if BT/BLE needs to be connected before a Wi-Fi connection, use with high supervision timeout and high connection interval for BLE and high sniff interval for BT, to ensure seamless, stable connection. This configuration also ensures a stable BT/BLE connection when Wi-Fi connects/disconnects/rejoins.

### System
  - For User Store configuration and Configuration Save, do not enable power save or save it as a configuration. If power save is enabled and saved as a configuration, upon boot up, the RS9116 will boot with the saved configuration and will go to powersave without any indication to the host.
  - Set the compiler optimization level to `O0` in project settings for IDE (KEIL).
  - Memory configuration must be 384K for BT/BLE and co-ex operating modes
    - Usage of low power flash mode bit (bit 19 in extended customer feature bitmap). Enable this bit for ultra low power standby associated scenarios. This results in about 20&micro;A lower Wi-Fi standby associated current consumption.
  - Memory Configuration in SAPI functions: The default memory configuration is 384K.
  - Apply Opermode commands in AT mode correctly. Using the wrong opermode may lead to unspecified behavior.
  - Set the recommended Power Save Profile (PSP) type to Enhanced Max PSP.
  - During firmware updates, powersave operation should be disabled.
  - For high throughput applications, powersave operation should be disabled.
  - The application must set the real-time clock with correct timestamp when the feature is enabled before establishing a TLS connection.
  - The socket select and socket receive timeout must be at least 1 second. A timeout value under 1 second is not supported.

## Known Issues

### Network Stack
  - Reassembly of fragmented MQTT packets is not supported in embedded MQTT. MQTT transactions may fail with fragemented packets received from the broker.
  - Sometimes during a TLS handshake, ECC curve parameters may be incorrectly generated resulting in connection failure with BBD2 error. However, this recovers in the next attempt.
  - MQTT disconnects during firmware update when power save is enabled. Disable power save during firmware update and re-enable on update completion.
  - All GCM-based cipher suites are implemented in software and have limited performance.
  - The recommended MQTT publish payload is 1 kBytes.
  - If HTTP server functionality is enabled, do not use port 80 for the MQTT client.
  - Randomize the client port if using rapid connect/disconnect of the MQTT session on the same client port with powersave.
  - Secure TLS renegotiation is not supported in the embedded networking stack.
  - In WPA2 enterprise security mode getting wireless statistics using rsi_wlan_get() API has restrictions.
  - Example application wlan_throughtput_bt_spp_ble_dual_role is unsupported with bare metal.
  - In concurrent mode, there could be a loss of communication on the STA application, when simultaneously multiple clients are connected to the APUT.
  - IP address conflict detection has limitations in STA + AP concurrent mode, failure is not reported to host.
  - There may be instability with the MQTT connection when MQTT Keep alive period is configured as 0.
  - mqtt_client and mqtt_client_v6 applications don't support SDIO host interface
  - During firmware update via OTA (TCP/HTTP) methods, if the firmware image received is corrupted it can result in module going unresponsive on the next bootup.

### Wi-Fi
  - WPA3 Connection with Hunting and Pecking algorithm takes about 3-4 seconds.
  - If the station performs a scan in concurrent mode (Wi-Fi STA+AP), stations connected to the AP may be disconnected. Enable AP after the STA connection is completed.
  - Issue observed with WPA2 Enterprise Connectivity using Microsoft RADIUS Server.
  - PTA 3-wire coexistence does not work with power save. However, if a weak pull up is applied to the GRANT pin, PTA 3-wire functionality will work with power save. Details can be found in the application notes.
  - EAP security in the aws_device_shadow application is unsupported on the EFR platform due to memory constraints.
  - In STA+AP, If AP started first station scan is supported only in channel in which AP is brought up. If multiple channel scan is requested, AP behaviour is undefined.
  - Auto channel selection with EXT_FEAT_IEEE_80211J feature enabled is not supported in Access point operating mode.
  - Module fails to enter into deep sleep mode after AP stop.

### BT-Classic/BLE
  - Wake on wireless is not supported for BLE.
  - BT-HID may not interoperate with Apple devices.
  - In Wi-Fi + BT/BLE Co-Ex mode, high Wi-Fi broadcast traffic might cause BT/BLE disconnections.
  - In Wi-Fi + BT mode when there is continuous Wi-Fi data, you might observe BT not re-connecting to the remote after disconnection.
  - For BLE, dual role scenario issues may occur when connecting as a central when the peripheral is advertising. Central connections can be made after peripheral connections are established and advertising stops.
  - A BLE disconnection will occur if both the peer and RS9116 already have bond info and the RS9116 initiates security as part of BLE secure connections. To workaround this issue, the application should not initiate a security request if it already has bond info.
  - A2DP music streaming glitches may be observed if a Wi-Fi download is in progress at the same time.
  - RTOS support is not there in ae_peripheral & ae_central applications.
  - AE connection issue is expected with Samsung mobiles.
  - BT inquiry will not work in temperatures ranging from -40C to 0C.

### Coexistence
  - After power save is enabled in co-ex mode, `radio_init()` must be called to turn off power save.
  - In Wi-Fi + BLE mode:
    - BLE may disconnect when the supervision timeout is configured to be less than 16 seconds.
    - WPS does not work.
  - In Wi-Fi + BLE / BLE only modes:
    - When Wi-Fi disconnects, a BT/BLE reconnection issue is observed.
  - In Wi-Fi + BT mode:
    - When BT SPP is connected, WLAN throughput prints are not displayed in Teraterm with wlan_bt_throughput_app demo

### Interoperability
  - Disconnection may occur in Wi-Fi client mode if an AP does not acknowledge QoS null frames with power save configured.

### System
  - Chip Revision 1.3 doesn't support Low power mode optimization using the bit setting defined by EXT_FEAT_LOW_POWER_MODE, setting this results in a hang issue.
  - Sometimes the RS9116 enters power save mode before the configured monitor interval for inactivity when co-ex is configured.
  - The FTP client application does not work with EFx32/STM32 when an RTOS is used.
  - The HTTP OTA update application does not work with EFR32 and EFM32 when an RTOS is used.
  - Firmware update does not work for the EFM32 platform.
  - SAPI driver with UART host interface is supported only on bare metal.
  - Powersave without RAM retention does not work when the SPI interface is used.
  - MQTT Receive may encounter failures in STA + AP concurrent mode.
  - Standby associated listen interval current consumption is higher than expected with EFM Platform.

## Limitations and Unsupported Features

### Wi-Fi/Network Stack
  - AMSDU transmit is not supported.
  - Fragmentation is not supported.
  - AMSDU's within AMPDU is not supported.
  - Currently, the RS9116 does not support the radio measurement requests feature of CCX V2
  - 802.11k is not supported
  - Short GI is not supported
  - 40 MHz bandwidth for both the 2.4 & 5 GHz bands is not supported.
  - 802.11j channels less than 16 are not supported.
  - The USB host interface and USB power save are not supported.
  - In AT mode, the total MQTT command length (apart from MQTT publish) should not exceed `150` bytes. This includes `at+rsi` (start of command) to (end of command)`\r\n`.
  - Configure `MQTT_VERSION` in `rsi_mqtt_client.h` based on the server configuration; only version 3 and 4 are supported.
  - 3xTLS connections or 1xTCP and 2xTLS connection, connections are supported concurrently in Wi-Fi only mode.
  - TLS curve IDs supported are 15-28. TLS handshake with 3rd party clients depends on the TLS curve.
  - Big endian architecture is not supported.
  - Auto PAC Provisioning in EAP-FAST with TLSv1.2 is not supported.
  - RC4_SHA, DES_CBC3_SHA, and RC4_MD5 ciphers are not supported in enterprise security
  - SA query procedure not supported in 11W AP mode.
  - If background scan is enabled along with custom feature bit map bit (8) configuration of the DFS channel support, issues may occur in obtaining an IP address for the Wi-Fi STA.
  - In AP mode, IPv6 Router Advertisements are not supported. Although STA connected to the APUT can transfer IPv6 data to the peers by using SLAAC for address configuration.
  - Transparent mode is unsupported in USB-CDC mode.
  - Channel 144 is not supported.
  - IOP issues may be observed with device running DHCPv6 server with Android devices, as latter don't support DHCPv6 process for IPv6 address configuration.
  - WPA3 AP PMKSA is not supported.
  - WPA3 AP transition mode is not supported.

### BT-Classic/BLE
  - BT A2DP is only supported for RS9116 silicon revision 1.5.
  - BT sniff mode does not work if BT multiple slaves feature is enabled.
  - For BLE, if the connection is established with a small connection interval (<15 ms), simultaneous roles are not supported (i.e., Central/Peripheral + Advertising/Scanning).
  - For BT Classic, only 1 connection is supported at any time.
  - BLE slave latency value is valid up to 32 only.
  - BT-A2DP encoder is not supported in the firmware.
  - BT-A2DP is not supported in AT mode.
  - Maximum supported  AE data length is 200 bytes.
  - Supports only two ADV_EXT sets.
  - Supports only one BLE connection with AE.
  - SMP is not supported in AE applications "ble_ae_central" and "ble_ae_peripheral".
  - GATT transactions & Data transfer are not supported in AE applications  "ble_ae_central" and "ble_ae_peripheral".
  - BLE duty cycling is not supported.

### Coexistence
  - Wi-Fi STA + BT + BLE multiprotocol use cases require a detailed understanding of use cases and associated configurations. Contact Silicon Labs support for additional details.
  - Wi-Fi AP + BLE, Wi-Fi AP + BT & Wi-Fi AP + BT + BLE modes are not supported.
  - For AT commands, Wi-Fi+BT+BLE( Opermode 9 ), BT+BLE(Opermode 8 ) do not work.
  - In Wi-Fi + BLE mode, if the BLE scan interval and window have the same value, then the Wi-Fi connection may be unsuccessful.
  - Advertising Extention feature is not supported in Coexistence.

### System
  - Wake on Wireless support has only been tested for the UART AT command interface.
  - Firmware update via bootloader method requires the user to check the integrity of the RPS file content. A corrupted image may cause the chip to go unresponsive to host commands.

## Notes
### BT-Classic/BLE
 - Acceptlist address should be given in reverse order, i.e in big endian format.
 - AE feature validated using 1.5Rev AC1 expansion board.
 - AE feature is not supported in 9116 1.4Rev.
 - Advertising Extentions is Beta quality feature.

# WiSeConnect 2.9.0 Release Notes

Last updated: December 4, 2023

## Highlights

  - New features
  - Bug fixes
  
## Release Details
| Item                                                | Details                                |
|-----------------------------------------------------|----------------------------------------|
| Release date                                        | December 12, 2023                        |
| API Version (SAPI)                                  | 2.9.0 (Build 74)                        |
| Firmware Version                                    | 2.9.0.0.74 (Build 74)                    |
| Package Name                                        | RS9116W.2.9.0.74                        |
| Supported RTOS                                      | FreeRTOS                               |
| Hardware Modules                                    | QMS, B00, C00, CC1, AB0, AB1, AA0, AA1, AC0, AC1 |
| Hardware Chipsets                                   | Chip Revision 1.3, Chip Revision 1.4, Chip Revision 1.5   |
| Operating Modes Supported                           | Wi-Fi STA, Wi-Fi AP, Wi-Fi STA+BLE, Wi-Fi STA+AP     |
| Additional Operating Modes Supported (Beta Quality) | Wi-Fi STA+BT, Wi-Fi STA+BT+BLE , BLE(With AE)     |

## Updates in this Release
RS9116W 2.9.0 release consists of two components, as follows:
  - Firmware - RS9116 Firmware Binary
  - SAPI Library - SAPI Library runs on Host

This release is meant only for use with designs based on RS9116 Silicon rev 1.4 (RS9116X-xxx-xxx-Bxx) and RS9116 Silicon rev 1.5 (RS9116X-xxx-xxx-Cxx).   
Customers using the RS9116X-xxx-xxx-Xxx parts (Silicon rev 1.3) can also upgrade to the latest firmware, however, the power optimization feature macro `EXT_FEAT_LOW_POWER_MODE` in `rsi_wlan_config.h` must be disabled for revision 1.3.


Choose the firmware to match the silicon revision used in your design, as follows:
- RS9116W.2.9.0.0.74.rps
  - RS9116X-xxx-xxx-Xxx (Silicon rev 1.3)
  - RS9116X-xxx-xxx-Bxx (Silicon rev 1.4)
- RS916W.2.9.0.0.74.rps
  - RS9116X-xxx-xxx-Cxx (Silicon rev 1.5)
  - RS916WS00ACxxx

## New Features
### Wi-Fi
  - Added WPA3 support in AP and concurrent (STA+AP) Mode
  - AP stop command support added for AT and SAPI modes.
  - Added support for power save after AP stop, in AP and concurrent (STA+AP) operating modes.
  - Added support to store two SSID profiles in AT mode as part of store configuration.
  
### System
  - None
  
### BT-Classic/BLE
  - None
  
### Network Stack
  - Added support to store the server's IPv6 address and port.Also added support to query the stored server credentials for both IPv4/IPv6 
  - Added support for non-zero pathlen field in the certificates, this feature requires to be enabled from the host.
  
## Changes and Fixes
### Wi-Fi
  - Added support to configure region in PER mode for ACX.
  - Added support for user gain table with store configuration feature.
  - Added fix for missing IEs in initial Beacon frames.
  - Added fix to return correct error code if invalid channels are passed in scan API when the module is in deep sleep.
  - Added fix for PMK generation command.
  - Added fix to set correct user provided region instead of US.
  - Added fix for the trigger auto-join functionality in the store configuration feature.
  - Added fix to reset Beacon sequence number when AP is restarted in concurrent mode.
  - Support added to return an error if EAP or WPA3 transition security modes are set in AP mode.

### Network Stack
  - Resolved instability of embedded MQTT states when run in a loop.
  - Enhanced embedded MQTT topic length to support upto 200 bytes.
  - Added the structure for decoding the embedded MQTT publish receive message at sapi
  - Resolved issue with multiple disconnect events sent to host on socket disconnection
  - Resolved instability with AP stop and start functionality
  - Enhanced embedded MQTT username length to support up to 120 bytes.
  - Resolved IP configuration issues in concurrent mode
  - Resolved instability with socket connection over a long run with connections/disconnection at Wi-Fi level.
  - Added support to send linklocal address in IP configuration response to the host.

### BT-Classic/BLE
  - Fixed Hang Issue after Bluetooth Paired Several Times.
  - Added a fix to obtain negative RSSI values instead of positive ones.
  - Fixed data length during SPP data transfer to ensure it doesn't exceed the TX MTU size in spp connect callback.
  - Fixed the issue of AE disconnection when DUT is configured as central.
  - Fixed issue of not being able to disable extended advertising.

### Coexistence
  - None

### System
  - Join fail callback registered by the application will be called for each disconnection event without requiring to re-register prior to every join command.
  - Added error checks in rsi_bl_upgrade_firmware API to validate the RPS header contents.
  - Modified the aws_iot/mqtt application to receive the publish data from the broker asynchronously
  - Modified semaphore wait timeout units from seconds to ticks
  - Fixed Powersave issue with 32 KHz RC clk in Rev 1p5

### Documentation
  - Removed unsupported feature mDNS Service Discovery from the documentation
  - Improved documentation of rsi_wlan_register_callbacks() API 
  - Enhanced documentation of socket handling at the sapi
  - Documentation corrected for rsi_transmit_test_start API
  - Readme documentation updated for powersave functionality with ACX board

## Recommendations and New Options

### Wi-Fi
  - To ensure graceful handling during asynchronous TCP closures from a peer, enable the opermode parameter [ext_tcp_ip_feature_bit_map\[16\]](https://docs.silabs.com/rs9116-wiseconnect/2.6/wifibt-wc-sapi-reference/opermode#rsi-ext-tcpip-feature-bitmap)
  - Enable aggregation using the opermode parameter [feature_bit_map\[2\]](https://docs.silabs.com/rs9116-wiseconnect/2.6/wifibt-wc-sapi-reference/opermode#rsi-feature-bit-map)
  - To avoid TCP disconnects during a rejoin, set TCP retransmission count to >= `30`
  - To make a connection more robust for low throughput applications in busy networks, disable high MCS rates. For example, in a congested wireless envionment, a Smart Lock may benefit by disabling `MCS6` and `MCS7`.
  - To restart the RS9116, the application should call `rsi_driver_deinit()` followed by `rsi_driver_init()` and `rsi_device_init()`. For OS cases, additionally call `rsi_task_destroy(driver_task_handle)` to delete the driver task before calling `rsi_driver_deinit()` and create again after `rsi_device_init()` using `rsi_task_create()`.

### BT-Classic/BLE
  - In BLE, the recommended range of the BLEConnection Interval is as follows:
    - Power Save (BLE Only) - 100 ms to 1.28 s
    - BT Classic + BLE Dual Mode is >= 200 ms
    - Wi-Fi + BLE coex - 30 ms to 250 ms
  - In BLE during Connection, the configuration of Scan Interval and Scan Window with the same value is not recommended
  - In BT Classic, the recommended Sniff Interval configuration during powersave is limited to 100 ms (\<= 100).
  - In BLE, if a device is acting as Central, the scan window (in `set_scan_params()` and `create_connection()` APIs) must be less than the existing Connection Interval.
  - In BLE mode, if scanning and advertising is in progress, and the device subsequently connects and moves to the central role, scanning and advertising stops. To further establish connection to another peripheral device or to a central device, the application must initiate advertising and scanning again.
  - Device powersave must be disabled prior to BT init and de-init.

### Coexistence
  - For concurrent Wi-Fi + BLE, and while a Wi-Fi connection is active, we recommend setting the ratio of the BLE scan window to BLE scan interval to 1:3 or 1:4.
  - Wi-Fi + BLE Advertising
    - All standard advertising intervals are supported. As Wi-Fi throughput is increased, a slight difference in on-air advertisements compared to configured intervals may be observed.
    - BLE advertising is skipped if the advertising interval collides with Wi-Fi activity.
  - Wi-Fi + BLE scanning
    - All standard scan intervals are supported. For better scan results, we recommend setting the ratio of the BLE scan window to BLE scan interval to 1:3 or 1:4.
    - BLE scanning will be stopped for intervals that collide with Wi-Fi activity.
  - Wi-Fi + BLE Central/Peripheral Connections
    - All standard connection intervals are supported.
    - For a stable connection, use optimal connection intervals and max supervision timeout in the presence of Wi-Fi activity.
  - Wi-Fi + BLE Central/Peripheral Data Transfer
    - To achieve higher throughput for both Wi-Fi and BLE, use medium connection intervals, such as 45 to 80 ms with maximum supervision timeout.
    - Ensure Wi-Fi activity consumes lower intervals.
  - For Wi-Fi + BT + BLE, Wi-Fi + BT, Wi-Fi + BLE operating modes, connect Wi-Fi before BT/BLE connections to ensure seamless, stable connection for all 3 protocols.
  - For Wi-Fi + BT + BLE, Wi-Fi + BT, Wi-Fi + BLE operating modes, if BT/BLE needs to be connected before a Wi-Fi connection, use with high supervision timeout and high connection interval for BLE and high sniff interval for BT, to ensure seamless, stable connection. This configuration also ensures a stable BT/BLE connection when Wi-Fi connects/disconnects/rejoins.

### System
  - For User Store configuration and Configuration Save, do not enable power save or save it as a configuration. If power save is enabled and saved as a configuration, upon boot up, the RS9116 will boot with the saved configuration and will go to powersave without any indication to the host.
  - Set the compiler optimization level to `O0` in project settings for IDE (KEIL).
  - Memory configuration must be 384K for BT/BLE and co-ex operating modes
    - Usage of low power flash mode bit (bit 19 in extended customer feature bitmap). Enable this bit for ultra low power standby associated scenarios. This results in about 20&micro;A lower Wi-Fi standby associated current consumption.
  - Memory Configuration in SAPI functions: The default memory configuration is 384K.
  - Apply Opermode commands in AT mode correctly. Using the wrong opermode may lead to unspecified behavior.
  - Set the recommended Power Save Profile (PSP) type to Enhanced Max PSP.
  - During firmware updates, powersave operation should be disabled.
  - For high throughput applications, powersave operation should be disabled.
  - The application must set the real-time clock with correct timestamp when the feature is enabled before establishing a TLS connection.
  - The socket select and socket receive timeout must be at least 1 second. A timeout value under 1 second is not supported.

## Known Issues
  
### Wi-Fi
  - For WPA/WPA2 mixed mode, security mode is updated as WPA2.
  - Auto channel selection with EXT_FEAT_IEEE_80211J feature enabled is not supported in Access point operating mode.

### Network Stack 
  - Reassembly of fragmented MQTT packets is not supported in embedded MQTT. MQTT transactions may fail with fragemented packets received from the broker. 
  - Sometimes during a TLS handshake, ECC curve parameters may be incorrectly generated resulting in connection failure with BBD2 error. However, this recovers in the next attempt.
  - MQTT disconnects during firmware update when power save is enabled. Disable power save during firmware update and re-enable on update completion.
  - All GCM-based cipher suites are implemented in software and have limited performance.
  - The recommended MQTT publish payload is 1 kBytes.
  - If HTTP server functionality is enabled, do not use port 80 for the MQTT client.
  - Randomize the client port if using rapid connect/disconnect of the MQTT session on the same client port with powersave.
  - Secure TLS renegotiation is not supported in the embedded networking stack.
  - In WPA2 enterprise security mode getting wireless statistics using rsi_wlan_get() API has restrictions.
  - Example application wlan_throughtput_bt_spp_ble_dual_role is unsupported with bare metal.
  - In concurrent mode, there could be a loss of communication on the STA application, when simultaneously multiple clients are connected to the APUT.
  - IP address conflict detection has limitations in STA + AP concurrent mode, failure is not reported to host.
  - There may be instability with the MQTT connection when MQTT Keep alive period is configured as 0.
  - mqtt_client and mqtt_client_v6 applications don't support SDIO host interface
  - During firmware update via OTA (TCP/HTTP) methods, if the firmware image received is corrupted it can result in module going unresponsive on the next bootup.

### Wi-Fi
  - WPA3 Connection with Hunting and Pecking algorithm takes about 3-4 seconds.
  - If the station performs a scan in concurrent mode (Wi-Fi STA+AP), stations connected to the AP may be disconnected. Enable AP after the STA connection is completed.
  - Issue observed with WPA2 Enterprise Connectivity using Microsoft RADIUS Server.
  - PTA 3-wire coexistence does not work with power save. However, if a weak pull up is applied to the GRANT pin, PTA 3-wire functionality will work with power save. Details can be found in the application notes.
  - EAP security in the aws_device_shadow application is unsupported on the EFR platform due to memory constraints.
  - In STA+AP, If AP started first station scan is supported only in channel in which AP is brought up. If multiple channel scan is requested, AP behaviour is undefined.

### BT-Classic/BLE
  - Wake on wireless is not supported for BLE.
  - BT-HID may not interoperate with Apple devices.
  - In Wi-Fi + BT/BLE Co-Ex mode, high Wi-Fi broadcast traffic might cause BT/BLE disconnections.
  - In Wi-Fi + BT mode when there is continuous Wi-Fi data, you might observe BT not re-connecting to the remote after disconnection.
  - For BLE, dual role scenario issues may occur when connecting as a central when the peripheral is advertising. Central connections can be made after peripheral connections are established and advertising stops.
  - A BLE disconnection will occur if both the peer and RS9116 already have bond info and the RS9116 initiates security as part of BLE secure connections. To workaround this issue, the application should not initiate a security request if it already has bond info.
  - A2DP music streaming glitches may be observed if a Wi-Fi download is in progress at the same time.
  - RTOS support is not there in ae_peripheral & ae_central applications.
  - AE connection issue is expected with Samsung mobiles.
  - BT inquiry will not work in temperatures ranging from -40C to 0C.
  - BLR disconnection will not work, when remote device sends a disconnection packet. 
  
### Coexistence
  - After power save is enabled in co-ex mode, `radio_init()` must be called to turn off power save.
  - In Wi-Fi + BLE mode:
    - BLE may disconnect when the supervision timeout is configured to be less than 16 seconds.
    - WPS does not work.
  - In Wi-Fi + BLE / BLE only modes:
    - When Wi-Fi disconnects, a BT/BLE reconnection issue is observed.
  - In Wi-Fi + BT mode:
    - When BT SPP is connected, WLAN throughput prints are not displayed in Teraterm with wlan_bt_throughput_app demo

### Interoperability
  - Disconnection may occur in Wi-Fi client mode if an AP does not acknowledge QoS null frames with power save configured.

### System
  - Chip Revision 1.3 doesn't support Low power mode optimization using the bit setting defined by EXT_FEAT_LOW_POWER_MODE, setting this results in a hang issue.
  - Sometimes the RS9116 enters power save mode before the configured monitor interval for inactivity when co-ex is configured.
  - The FTP client application does not work with EFx32/STM32 when an RTOS is used.
  - The HTTP OTA update application does not work with EFR32 and EFM32 when an RTOS is used.  
  - Firmware update does not work for the EFM32 platform.
  - SAPI driver with UART host interface is supported only on bare metal.
  - Powersave without RAM retention does not work when the SPI interface is used.
  - MQTT Receive may encounter failures in STA + AP concurrent mode.
  - Standby associated listen interval current consumption is higher than expected with EFM Platform.

## Limitations and Unsupported Features

### Wi-Fi/Network Stack
  - AMSDU transmit is not supported.
  - Fragmentation is not supported.
  - AMSDU's within AMPDU is not supported.
  - Currently, the RS9116 does not support the radio measurement requests feature of CCX V2
  - 802.11k is not supported
  - Short GI is not supported
  - 40 MHz bandwidth for both the 2.4 & 5 GHz bands is not supported.
  - 802.11j channels less than 16 are not supported.
  - The USB host interface and USB power save are not supported.
  - In AT mode, the total MQTT command length (apart from MQTT publish) should not exceed `150` bytes. This includes `at+rsi` (start of command) to (end of command)`\r\n`.
  - Configure `MQTT_VERSION` in `rsi_mqtt_client.h` based on the server configuration; only version 3 and 4 are supported.
  - 3xTLS connections or 1xTCP and 2xTLS connection, connections are supported concurrently in Wi-Fi only mode.
  - TLS curve IDs supported are 15-28. TLS handshake with 3rd party clients depends on the TLS curve.
  - Big endian architecture is not supported.
  - Auto PAC Provisioning in EAP-FAST with TLSv1.2 is not supported.
  - RC4_SHA, DES_CBC3_SHA, and RC4_MD5 ciphers are not supported in enterprise security
  - SA query procedure not supported in 11W AP mode.
  - If background scan is enabled along with custom feature bit map bit (8) configuration of the DFS channel support, issues may occur in obtaining an IP address for the Wi-Fi STA.
  - In AP mode, IPv6 Router Advertisements are not supported. Although STA connected to the APUT can transfer IPv6 data to the peers by using SLAAC for address configuration.
  - Transparent mode is unsupported in USB-CDC mode.
  - Channel 144 is not supported.
  - IOP issues may be observed with device running DHCPv6 server with Android devices, as latter don't support DHCPv6 process for IPv6 address configuration.
  - WPA3 AP PMKSA is not supported.
  - WPA3 AP transition mode is not supported.

### BT-Classic/BLE
  - BT A2DP is only supported for RS9116 silicon revision 1.5.
  - BT sniff mode does not work if BT multiple slaves feature is enabled.
  - For BLE, if the connection is established with a small connection interval (<15 ms), simultaneous roles are not supported (i.e., Central/Peripheral + Advertising/Scanning).
  - For BT Classic, only 1 connection is supported at any time.
  - BLE slave latency value is valid up to 32 only.
  - BT-A2DP encoder is not supported in the firmware.
  - BT-A2DP is not supported in AT mode.
  - Maximum supported  AE data length is 200 bytes.
  - Supports only two ADV_EXT sets.
  - Supports only one BLE connection with AE.
  - Filtering policy is not supported for Periodic advertising.
  - SMP is not supported in AE applications "ble_ae_central" and "ble_ae_peripheral". 
  - GATT transactions & Data transfer are not supported in AE applications  "ble_ae_central" and "ble_ae_peripheral".
  - BLE dutty cycling is not supported.  

### Coexistence
  - Wi-Fi STA + BT + BLE multiprotocol use cases require a detailed understanding of use cases and associated configurations. Contact Silicon Labs support for additional details.
  - Wi-Fi AP + BLE, Wi-Fi AP + BT & Wi-Fi AP + BT + BLE modes are not supported.
  - For AT commands, Wi-Fi+BT+BLE( Opermode 9 ), BT+BLE(Opermode 8 ) do not work.
  - In Wi-Fi + BLE mode, if the BLE scan interval and window have the same value, then the Wi-Fi connection may be unsuccessful.
  - Advertising Extention feature is not supported in Coexistence.  

### System
  - Wake on Wireless support has only been tested for the UART AT command interface.
  - Firmware update via bootloader method requires the user to check the integrity of the RPS file content. A corrupted image may cause the chip to go unresponsive to host commands.

## Notes
### BT-Classic/BLE
 - Whitelist address should be given in reverse order.
 - AE feature validated using 1.5Rev AC1 expansion board.
 - AE feature is not supported in 9116 1.4Rev.
 - Advertising Extentions is Beta quality feature.

# WiSeConnect 2.8.2 Release Notes

Last updated: October 3, 2023

## Highlights

  - New features
  - Bug fixes
  
## Release Details
| Item                                                | Details                                |
|-----------------------------------------------------|----------------------------------------|
| Release date                                        | October 5, 2023                        |
| API Version (SAPI)                                  | 2.8.2 (Build 27)                        |
| Firmware Version                                    | 2.8.2.0.27 (Build 27)                    |
| Package Name                                        | RS9116W.2.8.2.27                        |
| Supported RTOS                                      | FreeRTOS                               |
| Hardware Modules                                    | QMS, B00, C00, CC1, AB0, AB1, AA0, AA1, AC0, AC1 |
| Hardware Chipsets                                   | Chip Revision 1.3, Chip Revision 1.4, Chip Revision 1.5   |
| Operating Modes Supported                           | Wi-Fi STA, Wi-Fi AP, Wi-Fi STA+BLE     |
| Additional Operating Modes Supported (Beta Quality) | Wi-Fi STA+BT, Wi-Fi STA+BT+BLE , BLE(With AE)     |

## Updates in this Release
RS9116W 2.8.2 release consists of two components, as follows:
  - Firmware - RS9116 Firmware Binary
  - SAPI Library - SAPI Library runs on Host

This release is meant only for use with designs based on RS9116 Silicon rev 1.4 (RS9116X-xxx-xxx-Bxx) and RS9116 Silicon rev 1.5 (RS9116X-xxx-xxx-Cxx).   
Customers using the RS9116X-xxx-xxx-Xxx parts (Silicon rev 1.3) can also upgrade to the latest firmware, however, the power optimization feature macro `EXT_FEAT_LOW_POWER_MODE` must be disabled for revision 1.3.


Choose the firmware to match the silicon revision used in your design, as follows:
- RS9116W.2.8.2.0.27.rps
  - RS9116X-xxx-xxx-Xxx (Silicon rev 1.3)
  - RS9116X-xxx-xxx-Bxx (Silicon rev 1.4)
- RS916W.2.8.2.0.27.rps
  - RS9116X-xxx-xxx-Cxx (Silicon rev 1.5)
  - RS916WS00ACxxx

## New Features
### Wi-Fi
  - Added support to configure MQTT Keepalive retries count from the host.
  - Added support for TLS certificate indexing for embedded MQTTS, and embedded HTTPS applications

### System
  - None

### BT-Classic/BLE
  - None

## Changes and Fixes
### Wi-Fi
  - Added support for Wi-Fi auto join when WPA3 personal transition security mode used.
  - Fix added to get a response for rsi_apconf command when AP is configured in ACS mode with 802.11j is enabled.
  - Increased EAP cert size from 12k to 20k when HTTP server is disabled.
  - Added support to set region in AP mode for ACX.
  - Added support to store WPS mode AP credentials to enable auto-join connection without the need for giving the WPS Push button at the AP again.
  - Updated condition to send card ready without GPIO validation during auto join when wake on wireless LAN (WOWLAN) feature is enabled in opermode.
  - Fixed issue with PER mode Rx functionality.
  - AC0,AC1 Certified Gaintables added
  - Additional EVM backoff of ~2dB added for ACx 23 ECN SW Version.
  - Changed lpf cutoff offsets for ETSI blocker requirements for ACx.
  - DPD is Disabled
  - Instead of enabling the PMF bits in the join feature bitmap in rsi_wlan_config.h, these are now run-time configurable for WPA2 & WPA3 transition.

### Network Stack
  - Fixed issues with IPv6 short addressing for HTTP client functionality.
  - Resolved issues with SNTP get time returning static time.

### BT-Classic/BLE
  - Fixed low output TX power issue for AE ADV packets.
  - Fixed less number of AUX_SCAN_REQ packets transmission issue in extended active scanning. 
  - Fixed lesser number of AE ADV events issue.
  - Fixed AE Advertising packet drop issue.
  - Added support to get the state of device in BLE. 
  - Added BTC/BLE rate based TX power support for AC0 & AC1 modules in PER mode for all regions, and this support is added only for FCC region in E2E mode. 

### Coexistence
  - None

### System
  - Fixed issues with rsi_calib_read API.

### Documentation
  - rsi_wlan_ap_start description corrected in SAPI PRM.
  - SAPI PRM application diagrams corrected to display power connection instead of UART between PC and RS9116W module.
  - Added additional info for Power Save Mode 2 in AT PRM
  - Added additional notes for ext_tcp_ip_feature_bit_map[6]
  - Updated documentation for SO_TLS_SNI, SO_RCVTIMEO parameters in rsi_setsockopt API
  - Enhanced description for MQTT keepalive timeout handler usage
  - AT PRM is updated for rsi_calib_write command usage for 1.4 and 1.5 revisions
  - Gain offset updates in NVM is explained for rsi_calib_write API
  - Improved description of socket handling at sapi and firmware

## Known Issues

### Network Stack 
  - Sometimes during a TLS handshake, ECC curve parameters may be incorrectly generated resulting in connection failure with BBD2 error. However, this recovers in the next attempt.
  - MQTT disconnects during firmware update when power save is enabled. Disable power save during firmware update and re-enable on update completion.
  - All GCM-based cipher suites are implemented in software and have limited performance.
  - The recommended MQTT publish payload is 1 kBytes.
  - If HTTP server functionality is enabled, do not use port 80 for the MQTT client.
  - Randomize the client port if using rapid connect/disconnect of the MQTT session on the same client port with powersave.
  - Secure TLS renegotiation is not supported in the embedded networking stack.
  - In WPA2 enterprise security mode getting wireless statistics using rsi_wlan_get() API has restrictions.
  - Example application wlan_throughtput_bt_spp_ble_dual_role is unsupported with bare metal.
  - In concurrent mode, there could be a loss of communication on the STA application, when simultaneously multiple clients are connected to the APUT.
  - IP address conflict detection has limitations in STA + AP concurrent mode, failure is not reported to host.
  - There may be instability with the MQTT connection when MQTT Keep alive period is configured as 0.
  - mqtt_client and mqtt_client_v6 applications don't support SDIO host interface

### Wi-Fi
  - WPA3 Connection with Hunting and Pecking algorithm takes about 3-4 seconds.
  - If the station performs a scan in concurrent mode (Wi-Fi STA+AP), stations connected to the AP may be disconnected. Enable AP after the STA connection is completed.
  - Issue observed with WPA2 Enterprise Connectivity using Microsoft RADIUS Server.
  - PTA 3-wire coexistence does not work with power save. However, if a weak pull up is applied to the GRANT pin, PTA 3-wire functionality will work with power save. Details can be found in the application notes.
  - EAP security in the aws_device_shadow application is unsupported on the EFR platform due to memory constraints.
  - In STA+AP, If AP started first station scan is supported only in channel in which AP is brought up. If multiple channel scan is requested, AP behaviour is undefined.

### BT-Classic/BLE
  - Wake on wireless is not supported for BLE.
  - BT-HID may not interoperate with Apple devices.
  - In Wi-Fi + BT/BLE Co-Ex mode, high Wi-Fi broadcast traffic might cause BT/BLE disconnections.
  - In Wi-Fi + BT mode when there is continuous Wi-Fi data, you might observe BT not re-connecting to the remote after disconnection.
  - For BLE, dual role scenario issues may occur when connecting as a central when the peripheral is advertising. Central connections can be made after peripheral connections are established and advertising stops.
  - A BLE disconnection will occur if both the peer and RS9116 already have bond info and the RS9116 initiates security as part of BLE secure connections. To workaround this issue, the application should not initiate a security request if it already has bond info.
  - A2DP music streaming glitches may be observed if a Wi-Fi download is in progress at the same time.
  - BLE disconnection can happen, when BLE Duty cycling is enabled.
  - RTOS support is not there in ae_peripheral & ae_central applications.
  - AE connection issue is expected with Samsung mobiles. 
  
### Coexistence
  - After power save is enabled in co-ex mode, `radio_init()` must be called to turn off power save.
  - In Wi-Fi + BLE mode:
    - BLE may disconnect when the supervision timeout is configured to be less than 16 seconds.
    - WPS does not work.
  - In Wi-Fi + BLE / BLE only modes:
    - When Wi-Fi disconnects, a BT/BLE reconnection issue is observed.

### Interoperability
  - Disconnection may occur in Wi-Fi client mode if an AP does not acknowledge QoS null frames with power save configured.

### System
  - Chip Revision 1.3 doesn't support Low power mode optimization using the bit setting defined by EXT_FEAT_LOW_POWER_MODE, setting this results in a hang issue.
  - Sometimes the RS9116 enters power save mode before the configured monitor interval for inactivity when co-ex is configured.
  - The FTP client application does not work with EFx32/STM32 when an RTOS is used.
  - The HTTP OTA update application does not work with EFR32 and EFM32 when an RTOS is used.  
  - Firmware update does not work for the EFM32 platform.
  - SAPI driver with UART host interface is supported only on bare metal.
  - Powersave without RAM retention does not work when the SPI interface is used.
  - MQTT Receive may encounter failures in STA + AP concurrent mode.

## Limitations and Unsupported Features

### Wi-Fi/Network Stack
  - AMSDU transmit is not supported.
  - Fragmentation is not supported.
  - AMSDU's within AMPDU is not supported.
  - Currently, the RS9116 does not support the radio measurement requests feature of CCX V2
  - 802.11k is not supported
  - Short GI is not supported
  - 40 MHz bandwidth for both the 2.4 & 5 GHz bands is not supported.
  - 802.11j channels less than 16 are not supported.
  - The USB host interface and USB power save are not supported.
  - In AT mode, the total MQTT command length (apart from MQTT publish) should not exceed `150` bytes. This includes `at+rsi` (start of command) to (end of command)`\r\n`.
  - Configure `MQTT_VERSION` in `rsi_mqtt_client.h` based on the server configuration; only version 3 and 4 are supported.
  - 3xTLS connections or 1xTCP and 2xTLS connection, connections are supported concurrently in Wi-Fi only mode.
  - TLS curve IDs supported are 15-28. TLS handshake with 3rd party clients depends on the TLS curve.
  - Big endian architecture is not supported.
  - Auto PAC Provisioning in EAP-FAST with TLSv1.2 is not supported.
  - RC4_SHA, DES_CBC3_SHA, and RC4_MD5 ciphers are not supported in enterprise security
  - SA query procedure not supported in 11W mode.
  - WPA3 is not supported in AP  / concurrent (AP+STA) modes.
  - If background scan is enabled along with custom feature bit map bit (8) configuration of the DFS channel support, issues may occur in obtaining an IP address for the Wi-Fi STA.
  - In AP mode, IPv6 Router Advertisements are not supported. Although STA connected to the APUT can transfer IPv6 data to the peers by using SLAAC for address configuration.
  - Transparent mode is unsupported in USB-CDC mode.
  - Channel 144 is not supported.

### BT-Classic/BLE
  - BT A2DP is only supported for RS9116 silicon revision 1.5.
  - BT sniff mode does not work if BT multiple slaves feature is enabled.
  - For BLE, if the connection is established with a small connection interval (<15 ms), simultaneous roles are not supported (i.e., Central/Peripheral + Advertising/Scanning).
  - For BT Classic, only 1 connection is supported at any time.
  - BLE slave latency value is valid up to 32 only.
  - BT-A2DP encoder is not supported in the firmware.
  - BT-A2DP is not supported in AT mode.
  - Maximum supported  AE data length is 200 bytes.
  - Supports only two ADV_EXT sets.
  - Supports only one BLE connection with AE.
  - Filtering policy is not supported for Periodic advertising.
  - SMP is not supported in AE applications "ble_ae_central" and "ble_ae_peripheral". 
  - GATT transactions & Data transfer are not supported in AE applications  "ble_ae_central" and "ble_ae_peripheral".  

### Coexistence
  - Wi-Fi STA + BT + BLE multiprotocol use cases require a detailed understanding of use cases and associated configurations. Contact Silicon Labs support for additional details.
  - Wi-Fi AP + BLE, Wi-Fi AP + BT & Wi-Fi AP + BT + BLE modes are not supported.
  - For AT commands, Wi-Fi+BT+BLE( Opermode 9 ), BT+BLE(Opermode 8 ) do not work.
  - In Wi-Fi + BLE mode, if the BLE scan interval and window have the same value, then the Wi-Fi connection may be unsuccessful.
  - Advertising Extention feature is not supported in Coexistence.  

### System
  - Wake on Wireless support has only been tested for the UART AT command interface.

## Notes
### BT-Classic/BLE
 - Whitelist address should be given in reverse order.
 - AE feature validated using 1.5Rev AC1 expansion board.
 - AE feature is not supported in 9116 1.4Rev.
 - Advertising Extentions is Beta quality feature.

# WiSeConnect 2.8.1 Release Notes

Last updated: July 13, 2023

## Highlights

  - New features
  - Bug fixes
  
## Release Details
| Item                                                | Details                                |
|-----------------------------------------------------|----------------------------------------|
| Release date                                        | August 3, 2023                            |
| API Version (SAPI)                                  | 2.8.1 (Build 19)                        |
| Firmware Version                                    | 2.8.1.0.19. (Build 10)                    |
| Package Name                                        | RS9116W.2.8.1.19                        |
| Supported RTOS                                      | FreeRTOS                               |
| Hardware Modules                                    | QMS, B00, C00, CC1, AB0, AB1, AA0, AA1, AC0, AC1 |
| Hardware Chipsets                                   | Chip Revision 1.3, Chip Revision 1.4, Chip Revision 1.5   |
| Operating Modes Supported                           | Wi-Fi STA, Wi-Fi AP, Wi-Fi STA+BLE     |
| Additional Operating Modes Supported (Beta Quality) | Wi-Fi STA+BT, Wi-Fi STA+BT+BLE , BLE(With AE)     |

## Updates in this Release
RS9116W 2.8.1 release consists of two components, as follows:
  - Firmware - RS9116 Firmware Binary
  - SAPI Library - SAPI Library runs on Host

This release is meant only for use with designs based on RS9116 Silicon rev 1.4 (RS9116X-xxx-xxx-Bxx) and RS9116 Silicon rev 1.5 (RS9116X-xxx-xxx-Cxx).   
Customers using the RS9116X-xxx-xxx-Xxx parts (Silicon rev 1.3) can also upgrade to the latest firmware, however, the power optimization feature macro `EXT_FEAT_LOW_POWER_MODE` must be disabled for revision 1.3.


Choose the firmware to match the silicon revision used in your design, as follows:
- RS9116W.2.8.1.0.19.rps
  - RS9116X-xxx-xxx-Xxx (Silicon rev 1.3)
  - RS9116X-xxx-xxx-Bxx (Silicon rev 1.4)
- RS916W.2.8.1.0.19.rps 
  - RS9116X-xxx-xxx-Cxx (Silicon rev 1.5)
  - RS916WS00ACxxx

## New Features
### Wi-Fi
  - None
### System
  - None
### BT-Classic/BLE
  - None
## Changes and Fixes
### Wi-Fi
  - Fixed Background Scan failure issue when the 11J feature is enabled.
  - Fixed EAP connection issue with Store Configuration when Quick Join is enabled.
  - Removed the EAPOL timeout configuration provision to user and fixed it to 10 seconds.
  - Added change to allow Set RTC command only after Opermode command.
  - Added change to avoid sending multiple ADDBA requests for a particular TID if AP is rejecting(Request will not get triggered again for that TID if it is already rejected by AP until new association).
  - Fixed WPS connection issue in station mode.
  - Added fix for GPIO based deep sleep without RAM retention

### Network Stack
 - Resolved issues with MQTT AT commands while using USB-CDC interface.
 - Replaced infinite waiting for card ready with a user configurable timeout.
 - Resolved race conditions with event handling.
 - Added support for asynchronous messages to host in transparent mode.
 - Corrected the error code sent to host when WLAN gets disconnected in transparent mode.

### BT-Classic/BLE
  - Fixed BLE performance issues with HP chain in RS9116 1.3Rev boards
  - Fixed hang issue while sending SPP data packets continuously 
  - Added a new feature to filter duplicate legacy reports during Extended Scanning
  - Added a fix for SMP failed issue when RS9116 is in the peripheral role and the central device does not distribute any keys
  - Added fix for connection issue in Advertising extensions
  - Fixed the auxiliary pointer field information in non-connectable and non-scannable extended advertising 
  - Added fix for sending Auxiliary scan responses when Extended scannable advertisement is enabled
  - Fixed the enabling and disabling of Extended scan in loop issue 
  - Fix made to handle Get_Element_Attribute SAPI Command to retrieve proper response
  - Fixed HID SDP command failure 
  - Added support for BT role switch
  - Fixed BLR disconnection issue
  - Added a check for validating data in legacy and extended advertisement reports
  - Fixed Non-connectable and non-scannable legacy advertising issue
  - Added a fix for SMP failed issue when Passkey is the Pairing method
  - Added fix in the application to resolve extended scannable advertisement issue
  - Added fix in the application for enabling Periodic advertising
  - Fixed the AE connection issue where remote is Central and DUT is Peripheral. 

### Coexistence
  - None

### System
  - Added fix for updating the rsi_driver_cb state in Auto-Join.
  - Added a check for ACTIVE LOW INTERRUPT in rsi_wireless_deint API. 

### Documentation
  - Added a note in PRM mentioning that BIT(3) in ext_tcp_ip_feature_bitmap has to be enabled for Bi-directional data transfer.
  - Removed EAPOL timeout error code(0x73) in SAPI and AT PRM

## Known Issues

### Network Stack 
  - Sometimes during a TLS handshake, ECC curve parameters may be incorrectly generated resulting in connection failure with BBD2 error. However, this recovers in the next attempt.
  - MQTT disconnects during firmware update when power save is enabled. Disable power save during firmware update and re-enable on update completion.
  - All GCM-based cipher suites are implemented in software and have limited performance.
  - The recommended MQTT publish payload is 1 kBytes.
  - If HTTP server functionality is enabled, do not use port 80 for the MQTT client.
  - Randomize the client port if using rapid connect/disconnect of the MQTT session on the same client port with powersave.
  - Secure TLS renegotiation is not supported in the embedded networking stack.
  - In WPA2 enterprise security mode getting wireless statistics using rsi_wlan_get() API has restrictions.
  - Example application wlan_throughtput_bt_spp_ble_dual_role is unsupported with bare metal.
  - In concurrent mode, there could be a loss of communication on the STA application, when simultaneously multiple clients are connected to the APUT.
  - IP address conflict detection has limitations in STA + AP concurrent mode, failure is not reported to host.
  - There may be instability with the MQTT connection when MQTT Keep alive period is configured as 0.
  - HTTP client application doesn't support server IP short addressing for IPv6 in canonical form. User is required to input all octets including the zeros.
  - mqtt_client and mqtt_client_v6 applications don't support SDIO host interface

### Wi-Fi
  - WPA3 Connection with Hunting and Pecking algorithm takes about 3-4 seconds.
  - If the station performs a scan in concurrent mode (Wi-Fi STA+AP), stations connected to the AP may be disconnected. Enable AP after the STA connection is completed.
  - Issue observed with WPA2 Enterprise Connectivity using Microsoft RADIUS Server.
  - PTA 3-wire coexistence does not work with power save. However, if a weak pull up is applied to the GRANT pin, PTA 3-wire functionality will work with power save. Details can be found in the application notes.
  - EAP security in the aws_device_shadow application is unsupported on the EFR platform due to memory constraints.
  - In STA+AP, If AP started first station scan is supported only in channel in which AP is brought up. If multiple channel scan is requested, AP behaviour is undefined.

### BT-Classic/BLE
  - Wake on wireless is not supported for BLE.
  - BT-HID may not interoperate with Apple devices.
  - In Wi-Fi + BT/BLE Co-Ex mode, high Wi-Fi broadcast traffic might cause BT/BLE disconnections.
  - In Wi-Fi + BT mode when there is continuous Wi-Fi data, you might observe BT not re-connecting to the remote after disconnection.
  - For BLE, dual role scenario issues may occur when connecting as a central when the peripheral is advertising. Central connections can be made after peripheral connections are established and advertising stops.
  - A BLE disconnection will occur if both the peer and RS9116 already have bond info and the RS9116 initiates security as part of BLE secure connections. To workaround this issue, the application should not initiate a security request if it already has bond info.
  - A2DP music streaming glitches may be observed if a Wi-Fi download is in progress at the same time.
  - BLE disconnection can happen, when BLE Duty cycling is enabled.
  - Performance issue (packet drops) observed while testing AE feature with 916 AC1 expansion board.
  - DUT is not able to send scan response (AUX_SCAN_RSP) for extended scan request (AUX_SCAN_REQ) in power save.
  - DUT is not using all the primary advertising channels (37, 38, 39) for advertising.
  - Aux pointer field is not present in some packets when DUT is advertising using Non connectable and Non scannable advertising types.
  - Observed DUT is going into hang state after 1 hour 15 mins with Advertising enable and disable in a loop with BLE_MAX_AE_EVNTS = 0x020.
  - Number of Advertising events advertised by DUT is less than the value defined in MAX_ADV_EVENTS.
  - Extended scan enable command is failing with error 0x103 with DUT enabling extended Scan and disabling it in a loop.
  - Less number of scan request (AUX_SCAN_REQ) are sent by DUT when continuous Extended Scan is enabled with scan interval and window to be 0x0030 and 0x0020 respectively for both Primary and Secondary PHY.
  - The time delta between AUX_ADV_IND and AUX_SYNC_IND packets when periodic advertising is enabled is different from the Sync Info field (Calculated Offset) for 2M and LE Coded PHY.
  - RTOS support is not there in ae_peripheral & ae_central applications.
  - Number of Advertising events advertised by DUT are less than the value defined in MAX_ADV_EVENTS.


### Coexistence
  - After power save is enabled in co-ex mode, `radio_init()` must be called to turn off power save.
  - In Wi-Fi + BLE mode:
    - BLE may disconnect when the supervision timeout is configured to be less than 16 seconds.
    - WPS does not work.
  - In Wi-Fi + BLE / BLE only modes:
    - When Wi-Fi disconnects, a BT/BLE reconnection issue is observed.

### Interoperability
  - Disconnection may occur in Wi-Fi client mode if an AP does not acknowledge QoS null frames with power save configured.

### System
  - Chip Revision 1.3 doesn't support Low power mode optimization using the bit setting defined by EXT_FEAT_LOW_POWER_MODE, setting this results in a hang issue.
  - Sometimes the RS9116 enters power save mode before the configured monitor interval for inactivity when co-ex is configured.
  - The FTP client application does not work with EFx32/STM32 when an RTOS is used.
  - The HTTP OTA update application does not work with EFR32 and EFM32 when an RTOS is used.  
  - Firmware update does not work for the EFM32 platform.
  - SAPI driver with UART host interface is supported only on bare metal.
  - Powersave without RAM retention does not work when the SPI interface is used.
  - MQTT Receive may encounter failures in STA + AP concurrent mode.

## Limitations and Unsupported Features

### Wi-Fi/Network Stack
  - AMSDU transmit is not supported.
  - Fragmentation is not supported.
  - AMSDU's within AMPDU is not supported.
  - Currently, the RS9116 does not support the radio measurement requests feature of CCX V2
  - 802.11k is not supported
  - Short GI is not supported
  - 40 MHz bandwidth for both the 2.4 & 5 GHz bands is not supported.
  - 802.11j channels less than 16 are not supported.
  - The USB host interface and USB power save are not supported.
  - In AT mode, the total MQTT command length (apart from MQTT publish) should not exceed `150` bytes. This includes `at+rsi` (start of command) to (end of command)`\r\n`.
  - Configure `MQTT_VERSION` in `rsi_mqtt_client.h` based on the server configuration; only version 3 and 4 are supported.
  - 3xTLS connections or 1xTCP and 2xTLS connection, connections are supported concurrently in Wi-Fi only mode.
  - TLS curve IDs supported are 15-28. TLS handshake with 3rd party clients depends on the TLS curve.
  - Big endian architecture is not supported.
  - Auto PAC Provisioning in EAP-FAST with TLSv1.2 is not supported.
  - RC4_SHA, DES_CBC3_SHA, and RC4_MD5 ciphers are not supported in enterprise security
  - SA query procedure not supported in 11W mode.
  - WPA3 is not supported in AP  / concurrent (AP+STA) modes.
  - If background scan is enabled along with custom feature bit map bit (8) configuration of the DFS channel support, issues may occur in obtaining an IP address for the Wi-Fi STA.
  - In AP mode, IPv6 Router Advertisements are not supported. Although STA connected to the APUT can transfer IPv6 data to the peers by using SLAAC for address configuration.
  - Transparent mode is unsupported in USB-CDC mode.
  - Channel 144 is not supported.

### BT-Classic/BLE
  - BT A2DP is only supported for RS9116 silicon revision 1.5.
  - BT sniff mode does not work if BT multiple slaves feature is enabled.
  - For BLE, if the connection is established with a small connection interval (<15 ms), simultaneous roles are not supported (i.e., Central/Peripheral + Advertising/Scanning).
  - For BT Classic, only 1 connection is supported at any time.
  - BLE slave latency value is valid up to 32 only.
  - BT-A2DP encoder is not supported in the firmware.
  - BT-A2DP is not supported in AT mode.
  - Maximum supported  AE data length is 200 bytes.
  - Supports only two ADV_EXT sets.
  - Supports only one BLE connection with AE.
  - Duplicate filtering for Legacy ADV packets is not supported.
  - Filtering policy is not supported for Periodic advertising.
  - SMP is not supported in AE applications "ble_ae_central" and "ble_ae_peripheral". 
  - GATT transactions & Data transfer are not supported in AE applications  "ble_ae_central" and "ble_ae_peripheral".  

### Coexistence
  - Wi-Fi STA + BT + BLE multiprotocol use cases require a detailed understanding of use cases and associated configurations. Contact Silicon Labs support for additional details.
  - Wi-Fi AP + BLE, Wi-Fi AP + BT & Wi-Fi AP + BT + BLE modes are not supported.
  - For AT commands, Wi-Fi+BT+BLE( Opermode 9 ), BT+BLE(Opermode 8 ) do not work.
  - In Wi-Fi + BLE mode, if the BLE scan interval and window have the same value, then the Wi-Fi connection may be unsuccessful.

### System
  - Wake on Wireless support has only been tested for the UART AT command interface.

## Notes
### BT-Classic/BLE
 - Whitelist address should be given in reverse order.
 - AE feature validated using 1.5Rev AC1 expansion board.
 - AE feature is not supported in 9116 1.4Rev.
 - The users are recommended to use some delay while running continuous SPP data transfer to avoid packet allocation failure errors.
 

# WiSeConnect 2.8 Release Notes

Last updated: May 18, 2023
Last updated: May 18, 2023

## Highlights

  - New features
  - Bug fixes
  
## Release Details
| Item                                                | Details                                |
|-----------------------------------------------------|----------------------------------------|
| Release date                                        | June 7, 2023                            |
| API Version (SAPI)                                  | 2.8.0 (Build 45)                        |
| Firmware Version                                    | 2.8.0.0.45 (Build 45)                    |
| Package Name                                        | RS9116W.2.8.0.45                        |
| Release date                                        | June 7, 2023                            |
| API Version (SAPI)                                  | 2.8.0 (Build 45)                        |
| Firmware Version                                    | 2.8.0.0.45 (Build 45)                    |
| Package Name                                        | RS9116W.2.8.0.45                        |
| Supported RTOS                                      | FreeRTOS                               |
| Hardware Modules                                    | QMS, B00, C00, CC1, AB0, AB1, AA0, AA1, AC0, AC1 |
| Hardware Chipsets                                   | Chip Revision 1.3, Chip Revision 1.4, Chip Revision 1.5   |
| Operating Modes Supported                           | Wi-Fi STA, Wi-Fi AP, Wi-Fi STA+BLE     |
| Additional Operating Modes Supported (Beta Quality) | Wi-Fi STA+BT, Wi-Fi STA+BT+BLE         |


## Updates in this Release
RS9116W 2.8.0 release consists of two components, as follows:
  - Firmware - RS9116 Firmware Binary
  - SAPI Library - SAPI Library runs on Host

This release is meant only for use with designs based on RS9116 Silicon rev 1.4 (RS9116X-xxx-xxx-Bxx) and RS9116 Silicon rev 1.5 (RS9116X-xxx-xxx-Cxx).   
Customers using the RS9116X-xxx-xxx-Xxx parts (Silicon rev 1.3) can also upgrade to the latest firmware, however, the power optimization feature macro `EXT_FEAT_LOW_POWER_MODE` must be disabled for revision 1.3.


Choose the firmware to match the silicon revision used in your design, as follows:
- RS9116W.2.8.0.0.45.rps
- RS9116W.2.8.0.0.45.rps
  - RS9116X-xxx-xxx-Xxx (Silicon rev 1.3)
  - RS9116X-xxx-xxx-Bxx (Silicon rev 1.4)
- RS916W.2.8.0.0.45.rps 
- RS916W.2.8.0.0.45.rps 
  - RS9116X-xxx-xxx-Cxx (Silicon rev 1.5)
  - RS916WS00ACxxx
  - RS916WS00ACxxx

## New Features
### Wi-Fi
  - Added support for the interface to have dual IP i.e. IPv4 and IPv6 addresses simultaneously.
  - Added support for Type of Service (DSCP) configuration for IPv6 Tx data from host.
  - Added support for the interface to have dual IP i.e. IPv4 and IPv6 addresses simultaneously.
  - Added support for Type of Service (DSCP) configuration for IPv6 Tx data from host.
### System
  - None
  - None
### Bluetooth/BLE
- Added Advertising Extensions feature , use "ble_ae_central" & "ble_ae_peripheral" applications to validate this feature.

## Changes and Fixes
### Wi-Fi
  - Added Fix to give EAPOL timeout error to host, if EAPOL is not received from AP within specified time.
  - Added fix to update security type in scan_results response.
  - Added support for autojoin in WPA3.
  - Added Fix to give EAPOL timeout error to host, if EAPOL is not received from AP within specified time.
  - Added fix to update security type in scan_results response.
  - Added support for autojoin in WPA3.

### Network Stack
  - Resolved embedded MQTT issues in concurrent mode (STA + AP).
  - The user is recommended to set EXT_TCP_IP_WAIT_FOR_SOCKET_CLOSE and use socket shutdown to close the TCP socket when intended. This ensures that the last bit of data received from the remote is successfully transferred to the host before socket termination in firmware.
  - Resolved embedded MQTT issues in concurrent mode (STA + AP).
  - The user is recommended to set EXT_TCP_IP_WAIT_FOR_SOCKET_CLOSE and use socket shutdown to close the TCP socket when intended. This ensures that the last bit of data received from the remote is successfully transferred to the host before socket termination in firmware.

### Bluetooth/BLE
  - Fixed the timeout issue for SMP failure.
  - Added support to identify the disconnected remote device type on the BLE link when it is a Public address or Random address.

### Coexistence
  - None

### System
  - Resolved instability issues with MQTT Rx data in concurrent mode (STA + AP).
  - Resolved instability issues with MQTT Rx data in concurrent mode (STA + AP).

### Documentation
  - None

## Known Issues

### Network Stack 
  - Sometimes during a TLS handshake, ECC curve parameters may be incorrectly generated resulting in connection failure with BBD2 error. However, this recovers in the next attempt.
  - MQTT disconnects during firmware update when power save is enabled. Disable power save during firmware update and re-enable on update completion.
  - All GCM-based cipher suites are implemented in software and have limited performance.
  - The recommended MQTT publish payload is 1 kBytes.
  - If HTTP server functionality is enabled, do not use port 80 for the MQTT client.
  - Randomize the client port if using rapid connect/disconnect of the MQTT session on the same client port with powersave.
  - Secure TLS renegotiation is not supported in the embedded networking stack.
  - In WPA2 enterprise security mode getting wireless statistics using rsi_wlan_get() API has restrictions.
  - Example application wlan_throughtput_bt_spp_ble_dual_role is unsupported with bare metal.
  - In concurrent mode, there could be a loss of communication on the STA application, when simultaneously multiple clients are connected to the APUT.
  - IP address conflict detection has limitations in STA + AP concurrent mode, failure is not reported to host.
  - There may be instability with the MQTT connection when MQTT Keep alive period is configured as 0.
  - There may be instability with the MQTT connection when MQTT Keep alive period is configured as 0.

### Wi-Fi
  - WPA3 Connection with Hunting and Pecking algorithm takes about 3-4 seconds.
  - If the station performs a scan in concurrent mode (Wi-Fi STA+AP), stations connected to the AP may be disconnected. Enable AP after the STA connection is completed.
  - Issue observed with WPA2 Enterprise Connectivity using Microsoft RADIUS Server.
  - PTA 3-wire coexistence does not work with power save. However, if a weak pull up is applied to the GRANT pin, PTA 3-wire functionality will work with power save. Details can be found in the application notes.
  - EAP security in the aws_device_shadow application is unsupported on the EFR platform due to memory constraints.
  - In STA+AP, If AP started first station scan is supported only in channel in which AP is brought up. If multiple channel scan is requested, AP behaviour is undefined.
  - In STA+AP, If AP started first station scan is supported only in channel in which AP is brought up. If multiple channel scan is requested, AP behaviour is undefined.

### BT-Classic/BLE
  - Wake on wireless is not supported for BLE.
  - BT-HID may not interoperate with Apple devices.
  - In Wi-Fi + BT/BLE Co-Ex mode, high Wi-Fi broadcast traffic might cause BT/BLE disconnections.
  - In Wi-Fi + BT mode when there is continuous Wi-Fi data, you might observe BT not re-connecting to the remote after disconnection.
  - For BLE, dual role scenario issues may occur when connecting as a central when the peripheral is advertising. Central connections can be made after peripheral connections are established and advertising stops.
  - A BLE disconnection will occur if both the peer and RS9116 already have bond info and the RS9116 initiates security as part of BLE secure connections. To workaround this issue, the application should not initiate a security request if it already has bond info.
  - A2DP music streaming glitches may be observed if a Wi-Fi download is in progress at the same time.
  - BLE disconnection can happen, when BLE Duty cycling is enabled.
  - Performance issue (packet drops) observed while testing AE feature with 916 AC1 expansion board.
  - BLE connection is not working with Connection PHY is 2M in power save.
  - DUT is not able to send scan response(AUX_SCAN_RSP) for extended scan request (AUX_SCAN_REQ) in power save.
  - DUT is not using all the primary advertising channels (37, 38, 39) for advertising.
  - Aux pointer field is not present in some packets when DUT is advertising using Non connectable and Non scannable advertising type.
  - Observed DUT is going into hang state after 1 hour 15 mins with Advertising enable and disable in a loop with BLE_MAX_AE_EVNTS = 0x020.
  - Number of Advertising events advertised by DUT are less than the value defined in MAX_ADV_EVENTS.
  - Extended scan enable command is failing with error 0x103 with DUT enabling extended Scan and disabling it in a loop.
  - Less number of scan request(AUX_SCAN_REQ) are sent by DUT when continuous Extended Scan is enabled with scan interval and window to be 0x0030 and 0x0020 respectively for both Primary and Secondary PHY.
  - The time delta between AUX_ADV_IND and AUX_SYNC_IND packets when periodic advertising is enabled is different from the Sync Info field(Calculated Offset) for 2M and LE Coded PHY.
  - RTOS support is not there in ae_peripheral & ae_central applications.


### Coexistence
  - After power save is enabled in co-ex mode, `radio_init()` must be called to turn off power save.
  - In Wi-Fi + BLE mode:
    - BLE may disconnect when the supervision timeout is configured to be less than 16 seconds.
    - WPS does not work.
  - In Wi-Fi + BLE / BLE only modes:
    - When Wi-Fi disconnects, a BT/BLE reconnection issue is observed.

### Interoperability
  - Disconnection may occur in Wi-Fi client mode if an AP does not acknowledge QoS null frames with power save configured.

### System
  - Chip Revision 1.3 doesn't support Low power mode optimization using the bit setting defined by EXT_FEAT_LOW_POWER_MODE, setting this results in a hang issue.
  - Sometimes the RS9116 enters power save mode before the configured monitor interval for inactivity when co-ex is configured.
  - The FTP client application does not work with EFx32/STM32 when an RTOS is used.
  - The HTTP OTA update application does not work with EFR32 and EFM32 when an RTOS is used.  
  - Firmware update does not work for the EFM32 platform.
  - SAPI driver with UART host interface is supported only on bare metal.
  - Powersave without RAM retention does not work when the SPI interface is used.
  - MQTT Receive may encounter failures in STA + AP concurrent mode.

## Limitations and Unsupported Features

### Wi-Fi/Network Stack
  - AMSDU transmit is not supported.
  - Fragmentation is not supported.
  - AMSDU's within AMPDU is not supported.
  - Currently, the RS9116 does not support the radio measurement requests feature of CCX V2
  - 802.11k is not supported
  - Short GI is not supported
  - 40 MHz bandwidth for both the 2.4 & 5 GHz bands is not supported.
  - 802.11j channels less than 16 are not supported.
  - The USB host interface and USB power save are not supported.
  - In AT mode, the total MQTT command length (apart from MQTT publish) should not exceed `150` bytes. This includes `at+rsi` (start of command) to (end of command)`\r\n`.
  - Configure `MQTT_VERSION` in `rsi_mqtt_client.h` based on the server configuration; only version 3 and 4 are supported.
  - 3xTLS connections or 1xTCP and 2xTLS connection, connections are supported concurrently in Wi-Fi only mode.
  - TLS curve IDs supported are 15-28. TLS handshake with 3rd party clients depends on the TLS curve.
  - Big endian architecture is not supported.
  - Auto PAC Provisioning in EAP-FAST with TLSv1.2 is not supported.
  - RC4_SHA, DES_CBC3_SHA, and RC4_MD5 ciphers are not supported in enterprise security
  - SA query procedure not supported in 11W mode.
  - WPA3 is not supported in AP  / concurrent (AP+STA) modes.
  - If background scan is enabled along with custom feature bit map bit (8) configuration of the DFS channel support, issues may occur in obtaining an IP address for the Wi-Fi STA.
  - In AP mode, IPv6 Router Advertisements are not supported. Although STA connected to the APUT can transfer IPv6 data to the peers by using SLAAC for address configuration.
  - Transparent mode is unsupported in USB-CDC mode.
  - Transparent mode is unsupported in USB-CDC mode.

### BT-Classic/BLE
  - BT A2DP is only supported for RS9116 silicon revision 1.5.
  - BT sniff mode does not work if BT multiple slaves feature is enabled.
  - For BLE, if the connection is established with a small connection interval (<15 ms), simultaneous roles are not supported (i.e., Central/Peripheral + Advertising/Scanning).
  - For BT Classic, only 1 connection is supported at any time.
  - BLE slave latency value is valid up to 32 only.
  - BT-A2DP encoder is not supported in the firmware.
  - BT-A2DP is not supported in AT mode.
  - Maximum supported  AE data length is 200 bytes.
  - Supports only two ADV_EXT sets.
  - Supports only one BLE connection with AE.
  - Filtering policy is not supported for Periodic advertising.
  - SMP is not supported in AE applications "ble_ae_central" and "ble_ae_peripheral". 
  - GATT transactions & Data transfer are not supported in AE applications  "ble_ae_central" and "ble_ae_peripheral".
  - BT role switch is unsupported  

### Coexistence
  - Wi-Fi STA + BT + BLE multiprotocol use cases require a detailed understanding of use cases and associated configurations. Contact Silicon Labs support for additional details.
  - Wi-Fi AP + BLE, Wi-Fi AP + BT & Wi-Fi AP + BT + BLE modes are not supported.
  - For AT commands, Wi-Fi+BT+BLE( Opermode 9 ), BT+BLE(Opermode 8 ) do not work.
  - In Wi-Fi + BLE mode, if the BLE scan interval and window have the same value, then the Wi-Fi connection may be unsuccessful.

### System
  - Wake on Wireless support has only been tested for the UART AT command interface.

## Notes
### BT/BLE
 - Whitelist address should be given in reverse order.
 - AE feature validated using 1.5Rev AC1 expansion board.
 - AE feature is not supported in 9116 1.4Rev.
 - The users are recommended to use some delay while running continuous SPP data transfer to avoid packet allocation failure errors.

# WiSeConnect 2.7.3 Release Notes

Last updated: April 13, 2023

## Highlights

  - New features
  - Bug fixes
  
## Release Details
| Item                                                | Details                                |
|-----------------------------------------------------|----------------------------------------|
| Release date                                        | May 3, 2023                            |
| API Version (SAPI)                                  | 2.7.3 (Build 7)                        |
| Firmware Version                                    | 2.7.3.0.7 (Build 7)                    |
| Package Name                                        | RS9116W.2.7.3.7                        |
| Supported RTOS                                      | FreeRTOS                               |
| Hardware Modules                                    | QMS, B00, C00, CC1, AB0, AB1, AA0, AA1, AC0, AC1 |
| Hardware Chipsets                                   | Chip Revision 1.3, Chip Revision 1.4, Chip Revision 1.5   |
| Operating Modes Supported                           | Wi-Fi STA, Wi-Fi AP, Wi-Fi STA+BLE     |
| Additional Operating Modes Supported (Beta Quality) | Wi-Fi STA+BT, Wi-Fi STA+BT+BLE         |


## Updates in this Release
RS9116W 2.7.3 release consists of two components, as follows:
  - Firmware - RS9116 Firmware Binary
  - SAPI Library - SAPI Library runs on Host

This release is meant only for use with designs based on RS9116 Silicon rev 1.4 (RS9116X-xxx-xxx-Bxx) and RS9116 Silicon rev 1.5 (RS9116X-xxx-xxx-Cxx).   
Customers using the RS9116X-xxx-xxx-Xxx parts (Silicon rev 1.3) can also upgrade to the latest firmware, however, the power optimization feature macro `EXT_FEAT_LOW_POWER_MODE` must be disabled for revision 1.3.


Choose the firmware to match the silicon revision used in your design, as follows:
- RS9116W.2.7.3.0.7.rps
  - RS9116X-xxx-xxx-Xxx (Silicon rev 1.3)
  - RS9116X-xxx-xxx-Bxx (Silicon rev 1.4)
- RS916W.2.7.3.0.7.rps 
  - RS9116X-xxx-xxx-Cxx (Silicon rev 1.5)

## New Features
### Wi-Fi 

### System

## Changes and Fixes
### Wi-Fi
- Fixed TCP data miss for the UART interface.
- Fixed instability issues with trans mode when the command is provided multiple times.

### Network Stack
- Resolved issues with AT mode commands while using the USB-CDC host interface.
- Added inet6 utility to convert IPv6 canonical to hexadecimal form and vice-versa.

### Bluetooth/BLE
- Implemented a new event to know the remote device's supported version in BLE.
- Implemented a SAPI command to reject the SMAP paring request which received from the remote device.
- Resolved BT and BLE user gain table issues in PER mode.
- Fixed BT hang issue when opermode (0x9) command is given.

### Coexistence
- Fixed Wi-Fi HTTPS downloading issue, during continuous BLE connection & disconnection.
- rsi_wireless_init graciously throws error (0xFF2C) when memory is insufficient for the requested features.

### System
- Resolved issue of "--!>"(string) seen on webpage.
- Resolved issues with rsi_bl_power_cycle API for POC_IN implementation.
- Fixed memory leak in rsi_driver_deinit API by ensuring the memory allocated for socket semaphores is freed.

### Documentation

## Known Issues

### Network Stack 
  - Sometimes during a TLS handshake, ECC curve parameters may be incorrectly generated resulting in connection failure with BBD2 error. However, this recovers in the next attempt.
  - MQTT disconnects during firmware update when power save is enabled. Disable power save during firmware update and re-enable on update completion.
  - All GCM-based cipher suites are implemented in software and have limited performance.
  - The recommended MQTT publish payload is 1 kBytes.
  - If HTTP server functionality is enabled, do not use port 80 for the MQTT client.
  - Randomize the client port if using rapid connect/disconnect of the MQTT session on the same client port with powersave.
  - Secure TLS renegotiation is not supported in the embedded networking stack.
  - In IPv6, the type of service selection is unsupported.
  - In WPA2 enterprise security mode getting wireless statistics using rsi_wlan_get() API has restrictions.
  - Example application wlan_throughtput_bt_spp_ble_dual_role is unsupported with bare metal.
  - In concurrent mode, there could be a loss of communication on the STA application, when simultaneously multiple clients are connected to the APUT.
  - IP address conflict detection has limitations in STA + AP concurrent mode, failure is not reported to host.

### Wi-Fi
  - WPA3 Connection with Hunting and Pecking algorithm takes about 3-4 seconds.
  - If the station performs a scan in concurrent mode (Wi-Fi STA+AP), stations connected to the AP may be disconnected. Enable AP after the STA connection is completed.
  - Issue observed with WPA2 Enterprise Connectivity using Microsoft RADIUS Server.
  - PTA 3-wire coexistence does not work with power save. However, if a weak pull up is applied to the GRANT pin, PTA 3-wire functionality will work with power save. Details can be found in the application notes.
  - EAP security in the aws_device_shadow application is unsupported on the EFR platform due to memory constraints.

### BT-Classic/BLE
  - Wake on wireless is not supported for BLE.
  - BT-HID may not interoperate with Apple devices.
  - In Wi-Fi + BT/BLE Co-Ex mode, high Wi-Fi broadcast traffic might cause BT/BLE disconnections.
  - In Wi-Fi + BT mode when there is continuous Wi-Fi data, you might observe BT not re-connecting to the remote after disconnection.
  - For BLE, dual role scenario issues may occur when connecting as a central when the peripheral is advertising. Central connections can be made after peripheral connections are established and advertising stops.
  - A BLE disconnection will occur if both the peer and RS9116 already have bond info and the RS9116 initiates security as part of BLE secure connections. To workaround this issue, the application should not initiate a security request if it already has bond info.
  - A2DP music streaming glitches may be observed if a Wi-Fi download is in progress at the same time.
  - BLE disconnection can happen, when BLE Duty cycling is enabled.

### Coexistence
  - After power save is enabled in co-ex mode, `radio_init()` must be called to turn off power save.
  - In Wi-Fi + BLE mode:
    - BLE may disconnect when the supervision timeout is configured to be less than 16 seconds.
    - WPS does not work.
  - In Wi-Fi + BLE / BLE only modes:
    - When Wi-Fi disconnects, a BT/BLE reconnection issue is observed.

### Interoperability
  - Disconnection may occur in Wi-Fi client mode if an AP does not acknowledge QoS null frames with power save configured.

### System
  - Chip Revision 1.3 doesn't support Low power mode optimization using the bit setting defined by EXT_FEAT_LOW_POWER_MODE, setting this results in a hang issue.
  - Sometimes the RS9116 enters power save mode before the configured monitor interval for inactivity when co-ex is configured.
  - The FTP client application does not work with EFx32/STM32 when an RTOS is used.
  - The HTTP OTA update application does not work with EFR32 and EFM32 when an RTOS is used.  
  - Firmware update does not work for the EFM32 platform.
  - SAPI driver with UART host interface is supported only on bare metal.
  - Powersave without RAM retention does not work when the SPI interface is used.
  - MQTT Receive may encounter failures in STA + AP concurrent mode.

## Limitations and Unsupported Features

### Wi-Fi/Network Stack
  - AMSDU transmit is not supported.
  - Fragmentation is not supported.
  - AMSDU's within AMPDU is not supported.
  - Currently, the RS9116 does not support the radio measurement requests feature of CCX V2
  - 802.11k is not supported
  - Short GI is not supported
  - 40 MHz bandwidth for both the 2.4 & 5 GHz bands is not supported.
  - 802.11j channels less than 16 are not supported.
  - The USB host interface and USB power save are not supported.
  - In AT mode, the total MQTT command length (apart from MQTT publish) should not exceed `150` bytes. This includes `at+rsi` (start of command) to (end of command)`\r\n`.
  - Configure `MQTT_VERSION` in `rsi_mqtt_client.h` based on the server configuration; only version 3 and 4 are supported.
  - 3xTLS connections or 1xTCP and 2xTLS connection, connections are supported concurrently in Wi-Fi only mode.
  - TLS curve IDs supported are 15-28. TLS handshake with 3rd party clients depends on the TLS curve.
  - Big endian architecture is not supported.
  - Auto PAC Provisioning in EAP-FAST with TLSv1.2 is not supported.
  - RC4_SHA, DES_CBC3_SHA, and RC4_MD5 ciphers are not supported in enterprise security
  - SA query procedure not supported in 11W mode.
  - WPA3 is not supported in AP  / concurrent (AP+STA) modes.
  - If background scan is enabled along with custom feature bit map bit (8) configuration of the DFS channel support, issues may occur in obtaining an IP address for the Wi-Fi STA.
  - In AP mode, IPv6 Router Advertisements are not supported. Although STA connected to the APUT can transfer IPv6 data to the peers by using SLAAC for address configuration.

### BT/BLE
  - BT A2DP is only supported for RS9116 silicon revision 1.5.
  - BT sniff mode does not work if BT multiple slaves feature is enabled.
  - For BLE, if the connection is established with a small connection interval (<15 ms), simultaneous roles are not supported (i.e., Central/Peripheral + Advertising/Scanning).
  - For BT Classic, only 1 connection is supported at any time.
  - BLE slave latency value is valid up to 32 only.
  - BT-A2DP encoder is not supported in the firmware.
  - BT-A2DP is not supported in AT mode.

### Coexistence
  - Wi-Fi STA + BT + BLE multiprotocol use cases require a detailed understanding of use cases and associated configurations. Contact Silicon Labs support for additional details.
  - Wi-Fi AP + BLE, Wi-Fi AP + BT & Wi-Fi AP + BT + BLE modes are not supported.
  - For AT commands, Wi-Fi+BT+BLE( Opermode 9 ), BT+BLE(Opermode 8 ) do not work.
  - In Wi-Fi + BLE mode, if the BLE scan interval and window have the same value, then the Wi-Fi connection may be unsuccessful.

### System
  - Wake on Wireless support has only been tested for the UART AT command interface.

# WiSeConnect 2.7.1 Release Notes

Last updated: March 1, 2023

## Highlights

  - New features
  - Bug fixes
  
## Release Details
| Item                                                | Details                                |
|-----------------------------------------------------|----------------------------------------|
| Release date                                        | March 16, 2023                         |
| API Version (SAPI)                                  | 2.7.1 (Build 6)                        |
| Firmware Version                                    | 2.7.1.0.6 (Build 6)                    |
| Package Name                                        | RS9116W.2.7.1.6                        |
| Supported RTOS                                      | FreeRTOS                               |
| Hardware Modules                                    | QMS, B00, C00, CC1, AB0, AB1, AA0, AA1 |
| Hardware Chipsets                                   | Chip Revision 1.3, Chip Revision 1.4, Chip Revision 1.5   |
| Operating Modes Supported                           | Wi-Fi STA, Wi-Fi AP, Wi-Fi STA+BLE     |
| Additional Operating Modes Supported (Beta Quality) | Wi-Fi STA+BT, Wi-Fi STA+BT+BLE         |


## Updates in this Release
RS9116W 2.7.1 release consists of two components, as follows:
  - Firmware - RS9116 Firmware Binary
  - SAPI Library - SAPI Library runs on Host

This release is meant only for use with designs based on RS9116 Silicon rev 1.4 (RS9116X-xxx-xxx-Bxx) and RS9116 Silicon rev 1.5 (RS9116X-xxx-xxx-Cxx).   
Customers using the RS9116X-xxx-xxx-Xxx parts (Silicon rev 1.3) can also upgrade to the latest firmware, however, the power optimization feature macro `EXT_FEAT_LOW_POWER_MODE` must be disabled for revision 1.3.


Choose the firmware to match the silicon revision used in your design, as follows:
- RS9116W.2.7.1.0.6.rps
  - RS9116X-xxx-xxx-Xxx (Silicon rev 1.3)
  - RS9116X-xxx-xxx-Bxx (Silicon rev 1.4)
- RS916W.2.7.1.0.6.rps 
  - RS9116X-xxx-xxx-Cxx (Silicon rev 1.5)

## New Features
### Wi-Fi
- Added a feature to hide sensitive information like Wi-Fi passwords, PMK, and EAP credentials.

## Changes and Fixes
### Wi-Fi
- 11b tx evm improved for AC0 & AC1 modules.
- Added fix in auto rate functionality to support 9mbps
- Added fix for bgscan not working properly for AC0 and AC1 modules
- Added fix for giving proper certificate parsing error codes in async messages of EAP connection
- Added fix for WPA3 connection failure, if AP sends 3-AKM suites
- Added fix for scan failure in channel 140 in EU and JP regions
- Added fix for enabling antenna select option in autojoin
- Resolved firmware hang issue when LTCP sockets are used in transparent mode.
- Added change to ensure TCP disconnection occurs graciously by sending FIN-ACK

### Bluetooth
- Fixed SPP TX issue, module can transfer data continuously without adding delay in between transmissions.

### System
- Fixed rsi_bl_module_power_cycle API to toggle POC_IN pin as well as reset line, during the execution of  rsi_device_init API.  This requires the user to define ENABLE_POC_IN_TOGGLE to be enabled.
- Fixed memory leak in rsi_device_deinit API by ensuring the memory allocated for socket semaphores is freed.
- Cleaned rsi_tx_event_handler by removing dead code.
- Fixed logging initialization sequence in rsi_driver_init API.
- Corrected sapi driver priority in example applications to be higher than other tasks priorities.
- Resolved feature unsupported error when the rsi_wlan_get() API is called to get the Wireless Statistics(RSI_WLAN_GET_STATS) in WPA2 Enterprise Mode

### Documentation
- Added documentation for FEAT_HIDE_PSK_CREDENTIALS bit in feature bitmap.
- Enhanced documentation for EXT_FEAT_RSA_KEY_WITH_4096_SUPPORTbit in ext_custom_feature_bit_map.
- Enhanced documentation for sapi logging feature.

## Known Issues

### Network Stack 
  - Sometimes during a TLS handshake, ECC curve parameters may be incorrectly generated resulting in connection failure with BBD2 error. However, this recovers in the next attempt.
  - MQTT disconnects during firmware update when power save is enabled. Disable power save during firmware update and re-enable on update completion.
  - All GCM-based cipher suites are implemented in software and have limited performance.
  - The recommended MQTT publish payload is 1 kBytes.
  - If HTTP server functionality is enabled, do not use port 80 for the MQTT client.
  - Randomize the client port if using rapid connect/disconnect of the MQTT session on the same client port with powersave.
  - Secure TLS renegotiation is not supported in the embedded networking stack.
  - In IPv6, the type of service selection is unsupported.
  - Continuous data transmission using WebSocket over SSL is inconsistent.
  - in WPA2 enterprise security mode getting wireless statistics using rsi_wlan_get() API has restrictions.
  - Example application wlan_throughtput_bt_spp_ble_dual_role is unsupported with bare metal.
  - In concurrent mode, there could be a loss of communication on the STA application, when simultaneously multiple clients are connected to the APUT.

### Wi-Fi
  - WPA3 Connection with Hunting and Pecking algorithm takes about 3-4 seconds.
  - If the station performs a scan in concurrent mode (Wi-Fi STA+AP), stations connected to the AP may be disconnected. Enable AP after the STA connection is completed.
  - Issue observed with WPA2 Enterprise Connectivity using Microsoft RADIUS Server.
  - PTA 3-wire coexistence does not work with power save. However, if a weak pull up is applied to the GRANT pin, PTA 3-wire functionality will work with power save. Details can be found in the application notes.
  - EAP security in the aws_device_shadow application is unsupported on the EFR platform due to memory constraints.

### BT-Classic/BLE
  - Wake on wireless is not supported for BLE.
  - BT-HID may not interoperate with Apple devices.
  - In Wi-Fi + BT/BLE Co-Ex mode, high Wi-Fi broadcast traffic might cause BT/BLE disconnections.
  - In Wi-Fi + BT mode when there is continuous Wi-Fi data, you might observe BT not re-connecting to the remote after disconnection.
  - For BLE, dual role scenario issues may occur when connecting as a central when the peripheral is advertising. Central connections can be made after peripheral connections are established and advertising stops.
  - A BLE disconnection will occur if both the peer and RS9116 already have bond info and the RS9116 initiates security as part of BLE secure connections. To workaround this issue, the application should not initiate a security request if it already has bond info.
  - A2DP music streaming glitches may be observed if a Wi-Fi download is in progress at the same time.
  - In BT/BLE PER mode, user gain tables may not work as expected.

### Coexistence
  - After power save is enabled in co-ex mode, `radio_init()` must be called to turn off power save.
  - In Wi-Fi + BLE mode:
    - BLE may disconnect when the supervision timeout is configured to be less than 16 seconds.
    - WPS does not work.
  - In Wi-Fi + BLE / BLE only modes:
    - When Wi-Fi disconnects, a BT/BLE reconnection issue is observed.

### Interoperability
  - Disconnection may occur in Wi-Fi client mode if an AP does not acknowledge QoS null frames with power save configured.

### System
  - Chip Revision 1.3 doesn't support Low power mode optimization using the bit setting defined by EXT_FEAT_LOW_POWER_MODE, setting this results in a hang issue.
  - Sometimes the RS9116 enters power save mode before the configured monitor interval for inactivity when co-ex is configured.
  - The FTP client application does not work with EFx32/STM32 when an RTOS is used.
  - The HTTP OTA update application does not work with EFR32 and EFM32 when an RTOS is used.  
  - Firmware update does not work for the EFM32 platform.
  - SAPI driver with UART host interface is supported only on bare metal.
  - Powersave without RAM retention does not work when the SPI interface is used.

## Limitations and Unsupported Features

### Wi-Fi/Network Stack
  - AMSDU transmit is not supported.
  - Fragmentation is not supported.
  - AMSDU's within AMPDU is not supported.
  - Currently, the RS9116 does not support the radio measurement requests feature of CCX V2
  - 802.11k is not supported
  - Short GI is not supported
  - 40 MHz bandwidth for both the 2.4 & 5 GHz bands is not supported.
  - 802.11j channels less than 16 are not supported.
  - The USB host interface and USB power save are not supported.
  - In AT mode, the total MQTT command length (apart from MQTT publish) should not exceed `150` bytes. This includes `at+rsi` (start of command) to (end of command)`\r\n`.
  - Configure `MQTT_VERSION` in `rsi_mqtt_client.h` based on the server configuration; only version 3 and 4 are supported.
  - 3xTLS connections or 1xTCP and 2xTLS connection, connections are supported concurrently in Wi-Fi only mode.
  - TLS curve IDs supported are 15-28. TLS handshake with 3rd party clients depends on the TLS curve.
  - Big endian architecture is not supported.
  - Auto PAC Provisioning in EAP-FAST with TLSv1.2 is not supported.
  - RC4_SHA, DES_CBC3_SHA, and RC4_MD5 ciphers are not supported in enterprise security
  - SA query procedure not supported in 11W mode.
  - WPA3 is not supported in AP  / concurrent (AP+STA) modes.
  - If background scan is enabled along with custom feature bit map bit (8) configuration of the DFS channel support, issues may occur in obtaining an IP address for the Wi-Fi STA

### BT-Classic/BLE
  - BT A2DP is only supported for RS9116 silicon revision 1.5.
  - BT sniff mode does not work if BT multiple slaves feature is enabled.
  - For BLE, if the connection is established with a small connection interval (<15 ms), simultaneous roles are not supported (i.e., Central/Peripheral + Advertising/Scanning).
  - For BT Classic, only 1 connection is supported at any time.
  - BLE slave latency value is valid up to 32 only.
  - BT-A2DP encoder is not supported in the firmware.
  - BT-A2DP is not supported in AT mode.

### Coexistence
  - Wi-Fi STA + BT + BLE multiprotocol use cases require a detailed understanding of use cases and associated configurations. Contact Silicon Labs support for additional details.
  - Wi-Fi AP + BLE, Wi-Fi AP + BT & Wi-Fi AP + BT + BLE modes are not supported.
  - For AT commands, Wi-Fi+BT+BLE( Opermode 9 ), BT+BLE(Opermode 8 ) do not work.
  - In Wi-Fi + BLE mode, if BLE scan interval and window have the same value, then Wi-Fi connection may be unsuccessful.

### System
  - Wake on Wireless support has only been tested for the UART AT command interface.

# WiSeConnect 2.7 Release Notes

## Index
  - [Highlights](#highlights)
  - [Release Details](#release-details)
  - [Updating to this Release](#updating-to-this-release)
  - [New Features](#new-features)
  - [Changes and Fixes](#changes-and-fixes)
  - [Recommendations and New Options](#recommendations-and-new-options)
  - [Known Issues](#known-issues)
  - [Limitations and Unsupported Features](#limitations-and-unsupported-features)
  - [SDK Refactor](#sdk-refactor)
  - [Terminology](#terminology)

## Highlights
  - Bug Fixes. 

## Release Details
| Item                                                | Details                                |
|-----------------------------------------------------|----------------------------------------|
| Release date                                        | December 6, 2022                       |
| API Version (SAPI)                                  | 2.7.0 (Build 39)                       |
| Firmware Version                                    | 2.7.0.0.39 (Build 39)                       |
| Package Name                                        | RS9116W.2.7.0.39                       |
| Supported RTOS                                      | FreeRTOS                               |
| Hardware Modules                                    | QMS, B00, C00, CC1, AB0, AB1, AA0, AA1 | 
| Hardware Chipsets                                   | Chip Revision 1.4, Chip Revision 1.5   |
| Operating Modes Supported                           | Wi-Fi STA, Wi-Fi AP, Wi-Fi STA+BLE     |
| Additional Operating Modes Supported (Beta Quality) | Wi-Fi STA+BT, Wi-Fi STA+BT+BLE         |


## Updates in this Release
RS9116W 2.7 release consists of two components, as follows:
  - Firmware - RS9116 Firmware Binary
  - SAPI Library - SAPI Library runs on Host

This release is meant only for use with designs based on RS9116 Silicon rev 1.4 (RS9116X-xxx-xxx-Bxx) and RS9116 Silicon rev 1.5 (RS9116X-xxx-xxx-Cxx). Customers using the RS9116X-xxx-xxx-Xxx parts (Silicon rev 1.3) can also upgrade to the latest firmware, however the power optimization feature macro `EXT_FEAT_LOW_POWER_MODE` must be disabled for revision 1.3.


Choose the firmware to match the silicon revision used in your design, as follows:
- RS9116W.2.7.0.0.39.rps
  - RS9116X-xxx-xxx-Xxx (Silicon rev 1.3)
  - RS9116X-xxx-xxx-Bxx (Silicon rev 1.4)
- RS916W.2.7.0.0.39.rps 
  - RS9116X-xxx-xxx-Cxx (Silicon rev 1.5)

This release includes bug-fixes, enhancements, relevant AT command updates, new SAPI features, and firmware. Most of the new features have associated APIs, which are available in the latest SAPI implementation only. It is strongly recommended to upgrade SAPI and Firmware together.

## New Features
- IPv6 support is added in station and AP modes.
- Simultaneous STA + AP support is added.
- CCA disable support added in PER mode.
- RvR enhancements are done.

## Changes and Fixes
  - For CC0/CC1 Modules, Corrected the transmit power approximately 2dB to fix an issue. This fix will not affect any regulatory requirements. 

### Wi-Fi/Network Stack
- Fixed device hang issue when rsi_wireless_init() API is called with features that result in memory insufficiency in firmware, now it throws an error indicating the same.
- Resolved instability issues with rsi_set_certificate API, when called multiple times.
- Fixed network disconnection issues in the concurrent mode for peer stations.
- Resolved instability issues with SNTP get time feature by adding a configured number of retries.
- Resolved inconsistency with TCP keepalive message to the server.
- Added Fix for EAP disconnection issue during reauthentication.
- Added Fix for EAP-FAST connectivity failure.
- Added changes to notify users of different certificate validation failures in EAP. The change is supported for both TCP/IP stack and bypass mode on 1.5 silicon. However, on 1.4 silicon it is supported only in TCP/IP bypass mode.
- Added changes to notify user for deauth/disassoc reason codes received from AP.
- Added support to configure the ciphers from user for EAP connection.
- Added configuration parameter to mandate the CA certificate for EAP-PEAP connection.
- Added changes to disable region selection from user for RS916AC0 and RS916AC1 modules.
- Added fix for issue in using TXOP value for V0 and VI access categories.
- Added fix in handling unprotected disassoc/deauth frame in PMF(11W).
- Added fix for connection failure with wrong password in STA+AP mode.
- Added fix for occasional scan failure in STA+AP mode.
- Added changes to support to start STA/AP in any order in STA+AP mode.
- Resolved issues with TCP FIN-ACK handshake.

### Bluetooth
- Fix for BLE controller not responding when we run the BLE connect/disconnect script.
- Fix for module returns the improper remote device MTU size in the rsi_bt_app_on_spp_connect callback, sending an over limit error to host when SPP TX length is exceeds the limit.
- Fix for BLE connection device state is updating as 12 instead of 8 after the successful connection. Cleared BLE initiation state properly after connection.
- Fix for Lack of Connection Event / Mobile Device BT Connection Failure when the Power Index is more than 70.

## Recommendations and New Options

### Wi-Fi
  - NA

### BT-Classic/BLE
  - NA

### Co-Existence
  - Recommemded to use soc clock upto 120MHz only (CUSTOM_FEAT_SOC_CLK_CONFIG_120MHZ). At 160MHz observing BLE disconnection while running WLAN + BLE. 

### Documentation
  - Method to identify chip revision (1.4 or 1.5 etc) has been added.
  - Enhanced description of parameters related to wlan_rf_test example application.
  - Modified description of PER modes.

## Known Issues

### Network Stack 
  - Sometimes during a TLS handshake, ECC curve parameters may be incorrectly generated resulting in connection failure with BBD2 error. However, this recovers in the next attempt.
  - MQTT disconnects during firmware update when power save is enabled. Disable power save during firmware update and re-enable on update completion.
  - All GCM-based cipher suites are implemented in software and have limited performance.
  - The recommended MQTT publish payload is 1 kBytes.
  - If HTTP server functionality is enabled, do not use port 80 for the MQTT client.
  - Randomize the client port if using rapid connect/disconnect of the MQTT session on the same client port with powersave.
  - Secure TLS renegotiation is not supported in the embedded networking stack.
  - In IPv6, type of service selection is unsupported.
  - Continuous data transmission using WebSocket over SSL is inconsistent.
  - in WPA2 enterprise security mode getting wireless statistics using rsi_wlan_get() API has restrictions.
  - Example application wlan_throughtput_bt_spp_ble_dual_role is unsupported with baremetal.
  - In concurrent mode, there could be a loss of communication on the STA application, when simultaneously multiple clients are connected to the APUT.

### Wi-Fi
  - WPA3 Connection with Hunting and Pecking algorithm takes about 3-4 seconds.
  - If the station performs a scan in concurrent mode (Wi-Fi STA+AP), stations connected to the AP may be disconnected. Enable AP after STA connection is completed.
  - Issue observed with WPA2 Enterprise Connectivity using Microsoft RADIUS Server.
  - In fast PSP, configuration socket close is observed in full duplex communication in long duration tests.
  - PTA 3-wire coexistence does not work with powersave. However, if a weak pull up is applied to the GRANT pin, PTA 3-wire functionality will work with powersave. Details can be found in application notes.
  - EAP security in the aws_device_shadow application is unsupported on the EFR platform due to memory constraints.
  - EXT_FEAT_RSA_KEY_WITH_4096_SUPPORT bit of ext_custom_feature_bitmap is not valid in SAPI applications

### BT-Classic/BLE
  - Wake on wireless is not supported for BLE.
  - BT-HID may not interoperate with Apple devices.
  - In Wi-Fi + BT/BLE Co-Ex mode, high Wi-Fi broadcast traffic might cause BT/BLE disconnections.
  - In Wi-Fi + BT mode when there is continuous Wi-Fi data, you might observe BT not re-connecting to remote after disconnection.
  - For BLE, dual role scenario issues may occur when connecting as a central when the peripheral is advertising. Central connections can be made after peripheral connections are established and advertising stops.
  - A BLE disconnection will occur if both the peer and RS9116 already have bond info and the RS9116 initiates security as part of BLE secure connections. To workaround this issue, the application should not initiate a security request if it already has bond info.
  - A2DP music streaming glitches may be observed if a Wi-Fi download is in progress at the same time.
  - BT SPP Tx data transfer may fail if you send packets back to back in a loop. It will work fine if you send packets with small delay.
  - In BT/BLE PER mode, user gain tables may not work as expected.

### Coexistence
  - After powersave is enabled in co-ex mode, `radio_init()` must be called to turn off powersave.
  - In Wi-Fi + BLE mode:
    - BLE may disconnect when the supervision timeout is configured to less than 16 seconds.
    - WPS does not work.
  - In Wi-Fi + BLE / BLE only modes:
    - When Wi-Fi disconnects, a BT/BLE reconnection issue is observed.

### Interoperability
  - Disconnection may occur in Wi-Fi client mode if an AP does not acknowledge QoS null frames with powersave configured.

### System
  - Chip Revision 1.3 doesn't support Low power mode optimization using bit setting defined by EXT_FEAT_LOW_POWER_MODE, setting this results in hang issue.
  - Sometimes the RS9116 enters powersave mode before the configured monitor interval for inactivity when co-ex is configured.
  - Wake on Wireless support has only been tested for the UART AT command interface.
  - Powersave without RAM retention does not work when the SPI interface is used.
  - The FTP client application does not work with EFx32/STM32 when an RTOS is used.
  - The HTTP OTA update application does not work with EFR32 and EFM32 when an RTOS is used.  
  - Firmware update does not work for the EFM32 platform.
  - SAPI driver with UART host interface is supported only on baremetal.

## Limitations and Unsupported Features

### Wi-Fi/Network Stack
  - AMSDU transmit is not supported.
  - Fragmentation is not supported.
  - AMSDU's within AMPDU is not supported.
  - Currently, the RS9116 does not support the radio measurement requests feature of CCX V2
  - 802.11k is not supported
  - Short GI is not supported
  - 40 MHz bandwidth for both the 2.4 & 5 GHz band is not supported.
  - 802.11j channels less than 16 are not supported.
  - The USB host interface and USB powersave are not supported.
  - In AT mode, the total MQTT command length (apart from MQTT publish) should not exceed `150` bytes. This includes `at+rsi` (start of command) to (end of command)`\r\n`.
  - Configure `MQTT_VERSION` in `rsi_mqtt_client.h` based on the server configuration; only version 3 and 4 are supported.
  - 3xTLS connections or 1xTCP and 2xTLS connection, connections are supported concurrently in Wi-Fi only mode.
  - TLS curve IDs supported are 15-28. TLS handshake with 3rd party clients depends on the TLS curve.
  - Big endian architecture is not supported.
  - Auto PAC Provisioning in EAP-FAST with TLSv1.2 is not supported.
  - RC4_SHA, DES_CBC3_SHA and RC4_MD5 ciphers are not supported in enterprise security
  - SA query procedure not supported in 11W mode.
  - WPA3 is not supported in AP  / concurrent (AP+STA) modes.
  - If background scan is enabled along with custom feature bit map bit (8) configuration of the DFS channel support, issues may occur in obtaining an IP address for the Wi-Fi STA

### BT-Classic/BLE
  - BT A2DP is only supported for RS9116 silicon revision 1.5.
  - BT sniff mode does not work if BT multiple slaves feature is enabled.
  - For BLE, if the connection is established with a small connection interval (<15 ms), simultaneous roles are not supported (i.e., Central/Peripheral + Advertising/Scanning).
  - For BT Classic, only 1 connection is supported at any time.
  - BLE slave latency value is valid up to 32 only.
  - BT-A2DP encoder is not supported in the firmware.
  - BT-A2DP not supported in AT mode.

### Coexistence
  - In Wi-Fi + BLE mode, if BLE scan interval and window have the same value, then Wi-Fi connection may be unsuccessful.
  - Wi-Fi STA + BT + BLE multiprotocol use cases require a detailed understanding of use cases and associated configurations. Contact Silicon Labs support for additional details.
  - Wi-Fi AP + BLE, Wi-Fi AP + BT & Wi-Fi AP + BT + BLE modes are not supported.
---

# WiSeConnect 2.6.1 Release Notes

Last updated: September 14, 2022

## Highlights

  - Bug fixes
  
## Release Details
| Item                                                | Details                                |
|-----------------------------------------------------|----------------------------------------|
| Release date                                        | September 16, 2022                     |
| API Version (SAPI)                                  | 2.6.1 (Build 7)                        |
| Firmware Version                                    | 2.6.1.0.7 (Build 7)                      |
| Package Name                                        | RS9116W.2.6.1.7                        |
| Supported RTOS                                      | FreeRTOS                               |
| Hardware Modules                                    | QMS, B00, C00, CC1, AB0, AB1, AA0, AA1 |
| Hardware Chipsets                                   | Chip Revision 1.4, Chip Revision 1.5   |
| Operating Modes Supported                           | Wi-Fi STA, Wi-Fi AP, Wi-Fi STA+BLE     |
| Additional Operating Modes Supported (Beta Quality) | Wi-Fi STA+BT, Wi-Fi STA+BT+BLE         |


## Updates in this Release
RS9116W 2.6.1 release consists of two components, as follows:
  - Firmware - RS9116 Firmware Binary
  - SAPI Library - SAPI Library runs on Host

This release is meant only for use with designs based on RS9116 Silicon rev 1.4 (RS9116X-xxx-xxx-Bxx) and RS9116 Silicon rev 1.5 (RS9116X-xxx-xxx-Cxx).   
Customers using the RS9116X-xxx-xxx-Xxx parts (Silicon rev 1.3) can also upgrade to the latest firmware, however the power optimization feature macro `EXT_FEAT_LOW_POWER_MODE` must be disabled for revision 1.3.


Choose the firmware to match the silicon revision used in your design, as follows:
- RS9116W.2.6.1.0.7.rps
  - RS9116X-xxx-xxx-Xxx (Silicon rev 1.3)
  - RS9116X-xxx-xxx-Bxx (Silicon rev 1.4)
- RS916W.2.6.1.0.7.rps 
  - RS9116X-xxx-xxx-Cxx (Silicon rev 1.5)

## Changes and Fixes
### Wi-Fi
- Resolved issues with DHCP renewal process when HTTP download is in progress.
- Enhanced frequency offset calibration by reducing the number of iterations required. Also added checks to ensure frequency offset tuning doesn't get wrapped-around.
- Resolved stability issues with static IP assignment when a duplicate IP address on the network is configured on STA.
- For RSSI Query, responding with beacon avg rssi instead of last beacon RSSI.
- Memory read/write support added in AT mode.
- Fixed firmware stuck issue in WAP3 connection failure cases and also added changes to support WPA3-R3 certification.
- Fixed issue with error code 0x0015 observed for Configuration save or Configuration Enable commands with some Opermode configurations.

### Bluetooth
- Fix for fragmentation issue with 1Mbps to improve BLE performance.
- Fix to change BLE advertise frame Tx power by rsibt_updategaintableoffsetMaxpower.
- Fix to reduce the timing for BT pairing and connection with with BT_Power_save application.
- Fix for BLE disconnection with different error coes (4e08,4e22,4e3e) in AT modes.
- Fix to improve BLE scan performance on 1.3 revison boards.
- Fix for disconnection in audio streaming after connecting DUT to remote BLE device.
- Fix to get exact response for "at+rsibt_getlocalcod?" command. 
- Fix for Bluetooth classic work in the coex mode of RSI_OPERMODE_WLAN_BT_DUAL_MODE.
- Fix for module returning the improper remote device MTU size in the rsi_bt_app_on_spp_connect callback.
- Fix for first byte missing During the spp_tx in AT_commands.
- Fix for AFH map reported by HCI command HCI_Read_AFH_Channel_Map.
- Fix for bluetooth AFH disabling after role switch.
- Fix for SPP issues reported by automation team.
- Fix for BLE TX Power is not changing for TELEC and KCC regions.
- Fix for BLE SMP connection, if pairing information available, slave does not require to send pairing request.

### Documentation
  - Updated documentation for user store configurations feature.
  - Updated Wi-Fi PRM with information on the commands supported in concurrent mode.
  - Indicating error(0xFFF8) for UDP command when wrong parameters are issued.
  - Added a note point in SAPI PRM that provided values for x0_ctune in calibration app.
  - Updated the Document by adding WPA3 new features.

## Known Issues
- Power save in PTA 3-wire coexistence functionality requires a weak pull up to be applied to the GRANT pin. However, Beacon protection with power save is unsupported.

# WiSeConnect 2.6 Release Notes

## Index
  - [Highlights](#highlights)
  - [Release Details](#release-details)
  - [Updating to this Release](#updating-to-this-release)
  - [New Features](#new-features)
  - [Changes and Fixes](#changes-and-fixes)
  - [New APIs](#new-apis)
  - [Recommendations and New Options](#recommendations-and-new-options)
  - [Known Issues](#known-issues)
  - [Limitations and Unsupported Features](#limitations-and-unsupported-features)
  - [SDK Refactor](#sdk-refactor)
  - [Terminology](#terminology)

## Highlights
  - Support for storing server IP and port in non-volatile memory
  - IP conflict detection support for RS9116 internal network stack 
  - SNI (Server Name Indication) support for RS9116 silicon revision 1.4 and 1.5
  - Enhancements to support for logging
  - Enhancements to PTA 3-wire coexistence to protect beacon reception
  - Swyentooh and braktooth vulnerability fixes for BT/BLE

## Release Details
| Item                                                | Details                                |
|-----------------------------------------------------|----------------------------------------|
| Release date                                        | June 15, 2022                          |
| API Version (SAPI)                                  | 2.6.0 (Build 34)                       |
| Firmware Version                                    | 2.6.0.0.34 (Build 34)                       |
| Package Name                                        | RS9116W.2.6.0.34                       |
| Supported RTOS                                      | FreeRTOS                               |
| Hardware Modules                                    | QMS, B00, C00, CC1, AB0, AB1, AA0, AA1 | 
| Hardware Chipsets                                   | Chip Revision 1.4, Chip Revision 1.5   |
| Operating Modes Supported                           | Wi-Fi STA, Wi-Fi AP, Wi-Fi STA+BLE     |
| Additional Operating Modes Supported (Beta Quality) | Wi-Fi STA+BT, Wi-Fi STA+BT+BLE         |


## Updates in this Release
RS9116W 2.6 release consists of two components, as follows:
  - Firmware - RS9116 Firmware Binary
  - SAPI Library - SAPI Library runs on Host 

This release is meant only for use with designs based on RS9116 Silicon rev 1.4 (RS9116X-xxx-xxx-Bxx) and RS9116 Silicon rev 1.5 (RS9116X-xxx-xxx-Cxx). Customers using the RS9116X-xxx-xxx-Xxx parts (Silicon rev 1.3) can also upgrade to the latest firmware, however the power optimization feature macro `EXT_FEAT_LOW_POWER_MODE` must be disabled for revision 1.3.


Choose the firmware to match the silicon revision used in your design, as follows:
- RS9116W.2.6.0.0.34.rps
  - RS9116X-xxx-xxx-Xxx (Silicon rev 1.3)
  - RS9116X-xxx-xxx-Bxx (Silicon rev 1.4)
- RS916W.2.6.0.0.34.rps 
  - RS9116X-xxx-xxx-Cxx (Silicon rev 1.5)

This release includes bug-fixes, enhancements, relevant AT command updates, new SAPI features, and firmware. Most of the new features have associated APIs, which are available in the latest SAPI implementation only. It is strongly recommended to upgrade SAPI and Firmware together.

## New Features

### Wi-Fi
- Support for storing server IP and port in non-volatile memory
- IP conflict detection support for RS9116 internal network stack
- SNI (Server Name Indication) support for RS9116 silicon revision 1.4 and 1.5
- Enhancements to PTA 3-Wire coexitence to protect beacon reception

### Bluetooth - Common
- Swyentooh and braktooth vulnerability fixes for BT/BLE

### General
- Enhancements to support for logging
- Support for Giga FLASH
- Wi-Fi interoperability bug fixes

## Changes and Fixes
- `rsi_recv` timeout granularity changed from 1s to 10 ms.
- Fix for BLE disconnection and BLE SMP connection issues in example applications
- Fix for Wi-Fi reconnections due to improper handling of isolated cipher mismatch event
- Fix in example application for SonyMDR headset glitch issue
- Fix for glitches and audio pauses for AirPods Pro, Sennheiser, Jabra Elite and Mi Earphones while connected to Wi-Fi
- Fix for the issue where at+rsi_scan return invalid channel in case of reconnection with 5GHz band AP
- Fix for HTTPS file download issue in EAP-TLS network

## New APIs
For a complete list of new APIs, see [SAPI Changelog](https://docs.silabs.com/rs9116-wiseconnect/2.6/wifibt-wc-sapi-reference/changelog)

## Recommendations and New Options

### Wi-Fi
  - To ensure graceful handling during asynchronous TCP closures from a peer, enable the opermode parameter [ext_tcp_ip_feature_bit_map\[16\]](https://docs.silabs.com/rs9116-wiseconnect/2.6/wifibt-wc-sapi-reference/opermode#rsi-ext-tcpip-feature-bitmap)
  - Enable aggregation using the opermode parameter [feature_bit_map\[2\]](https://docs.silabs.com/rs9116-wiseconnect/2.6/wifibt-wc-sapi-reference/opermode#rsi-feature-bit-map)
  - To avoid TCP disconnects during a rejoin, set TCP retransmission count to >= `30`
  - To make a connection more robust for low throughput applications in busy networks, disable high MCS rates. For example, in a congested wireless envionment, a Smart Lock may benefit by disabling `MCS6` and `MCS7`.
  - To restart the RS9116, the application should call `rsi_driver_deinit()` followed by `rsi_driver_init()` and `rsi_device_init()`. For OS cases, additionally call `rsi_task_destroy(driver_task_handle)` to delete the driver task before calling `rsi_driver_deinit()` and create again after `rsi_device_init()` using `rsi_task_create()`.
  - Register the 'Join Fail Callback' function each time join is called. This is required because if a join fails, the callback is deregistered. Failure to register the callback may stop scan working correctly on a rejoin failure.

### BT-Classic/BLE
  - In BLE, the recommended range of the BLEConnection Interval is as follows:
    - Power Save (BLE Only) - 100 ms to 1.28 s
    - BT Classic + BLE Dual Mode is >= 200 ms
    - Wi-Fi + BLE coex - 30 ms to 250 ms
  - In BLE during Connection, the configuration of Scan Interval and Scan Window with the same value is not recommended
  - In BT Classic, the recommended Sniff Interval configuration during powersave is limited to 100 ms (\<= 100).
  - In BLE, if a device is acting as Central, the scan window (in `set_scan_params()` and `create_connection()` APIs) must be less than the existing Connection Interval.
  - In BLE mode, if scanning and advertising is in progress, and the device subsequently connects and moves to the central role, scanning and advertising stops. To further establish connection to another peripheral device or to a central device, the application must initiate advertising and scanning again.
  - Device powersave must be disabled prior to BT init and de-init.

### Coexistence
  - For concurrent Wi-Fi + BLE, and while a Wi-Fi connection is active, we recommend setting the ratio of the BLE scan window to BLE scan interval to 1:3 or 1:4.
  - Wi-Fi + BLE Advertising
    - All standard advertising intervals are supported. As Wi-Fi throughput is increased, a slight difference in on-air advertisements compared to configured intervals may be observed.
    - BLE advertising is skipped if the advertising interval collides with Wi-Fi activity.
  - Wi-Fi + BLE scanning
    - All standard scan intervals are supported. For better scan results, we recommend setting the ratio of the BLE scan window to BLE scan interval to 1:3 or 1:4.
    - BLE scanning will be stopped for intervals that collide with Wi-Fi activity.
  - Wi-Fi + BLE Central/Peripheral Connections
    - All standard connection intervals are supported.
    - For a stable connection, use optimal connection intervals and max supervision timeout in the presence of Wi-Fi activity.
  - Wi-Fi + BLE Central/Peripheral Data Transfer
    - To achieve higher throughput for both Wi-Fi and BLE, use medium connection intervals, such as 45 to 80 ms with maximum supervision timeout.
    - Ensure Wi-Fi activity consumes lower intervals.
  - For Wi-Fi + BT + BLE, Wi-Fi + BT, Wi-Fi + BLE operating modes, connect Wi-Fi before BT/BLE connections to ensure seamless, stable connection for all 3 protocols.
  - For Wi-Fi + BT + BLE, Wi-Fi + BT, Wi-Fi + BLE operating modes, if BT/BLE needs to be connected before a Wi-Fi connection, use with high supervision timeout and high connection interval for BLE and high sniff interval for BT, to ensure seamless, stable connection. This configuration also ensures a stable BT/BLE connection when Wi-Fi connects/disconnects/rejoins.
  - Wi-Fi + BLE Central/Peripheral Data Transfer

### System
  - For User Store configuration and Configuration Save, do not enable power save or save it as a configuration. If power save is enabled and saved as a configuration, upon boot up, the RS9116 will boot with the saved configuration and will go to powersave without any indication to the host.
  - Set the compiler optimization level to `O0` in project settings for IDE (KEIL).
  - Memory configuration must be 384K for BT/BLE and co-ex operating modes
    - Usage of low power flash mode bit (bit 19 in extended customer feature bitmap). Enable this bit for ultra low power standby associated scenarios. This results in about 20&micro;A lower Wi-Fi standby associated current consumption.
  - Memory Configuration in SAPI functions: The default memory configuration is 384K. The memory configuration can be changed to 256/320/384kB by changing the macros defined in the following files:
    - In `rsi_wlan_config.h` : `RSI_EXT_CUSTOM_FEATURE_BIT_MAP` `EXT_FEAT_256K_MODE` or `EXT_FEAT_320K_MODE` or `EXT_FEAT_384K_MODE`
    - In `rsi_common.c` : `rsi_uint32_to_4bytes(rsi_opermode->ext_custom_feature_bit_map, (EXT_FEAT_256K_MODE (or) EXT_FEAT_320K_MODE (or) EXT_FEAT_384K_MODE | RSI_EXT_CUSTOM_FEATURE_BIT_MAP))`;
  - Apply Opermode commands in AT mode correctly. Using the wrong opermode may lead to unspecified behavior.
  - Set the recommended Power Save Profile (PSP) type to Enhanced Max PSP.
  - During firmware updates, powersave operation should be disabled.
  - For high throughput applications, powersave operation should be disabled.
  - The application must set the real-time clock with correct timestamp when the feature is enabled before establishing a TLS connection.
  - The socket select and socket receive timeout must be at least 1 second. A timeout value under 1 second is not supported.
  - For more details, see 'Configuration parameters' in the [SAPI Reference Manual](https://docs.silabs.com/rs9116-wiseconnect/2.6/wifibt-wc-sapi-reference/).

## Known Issues

### Network Stack
  - DHCP renewal during HTTP download may fail, this may cause disconnection. 
  - Sometimes during a TLS handshake, ECC curve parameters may be incorrectly generated resulting in connection failure with BBD2 error. However, this recovers in the next attempt.
  - MQTT disconnects during firmware update when power save is enabled. Disable power save during firmware update and re-enable on update completion.
  - All GCM-based cipher suites are implemented in software and have limited performance.
  - The recommended MQTT publish payload is 1 kBytes.
  - If HTTP server functionality is enabled, do not use port 80 for the MQTT client.
  - Randomize the client port if using rapid connect/disconnect of the MQTT session on the same client port with powersave.
  - Secure TLS renegotiation is not supported in the embedded networking stack.
  - IPv6 support is not available in this release.


### Wi-Fi
  - WPA3 known issues
	  - Connection takes about 3-4 second
	  - SAE H2E (Hash to Element) is not supported
	  - Transition disable indication is not supported
	  - PMK SA caching is not supported
  - If the station performs a scan in concurrent mode (Wi-Fi STA+AP), stations connected to the AP may be disconnected. Enable AP after STA connection is completed.
  - Issue observed with WPA2 Enterprise Connectivity using Microsoft RADIUS Server.
  - If background scan is enabled along with custom feature bit map bit (8) configuration of the DFS channel support, issues may occur in obtaining an IP address for the Wi-Fi STA.
  - In fast PSP, configuration socket close is observed in full duplex communication in long duration tests.
  - PTA 3-wire coexistence does not work with powersave. However, if a weak pull up is applied to the GRANT pin, PTA 3-wire functionality will work with powersave. Details can be found in application notes.

### BT-Classic/BLE
  - SPP as MASTER connection doesn't work in AT/Binary mode.
  - In AT mode for SPP transfer the first byte is missing.
  - BT/BLE user gain table APIs don't work for Korea and Japan region.
  - In the configuration of WLAN connection, BT A2DP Streaming and BLE slave connection with SMP mode, BLE disconnections are observed.
  - Wake on wireless is not supported for BLE.
  - BT-HID may not interoperate with Apple devices.
  - In Wi-Fi + BT/BLE Co-Ex mode, high Wi-Fi broadcast traffic might cause BT/BLE disconnections.
  - In Wi-Fi + BT mode when there is continuous Wi-Fi data, you might observe BT not re-connecting to remote after disconnection.
  - BT-A2DP may not interoperate with some headsets.
  - For BLE, dual role scenario issues may occur when connecting as a central when the peripheral is advertising. Central connections can be made after peripheral connections are established and advertising stops.
  - A BLE disconnection will occur if both the peer and RS9116 already have bond info and the RS9116 initiates security as part of BLE secure connections. To workaround this issue, the application should not initiate a security request if it already has bond info.
  - A2DP music streaming glitches may be observed if a Wi-Fi download is in progress at the same time.

### Coexistence
  - After powersave is enabled in co-ex mode, `radio_init()` must be called to turn off powersave.
  - In Wi-Fi + BLE mode:
    - BLE may disconnect when the supervision timeout is configured to less than 16 seconds.
    - WPS does not work.
  - In Wi-Fi + BLE / BLE only modes:
    - For SPI as host interface, continuous BLE Tx notification may cause the host interface to hang needing a hardware reset. The issue is not seen with discrete burst BLE data.
    - When Wi-Fi disconnects, a BT/BLE reconnection issue is observed.

### Interoperability
  - Disconnection may occur in Wi-Fi client mode if an AP does not acknowledge QoS null frames with powersave configured.
  - Under some scenarios, the RS9116 may not send unicast probe requests if the access point does not send beacons as expected. This may result in disconnection from the AP.

### System
  - Chip Revision 1.3 doesn't support Low power mode optimization using bit setting defined by EXT_FEAT_LOW_POWER_MODE, setting this results in hang issue.
  - Sometimes the RS9116 enters powersave mode before the configured monitor interval for inactivity when co-ex is configured.
  - Wake on Wireless support has only been tested for the UART AT command interface.
  - Powersave without RAM retention does not work when the SPI interface is used.
  - The FTP client application does not work with EFx32/STM32 when an RTOS is used.
  - The HTTP OTA update application does not work with EFR32 and EFM32 when an RTOS is used.  
  - Firmware update does not work for the EFM32 platform.

## Limitations and Unsupported Features

### Wi-Fi/Network Stack
  - AMSDU transmit is not supported.
  - Fragmentation is not supported.
  - AMSDU's within AMPDU is not supported.
  - Currently, the RS9116 does not support the radio measurement requests feature of CCX V2
  - 802.11k is not supported
  - Short GI is not supported
  - 40 MHz bandwidth for both the 2.4 & 5 GHz band is not supported.
  - 802.11j channels less than 16 are not supported.
  - The USB host interface and USB powersave are not supported.
  - In AT mode, the total MQTT command length (apart from MQTT publish) should not exceed `150` bytes. This includes `at+rsi` (start of command) to (end of command)`\r\n`.
  - Configure `MQTT_VERSION` in `rsi_mqtt_client.h` based on the server configuration; only version 3 and 4 are supported.
  - 3xTLS connections or 1xTCP and 2xTLS connection, connections are supported concurrently in Wi-Fi only mode.
  - TLS curve IDs supported are 15-28. TLS handshake with 3rd party clients depends on the TLS curve.
  - Big endian architecture is not supported.

### BT-Classic/BLE
  - BT A2DP is only supported for RS9116 silicon revision 1.5.
  - BT sniff mode does not work if BT multiple slaves feature is enabled.
  - For BLE, if the connection is established with a small connection interval (<15 ms), simultaneous roles are not supported (i.e., Central/Peripheral + Advertising/Scanning).
  - A maximum of 2 concurrent BLE connections are supported; the connections can be either a connection to two peripheral devices or to 1 central and 1 peripheral device.
  - For BT Classic, only 1 connection is supported at any time.
  - BLE slave latency value is valid up to 32 only.
  - BT-A2DP encoder is not supported in the firmware.
  - BT-A2DP not supported in AT mode.

### Coexistence
  - In Wi-Fi + BLE mode, if BLE scan interval and window have the same value, then Wi-Fi connection may be unsuccessful.
  - For AT commands, Wi-Fi + BT + BLE (Opermode 9), BT + BLE (Opermode 8 ) do not work.
  - Wi-Fi STA + BT + BLE multiprotocol use cases require a detailed understanding of use cases and associated configurations. Contact Silicon Labs support for additional details.
  - Wi-Fi AP + BLE, Wi-Fi AP + BT & Wi-Fi AP + BT + BLE modes are not supported.

---

# WiSeConnect 2.5.2 Release Notes

Last updated: April 13, 2022 

## Highlights

  - Issue fixes
  
## Release Details
| Item                                                | Details                                |
|-----------------------------------------------------|----------------------------------------|
| Release date                                        | April 13, 2022                         |
| API Version (SAPI)                                  | 2.5.2 (Build 4)                        |
| Firmware Version                                    | 2.5.2.0.4 (Build 4)                      |
| Package Name                                        | RS9116W.2.5.2.4                        |
| Supported RTOS                                      | FreeRTOS                               |
| Hardware Modules                                    | QMS, B00, C00, CC1, AB0, AB1, AA0, AA1 |
| Hardware Chipsets                                   | Chip Revision 1.4, Chip Revision 1.5   |
| Operating Modes Supported                           | Wi-Fi STA, Wi-Fi AP, Wi-Fi STA+BLE     |
| Additional Operating Modes Supported (Beta Quality) | Wi-Fi STA+BT, Wi-Fi STA+BT+BLE         |


## Updates in this Release
RS9116W 2.5.2 release consists of two components
  - Firmware - RS9116 Firmware Binary
  - SAPI Library - SAPI Library runs on Host

This release contains bug-fixes and firmware binary updates. It is strongly recommended to update SAPI and firmware together.

## Changes and Fixes
- Fix for Bluetooth SPP Rx latency with SDIO as host interface 
- Fix for Bluetooth host application freeze issue with SDIO as host interface 
- Documentation updates to address following issues
  - SNTP Error BB08
  - BT Classic and BLE PER Stats command responses are NULL

## Known Issues
- PTA 3-Wire is not supported with power save being enabled.

---
# WiSeConnect 2.5.1.6 Release Notes

Last updated: January 2, 2023
  
## Release Details
| Item                                                | Details                                |
|-----------------------------------------------------|----------------------------------------|
| Release date                                        | January 2, 2023                         |
| API Version (SAPI)                                  | 2.5.1 (Build 6)                        |
| Firmware Version                                    | 2.5.1.6 (Build 6)                      |
| Package Name                                        | RS9116W.2.5.1.6                        |
| Supported RTOS                                      | FreeRTOS                               |
| Hardware Modules                                    | QMS, B00, C00, CC1, AB0, AB1, AA0, AA1 |
| Hardware Chipsets                                   | Chip Revision 1.4, Chip Revision 1.5   |
| Operating Modes Supported                           | Wi-Fi STA, Wi-Fi AP, Wi-Fi STA+BLE     |
| Additional Operating Modes Supported (Beta Quality) | Wi-Fi STA+BT, Wi-Fi STA+BT+BLE         |

## Changes and Fixes
  - For CC0/CC1 Modules, Corrected the transmit power approximately 2dB to fix an issue. This fix will not affect any regulatory requirements. 

## Note
  - The SAPI SDK for this firmware is same as 2.5.0.26. 
---

# WiSeConnect 2.5.1 Release Notes

Last updated: March 29, 2022

## Highlights

  - PTA 3-wire coexistence support in Wi-Fi only mode with Thread protocol
  - Issue fixes
  
## Release Details
| Item                                                | Details                                |
|-----------------------------------------------------|----------------------------------------|
| Release date                                        | March 29, 2022                         |
| API Version (SAPI)                                  | 2.5.1 (Build 5)                        |
| Firmware Version                                    | 2.5.1.0.5 (Build 5)                      |
| Package Name                                        | RS9116W.2.5.1.5                        |
| Supported RTOS                                      | FreeRTOS                               |
| Hardware Modules                                    | QMS, B00, C00, CC1, AB0, AB1, AA0, AA1 |
| Hardware Chipsets                                   | Chip Revision 1.4, Chip Revision 1.5   |
| Operating Modes Supported                           | Wi-Fi STA, Wi-Fi AP, Wi-Fi STA+BLE     |
| Additional Operating Modes Supported (Beta Quality) | Wi-Fi STA+BT, Wi-Fi STA+BT+BLE         |


## Updates in this Release
RS9116W 2.5.1 release consists of two components
  - Firmware - RS9116 Firmware Binary
  - SAPI Library - SAPI Library runs on Host

This release contains bug-fixes and firmware binary updates. It is strongly recommended to update SAPI and firmware together.

## Changes and Fixes
- PTA 3-wire coexistence support in Wi-Fi only mode with Thread protocol
- Fix for `rsi_timer_left()` to return the remaining time for expiry instead of total time expired so far
- Fix for low transmit power issue reported on latest AA0 modules
- Fix for firmware version showing differently in the package and in the software
- Fix for configuration of `SPI_INTR` to active low
- Fix for regulator voltages (`VOUTLDOAFE`, `UULP_VOUTSCDC`) not matching data sheet specifications
- Fix for HTTP OTAF upgrade issue when done with corrupt firmware in loop
- Fix for EAP TLS certificate loading issue in TCP/IP bypass enabled mode
- Fix for SNTP BB08 error

---

# WiSeConnect 2.5 Release Notes

## Index
  - [Highlights](#highlights)
  - [Release Details](#release-details)
  - [Updating to this Release](#updating-to-this-release)
  - [New Features](#new-features)
  - [Changes and Fixes](#changes-and-fixes)
  - [New APIs](#new-apis)
  - [Recommendations and New Options](#recommendations-and-new-options)
  - [Known Issues](#known-issues)
  - [Limitations and Unsupported Features](#limitations-and-unsupported-features)
  - [SDK Refactor](#sdk-refactor)
  - [Terminology](#terminology)

## Highlights
  - Support for RS9116 chip revision 1.5
  - WPA2 Security enhancements support
  - WPA3 Personal support for station mode
  - Support for enhanced MaxPSP in co-ex mode
  - Support for FAST PSP
  - PTA 3-wire coexistence support for Wi-Fi as PTA master
  - BT/BLE user gain API support
  - WRECK vulnerability fixes
  - FragAttacks vulnerability fixes
  - SDIO interface support
  - A2DP source support with coexistence
  - EFM32 (Silicon Labs MCU SLSTK3701A) platform support

## Release Details
| Item                                                | Details                                |
|-----------------------------------------------------|----------------------------------------|
| Release date                                        | December 31, 2021                      |
| API Version (SAPI)                                  | 2.5.0 (Build 26)                       |
| Firmware Version                                    | 2.5.0.0.26 (Build 26)                       |
| Package Name                                        | RS9116W.2.5.0.26                       |
| Supported RTOS                                      | FreeRTOS                               |
| Hardware Modules                                    | QMS, B00, C00, CC1, AB0, AB1, AA0, AA1 | 
| Hardware Chipsets                                   | Chip Revision 1.4, Chip Revision 1.5   |
| Operating Modes Supported                           | Wi-Fi STA, Wi-Fi AP, Wi-Fi STA+BLE     |
| Additional Operating Modes Supported (Beta Quality) | Wi-Fi STA+BT, Wi-Fi STA+BT+BLE         |


## Updates in this Release
RS9116W 2.5 release consists of two components, as follows:
  - Firmware - RS9116 Firmware Binary
  - SAPI Library - SAPI Library runs on Host

This release is meant only for use with designs based on RS9116 Silicon rev 1.4 (RS9116X-xxx-xxx-Bxx) and RS9116 Silicon rev 1.5 (RS9116X-xxx-xxx-Cxx).   
Customers using the RS9116X-xxx-xxx-Xxx parts (Silicon rev 1.3) can also upgrade to the latest firmware. However, you will have to disable the power optimization feature enabling macro EXT_FEAT_LOW_POWER_MODE.


Choose the firmware to match the silicon revision used in your design, as follows:
- RS9116W.2.5.0.26.rps
  - RS9116X-xxx-xxx-Xxx (Silicon rev 1.3)
  - RS9116X-xxx-xxx-Bxx (Silicon rev 1.4)
- RS916W.2.5.0.26.rps
  - RS9116X-xxx-xxx-Cxx (Silicon rev 1.5)


This release has bug-fixes, enhancements, relevant AT command updates, new SAPI features, and firmware. Most of the new features have associated APIs, which are available in the latest SAPI implementation only. It is strongly recommended to upgrade SAPI and Firmware together.

## New Features

### Wi-Fi
- WPA2 security enhancements
- WPA3 personal support for station mode
- Support for enhanced MaxPSP in co-ex mode
- Support for FAST PSP
- WRECK vulnerability fixes
- FragAttacks vulnerability fixes
- PTA 3-wire coexistence support in Wi-Fi only mode (with external Zigbee and BLE)

### Bluetooth - Common
- BT/BLE user gain API support

### Bluetooth - Classic
- Support for BT advanced audio distribution profile source in co-ex mode. An example application project is included for the RT595 host platform. This is supported for chip revision 1.5 only.

### Power Save
- None

### General
- Support for RS9116 chip revision 1.5
- Support for SDIO interface
- Logging infrastructure support for SAPI logging
- 802.11b Tx gain table improvements for chip revision 1.5 QMS
- Chip revision 1.5 B00 certified FCC/ETSI/TELEC gain tables included
- RS9116X-DB00-CCX-BXX certified TELEC gain tables included
- TELEC certified
- Support for examples on EFM32 (Silicon Labs MCU SLSTK3701A) platform for SDIO interface only
- Certified gain tables for FCC/IC, CE and TELEC are included in the release package in the folder: `.../resources/gain_tables`

## Changes and Fixes
- Fix for BT connectivity issue with HID profile with Windows 10 PC
- New API, which allows destroying a driver task before driver de-initialization to facilitate clean termination 
- Fix for WLAN scan issue with error code 3 in some of the 1.3-based AA0 modules(RS9116X-SB00-AAX-XXX)
- Fix for Module not responsive after few iterations of SPP profile data transfer in AT mode 
- Fix for SPP throughput issue
- Fix for UDP Server socket issue in AP mode

## New APIs
For a complete list of new APIs, see [SAPI Changelog](https://docs.silabs.com/rs9116-wiseconnect/2.5/wifibt-wc-sapi-reference/changelog)

## Recommendations and New Options

### Wi-Fi
  - Enable bit 16 of the 'Extended TCP IP Feature' bit map in opermode command for all Wi-Fi socket operations from host to ensure graceful handling during asynchronous closures from peer.
  - Enable aggregation (bit 2 of feature_bit_map) in opermode.
  - TCP retransmission count value should be 30 or higher, so that the TCP does not disconnect while the rejoin happens.
  - For low throughput applications, such as smart lock, disabling the highest MCS rates, such as MCS7 and MCS6 might help reducing packet retransmissions and ensure robust connection during heavy interference.
  - To restart the RS9116, the application needs to call `rsi_driver_deinit()` followed by `rsi_driver_init()` and `rsi_device_init()`. For OS cases, additionally call `rsi_task_destroy(driver_task_handle)` to delete the driver task before calling `rsi_driver_deinit()` and create again after `rsi_device_init()` using `rsi_task_create()`.
  - Register 'Join Fail Callback' function every time when join is called, as in the rejoin failure path the callback is deregistered. If not done, this may result in scan not working on rejoin failure.

### BT-Classic/BLE
  - In BLE, the recommended range of the connection interval is as follows:
    - Power Save (BLE Only) - 100 ms to 1.28 s
    - BT Classic + BLE Dual Mode is >= 200 ms
    - Wi-Fi + BLE coex - 30 ms to 250 ms
  - In BLE during connection, the configuration of scan interval and scan window with the same value is not recommended.
  - In BT Classic, the recommended sniff interval configuration during powersave is limited to 100 ms (\<= 100).
  - In BLE, if a device is acting as central, the scan window (in set_scan_params and create_connection command) must be less than the existing connection interval.
  - In BLE mode, if scanning and advertising is in progress and subsequently gets connected and moves to the central role, scanning and advertising stops. To further establish connection to another peripheral device or to a central device, the application should - give a command for starting advertising and scanning again.
  - Device Power save need to be disabled before BT init and de-init.

### Coexistence
  - For Wi-Fi + BLE, during Wi-Fi connection, lesser BLE scan Window and larger BLE scan Interval are recommended (1:3 or 1:4).
  - Wi-Fi + BLE Advertising
    - All advertising intervals are supported, as per the BT Spec. If the Wi-Fi transactions are more, you may see a slight difference in on-air advertisements compared to the configured intervals.
    - BLE Advertising will be stopped for intervals that collide with Wi-Fi activity.
  - Wi-Fi + BLE scanning
    - All scan intervals are supported as per BT Spec. For better scan results, use scan-window and scan interval in 1:3 or 1:4 ratio when the Wi-Fi scan activity is present.
    - BLE scanning will be stopped for intervals that collide with Wi-Fi activity.
  - Wi-Fi + BLE Central/Peripheral Connections
    - All connection intervals are supported as per BT Spec.
    - For a stable connection, use optimal connection intervals and max supervision timeout in the presence of Wi-Fi activity.
  - Wi-Fi + BLE Central/Peripheral Data Transfer
    - To get better throughputs in both Wi-Fi and BLE, use medium connection intervals, such as 45 to 80 ms with maximum supervision timeout.
    - Ensure Wi-Fi activity consumes lesser intervals.
  - For Wi-Fi + BT + BLE, Wi-Fi + BT, Wi-Fi + BLE operating modes, connect Wi-Fi ahead of BT/BLE connections to ensure seamless, stable connection for all 3 protocols.
  - For Wi-Fi + BT + BLE, Wi-Fi + BT, Wi-Fi + BLE operating modes, if BT/BLE needs to be connected ahead of Wi-Fi connection, use with high supervision timeout and high connection interval for BLE and high sniff interval for BT, to ensure seamless, stable - connection. This configuration will also ensure a stable BT/BLE connection when Wi-Fi connects/disconnects/rejoins.
  - Wi-Fi + BLE Central/Peripheral Data Transfer

### System
  - For User Store configuration and Configuration Save, do not enable power save or save it as a configuration. If power save is enabled and saved as a configuration, upon boot up, the module will come up with the saved configuration and will go to power save without any indication to the host.
  - Set Optimization level to O0 in project settings for IDE (KEIL).
  - Memory Configuration must be 384K for BT/BLE and Co-Ex Operating Modes
    - Usage of Low Power Flash mode bit (Bit 19 in Extended customer feature bitmap). Enable this bit for ultra low power standby associated scenarios. This results in about 20&micro;A lower Wi-Fi standby associated current consumption.
  - Memory Configuration in SAPI functions: The default memory configuration is 384K. You can configure it to 256/320/384kB by changing the macros defined in the below two files:
    - In `rsi_wlan_config.h` : `RSI_EXT_CUSTOM_FEATURE_BIT_MAP` `EXT_FEAT_256K_MODE` or `EXT_FEAT_320K_MODE` or `EXT_FEAT_384K_MODE`
    - In `rsi_common.c` : `rsi_uint32_to_4bytes(rsi_opermode->ext_custom_feature_bit_map, (EXT_FEAT_256K_MODE (or) EXT_FEAT_320K_MODE (or) EXT_FEAT_384K_MODE | RSI_EXT_CUSTOM_FEATURE_BIT_MAP));`
  - Apply opermode commands in AT mode correctly. The wrong opermode in some cases might lead to unspecified behavior.
  - Set the recommended Power Save Profile (PSP) type to Enhanced Max PSP.
  - Disable power save during firmware upgrade and re-enable on upgrade completion.
  - Disable power save for high throughput applications.
  - The application needs to ensure that it sets RTC with correct timestamp when the feature is enabled before establishing the SSL connection.
  - Timeout value should be minimum 1 second for socket select and socket receive calls. Timeout value less than 1 second is not currently supported.
  - For more details, see 'Configuration parameters' in the SAPI Reference Manual.

## Known Issues

### Network Stack
  - Sometimes during the SSL Handshake, the generated ECC curve parameters are incorrect, resulting in connection failure with BBD2 error. However, this recovers in the next attempt.
  - MQTT disconnects during firmware upgrade using OTAF, when power save is enabled. Disable power save during firmware upgrade and re-enable on upgrade completion.
  - Newly Added GCM Based Strong Cipher Suites will have performance issues as Hardware support is not available in the current platform.
  - Recommended MQTT Publish payload is 1 KB.
  - If the HTTP Server is configured in the module, MQTT client port should not be port configured on port 80.
  - Randomize the client port if using rapid connect/disconnect of the MQTT session on the same client port with the power save.
  - Secure SSL Renegotiation not supported in Embedded Networking Stack.
  - IPv6 support is not available in this release.

### Wi-Fi
  - WPA3 known issues
	- Connection takes about 1 second
	- SAE H2E (Hash to Element) is not supported
	- Transition disable indication is not supported
	- PMK SA caching is not supported
  - If the station performs the scan in concurrent mode (Wi-Fi STA+AP), it is possible for stations connected to the AP to get disconnected. Enable AP after STA connection is completed.
  - Issue observed with WPA2 Enterprise Connectivity using Microsoft RADIUS Server.
  - If BG Scan is enabled along with custom feature bit map bit (8) configuration of the DFS channel support, issues may occur in getting IP address for the Wi-Fi STA.
  - In FAST PSP, a configuration socket close is observed in full duplex communication in long duration tests.
 

### BT-Classic/BLE
  - Wake on Wireless not supported for BLE.
  - BT-HID might not inter-operate with Apple devices.
  - In Wi-Fi + BT/BLE Co-Ex mode, high Wi-Fi broadcast traffic might cause BT/BLE disconnections.
  - In Wi-Fi + BT mode when there is continuous Wi-Fi data, you might observe BT not re-connecting to remote after disconnection.
  - BT-A2DP interoperability issue might be observed with some headsets.
  - For BLE, dual role scenario issues might occur when connecting as central if the advertising is also going out for the peripheral role. Central connections can be made after the peripheral connections are established and advertising is stopped.
  - BLE disconnection is observed if both peer and module already have bond info and module initiating security as part of BLE secure connections. A possible workaround in the application is not to initiate a security request if the bond information is already available.
  - LMP timeout BLE disconnection observed with latest Realme mobile handsets.
  - Some of the A2DP music streaming glitches are observed if the Wi-Fi download is in progress at the same time.
  - Issue with EDR-3DH5 packet reception, where in bit error rate of more than 50% is observed.

### Coexistence
  - In co-ex Mode if power save is enabled, it stays in enabled state even after Wi-Fi disconnection. Disable power save after every `radio_init()` and enable it when intended by application.
  - BLE may disconnect with Wi-Fi + BLE configuration and Wi-Fi continuous data transfer when the low BLE supervision timeout is configured. When the supervision timeout is configured with the value of 16 seconds, no disconnections are observed.
  - Wi-Fi + BLE mode WPS is not working.
  - Wi-Fi +BLE/BLE only mode: For SPI as host interface, continuous BLE TX notification might cause the host interface to hang needing a hardware reset. The issue is not seen with discrete burst BLE data.
  - In Wi-Fi + BT/BLE configuration with Wi-Fi disconnects, BT/BLE reconnection issue is observed (see the earlier section 'Recommended Configurations and Application Development options' in this document).

### Interoperability
  - QoS null frames indicating going to power save if not acknowledged by AP may cause AP not to sync with the power save state, resulting in possible disconnections.
  - If 3rd party access points are periodically not sending beacons, in some scenarios unicast probe requests are not sent from the module, as expected, which might result in disconnections.

### System
  - Chip Revision 1.3 doesn't support low power mode optimization using bit setting defined by EXT_FEAT_LOW_POWER_MODE, setting, which results in hang issue.
  - Sometimes the module goes to power save before the configured monitor interval for inactivity when co-ex mode is configured.
  - Wake on Wireless support is tested only for UART AT command interface.
  - Power save without RAM retention is not working for the SPI interface.
  - FTP Client application is not working with EFx32/STM32 in the OS scenario. The demo app to be tested only for non-OS scenario.
  - HTTP OTAF application is not working with EFR32 and EFM32 in OS scenario. The demo app to be tested only for non-OS scenario on EFR32.  
  - Firmware upgrade for both OS and non-OS scenarios is not working for EFM32 application.

## Limitations and Unsupported Features

  ### Wi-Fi/Network Stack
  - AMSDU TX is not supported.
  - Fragmentation is not supported.
  - AMSDU's within AMPDU is not supported.
  - Currently, the module does not support Radio Measurement Requests feature of CCX V2
  - 802.11k is not supported
  - Short GI is not supported
  - 40 MHz bandwidth for both 2.4 & 5 GHz band is not supported.
  - 802.11J channels less than 16 are not supported.
  - USB host interface and USB Power save are not supported.
  - Total MQTT Command Length, apart from MQTT Publish, should not exceed 150 bytes. This includes at+rsi (start of command) to (end of command)\r\n.
  - Configure MQTT_VERSION in rsi_mqtt_client.h based on the server configuration; only version 3 and 4 are supported.
  - 3xSSL connections or 1xTCP and 2xSSL connection, connections are supported concurrently in Wi-Fi only mode.
  - SSL Curve IDs supported are 15-28. SSL handshake with 3rd party clients depends on the SSL Curve ID.

### BT-Classic/BLE
  - BT A2DP is supported on chip version 1.5 only
  - BT Sniff mode does not work if BT multiple slaves feature is enabled.
  - For BLE, if the connection is established with a small connection interval (< 15 ms), simultaneous roles (i.e., Central/Peripheral + Advertising/Scanning) are not supported.
  - BLE maximum 2 concurrent connections are supported, which can be either a connection to two peripheral devices or to 1 central and 1 peripheral device.
  - For BT Classic, only 1 connection is supported at any time
  - BLE Slave latency value is valid up to 32 only.
  - BT-A2DP encoder is not supported in the firmware
  - BT-A2DP not supported in AT mode

### Coexistence
  - In Wi-Fi + BLE during Wi-Fi connection, if both BLE scan interval and window are the same, an issue will occur in successfully making the Wi-Fi connection.
  - For AT commands, Wi-Fi + BT + BLE (Opermode 9), BT + BLE (Opermode 8 ) are not working. However, BT, Wi-Fi + BT modes (Opermode 5) works fine. Also, all combinations work fine with SAPI.
  - Wi-Fi STA + BT + BLE multiprotocol cases require detailed understanding of use cases and associated configurations. Contact Silicon Labs support for more details.
  - Wi-Fi AP + BLE, Wi-Fi AP + BT & Wi-Fi AP + BT + BLE modes are not supported.

## SDK Refactor
The SDK folder structure and names were significantly changed in the 2.4 release. For detailed information, see [SAPI Changelog](https://docs.silabs.com/rs9116-wiseconnect/2.4/wifibt-wc-sapi-reference/changelog)

---

# WiSeConnect 2.4.1.19 Release Notes

Last updated: January 2, 2023

## Release Details
| Item                                                | Details                                |
|-----------------------------------------------------|----------------------------------------|
| Release date                                        | January 2, 2023                      |
| API Version (SAPI)                                  | 2.4.1 (Build 19)                       |
| Firmware Version                                    | 2.4.1.0.19 (Build 19)                  |
| Package Name                                        | RS9116W.2.4.1.19                       |
| Supported RTOS                                      | FreeRTOS                               |
| Hardware Modules/Chipsets                           | QMS, B00, C00, CC1, AB0, AB1, AA0, AA1 |
| Operating Modes Supported                           | Wi-Fi STA, Wi-Fi AP, Wi-Fi STA+BLE     |
| Additional Operating Modes Supported (Beta Quality) | Wi-Fi STA+BT, Wi-Fi STA+BT+BLE         |

## Changes and Fixes
  - For CC0/CC1 Modules, Corrected the transmit power approximately 2dB to fix an issue. This fix will not affect any regulatory requirements. 

## Note
  - The SAPI SDK for this firmware is same as 2.4.1.16. 
---

# WiSeConnect 2.4.1 Release Notes

Last updated: September 10, 2021

## Highlights
  - FragAttack vulnerability fixes
  - Wi-Fi Interoperability issue fixes
  

## Release Details
| Item                                                | Details                                |
|-----------------------------------------------------|----------------------------------------|
| Release date                                        | September 1, 2021                      |
| API Version (SAPI)                                  | 2.4.1 (Build 16)                       |
| Firmware Version                                    | 2.4.1.0.16 (Build 16)                  |
| Package Name                                        | RS9116W.2.4.1.16                       |
| Supported RTOS                                      | FreeRTOS                               |
| Hardware Modules/Chipsets                           | QMS, B00, C00, CC1, AB0, AB1, AA0, AA1 |
| Operating Modes Supported                           | Wi-Fi STA, Wi-Fi AP, Wi-Fi STA+BLE     |
| Additional Operating Modes Supported (Beta Quality) | Wi-Fi STA+BT, Wi-Fi STA+BT+BLE         |


## Updates in this Release
RS9116W 2.4.1 release consists of two components
  - Firmware - RS9116 Firmware Binary
  - SAPI Library - SAPI Library runs on Host

This release has bug-fixes and Firmware. It is strongly recommended to upgrade SAPI and Firmware together.

## Changes and Fixes
  - Fix for FragAttack Vulnerabilities
  - Fix for issues on Swyentooth Vulnerability fixes
  - Fix for few Wi-Fi interoperability issues with certain Access Points
  - Fix for issue in BLE local attribute setting, when notification not enabled in remote, resulting in error 4046
  - Fix for issue in AP mode, switching to 5 GHz band results in 0x5B error
  - Fix for UDP Asynchronous socket not storing and providing remote IP and Port information
  - Fix for Wi-Fi low RF transmit power output issue for AA0 modules
  - Fix for ping failure after reconnection to AP in concurrent mode
  - Fix for HTTP OTAF upgrade failing in subsequent iterations if first iteration fails due to network connections resulting in 0xBBED error
  - Fix for issue with BLE connection 2nd iteration onwards after connect/disconnect when remote device is advertising as connectable
  - Fix for issue of BT disconnection with continous SPP_rx traffic, sometime due to LMP timeout

## Known Issues
  - FragAttack Vulnerability [CVE-2020-24588: Aggregation Attack (accepting non SPP A-MSDU frames)](https://nvd.nist.gov/vuln/detail/CVE-2020-24588) is not addressed in this release, as this needs a change in specification
  - FragAttack Vulnerability [CVE-2020-26146: Aggregation Attack (reassemble fragments with non-consecutive packet numbers)](https://nvd.nist.gov/vuln/detail/CVE-2020-26146) is not addressed in this release


## Workarounds

### CC0/CC1 Host Interface Detection at Low Temperatures

One or more of the serial host interfaces of RS9116 CC0/CC1 modules may fail to respond to host detection at ambient temperatures lower than approximately -20 degC.
Execute the following sequence of commands to adjust the module calibration to workaround this issue.

```c
#define BURN_INTO_FLASH      1
#define BURN_XO_FAST_DISABLE 8

rsi_wireless_init(8, 0);  // Set the operating mode of the module
rsi_wlan_radio_init();    // Set the operating band and initialize the module
rsi_calib_write(BURN_INTO_FLASH, BURN_XO_FAST_DISABLE, 0, 0);   // Trigger the calibration write process to update the 
                                                                // information in flash and wait for a response from the firmware.
```

----

**Note!**
  - The workaround must be executed at room temperature, ~25 degC.
  - The command sequence must be executed once ONLY.
  - Ensure the power supply to the module is stable while issuing commands.
  - The workaround is needed for existing products/boards already mounted with CC0/CC1 modules only.
  - Once executed, the module must be power-cycled before it can be used for normal operation. 

---

# WiSeConnect 2.4 Release Notes

## Index
  - [Highlights](#highlights)
  - [Release Details](#release-details)
  - [Updating to this Release](#updating-to-this-release)
  - [New Features](#new-features)
  - [Changes and Fixes](#changes-and-fixes)
  - [New APIs](#new-apis)
  - [Recommendations and New Options](#recommendations-and-new-options)
  - [Known Issues](#known-issues)
  - [Limitations and Unsupported Features](#limitations-and-unsupported-features)
  - [SDK Refactor](#sdk-refactor)
  - [Terminology](#terminology)

## Highlights
  - Azure and AWS cloud connectivity support
  - SweynTooth vulnerabilities fixes
  - Strong cipher suite support
  - Coexistence algorithm enhancements
  - EN 300 328 v.2.2.2 certification support
  - TELEC certification support
  - Wi-Fi interoperability fixes and enhancements
  - Crystal calibration support (needed for RS9116 QMS)
  - Significant reorganization and refactoring of the SDK
  - Integrated support for EFR32MG21 hosts and Simplicity Studio

## Release Details
| Item                                                | Details                                |
|-----------------------------------------------------|----------------------------------------|
| Release date                                        | June 30, 2021                          |
| API Version (SAPI)                                  | 2.4.0 (Build 36)                       |
| Firmware Version                                    | 2.4.0.0.36 (Build 36)                       |
| Package Name                                        | RS9116W.2.4.0.36                       |
| Supported RTOS                                      | FreeRTOS                               |
| Hardware Modules/Chipsets                           | QMS, B00, C00, CC1, AB0, AB1, AA0, AA1 |
| Operating Modes Supported                           | Wi-Fi STA, Wi-Fi AP, Wi-Fi STA+BLE     |
| Additional Operating Modes Supported (Beta Quality) | Wi-Fi STA+BT, Wi-Fi STA+BT+BLE         |


## Updates in this Release
RS9116W 2.4 release consists of two components
  - Firmware - RS9116 Firmware Binary
  - SAPI Library - SAPI Library runs on Host

This release has bug-fixes, enhancements, new SAPI features and Firmware. Most of the new features have associated APIs, which are available in the latest SAPI implementation only. It is strongly recommended to upgrade SAPI and Firmware together.

## New Features

### Wi-Fi
- Azure support for cloud connectivity
- AWS support for cloud connectivity
- Support for dynamic switching between AP and STA mode of operation (see  `config_feature_bit_map[16]`)
- Support for loading certificate without requiring a reboot of the chip
- Configurability option for disabling transmit Data rates `MCS5`, `MCS6` and `MCS7`
- Support for stronger cipher suites
- Support for enhanced max-PSP mode for better interoperability, see `config_feature_bit_map[26]` and refer to API `rsi_wlan_power_save_profile()`. This PSP type is supported for STA-only mode.
- Enhancements for better interoperability. Keep alive configuration, graceful closure of previous WLAN connections.
- Enhancements to Wi-Fi Auto Rate adaptation algorithm
- Enhancements to Wi-Fi + BLE coexistence support
- Improvements to Wi-Fi + BLE provisioning examples

### Bluetooth - Common
 - Resolution to SweynTooth family of BT vulnerabilities

### Bluetooth - Classic
 - Support for BT Advanced Audio Distribution Profile Source (BT alone mode only). Example Application project included for RT595 host platform

### Power Save
 - Fixes for functional issues in power save mode at high temperature, 85°C

### General
- WLAN EN 300 328 v.2.2.2 ETSI Certification Compliance support added for the following modules and SoC Versions: B00 1.4, QMS 1.3, QMS 1.4, CC0/CC1 1.3, CC0/CC1 1.4, and AA0/AA1 1.3
- Certified gain tables are added for these modules for different antennas, which can be configured using the user gain table API for Wi-Fi and `rsi_bt_set_antenna_tx_power_level()` API for BT/BLE. Default firmware configuration is for the internal PCB/Chip antenna.
  - B00: Internal antenna (Fractus FR05-S1-N-0-102) and Dipole antenna (Taoglas GW.34.5153)
  - CC0/CC1 1.4 FCC/ETSI: Dipole antenna (GW.71.5153), dipole antenna (Inside WLAN PRO-IS 299), PCB antenna (RSIA7), PIFA antenna (Smarteq 4211613980), PIFA antenna (MARS-31A8 Wi-Fi antenna)
  - CC0/CC1 1.3 : PCB antenna , dipole antenna
  - AA0/AA1-1.3: Chip antenna, heavy-duty screw mount antenna
- TELEC certification support
- Support for chip manufacturing software (Crystal Calibration support for RS9116 QMS chip integration)
- Support for examples on the EFR Platform
- Package and documentation restructure

## Changes and Fixes
- Fixes for memory leak in SSL, ensuring stability for longer connection
- Resolved issue with TCP long data transfer with low power configuration for memory configuration greater than 256k.
- Fix for setting device local name for BLE
- Fix for `rsi_ble_white_list_using_adv_data()` failure
- Fix for store config/auto-config keep-alive timeout not working.
- Fix for HID Keyboard notifications not being seen on the iOS mobile side
- Configurability support in Firmware update APIs for the SAFE and FAST upgrade
- Mishandling of SDIO interrupts addressed in SAPI driver by calling `rsi_hal_intr_pin_status()` API. MCU HAL APIs will need to port this API for respective platform. Refer to SAPI porting guide for more information.
- Boot up issue at low temperature (refer to API `rsi_calib_write() Bit of 3`)

## New APIs
For a complete list of new APIs, see [SAPI Changelog](https://docs.silabs.com/rs9116-wiseconnect/2.4/wifibt-wc-sapi-reference/changelog)

## Recommendations and New Options

### Wi-Fi
  - It is recommended to enable bit 16 of the 'Extended TCP IP Feature' bit map in opermode command for all Wi-Fi Socket Operations from host to ensure graceful handling during asynchronous closures from peer.
  - Aggregation (`bit 2 of feature_bit_map`) is recommended to be enabled in opermode.
  - TCP retransmission count value is recommended to be 30 or higher, so that the TCP does not disconnect while the rejoin happens.
  - For low throughput applications, such as a Smart lock, disabling the highest MCS rates, such as `MCS7` and `MCS6` might help reducing packet retransmissions and ensure robust connection during heavy interference.
  - To restart the module, the application needs to call `rsi_driver_deinit()` followed by `rsi_driver_init()` and `rsi_device_init()`. For OS cases, additionally call `rsi_task_destroy(driver_task_handle)` to delete the driver task before calling `rsi_driver_deinit()` and create again after `rsi_device_init()` using `rsi_task_create()`
  - Register 'Join Fail Callback' function every time when join is called, as in the rejoin failure path the callback is deregistered. If not done, this might result in a scan not functioning on rejoin failure.

### BT-Classic/BLE
  - In BLE, recommended range of Connection Interval in
    - Power Save (BLE Only) - 100 ms to 1.28 s.
    - BT Classic + BLE Dual Mode is >= 200 ms.
    - Wi-Fi + BLE coex - 30 ms to 250 ms
  - In BLE, during Connection, the configuration of Scan Interval and Scan Window with same value is not recommended.
  - In BT Classic, recommended Sniff Interval configuration during Power Save is limited to 100 ms (\<= 100).
  - In BLE, if a device is acting as Central, the scan window (in set_scan_params and create_connection command) must be less than the existing Connection Interval.
  - In BLE mode, if scanning and advertising is in progress and subsequently gets connected and moves to the central role, scanning and advertising stops. To further establish connection to another peripheral device or to a central device, the application should give a command for starting advertising and scanning again.
  - Device Power save need to be disabled before BT init and de-init.

### Coexistence
  - For Wi-Fi + BLE, during Wi-Fi connection, lesser BLE scan window and larger BLE scan interval are recommended (1:3 or 1:4).
  - Wi-Fi + BLE Advertising
    - All the advertising intervals are supported, as per the BT Spec. If the Wi-Fi transactions are more, you may observe a slight difference in on-air advertisements compared to the configured intervals.
    - BLE Advertising will be stopped for intervals that collide with Wi-Fi activity.
  - Wi-Fi + BLE scanning
    - All scan intervals are supported as per BT Spec. For better scan results, use scan-window and scan interval in 1:3 or 1:4 ratio when the Wi-Fi scan activity is present.
    - BLE scanning will be stopped for intervals that collide with Wi-Fi activity.
  - Wi-Fi + BLE Central/Peripheral Connections
    - All connection intervals are supported as per BT Spec.
    - For stable connection, use optimal connection intervals and max supervision timeout in the presence of Wi-Fi activity.
  - Wi-Fi + BLE Central/Peripheral Data Transfer
    - To get better throughputs in both Wi-Fi and BLE, use medium connection intervals, such as 45 to 80 ms with Maximum supervision timeout.
    - Ensure Wi-Fi activity consumes lesser intervals.
  - For Wi-Fi + BT + BLE, Wi-Fi + BT, Wi-Fi + BLE operating modes, connect Wi-Fi ahead of BT/BLE connections to ensure seamless, stable connection for all 3 protocols.
  - For Wi-Fi + BT + BLE, Wi-Fi + BT, Wi-Fi + BLE operating modes, if BT/BLE needs to be connected ahead of Wi-Fi connection, use with high supervision timeout and high connection interval for BLE and high sniff interval for BT, to ensure seamless, stable connection. This configuration will also ensure a stable BT/BLE connection when Wi-Fi connects/disconnects/rejoins.
  - Wi-Fi + BLE Central/Peripheral data transfer

### System
  - For User Store configuration and Configuration Save, do not enable power save or save it as a configuration. If power save is enabled and saved as a configuration, upon boot up, the module will come up with the saved configuration and will go to power save without any indication to the host.
  - Set Optimization level to `O0` in project settings for IDE (KEIL).
  - Memory Configuration must be 384K for BT/BLE and Co-Ex Operating Modes
    - Usage of Low Power Flash mode bit (Bit 19 in Extended customer feature bitmap). Enable this bit for Ultra low power standby associated scenarios. This results in about 20&micro;A lower Wi-Fi standby associated current consumption.
  - Memory Configuration in SAPI functions: The default memory configuration is 384K. Users can configure it to 256/320/384kB by changing the macros defined in the below two files
    - In `rsi_wlan_config.h` : `RSI_EXT_CUSTOM_FEATURE_BIT_MAP` `EXT_FEAT_256K_MODE` or `EXT_FEAT_320K_MODE` or `EXT_FEAT_384K_MODE`
    - In `rsi_common.c` : `rsi_uint32_to_4bytes(rsi_opermode->ext_custom_feature_bit_map, (EXT_FEAT_256K_MODE (or) EXT_FEAT_320K_MODE (or) EXT_FEAT_384K_MODE | RSI_EXT_CUSTOM_FEATURE_BIT_MAP))`;
  - Apply Opermode commands in AT mode correctly. The wrong opermode in some cases might lead to unspecified behavior.
  - Set the recommended Power Save Profile (PSP) type to Enhanced Max PSP for Wi-Fi alone mode and MAX PSP for Wi-Fi Co-Ex Modes.
  - Disable power save during firmware upgrade and re-enable on upgrade completion.
  - Disable power save for high throughput applications.
  - The application needs to ensure that it sets RTC with correct timestamp when the feature is enabled before establishing the SSL connection.
  - Timeout value should be minimum 1 second for socket select and socket receive calls. Timeout value less than 1 second is not currently supported.
  - For more details, see 'Configuration parameters' in the SAPI Reference Manual.

## Known Issues

### Network Stack
  - MQTT disconnects during firmware upgrade using OTAF, when power save is enabled. Disable power save during firmware upgrade and re-enable on upgrade completion.
  - Newly Added GCM Based Strong Cipher Suites will have performance issues as Hardware support is not available in the current platform.
  - Recommended MQTT Publish payload is 1 kBytes.
  - If HTTP Server is configured in the module, MQTT client port should not be port configured on port 80.
  - Randomize the client port if using rapid connect/disconnect of the MQTT session on the same client port with the power save.
  - Secure SSL Renegotiation not supported in Embedded Networking Stack.
  - IPv6 support is not available in this release.

### Wi-Fi
  - If the station performs the scan in concurrent mode (Wi-Fi STA+AP), it is possible for stations connected to the AP to get disconnected. Enable AP after STA connection is completed.
  - Issue observed with WPA2 Enterprise Connectivity using Microsoft RADIUS Server.
  - If BG Scan is enabled along with custom feature bit map bit (8) configuration of the DFS Channel support, issues may occur in getting IP address for the Wi-Fi STA.

### BT-Classic/BLE
  - Wake on Wireless not supported for BLE.
  - BT-HID might not inter-operate with Apple devices.
  - In Wi-Fi + BT/BLE Co-Ex mode, high Wi-Fi broadcast traffic might cause BT/BLE disconnections.
  - Continuous data transfer on SPP (Rx/Tx) with packet size ~800 bytes in some scenarios might lead to BT disconnection. Introduce delay in between transfers to work around this issue.
  - In Wi-Fi + BT mode when there is continuous Wi-Fi data, you might observe BT not re-connecting to remote after disconnection.
  - BT-A2DP interoperability issue might be observed with some headsets.
  - For BLE, dual role scenario issues might occur when connecting as central if the advertising is also going out for the peripheral role. Central connections can be made after the peripheral connections are established and advertising is stopped.

### Coexistence
  - In Co-Ex Mode if power save is enabled, it stays in enabled state even after Wi-Fi disconnection. Disable power save after every `radio_init()` and enable it when intended by application.
  - BLE may disconnect with Wi-Fi + BLE configuration and Wi-Fi continuous data transfer when the low BLE supervision timeout is configured. When the supervision timeout is configured with the value of 16 seconds, no disconnections are observed.
  - Wi-Fi + BLE mode WPS is not working.
  - Wi-Fi +BLE/BLE only mode: For SPI as host interface, continuous BLE TX Notification might cause the host interface to hang needing a hardware reset. The issue is not seen with discrete burst BLE data.
  - In Wi-Fi + BT/BLE configuration with Wi-Fi disconnects, BT/BLE reconnection issue is observed (see the earlier section 'Recommended Configurations and Application Development options' in this document).

### Interoperability
  - QoS Null frames indicating going to Power Save if not acknowledged by AP may cause AP not to sync with the power save state, resulting in Possible disconnections.
  - If 3rd Party Access Points are periodically not sending beacons, in some scenarios unicast probe requests are not sent from the module, as expected, which might result in disconnections.

### System
  - A known issue exists with FAST-PSP mode. Use Enhanced MAX-PSP power save.
  - Wake on Wireless support is tested only for UART AT command interface.
  - 802.11b transmit power of some AA0 modules might be 4 dBm less than that mentioned in data sheets. As a result, connectivity issues might be observed at 2.4 GHz when the AP is far.
  - Power Save without RAM retention is not working for the SPI interface.

## Limitations and Unsupported Features

### Wi-Fi/Network Stack
  - AMSDU TX is not supported.
  - Fragmentation is not supported.
  - AMSDU's within AMPDU is not supported.
  - Currently, the module does not support Radio Measurement Requests feature of CCX V2
  - 802.11k is not supported
  - Short GI is not supported
  - 802.11w is not supported
  - 40 MHz bandwidth in 2.4 GHz band is not supported.
  - 802.11J channels less than 16 are not supported.
  - USB host interface and USB Power save are not supported.
  - Total MQTT Command Length, apart from MQTT Publish, should not exceed `150` bytes. This includes `at+rsi` (start of command) to (end of command)`\r\n`.
  - Configure `MQTT_VERSION` in `rsi_mqtt_client.h` based on the server configuration; only version 3 and 4 are supported.
  - 3xSSL connections or 1xTCP and 2xSSL connection, connections are supported concurrently in Wi-Fi only mode.
  - SSL Curve IDs supported are 15-28. SSL handshake with 3rd party clients depends on the SSL Curve ID.

### BT-Classic/BLE
  - BT Sniff mode does not work if BT multiple slaves feature is enabled.
  - For BLE, if the connection is established with a small connection interval (\< 15 ms), simultaneous roles (i.e., Central/Peripheral + Advertising/Scanning) are not supported.
  - BLE maximum 2 concurrent connections are supported, which can be either a connection to two peripheral devices or to 1 central and 1 peripheral device.
  - For BT Classic, only 1 connection is supported at any time
  - BLE Slave latency value is valid up to 32 only.
  - BT-A2DP Encoder is not supported in the firmware
  - BT-A2DP not supported in AT mode
  - User Gain Table API is not available for BT/BLE Path. Limit the BT/BLE TX power for a specific antenna using rsi_bt_set_antenna_tx_power_level API.

### Coexistence
  - In Wi-Fi + BLE during Wi-Fi connection, if both BLE scan interval and window are the same, an issue will occur in successfully making the Wi-Fi connection.
  - For AT commands, Wi-Fi + BT + BLE (Opermode 9), BT + BLE(Opermode 8 ) are not working. However, BT, Wi-Fi + BT modes (Opermode 5) works fine. Also, all combinations work fine with SAPI.
  - Wi-Fi STA + BT + BLE multiprotocol cases require detailed understanding of use cases and associated configurations. Contact Silicon Labs Support for more details.
  - Wi-Fi AP + BLE, Wi-Fi AP + BT & Wi-Fi AP + BT + BLE modes are not supported.

## SDK Refactor
The SDK folder structure and names were significantly changed in the 2.4 release. For detailed information, see [SAPI Changelog](https://docs.silabs.com/rs9116-wiseconnect/2.5/wifibt-wc-sapi-reference/changelog)

## Terminology
| Term             | Description |
| -----------------|---------------------------------------- |
| **New Features** | These items are new to this release |
| **Changes/Issues Fixed** | Changes made to existing features found in previous releases of the software. |
|| Enhancements to existing product flow and feature set. |
||Bug fixes done in the Release |
| **Deprecated Items** | Features or functions or APIs that are removed from the distributed release to align with the software roadmap |
| **Known Issues** | Features or functions that do not work as planned at time of release. Workarounds may be offered to meet short term development goals, but longer-term solutions will be added in future software releases |
| **Limitations/Recommendations** | Describes the limitations on product usage and recommendations for optimal use cases |
