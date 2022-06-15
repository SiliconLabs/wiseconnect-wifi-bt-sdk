# BLE PER

## 1. Purpose / Scope

This application demonstrates how to configure the necessary parameters to start transmitting or receiving BLE PER packets.

## 2. Prerequisites / Setup Requirements

Before running the application, the user will need the following things to setup.

### 2.1 Hardware Requirements

- Windows PC with Host interface(UART/ SPI/ SDIO).
- Silicon Labs [RS9116 Wi-Fi Evaluation Kit](https://www.silabs.com/development-tools/wireless/wi-fi/rs9116x-sb-evk-development-kit)
- Host MCU Eval Kit. This example has been tested with:
   - Silicon Labs [WSTK + EFR32MG21](https://www.silabs.com/development-tools/wireless/efr32xg21-bluetooth-starter-kit)
   - Silicon Labs [WSTK + EFM32GG11](https://www.silabs.com/development-tools/mcu/32-bit/efm32gg11-starter-kit)
   - [STM32F411 Nucleo](https://st.com/)
- Spectrum Analyzer and/or Power Analyzer


	
![figureSetup Diagram for BLE PER Example](resources/readme/images23.png) 
   			
### 2.2 Software Requirements

- [WiSeConnect SDK](https://github.com/SiliconLabs/wiseconnect-wifi-bt-sdk/)
    
- Embedded Development Environment

   - For STM32, use licensed [Keil IDE](https://www.keil.com/demo/eval/arm.htm)

   - For Silicon Labs EFx32, use the latest version of [Simplicity Studio](https://www.silabs.com/developers/simplicity-studio)
   
## 3. Application Build Environment

### 3.1 Platform

The Application can be built and executed on below Host platforms
*	[STM32F411 Nucleo](https://st.com/)
*	[WSTK + EFR32MG21](https://www.silabs.com/development-tools/wireless/efr32xg21-bluetooth-starter-kit) 
*   [WSTK + EFM32GG11](https://www.silabs.com/development-tools/mcu/32-bit/efm32gg11-starter-kit)

### 3.2 Host Interface

* By default, the application is configured to use the SPI bus for interfacing between Host platforms(STM32F411 Nucleo / EFR32MG21) and the RS9116W EVK.
* This application is also configured to use the SDIO bus for interfacing between Host platforms(EFM32GG11) and the RS9116W EVK.

### 3.3 Project Configuration

The Application is provided with the project folder containing Keil and Simplicity Studio project files.

*	Keil Project
	- The Keil project is used to evaluate the application on STM32.
	- Project path: `<SDK>/examples/featured/ble_per/projects/bt_per-nucleo-f411re.uvprojx`

*	Simplicity Studio
	- The Simplicity Studio project is used to evaluate the application on EFR32MG21.
	- Project path: 
		- If the Radio Board is **BRD4180A** or **BRD4181A**, then access the path `<SDK>/examples/featured/ble_per/projects/ble_per-brd4180a-mg21.slsproj`
		- If the Radio Board is **BRD4180B** or **BRD4181B**, then access the path `<SDK>/examples/eatured/ble_per/projects/ble_per-brd4180b-mg21.slsproj` 
        - User can find the Radio Board version as given below 

![EFR Radio Boards](resources/readme/image23a.png)

  - EFM32GG11 platform
    - The Simplicity Studio project is used to evaluate the application on EFM32GG11.
      - Project path:`<SDK>/examples/featured/ble_per/projects/ble_per-brd2204a-gg11.slsproj`
		
### 3.4 Bare Metal/RTOS Support

This application supports bare metal and RTOS environment. By default, the application project files (Keil and Simplicity Studio) are provided with bare metal configuration. 

## 4. Application Configuration Parameters

The application can be configured to suit your requirements and development environment. Read through the following sections and make any changes needed.

**4.1** Open `rsi_ble_per.c` file

**4.1.1** Update or modify following macros 

`RSI_CONFIG_PER_MODE` refers configuration mode BT PER TX or RX

	 #define RSI_CONFIG_PER_MODE	RSI_BLE_PER_TRANSMIT_MODE
                                 OR
	 #define RSI_CONFIG_PER_MODE	RSI_BLE_PER_RECEIVE_MODE

`CMD_ID` refers the command id for transmit or receive

	 #define BLE_TRANSMIT_CMD_ID	0x13
	 #define BLE_RECEIVE_CMD_ID 	0x14

`PAYLOAD_TYPE` refers type of payload to be transmitted

	 #define DATA_PRBS9	                0x00
	 #define DATA_FOUR_ONES_FOUR_ZEROES	0x01
	 #define DATA_ALT_ONES_AND_ZEROES	0x02
	 #define DATA_PRSB15	                0x03
	 #define DATA_ALL_ONES	                0x04
	 #define DATA_ALL_ZEROES	        0x05
	 #define DATA_FOUR_ZEROES_FOUR_ONES	0x06
	 #define DATA_ALT_ZEROES_AND_ONES	0x07

`LE_CHNL_TYPE`: advertising channel - 0	data channel - 1

	 #define LE_ADV_CHNL_TYPE	0
	 #define LE_DATA_CHNL_TYPE	1

`PACKET_LEN`: Length of the packet, in bytes, to be transmitted. Packet length range 0 to 255.

	 #define BLE_TX_PKT_LEN	               32

`BLE_RX_CHNL_NUM`- Receive channel index, as per the Bluetooth standard.i.e, 0 to 39
`BLE_TX_CHNL_NUM` - Transmit channel index, as per the Bluetooth standard. i.e, 0 to 39

	 #define BLE_RX_CHNL_NUM	10
	 #define BLE_TX_CHNL_NUM	10

`BLE_PHY_RATE`: ,2Mbps - 2 , 125Kbps - 4, 500Kbps - 8

	 #define LE_ONE_MBPS	        1
	 #define LE_TWO_MBPS	        2
	 #define LE_125_KBPS_CODED  	4
	 #define LE_500_KBPS_CODED  	8
	 #define BLE_PHY_RATE	LE_ONE_MBPS

`SCRAMBLER_SEED`: Initial seed to be used for whitening. It should be set to '0' in order to disable whitening.

	 #define SCRAMBLER_SEED	0

`TX_MODE`: Burst mode - 0	Continuous mode - 1

	 #define BURST_MODE	        0
	 #define CONTINUOUS_MODE	1

`HOPPING TYPE` : no hopping -0	fixed hopping - 1	random hopping - 2

	 #define NO_HOPPING	0
	 #define FIXED_HOPPING	1
	 #define RANDOM_HOPPING	2

`ANT_SEL` : onchip antenna - 2	u.f.l - 3

	 #define ONBOARD_ANT_SEL	2
	 #define EXT_ANT_SEL    	3

`RF_TYPE` : External RF – 0	Internal RF – 1

	 #define BLE_EXTERNAL_RF	0
	 #define BLE_INTERNAL_RF	1

`RF CHAIN`: Select the required RF chain

     #define NO_CHAIN_SEL	0
     #define WLAN_HP_CHAIN_BIT	0
	 #define WLAN_LP_CHAIN_BIT	1
	 #define BT_HP_CHAIN_BIT	2
	 #define BT_LP_CHAIN_BIT	3

pll_mode : PLL_MODE0 – 0	PLL_MODE1 – 1

     #define PLL_MODE_0	0
	 #define PLL_MODE_1	1
 

`LOOP_BACK_MODE` : enable 1 or disable 0 #define LOOP_BACK_MODE_DISABLE 0

     #define LOOP_BACK_MODE_ENABLE	1


Following are the non-configurable macros in the application.

`BT_GLOBAL_BUFF_LEN` refers Number of bytes required by the application and the driver

	 #define BT_GLOBAL_BUFF_LEN                             15000

   

**4.2** Open `rsi_ble_config.h` file and update/modify following macros,

	 #define RSI_BLE_PWR_INX                                30
	 #define RSI_BLE_PWR_SAVE_OPTIONS                       BLE_DISABLE_DUTY_CYCLING
	 
   **Opermode command parameters**

	 #define RSI_FEATURE_BIT_MAP                            FEAT_SECURITY_OPEN
	 #define RSI_TCP_IP_BYPASS                              RSI_DISABLE
	 #define RSI_TCP_IP_FEATURE_BIT_MAP                     TCP_IP_FEAT_DHCPV4_CLIENT
	 #define RSI_CUSTOM_FEATURE_BIT_MAP                     FEAT_CUSTOM_FEAT_EXTENTION_VALID
	 #define RSI_EXT_CUSTOM_FEATURE_BIT_MAP                 0

**Note:** `rsi_ble_config.h` files are already set with desired configuration in respective example folders user need not change for each example. 
   
## 5. Testing the Application

Follow the steps below for the successful execution of the application.

### 5.1 Loading the RS9116W Firmware

Refer [Getting started with PC ](https://docs.silabs.com/rs9116/latest/wiseconnect-getting-started) to load the firmware into RS9116W EVK. The firmware binary is located in `<SDK>/firmware/`

### 5.2 Building the Application on the Host Platform

#### 5.2.1 Using STM32

Refer [STM32 Getting Started](https://docs.silabs.com/rs9116-wiseconnect/latest/wifibt-wc-getting-started-with-efx32/)  

- Open the project `<SDK>/examples/featured/ble_per/projects/ble_per-nucleo-f411re.uvprojx` in Keil IDE.
- Build and Debug the project
- Check for the RESET pin:
	- If RESET pin is connected from STM32 to RS9116W EVK, then user need not press the RESET button on RS9116W EVK before free run.
	- If RESET pin is not connected from STM32 to RS9116W EVK, then user need to press the RESET button on RS9116W EVK before free run.
- Free run the project
- Then continue the common steps from **Section 5.3**

#### 5.2.2 Using EFX32

Refer [EFx32 Getting Started](https://docs.silabs.com/rs9116-wiseconnect/latest/wifibt-wc-getting-started-with-efx32/), for settin-up EFR & EFM host platforms

- Import the EFR32/EFM32 project from `<SDK>/examples/eatured/ble_per/projects`
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
   
1. After the program gets executed, Silicon Labs module starts BLE PER transmit or BLE PER receive. 

2. For receiving purpose use BT dongle and keep it in BLE PER RX mode.

3. Check for BLE PER stats whatever configured values are affecting or not.

4. After successful program execution of BLE PER Transmit the waveform on the spectrum looks as shown below.  

![Figure: Spectrum Analyzer 1Mbps](resources/readme/image23_spectrum.png)

5. After successful program execution the prints in coolterm looks as shown below.
     
![Prints in Coolterm window](resources/readme/image23_txlog.PNG) 
![Prints in Coolterm window](resources/readme/images23_rxlog.PNG) 