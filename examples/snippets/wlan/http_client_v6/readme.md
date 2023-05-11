# HTTP Client

## 1. Purpose / Scope

This application demonstrates how to create Silicon Labs device as HTTP/HTTPs client and do HTTP PUT, GET and POST operations with the HTTP server opened on remote peer.
In this application, the device configures as Wi-Fi station and connects to Access point and do HTTP/HTTPs PUT, GET and post operation with HTTP server opened on remote peer.

## 2. Prerequisites / Setup Requirements

Before running the application, the user will need the following things to setup.

### 2.1 Hardware Requirements 
  
* Windows PC with Host interface (UART / SPI/ SDIO).
* Silicon Labs [RS9116 Wi-Fi Evaluation Kit](https://www.silabs.com/development-tools/wireless/wi-fi/rs9116x-sb-evk-development-kit)
* Host MCU Eval Kit. This example has been tested with:
  - Silicon Labs [WSTK + EFR32MG21](https://www.silabs.com/development-tools/wireless/efr32xg21-bluetooth-starter-kit)
  - Silicon Labs [WSTK + EFM32GG11](https://www.silabs.com/development-tools/mcu/32-bit/efm32gg11-starter-kit)
  - [STM32F411 Nucleo](https://st.com/)
* Windows PC (Remote PC) with iperf application

![Setup Diagram for Access Point Start Example](resources/readme/image83.png)

### 2.2 Software Requirements

- [WiSeConnect SDK](https://github.com/SiliconLabs/wiseconnect-wifi-bt-sdk/).

- Embedded Development Environment.

   - For STM32, use licensed [Keil IDE](https://www.keil.com/demo/eval/arm.htm)

   - For Silicon Labs EFx32, use the latest version of [Simplicity Studio](https://www.silabs.com/developers/simplicity-studio).
- Windows PC with openssl support and python installed.

## 3. Application Build Environment

### 3.1 Platform

The Application can be built and executed on below Host platforms
* [STM32F411 Nucleo](https://st.com/)
* [WSTK + EFR32MG21](https://www.silabs.com/development-tools/wireless/efr32xg21-bluetooth-starter-kit) 
* [WSTK + EFM32GG11](https://www.silabs.com/development-tools/mcu/32-bit/efm32gg11-starter-kit)

### 3.2 Host Interface

* By default, the application is configured to use the SPI bus for interfacing between Host platforms (STM32F411 Nucleo / EFR32MG21) and the RS9116W EVK.
* This application is also configured to use the SDIO bus for interfacing between Host platforms (EFM32GG11) and the RS9116W EVK.

### 3.3 Project Configuration

The Application is provided with the project folder containing Keil and Simplicity Studio project files.

* Keil Project
  - The Keil project is used to evaluate the application on STM32.
  - Project path: `<SDK>/examples/snippets/wlan/http_client_v6/projects/http_client_v6-nucleo-f411re.uvprojx`

* Simplicity Studio
  - The Simplicity Studio project is used to evaluate the application on EFR32MG21.
  - Project path: 
    - If the Radio Board is **BRD4180A** or **BRD4181A**, then access the path `<SDK>/examples/snippets/wlan/http_client_v6/projects/http_client_v6-brd4180a-mg21.slsproj`
    - If the Radio Board is **BRD4180B** or **BRD4181B**, then access the path `<SDK>examples/snippets/wlan/http_client_v6/projects/http_client_v6-brd4180b-mg21.slsproj`
    - User can find the Radio Board version as given below 

![EFR Radio Boards](resources/readme/image83a.png)

  - EFM32GG11 platform
    - The Simplicity Studio project is used to evaluate the application on EFM32GG11.
      - Project path:`<SDK>examples/snippets/wlan/http_client_v6/projects/http_client_v6-brd2204a-gg11.slsproj`

### 3.4 Bare Metal/RTOS Support

This application supports bare metal and RTOS environment. By default, the application project files (Keil and Simplicity studio) are provided with bare metal configuration in the SDK. 

## 4. Application Configuration Parameters

The application can be configured to suit user requirements and development environment. Read through the following sections and make any changes needed.

### 4.1  Open rsi_http_client_app.c file 

#### 4.1.1 The desired parameters are provided below. User can also modify the parameters as per their needs and requirements.

SSID refers to the name of the Access point to be created.

```c
#define SSID                                 "SILABS_AP"
```

CHANNEL_NO refers to the channel in which AP would be started

```c
#define CHANNEL_NO                           11
```

SECURITY_TYPE refers type of security. Access point supports Open, WPA, WPA2 securities.

   Valid configurations are:

   - RSI_OPEN - For OPEN security mode

   - RSI_WPA - For WPA security mode

   - RSI_WPA2 - For WPA2 security mode

```c
#define SECURITY_TYPE                        RSI_WPA2
```

ENCRYPTION_TYPE refers to the type of Encryption method. Access point supports OPEN, TKIP, CCMP encryption methods.

Valid configurations are:

   - RSI_CCMP - For CCMP encryption

   - RSI_TKIP - For TKIP encryption

   - RSI_NONE - For open encryption


PSK refers to the secret key if the Access point to be configured in WPA/WPA2 security modes.

```c
#define PSK                      "1234567890"            
```
To Load certificate

```c
#define LOAD_CERTIFICATE              1
```

To configure IP address
DHCP_MODE refers whether IP address configured through DHCP or STATIC.

```c
#define DHCP_MODE                      1
```

**Note!**
> If the user wants to configure the STA IP address through DHCP then set DHCP_MODE to "1" and skip configuring the following DEVICE_IP6, and GATEWAY6 macros Otherwise set the DHCP_MODE macro to "0" and configure the following DEVICE_IP6 and GATEWAY6 macros.
****

IPv6 address to be configured to the device 

   Example: To configure "2001:db8:0:1::121" as IP address, update the macro DEVICE_IP6 as below

```c
#define DEVICE_IP6                             "2001:db8:0:1::121"
```

IP address of the gateway 

   Example: To configure "2001:db8:0:1::121" as Gateway, update the macro GATEWAY6 as below

```c
#define GATEWAY6                               "2001:db8:0:1::121"
```

HTTP_SERVER_IP_ADDRESS refers IP address of the HTTP/HTTPS server

```c
#define HTTP_SERVER_IP_ADDRESS                "2001:0db8:0000:0001:0000:0000:0000:0121"
```

### 4.2 Open rsi_wlan_config.h file. User can also modify the below parameters as per their needs and requirements.


```c
#define CONCURRENT_MODE                      RSI_DISABLE
#define RSI_FEATURE_BIT_MAP                  FEAT_SECURITY_PSK
#define RSI_TCP_IP_BYPASS                    RSI_DISABLE
#define RSI_TCP_IP_FEATURE_BIT_MAP           (TCP_IP_FEAT_DHCPV4_CLIENT | TCP_IP_FEAT_DNS_CLIENT |TCP_IP_FEAT_SSL | TCP_IP_FEAT_EXTENSION_VALID \
                                             |TCP_IP_FEAT_DHCPV6_CLIENT | TCP_IP_FEAT_IPV6 | TCP_IP_FEAT_HTTP_CLIENT)
#define RSI_BAND                             RSI_BAND_2P4GHZ
```

## 5. Testing the Application

User has to follow the below steps for the successful execution of the application. 

### 5.1 Loading the RS9116W Firmware

Refer [Getting started with a PC](https://docs.silabs.com/rs9116/latest/wiseconnect-getting-started) to load the firmware into RS9116W EVK. The firmware file is located in `<SDK>/firmware/`


### 5.2 Building the Application on Host Platform

#### 5.2.1 Using STM32

Refer [Getting started with STM32](https://docs.silabs.com/rs9116-wiseconnect/latest/wifibt-wc-getting-started-with-stm32/)

- Open the project `<SDK>/examples/snippets/wlan/http_client_v6/projects/http_client_v6-nucleo-f411re.uvprojx`
- Build and Debug the project
- Check for the RESET pin:
  - If RESET pin is connected from STM32 to RS9116W EVK, then user need not press the RESET button on RS9116W EVK before free run.
  - If RESET pin is not connected from STM32 to RS9116 , then user need to press the RESET button on RS9116W EVK before free run.
- Add the macro RSI_CONFIGURE_IPV6=1 in the preprocessor settings (Project -> Options for Target -> C/C++ -> Preprosessor Symbols)
- Free run the project
- Then continue the common steps from **5.3**

#### 5.2.2 Using EFX32

Refer [Getting started with EFX32](https://docs.silabs.com/rs9116-wiseconnect/latest/wifibt-wc-getting-started-with-efx32/), for settin-up EFR & EFM host platforms

- Open Simplicity Studio and import the EFR32/EFM32 project from `<SDK>/examples/snippets/wlan/http_client_v6/projects`
    - Select the appropriate .slsproj as per Radio Board type mentioned in **Section 3.3** for EFR32 board.
   (or)
    - Select the *.brd2204a-gg11.slsproj  for EFM32GG11 board.
- Compile and flash the project in to Host MCU
- Debug the project
- Check for the RESET pin:
  - If RESET pin is connected from EFX32 to RS9116W EVK, then user need not press the RESET button on RS9116W EVK before free run
  - If RESET pin is not connected from EFX32 to Rs9116W EVK, then user need to press the RESET button on RS9116W EVK before free run
- Add the macro RSI_CONFIGURE_IPV6=1 in the preprocessor settings to enable IPv6 (Project -> Properties -> C/C++ Build -> Settings ->GNU ARM C Compiler ->Preprosessor)
- Free run the project
- Then continue the common steps from **5.3**

### 5.3 Common Steps

1. In Windows PC, install python and run HTTP server.

2. In release package python scripts are provided to open HTTP server in the path: utilities/script

3. Run simple_http_server_v6.py by port number 80 as argument to open HTTP server.

4. After the program gets executed, the device connects to AP and get IP.

5. After successful connection with Access Point, the Silicon Labs device request for HTTP PUT to PUT/Create the file on to the server, which is given in index.txt file and wait until put file complete.

6. Remote web server accepts a PUT request and writes the received data to a file. User can find the created new file "index.html" on Windows PC in the following path, utilities/scripts

7. After successful creation of file using HTTP PUT, Silicon Labs device request for the file "index.html" using HTTP GET method and wait until complete response receive from Server.

8. After receiving complete response for the given HTTP GET, the device post the given data in HTTP_DATA macro to HTTP server using HTTP POST method.

9. User can see the log messages at HTTP server. 

## Compressed Debug Logging

To enable the compressed debug logging feature please refer to [Logging User Guide](https://docs.silabs.com/rs9116-wiseconnect/latest/wifibt-wc-sapi-reference/logging-user-guide)