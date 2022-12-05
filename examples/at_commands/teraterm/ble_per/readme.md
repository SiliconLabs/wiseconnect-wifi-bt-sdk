## BLE PER Mode Tera Term Script
This application demonstrates how to configure the RS9116W EVK in BLE PER mode.

Before continuing, ensure the RS9116W EVK is plugged into your computer and is connected via Tera Term as described in [Getting Started with PC using AT Commands](http://docs.silabs.com/rs9116-wiseconnect/latest/wifibt-wc-getting-started-with-pc/). 

## Contents
  - [Setting up the RS9116](#setting-up)
  - [Transmit PER instructions](#transmit-per-instructions)
  - [Receive PER instructions](#receive-per-instructions)

## Setting up

**STEP 1.** Reset the RS9116W EVK.

**STEP 2.** In the Tera Term menu select `Control->Macro`.

![Tera Term Macro](./resources/tera-term-macro-1.png)
	
**STEP 3.** Navigate to the folder `<SDK>/examples/at_commands/teraterm/ble_per` containing the example script and select the file `ble_per.ttl`.

![Tera Term Script to be selected](./resources/tera-term-ble-per.png)

**STEP 4.** After selecting the `ble_per.ttl` script, a popup shows to indicate the BLE PER script is running. Two additional popups then show to indicate auto baud and the `opermode` command is successul.

![Script execution start](./resources/script-start-pop-up.png)

![ABRD State](./resources/firmware-loading-done-2.png)

![opermode success](./resources/ble-opermode-3.png)

## Transmit PER Instructions
The following steps describe how to configure the RS9116W for BLE Transmit PER measurement.

**Step 1.** Enter `1` to run the script in transmit PER mode.

![select the tarnsmit/receive](./resources/ble-per-transmit-4.png)

**STEP 2.** Enter `1` to enable PER mode.

![Enable PER mode](./resources/ble-per-enable-5.png)

**Step 3.** Enter the 32-bit device address in hex format. In the following example, the access address of the BLE PER packet is `0x71764129`.

![Enter the device address](./resources/ble-devices-address-6.png)

**Step 4.** Enter the desired PHY data rate.

![Phy rate election](./resources/ble-phy-rate-7.png)

**Step 5.** Enter the Receive (Rx) and then Transmit (Tx) channel numbers.

![rx_channel_number](./resources/ble-rx-channel-8.png)

![tx_channel_number](./resources/ble-tx-channel-9.png)

**Step 6.** Enter the scrambler seed according to the PER mode.

![scrmbler-seed](./resources/ble-scrambler-seed-9.png)

**Step 7.** Enter the desired `le channel type`.

![Le_channel_type](./resources/ble-channel-type-10.png)

**Step 8.** Enter the desired `hopping type`.

![Hopping type](./resources/hopping-type-11.png)

**Step 9.** Enter the desired `antenna type`.

![Antenna_selection](./resources/ble-antenna-sel-11.png)

**Step 10.** Enter the desired `rf chain`.

![RFChain selection](./resources/ble-rf-chain-12.png)

**Step 11.** Enter the desired `payload type`.

![Packet Length](./resources/payload-type-13.png)

**Step 12.** Enter the desired `tx power`.

![tx_power_index](./resources/tx-power-index-14.png)

**Step 13.** Enter the desired PER transmission mode.

![tx_mode](./resources/ble-tx-mode-15.png)

**Step 14.** Enter the desired number of packets to transmit.

![number of packets](./resources/number-of-pkts.png)

**Step 15.** With all inputs successfully entered, PER packet(s) are transmitted. 

![Final screen-shot](./resources/ble-transmit-executed-16.png)

![Final command prompt](./resources/ble-tx-command-prompt-17.png)


## Receive PER Instructions
The following steps describe how to configure the RS9116W for BLE Receive PER measurement.

**Step 1.** Enter `2` to run the script in PER receive mode.

![PER Receive](./resources/ble-per-rx-1.png)

**STEP 2.** Enter `1` to enable PER mode.

![Enable PER mode](./resources/ble-per-enable-5.png)

**Step 3.** Enter the 32-bit device address in hex format. In the following example, the access address of the BLE PER packet is `0x71764129`.

![Enter the device address](./resources/device-address-rx.png)

**Step 4.** Enter the desired PHY data rate.

![Phy rate election](./resources/ble-phy-rate-rx.png)

**Step 5.** Enter the Receive (Rx) and then Transmit (Tx) channel numbers.

![rx_channel_number](./resources/rx-channel-rx.png)

![tx_channel_number](./resources/tx-channel-rx.png)

**Step 6.** Enter the scrambler seed according to the PER mode.

![scrmbler-seed](./resources/ble-scrambler-seed-9.png)

**Step 7.** Enter the desired `le_channel type`.

![Le_channel_type](./resources/rate-rx.png)

**Step 8.** Enter the desired `hopping type`.

![Hopping type](./resources/hopping-type-rx.png)

**Step 9.** Enter the desired `antenna type`.

![Antenna_selection](./resources/antenna-selection-rx.png)

**Step 10.** Enter the desired `rf_chain`.

![RFChain selection](./resources/lp-chain-election-rx.png)

**Step 11.** Enter the desired extended data length option.

![Extended data length ind](./resources/ble-extended-data-length.png)

**Step 12.** Enter the desired loop back mode option.

![Loop-back-mode](./resources/loop-back-mode.png)

**Step 13.** Enter the desired duty cycling option.

![Duty cycling](./resources/duty-cycling.png)

**Step 14.** With all inputs successfully entered, the RS9116W waits to receive packets.   

![Receive command](./resources/ble-receive-success.png)

![PER Stats command](./resources/ble-per-stats.png)

Refer to the [BLE Programming Reference Manual](https://docs.silabs.com/rs9116/wiseconnect/rs9116w-ble-at-command-prm/latest/08-ble-commands) for further details on how to change the parameters.
