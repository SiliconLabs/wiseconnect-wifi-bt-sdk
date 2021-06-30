# Make File

rm=/bin/rm -f
CC= cc
DEFS=  
PROGNAME= rsi_wc_app
LINKS= -pthread -L../../../sapi/build/ -lrps
PATH_CONFIG=$(shell pwd)

INCLUDE_PATH = ../../../utilities/certificates
AWS_INCLUDE_PATH = ../../../third_party/aws_sdk/include
AWS_PLATFORM_PATH = ../../../third_party/aws_sdk/platform/silabs/inc
RSI_DRIVER_PATH = ../../../sapi/include
PATH_CONFIG=$(shell pwd)

CFLAGS=  -I$(RSI_DRIVER_PATH)/
CFLAGS+=  -I$(PATH_CONFIG)/
CFLAGS+=  -I$(AWS_INCLUDE_PATH)/
CFLAGS+=  -I$(AWS_PLATFORM_PATH)/
CFLAGS+=  -I$(INCLUDE_PATH)\
          -I ../../../sapi/rom/

CFLAGS+= -DRSI_SAMPLE_HAL
CFLAGS+= -D RSI_BLE_ENABLE

WLAN_STATION_BLE_BRIDGE_APP = main.o \
                      rsi_wlan_app.o \
                      rsi_ble_app.o 

all: $(WLAN_STATION_BLE_BRIDGE_APP) 

	make wlan_ble_aws CC=$(CC) -C ../../../sapi/build/ path_config=$(PATH_CONFIG) 
	$(CC) $(CFLAGS) *.o $(LINKS) -o $(PROGNAME) 
	rm -f  *.o  


clean:
	$(rm) $(WLAN_STATION_BLE_BRIDGE_APP) core *.o *~ rsi_wc_app
