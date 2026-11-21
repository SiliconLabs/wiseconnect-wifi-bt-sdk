rm=/bin/rm -f
CC=cc
AR=ar

SDK_FEATURES += linux

#VALID_BUSES := uart usb sdio spi

VALID_BUSES := usb
#VALID_BUSES := uart
#VALID_BUSES := sdio
#VALID_BUSES := spi

LINKER_FLAGS= -m32 -pthread -lrt

CFLAGS+= -m32 -D LINUX_PLATFORM -D RSI_UART_INTERFACE

INCLUDES += -I $(RSI_SDK_PATH)/platforms/linux \
	    -I $(RSI_SDK_PATH)/platforms/linux/core/include \
	    -I $(RSI_SDK_PATH)/platforms/linux/hal/include

linux_SOURCES =  $(RSI_SDK_PATH)/platforms/linux/hal/uart/rsi_uart.c \
                 $(RSI_SDK_PATH)/platforms/linux/core/src/rsi_nl_app.c \
                 $(RSI_SDK_PATH)/platforms/linux/core/src/rsi_linux_apis.c \
                 $(RSI_SDK_PATH)/platforms/linux/core/src/rsi_bootup_config.c \
                 $(RSI_SDK_PATH)/platforms/linux/core/src/rsi_os_wrapper.c \
                 $(RSI_SDK_PATH)/platforms/linux/hal/src/rsi_hal_platform_init.c \
                 $(RSI_SDK_PATH)/platforms/linux/hal/src/rsi_hal_mcu_interrupt.c \
                 $(RSI_SDK_PATH)/platforms/linux/hal/src/rsi_hal_mcu_ioports.c \
                 $(RSI_SDK_PATH)/platforms/linux/hal/src/rsi_hal_mcu_timer.c \
                 $(RSI_SDK_PATH)/platforms/linux/hal/src/rsi_hal_mcu_platform_init.c
                 
linux: all
