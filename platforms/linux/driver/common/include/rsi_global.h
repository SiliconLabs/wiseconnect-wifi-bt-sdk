/*******************************************************************************
* @file  rsi_global.h
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
 * @file     rsi_global.h
 * @version  3.6
 * @date     2013-May-16
 *
 *
 *
 * @brief HEADER, GLOBAL, Global Header file, the things that must be almost everywhere 
 *
 * @section Description
 * This is the top level global.h file for data/functions that need to be in every module
 * This file contains all the structures and defines used in the entire application 
 *
 * @section Improvements
 * New structures are added. 
 * The definition for MACRO "RSI_TXDATA_OFFSET_LUDP" from 28 to 26.
 * LTCP Conn Establishment Rcv structure is changed. 
 * rsi_uSleepTimer is changed
 *
 */


#ifndef _RSIGLOBAL_H_
#define _RSIGLOBAL_H_


/**
 * Global defines
 */
#define RSI_TRUE            1    
#define RSI_FALSE           0
#ifndef NULL
#define NULL                0
#endif

#define DATA_RX             0
#define DATA_TX             1

//#define PACKED            1 

#define WLAN_MGMT_TYPE  0x4
#define WLAN_DATA_TYPE  0x5
#define ZB_MGMT_TYPE    0x1
#define ZB_DATA_TYPE    0x1
#define BT_MGMT_TYPE    0x2
#define BT_DATA_TYPE    0x2

//!Comment This in case the host MCU is of BIG ENDIAN 
#define RSI_LITTLE_ENDIAN                       1

//#define RSI_FEATSEL_ENABLE                    0

//!Uncomment this if host has hardware timers
//#define RSI_HWTIMER

//! Interrupt Mode Selection 
#define RSI_INTERRUPTS

//! Polled Mode selection
//#define RSI_POLLED

#include "rsi_common_types.h"

#define ENABLE                      1
#define DISABLE                     0


#ifndef RSI_HWTIMER 
//! need to define this macro if h/w timer is available and it should increment spiTimer2, spiTimer1 
#define RSI_TICKS_PER_SECOND        50000 
#else
#define RSI_TICKS_PER_SECOND        10
#endif


/*@ firmware upgradation timeout */
#define RSI_FWUPTIMEOUT                100 * RSI_TICKS_PER_SECOND
/*@ wireless firmware upgradation timeout */
#define RSI_WIRELESSFWUPTIMEOUT        500 * RSI_TICKS_PER_SECOND
/*@ bootloading timeout */
#define RSI_BLTIMEOUT                  1 * RSI_TICKS_PER_SECOND
/*@ band timeout */
#define RSI_BANDTIMEOUT                1 * RSI_TICKS_PER_SECOND
/*@ Init timeout */
#define RSI_INITTIMEOUT                1 * RSI_TICKS_PER_SECOND
/*@ Query firmware version timeout */
#define RSI_QFWVTIMEOUT                1 * RSI_TICKS_PER_SECOND
/*@ Set Mac address timeout */
#define RSI_SMATIMEOUT                 1 * RSI_TICKS_PER_SECOND
/*@ Scan timeout */
#define RSI_SCANTIMEOUT                12 * RSI_TICKS_PER_SECOND
/*@ Sleep timer timeout */
#define RSI_SLEEPTIMERTIMEOUT          1 * RSI_TICKS_PER_SECOND
/*@ Wepkeys timeout */
#define RSI_WEPKEYSTIMEOUT             1 * RSI_TICKS_PER_SECOND
/*@ Join timeout */
#define RSI_JOINTIMEOUT                12 * RSI_TICKS_PER_SECOND
/*@ Disconnect timeout */
#define RSI_DISCONTIMEOUT              1 * RSI_TICKS_PER_SECOND
/*@ Query connection status timeout */
#define RSI_QCSTIMEOUT                 3 * RSI_TICKS_PER_SECOND
/*@ Query dhcp params timeout */
#define RSI_QDPTIMEOUT                 3 * RSI_TICKS_PER_SECOND
/*@ Query network params timeout */
#define RSI_QNPTIMEOUT                 3 * RSI_TICKS_PER_SECOND
/*@ Ip configuration timeout */
#define RSI_IPPTIMEOUT                 6 * RSI_TICKS_PER_SECOND
/*@ Query RSSI Value timeout */
#define RSI_RSSITIMEOUT                1 * RSI_TICKS_PER_SECOND
/*@ recv timeout */
#define RSI_RECVTIMEOUT                1 * RSI_TICKS_PER_SECOND
/*@ Socket open timeout */
#define RSI_SOPTIMEOUT                 6 * RSI_TICKS_PER_SECOND
/*@ Regread timeout */
#define RSI_REGREADTIMEOUT             1 * RSI_TICKS_PER_SECOND
/*@ Query DNS timeout */
#define RSI_QDNSTIMEOUT                6 * RSI_TICKS_PER_SECOND
/*@ Start token timeout */
#define RSI_START_TOKEN_TIMEOUT        10 * RSI_TICKS_PER_SECOND
/*@ Set Listen interval timeout */
#define RSI_SLITIMEOUT                 1 * RSI_TICKS_PER_SECOND
/*@ Config Enable timeout */
#define RSI_CETIMEOUT                  1 * RSI_TICKS_PER_SECOND
/*@ Config store timeout */
#define RSI_CSTIMEOUT                  1 * RSI_TICKS_PER_SECOND
/*@ Config get timeout */
#define RSI_CGTIMEOUT                  1 * RSI_TICKS_PER_SECOND
/*@ Query BSSID/NW TYPE timeout */
#define RSI_QBSSIDNWTIMEOUT            6 * RSI_TICKS_PER_SECOND
#define RSI_QMACADDTIMEOUT             6 * RSI_TICKS_PER_SECOND
/*@ Query MAC ADDR timeout */
/*@ Get HTTP timeout */
#define RSI_GETHTTPTIMEOUT             40 * RSI_TICKS_PER_SECOND
/*@ Post HTTP timeout */
#define RSI_POSTHTTPTIMEOUT            6 * RSI_TICKS_PER_SECOND
/*@ Mode select timeout */
#define RSI_MODESEL_TIMEOUT            1 * RSI_TICKS_PER_SECOND
/*@ Feature select timeout */
#define RSI_FEATSEL_TIMEOUT            1 * RSI_TICKS_PER_SECOND
/*@ WPS RESPONSE timeout */
#define RSI_WPSRSPTIMEOUT              120 * RSI_TICKS_PER_SECOND
/*@ Power save timeout*/
#define RSI_PWSAVE_TIMEOUT             5 * RSI_TICKS_PER_SECOND
/*@ Command response timeout */
#define RSI_RESPONSE_TIMEOUT(A)        RSI_RESET_TIMER3;                         \
                                           while (rsi_checkPktIrq() != RSI_TRUE) \
                                            {                                    \
                                              if (RSI_INC_TIMER_3 > A)           \
                                               {                                 \
                                                 retval = -1;                    \
                                                 break;                          \
                                               }                                 \
                                            }     



/*=======================================================================================*/
/**
 * Device Parameters
 */
#define RSI_MAXSOCKETS                       10     //@ Maximum number of open sockets

/**
 * Debugging Parameters
 */
#define RSI_DEBUG_DEVICE                    "UART_1"
#define RSI_MAX_PAYLOAD_SIZE                 1600  //@ Maximum data payload size
#define RSI_WEBSOCKET_MAX_SEND_SIZE          1450
#define RSI_TCP_MAX_SEND_SIZE                1460
#define RSI_UDP_MAX_SEND_SIZE                1472
#define RSI_RAW_DATA_MAX_SEND_SIZE           1514
#define RSI_AP_SCANNED_MAX                   11    //@ Maximum number of scanned acces points
#define RSI_MAX_WFD_DEV_CNT                  10    //@ Maximum wifi direct device count

/**
 * Things that are needed in this .h file
 */
#define RSI_FRAME_DESC_LEN                  16     //@ Length of the frame descriptor, for both read and write
#define RSI_FRAME_CMD_RSP_LEN               56     //@ Length of the command response buffer/frame
#define RSI_TXDATA_OFFSET_TCP               46     //@ required Tx data offset value for TCP, 46
#define RSI_TXDATA_OFFSET_UDP               34     //@ required Tx data offset value for UDP, 34
#define RSI_RXDATA_OFFSET_TCP_V4            26     //@ required Rx data offset value for TCPV4, 26
#define RSI_RXDATA_OFFSET_TCP_V6            46     //@ required Rx data offset value for TCPV6, 46
#define RSI_RXDATA_OFFSET_UDP_V4            14     //@ required Rx data offset value for UDP_V4, 14
#define RSI_RXDATA_OFFSET_UDP_V6            34     //@ required Rx data offset value for UDP_V6, 34
#define RSI_TXDATA_OFFSET_LUDP              16     //@ required Rx data offset value for LUDP, 26
#define RSI_PSK_LEN                         64     //@ maximum length of PSK
#define RSI_SSID_LEN                        34     //@ maximum length of SSID
#define RSI_BSSID_LEN                       6      //@ BSSID length
#define RSI_IP_ADD_LEN                      4 
#define RSI_MAC_ADD_LEN                     6
#define RSI_MGMT_PKT_TYPE                   0x04
#define RSI_DATA_PKT_TYPE                   0x05
#define RSI_FRAME_TYPE                      1
#define RSI_PMK_LEN                         32     //@PMK maximum length                                   

/**
 * Const declaration
 *
 */
#define RSI_BYTES_3                         3
#define RSI_BYTES_2                         2




extern const uint8            rsi_frameCmdDebug[RSI_BYTES_3];            
extern const uint8            rsi_frameCmdBootInsn[RSI_BYTES_3];
extern const uint8            rsi_frameCmdUpdateInfo[RSI_BYTES_3];            
extern const uint8            rsi_frameCmdBand[RSI_BYTES_3];            
extern const uint8            rsi_frameCmdInit[RSI_BYTES_3];            
extern const uint8            rsi_frameCmdScan[RSI_BYTES_3];                
extern const uint8            rsi_frameCmdBGScan[RSI_BYTES_3];                
extern const uint8            rsi_frameCmdJoin[RSI_BYTES_3];                
extern const uint8            rsi_frameCmdPsk[RSI_BYTES_3];                
extern const uint8            rsi_frameCmdPower[RSI_BYTES_3];                
extern const uint8            rsi_frameCmdIpparam[RSI_BYTES_3];                
extern const uint8            rsi_frameCmdSocket[RSI_BYTES_3];                
extern const uint8            rsi_frameCmdSocketClose[RSI_BYTES_3];                
extern const uint8            rsi_frameCmdRssi[RSI_BYTES_3];                
extern const uint8            rsi_frameCmdSend[RSI_BYTES_3];  
extern const uint8            rsi_frameCmdCert[RSI_BYTES_3];    
extern const uint8            rsi_frameCmdRecv[RSI_BYTES_3];  
extern const uint8            rsi_frameCmdConnStatus[RSI_BYTES_3];                
extern const uint8            rsi_frameCmdQryNetParms[RSI_BYTES_3];                
extern const uint8            rsi_frameCmdDisconnect[RSI_BYTES_3];                
extern const uint8            rsi_frameCmdQryFwVer[RSI_BYTES_3];                
extern const uint8            rsi_frameCmdSetMacAddr[RSI_BYTES_3];                
extern const uint8            rsi_frameCmdQryMacAddress[RSI_BYTES_3];                
extern const uint8            rsi_frameCmdOperMode[RSI_BYTES_3];
extern const uint8            rsi_frameCmdP2pConfig[RSI_BYTES_3];
extern const uint8            rsi_frameCmdSetEap[RSI_BYTES_3];
extern const uint8            rsi_frameCmdWebFields[RSI_BYTES_3];
extern const uint8            rsi_frameCmdAntSel[RSI_BYTES_3];
extern const uint8            rsi_frameCmdQryGoParms[RSI_BYTES_3];
extern const uint8            rsi_frameCmdSleepTimer[RSI_BYTES_3];
extern const uint8            rsi_frameCmdDnsQuery[RSI_BYTES_3];
extern       uint8            rsi_frameCmdHttpGetReq[RSI_BYTES_3];
extern const uint8            rsi_frameCmdDnsServer[RSI_BYTES_3];
extern       uint8            rsi_frameCmdHttpPostReq[RSI_BYTES_3];
extern const uint8            rsi_frameCmdReset[RSI_BYTES_3];
extern const uint8            rsi_frameCmdCfgSave[RSI_BYTES_3];
extern const uint8            rsi_frameCmdCfgEnable[RSI_BYTES_3];
extern const uint8            rsi_frameCmdCfgGet[RSI_BYTES_3];
extern const uint8            rsi_frameCmdFeatsel[RSI_BYTES_3];
extern const uint8            rsi_frameCmdSnr[RSI_BYTES_3];
extern const uint8            rsi_frameCmdLtcpConnStatus[RSI_BYTES_3];
extern const uint8            rsi_frameCmdSentBytesCount[RSI_BYTES_3];
extern const uint8            rsi_frameCmdAPconf[RSI_BYTES_3];
extern const uint8            rsi_frameCmdWepkey[RSI_BYTES_3];
extern const uint8            rsi_frameCmdUrlRsp[RSI_BYTES_3];
extern const uint8            rsi_frameCmdPingRequest[RSI_BYTES_3];
extern const uint8            rsi_frameCmdGpioconf[RSI_BYTES_3];
//!snmp commands
extern const uint8            rsi_frameCmdSnmpEnable[RSI_BYTES_3];
extern const uint8            rsi_frameCmdSnmpGetRsp[RSI_BYTES_3];
extern const uint8            rsi_frameCmdSnmpGetStats[RSI_BYTES_3];
extern const uint8            rsi_frameCmdSnmpGetNextRsp[RSI_BYTES_3];
extern const uint8            rsi_frameCmdSnmpTrap[RSI_BYTES_3];
//!IPV6 ipconfig command resp
extern const uint8            rsi_frameCmdIPconf6[RSI_BYTES_3];
//!Wireless firmware upgrade ok command
extern const uint8            rsi_frameCmdWirelessFwUpgrade[RSI_BYTES_3];
//!Roaming parameters frame 
extern const uint8            rsi_frameCmdRoamParams[RSI_BYTES_3];
//!High throughput Capabilities parameters frame 
extern const uint8            rsi_frameCmdHtCaps[RSI_BYTES_3];
//!WMM PS parameters frame 
extern const uint8            rsi_frameCmdWmmPs[RSI_BYTES_3];
//!WPS Method parameters frame 
extern const uint8            rsi_frameCmdWpsMethod[RSI_BYTES_3];
//!Multicast params 
extern const uint8            rsi_frameCmdMulticast[RSI_BYTES_3]; 
//!PER MODE
extern const uint8            rsi_frameCmdPerMode[RSI_BYTES_3];
//!PER STATS
extern const uint8            rsi_frameCmdPerStats[RSI_BYTES_3];
//! UART flow control Enable
extern const uint8            rsi_frameCmdUartFlowControl[RSI_BYTES_3];
//!Sleep ack
extern const uint8            rsi_sleepack[RSI_BYTES_3];
//!Multicast filter bitmap
extern const uint8            rsi_frameCmdMcastFilter[RSI_BYTES_3];

//!Management Response list
extern const uint8            rsi_mgmtRspList[];   

//!JSON & Webpage related
extern const uint8            rsi_frameCmdWebpageClearFiles[RSI_BYTES_3];
extern const uint8            rsi_frameCmdWebpageEraseFile[RSI_BYTES_3];
extern const uint8            rsi_frameCmdJSONCreate[RSI_BYTES_3];
extern const uint8            rsi_frameCmdJSONEraseFile[RSI_BYTES_3];

//!Set region 
extern const uint8            rsi_frameSetRegion[RSI_BYTES_3];
extern const uint8            rsi_frameSetRegionAp[RSI_BYTES_3];

//! Rejoin Params response Frame
extern const uint8            rsi_frameCmdRejoinParams[RSI_BYTES_3];

//! User store configuration Frame
extern const uint8			  rsi_frameCmdUserStoreCfg[RSI_BYTES_3];
//! Http ABort Frame
extern const uint8            rsi_frameCmdHttpABort[RSI_BYTES_3];

//! Socket configuration
extern const uint8   rsi_frameSocketConfig[RSI_BYTES_3];
//! RF Power Level
extern const uint8   rsi_frameCmdRFCurConfig[RSI_BYTES_3];

//! Trigger auto config Request Frame 
extern const uint8   rsi_frameCmdTriggerAutoConfigReq[RSI_BYTES_3];

/* set rtc time from host frame frame */ 
extern const uint8   rsi_frameCmdRtcTime[RSI_BYTES_3];

/* set DHCP client user class option frame */
extern const uint8   rsi_frameCmdDHCPUsrCls[RSI_BYTES_3]; 

/* set timeout value frame */
extern const uint8   rsi_frameCmdTimeout[RSI_BYTES_3];



/*feature frame */
extern const uint8   rsi_frameCmdFeatureFrame[RSI_BYTES_3];




/*===============================================*/
/**
 * Debug Structures
 */
typedef union {
    struct {
    uint8     assertion_type[4];
    uint8     assertion_level[4];
    } debugFrameSnd;
    uint8                 uDebugBuf[8];            //@ byte format to send to the spi interface,8 bytes
} rsi_uDebug;


/*===================================================*/
/**
 * set region
 */

typedef union {
    struct{
        uint8     setregion_code_from_user_cmd;
      /*Enable or disable set region from user:
         1-take from user configuration;
         0-Take from Beacons*/
        uint8     region_code;
       /*region code(1-US,2-EU,3-JP.4-World Domain)*/
        uint8 module_type[2];
        }setRegionFrameSnd;
        uint8 usetRegionBuf[4];
}rsi_usetregion;


/*======================================================*/
/**Set region in AP mode
*
*/
#define COUNTRY_CODE_LENGTH      3

#define MAX_POSSIBLE_CHANNEL     24
typedef union{
    struct{
        uint8     setregion_code_from_user_cmd;
      /*Enable or disable set region from user:
         1-take from user configuration;
         0-Take US or EU or JP*/
        uint8     country_code[3];
        /*region code(1-US,2-EU,3-JP)*/
        uint8     no_of_rules[4];
        struct{
            uint8   first_channel;
            uint8   no_of_channels;
            uint8   max_tx_power;
        }channel_info[MAX_POSSIBLE_CHANNEL];
    }setRegionApFrameSnd;
        uint8 usetRegionApBuf[80];
}rsi_usetregion_ap_t;


/*===============================================*/
/**
 * Scan Structures
 */

//! The scan command argument union/variables

typedef union {
    struct {
        uint8     channel[4];                             //@ RF channel to scan, 0=All, 1-14 for 2.5GHz channels 1-14
        uint8     ssid[RSI_SSID_LEN];                     //@ uint8[34], ssid to scan         
        uint8     reserved[5];                            //@ uint8[6], reserved fields
        uint8     scan_feature_bitmap;                    //@ uint8 , scan_feature_bitmap
        uint8     channel_bit_map_2_4[2];                 //@ uint8[2], channel bit map for 2.4 Ghz
        uint8     channel_bit_map_5[4];                   //@ uint8[4], channel bit map for 5 Ghz
    } scanFrameSnd;
    uint8                 uScanBuf[RSI_SSID_LEN + 16];    //@ byte format to send to the spi interface, 48 bytes
} rsi_uScan;

/*===============================================*/
/**
 * Multicast Structures
 */

//! Multicast command structure
typedef union {
      struct {
        uint8     ip_version[2];
        uint8     req_Type[2]; 
          union
          {
             uint8      ipv4_address[4];
             uint8      ipv6_address[16];
          }group_address;
         }multicastFrameSnd;
        uint8     uMulticastBuf[20];
}rsi_uMulticast;


/*===============================================*/
/**
 * BGScan Structures
 */

//! The BG scan command argument union/variables
typedef union {
     struct {
           uint8    bgscan_enable[2];                //@ enable or disable BG scan        
           uint8    enable_instant_bgscan[2];        //@ Is it instant bgscan or normal bgscan
           uint8    bgscan_threshold[2];             //@ bg scan threshold value
           uint8    rssi_tolerance_threshold[2];     //@ tolerance threshold
           uint8    bgscan_periodicity[2];           //@ periodicity
           uint8    active_scan_duration[2];         //@ sctive scan duration
           uint8    passive_scan_duration[2];        //@ passive scan duration
           uint8    multi_probe;                     //@ multi probe
         } bgscanFrameSnd;
     uint8           ubgScanBuf[15];
     //@ byte format to send to the spi interface, 68 bytes
} rsi_ubgScan;


/*===============================================*/
/**
 * Join Data Frame Structure
 */
//! The BG scan command argument union/variables
typedef union {
     struct {
           uint8    timeout_bitmap[4];                //@ enable timeout for join or scan        
           uint8    timeout_value[2];        //@ value of the timeout in ms
         } timeoutFrameSnd;
     uint8           utimeoutBuf[6];
     //@ byte format to send to the spi interface, 68 bytes
} rsi_utimeout;


/*===============================================*/
/**
 * Join Data Frame Structure
 */
typedef union {
    struct {
           uint8    reserved1;                       //@ reserved bytes:Can be used for security Type
           uint8    securityType;                    //@ 0- Open, 1-WPA, 2-WPA2,6-MIXED_MODE
           uint8    dataRate;                        //@ data rate, 0=auto, 1=1Mbps, 2=2Mbps, 3=5.5Mbps, 4=11Mbps, 12=54Mbps
           uint8    powerLevel;                      //@ transmit power level, 0=low (6-9dBm), 1=medium (10-14dBm, 2=high (15-17dBm)
           uint8    psk[RSI_PSK_LEN];                //@ pre-shared key, 63-byte string , last charecter is NULL
           uint8    ssid[RSI_SSID_LEN];              //@ ssid of access point to join to, 34-byte string
		   uint8    join_feature_bitmap;
           uint8    reserved2[2];                    //@ reserved bytes
           uint8    ssid_len;
		   uint8 	listen_interval[4];
       uint8  vap_id;
       uint8  join_bssid[6];
    } joinFrameSnd;
    uint8    uJoinBuf[RSI_SSID_LEN + RSI_PSK_LEN + 8];            
    //@ byte format to send to the spi interface, 106 (0x6A) bytes
} rsi_uJoin;

/*===============================================*/
/**
 * Stations Connected structure
 */
typedef struct {
    uint8 mac_addr[6]; 
}rsi_uConnected_station_t;

/*===============================================*/
/**
 * PSK Frame Structure
 */
typedef union {
  struct {
    uint8    TYPE;
    uint8    psk_or_pmk[RSI_PSK_LEN];
    uint8    ap_ssid[RSI_SSID_LEN] ;
  } PskFrameSnd;
  uint8 uPskBuf[1 + RSI_PSK_LEN + RSI_SSID_LEN];
} rsi_uPsk;


/*===============================================*/
/**
 * Disconnect Data Frame Structure
 */
typedef struct {
        uint8    mode_flag[2];                       //@ 0- Module in Client mode, 1- AP mode
        uint8    client_mac_addr[6];                 //@ client MAC address, Ignored/Reserved in case of client mode
}rsi_disassoc_t;


/*===============================================*/
/**
 * JSON Structures
 */
#define JSON_BUFFER_SIZE                    512
#define JSON_CHUNK_LEN                      1024
#define RSI_JSON_MAX_CHUNK_LENGTH           1024
typedef struct rsi_jsonCreateObject_s
{
    char    filename[24];
    uint8   total_length[2];
    uint8   current_length[2];
    char    json_data[RSI_JSON_MAX_CHUNK_LENGTH];
} rsi_jsonCreateObject_t;

typedef struct rsi_tfs_clear_files_s
{
    uint8   clear;
} rsi_tfs_clear_files_t;

typedef struct rsi_tfs_erase_file_s
{
    char    filename[24];
} rsi_tfs_erase_file_t;

/*=======================================================*/
/*
 * TCP/IP Configure structures
 */

typedef union {
    struct {
        uint8    dhcpMode;                           //@ 0=Manual, 1=Use DHCP
        uint8    ipaddr[4];                          //@ IP address of this module if in manual mode
        uint8    netmask[4];                         //@ Netmask used if in manual mode
        uint8    gateway[4];                         //@ IP address of default gateway if in manual mode
		    uint8    hostname[31];                       //@ DHCP client host name
        uint8    vap_id;                             //@ vap_id used in concurrent mode, 0 - Station and 1 - AP.
        uint8    fqdn_flag[4];                       //@ DHCP client FQDN flag option 81
    } ipparamFrameSnd;
    uint8                    uIpparamBuf[49];        
    //@ 16 bytes, byte format to send to spi
} rsi_uIpparam;

/*===============================================*/
/*
 * IPV6 Configure
 */
typedef union{
   struct{
    uint8     mode[2];
    uint8     prefixLength[2];
    uint8     ipaddr6[16];
    uint8     gateway6[16];
  }ipconf6FrameSnd;
   uint8 uIpconf6Buf[36];
}rsi_uIPconf6;


/*===================================================*/
/**
 * Socket Configure
 */

#define WEBS_MAX_URL_LEN   51
#define WEBS_MAX_HOST_LEN  51

typedef union {
  struct {
    uint8    ip_version[2];                     //@ ip version4 or 6
    uint8    socketType[2];                     //@ 0=TCP Client, 1=UDP Client, 2=TCP Server (Listening TCP)
    uint8    moduleSocket[2];                   //@ Our local module port number
    uint8    destSocket[2];                     //@ Port number of what we are connecting to
    union{
      uint8     ipv4_address[4];
      uint8     ipv6_address[16];
    }destIpaddr;
    uint8     max_count[2];
    uint8     tos[4];
    uint8     ssl_bitmap;
    uint8     ssl_ciphers;
    uint8     webs_resource_name[WEBS_MAX_URL_LEN];
    uint8     webs_host_name[WEBS_MAX_HOST_LEN];
    uint8     tcp_retry_count;
    uint8     socket_bitmap;
    uint8     rx_window_size;
    uint8     tcp_keepalive_initial_time[2];
    uint8     vap_id;
    uint8     socket_cert_inx;

  } socketFrameSnd;
    uint8                    uSocketBuf[141];        //@ 24 bytes, byte format to send to spi
} rsi_uSocket;

#if 0
/*===================================================*/
/**
 * Sockets Structure
 * Structure linking socket number to protocol
 */
typedef struct {
  //! Socket protocol
  uint8    protocol;                      
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
#endif

/*===================================================*/
/**
 * Socket Close
 */
typedef union {
    struct {
        uint8    socketDescriptor[2];                
        uint8    port_number[2];                   //@ 2bytes, socket descriptor to close
    } socketCloseFrameSnd;
    uint8                 uSocketCloseBuf[2];      //@ byte format to send to the spi interface, 2 bytes
} rsi_uSocketClose;


/*===================================================*/
/**
 * Socket Read
 */
typedef union {
  struct {
    //! Socket descriptor
    uint8      socketDescriptor; 

    //! Receive data length
    uint8      data_length[4];  

    //! Timeout in milli seconds
    uint8      timeout_in_ms[2];

  } socketReadFrameSnd;
    
   uint8                 uSocketReadBuf[7];      //@ byte format to send to the spi interface, 2 bytes
} rsi_uSocketRead;

/*===================================================*/
/**
 * LTCP socket Connection status
 */
typedef union {
    struct {
       uint8    socketDescriptor[2];              //@ 2bytes, socket descriptor for LTCP socket
    } queryLtcpConnStatusFrameSnd;
    uint8                 uLtcpConnStatusBuf[2];                    
    //@ byte format to send to the spi interface, 2 bytes
 } rsi_uQueryLtcpConnStatus;


/*===================================================*/
/**
 * sent bytes count
 */
typedef union {
    struct {
       uint8    socketDescriptor[2];              //@ 2bytes, socket descriptor
    } querySentBytesCountFrameSnd;
    uint8                 uSentBytesStatusBuf[2];                    
    //@ byte format to send to the spi interface, 2 bytes
 } rsi_uQuerySentBytesCount;


/**
 * New commads for WiseConnect
 *
 */

/*===================================================*/
/**
 * FIPS Mode 
 *
 */  
typedef union {
  struct {
    uint8 fips_mode_enable[4];
  } fipsModeFrameSnd;
  uint8         uFipsModeBuf[4];        
} rsi_uFipsMode;

/*===============================================*/
/**
 * RECHECK KEY  Frame Structure
 */
typedef union {
  struct {
    uint8    type; /*0- PMK 1- EAP password*/
    uint8    key_store;
    uint8    key[128]; /*PMK/EAP password*/
  } RecheckkeyFrameSnd;
  uint8 uRecheckKeyBuf[2 + 128];
} rsi_urecheck_key;

/*===============================================*/
/**
 * AUTO JOIN KEY  Frame Structure
 */
typedef union {
  struct {
    uint8    type[2]; /*0- PMK 1- EAP password*/
    uint8    key[128]; /*PMK/EAP password*/
  } AutojoinkeyFrameSnd;
  uint8 uAutojoinKeyBuf[2 + 128];
} rsi_uautojoin_key;

/*===============================================*/
/**
 * FWUPGRADATION KEY  Frame Structure
 */
typedef union {
  struct {
    uint8    key[16]; /*PMK/EAP password*/
  } FwupgradationkeyFrameSnd;
  uint8 uFwupgradationKeyBuf[16];
} rsi_ufwupgradation_key;

#define RSI_RPS_PAYLOAD_LEN 1024
typedef struct rsi_fw_up_frm_host_s{
  uint8 packet_info[4];
  uint8 payload[RSI_RPS_PAYLOAD_LEN];
}rsi_fw_up_t;

/*===================================================*/
/**
 * Operational Mode 
 *
 */  
typedef union {
    struct {
       uint8    oper_mode[4];                       //@ operating mode 0-client, 1-p2p, 2-EAP, 6-AP, 8-PER
       uint8    feature_bit_map[4];                 //@ BIT(0)-Open mode security, BIT(1)-PSK security, BIT(2) JSON objects
       uint8    tcp_ip_feature_bit_map[4];          //@ BIT(0)-tcp/ip bypass, BIT(1)-HTTP server,BIT(2)-DHCPV4 client, 
                                                    //@ BIT(3)-DHCPV6 client, BIT(4)-DHCPV4 server, BIT(5)-DHCPV6 server
       uint8   custom_feature_bit_map[4]; 
       uint8   ext_custom_feature_bit_map[4];       //@ extention of custom feature bitmap, valid only if BIT(31) of custom feature bitmap is set
       uint8   bt_feature_bit_map[4];              //@ BT custom feature bitmap, valid only if BIT(31) of extention custom feature bitmap is set
       uint8   ext_tcpip_feature_bit_map[4];
       uint8   ble_feature_bit_map[4];              //@ BLE custom feature bitmap, valid only if BIT(31) of bt custom feature bitmap is set
    } operModeFrameSnd;
    uint8    uOperModeBuf[28];                
} rsi_uOperMode;

/*===================================================*/
/**
 * Antenna Select 
 *
 */  
typedef union {
    struct {
        uint8    AntennaVal;                       //@ uint8, Antenna value to set    
        uint8    gain_2g;                       //@ uint8, Antenna 2G gain value    
        uint8    gain_5g;                       //@ uint8, Antenna 5G gain value    
    } AntennaSelFrameSnd;
    uint8     AnetnnaReqBuf[3];    
} rsi_uAntenna;


/*===================================================*/
/**
 * feature mode 
 *
 */  
typedef union {
    struct {
        uint8    pll_mode;                       //@ uint8, to set pll mode val   
        uint8    rf_type;                       //@ uint8, to select rf type    
        uint8    wireless_mode;                       //@ uint8, to select wireless mode    
        uint8    enable_ppp;                       //@ uint8, to select enable ppp   
        uint8    afe_type;                       //@ uint8, to select afe type   
        uint8  reserved[3];                       //@ reserved
        uint8  feature_enables[4];                       //@ uint32, feature enables    
    } FeatureFrameSnd;
    uint8     FeatureFrameReqBuf[12];  } rsi_uFeatureFrame;



/*===================================================*/
/**
 * Config p2p command 
 *
 */  
typedef union {
    struct {
        uint8    GOIntent[2];                     //@ GO Intent Value 0-15 for P2p GO or client , 16 - Soft AP 
        uint8    deviceName[64];                  //@ name of the device
        uint8    operChannel[2];                  //@ In which channel we are operating after becomes Group owner
        uint8    ssidPostFix[64];                 //@ Postfix SSID
        uint8    psk[64];                         //@ PSK of the device 
    }configP2pFrameSnd;
    uint8    uConfigP2pBuf[196];
}rsi_uConfigP2p;

/*===================================================*/
/**
 * DNS Server command 
 *
 */  
typedef union {
    struct {
        uint8    ip_version[2];
        uint8    DNSMode[2];
        union{
        uint8    ipv4_address[4];       
        uint8    ipv6_address[16];       
    }primary_dns_ip;
    union{
        uint8    ipv4_address[4];    
        uint8    ipv6_address[16];    
    }secondary_dns_ip;
    }dnsServerFrameSnd;
    uint8       uDnsBuf[36];
}rsi_uDnsServer;



/*===================================================*/
/**
 * DNS query command 
 *
 */  

#define MAX_URL_LEN 90
typedef union {
    struct {
        uint8    ip_version[2];
        uint8    aDomainName[MAX_URL_LEN];        
        uint8    uDNSServerNumber[2];            
    }dnsQryFrameSnd;
    uint8    uDnsQryBuf[MAX_URL_LEN + 4];
}rsi_uDnsQry;


/*===================================================*/
/**
 * DNS update command 
 *
 */  

#define MAX_ZONE_LEN 31
#define MAX_HOST_NAME_LEN 31

typedef union {
    struct {
        uint8    ip_version;
        uint8    aZoneName[MAX_ZONE_LEN];        
        uint8    aHostName[MAX_HOST_NAME_LEN];        
        uint8    uDNSServerNumber[2]; 
    		uint8    ttl[2];           
    }dnsUpdateFrameSnd;
    uint8    uDnsUpdateBuf[MAX_ZONE_LEN + MAX_HOST_NAME_LEN + 5];
}rsi_uDnsUpdate;


/*====================================================*/
/**
 * DHCP USER CLASS command
 *
 */
 
 
/************************DHCP User CLass MAcros************/
#define RSI_DHCP_USER_CLASS_MAX_COUNT           2      //@ MAX count of DHCP user class
#define RSI_DHCP_USER_CLASS_DATA_MAX_LEN	    64     //@ MAX DATA LENGTH of DHCP user class


typedef struct dhcp_user_class_data_s
{
  uint8 length;

  uint8 data[RSI_DHCP_USER_CLASS_DATA_MAX_LEN];
} dhcp_user_class_data_t;
typedef struct rsi_dhcp_user_class_s
{
	uint8 mode;
	//! User class list count
	uint8 count;

	//! User class data
	dhcp_user_class_data_t user_class_data[RSI_DHCP_USER_CLASS_MAX_COUNT];

} rsi_dhcp_user_class_t;

/*===================================================*/
/**
 * OTAF REQ command 
 *
 */ 
typedef union {
  struct {
    uint8    ip_version;
    union{
      uint8   ipv4_address[RSI_IP_ADD_LEN];
      uint8   ipv6_address[RSI_IP_ADD_LEN * 4];
    }server_address;
    uint8    server_port[4];        
    uint8    chunk_number[2]; 
    uint8    time_out[2];
    uint8    retry_count[2];           
  }OtafReqFrameSnd;
  uint8    uOtafReqBuf[27];
}rsi_uOtafReq;

/*===================================================*/
/**
 * Config EAP command 
 *
 */  
typedef union {
    struct {
        uint8   eapMethod[32];                    //@ EAP method
        uint8   innerMethod[32];                  //@ Inner method
        uint8   userIdentity[64];                 //@ user name
        uint8   password[128];                    //@ Password    
        uint8   okc_enable[4];                    //@Opportunistic Key Caching enable
        uint8   private_key_passwd[82];          //@ Private key password for encrypted certificates
    }setEapFrameSnd;
    uint8   uSetEapBuf[342];
}rsi_uSetEap;

/*===================================================*/
/**
 * Web server command 
 *
 */  
#define MAX_URL_LENGTH             40
#define MAX_POST_DATA_LENGTH       512

typedef struct
{
   uint8   url_length;
   uint8   url_name[MAX_URL_LENGTH];
   uint8   request_type;
   uint8   post_content_length[2];
   uint8   post_data[MAX_POST_DATA_LENGTH];

}rsi_urlReqFrameRcv; 

#define MAX_WEBPAGE_SEND_SIZE      1024
typedef struct 
{ 
    uint8   filename[24];
    uint8   total_len[2];
    uint8   current_len[2];
    uint8   has_json_data;
    uint8   webpage[MAX_WEBPAGE_SEND_SIZE];
} WebpageSnd_t;

typedef union 
{
    struct {
    WebpageSnd_t    Webpage_info; 
    }webServFrameSnd;
    uint8     uWebServBuf[1024 + 2 + 2 + 1 + 24]; //@ byte format to send to the spi interface, 1026 bytes 
}rsi_uWebServer;


/*===================================================*/
/**
 * Host Web page command 
 *
 */  
#define MAX_HOST_WEBPAGE_SEND_SIZE   1400

typedef struct 
{ 
    uint8   total_len[4];
    uint8   more_chunks;
    uint8   webpage[MAX_HOST_WEBPAGE_SEND_SIZE];
} HostWebpageSnd_t;


/*===================================================*/
/**
 * Web Fields command 
 *
 */
 
#define MAX_NO_OF_FIELDS  10
#ifdef PACKED 
typedef struct __attribute__((packed)){
#else
typedef struct {
#endif
  uint8   field_index;
  uint8   field_val[64];    
}field_st_t;

typedef union {
#ifdef PACKED 
  struct __attribute__((packed)){
#else
  struct {
#endif
    uint8    field_cnt;
    field_st_t   field_st[MAX_NO_OF_FIELDS];
  }webFieldsFrameSnd;
  uint8   uWebFieldBuf[680];                      //@ byte format to send to the spi interface, 680 bytes 
}rsi_uWebFields;

/*===================================================*/
/**
 * Set Mac Address
 */

typedef union
{
    struct {
        uint8    macAddr[6];                      //@ byte array, mac address
    } setMacAddrFrameSnd;
    uint8   setMacAddrBuf[6];                    
} rsi_uSetMacAddr;

/*===================================================*/
/**
 * Feature select
 */

typedef union
{
    struct {
        uint8    featsel_bitmapVal[4];            //@ 4 bytes, feat select bitmap value to set    
    } FeatselFrameSnd;
    uint8   uFeatselBuf[4];                    
} rsi_uFeatsel;

/*===================================================*/
/**
 * Band
 */

typedef union 
{
    struct {          
        uint8    bandVal;                         //@ uint8, band value to set    
    } bandFrameSnd;
    uint8    uBandBuf;                        
} rsi_uBand;

/*===================================================*/
/**
 * Cfg enable
 */

typedef union
{
    struct {
        uint8    cfg_enable;                      //@ uint8, config enable flag    
    } cfgEnableFrameSnd;
    uint8   ucfgEnableBuf;                        
} rsi_uCfgEnable;

/*===================================================*/
/**
 * 
 * UART Hardware flow control 
 *
 * */
typedef union {
struct {
  uint8 uart_hw_flowcontrol_enable;
}HwFlowControlEnableFrameSnd;
  uint8 uHwFlowControlEnableBuf;
}rsi_uHwFlowControl;


/*===================================================*/
/**
 * Sleep timer
 */

typedef union {
    struct {
        uint8   TimeVal[2];                       //@ 2bytes, sleep timer value to set    
    } SleepTimerFrameSnd;
    uint8   uSleepTimerBuf[2];                        
} rsi_uSleepTimer;

/*===================================================*/
/**
 * RTC time from host
 */

typedef struct module_rtc_time_s{
    uint8    tm_sec[4];                           //@ seconds after the minute [0-60] 
    uint8    tm_min[4];                           //@ minutes after the hour [0-59] 
    uint8    tm_hour[4];                          //@ hours since midnight [0-23] 
    uint8    tm_mday[4];                          //@ day of the month [1-31] 
    uint8    tm_mon[4];                           //@ months since January [0-11] 
    uint8    tm_year[4];                          //@ year since 0 
}module_rtc_time_t;


#define HTTP_BUFFER_LEN       1200
#define HTTP_POST_BUFFER_LEN  900
#define HTTP_PUT_BUFFER_LEN  900

/*===================================================*/
/** 
 *  HTTP GET / POST Request
  */
typedef union {
    struct {
        uint8  ip_version[2];                     //@ ip version 4 or 6
        uint8  https_enable[2];                   //@ enable http features
        uint8  http_port[2];                         //@ server port number
        uint8  buffer[HTTP_BUFFER_LEN];           //@ Username, Password,Hostname, IP address,url,header,data
    } HttpReqFrameSnd;
    uint8   uHttpReqBuf[HTTP_BUFFER_LEN + 6];
} rsi_uHttpReq;



/*===================================================*/
/** 
 *  HTTP POST DATA Request
  */
typedef union {
    struct {
        uint8  current_chunk_length[2];                     //@ HTTP data chunk length
        uint8  buffer[HTTP_POST_BUFFER_LEN];                //@ HTTP data
    } HttpPostDataReqFrameSnd;
    uint8   uHttpPostDataReqBuf[HTTP_POST_BUFFER_LEN + 2];
} rsi_uHttpPostDataReq;





/*===================================================*/
/**
 * HTTP GET/POST Response
 */
typedef struct TCP_EVT_HTTP_Data_t{
  uint8   more[4]; 
  uint8   offset[4];
  uint8   data_len[4];
  uint8   data[1400];
} rsi_uHttpRsp;

/*===================================================*/
/**
 * DNS query struct
 */



#define MAX_DNS_REPLY 10 

typedef struct TCP_EVT_DNS_Query_Resp 
{ 
 uint8   ip_version[2];
 uint8   uIPCount[2];  
 union{
   uint8   ipv4_address[4];
   uint8   ipv6_Address[16];
 }aIPaddr[MAX_DNS_REPLY]; 
}TCP_EVT_DNS_Query_Resp; 

/*===================================================*/
/**
 * Power mode
 */
typedef union {
    struct {
        uint8   powerVal;                         //@ uint8, power value to set
        uint8   ulp_mode_enable;                  //@ 0 - LP, 1 - ULP with RAM retention and 2 - ULP without RAM retention
        uint8   listen_interval_dtim;             //@ valid value is 0 and 1
        uint8   rsi_psp_type;                     //@ 0 - max PSP, 1 - Fast PSP and 2 - UAPSD
        uint16  monitor_interval;                  //@ Wake up time in ms when psp_type is 1
    } powerFrameSnd;
    uint8   uPowerBuf[6];         
} rsi_uPower;

/*===================================================*/
/**
 * Per mode
 */
typedef union {
    struct {
        uint8   per_mode_enable[2];               //@ uint8, enable/disable per mode
        uint8   power[2];                         //@ uint8, per mode power
        uint8   rate[4];                          //@ uint8, per mode rate
        uint8   length[2];                        //@ uint8, per mode length
        uint8   mode[2];                          //@ uint8, per mode mode
        uint8   channel[2];                       //@ uint8, per mode channel
        uint8   rate_flags[2];                    //@ uint8, per mode rate_flags
        uint8   reserved1[2];                     //@ uint8, per mode reserved
        uint8   aggr_enable[2];                   //@ uint8, per mode aggr_enable
        uint8   reserved2[2];                    //@ uint8, per mode reserved
        uint8   no_of_pkts[2];                    //@ uint8, per mode no_of_pkts
        uint8   delay[4];                         //@ uint8, per mode delay
    } perModeFrameSnd;
    uint8   uPerModeBuf[28];         
}rsi_uPerMode;

/*===================================================*/
/**
 * Per stats
 */
typedef union {
    struct {
      uint8 per_stats_enable[2];
      uint8 per_stats_channel[2];
    } perStatsFrameSnd;
    uint8   uPerStatsBuf[4];         
}rsi_uPerStats;

/*===================================================*/
/**
 * SEND
 */
typedef union {
    struct {
        uint8   ip_version[2];                    //@ ip version 4 or 6
        uint8   socketDescriptor[2];              //@ socket descriptor of the already opened socket connection
        uint8   sendBufLen[4];                    //@ length of the data to be sent
        uint8   sendDataOffsetSize[2];            //@ Data Offset, TCP=46, UDP=34
        uint8   padding[RSI_MAX_PAYLOAD_SIZE];    
        //@ large enough for TCP or UDP frames
    } sendFrameSnd;
    struct {
        uint8   ip_version[2];                    //@ ip version 4 or 6
        uint8   socketDescriptor[2];              //@ socket descriptor of the already opened socket connection
        uint8   sendBufLen[4];                    //@ length of the data to be sent
        uint8   sendDataOffsetSize[2];            //@ Data Offset, TCP=44, UDP=32
        uint8   destPort[2];
        union{
          uint8   ipv4_address[RSI_IP_ADD_LEN];
          uint8   ipv6_address[RSI_IP_ADD_LEN * 4];
    }destIPaddr;
        uint8   sendDataOffsetBuf[RSI_TXDATA_OFFSET_LUDP];    
        //@ Empty offset buffer, UDP=26
        uint8   sendDataBuf[RSI_MAX_PAYLOAD_SIZE];            
        //@ Data payload buffer, 1400 bytes max
    } sendFrameLudpSnd;
    uint8   uSendBuf[RSI_MAX_PAYLOAD_SIZE];        
    //@ byte format to send to spi, TCP is the larger of the two, 1456 bytes
} rsi_uSend;

/*=============================*/

typedef struct
{
  uint8      ip_version;
  uint8      ttl[2];
} mdns_init_t;

typedef struct
{
  uint8      port[2];
  uint8      ttl[2];
  uint8      more;
} mdns_reg_srv_t;

typedef struct rsi_mdns_t 
{
  uint8    command_type;
  union 
  {
    mdns_init_t      mdns_init;
    mdns_reg_srv_t   mdns_reg_srv;     
  } mdns_struct;
    
  uint8    buffer[1000];

} rsi_mdns_t;

/*=============================*/
/*===================================================*/
/**
 * Frame Descriptor
 */

typedef union {
    struct {
        uint8   dataFrmLenAndQueue[2];                
        //@ Data frame body length. Bits 14:12=queue, 010 for data, Bits 11:0 are the length
        uint8    padding[14];                     //@ Unused, set to 0x00
 } frameDscDataSnd;
    struct {
        uint8   mgmtFrmLenAndQueue[2];            
        //@ Data frame body length. Bits 14:12=queue, 000 for data, Bits 11:0 are the length
        uint8   mgmtRespType;                
        //@ Management frame descriptor response status, 0x00=success, else error
        uint8   padding[9];                       //@ Unused , set to 0x00
        uint8   mgmtFrmDscRspStatus;
        uint8   padding1[3];
    } frameDscMgmtRsp;
    uint8   uFrmDscBuf[RSI_FRAME_DESC_LEN];       //@ byte format for spi interface, 16 bytes
} rsi_uFrameDsc;

/*===================================================*/
/**
 * Roaming Parameters set structure
 */
typedef union {
    struct {
       uint8   roam_enable[4];
       uint8   roam_threshold[4];
       uint8   roam_hysteresis[4];
   }roamParamsFrameSnd;
 uint8   uRoamParamsBuf[12];
}rsi_uRoamParams;

/*===================================================*/
/**
 *Structure for rejoin_params
 */
typedef struct rsi_rejoin_params_s{
      uint8   rsi_max_try[4];
      uint8    rsi_scan_interval[4];
      uint8    rsi_beacon_missed_count[4];
      uint8   rsi_first_time_retry_enable[4];
} rsi_rejoin_params_t;


/*===================================================*/
/**
 * HT CAPS Parameters set structure
 */
typedef union { 
  struct {
    uint8    mode_11n_enable[2];
    uint8    ht_caps_bitmap[2];
  }htCapsFrameSnd;
  uint8   uHtCapsBuf[4];
}rsi_uHtCaps;


/*===================================================*/
/**
 * WMM PS Parameters set structure
 */
typedef union {
  struct {
    uint8   wmm_ps_enable[2];
    uint8   wmm_ps_type[2];
    uint8   wmm_ps_wakeup_interval[4];
    uint8   wmm_ps_uapsd_bitmap; 
   }wmmPsFrameSnd;
   uint8  uWmmPsBuf[9];
}rsi_uWmmPs;

/*===================================================*/
/**
 * WPS Parameters set structure
 */
#define RSI_WPS_PIN_LEN  8
typedef union { 
  struct {
    uint8   wps_method[2];
    uint8   generate_pin[2];
    uint8   wps_pin[RSI_WPS_PIN_LEN];
  }wpsMethodFrameSnd;
  uint8   uWpsMethodBuf[12];
}rsi_uWpsMethod;

/*===================================================*/
/**
 * Command Response Frame Union
 */
typedef struct {
    uint8   devState;                             //@ If New device  1; Device left 0
    uint8   devName[32];                          //@ Name the device found or left 32 bytes
    uint8   macAddress[6];                        //@ Mac address of the device 
    uint8   devtype[2];                           //@ Type of the device 1st byte inidcates primary device type;
                                                  //@ 2nd byte indicates sub catagory
}rsi_wfdDevInfo;

typedef struct {     
    rsi_wfdDevInfo  devInfo;    
}rsi_wfdDevRsp;

typedef struct {
       uint8   devCount;
       rsi_wfdDevInfo   strWfdDevInfo[RSI_MAX_WFD_DEV_CNT];    
                                                   //@ 32 maximum responses from scan command    
} rsi_wfdDevResponse;


typedef struct {
    uint8   rfChannel;                             //@ rf channel to us, 0=scan for all
    uint8   securityMode;                          //@ security mode, 0=open, 1=wpa1, 2=wpa2, 3=wep
    uint8   rssiVal;                               //@ absolute value of RSSI
    uint8   uNetworkType;
    uint8   ssid[RSI_SSID_LEN];                    //@ 32-byte ssid of scanned access point
    uint8   bssid[RSI_BSSID_LEN];
    uint8   reserved[2];
} rsi_scanInfo;

typedef struct {
    uint8   nwType;                                //@ network type, 0=Ad-Hoc (IBSS), 1=Infrastructure
    uint8   securityType;                          //@ security type, 0=Open, 1=WPA1, 2=WPA2, 3=WEP
    uint8   dataRate;                              //@ data rate, 0=auto, 1=1Mbps, 2=2Mbps, 3=5.5Mbps, 4=11Mbps, 12=54Mbps
    uint8   powerLevel;                            //@ transmit power level, 0=low (6-9dBm), 1=medium (10-14dBm, 2=high (15-17dBm)
    uint8   psk[RSI_PSK_LEN];                      //@ pre-shared key, 32-byte string
    uint8   ssid[RSI_SSID_LEN];                    //@ ssid of access point to join to, 32-byte string
    uint8   ibssMode;                              //@ Ad-Hoc Mode (IBSS), 0=Joiner, 1=Creator
    uint8   ibssChannel;                           //@ rf channel number for Ad-Hoc (IBSS) mode
    uint8   reserved;
} rsi_joinInfo;

typedef struct {
    uint8   scanCount[4];                          //@ 4 bytes, number of access points found
    uint8   padding[4];
    rsi_scanInfo    strScanInfo[RSI_AP_SCANNED_MAX];    
                                                   //@ 32 maximum responses from scan command
} rsi_scanResponse;

typedef struct {
  uint8   macAddress1[6];
  uint8   macAddress2[6];
}rsi_initResponse;

typedef struct {
    uint8   macAddress1[6];
    uint8   macAddress2[6];
}rsi_qryMacFrameRcv;

typedef struct {
    uint8   operState;    
}rsi_joinResponse;

typedef struct {
    uint8   rssiVal[2];                           //@ uint8, RSSI value for the device the module is currently connected to
} rsi_rssiFrameRcv;

typedef struct {
    uint8   snrVal;                               //@ uint8, RSSI value for the device the module is currently connected to
} rsi_snrFrameRcv;


typedef struct {
    uint8   ip_version[2];                       //@ ip version 4 or 6  
    uint8   socketType[2];                       //@ 2 bytes, type of socket created
    uint8   socketDescriptor[2];                 //@ uinr16, socket descriptor, like a file handle, usually 0x00
    uint8   moduleSocket[2];                     //@ 2 bytes, Port number of our local socket
    union{
      uint8   ipv4_addr[4];                      //@ 4 bytes, Our (module) IPv4 Address
      uint8   ipv6_addr[16];                     //@ 4 bytes, Our (module) IPv6 Address
  }moduleIPaddr;
  uint8   mss[2];                                //@ 2 bytes, Remote peer MSS size
  uint8   window_size[4];                        //@ 4 bytes, Remote peer Window size
} rsi_socketFrameRcv;

typedef struct {
    uint8   socketDsc[2];                         //@ 2 bytes, socket that was closed
    uint8   sentBytescnt[4];                      //@ 4 bytes, sent bytes count
} rsi_socketCloseFrameRcv;

typedef struct {
    uint8    macAddr[6];                          //@ MAC address of this module
    uint8    ipaddr[4];                           //@ Configured IP address
    uint8    netmask[4];                          //@ Configured netmask
    uint8    gateway[4];                          //@ Configured default gateway
} rsi_ipparamFrameRcv;

typedef struct {
    uint8   macAddr[6];                           //@ MAC address of this module
    uint8   ipaddr[4];                            //@ Configured IP address
    uint8   netmask[4];                           //@ Configured netmask
    uint8   gateway[4];                           //@ Configured default gateway
} rsi_recvIpChange;



typedef struct {
    uint8   prefixLength[2];                      //@ prefix length
    uint8   ipaddr6[16];                          //@ Configured IPv address
    uint8   defaultgw6[16];                       //@ Router IPv6 address
} rsi_ipconf6FrameRcv;

typedef struct {
    uint8   object_id[32];                         //@ Object id  
    uint8   length[4];                             //@ Length of set request
    uint8   value[200];                            //@ value to be set
} rsi_snmp_set;

typedef struct {
    uint8   wps_pin[RSI_WPS_PIN_LEN];
}rsi_wpsMethodFrameRcv;

/*Region Code response in set region command*/
typedef struct 
{
  //! region code selected
  uint8 region_code;
}rsi_uSetRegionRsp;

typedef struct {
    uint8   state[2];                             //@ 2 bytes, connection state, 0=Not Connected, 1=Connected
} rsi_conStatusFrameRcv;

typedef struct {

   uint8  sock_handle[2];
   uint8  SentBytes[4];

} rsi_sentBytesRsp;

typedef struct {
    uint8   socketDescriptor[2];
  uint8 ip_version[2];
  union
  {
    uint8   ipv4_address[4];
    uint8   ipv6_address[16];
  }dest_ip;
    uint8   dPort[2];
} rsi_LtcpConnStatusFrameRcv;

typedef struct sock_info_query_t
{
    uint8   sock_id[2];
    uint8   sock_type[2];
    uint8   sPort[2];
    uint8   dPort[2];
    union{
        uint8   ipv4_address[4];
        uint8   ipv6_address[16];
      }destIpaddr;
    
}sock_info_query_t;

typedef struct ftp_rsp_t
{
  uint8 command_type;
  uint8 more;
  uint16 length;
  uint8 data[1024];
}rsi_ftp_rsp_t;


typedef struct rsi_sntp_rsp_t
{
  uint8 command_type;
  uint8 sntp_buffer[50];
}rsi_sntp_rsp_t;


//! SMTP response structure
typedef struct rsi_smtp_rsp_t
{
  //! Receive SMTP command type
  uint8 command_type;
}rsi_smtp_rsp_t;

typedef struct rsi_http_client_put_rsp_t
{
  //!Receive HTTP_PUT command type
  uint8 command_type;
  
  //! HTTP file/resource end of content
  uint8 end_of_file;

}rsi_http_client_put_rsp_t;

//! SNTP server response structure
typedef struct rsi_sntp_server_rsp_t
{
  UINT8  ip_version;
  union
  {
    UINT8  ipv4_address[4];
    UINT8  ipv6_address[16];
  }server_ip_address;

  UINT8  sntp_method;
}rsi_sntp_server_rsp_t;


//! SNTP server info response structure
typedef struct rsi_sntp_server_info_rsp_t
{
  UINT8  command_type;
  UINT8  ip_version;
  union
  {
    UINT8  ipv4_address[4];
    UINT8  ipv6_address[16];
  }server_ip_address;

  UINT8  sntp_method;
}rsi_sntp_server_info_rsp_t;

typedef struct mdns_rsp_t
{
  uint8 command_type;

}rsi_mdns_rsp_t;

#define MN_NUM_SOCKETS             10 
typedef struct {
    uint8   wlanState;                            //@ uint8, 0=NOT Connected, 1=Connected
    uint8   Chn_no;                               //@ channel number of connected AP 
    uint8   psk[64];                              //@ PSK 
    uint8   mac_addr[6];                          //@ Mac address
    uint8   ssid[RSI_SSID_LEN];                   //@ uint8[32], SSID of connected access point
    uint8   connType[2];                          //@ 2 bytes, 0=AdHoc, 1=Infrastructure
    uint8   sec_type;
    uint8   dhcpMode;                             //@ uint8, 0=Manual IP Configuration,1=DHCP
    uint8   ipaddr[4];                            //@ uint8[4], Module IP Address
    uint8   subnetMask[4];                        //@ uint8[4], Module Subnet Mask
    uint8   gateway[4];                           //@ uint8[4], Gateway address for the Module
    uint8   num_open_socks[2];                    //@ number of sockets opened
    uint8   prefix_length[2];                     //@ prefix length for ipv6 address
    uint8   ipv6addr[16];                         //@ modules ipv6 address
    uint8   defaultgw6[16];                       //@ router ipv6 address
    uint8   tcp_stack_used;                       //@ BIT(0) =1 - ipv4, BIT(1)=2 - ipv6, BIT(0) & BIT(1)=3 - BOTH
    sock_info_query_t   socket_info[MN_NUM_SOCKETS];    
} rsi_qryNetParmsFrameRcv;


#define MAX_STA_SUPPORT 4

struct  go_sta_info_s
{
   uint8   ip_version[2];                         //@ IP version if the connected client
   uint8   mac[6];                                //@ Mac Address of the connected client
  union
  {
    uint8   ipv4_address[4];                      //@ IPv4 Address of the Connected client
    uint8   ipv6_address[16];                     //@ IPv6 Address of the Connected client
  }ip_address;
#ifdef PACKED
}__attribute__((packed));                         //@ to avoid padding in the structures
#else
};
#endif

typedef struct {
    uint8   ssid[RSI_SSID_LEN];                   //@ SSID of the P2p GO
    uint8   bssid[6];                             //@ BSSID of the P2p GO
    uint8   channel_number[2];                    //@ Operating channel of the GO 
    uint8   psk[64];                              //@ PSK of the GO
    uint8   ipv4_address[4];                      //@ IPv4 Address of the GO
    uint8   ipv6_address[16];                     //@ IPv6 Address of the GO
    uint8   sta_count[2];                         //@ Number of stations Connected to GO
    struct  go_sta_info_s sta_info[MAX_STA_SUPPORT];
}rsi_qryGOParamsFrameRcv;



typedef struct {
    uint8    fwversion[20];                       //@ uint8[20], firmware version text string, x.y.z as 1.3.0
} rsi_qryFwversionFrameRcv;


typedef struct {
    uint8   ip_version[2];                        //@ 2 bytes, the ip version of the ip address , 4 or 6
    uint8   recvSocket[2];                        //@ 2 bytes, the socket number associated with this read event
    uint8   recvBufLen[4];                        //@ 4 bytes, length of data received
    uint8   recvDataOffsetSize[2];                //@ 2 bytes, offset of data from start of buffer
    uint8   fromPortNum[2];                       //@ 2 bytes, port number of the device sending the data to us
    union{
      uint8   ipv4_address[4];                    //@ 4 bytes, IPv4 Address of the device sending the data to us
      uint8   ipv6_address[16];                   //@ 4 bytes, IPv6 Address of the device sending the data to us
  }fromIPaddr;
    uint8    recvDataOffsetBuf[RSI_RXDATA_OFFSET_UDP_V4]; 
                                                  //@ uint8, empty offset buffer, 14 for UDP, 42 bytes from beginning of buffer
    uint8    recvDataBuf[RSI_MAX_PAYLOAD_SIZE];   //@ uint8, buffer with received data
} rsi_recvFrameUdp;

typedef struct {
      uint8   ip_version[2];                      //@ 2 bytes, the ip version of the ip address , 4 or 6
      uint8   recvSocket[2];                      //@ 2 bytes, the socket number associated with this read event
      uint8   recvBufLen[4];                      //@ 4 bytes, length of data received
      uint8   recvDataOffsetSize[2];              //@ 2 bytes, offset of data from start of buffer
      uint8   fromPortNum[2];                     //@ 2 bytes, port number of the device sending the data to us
    union{
        uint8   ipv4_address[4];                  //@ 4 bytes, IPv4 Address of the device sending the data to us
        uint8   ipv6_address[16];                 //@ 4 bytes, IPv6 Address of the device sending the data to us
  }fromIPaddr;
    uint8   recvDataOffsetBuf[RSI_RXDATA_OFFSET_UDP_V6];
    //@ uint8, empty offset buffer, 14 for UDP, 42 bytes from beginning of buffer
    uint8   recvDataBuf[RSI_MAX_PAYLOAD_SIZE];    //@ uint8, buffer with received data
} rsi_recvFrameUdp6;


typedef struct { 
      uint8   ip_version[2];                      //@ 2 bytes, the ip version of the ip address , 4 or 6
      uint8   recvSocket[2];                      //@ 2 bytes, the socket number associated with this read event
      uint8   recvBufLen[4];                      //@ 4 bytes, length of payload data received
      uint8   recvDataOffsetSize[2];              //@ 2 bytes, offset of data from start of buffer
      uint8   fromPortNum[2];                     //@ 2 bytes, port number of the device sending the data to us
    union{
       uint8   ipv4_address[4];                   //@ 4 bytes, IPv4 Address of the device sending the data to us
       uint8   ipv6_address[16];                  //@ 4 bytes, IPv6 Address of the device sending the data to us
  }fromIPaddr;
    uint8   recvDataOffsetBuf[RSI_RXDATA_OFFSET_TCP_V4]; //@ uint8, empty offset buffer, 26 for TCP
    uint8   recvDataBuf[RSI_MAX_PAYLOAD_SIZE];    //@ uint8, buffer with received data
} rsi_recvFrameTcp;

typedef struct { 
    uint8   ip_version[2];                        //@ 2 bytes, the ip version of the ip address , 4 or 6
    uint8   recvSocket[2];                        //@ 2 bytes, the socket number associated with this read event
    uint8   recvBufLen[4];                        //@ 4 bytes, length of payload data received
    uint8   recvDataOffsetSize[2];                //@ 2 bytes, offset of data from start of buffer
    uint8   fromPortNum[2];                       //@ 2 bytes, port number of the device sending the data to us
    union{
       uint8   ipv4_address[4];                   //@ 4 bytes, IPv4 Address of the device sending the data to us
       uint8   ipv6_address[16];                  //@ 4 bytes, IPv6 Address of the device sending the data to us
  }fromIPaddr;
    uint8   recvDataOffsetBuf[RSI_RXDATA_OFFSET_TCP_V6]; //@ uint8, empty offset buffer, 26 for TCP
    uint8   recvDataBuf[RSI_MAX_PAYLOAD_SIZE];    //@ uint8, buffer with received data
} rsi_recvFrameTcp6;

typedef struct {
    uint8   socket[2];                                //@ uint8, socket handle for the terminated connection
    uint8   sentBytescnt[4];                          //@ 4 bytes, sent bytes count
} rsi_recvRemTerm;

typedef struct{
 union{
  uint8   ipv4_address[4];                         //@ primary DNS IPv4
  uint8   ipv6_address[16];                        //@ primary DNS IPv6
 }primary_dns_ip;

 union{
    uint8   ipv4_address[4];                       //@ secondary DNS IPv4
    uint8   ipv6_address[16];                      //@ secondary DNS IPv6
 }secondary_dns_ip;
}rsi_dnsserverResponse;

typedef struct {
    uint8   ip_version[2];
    uint8   sock_id[2];                           //@ 2 bytes, socket handle 
    uint8   fromPortNum[2];                       //@ 2 bytes, remote port number 
    union{
        uint8   ipv4_address[4];                  //@  remote IPv4 Address 
        uint8   ipv6_address[16];                 //@  remote IPv6 Address 
        }dst_ip_address;
    uint8   mss[2];                               //@ 2 bytes, remote peer MSS size 
    uint8   window_size[4];                       //@ 4 bytes, remote peer Window size
	uint8  srcPortNum[2];
} rsi_recvLtcpEst;

/* Certificate loading related macros */
#define MAX_CERT_SEND_SIZE 1400

struct cert_info_s
{
  uint8   total_len[2];
  uint8   CertType;
  uint8   more_chunks;
  uint8   CertLen[2];
  uint8   Cert_inx;
  uint8   KeyPwd[127];
#ifdef PACKED
}__attribute__((packed));                         //@packed is used to avoid padding
#else
};
#endif

#define MAX_DATA_SIZE (MAX_CERT_SEND_SIZE - sizeof(struct cert_info_s))

struct SET_CHUNK_S
{ 
    struct  cert_info_s cert_info;    
    uint8   Certificate[MAX_DATA_SIZE];
};


/*==================================================*/
/*This will keep the wepkey params*/

typedef struct {
    uint8   index[2];
    uint8   key[4][32];
}rsi_wepkey;

/*====================================================*/
/*This will keep the AP configuration parameter*/

typedef struct {
    uint8   channel_no[2];    
    uint8   ssid[RSI_SSID_LEN];    
    uint8   security_type;
    uint8   encryp_mode;    
    uint8   psk[RSI_PSK_LEN];
    uint8   beacon_interval[2];    
    uint8   dtim_period[2];    
    uint8   ap_keepalive_type;
    uint8   ap_keepalive_period;
    uint8   max_sta_support[2]; // it can be configured from 1 to 4
}rsi_apconfig;

/*===================================================*/
/**
 * HTTP Credentials command 
 *
 */  

#define MAX_USERNAME_LEN 31  //! Including NULL character
#define MAX_PASSWORD_LEN 31  //! Including NULL character
typedef union {
    struct {
        uint8    username[MAX_USERNAME_LEN];
        uint8    password[MAX_PASSWORD_LEN];        
    }httpCredentialsFrameSnd;
    uint8    uhttpCredentialsBuf[MAX_USERNAME_LEN + MAX_PASSWORD_LEN];
}rsi_uhttpCredentials;
/*SNMP command structure*/
#define MAX_SNMP_VALUE 200

/* Maximum length of OID */
#define MAX_OID_LENGTH     128
typedef union {
    struct {
        uint8   type;
        uint8   value[MAX_SNMP_VALUE];     
        uint8   objid[MAX_OID_LENGTH];
    } snmpFrameSnd;
    uint8   uSnmpBuf[MAX_SNMP_VALUE + 4 + MAX_OID_LENGTH];
} rsi_uSnmp;



typedef struct SNMP_OBJECT_DATA_STRUCT
{

  uint8           snmp_object_data_type[4];                       /* Type of SNMP data contained         */
  uint8           snmp_object_data_msw[4];                        /* Most significant 32 bits            */ 
  uint8           snmp_object_data_lsw[4];                        /* Least significant 32 bits           */ 
  uint8           snmp_ip_version[4];
  union{
    uint8   ipv4_address[4];     
    uint8   ipv6_address[16];
  }snmp_nxd_address;
  uint8           snmp_object_octet_string_size[4];               /* Size of OCTET string                */ 

} SNMP_OBJECT_DATA;

typedef struct SNMP_TRAP_OBJECT_STRUCT
{

    uint8               snmp_object_string_ptr[40];              /* SNMP object string*/
    SNMP_OBJECT_DATA    snmp_object_data;                        /* SNMP object data */
} SNMP_TRAP_OBJECT;


#define  RSI_SNMP_TAP_BUFFER_LENGTH  1024

/*SNMP trap structure*/
typedef union {
  struct {
    uint8   snmp_version;
    uint8   ip_version[4];
    union{
      uint8   ipv4_address[4];     
      uint8   ipv6_address[16];
    }destIPaddr;
    uint8   community[32];
    uint8   trap_type;
    uint8   elapsed_time[4]; 
    uint8   trap_oid[51];
    uint8   obj_list_count;
    uint8   snmp_buf[RSI_SNMP_TAP_BUFFER_LENGTH];
  } snmptrapFrameSnd;

     uint8   uSnmptrapBuf[110+RSI_SNMP_TAP_BUFFER_LENGTH];
} rsi_uSnmptrap;

/* Structure for SNMP Enable */
typedef union{
  struct{
    uint8   snmpEnable;
 }snmpEnableFrameSnd;
  uint8   uSnmpenableBuf;
}rsi_uSnmpEnable;


//! PUF
typedef union{
  struct{
    uint8    key_index;
    uint8    key_size;
    uint8    key[32];
  }pufSetKeyFrameSnd;
  uint8 uPufsetkeyBuf[36];
} rsi_uPufsetkey; 


typedef union{
  struct{
    uint8    key_code[44];
  }pufGetKeyFrameSnd;
  uint8 uPufgetkey[44];
} rsi_uPufgetkey; 


typedef union{
  struct{
    uint8    key_code[44];
  }pufLoadKeyFrameSnd;
  uint8 uPufloadkey[44];
} rsi_uPufloadkey; 

typedef union{
  struct{
    uint8    mode;
    uint8    key[32];
    uint8    iv[32];
    uint8    data_size[2];
    uint8    data[1400];
  }aesEncryptFrameSnd;
  uint8 uAesEncrypt[1468];
} rsi_uAesencrypt; 

typedef union{
  struct{
    uint8    mode;
    uint8    key[32];
    uint8    iv[32];
    uint8    data_size[2];
    uint8    data[1400];
  }aesDecryptFrameSnd;
  uint8 uAesDecrypt[1468];
} rsi_uAesdecrypt; 

typedef union{
  struct{
    UINT8    mode;
    UINT8    key[32];
    UINT8    iv[32];
    UINT8    data_size[2];
    UINT8    data[1400];
  }aesMacFrameSnd;
  uint8 uAesMac[1468];
} rsi_uAesMac; 

/* Ping Response Frame */
typedef struct {
	uint8   ip_version[2];
	uint8   ping_size[2]; 
	union{
		uint8   ipv4_addr[4];                         //@ 4 bytes, Our (module) IPv4 Address
		uint8   ipv6_addr[16];                        //@ 4 bytes, Our (module) IPv6 Address
	}ping_address;
} rsi_uPingRsp;

/*structure for ping request command*/
typedef struct rsi_ping_request_s{
  uint8   ip_version[2];
  uint8   ping_size[2];
  union{
    uint8   ipv4_address[4];
    uint8   ipv6_address[16];
  }ping_address;
  
  uint8  timeout[2];
}rsi_ping_request_t;

/*PSK response structure*/
typedef struct{
  uint8   pmk[32];
}rsi_PmkResponse;

/*P2P connection request from wi-fi device*/
typedef struct rsi_p2p_conn_req_s{
  uint8   device_name[32];    
}rsi_p2p_conn_req_t;

/* Module state response */
typedef struct rsi_state_notificaton_s{
    uint8   TimeStamp[4];
    uint8   stateCode;
    uint8   reason_code;
    uint8   rsi_channel;
    uint8   rsi_rssi;
    uint8   rsi_bssid[6];
} rsi_state_notificaton_t;


/*User store configuration parameters*/
#define MAX_HTTP_SERVER_USERNAME 31
#define MAX_HTTP_SERVER_PASSWORD 31

typedef struct sc_params_s
{
  uint8    cfg_enable;
  uint8    opermode[4];
  uint8    feature_bit_map[4];
  uint8    tcp_ip_feature_bit_map[4];
  uint8    custom_feature_bit_map[4];
  uint8    band;
  uint8    scan_feature_bitmap;
  uint8    join_ssid[RSI_SSID_LEN];
  uint8    uRate;
  uint8    uTxPower;
  uint8    reserved_1;
  uint8    reserved_2;
  uint8    scan_ssid_len;
  uint8    keys_restore;
  uint8    csec_mode;
  uint8    psk[RSI_PSK_LEN];
  uint8    scan_ssid[RSI_SSID_LEN];
  uint8    scan_cnum;
  uint8    dhcp_enable;
#define IP_ADDRESS_SZ 4
  uint8    ip[IP_ADDRESS_SZ];
  uint8    sn_mask[IP_ADDRESS_SZ];
  uint8    dgw[IP_ADDRESS_SZ];

  uint8    eap_method[32];
  uint8    inner_method[32];
  uint8    user_identity[64];
  uint8    passwd[128];

  uint8    go_intent[2];
  uint8    device_name[64];
  uint8    operating_channel[2];
  uint8    ssid_postfix[64];
  uint8    psk_key[64];
#define  WISE_PMK_LEN 32
  uint8    pmk[WISE_PMK_LEN];
  rsi_apconfig apconfig;
  uint8    module_mac[6];
  uint8    antenna_select[2];
  uint8    fips_bypass_mode[2];
  rsi_wepkey wep_key;
  uint8    dhcp6_enable[2];
  uint8    prefix_length[2];
  uint8    ip6[16];
  uint8    dgw6[16];
  uint8    tcp_stack_used;
  uint8    bgscan_magic_code[2];
  uint8    bgscan_enable[2];
  uint8 	 bgscan_threshold[2];
  uint8 	 rssi_tolerance_threshold[2];
  uint8 	 bgscan_periodicity[2];
  uint8 	 active_scan_duration[2];
  uint8 	 passive_scan_duration[2];
  uint8    multi_probe;
  //!Channel bitmap info
  uint8    chan_bitmap_magic_code[2];
  uint8    scan_chan_bitmap_stored_2_4_GHz[4];
  uint8    scan_chan_bitmap_stored_5_GHz[4];
  //!Roaming Params info
  uint8    roam_magic_code[2];
  rsi_uRoamParams  roam_params_stored;
  //!rejoin params info
  uint8    rejoin_magic_code[2];
  rsi_rejoin_params_t rejoin_param_stored;
  uint8     region_request_from_host;
  uint8     rsi_region_code_from_host;
  uint8     region_code;
  uint8 	  reserved_4[43];
  uint8     multicast_magic_code[2];
  uint8     multicast_bitmap[2];
  uint8     powermode_magic_code[2];
  uint8     powermode;
  uint8     ulp_mode;
  uint8     wmm_ps_magic_code[2];
  uint8     wmm_ps_enable;
  uint8     wmm_ps_type;
  uint8     wmm_ps_wakeup_interval[4];
  uint8     wmm_ps_uapsd_bitmap;
  uint8     listen_interval[4];
  uint8     listen_interval_dtim;
  
  uint8     ext_custom_feature_bit_map[4];
  uint8     private_key_password[82];
  uint8     join_bssid[6];
  uint8    join_feature_bitmap;

  //! HT caps
  rsi_uHtCaps ht_caps;
  uint8      ht_caps_magic_word[2];
  
  //! Fast psp parameters
  uint8      fast_psp_enable;
  uint8      monitor_interval[2];

  //! Request timeout parameters
  uint8      req_timeout_magic_word[2];
  uint8      timeout_value[2];
  uint8      timeout_bitmap[4];

  //! AP IP parameters in Concurrent mode
  UINT8     dhcp_ap_enable;
  UINT8     ap_ip[4]; /* Module IP address */
  UINT8     ap_sn_mask[4]; /* Sub-net mask */
  UINT8     ap_dgw[4]; /* Default gateway */

  uint8     dhcp6_ap_enable[2]; /* DHCPv6 enable or disable */
  UINT8     ap_prefix_length[2];/* Prefix length */
  UINT8     ap_ip6[16];       /* Module IPv6 address */  
  UINT8     ap_dgw6[16];       /* Module IPv6 address */
  UINT8     ext_tcp_ip_feature_bit_map[4];
  

  /* HTTP/HTTPS Server credentials */
  uint8 http_credentials_avail;
  uint8 http_username[MAX_HTTP_SERVER_USERNAME];
  uint8 http_password[MAX_HTTP_SERVER_PASSWORD];

}rsi_user_store_config_t, rsi_cfgGetFrameRcv;


typedef struct {
    uint8   dev_name[32];                         //@ All the characters are part of device name, no Null termination 
} rsi_ConnAcceptRcv;


/* PER stats response */
typedef struct per_stats_s
{
   //! no. of tx pkts
  uint8 tx_pkts[2];
  //! no. of rx pkts
  uint8 reserved_1[2];
  //! no. of tx retries
  uint8 tx_retries[2];
  //! no. of pkts that pass crc
  uint8 crc_pass[2];
  //! no. of pkts failing crc chk
  uint8 crc_fail[2];
  //! no. of times cca got stuck
  uint8 cca_stk[2];
  //! no of times cca didn't get stuck
  uint8 cca_not_stk[2];
  //! no. of pkt aborts
  uint8 pkt_abort[2];
  //! no. of false rx starts
  uint8 fls_rx_start[2];
  //! cca idle time
  uint8 cca_idle[2];
  //! Reserved fields
  uint8 reserved_2[26];
  //! no. of rx retries
  uint8 rx_retries[2];
  //! rssi value
  uint8 reserved_3[2];
  //! cal_rssi
  uint8 cal_rssi[2];
  //! lna_gain bb_gain
  uint8 reserved_4[4];
	//! xretries pkts dropped
  //! number of tx packets dropped after maximum retries 
	uint8 xretries[2];
	//! consecutive pkts dropped
	uint8 max_cons_pkts_dropped[2];
  uint8 reserved_5[2];
  //! BSSID matched broadcast packets count
  uint8 bss_broadcast_pkts[2];
  //! BSSID matched multicast packets count
  uint8 bss_multicast_pkts[2];
  //! BSSID & multicast filter matched packets count
  uint8 bss_filter_matched_multicast_pkts[2];

}rsi_uPerStatsRsp;


/* Card ready Response */
typedef struct card_ready_s
{
  //! Boot loader checksum
  uint8 bootloader_checksum[4];

  //! Firmware checksum
  uint8 firmware_checksum[4];
}rsi_card_readyRsp;

/* Store Config checksum Response */
typedef struct store_config_chksum_s
{
  uint8 checksum[20];
}rsi_store_config_checksumRsp;

//! Generate DRBG random
typedef struct generate_drbg_rand_s
{
  uint8 random_number[32];
}rsi_generate_DRBGRandRsp;


//! Socket configuration request frame
typedef struct socket_config_s
{
  //! TCP TX + TCP RX + UDP TX + UDP RX
  uint8 total_sockets; 

  //! TCP TX + TCP RX
  uint8 total_tcp_sockets;

  //! UDP TX + UDP RX
  uint8 total_udp_sockets;

  //! TCP TX
  uint8 tcp_tx_only_sockets;

  //! TCP RX
  uint8 tcp_rx_only_sockets;

  //! UDP TX
  uint8 udp_tx_only_sockets;

  //! UDP RX
  uint8 udp_rx_only_sockets;

  //! TCP RX High Performance
  uint8 tcp_rx_high_performance_sockets;
  
} rsi_socket_config_t;

//! RF Current mode configuration 
typedef struct rf_current_config_s
{
  //! RF RX current/Power mode
  uint8 rf_rx_curr_mode;
  //! RF TX current/Power mode
  uint8 rf_tx_curr_mode;
  //! RF TX strength in dbm
  int16 rf_tx_dbm;
} rsi_rf_current_config_t;

/* PUF Setkey Response */
typedef struct puf_setkey_s
{
	//! Key Code for set key
    uint8 key_code[44];
}rsi_puf_setkeyRsp;

/* PUF Getkey Response */
typedef struct puf_getkey_s
{
	//! Key for get key
    uint8 key[32];
}rsi_puf_getkeyRsp;

/* AES Encrypt Response */
typedef struct aes_encrypt_s
{
	//! Encrypted data
    uint8 encryt_data[1400];
}rsi_aes_encryptRsp;

/* AES Decrypt Response */
typedef struct aes_decrypt_s
{
	//! decrypted data
    uint8 decryt_data[1400];
}rsi_aes_decryptRsp;

/* AES MAC Response */
typedef struct aes_mac_s
{
	//! MAC data
    uint8 mac[32];
}rsi_aes_macRsp;

//! FTP client
#define FTP_USERNAME_LENGTH   31
#define FTP_PASSWORD_LENGTH   31
#define FTP_PATH_LENGTH       51
#define FTP_MAX_CHUNK_LENGTH  1400

typedef struct ftp_connect
{
  //! FTP client IP cersion
  uint8 ip_version;

  union
  {
    //! IPv4 address
    UINT8  ipv4_address[4];

    //! IPv6 address
    UINT8  ipv6_address[16];
  } server_ip_address;

  //! FTP client username
  uint8 username[FTP_USERNAME_LENGTH];

  //! FTP client password
  uint8 password[FTP_PASSWORD_LENGTH];

  //! FTP server port 
  uint8 server_port[4];
} ftp_connect_t;

typedef struct ftp_command
{
  //! Directory or file path
  uint8 path[FTP_PATH_LENGTH];

  //! New file name
  uint8 new_file_name[FTP_PATH_LENGTH];
} ftp_command_t;

typedef struct ftp_file_write
{
  //! command type
  uint8 command_type;

  //! End of file
  uint8 end_of_file;

  //! Path of file to write
  uint8 file_content[FTP_MAX_CHUNK_LENGTH];

} rsi_ftp_file_write_t;

typedef struct
{
  //! FTP command type
  uint8              command_type;

  union
  {
    //! structure for FTP connect
    ftp_connect_t    ftp_connect;

    //! Structure for other commands
    ftp_command_t    ftp_command;

  }ftp_client_struct;

}rsi_ftp_client_t;

typedef struct 
{
  UINT8  command_type;
  UINT8  ip_version;
  union
  {
    UINT8  ipv4_address[4];
    UINT8  ipv6_address[16];
  }server_ip_address;

  UINT8  sntp_method;
  UINT8  sntp_timeout[2];

} rsi_sntp_client_t;


/*
 * SMTP client
 */

#define SMTP_CLIENT_CREATE              1
#define SMTP_CLIENT_INIT                2
#define SMTP_CLIENT_MAIL_SEND           3
#define SMTP_CLIENT_DEINIT              4           


/* Define for SMTP client initialization */

typedef struct 
{
  //! SMTP server ip version
  uint8 ip_version;

  union
  {
    //! Server ipv4 address
    uint8  ipv4_address[4];

    //! Server ipv6 address
    uint8  ipv6_address[16];

  } server_ip_address;
  
  //! SMTP server authentication type
  uint8  auth_type;

  //! SMTP server port number
  uint8 server_port[4];

}  smtp_client_init_t;


/* Define for SMTP client mail send */

typedef struct 
{
  //! SMTP mail priority level
  uint8 smtp_feature;

  // SMTP client mail body length
  uint8 smtp_client_mail_body_length[2];

} smtp_mail_send_t;


/* Define SMTP client structure */

#define  RSI_SMTP_BUFFER_LENGTH  1024

typedef struct 
{
  //! SMTP client command type
  uint8 command_type;

  //! SMTP client command structure
  union
  {
    smtp_client_init_t  smtp_client_init;
    smtp_mail_send_t    smtp_mail_send;
  
  } smtp_struct;

  uint8  smtp_buffer[RSI_SMTP_BUFFER_LENGTH];

} rsi_smtp_client_t;


/*
 * HTTP PUT client
 */

#define HTTP_CLIENT_PUT_CREATE 1
#define HTTP_CLIENT_PUT_START  2
#define HTTP_CLIENT_PUT_PACKET 3
#define HTTP_CLIENT_PUT_DELETE 4
#define RSI_HTTP_CLIENT_PUT_BUFFER_LENGTH  900

/* Define for HTTP PUT client initialization */

typedef struct  rsi_http_client_put_start_t
{
	//! HTTP server ip version
	uint8 ip_version;

	//! HTTPS bit map
	uint8 https_enable[2];

	//! HTTP server port number
	uint8 port_number[4];

	//! HTTP Content Length
	uint8 content_length[4];

} rsi_http_client_put_start_t;

/* Define for HTTP PUT client current length */


typedef struct rsi_http_client_put_data_req_t
{
	//! Current chunk length
	uint8 current_length[2];

} rsi_http_client_put_data_req_t;

/* Define for HTTP PUT client structure  */


typedef struct  rsi_http_client_put_req_s
{
	//! Command type
	uint8  command_type;

	union
	{
		rsi_http_client_put_start_t http_client_put_start;
		rsi_http_client_put_data_req_t http_client_put_data_req;

	}  http_client_put_struct;

	uint8 http_put_buffer[RSI_HTTP_CLIENT_PUT_BUFFER_LENGTH];
} rsi_http_client_put_req_t;



/*
 * POP3 client
 */


#define POP3_CLIENT_MAX_USERNAME_LENGTH   101
#define POP3_CLIENT_MAX_PASSWORD_LENGTH   101

//! POP3 client commands
#define POP3_CLIENT_SESSION_CREATE              1
#define POP3_CLIENT_GET_MAIL_STATS              2
#define POP3_CLIENT_GET_MAIL_LIST               3
#define POP3_CLIENT_RETR_MAIL                   4           
#define POP3_CLIENT_MARK_MAIL                   5           
#define POP3_CLIENT_UNMARK_MAIL                 6            
#define POP3_CLIENT_GET_SERVER_STATUS           7           
#define POP3_CLIENT_SESSION_DELETE              8


typedef struct pop3_client_session_create
{
    //! POP3 server ip version
  uint8 ip_version;

  union
  {
    //! Server ipv4 address
    uint8  ipv4_address[4];

    //! Server ipv6 address
    uint8  ipv6_address[16];

  } server_ip_address;

  //! POP3 server port number
  uint8 server_port_number[2];

  //! POP3 client authentication type
  uint8 auth_type;

  //! POP3 client username
  uint8 username[POP3_CLIENT_MAX_USERNAME_LENGTH];

  //! POP3 client password
  uint8 password[POP3_CLIENT_MAX_PASSWORD_LENGTH];

} pop3_client_session_create_t;

typedef struct {

  //! POP3 client command type
  uint8 command_type;

  //! POP3 client command structure

  union
  {
    //! POP3 client session create structure
    pop3_client_session_create_t  pop3_client_session_create;

    //! POP3 client mail index
    uint8     pop3_client_mail_index[2];

  } pop3_struct;

} rsi_pop3_client_t;


typedef struct  rsi_pop3_rsp_s
{

  uint8 command_type;

  //! Total number of mails
  uint8  mail_count[2];

  //! Total size of all the mails
  uint8  size[4];

} rsi_pop3_rsp_t;


typedef struct  rsi_pop3_mail_data_resp_s
{
  //! Type of the POP3 client command
  uint8 command_type;

  //! More data pending flag
  uint8 more;

  //! Length the mail chunk
  uint8 length[2];
  
  //! Data buffer
  uint8  data[1000];

} rsi_pop3_mail_data_resp_t;



typedef struct {
    uint8   rspCode[2];
    uint8   status[2];                  
    //@ 0- For Success ,Non-Zero Value is the Error Code return
    union {
      //@ response payload    
      rsi_qryMacFrameRcv              qryMacaddress;
      rsi_initResponse                initResponse;          
      rsi_scanResponse                scanResponse;
      rsi_joinResponse                joinResponse;
      rsi_PmkResponse                 PmkResponse;   
      rsi_wfdDevResponse              wfdDevResponse;
      rsi_rssiFrameRcv                rssiFrameRcv;
      rsi_socketFrameRcv              socketFrameRcv;
      rsi_socketCloseFrameRcv         socketCloseFrameRcv;
      rsi_ipparamFrameRcv             ipparamFrameRcv;
      rsi_recvIpChange                recvIpchange;
      rsi_ipconf6FrameRcv             ipconf6FrameRcv;
      rsi_conStatusFrameRcv           conStatusFrameRcv;
      rsi_qryNetParmsFrameRcv         qryNetParmsFrameRcv;
      rsi_qryGOParamsFrameRcv         qryGoParamsFrameRcv;
      rsi_qryFwversionFrameRcv        qryFwversionFrameRcv;
      rsi_dnsserverResponse           dnsserverresponse;    
      rsi_recvFrameUdp                recvFrameUdp;
      rsi_recvFrameTcp                recvFrameTcp;  
      rsi_recvFrameUdp6               recvFrameUdp6;
      rsi_recvFrameTcp6               recvFrameTcp6; 
      rsi_recvRemTerm                 recvRemTerm;
      rsi_recvLtcpEst                 recvLtcpEst;
      TCP_EVT_DNS_Query_Resp          dnsqryresponse;
      rsi_snrFrameRcv                 snrFrameRcv;    
      rsi_cfgGetFrameRcv              cfgGetFrameRcv;
      rsi_LtcpConnStatusFrameRcv      LtcpConnStatRcv;
      rsi_urlReqFrameRcv              urlReqRcv;
      rsi_p2p_conn_req_t              dev_req;
      rsi_ConnAcceptRcv               ConnectReqRcv;
      rsi_snmp_set                    snmp_set;
      rsi_wpsMethodFrameRcv           wpsMethodFrameRcv;
      rsi_state_notificaton_t         stateFrameRcv;
      rsi_uHttpRsp                    httpFrameRcv;
      rsi_uSetRegionRsp               setRegFrameRcv;
      rsi_uPingRsp                    rsi_pingFrameRcv;
      rsi_uPerStatsRsp                rsi_perFrameRcv;
      rsi_card_readyRsp               rsi_card_readyRcv;
      rsi_store_config_checksumRsp    rsi_cfg_chksumRcv;
      rsi_generate_DRBGRandRsp        rsi_generateDRBGrandRcv; 
      rsi_sentBytesRsp                SentBytes;
      rsi_ftp_rsp_t                   ftpFrameRcv;
      rsi_puf_setkeyRsp				        setkeyFrameRcv;
      rsi_puf_getkeyRsp				        getkeyFrameRcv;
      rsi_aes_encryptRsp				      aesEncryptedFrameRcv;
      rsi_aes_decryptRsp				      aesDecryptedFrameRcv;
      rsi_aes_macRsp					        aesMacFrameRcv;
      rsi_sntp_rsp_t                  sntpFrameRcv;
      rsi_sntp_server_rsp_t           sntp_ServerFrameRcv;
      rsi_sntp_server_info_rsp_t      sntpServerInfoRcv;
      rsi_mdns_rsp_t                  mdnsFrameRcv;
      rsi_smtp_rsp_t                  smtpFrameRcv;
      rsi_http_client_put_rsp_t       httpClientPutFrameRcv;
      rsi_pop3_rsp_t                  pop3CmdFrameRcv;
      rsi_pop3_mail_data_resp_t       pop3MailContentRcv;
      uint8                           uCmdRspBuf[RSI_MAX_PAYLOAD_SIZE];
    }uCmdRspPayLoad;
} rsi_uCmdRsp;


/*===================================================*/
/**
 * Interrupt Handeling Structure
 */
typedef struct {
    uint8   mgmtPacketPending;                    //@ TRUE for management packet pending in module
    uint8   dataPacketPending;                    //@TRUE for data packet pending in module
    uint8   powerIrqPending;                      //@ TRUE for power interrupt pending in the module
    uint8   bufferFull;                           //@ TRUE=Cannot send data, FALSE=Ok to send data
    uint8   bufferEmpty;                          //@TRUE, Tx buffer empty, seems broken on module
    uint8   isrRegLiteFi;
} rsi_intStatus;




/*==============================================*/

typedef struct {
      uint8    *ptrRecvBuf;                       //@ Location of the payload data
      uint8    recvBufLen[4];                        //@ Length of the payload data
      int      recvType;                          //@ Whether this receive event was a remote disconnect or read data
      uint8    socketNumber[2];                   //@ The socket number the event is associated with
} recvArgs;




/*==================================================*/
/**
 * This structure maintain power save state.
 *
 */ 
typedef struct {
    uint8   current_mode;
    uint8   ack_pwsave;
    uint8   sleep_received;
    uint8   ack_sent;
}rsi_powerstate;

//! Debug Print Levels
#define RSI_DEBUG_LVL         0x00ff
//! These bit values may be ored to all different combinations of debug printing
#define RSI_PL0                0xffff
#define RSI_PL1                0x0001
#define RSI_PL2                0x0002
#define RSI_PL3                0x0004
#define RSI_PL4                0x0008
#define RSI_PL5                0x0010
#define RSI_PL6                0x0020
#define RSI_PL7                0x0040
#define RSI_PL8                0x0080
#define RSI_PL9                0x0100
#define RSI_PL10               0x0200
#define RSI_PL11               0x0400
#define RSI_PL12               0x0800
#define RSI_PL13               0x1000
#define RSI_PL14               0x2000
#define RSI_PL15               0x4000
#define RSI_PL16               0x8000

//#define RSI_DPRINT(lvl, fmt, args)              if (lvl & DEBUG_LVL) printf(fmt, ##args)



enum RSI_INTERRUPT_TYPE {
    RSI_TXBUFFER_FULL       = 0x01,
    RSI_TXBUFFER_EMPTY      = 0x02,
    RSI_MGMT_PENDING        = 0x04,
    RSI_DATA_PENDING        = 0x08,
    RSI_PWR_MODE            = 0x10
};

/**
 * Enumerations
 */
enum RSI_PROTOCOL {
    RSI_PROTOCOL_UDP_V4     = 0x00,
    RSI_PROTOCOL_TCP_V4     = 0x01,
    RSI_PROTOCOL_UDP_V6     = 0x02,
    RSI_PROTOCOL_TCP_V6     = 0x03
};

#define BUFFER_FULL_FAILURE -3

/* Status indications */
#define BUFFER_FULL     0x01
#define BUFFER_EMPTY    0x02
#define RX_PKT_PENDING  0x08
#define POWER_SAVE      0x08



#define RSI_STATUS_OFFSET         12
#define RSI_TWOBYTE_STATUS_OFFSET 12 
#define RSI_RSP_TYPE_OFFSET       2

extern struct SET_CHUNK_S set_chunks;
extern rsi_wfdDevResponse wfdDevData;
extern uint32   interrupt_rcvd;

#ifdef RSI_HAL
#include "rsi_hal.h"
#endif
#include "platform_specific.h"
#include "rsi_api.h"
#ifdef LINUX_PLATFORM
#include "string.h"
#endif
#endif
