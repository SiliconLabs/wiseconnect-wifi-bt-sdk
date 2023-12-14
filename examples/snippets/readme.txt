Follow the below description to execute examples in snippets folder.

snippets folder contains following examples
============================================
1. ble_central
2. ble_datalength
3. ble_dual_role
4. ble_heart_rate_profile
5. ble_ibeacon
6. ble_longrange_2mbps
7. ble_power_save
8. ble_privacy
9. ble_secureconnection
10. ble_testmodes
11. ble_throughput_app
12. ble_accept_list
13. gatt_long_read
14. hid_on_gatt
15. bt_power_save
16. bt_spp_dual_role_with_secure_pairing
17. bt_ble_dual_mode
18. update_gain_table
19. ram_dump
20. access_point
21. calibration_app
22. cloud_apps
    |- aws_iot\device_shadow_logging_stats
	|- aws_iot\mqtt
	|- azure_iot
23. concurrent_mode
24. embedded_mqtt
25. enterprise_client
26. ftp_client
27. http_client
28. http_otaf
29. https_client
30. instant_bgscan
31. mqtt_client
32. power_save_deep_sleep
33. sntp_client
34. socket_select_app
35. socket_select_app_udp
36. station_ping
37. tcp_client
38. tcp_client_twt
39. tcp_logging_stats
40. three_ssl_concurrent_client_sockets
41. tls_client
42. udp_client
43. wifi6_mimo
44. wifi6_mqtt_twt
45. wifi6_twt_tcp_client
46. access_point_v6
47. http_client_v6
48. mqtt_client_v6
49. sntp_client_v6
50. station_ping_v6
41. throughput_dual_ip_v4_v6
51. wlan_throughput_v6
52. wlan_ble_power_save
53. wlan_station_ble_bridge
54. wlan_station_ble_provisioning
55. wlan_station_ble_provisioning_aws
56. wlan_station_ble_throughput_app
57. unified_ae_coex_app
58. ble_ae_central
59. ble_ae_peripheral
60. twt_use_case_remote_app
61. wifi6_twt_use_case_demo
62. wlan_station_ble_provisioning_select
	
The above examples can be tested on
1. Simplicity studio/EFR & EFM platform.
2. Baremetal and FreeRTOS

snippets folder contains following examples in FreeRTOS only
==============================================================
	
1. ble_multiconnection_gatt_test  
2. wlan_https_bt_spp_ble_dual_role
3. wlan_throughput_bt_spp_ble_dual_role
	
The above examples can be tested on 
1. Simplicity studio/EFR & EFM platform.
2. In FreeRTOS only
	
4.bt_a2dp_source_with_avrcp
5.wlan_https_bt_a2dp
6.wlan_https_bt_a2dp_ble_dual_role

The above example can be tested on 
1. IAR/NXP RT595 platform.
2. In FreeRTOS only
	
Example folder contents
========================
1. 'projects' folder which has simplicity studio project files which are used to test the example.
2. 'resources' and 'readme.md' - documentaion for the example
3. source files

For ex: 
=======
ble_central/projects consists of two projects
-for simplicity studio - please access below project file for executing the example on EFR platform.
	ble_central-brd4180b-mg21.slsproj
-for simplicity studio - please access below project file for executing the example on EFM platform.
	ble_central-brd2204a-gg11.slsproj
