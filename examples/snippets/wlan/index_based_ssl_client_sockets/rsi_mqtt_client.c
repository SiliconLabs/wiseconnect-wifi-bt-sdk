/*******************************************************************************
* @file  rsi_mqtt_client.c
* @brief
*******************************************************************************
* # License
* <b>Copyright 2021 Silicon Laboratories Inc. www.silabs.com</b>
*******************************************************************************
*
* The licensor of this software is Silicon Laboratories Inc. Your use of this
* software is governed by the terms of Silicon Labs Master Software License
* Agreement (MSLA) available at
* www.silabs.com/about-us/legal/master-software-license-agreement. This
* software is distributed to you in Source Code format and is governed by the
* sections of the MSLA applicable to Source Code.
*
******************************************************************************/
#include "rsi_index_based_ssl_client_sockets.h"

//! Global variables
//! Message to publish
uint8_t publish_message[] = "THIS IS MQTT CLIENT DEMO FROM APPLICATION";

//! MQTT CLient ID
int8_t clientID[] = "MQTTCLIENT";

//! user name for login credentials
int8_t mqtt_username[] = "admin";

//! password for login credentials
int8_t mqtt_password[] = "admin";

volatile int halt = 0;

int8_t mqtt_client_buffer[MQTT_CLIENT_INIT_BUFF_LEN];

#ifdef ASYNC_MQTT
void handleMQTT(uint32_t sock_no, uint8_t *buffer, uint32_t length)
{
  char txt[2048];
  int len = 0;
  int i   = 0;

  /*Check for CONNACK packet*/
  if (buffer[0] == MQTT_CONNACK) {
    if (buffer[3] == 0) {
      rsi_mqtt_client->mqtt_client.isconnected = 1;
      connack                                  = 1;
    } else {
      return;
    }
  }

  /*Check for SUBACK packet*/
  else if (buffer[0] == MQTT_SUBACK) {
    ++suback;
  }

  /*Check for MQTT data packet*/
  else {
    memset(rcvbuf, '\0', sizeof(rcvbuf));
    memset(txt, '\0', sizeof(txt));

    memcpy(txt, buffer, length);

    /*Enable the command flag*/
    data_received = 1;
  }
}
#else
//! Call back when a message is received on the published topic
void rsi_message_received(MessageData *md)
{
  UNUSED_PARAMETER(md); //This statement is added only to resolve compilation warning, value is unchanged
  //! process the received data
  halt = 1;
  return;
}
#endif

//! Remote socket terminate call back handler
void rsi_remote_socket_terminate_handler(uint16_t status, uint8_t *buffer, const uint32_t length)
{
  UNUSED_PARAMETER(status);       //This statement is added only to resolve compilation warning, value is unchanged
  UNUSED_CONST_PARAMETER(buffer); //This statement is added only to resolve compilation warning, value is unchanged
  UNUSED_CONST_PARAMETER(length); //This statement is added only to resolve compilation warning, value is unchanged
  //! Remote socket has been terminated
}

int32_t rsi_mqtt_client_task()
{
  int32_t status = RSI_SUCCESS;
  uint8_t flags  = MQTT_INDEX1_FLAGS;

  uint32_t server_address = ip_to_reverse_hex((char *)SERVER_IP_ADDRESS);

#ifndef ASYNC_MQTT
  rsi_mqtt_client_info_t *rsi_mqtt_client = NULL;
#endif

  MQTTMessage publish_msg;

  //! Initialze remote terminate call back
  rsi_wlan_register_callbacks(RSI_REMOTE_SOCKET_TERMINATE_CB, rsi_remote_socket_terminate_handler);

#ifdef ASYNC_MQTT
  while (1) {
start:
    //! MQTT client initialisation
    rsi_mqtt_client = rsi_mqtt_client_init((uint8_t *)&mqqt_client_buffer,
                                           MQTT_CLIENT_INIT_BUFF_LEN,
                                           (int8_t *)&server_address,
                                           SERVER_PORT,
                                           CLIENT_PORT,
                                           0,
                                           RSI_KEEP_ALIVE_PERIOD);
    if (mqqt_client_buffer == NULL) {
      goto start;
    }
    LOG_PRINT("\r\nMQTT initialization Success\r\n");
    //! Connect to the MQTT broker/server
    status = rsi_mqtt_connect(rsi_mqtt_client, 0, (uint8_t *)&clientID, NULL, NULL, handleMQTT);
    if (status != RSI_SUCCESS) {
      LOG_PRINT("\r\nConnect to the MQTT broker/server Failed, Error Code : 0x%lX\r\n", status);
      rsi_mqtt_disconnect(rsi_mqtt_client);
      continue;
    } else {
      LOG_PRINT("\r\nConnect to the MQTT broker/server Success\r\n");
    }

    while (1) {
#ifndef RSI_WITH_OS
      rsi_wireless_driver_task();
#endif
      if (rsi_mqtt_client->mqtt_client.isconnected == 1) {
        //! Subscribe to the topic given
        status = rsi_mqtt_subscribe(rsi_mqtt_client, QOS, (int8_t *)RSI_MQTT_TOPIC, NULL);
        if (status != RSI_SUCCESS) {
          LOG_PRINT("\r\nSubscription to Topic Failed, Error Code : 0x%lX\r\n", status);
          rsi_mqtt_disconnect(rsi_mqtt_client);
          continue;
        } else {
          LOG_PRINT("\r\nSubscription to Topic Success\r\n");
        }

        while (1) {
#ifndef RSI_WITH_OS
          rsi_wireless_driver_task();
#endif

          if ((suback) && pub == 1) {
            //!The DUP flag MUST be set to 1 by the Client or Server when it attempts to re-deliver a PUBLISH Packet
            //!The DUP flag MUST be set to 0 for all QoS 0 messages
            publish_msg.dup = 0;

            //! This field indicates the level of assurance for delivery of an Application Message. The QoS levels are
            //! 0  - At most once delivery
            //! 1  - At least once delivery
            //! 2  - Exactly once delivery
            publish_msg.qos = QOS;

            //! If the RETAIN flag is set to 1, in a PUBLISH Packet sent by a Client to a Server, the Server MUST store
            //! the Application Message and its QoS, so that it can be delivered to future subscribers whose
            //! subscriptions match its topic name
            publish_msg.retained = 0;

            //! Attach paylaod
            publish_msg.payload = publish_message;

            //! Fill paylaod length
            publish_msg.payloadlen = sizeof(publish_message);

            //! Publish message on the topic
            status = rsi_mqtt_publish(rsi_mqtt_client, (int8_t *)RSI_MQTT_TOPIC, &publish_msg);
            if (status != RSI_SUCCESS) {
              status = rsi_wlan_socket_get_status(rsi_mqtt_client->mqtt_client.ipstack->my_socket);
              LOG_PRINT("\r\nPublish to Topic Failed, Error Code : 0x%lX\r\n", status);
              continue;
            } else {
              LOG_PRINT("\r\nPublish to Topic Success\r\n");
            }
            pub = 0;
          }
        }
      }
    }
  }
#else
  //! MQTT client initialisation
  rsi_mqtt_client = rsi_mqtt_client_init(mqtt_client_buffer,
                                         MQTT_CLIENT_INIT_BUFF_LEN,
                                         (int8_t *)&server_address,
                                         MQTT_SERVER_PORT,
                                         MQTT_CLIENT_PORT,
                                         0,
                                         RSI_KEEP_ALIVE_PERIOD);
  if (mqtt_client_buffer == NULL) {
    return status;
  }
  //! Connect to the MQTT broker/server
  status = rsi_mqtt_connect(rsi_mqtt_client, flags, clientID, mqtt_username, mqtt_password);
  if (status != RSI_SUCCESS) {
    LOG_PRINT("\r\nConnect to the MQTT broker/server Failed, Error Code : 0x%lX\r\n", status);
    //! Shut Down the port
    mqtt_disconnect(rsi_mqtt_client->mqtt_client.ipstack);
    return status;
  } else {
    LOG_PRINT("\r\nConnect to the MQTT broker/server Success\r\n");
  }

  //! Subscribe to the topic given
  status = rsi_mqtt_subscribe(rsi_mqtt_client, QOS, (int8_t *)RSI_MQTT_TOPIC, rsi_message_received);
  if (status != RSI_SUCCESS) {
    if (rsi_wlan_get_status() != RSI_SUCCESS) {
      status = rsi_wlan_socket_get_status(rsi_mqtt_client->mqtt_client.ipstack->my_socket);
    }
    LOG_PRINT("\r\nSubscription to Topic Failed, Error Code : 0x%lX\r\n", status);
    return status;
  } else {
    LOG_PRINT("\r\nSubscription to Topic Success\r\n");
  }

  //!The DUP flag MUST be set to 1 by the Client or Server when it attempts to re-deliver a PUBLISH Packet
  //!The DUP flag MUST be set to 0 for all QoS 0 messages
  publish_msg.dup = 0;

  //! This field indicates the level of assurance for delivery of an Application Message. The QoS levels are
  //! 0  - At most once delivery
  //! 1  - At least once delivery
  //! 2  - Exactly once delivery
  if (QOS == QOS0) {
    publish_msg.qos = QOS0;
  } else if (QOS == QOS1) {
    publish_msg.qos = QOS1;
  } else {
    publish_msg.qos = QOS2;
  }

  //! If the RETAIN flag is set to 1, in a PUBLISH Packet sent by a Client to a Server, the Server MUST store
  //! the Application Message and its QoS, so that it can be delivered to future subscribers whose
  //! subscriptions match its topic name
  publish_msg.retained = 0;

  //! Attach paylaod
  publish_msg.payload = publish_message;

  //! Fill paylaod length
  publish_msg.payloadlen = sizeof(publish_message);

  //! Publish message on the topic
  status = rsi_mqtt_publish(rsi_mqtt_client, (int8_t *)RSI_MQTT_TOPIC, &publish_msg);
  if (status != RSI_SUCCESS) {
    if (rsi_wlan_get_status() != RSI_SUCCESS) {
      status = rsi_wlan_socket_get_status(rsi_mqtt_client->mqtt_client.ipstack->my_socket);
    }
    LOG_PRINT("\r\nPublish to Topic Failed, Error Code : 0x%lX\r\n", status);
    return status;
  } else {
    LOG_PRINT("\r\nPublish to Topic Success\r\n");
  }

  while (!halt) {
    //! Recv data published on the subscribed topic
    status = rsi_mqtt_poll_for_recv_data(rsi_mqtt_client, 30000);
    if (status != RSI_SUCCESS) {
      if (rsi_wlan_get_status() != RSI_SUCCESS) {
        status = rsi_wlan_socket_get_status(rsi_mqtt_client->mqtt_client.ipstack->my_socket);
      }
      //! Error in receiving
      LOG_PRINT("\r\nReceive Data Failed, Error Code : 0x%lX\r\n", status);
      return status;
    } else {
      LOG_PRINT("\r\nReceive Data Success\r\n");
    }
  }
#endif

  //! UnSubscribe to the topic given
  status = rsi_mqtt_unsubscribe(rsi_mqtt_client, (int8_t *)RSI_MQTT_TOPIC);
  if (status != RSI_SUCCESS) {
    if (rsi_wlan_get_status() != RSI_SUCCESS) {
      status = rsi_wlan_socket_get_status(rsi_mqtt_client->mqtt_client.ipstack->my_socket);
    }
    LOG_PRINT("\r\nUnsubscription to Topic Failed, Error Code : 0x%lX\r\n", status);
    return status;
  } else {
    LOG_PRINT("\r\nUnsubscription to Topic Success\r\n");
  }

  //! Disconnect to the MQTT broker
  status = rsi_mqtt_disconnect(rsi_mqtt_client);
  if (status != RSI_SUCCESS) {
    if (rsi_wlan_get_status() != RSI_SUCCESS) {
      status = rsi_wlan_socket_get_status(rsi_mqtt_client->mqtt_client.ipstack->my_socket);
    }
    LOG_PRINT("\r\nDisconnect to the MQTT broker Failed, Error Code : 0x%lX\r\n", status);
    // Shut Down the port
    mqtt_disconnect(rsi_mqtt_client->mqtt_client.ipstack);
    return status;
  } else {
    LOG_PRINT("\r\nDisconnect to the MQTT broker Success\r\n");
  }

  return 0;
}
