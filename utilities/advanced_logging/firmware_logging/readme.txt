Description  of "firmware_logging" folder
===========================================================================================================
   Below files are required for the firmware logging enhancement support,

   firmware_logging_utils.c : Script having utilities for handling of firmware logs. 
   manifest.json : JSON file to load inputs required for decoding the firmware logs.
   fw_log_decoder.py : Script to decode firmware logs.

   NOTE:
   firmware_logging_utils.c represents a mechanism of handling the incoming firmware logs using 
   queue, to avoid the consumption of data in driver (callback) context.

   More details can be found at the link below,
   https://docs.silabs.com/rs9116-wiseconnect/latest/wifibt-wc-sapi-reference/logging-user-guide