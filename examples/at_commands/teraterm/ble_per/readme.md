## BLE PER Mode Tera Term Script
This application demonstrates how to configure the RS9116W EVK in BLE PER mode.

Before continuing, ensure that the RS9116W EVK is plugged into your computer and is connected via Teraterm as described in [Getting Started with PC using AT Commands](http://docs.silabs.com/rs9116-wiseconnect/2.4/wifibt-wc-getting-started-with-pc/). 

**STEP 1.** Reset the RS9116W EVK.

**STEP 2.** In the Tera Term menu select `Control->Macro`.

![Tera Term Macro](./resources/tera-term-macro-1.png)
	
**STEP 3.** Navigate to the folder `<SDK>/examples/at_commands/teraterm/ble_per` which contains the example script and select the file `ble_per.ttl`.

![Tera Term Script to be selected](./resources/tera-term-ble-per.png)



**STEP 4.** After selecting the ble_per.ttl script from tera-term macro, it will start executing.
1. ABRD.
2. Opermode.

![Script execution start](./resources/script-start-pop-up.png)

![ABRD State](./resources/firmware-loading-done-2.png)

![opermode success](./resources/ble-opermode-3.png)

Please follow the below steps when you select the BLE_PER as "Transmit" .

**Step 5.** Enter the value to run the script in PER "Transmit/Receive" mode.

![select the tarnsmit/receive](./resources/ble-per-transmit-4.png)

**STEP 6.** Enter the value to enable/disable the PER mode.

![Enable PER mode](./resources/ble-per-enable-5.png)

**Step 7.** Enter the device address. It is a 32-bit address in Hex format (Access address of BLE PER packet : 0x71764129)

![Enter the device address](./resources/ble-devices-address-6.png)

**Step 8.** Enter the desired 'phyrate' which is mentioned in below figure into the input box.

![Phy rate election](./resources/ble-phy-rate-7.png)

**Step 9.** Enter the rx and tx channel number.

![rx_channel_number](./resources/ble-rx-channel-8.png)

![tx_channel_number](./resources/ble-tx-channel-9.png)

**Step 10.** As per the PER mode give the "scrambler-seed" value.

![scrmbler-seed](./resources/ble-scrambler-seed-9.png)

**Step 11.** Enter the "le_channel" type as per your requirement.

![Le_channel_type](./resources/ble-channel-type-10.png)

**Step 12.** Enter the "hopping-type" as per your requirement.

![Hopping type](./resources/hopping-type-11.png)

**Step 13.** Enter the "antenna_type" as per your requirement.

![Antenna_selection](./resources/ble-antenna-sel-11.png)

**Step 14.** Enter the "rf_chain" as per your requirement.

![RFChain selection](./resources/ble-rf-chain-12.png)

**Step 15.** Enter the "payload_type" which is listed in below image.

![Packet Length](./resources/payload-type-13.png)

**Step 16.** Enter the "tx_power" index which is listed in below image.

![tx_power_index](./resources/tx-power-index-14.png)

**Step 17.** Enter the "PER transmission mode".

![tx_mode](./resources/ble-tx-mode-15.png)

**Step 18.** Enter the "number_of_packets" based on the selected "tx_mode" parameter.

![number of packets](./resources/number-of-pkts.png)

**Step 19.** After giving all the inputs BLE Transmit command executed successfully.

![Final screen-shot](./resources/ble-transmit-executed-16.png)

### BLE CW-Mode Selection

**Step 20.** To run the BLE_CW mode all the parameters you need to enter as mentioned step by step procedure in above
method. But "Scrambler_seed" parameter you need to give input as 5.

![Scrambler_seed](./resources/ble-scrambler-cw-mode.png)

**Step 21.**  After "Scrambler_seed" parameter script will ask all the inputs as mentioned in above BLE_Trasmit method, you
need to enter .

**Step 22.** All the necessary inputs are completed , final input will ask for "Continuous Wave" selection you need to enter
2.Like as below pic.

![PER mode selection](./resources/ble-cw-mode-selection.png)

**Step 23.** After giving all the inputs BLE CW mode Transmit command executed successfully.

![CW mode success](./resources/ble-cw-mode-executed.png)

![CW mode Final](./resources/ble-cw-mode-final.png)

Please follow the below steps when you select the BLE_PER as "Receive".

**Step 1.** Enter the value to run the script in PER "Transmit/Receive" mode.

![PER Receive](./resources/ble-per-rx-1.png)

**STEP 2.** Enter the value to enable/disable the PER mode.

![Enable PER mode](./resources/ble-per-enable-5.png)

**Step 3.** Enter the device address. It is a 32-bit address in Hex format (Access address of BLE PER packet : 0x71764129)

![Enter the device address](./resources/ble-devices-address-6.png)

**Step 4.** Enter the desired 'phyrate' which is mentioned in below figure into the input box.

![Phy rate election](./resources/ble-phy-rate-7.png)

**Step 5.** Enter the rx and tx channel numbers.

![rx_channel_number](./resources/ble-rx-channel-8.png)

![tx_channel_number](./resources/ble-tx-channel-9.png)

**Step 6.** As per the PER mode give the "scrambler-seed" value.

![scrmbler-seed](./resources/ble-scrambler-seed-9.png)

**Step 7.** Enter the "le_channel" type as per your requirement.

![Le_channel_type](./resources/ble-channel-type-10.png)

**Step 8.** Enter the "hopping-type" as per your requirement.

![Hopping type](./resources/hopping-type-11.png)

**Step 9.** Enter the "antenna_type" as per your requirement.

![Antenna_selection](./resources/ble-antenna-sel-11.png)

**Step 10.** Enter the "rf_chain" as per your requirement.

![RFChain selection](./resources/ble-rf-chain-12.png)

**Step 11.** Enter the "Extnd_data_length_ind" which is listed in below image.

![Extended data length ind](./resources/ble-extended-data-length.png)

**Step 12.** Enter the "loop-back" mode .

![Loop-back-mode](./resources/ble-loop-back-mode.png)

**Step 13.** Enter the "Duty cycling" .

![Duty cycling](./resources/ble-duty-cycling.png)

**Step 14.** After giving all the inputs BLE PER Receive command executed and PER stats command also executes simultaneously.

![Receive command](./resources/ble-receive-success.png)

![PER Stats command](./resources/ble-per-stats.png)

![Final per receive and stats command](./resources/ble-per-stats-final.png)

### BLE CW-Mode Selection

**Step 15.** To run the BLE_CW mode all the parameters you need to enter as mentioned step by step procedure in above
method. But "Scrambler_seed" parameter you need to give input as 5.

![Scrambler_seed](./resources/ble-scrambler-cw-mode.png)

**Step 16.**  After "Scrambler_seed" parameter script will ask all the inputs as mentioned in above BLE_Trasmit method, you
need to enter .

**Step 17.** All the necessary inputs are completed , final input will ask for "Continuous Wave" selection you need to enter
2.Like as below pic.

![PER mode selection](./resources/ble-cw-mode-selection.png)

**Step 18.** After giving all the inputs BLE PER CW mode Receive command executed and PER stats command also executes simultaneously.

![CW mode success](./resources/ble-cw-receive.png)

![CW mode Final](./resources/ble-cw-rx-stats-command.png)


Refer to the [BLE Programming Reference Manual](https://docs.silabs.com/rs9116/wiseconnect/rs9116w-ble-at-command-prm/latest/08-ble-commands) for further details on how to change the parameters.
