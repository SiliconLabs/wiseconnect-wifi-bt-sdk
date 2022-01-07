#update gain table

## 1. Purpose / Scope

This application demonstrates how gain table offset/max power details can be updated.
This application is used to configure region based gain table the module from user. This method is used for overwriting default region based gain tables that are present in firmware.

There are two types of gain tables for BT and BLE protocols
* Gain table holding Max Tx power values
* Gain table with Max power vs offset values

Customer must configure both gain tables mentioned above in the same order to apply the changes in the firmware properly.

## 2. Prerequisites / Setup Requirements

Before running the application, the user will need the following things to setup.

### 2.1 Hardware Requirements

- Windows PC with Host interface(UART/ SPI/ SDIO).
- Silicon Labs [RS9116 Wi-Fi Evaluation Kit](https://www.silabs.com/development-tools/wireless/wi-fi/rs9116x-sb-evk-development-kit)
- Host MCU Eval Kit. This example has been tested with:
   - Silicon Labs [WSTK + EFR32MG21](https://www.silabs.com/development-tools/wireless/efr32xg21-bluetooth-starter-kit)
   - Silicon Labs [WSTK + EFM32GG11](https://www.silabs.com/development-tools/mcu/32-bit/efm32gg11-starter-kit)
   - [STM32F411 Nucleo](https://st.com/)

  
### 2.2 Software Requirements

- [WiSeConnect SDK](https://github.com/SiliconLabs/wiseconnect-wifi-bt-sdk/)
    
- Embedded Development Environment

   - For STM32, use licensed [Keil IDE](https://www.keil.com/demo/eval/arm.htm)

   - For Silicon Labs EFx32, use the latest version of [Simplicity Studio](https://www.silabs.com/developers/simplicity-studio)
   
- Download and install the Silicon Labs [EFR Connect App](https://www.silabs.com/developers/efr-connect-mobile-app) in the android smart phones for testing BLE applications. Users can also use their choice of BLE apps available in Android/iOS smart phones. 

- Download and install the Silicon Labs [Bluetooth SPP manager](https://play.google.com/store/apps/details?id=at.rtcmanager&hl=en_IN) in the android smart phones for testing BT applications. Users can also use their choice of BT apps available in Android/iOS smart phones.

## 3. Application Build Environment

### 3.1 Platform

The Application can be built and executed on below Host platforms
* [STM32F411 Nucleo](https://st.com/)
* [WSTK + EFR32MG21](https://www.silabs.com/development-tools/wireless/efr32xg21-bluetooth-starter-kit) 
* [WSTK + EFM32GG11](https://www.silabs.com/development-tools/mcu/32-bit/efm32gg11-starter-kit)

### 3.2 Host Interface

* By default, the application is configured to use the SPI bus for interfacing between Host platforms(STM32F411 Nucleo / EFR32MG21) and the RS9116W EVK.
* This application is also configured to use the SDIO bus for interfacing between Host platforms(EFM32GG11) and the RS9116W EVK.

### 3.3 Project Configuration

The Application is provided with the project folder containing Keil and Simplicity Studio project files.

* Keil Project
  - The Keil project is used to evaluate the application on STM32.
  - Project path: `<SDK>/examples/snippets/bt_ble/update_gain_table/projects/update_gain_table-nucleo-f411re.uvprojx`

* Simplicity Studio
  - The Simplicity Studio project is used to evaluate the application on EFR32MG21.
  - Project path: 
    - If the Radio Board is **BRD4180A** or **BRD4181A**, then access the path `<SDK>/examples/snippets/bt_ble/update_gain_table/projects/update_gain_table-brd4180a-mg21.slsproj`
    - If the Radio Board is **BRD4180B** or **BRD4181B**, then access the path `<SDK>/examples/snippets/bt_ble/update_gain_table/projects/update_gain_table-brd4180b-mg21.slsproj` 
        - User can find the Radio Board version as given below 

![Figure: EFR Radio Boards](resources/readme/image82a.png)

  - EFM32GG11 platform
    - The Simplicity Studio project is used to evaluate the application on EFM32GG11.
      - Project path:`<SDK>/examples/snippets/bt_ble/update_gain_table/projects/update_gain_table-brd2204a-gg11.slsproj`
    
### 3.4 Bare Metal/RTOS Support

This application supports bare metal and RTOS environment. By default, the application project files (Keil and Simplicity Studio) are provided with bare metal and RTOS configuration. 


## 4. Application Configuration Parameters

The application can be configured to suit your requirements and development environment. Read through the following sections and make any changes needed.

**4.1** `rsi_wlan_config.h` file and update/modify following macros,

**4.1.1** User must update the below parameters 
    
```c
#define CONCURRENT_MODE                RSI_DISABLE
#define RSI_FEATURE_BIT_MAP            FEAT_SECURITY_OPEN
#define RSI_TCP_IP_BYPASS              RSI_DISABLE
#define RSI_TCP_IP_FEATURE_BIT_MAP     TCP_IP_FEAT_DHCPV4_CLIENT
#define RSI_CUSTOM_FEATURE_BIT_MAP     FEAT_CUSTOM_FEAT_EXTENTION_VALID
#define RSI_EXT_CUSTOM_FEATURE_BIT_MAP EXT_FEAT_384K_MODE 
```

**4.1.2** Open `rsi_update_gain_table.c` file and update/modify following macros:

To set the opermode update RSI_BT_BLE_MODE macro. This can be set to :
12 - BLE only mode
4 - BT only mode
9 - Dual mode 
By default RSI_BT_BLE_MODE is set to Dual Mode i.e. 9
```
#define RSI_BT_BLE_MODE 9
```
Following is the API which updates the gain table.
```
int32_t rsi_bt_cmd_update_gain_table_offset_or_max_pwr(uint8_t node_id,
                                                       uint8_t payload_len,
                                                       uint8_t *payload,
                                                       uint8_t req_type)
```
---
**NOTE**
* This command must be used immediately after opermode request
* Internally firmware maintains two tables
    * Gain table holding Max Tx power values for all regions
    * Gain table with Max power vs offset values for each channel of all regions
* There are 5 regions supported and are FCC, ETSI, TELEC, KCC, WORLDWIDE. These FCC/ETSI/TELEC/KCC gain table max power level and offset values should be loaded in end-to-end mode via BT/BLE User Gain table. This has to be called upon every boot-up since this information is not saved inside flash. SoC uses these tables in FCC/ETSI/TELEC/KCC to limit power and not to violate allowed limits.
* For Worldwide region firmware uses Worldwide values for Tx. For other regions(FCC/ETSI/TELEC/KCC), Firmware uses min value out of Worldwide & Region based values for Tx.  Also there will be part to part variation across chips and offsets are estimated during manufacturing flow which will be applied as correction factor during normal mode of operation.
**This frame has to be used by customers who has done FCC/ETSI/TELEC/KCC certification with their own antenna.  All other customers should not use this. Inappropriate use of this frame may result in violation of FCC/ETSI/TELEC/KCC or any certifications and Silicon labs is not liable for that.**
---
**req_type** can be set to one of the following macros :
```
#define UPDATE_GAIN_TABLE_MAX_POWER 0  \\ For updating Gain table Max power
#define UPDATE_GAIN_TABLE_OFFSET    1  \\ For updating Gain table offset
```
**node_id** can be set to one of the following macros :
```
#define BLE_NODE 0  \\ For selecting BLE node
#define BT_NODE  1  \\ For selecting BT node
```
Following arrays will be used to update_gain_table based on 'node_id' and 'req_type',
|node_id    |req_type                    |Payload Array                                       |
|-----------|--------------------------- |----------------------------------------------------|
|BLE_NODE   |UPDATE_GAIN_TABLE_MAX_POWER |_RS9116_BLE_REGION_BASED_MAXPOWER_XX                |
|BLE_NODE   |UPDATE_GAIN_TABLE_OFFSET    |_RS9116_BLE_REGION_BASED_MAXPOWER_VS_OFFSET_XX      |
|BT_NODE    |UPDATE_GAIN_TABLE_MAX_POWER |_RS9116_BT_REGION_BASED_MAXPOWER_XX                 |
|BT_NODE    |UPDATE_GAIN_TABLE_OFFSET    |_RS9116_BT_REGION_BASED_MAXPOWER_VS_OFFSET_XX       |

**Gain Table Max Power Array format**

uint8_t **_RS9116_BT/BLE_REGION_BASED_MAXPOWER_XX[]** = {}; //! Fill the user gain table max power values in the below mentioned way.

<TABLE NAME>[] = { 
                <REGION NAME 1>, <MAX POWER>, 
                <REGION NAME 1>, <MAX POWER>,
                  .
                  .
                <REGION NAME N>, <MAX POWER> 
                  };

**Gain Table Offset array format**

uint8_t **_RS9116_BT/BLE_REGION_BASED_MAXPOWER_VS_OFFSET_XX[]** = {}; //! Fill the user gain table offset values in the below mentioned way.
 
<TABLE NAME>[] = {
                  <Number Of Regions - 'r'>, 
                    <REGION NAME 1>, <Number Of Channels - 'm'>,
                      <CHANNEL NUMBER 1>, <OFFSET>, 
                      <CHANNEL NUMBER 2>, <OFFSET>,
                      . 
                      .
                      <CHANNEL NUMBER m>, <OFFSET>,
                    <REGION NAME 2>, <No Of Channels - 'n'>,
                      <CHANNEL NUMBER 1>, <OFFSET>, 
                      <CHANNEL NUMBER 2>, <OFFSET>,
                      . 
                      .
                      <CHANNEL NUMBER n>, <OFFSET>,
                     .
                     .
                     <REGION NAME r>, <No Of Channels - 'n'>,
                       <CHANNEL NUMBER 1>, <OFFSET>, 
                       <CHANNEL NUMBER 2>, <OFFSET>,
                       .
                       .
                       <CHANNEL NUMBER n>, <OFFSET>,
                   };

**Table of Region name and its corresponding value**
|  Region Name | Value   | 
|--------------|---------|
| FCC          | 0       |
| ETSI         | 1       | 
| TELEC        | 2       |
| WORLDWIDE    | 3       |
| KCC          | 4       |




## 5. Testing the Application

Follow the steps below for the successful execution of the application.

### 5.1 Loading the RS9116W Firmware

Refer [Getting started with PC ](https://docs.silabs.com/rs9116/latest/wiseconnect-getting-started) to load the firmware into RS9116W EVK. The firmware binary is located in `<SDK>/firmware/`


### 5.2 Building the Application on the Host Platform

### 5.2.1 Using STM32

Refer [STM32 Getting Started](https://docs.silabs.com/rs9116-wiseconnect/latest/wifibt-wc-getting-started-with-efx32/)  

- Open the project `<SDK>/examples/snippets/bt_ble/update_gain_table/projects/update_gain_table-nucleo-f411re.uvprojx` in Keil IDE.
- Build and Debug the project
- Check for the RESET pin:
  - If RESET pin is connected from STM32 to RS9116W EVK, then user need not press the RESET button on RS9116W EVK before free run.
  - If RESET pin is not connected from STM32 to RS9116W EVK, then user need to press the RESET button on RS9116W EVK before free run.
- Free run the project
- Then continue the common steps from **Section 5.3**


#### 5.2.2 Using EFX32

Refer [EFx32 Getting Started](https://docs.silabs.com/rs9116-wiseconnect/latest/wifibt-wc-getting-started-with-efx32/), for settin-up EFR & EFM host platforms

- Import the EFR32/EFM32 project from `<SDK>/examples/snippets/bt_ble/update_gain_table/projects`
    - Select the appropriate .slsproj as per Radio Board type mentioned in **Section 3.3** for EFR32 board.
   (or)
    - Select the *.brd2204a-gg11.slsproj  for EFM32GG11 board.
- Compile and flash the project in to Host MCU
- Debug the project
- Check for the RESET pin:
  - If RESET pin is connected from STM32 to RS9116W EVK, then user need not press the RESET button on RS9116W EVK before free run
  - If RESET pin is not connected from STM32 to RS9116W EVK, then user need to press the RESET button on RS9116W EVK before free run
- Free run the project
- Then continue the common steps from **Section 5.3**

### 5.3 Common Steps

   1. Connect Silicon Labs device to the Windows PC running KEIL or IAR IDE

   2. Build and launch the application.

   3. After the program gets executed, you can check the status of the gain table updation on serial terminal.
