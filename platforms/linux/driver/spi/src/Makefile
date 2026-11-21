#*HEADER*******************************************************************
#Copyright (c)
#All rights reserved
#This software embodies materials and concepts that are confidential
#to Silicon Labs and is made available solely pursuant to the terms
#of a written license agreement with Silicon Labs
#
#Project name : WiSeConnect
#Module name  : Linux SPI Driver
#File Name    : Makefile
#
#File Description:
#    This is the Makefile For RSI Linux SPI Driver
#        
#
#Author :
#
#Rev History:
#Ver        By             date       Description
#---------------------------------------------------------
#1.1                            Initial version
#---------------------------------------------------------
#*END*********************************************************************

DEF_KERNEL_DIR := /lib/modules/$(shell uname -r)/build 

ifeq ($(KERNELDIR),)
	KERNELDIR := $(DEF_KERNEL_DIR)
endif


ifeq ($(DRIVER_DIR),)
DRIVER_DIR=$(PWD)
endif


SPI_INCLUDE=$(DRIVER_DIR)/../include/
COMMON_INCLUDE=$(DRIVER_DIR)/../../common/include/
COMMON_SRC=../../common/src
EXTRA_CFLAGS += -DLINUX 
EXTRA_CFLAGS += -DRSI_SPI_INTERFACE
EXTRA_CFLAGS += -D m32
EXTRA_CFLAGS += -DENABLE_WMM_FEATURE

EXTRA_CFLAGS += -I$(SPI_INCLUDE)
EXTRA_CFLAGS += -I$(COMMON_INCLUDE)
EXTRA_CFLAGS += -I$(KERNELDIR)
EXTRA_CFLAGS += -Wimplicit
EXTRA_CFLAGS += -w
EXTRA_CFLAGS += -DRSI_AUTOMATION_ENABLE
EXTRA_CFLAGS += -Wstrict-prototypes
EXTRA_CFLAGS += -DRS9116
EXTRA_CFLAGS += -g3

SRC_FILES :=  $(COMMON_SRC)/rsi_linux_netlink.o \
              $(COMMON_SRC)/rsi_net_device.o \
	      $(COMMON_SRC)/rsi_linux_specific.o \
	      $(COMMON_SRC)/rsi_linux_data.o \
	      $(COMMON_SRC)/rsi_core_wmm.o \
	      $(COMMON_SRC)/rsi_send_boot_insn.o \
	      $(COMMON_SRC)/rsi_lib_util.o \
	      rsi_91x_spi.o \
	      rsi_91x_spi_ops.o \

obj-m := $(INSTALLDIR)rpsspi.o
$(INSTALLDIR)rpsspi-objs :=  ${SRC_FILES}

all:
	@echo "Compiling driver code"
	make -C  $(KERNELDIR) M=$(PWD) modules

clean:
	@echo "Cleaning driver binary files"
	rm -rf $(INSTALLDIR)rpsspi.* 
	rm -rf $(INSTALLDIR)*.rpsspi.*.*
	rm -rf *.o *.mod.* *.symvers
	rm -rf .rps*
	rm -rf .rsi*
	rm -rf .tmp_versions
	rm -rf modules.order
	rm -rf $(COMMON_SRC)/.o*
	rm -rf $(COMMON_SRC)/*.cmd 
	rm -rf $(COMMON_SRC)/.*.cmd 
