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

![opermode success](./resources/opermode-success-3.png)

Please follow the below steps when you select the BLE_PER as "Transmit" .

**Step 5.** Enter the value to run the script in PER "Transmit/Receive" mode.

![select the tarnsmit/receive](./resources/per-tx-rx-4.png)

**STEP 6.** Enter the value to enable/disable the PER mode.

![Enable PER mode](./resources/per-enable-5.png)

**Step 7.** Enter the device address. It is a 32-bit address in Hex format (Access address of BLE PER packet : 0x71764129)

![Enter the device address](./resources/device_address-6.png)

**Step 8.** Enter the desired 'phyrate' which is mentioned in below figure into the input box.

![Phy rate election](./resources/ble-phy-rate-7.png)

**Step 9.** Enter the rx and tx channel number.

![rx_channel_number](./resources/rx-channel-number-8.png)

![tx_channel_number](./resources/tx-channel-number-9.png)

**Step 10.** As per the PER mode give the "scrambler-seed" value.

![scrmbler-seed](./resources/scrambler-seed-9.png)

**Step 11.** Enter the "le_channel" type as per your requirement.

![Le_channel_type](./resources/data-adv-channel-10.png)

**Step 12.** Enter the "hopping-type" as per your requirement.

![Hopping type](./resources/hopping-type-11.png)

**Step 13.** Enter the "antenna_type" as per your requirement.

![Antenna_selection](./resources/antenna-sel-11.png)

**Step 14.** Enter the "rf_chain" as per your requirement.

![RFChain selection](./resources/rf-chain-12.png)

**Step 15.** Enter the "payload_type" which is listed in below image.

![Packet Length](./resources/payload-type-13.png)

**Step 16.** Enter the "tx_power" index which is listed in below image.

![tx_power_index](./resources/tx-power-index-14.png)

**Step 17.** Enter the "PER transmission mode".

![tx_mode](./resources/tx-mode-15.png)

**Step 18.** Enter the "number_of_packets" based on the selected "tx_mode" parameter.

![number of packets](./resources/number-of-pkts.png)

**Step 19.** After giving all the inputs BLE Transmit command executed successfully.

![Final screen-shot](./resources/ble-transmit-executed-16.png)

Please follow the below steps when you select the BLE_PER as "Receive".

**Step 1.** Enter the value to run the script in PER "Transmit/Receive" mode.

![PER Receive](./resources/ble-per-rx-1.png)

**STEP 2.** Enter the value to enable/disable the PER mode.

![Enable PER mode](./resources/per-enable-5.png)

**Step 3.** Enter the device address. It is a 32-bit address in Hex format (Access address of BLE PER packet : 0x71764129)

![Enter the device address](./resources/device_address-6.png)

**Step 4.** Enter the desired 'phyrate' which is mentioned in below figure into the input box.

![Phy rate election](./resources/ble-phy-rate-7.png)

**Step 5.** Enter the rx and tx channel numbers.

![rx_channel_number](./resources/rx-channel-number-8.png)

![tx_channel_number](./resources/tx-channel-number-9.png)

**Step 6.** As per the PER mode give the "scrambler-seed" value.

![scrmbler-seed](./resources/scrambler-seed-9.png)

**Step 7.** Enter the "le_channel" type as per your requirement.

![Le_channel_type](./resources/data-adv-channel-10.png)

**Step 8.** Enter the "hopping-type" as per your requirement.

![Hopping type](./resources/hopping-type-11.png)

**Step 9.** Enter the "antenna_type" as per your requirement.

![Antenna_selection](./resources/antenna-sel-11.png)

**Step 10.** Enter the "rf_chain" as per your requirement.

![RFChain selection](./resources/rf-chain-12.png)

**Step 11.** Enter the "Extnd_data_length_ind" which is listed in below image.

![Extended data length ind](./resources/extended-data-length-indication-rx.png)

**Step 12.** Enter the "loop-back" mode .

![Loop-back-mode](./resources/loop-back-mode-rx.png)

**Step 13.** Enter the "Duty cycling" .

![Duty cycling](./resources/duty-cycling-rx.png)

**Step 14.** After giving all the inputs BLE PER Receive command executed and PER stats command also executes simultaneously.

![Receive command](./resources/receive-executed-rx.png)

![PER Stats command](./resources/stats-command-executed-rx.png)

![Final per receive and stats command](./resources/per-receive-screen-shot-rx.png)


Refer to the [BLE Programming Reference Manual](https://docs.silabs.com/rs9116/wiseconnect/rs9116w-ble-at-command-prm/latest/08-ble-commands) for further details on how to change the parameters.
