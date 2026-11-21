/*******************************************************************************
* @file  rsi_app.h
* @brief 
*******************************************************************************
* # License
* <b>Copyright 2020 Silicon Laboratories Inc. www.silabs.com</b>
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
/**
 * @file     rsi_app.h
 * @version      1.0
 * @date         2014-Jan-31
 *
 *
 *
 * @brief HEADER, APP, APPLICATION Header file which contains application specific structures 
 *
 * @section Description
 * This file contains the Application related information.
 *
 *
 */
#include "rsi_config.h"
#include "sensor_data.h"
#ifndef _RSI_APP_CB_H
#define _RSI_APP_CB_H
#define RSI_MAXSOCKETS                       10     //@ Maximum number of open sockets

/* Application control block */


/*===================================================*/
/**
 * Sockets Structure
 * Structure linking socket number to protocol
 */
typedef struct {
  uint8    ip_version[2];                   //@ ip version
  uint8    socketDescriptor[2];             //@ socket descriptor
  uint8    protocol;                        //@ PROTOCOL_UDP, PROTOCOL_TCP or PROTOCOL_UNDEFINED
  uint8    src_port[2];                     //@ socket local port number
  union{
    uint8     ipv4_address[4];              //@ Destination ipv4 address
    uint8     ipv6_address[16];              //@ Destination ipv6 address
  }dest_ip;
  uint8    dest_port[2];                    //@ Destination port number  
} rsi_socketsStr;

/*===================================================*/
/**
 * Sockets Structure Array
 * Array of Structures linking socket number to protocol
 */
typedef struct {
    rsi_socketsStr      socketsArray[RSI_MAXSOCKETS+1];        
    //@ Socket numbers are from 1 to 10
} rsi_sockets;


//! Host MIB structure and Object OID list  
typedef struct MIB_ENTRY_STRUCT
{

  char       *obj_id;
} MIB_ENTRY;


typedef struct 
{
  /* Error code */
  uint16        error_code;
  /* Buffer to receive to response from Wi-Fi module */
  rsi_uCmdRsp   *uCmdRspFrame;

  /* For Certificate */
  struct        SET_CHUNK_S set_chunks;
  /* received paket count */
  uint32        rcvd_data_pkt_count;
  /* Mac address */
  uint8         mac_addr[6];
#if RSI_CONCURRENT_MODE
  /* Mac address */
  uint8         ap_mac_addr[6];
#endif
  /* packet pending flag */
  volatile      uint32 pkt_pending;

#if (RSI_POWER_MODE == RSI_POWER_MODE_3)
  //! backup of frame type 
  uint8         ps_descparam[16];   
  //! Paket pending for power save
  void *        ps_pkt_pending;   
  //! size of currently held packet
  uint16        ps_size_param;
  //! devide sleep indication
  uint16        ps_ok_to_send;
#endif
#if (RSI_POWER_MODE)
  uint16        power_save_enable;
#endif
#if (defined(RSI_UART_INTERFACE) && !defined(TCP_IP_BYPASS))
  volatile int  ack_flag;
#endif
  /* PER Continous wave mode state*/
  int8          per_cont_mode_state;

  rsi_uConnected_station_t stations_connected[RSI_NO_OF_CLIENTS_SUPPORTED];

  /* Buffer to hold the received packet */
  uint8         read_packet_buffer[RSI_MAX_PAYLOAD_SIZE];

  /* For Webpage write */
  uint8         webpage_morechunks;

  /* flag to enable send data*/
  uint8         glbl_send_data;

  /*structure to store socket information */
  volatile      rsi_sockets   socketsStrArray;  

  /* Send buffer data */
  uint8         send_buffer[RSI_MAX_PAYLOAD_SIZE];
 
  uint8         write_packet_buffer[RSI_MAX_PAYLOAD_SIZE];

#if JSON_LOAD
  /* Json buffer */
  uint8         json_buffer[JSON_BUFFER_SIZE];
  uint8         json_load_done;
  /* User Data Structures */
  app_data_t    sensor_data;
#endif
  #if WEB_PAGE_LOAD 
  uint8         webpage_load_done;
#endif
  //! frame sent for the send command,  includes data
  rsi_uSend     uSendFrame;   
  uint8         abort_call;
}rsi_app_cb_t;

#define RSI_FILL_PARAMETERS(x,y) rsi_fill_parameters(x,y)

extern rsi_app_cb_t rsi_app_cb;

#endif
