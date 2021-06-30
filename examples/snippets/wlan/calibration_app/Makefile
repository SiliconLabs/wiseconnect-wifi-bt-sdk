# Make File

rm=/bin/rm -f
CC= cc
DEFS=  
PROGNAME= rsi_wc_app
LINKS= -pthread -L../../../sapi/build/ -lrps
PATH_CONFIG=$(shell pwd)

RSI_DRIVER_PATH = ../../../sapi/include
RSI_ROM_PATH = ../../../sapi/rom

CFLAGS=  -I$(RSI_DRIVER_PATH)/
CFLAGS+=  -I$(INCLUDE_PATH)/
CFLAGS+=  -I$(RSI_ROM_PATH)/
CFLAGS+=  -I$(PATH_CONFIG)/
CFLAGS+= -D RSI_SAMPLE_HAL

RSI_CALIB_APP = rsi_calib_app.o 

all: $(RSI_CALIB_APP) 

	make wlan CC=$(CC) -Wall -C ../../../sapi/build/ path_config=$(PATH_CONFIG) 
	$(CC) $(CFLAGS) *.o $(LINKS) -o $(PROGNAME) 
	rm -f  *.o  


clean:
	$(rm) $(RSI_CALIB_TEST_APP) core *.o *~ rsi_wc_app
