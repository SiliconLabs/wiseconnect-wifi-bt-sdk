1. Description  of "ble_provisioning_apps" folder 
===========================================================================================================
   1) android_based_provisioning_app- 
      It consists of Silabs_connect.apk application installable on Android smart phone for testing WLAN BLE provisoing applcations in Snippets.  
   2) windows_based_provisioning_app - 
      It consists Widnows based application software to run on Windows PC for testing WLAN BLE provisoing applcations in Snippets.

2. Description of "Python" folder :
===========================================================================================================
   load_certificate.py : Script to load the Server/Client certificates and keys onto the module.
   load_certificate_with_inx.py : Script to load the Server/Client certificates along with index onto the module.

   USAGE :
   python load_certificate.py <certificate_type> <certificate_path>
   python load_certificate_with_inx.py <certificate_type> <certificate_index> <certificate_path>

3. Description of "usb_cdc" folder
===========================================================================================================
   rules : rules file for linux PC to recognize RS9116
   
4. Description of "advanced_logging" folder
===========================================================================================================
   "advanced_logs_generator.py", "log_decoder.py" are the python files required for the Logging enhancement support.

5. Description of "ipv6_utils" folder
===========================================================================================================
   "inet6.c" : It is required to convert IPv6 canonical address format(deleting leading zeros, replacing Longest 0’s string with ::) to hexadecimal address format and vice versa.

   EXAMPLE:
   Canonical address format   - 2001:4367:2983:1236::9             
   Hexadecimal address format - 0x20014367, 0x29831236, 0x00000000, 0x00000009
   
6. Description of "feature_selection_tool" folder
===========================================================================================================
	RS9116_TCP_IP_Feature_Feasibility_vx.x.x.x.exe : An application to determine the static memory footprint in an RS9116 module to establish applications using either WiFi only or Wi-Fi plus Bluetooth/BLE capabilities and determine whether the desired Wi-Fi, Bluetooth Classic, Bluetooth LE and TCP/IP capabilities for a customer application can be fit within the static memory available in the RS9116 system of choice.
	
	USAGE:
	Follow the instructions given in the "readme.md" file present in the folder for application usage.

                                