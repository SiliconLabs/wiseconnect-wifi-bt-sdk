#  Makefile
ifndef PROGNAME
$(error Must define PROGNAME)
endif

ifndef RSI_SDK_PATH
$(error Must define RSI_SDK_PATH that is location of SDK)
endif

ifneq ($(words $(MAKECMDGOALS)),1)
$(info Available platforms:)
$(foreach d,$(notdir $(wildcard $(RSI_SDK_PATH)/platforms/*)),$(info - $d))
$(error Must provide name of platform as an argument. See above for options)
endif

# Try identify the platform from the MAKECMDGOALS
ifeq ($(wildcard $(RSI_SDK_PATH)/platforms/$(MAKECMDGOALS)),)
$(error Cannot find platform for "$(MAKECMDGOALS)")
else
PLATFORM :=$(MAKECMDGOALS)
endif

include $(RSI_SDK_PATH)/platforms/$(PLATFORM)/$(PLATFORM).mk
 
 
# Try identify the bus
ifeq ($(words $(VALID_BUSES)),1)
BUS=$(VALID_BUSES)
else
BUS:=$(filter $(MAKECMDGOALS),$(VALID_BUSES))
ifeq ($(words $(BUS)),0)
$(error No valid bus found. Please select from the following: $(VALID_BUSES))
endif
ifneq ($(words $(BUS)),1)
$(error Too many buses. Please select from the following: $(BUS))
endif
endif
 
.PHONY: $(BUS)
 
INCLUDES += -I . \
            -I $(RSI_SDK_PATH)/sapi/include  \
            -I $(RSI_SDK_PATH)/sapi/network/protocols

CFLAGS += -g -Wall $(INCLUDES) -Os

# Interface options
uart_BUS_CFLAGS += -D RSI_UART_INTERFACE
spi_BUS_CFLAGS  += -D RSI_SPI_INTERFACE
sdio_BUS_CFLAGS += -D RSI_SDIO_INTERFACE
 
# Enable debug prints by default
CFLAGS+= -D RSI_ENABLE_DEBUG_PRINT -D RSI_DEBUG_PRINTS

CFLAGS+= -D RSI_SAMPLE_HAL
CFLAGS+= -D RSI_IPV6_ENABLE 
CFLAGS+= -D APIS_ROM
CFLAGS+= -D APIS_NON_ROM

wlan_CFLAGS       += -D RSI_WLAN_ENABLE
mqtt_CFLAGS       += -D ASYNC_MQTT \
                     -I $(RSI_SDK_PATH)/third_party/mqtt_client/src \
                     -I $(RSI_SDK_PATH)/third_party/mqtt_client/src/MQTTPacket/src
ble_CFLAGS        += -D RSI_BLE_ENABLE
bt_classic_CFLAGS += -D RSI_BT_ENABLE
prop_protocol_CFLAGS        += -D RSI_PROP_PROTOCOL_ENABLE
zigbee_CFLAGS     += -D RSI_ZB_ENABLE -D ZB_DEBUG
zigbee_mac_CFLAGS += -D ZB_MAC_API
homekit_CFLAGS    += -D RSI_HOMEKIT_APP
puf_CFLAGS        += -D RSI_PUF_ENABLE
crypto_CFLAGS     += -D RSI_CRYPTO_ENABLE
aws_CFLAGS        += -I $(RSI_SDK_PATH)/third_party/aws_sdk/include \
                     -I $(RSI_SDK_PATH)/third_party/aws_sdk/platform/silabs/inc
certificates_CFLAGS += -I $(RSI_SDK_PATH)/resources/certificates
firmware_udpate_CFLAGS += -I $(RSI_SDK_PATH)/sapi/network/protocols
freertos_CFLAGS += -I $(RSI_SDK_PATH)/third_party/freertos/include \
                   -I $(RSI_SDK_PATH)/third_party/freertos/portable/GCC/ARM_CM4F
firmware_update_CFLAGS += -I $(RSI_SDK_PATH)/sapi/network/protocols
mqtt_client_CFLAGS += -I $(RSI_SDK_PATH)/sapi/network/protocols \
                      -I $(RSI_SDK_PATH)/third_party/mqtt_client/src/MQTTPacket/src \
                      -I $(RSI_SDK_PATH)/third_party/mqtt_client/src

wlan_SOURCES = $(RSI_SDK_PATH)/sapi/driver/rsi_setregion_countryinfo.c \
               $(RSI_SDK_PATH)/sapi/driver/rsi_wlan.c \
               $(RSI_SDK_PATH)/sapi/driver/rsi_nwk.c \
               $(RSI_SDK_PATH)/sapi/driver/rsi_scheduler_rom.c \
               $(RSI_SDK_PATH)/sapi/driver/rsi_pkt_mgmt_rom.c \
               $(RSI_SDK_PATH)/sapi/driver/rsi_utils_rom.c \
               $(RSI_SDK_PATH)/sapi/driver/rsi_queue_rom.c \
               $(RSI_SDK_PATH)/sapi/driver/rsi_events_rom.c \
               $(RSI_SDK_PATH)/sapi/network/socket/rsi_socket.c \
               $(RSI_SDK_PATH)/sapi/network/protocols/rsi_dns.c \
               $(RSI_SDK_PATH)/sapi/network/protocols/rsi_ftp.c \
               $(RSI_SDK_PATH)/sapi/network/protocols/rsi_raw_data.c \
               $(RSI_SDK_PATH)/sapi/network/protocols/rsi_http_client.c \
               $(RSI_SDK_PATH)/sapi/network/protocols/rsi_web_socket.c \
               $(RSI_SDK_PATH)/sapi/network/protocols/rsi_smtp_client.c \
               $(RSI_SDK_PATH)/sapi/network/protocols/rsi_multicast.c \
               $(RSI_SDK_PATH)/sapi/network/protocols/rsi_mdnsd.c \
               $(RSI_SDK_PATH)/sapi/network/protocols/rsi_firmware_upgradation.c \
               $(RSI_SDK_PATH)/sapi/network/protocols/http_server/rsi_http_server.c \
               $(RSI_SDK_PATH)/sapi/network/protocols/http_server/rsi_json_handlers.c \
               $(RSI_SDK_PATH)/sapi/network/protocols/rsi_pop3_client.c \
               $(RSI_SDK_PATH)/sapi/network/protocols/rsi_sntp_client.c \
               $(RSI_SDK_PATH)/sapi/network/protocols/rsi_ota_fw_up.c \
               $(RSI_SDK_PATH)/sapi/network/protocols/rsi_http_ota_fw_up.c \
               $(RSI_SDK_PATH)/sapi/network/protocols/rsi_dhcp_user_class.c \
               $(RSI_SDK_PATH)/sapi/network/protocols/rsi_emb_mqtt_client.c \
               $(RSI_SDK_PATH)/sapi/network/socket/rsi_socket.c \
               $(RSI_SDK_PATH)/sapi/network/socket/rsi_socket_rom.c \
               $(RSI_SDK_PATH)/sapi/wlan/rsi_wlan_apis.c

mqtt_client_SOURCES = $(RSI_SDK_PATH)/sapi/network/protocols/rsi_mqtt_client.c \
               $(RSI_SDK_PATH)/third_party/mqtt_client/src/MQTTClient.c \
               $(RSI_SDK_PATH)/third_party/mqtt_client/src/MQTT_sapi_wrappers.c \
               $(mqtt_packet_SOURCES)
               
mqtt_packet_SOURCES = $(RSI_SDK_PATH)/third_party/mqtt_client/src/MQTTPacket/src/MQTTConnectClient.c \
                      $(RSI_SDK_PATH)/third_party/mqtt_client/src/MQTTPacket/src/MQTTDeserializePublish.c \
                      $(RSI_SDK_PATH)/third_party/mqtt_client/src/MQTTPacket/src/MQTTFormat.c \
                      $(RSI_SDK_PATH)/third_party/mqtt_client/src/MQTTPacket/src/MQTTPacket.c \
                      $(RSI_SDK_PATH)/third_party/mqtt_client/src/MQTTPacket/src/MQTTSerializePublish.c \
                      $(RSI_SDK_PATH)/third_party/mqtt_client/src/MQTTPacket/src/MQTTSubscribeClient.c \
                      $(RSI_SDK_PATH)/third_party/mqtt_client/src/MQTTPacket/src/MQTTUnsubscribeClient.c 

aws_SOURCES = $(RSI_SDK_PATH)/third_party/aws_sdk/src/aws_iot_mqtt_client.c \
              $(RSI_SDK_PATH)/third_party/aws_sdk/src/aws_iot_mqtt_client_connect.c \
              $(RSI_SDK_PATH)/third_party/aws_sdk/src/aws_iot_mqtt_client_common_internal.c \
              $(RSI_SDK_PATH)/third_party/aws_sdk/src/aws_iot_mqtt_client_subscribe.c \
              $(RSI_SDK_PATH)/third_party/aws_sdk/src/aws_iot_mqtt_client_publish.c \
              $(RSI_SDK_PATH)/third_party/aws_sdk/src/aws_iot_shadow_records.c \
              $(RSI_SDK_PATH)/third_party/aws_sdk/src/aws_iot_shadow_json.c \
              $(RSI_SDK_PATH)/third_party/aws_sdk/src/jsmn.c \
              $(RSI_SDK_PATH)/third_party/aws_sdk/src/aws_iot_json_utils.c \
              $(RSI_SDK_PATH)/third_party/aws_sdk/src/aws_iot_shadow_actions.c \
              $(RSI_SDK_PATH)/third_party/aws_sdk/src/aws_iot_mqtt_client_yield.c \
              $(RSI_SDK_PATH)/third_party/aws_sdk/src/aws_iot_mqtt_client_unsubscribe.c \
              $(RSI_SDK_PATH)/third_party/aws_sdk/src/aws_iot_shadow.c \
              $(RSI_SDK_PATH)/third_party/aws_sdk/platform/silabs/src/network_sapi_wrapper.c \
              $(RSI_SDK_PATH)/third_party/aws_sdk/platform/silabs/src/timer.c \
              $(mqtt_packet_SOURCES)

# Only include FreeRTOS for embedded platforms
ifeq ($(filter $(PLATFORM),linux),)
freertos_SOURCES = $(RSI_SDK_PATH)/sapi/rtos/freertos_wrapper/rsi_os_wrapper.c \
                   $(RSI_SDK_PATH)/third_party/freertos/croutine.c \
                   $(RSI_SDK_PATH)/third_party/freertos/event_groups.c \
                   $(RSI_SDK_PATH)/third_party/freertos/list.c \
                   $(RSI_SDK_PATH)/third_party/freertos/queue.c \
                   $(RSI_SDK_PATH)/third_party/freertos/stream_buffer.c \
                   $(RSI_SDK_PATH)/third_party/freertos/tasks.c \
                   $(RSI_SDK_PATH)/third_party/freertos/timers.c \
                   $(RSI_SDK_PATH)/third_party/freertos/portable/GCC/ARM_CM4F/port.c \
                   $(RSI_SDK_PATH)/third_party/freertos/portable/MemMang/heap_4.c
endif

COMMON_SOURCES = $(RSI_SDK_PATH)/sapi/common/rsi_common_apis.c \
                 $(RSI_SDK_PATH)/sapi/common/rsi_device_init_apis.c \
                 $(RSI_SDK_PATH)/sapi/common/rsi_apis_rom.c \
                 $(RSI_SDK_PATH)/sapi/common/rsi_apis_non_rom.c \
                 $(RSI_SDK_PATH)/sapi/driver/rsi_driver.c \
                 $(RSI_SDK_PATH)/sapi/driver/rsi_driver_event_handlers.c \
                 $(RSI_SDK_PATH)/sapi/driver/rsi_events.c \
                 $(RSI_SDK_PATH)/sapi/driver/rsi_os_none.c \
                 $(RSI_SDK_PATH)/sapi/driver/rsi_pkt_mgmt.c \
                 $(RSI_SDK_PATH)/sapi/driver/rsi_queue.c \
                 $(RSI_SDK_PATH)/sapi/driver/rsi_device_init.c \
                 $(RSI_SDK_PATH)/sapi/driver/rsi_scheduler.c \
                 $(RSI_SDK_PATH)/sapi/driver/rsi_utils.c \
                 $(RSI_SDK_PATH)/sapi/driver/rsi_timer.c \
                 $(RSI_SDK_PATH)/sapi/driver/device_interface/spi/rsi_spi_frame_rd_wr.c \
                 $(RSI_SDK_PATH)/sapi/driver/device_interface/spi/rsi_spi_functs.c \
                 $(RSI_SDK_PATH)/sapi/driver/device_interface/spi/rsi_spi_iface_init.c \
                 $(RSI_SDK_PATH)/sapi/driver/device_interface/spi/rsi_spi_mem_rd_wr.c \
                 $(RSI_SDK_PATH)/sapi/driver/device_interface/spi/rsi_spi_reg_rd_wr.c \
                 $(RSI_SDK_PATH)/sapi/driver/rsi_iap.c  \
                 $(RSI_SDK_PATH)/sapi/driver/rsi_common.c  \

zigbee_SOURCES = $(ZB_CORE_SRC)/rsi_zb.c \
             $(ZB_CORE_SRC)/rsi_zb_utility.c \
             $(RSI_SDK_PATH)sapi/hal/rsi_timer.c \
             $(ZB_REF_APPS_SRC)/rsi_zb_apis.c

zigbee_mac_SOURCES = $(ZB_MAC_SRC)/rsi_zigb_mac_apis.c 

BT_COMMON_SOURCES = $(RSI_SDK_PATH)/sapi/driver/rsi_bt_ble.c \
                    $(RSI_SDK_PATH)/sapi/bluetooth/rsi_bt_common_apis.c 

ble_SOURCES = $(RSI_SDK_PATH)/sapi/bluetooth/rsi_ble_gap_apis.c \
              $(RSI_SDK_PATH)/sapi/bluetooth/rsi_ble_gatt_apis.c \
              $(BT_COMMON_SOURCES)

ifeq ($(prop_protocol),1)
PROP_PROTOCOL_SOURCES = $(RSI_SDK_PATH)/sapi/prop_protocol/rsi_prop_protocol_apis.c
endif

bt_classic_SOURCES = $(RSI_SDK_PATH)/sapi/bluetooth/rsi_bt_gap_apis.c \
                     $(RSI_SDK_PATH)/sapi/bluetooth/rsi_bt_spp_apis.c   \
                     $(RSI_SDK_PATH)/sapi/bluetooth/rsi_bt_a2dp_apis.c  \
                     $(RSI_SDK_PATH)/sapi/bluetooth/rsi_bt_avrcp_apis.c \
                     $(BT_COMMON_SOURCES)

crypto_SOURCES = $(RSI_SDK_PATH)/sapi/crypto/rsi_crypto.c

# $1 is the list of SDK source files
# $2 is file extension to append
define MAP_SDK_SOURCE
$(addsuffix .$2,$(addprefix output/,$(subst $(RSI_SDK_PATH)/,sdk/,$(sort $(1)))))
endef

# $1 is the list of application source files
# $2 is file extension to append
define MAP_APPLICATION_SOURCE
$(addsuffix .$2,$(addprefix output/app/,$(1)))
endef
 
# Include all existing .d dependency files
$(eval -include $(call MAP_APPLICATION_SOURCE, $(APPLICATION_SOURCES),d))
$(eval -include $(call MAP_SDK_SOURCE, $(COMMON_SOURCES) $($(BUS)_BUS_SOURCES) $(foreach f,$(SDK_FEATURES),$($(f)_SOURCES)),d))

# Recipe to compile SDK source files
output/sdk/%.o: $(RSI_SDK_PATH)/% | output/sdk/%.directory
	@echo $< : $@
	$(CC) $(CFLAGS) $(foreach f,$(SDK_FEATURES),$($(f)_CFLAGS)) $(LINKER_FLAGS) -o $@ -c $<
	$(CC) $(CFLAGS) $(foreach f,$(SDK_FEATURES),$($(f)_CFLAGS)) $(LINKER_FLAGS) -MD -o $@ -c $<
 
# Recipe to compile application source files
output/app/%.o: % | output/app/%.directory
	@echo $< : $@
	$(CC) $(CFLAGS) $(foreach f,$(SDK_FEATURES),$($(f)_CFLAGS)) $(LINKER_FLAGS) -o $@ -c $<
	$(CC) $(CFLAGS) $(foreach f,$(SDK_FEATURES),$($(f)_CFLAGS)) $(LINKER_FLAGS) -MD -o $@ -c $<
 
 
# Recipe to create directories
%.directory:
	@mkdir -p $(dir $@)

all: $(PROGNAME)$(PROG_EXTENSION)
	@echo Building $(PROGNAME)$(PROG_EXTENSION)
 
$(PROGNAME)$(PROG_EXTENSION): $(call APPLICATION_SOURCE_TO_OBJECTS, $(APPLICATION_SOURCES)) $(call SDK_SOURCE_TO_OBJECTS, $(COMMON_SOURCES) $($(BUS)_BUS_SOURCES) $(foreach f,$(SDK_FEATURES),$($(f)_SOURCES)))
$(PROGNAME)$(PROG_EXTENSION): $(call MAP_APPLICATION_SOURCE,$(APPLICATION_SOURCES),o) $(call MAP_SDK_SOURCE,$(COMMON_SOURCES) $($(BUS)_BUS_SOURCES) $(foreach f,$(SDK_FEATURES),$($(f)_SOURCES)),o)
	@echo Linking $@
	$(CC) $(CFLAGS) $($(BUS)_BUS_CFLAGS) $(foreach f,$(SDK_FEATURES),$($(f)_CFLAGS)) $(LINKER_FLAGS) -o $@ $^ $(LIBS)
 
