# Make File


rm=/bin/rm -f
CC= cc
DEFS=  
PROGNAME= rsi_wc_app
LINKS= -pthread -L../../../sapi/build/ -lrps
PATH_CONFIG=$(shell pwd)

RSI_DRIVER_PATH = ../../../sapi/include
RSI_ROM_PATH = ../../../sapi/rom
CFLAGS+=  -I$(RSI_DRIVER_PATH)/
CFLAGS+=  -I$(PATH_CONFIG)/
CFLAGS+=  -I$(RSI_ROM_PATH)/
CFLAGS+= -D RSI_SAMPLE_HAL
CFLAGS+= -D RSI_BLE_ENABLE

BLE_CENTRAL_APP = rsi_ble_central.o 

all: $(BLE_CENTRAL_APP) 

	make ble CC=$(CC) -C ../../../sapi/build/ path_config=$(PATH_CONFIG) 
	$(CC) $(CFLAGS) *.o $(LINKS) -o $(PROGNAME) 
	rm -f  *.o  


clean:
	$(rm) $(BLE_CENTRAL_APP) core *.o *~
