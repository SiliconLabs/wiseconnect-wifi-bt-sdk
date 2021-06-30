# Makefile
PROGNAME= rsi_wc_app
RSI_SDK_PATH = ../../../
include $(RSI_SDK_PATH)/platforms/linux/linux_build.mk


LINKS= -pthread -L../../../../../build/ -lrps
PATH_CONFIG=$(shell pwd)

INCLUDE_PATH = ../../../../utilities/certificates
RSI_DRIVER_PATH = $(RSI_SDK_PATH)/sapi/../../include
AWS_INCLUDE_PATH = ../../../../../nwk/applications/aws_sdk/include
AWS_NWK_INCLUDE_PATH = ../../../../../nwk/applications/aws_sdk/platform/inc


CFLAGS=  -I$(RSI_DRIVER_PATH)/
CFLAGS+=  -I$(INCLUDE_PATH)/
CFLAGS+=  -I$(AWS_INCLUDE_PATH)/
CFLAGS+=  -I$(AWS_NWK_INCLUDE_PATH)/
CFLAGS+= -I$(PATH_CONFIG)/
CFLAGS+=  -I ../../../../../rom
CFLAGS+=  -I ../../../../../nwk/applications/mqtt_client/src/
CFLAGS+=  -I ../../../../../nwk/applications/mqtt_client/src/MQTTPacket/src
CFLAGS+=  -I ../../../../../nwk/applications/
CFLAGS+= -D RSI_SAMPLE_HAL
CFLAGS+= -D ASYNC_MQTT
CFLAGS+= -D LINUX_PLATFORM 

MQTT_CLIENT_APP =rsi_subscribe_publish_sample.o 

all: $(MQTT_CLIENT_APP) 

	make aws_iot CC=$(CC) -Wall -C ../../../../../build/ path_config=$(PATH_CONFIG) 
	$(CC) $(CFLAGS) *.o  $(LINKS) -o $(PROGNAME) 
	rm -f  *.o  


clean:
	$(rm) $(MQTT_CLIENT_APP) core *.o *~ rsi_wc_app
