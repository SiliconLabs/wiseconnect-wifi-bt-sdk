# WLAN Station BLE Provisioning with AWS cloud

## 1. Purpose / Scope

This example demonstrates how to configure/connects RS9116W EVK to get the WLAN connection functionality using BLE provisioning. In this application, the Module starts advertising and with BLE Provisioning the Access Point details are fetched

The RS9116W EVK module is configured  as a station, which associates to an access point and then communicates with an AWS server using MQTT, 

## 2. Prerequisites / Setup Requirements

Before running the application, the user will need the following things to setup.

### 2.1 Hardware Requirements

- Windows PC with Host interface (UART / SPI).
- Silicon labs [RS9116 Wi-Fi Evaluation Kit](https://www.silabs.com/development-tools/wireless/wi-fi/rs9116x-sb-evk-development-kit)
- Host MCU Eval Kit. This example has been tested with:
   - Silicon labs [WSTK + EFR32MG21](https://www.silabs.com/development-tools/wireless/efr32xg21-bluetooth-starter-kit)
   - [STM32F411 Nucleo](https://st.com/)
- Wireless Access point
- Android Mobile Phone with Silicon labs Connect App. 
        
   (or)
        
- Windows PC with windows Silicon labs connect application.
  
![Setup diagram of WLAN Station BLE Provisioning with AWS cloud ](resources/readme/image_aws1.png)

### 2.2 Software Requirements

- [WiSeConnect SDK](https://github.com/SiliconLabs/wiseconnect-wifi-bt-sdk/)
    
- Embedded Development Environment

   - For STM32, use licensed [Keil IDE](https://www.keil.com/demo/eval/arm.htm)

   - For Silicon Labs EFx32, use the latest version of [Simplicity Studio](https://www.silabs.com/developers/simplicity-studio)
   
- User can install any of the below apps to verify WLAN Station BLE Provisioning.

  1. Install Android based Silicon labs Connect APK:
    - Access the apk provided in the path: `<Release_Package>/utilities/Silicon_lab_connectApp.apk`
  
  2. Install windows Silicon labs connect application in laptop.
    - Refer the application provided in the path: `<Release_Package>/utilities/windows_ble_provisioning_app`
    - Follow the instructions provided in the document: `<Release_Package>/utilities/windows_ble_provisioning_app/Installation_steps_to_run_silabs_connect_windows_application.docx`


## 3. Application Build Environment

### 3.1 Platform

The Application can be built and executed on below Host platforms
* [STM32F411 Nucleo](https://st.com/)
* [WSTK + EFR32MG21](https://www.silabs.com/development-tools/wireless/efr32xg21-bluetooth-starter-kit) 

### 3.2 Host Interface

* By default, the application is configured to use the SPI bus for interfacing between Host platforms and the RS9116W EVK.
* The SAPI driver provides APIs to enable other host interfaces if SPI is not suitable for your needs.

### 3.3 Project Configuration

The Application is provided with the project folder containing Keil and Simplicity Studio project files. 
* Keil Project: 
  - The Keil project can be executed on STM32 platform.
  - Project path: `<Release_Package>/examples/snippets/wlan_ble/wlan_station_ble_provisioning_AWS/projects/wlan_station_ble_provisioning_AWS-nucleo-f411re.uvprojx`

* Simplicity Studio:
  - The Simplicity Studio project can be executed on EFR32MG21.
  - Project path: 
    - If the Radio Board is **BRD4180A** or **BRD4181A**, then access the path `<Release_Package>/examples/snippets/wlan_ble/wlan_station_ble_provisioning_AWS/projects/wlan_station_ble_provisioning_AWS/projects-brd4180a-mg21.slsproj` 
    - If the Radio Board is **BRD4180B** or **BRD4181B**, then access the path `<Release_Package>/examples/snippets/wlan_ble/wlan_station_ble_provisioning_AWS/projects/wlan_station_ble_provisioning_AWS/projects-brd4180b-mg21.slsproj`
    - User can find the Radio Board version as given below

![EFR Radio Boards](resources/readme/image279d.png) 


### 3.4 Bare Metal Support

This application supports only bare metal configuration. By default, the application project files (Keil and Simplicity Studio) are provided with bare metal environment in the release package. 

### 4. Application Configuration Parameters

The application can be configured to suit your requirements and development environment. Read through the following sections and make any changes needed.

**4.1** Open `rsi_wlan_app.c` file and update/modify following macros

   Modify the MQTT topics and give different names for both topicsRS9116 is subscribed to MQTT_TOPIC1 and publishing to MQTT_TOPIC2. 
   MQTT web application is subscribed to `MQTT_TOPIC2` and publishing on `MQTT_TOPIC1`.

```c
#define MQTT_TOPIC1           "$aws/things/SmartLockWebApp-v01/shadow/update/device_status"
#define MQTT_TOPIC2           "$aws/things/SmartLockWebAppv01/shadow/update/delta/temperature_reading"
```
   **NOTE:** If you would like to change the topic names, then you can modify only the last keyword of the topic, which are `device_status` and `temperature_reading`. 


**To configure the IP address**
   

   Memory length for driver

```c
#define GLOBAL_BUFF_LEN                          15000
```
  
   Memory length for the send buffer

```c
#define BUF_SIZE                                 1400
```
   
   The following parameters are configured if OS is used. WLAN task priority is given and this should be of low priority

```c
#define RSI_WLAN_TASK_PRIORITY                   1
```
   
   Driver task priority is given and this should be of the highest priority

```c
#define RSI_DRIVER_TASK_PRIORITY                 1
```
   
   WLAN Task stack size is configured by this macro

```c
#define RSI_WLAN_TASK_STACK_SIZE                 500
```
   
   Driver Task stack size is configured by this macro

```c
#define RSI_DRIVER_TASK_STACK_SIZE               500
```

**4.1.1** Open `rsi_wlan_config.h` file and update/modify the following macros

```c
#define CONCURRENT_MODE                          RSI_DISABLE
#define RSI_FEATURE_BIT_MAP                      (FEAT_SECURITY_OPEN  | FEAT_AGGREGATION | FEAT_ULP_GPIO_BASED_HANDSHAKE)
#define RSI_TCP_IP_BYPASS                        RSI_DISABLE 
#define RSI_TCP_IP_FEATURE_BIT_MAP               (TCP_IP_FEAT_DHCPV4_CLIENT | TCP_IP_FEAT_SSL | TCP_IP_FEAT_DNS_CLIENT | TCP_IP_TOTAL_SOCKETS_1 | TCP_IP_FEAT_EXTENSION_VALID)
#define RSI_CUSTOM_FEATURE_BIT_MAP               FEAT_CUSTOM_FEAT_EXTENTION_VALID
#define RSI_EXT_CUSTOM_FEATURE_BIT_MAP           (EXT_FEAT_384K_MODE | EXT_FEAT_LOW_POWER_MODE | EXT_FEAT_RSA_KEY_WITH_4096_SUPPORT | EXT_FEAT_SSL_CERT_WITH_4096_KEY_SUPPORT)  
define RSI_EXT_TCPIP_FEATURE_BIT_MAP             ( EXT_DYNAMIC_COEX_MEMORY | EXT_TCP_IP_FEAT_SSL_MEMORY_CLOUD | CONFIG_FEAT_EXTENTION_VALID)    
define RSI_CONFIG_FEATURE_BITMAP                 RSI_FEAT_SLEEP_GPIO_SEL_BITMAP 
define RSI_BT_FEATURE_BITMAP                     (BT_RF_TYPE | ENABLE_BLE_PROTOCOL)
define RSI_BAND                                  RSI_BAND_2P4GHZ 
```


   **Note:**
   `rsi_wlan_config.h` file is already set with the desired configuration in respective example folders, user need not change for each example.


**4.2** Open `rsi_ble_app.c` file and update/modify following macros
   
   **Configuring the BLE Application**
   
   `RSI_BLE_CHAR_SERV_UUID` refers to the attribute type of the characteristics to be added in a service.
   
```c
#define  RSI_BLE_CHAR_SERV_UUID                         0x2803
```

   `RSI_BLE_CLIENT_CHAR_UUID` refers to the attribute type of the client characteristics descriptor to be added in a service.
   
```c
#define RSI_BLE_CLIENT_CHAR_UUID                        0x2902
```

   `RSI_BLE_NEW_SERVICE_UUID` refers to the attribute value of the newly created service.
   
```c
#define  RSI_BLE_NEW_SERVICE_UUID                       0xAABB
```

   `RSI_BLE_ATTRIBUTE_1_UUID` refers to the attribute type of the first attribute under this service (RSI_BLE_NEW_SERVICE_UUID).
   
```c
#define  RSI_BLE_ATTRIBUTE_1_UUID                        0x1AA1
```
  
   `RSI_BLE_ATTRIBUTE_2_UUID` refers to the attribute type of the second attribute under this service (RSI_BLE_NEW_SERVICE_UUID).
   
```c
#define RSI_BLE_ATTRIBUTE_2_UUID                         0x1BB1
```

   `RSI_BLE_ATTRIBUTE_3_UUID` refers to the attribute type of the third attribute under this service (RSI_BLE_NEW_SERVICE_UUID).

```c
#define RSI_BLE_ATTRIBUTE_3_UUID                         0x1CC1
```

   `RSI_BLE_MAX_DATA_LEN` refers to the Maximum length of the attribute data.

```c
#define RSI_BLE_MAX_DATA_LEN                               20
```

   `RSI_BLE_APP_DEVICE_NAME` refers to the name of the Silicon Labs device to appear during scanning by remote devices.

```c
#define  RSI_BLE_APP_DEVICE_NAME               "BLE_CONFIGURATOR"
```

   The following are the **non-configurable** macros in the application.

   RSI_BLE_ATT_PROPERTY_READ is used to set the READ property to an attribute value.
   
```c
#define  RSI_BLE_ATT_PROPERTY_READ                    0x02
```
   `RSI_BLE_ATT_PROPERTY_WRITE` is used to set the WRITE property to an attribute value.

```c
#define RSI_BLE_ATT_PROPERTY_WRITE                       0x08
```

   `RSI_BLE_ATT_PROPERTY_NOTIFY` is used to set the NOTIFY property to an attribute value.

```c
#define  RSI_BLE_ATT_PROPERTY_NOTIFY                      0x10
```

   `BT_GLOBAL_BUFF_LEN` refers to the number of bytes required by the application and the driver.

```c
#define  BT_GLOBAL_BUFF_LEN                              15000
```

**4.2** Open `aws_iot_config.h` file and change the AWS_IOT_MQTT_CLIENT_ID to your choice (Make sure this is unique, if more than one user use has same client id it might get conflict at server side). 

```c
#define AWS_IOT_MQTT_CLIENT_ID       "Redp"
```
   
   **Note** Certificates are required for the AWS Cloud connection. Please make sure to replace the certficate files (aws_client_certificate.pem.crt, aws_client_private_key.pem.key, aws_starfield_ca.pem) in the release path: `<Release_Package>/resources/certificates/` . Certificates can be downloaded from AWS website.

## 5. Testing the Application

Follow the below steps for the successful execution of the application.

### 5.1 Loading the RS9116W Firmware

Refer [Getting started with PC ](https://docs.silabs.com/rs9116/latest/wiseconnect-getting-started) to load the firmware into RS9116W EVK. The firmware binary is located in `<Release_Package>/firmware/`

### 5.2 Building the Application on the Host Platform

### 5.2.1 Using STM32

Refer [STM32 Getting Started](https://docs.silabs.com/rs9116-wiseconnect/latest/wifibt-wc-getting-started-with-efx32/)  

- Open the project `<Release_Package>/examples/snippets/wlan_ble/wlan_station_ble_provisioning_AWS/projects/wlan_station_ble_provisioning_AWS-nucleo-f411re.uvprojx` in Keil IDE.
- Build and Debug the project
- Check for the RESET pin:
  - If RESET pin is connected from STM32 to RS9116W EVK, then user need not press the RESET button on RS9116W EVK before free run.
  - If RESET pin is not connected from STM32 to RS9116W EVK, then user need to press the RESET button on RS9116W EVK before free run.
- Free run the project
- Then continue the common steps from **Section 5.3**


### 5.2.2 Using EFX32

Refer [EFx32 Getting Started](https://docs.silabs.com/rs9116-wiseconnect/latest/wifibt-wc-getting-started-with-efx32/)

- Import the project from `<Release_Package>/examples/snippets/wlan_ble/wlan_station_ble_provisioning_AWS/projects`
- Select the appropriate .slsproj as per Radio Board type mentioned in **Section 3.3**
- Compile and flash the project in to Host MCU
- Debug the project
- Check for the RESET pin:
  - If RESET pin is connected from STM32 to RS9116W EVK, then user need not press the RESET button on RS9116W EVK before free run
  - If RESET pin is not connected from STM32 to RS9116W EVK, then user need to press the RESET button on RS9116W EVK before free run
- Free run the project
- Then continue the common steps from **Section 5.3**


### 5.3 Execution of AWS IoT with BLE Provisioning

#### 5.3.1  BLE Provisioning with Windows Application

1. Configure the Access point with internet connection in OPEN/WPA-PSK/WPA2-PSK mode to connect the RS9116W EVK in STA mode.

2. Connect any serial console for prints.

   **Note** Turn on **BT** in the PC.
  
3. Go to folder  `<Release_Package>/utilities/windows_ble_provisioning_app` and Run **Silabs_Connect.py** in the command prompt.
  
4. Executing the application in the command prompt as stated in the above point will open the GUI (with the “BLE SCAN ON” button.

![Silicon labs connect GUI](resources/readme/image_aws15.png)
   
5. Check for the configured name in the BLE scanned list and connect to the ‘SILABS_BLE_CONFIGURATOR’, by clicking on it.
   
![Silicon labs BLE Configurator ](resources/readme/image_aws16.png)
   
6. Once the BLE connection is successful, Wi-Fi access points scan list will be populated. Click on the Access Point (AP) SSID and enter the password of the AP, to which you would  like to connect. 
   
![ Silicon labs Wi-Fi access points](resources/readme/image_aws17.png)

   **Note** Now go to section **5.3.3** MQTT connection for AWS connection

### 5.3.2 BLE Provisioning with Android Application

1. Silicon Labs Connect app is available in the `<Release_Package>/utilities/` folder. Currently, this app will be available for Android devices only.
   
2. Once the application gets executed, launch the Android application which you have already installed on Android mobile. 

3. Click on BLE Provisioning.

![BLE Provisioning BLE connect app](resources/readme/image_aws18.png)

4. It will scan for the RS9116 module, and it appears in the list. For example, “SILABS_BLE_Configurator” on the UI, select this.
  
![Module appears in the list](resources/readme/image_aws19.png)
  
5. Now, RS9116 module will start as a station and scan for the access points (AP) nearby.
    
6. Once the list of AP scan results is displayed on the screen, you can select the SSID of the AP to which you want to connect. 

![Module SSID](resources/readme/image_aws20.png)
   
7. Click on the SSID of the AP, enter “password” if the AP is in security mode. Click on “Connect” to associate with the access point.

8. Once Silicon labs RS9116 module is connected to the access point, you can notice on the GUI as below.

![Module Access](resources/readme/image_aws21.png)
   
9. This completes the BLE provisioning using Android application, next step is the MQTT communication.

### 5.3.3 MQTT Connection

1. Once MQTT connection completed , device subscribed to specific Topic and waits for data from the Cloud.
   if any data received through AWS cloud from subscribed clients, then module publishes temperature sensor reading to MQTT client in case of EFR32.
   While in case of  STM32 random temperature readings ranges from 25-40 is published to MQTT client through AWS cloud.

2. You can use any MQTT client and connect to the AWS cloud, for subscribe and publishing messages.

![Module connected log ](resources/readme/image_aws23.png)