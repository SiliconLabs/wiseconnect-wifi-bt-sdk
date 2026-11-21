#*HEADER*******************************************************************
#Copyright (c)
#All rights reserved
#This software embodies materials and concepts that are confidential
#to Silicon Labs and is made available solely pursuant to the terms
#of a written license agreement with Silicon Labs
#
#Project name : WiSeConnect
#Module name  : Linux USB Driver
#File Name    : Makefile
#
#File Description:
#    This is the Makefile For RSI Linux USB Driver
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

PWD := $(shell pwd)

ifeq ($(DRIVER_DIR),)
	DRIVER_DIR := $(PWD)
endif

USB_INCLUDE=$(DRIVER_DIR)/../include
COMMON_INCLUDE=$(DRIVER_DIR)/../../common/include
COMMON_SRC=../../common/src
EXTRA_CFLAGS += -DLINUX 
EXTRA_CFLAGS += -DRSI_USB_INTERFACE
#EXTRA_CFLAGS += -DRSI_DUAL_MODE
EXTRA_CFLAGS += -DENABLE_WMM_FEATURE
EXTRA_CFLAGS += -D m32

EXTRA_CFLAGS += -I$(USB_INCLUDE)
EXTRA_CFLAGS += -I$(COMMON_INCLUDE)
EXTRA_CFLAGS += -I$(KERNELDIR)/include
EXTRA_CFLAGS += -I$(KERNELDIR)
EXTRA_CFLAGS += -Wimplicit
EXTRA_CFLAGS += -w
EXTRA_CFLAGS += -Wall
EXTRA_CFLAGS += -DRS9116
EXTRA_CFLAGS += -Wstrict-prototypes
EXTRA_CFLAGS += -DRSI_AUTOMATION_ENABLE

SRC_FILES := $(COMMON_SRC)/rsi_linux_netlink.o \
	$(COMMON_SRC)/rsi_net_device.o \
	$(COMMON_SRC)/rsi_linux_specific.o \
	$(COMMON_SRC)/rsi_linux_data.o \
	$(COMMON_SRC)/rsi_core_wmm.o \
	$(COMMON_SRC)/rsi_send_boot_insn.o \
	$(COMMON_SRC)/rsi_lib_util.o \
	rsi_usb_drv.o

obj-m := $(INSTALLDIR)rpsusb.o
$(INSTALLDIR)rpsusb-objs :=  ${SRC_FILES}

all:
	@echo "Compiling driver code"
	make -C $(KERNELDIR) M=$(PWD) EXTRA_CFLAGS="$(EXTRA_CFLAGS)" modules

clean:
	@echo "Cleaning driver binary files"
	rm -rf $(INSTALLDIR)rpsusb.* 
	rm -rf $(INSTALLDIR)*.rpsusb.*.*
	rm -rf *.o *.mod.* *.symvers
	rm -rf .rps*
	rm -rf .rsi*
	rm -rf .tmp_versions
	rm -rf modules.order
	rm -rf $(COMMON_SRC)/*.o  
	rm -rf $(COMMON_SRC)/*.cmd 
	rm -rf $(COMMON_SRC)/.*.cmd 
