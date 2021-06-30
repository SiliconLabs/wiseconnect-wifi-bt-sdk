# Makefile
PROGNAME= rsi_wc_app
RSI_SDK_PATH = ../../../
include $(RSI_SDK_PATH)/platforms/linux/linux_build.mk

LINKS= -pthread  -lrps
PATH_CONFIG=$(shell pwd)


RSI_DRIVER_PATH = $(RSI_SDK_PATH)/sapi/include

CFLAGS=  -I$(RSI_DRIVER_PATH)/
CFLAGS+= -D RSI_SAMPLE_HAL

AP_START_APP = rsi_ap_start.o 

all: $(AP_START_APP) 

	make wlan CC=$(CC) -Wall -C ../../../build/ path_config=$(PATH_CONFIG) 
	$(CC) $(CFLAGS) *.o $(LINKS) -o $(PROGNAME) 
	rm -f  *.o  


clean:
	$(rm) $(AP_START_APP) core *.o *~ rsi_wc_app
