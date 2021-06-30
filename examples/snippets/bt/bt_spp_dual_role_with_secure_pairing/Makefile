#Makefile

rm=/bin/rm -f
CC= cc
DEFS=  
PROGNAME= rsi_wc_app
LINKS= -pthread -L../../../sapi/build/ -lrps
PATH_CONFIG=$(shell pwd)

RSI_DRIVER_PATH = ../../../sapi/include
RSI_ROM_PATH = ../../../sapi/rom
CFLAGS+=  -I$(RSI_DRIVER_PATH)/
CFLAGS+=  -I$(RSI_ROM_PATH)/
CFLAGS+=  -I$(PATH_CONFIG)/
CFLAGS+= -D RSI_SAMPLE_HAL
CFLAGS+= -D RSI_BT_ENABLE

BT_SPP_MASTER_SLAVE_APP = rsi_spp_master_slave.o 

all: clean $(BT_SPP_MASTER_SLAVE_APP) 

	make bt_classic CC=$(CC) -C ../../../sapi/build/ path_config=$(PATH_CONFIG) 
	$(CC) $(CFLAGS) *.o $(LINKS) -o $(PROGNAME) 
	rm -f  *.o  


clean:
	$(rm) $(BT_SPP_MASTER_SLAVE_APP) core *.o *~
