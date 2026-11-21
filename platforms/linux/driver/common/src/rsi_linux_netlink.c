/*******************************************************************************
* @file  rsi_linux_netlink.c
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
 * @file      rsi_linux_netlink.c
 * @version   1.2
 * @date      2019-Mar-07
 *
 *
 *
 * @brief This contains all the functions with netlink socket
 * usage
 *
 * @section Description
 * This file contains the following functions.
 *    rsi_read_req_from_userspace
 *    rsi_send_rsp_to_userspace
 *    rsi_register_genl
 *    rsi_unregister_genl
 *
 */

#include <net/genetlink.h>
#include "rsi_linux.h"
#include "rsi_global.h"
#if KERNEL_VERSION_GREATER_THAN_EQUALS_2_6_(33)
#include <net/sock.h>
#endif
#if (defined (RSI_USB_INTERFACE) || defined (RSI_SDIO_INTERFACE) || defined(RSI_SPI_INTERFACE))
#include "rsi_config.h"
extern RSI_STATUS rsi_submit_rx_urb(PRSI_ADAPTER Adapter,UINT8 endpoint);
#endif
#ifdef RSI_UART_INTERFACE
INT32 rsi_send_rsp_to_userspace(PRSI_ADAPTER Adapter, UINT8 *rspBuff, UINT8 to_uart);
#endif

/* attributes (variables): the index in this enum is used as a reference for the type,
 *             userspace application has to indicate the corresponding type
 *             the policy is used for security considerations
 */
enum
{
  RSI_USER_A_UNSPEC,
  RSI_USER_A_MSG,
  __RSI_USER_A_MAX,
};
#ifdef RSI_UART_INTERFACE
#if RSI_TCP_IP_BYPASS
volatile int ack_flag = 0;
#endif
#endif
#define RSI_USER_A_MAX (__RSI_USER_A_MAX - 1)

/* attribute policy: defines which attribute has which type (e.g int, char * etc)
 * possible values defined in net/netlink.h
 */
static struct nla_policy rsi_user_genl_policy[RSI_USER_A_MAX + 1] = {
#if LINUX_VERSION_CODE == KERNEL_VERSION(2,6,18)
  [RSI_USER_A_MSG] = { .type = NLA_STRING },
#else
  [RSI_USER_A_MSG] = { .type = NLA_NUL_STRING },
#endif
};
extern struct net_device *glbl_net_device;

#define RSI_VERSION_NR 1
/* family definition */
static struct genl_family rsi_user_gnl_family = {
  //.id = RSI_GENL_ID_GENERATE,         //!genetlink should generate an id
  .hdrsize = 0,
  .name = "CTRL_PKT_TXRX",            //!the name of this family, used by userspace application
  .version = RSI_VERSION_NR,          //!version number
  .maxattr = RSI_USER_A_MAX,
};

/* commands: enumeration of all commands (functions),
 * used by userspace application to identify command to be executed
 */
enum {
  RSI_USER_C_UNSPEC,
  RSI_USER_C_CMD,
  __RSI_USER_C_MAX,
};

#if 0
 static inline int genlmsg_unicast(struct sk_buff *skb, u32 portid)
 {
         return nlmsg_unicast(skb, portid);
 }
#endif

#define RSI_USER_C_MAX (__RSI_USER_C_MAX - 1)

/*==============================================*/
/**
 * @fn          INT16 rsi_update_device(PRSI_ADAPTER Adapter,
 *                                     UINT8 *data)
 * @brief       updates the device
 * @param[in]   PRSI_ADAPTER Adapter, Pointer to Adapter
 * @param[in]   UINT8 *data
 * @param[out]  none
 * @return      Reurns 0
 * @section description
 * This function used to parse the data to be updated.
 * This is used when user sends some info to update.
 * Used to update MAC address and FSM state
 */
INT16 rsi_update_device(PRSI_ADAPTER Adapter, UINT8 *data)
{
  UINT8 type = *(data + RSI_DESC_LEN);

#ifdef RSI_DEBUG_PRINT
  RSI_DEBUG(RSI_ZONE_SPI_DBG,"\nrsi_update_device:\n");
#endif
  switch(type)
  {
    case UPDATE_JOIN_DONE:
      rsi_memcpy(Adapter->net_device0->dev_addr, data + RSI_DESC_LEN + 1/* type */, 6);
      Adapter->FSM_STATE = FSM_OPEN;
      break;

    case UPDATE_CONCURRENT_AP_JOIN_DONE:
      rsi_memcpy(Adapter->net_device1->dev_addr, data + RSI_DESC_LEN + 1/* type */, 6);
	    Adapter->FSM_STATE_AP1 = FSM_OPEN;
      break;    

    case MODULE_POWER_CYCLE:
      //rsi_module_power_cycle(Adapter->hDevice);
      break;

  case PS_CONTINUE:
    //rsi_pwrsave_continue(Adapter->hDevice);
    break;

  case WKP_FROM_HOST:
    //rsi_wakeup_from_host(Adapter->hDevice);
    break;
    case SOFT_RESET:
      Adapter->FSM_STATE = FSM_RESET;
      break;
#ifdef RSI_AUTOMATION_ENABLE
   case TCP_IP_BYPASS_MACRO:
   Adapter->tcp_ip_bypass = *(data + RSI_DESC_LEN + 1);
   break;
#endif
#ifdef ENABLE_WMM_FEATURE
    case WMM_UPDATE:
	rsi_memcpy(Adapter->wmm_ac_params, data + RSI_DESC_LEN+1 , 16);
	Adapter->wmmparamsfw[0].aifsn  = Adapter->wmm_ac_params[1].aifsn;
	Adapter->wmmparamsfw[0].cw_max = (1 << Adapter->wmm_ac_params[1].cw_max) - 1;
	Adapter->wmmparamsfw[0].cw_min = (1 << Adapter->wmm_ac_params[1].cw_min) - 1;
	Adapter->wmmparamsfw[0].tx_op  = (Adapter->wmm_ac_params[1].tx_op);

	Adapter->wmmparamsfw[1].aifsn  = Adapter->wmm_ac_params[0].aifsn;
	Adapter->wmmparamsfw[1].cw_max = (1 << Adapter->wmm_ac_params[0].cw_max) - 1;
	Adapter->wmmparamsfw[1].cw_min = (1 << Adapter->wmm_ac_params[0].cw_min) - 1;
	Adapter->wmmparamsfw[1].tx_op  = (Adapter->wmm_ac_params[0].tx_op);

	Adapter->wmmparamsfw[2].aifsn  = Adapter->wmm_ac_params[2].aifsn;
	Adapter->wmmparamsfw[2].cw_max = (1 << Adapter->wmm_ac_params[2].cw_max) - 1;
	Adapter->wmmparamsfw[2].cw_min = (1 << Adapter->wmm_ac_params[2].cw_min) - 1;
	Adapter->wmmparamsfw[2].tx_op  = (Adapter->wmm_ac_params[2].tx_op);

	Adapter->wmmparamsfw[3].aifsn  = Adapter->wmm_ac_params[3].aifsn;
	Adapter->wmmparamsfw[3].cw_max = (1 << Adapter->wmm_ac_params[3].cw_max) - 1;
	Adapter->wmmparamsfw[3].cw_min = (1 << Adapter->wmm_ac_params[3].cw_min) - 1;
	Adapter->wmmparamsfw[3].tx_op  = (Adapter->wmm_ac_params[3].tx_op);

	Adapter->wmm_vi_acm =  Adapter->wmm_ac_params[2].acm;

	update_wmm_params(Adapter);
      break;
#endif

  default:
    break;
  }

  return 0;
}



/*==============================================*/
/**
 * @fn          INT32 rsi_read_req_from_userspace(
 *                          struct genl_info *info)
 * @brief       Gets the command request from user space
 *              over netlink socket
 * @param[in]   struct sk_buff *skb_2, pointer to sk_buff structure
 * @param[in]   struct genl_info *info, read command info pointer
 * @param[out]  none
 * @return      errCode
 *              0  = SUCCESS
 *              else FAIL
 * @section description
 * This API is used to read the command from user over netlink
 * socket.
 */
INT32 rsi_read_req_from_userspace(struct sk_buff *skb_2, struct genl_info *info)
{
  PRSI_ADAPTER Adapter = rsi_getpriv(glbl_net_device);
  struct nlattr *na;
  INT32 rc;
  void *msg_head;
  UINT8 * mydata;
  INT16 retval = 0;
  UINT8 i=0;
  uint16 send_len;
  RSI_STATUS Status = RSI_STATUS_SUCCESS;

#ifdef RSI_DEBUG_PRINT
  RSI_DEBUG(RSI_ZONE_SPI_DBG,"\nrsi_read_req_from_userspace:\n");
#endif
  if (info == NULL)
    goto out;


  /*for each attribute there is an index in info->attrs which points to a nlattr structure
   *in this structure the data is given
   */
  na = info->attrs[RSI_USER_A_MSG];
  if (na)
  {
    mydata = (char *)rsi_nla_data(na);
    if (mydata == NULL)
      RSI_DEBUG(RSI_ZONE_SPI_DBG,"error while receiving data\n");
  }
  else
    RSI_DEBUG(RSI_ZONE_SPI_DBG,"no info->attrs %i\n", RSI_USER_A_MSG);

  /* mydata points to the descriptor + payload of the command.
   * We need to send this to WSC module by writing it over SPI interface.
   */
  if((*(mydata + 2 /* type offset */) != 0xE1) && (*(mydata + 2) != 0xEE))
  {
    rsi_down_interruptible(&Adapter->int_check_sem);
#ifdef RSI_UART_INTERFACE
    if(get_portid(info) == UART_PORT_ID)
    {      
      rsi_send_rsp_to_userspace(Adapter, mydata, TO_PROTOCOL);
    }
    else
    {      
      rsi_send_rsp_to_userspace(Adapter, mydata, TO_UART_APP);
    }
#else
    send_len = mydata[0] | (mydata[1] <<8);
    send_len = send_len & 0xfff;
    do
    {
      retval = rsi_execute_cmd(mydata, mydata+16, send_len);
    }while(retval == -3);
#endif
    rsi_up_sem(&Adapter->int_check_sem);
  }
  else if(*(mydata + 2) == 0xEE)
  {
#ifdef RSI_UART_INTERFACE
#if RSI_TCP_IP_BYPASS
    if(ack_flag)
	  {
      ack_flag = 0;
      Adapter->data_ack_missed = 1; 
      rsi_Set_Event(&Adapter->DATA_ACKEvent);
	  }
#endif
#endif
#if 0
    rsi_down_interruptible(&Adapter->int_check_sem);
    rsi_set_intr_mask(*(mydata + 16));
    rsi_up_sem(&Adapter->int_check_sem);
#endif
#if 0
    retval = rsi_spi_iface_init();
    if(retval == RSI_SUCCESS)
    {
      RSI_DEBUG(RSI_ZONE_INIT,"rsi_probe:rsi_spi_iface_init SUCCESSFULL\n");
    }
    else if(retval == RSI_BUSY)
    {
      RSI_DEBUG(RSI_ZONE_INIT,"rsi_probe:rsi_spi_iface_init BUSY\n");
      return -1;
    }
    else
    {
      RSI_DEBUG(RSI_ZONE_INIT,"rsi_probe:rsi_spi_iface_init FAIL\n");
      return -1;
    }
#endif

#ifdef RSI_USB_INTERFACE
     if(!Adapter->irq_registered)
     {
       rsi_submit_rx_urb(Adapter,USB_WLAN);
       rsi_submit_rx_urb(Adapter,USB_BT_ZB);
#ifdef RS9116
			 rsi_submit_rx_urb(Adapter,USB_ZB_ONLY);
#endif
     }
     Adapter->irq_registered = 1;
#endif
  }
  else
  {
    retval = rsi_update_device(Adapter, mydata);
  }

  if( get_portid(info) == WLAN_PORT_ID)
  {
    Adapter->wlan_seq_num = info->snd_seq;
  } 
  else if( get_portid(info) == ZB_PORT_ID)
  {
    Adapter->zb_seq_num = info->snd_seq;
  }
  else if( get_portid(info) == BT_PORT_ID)
  {
    Adapter->bt_seq_num = info->snd_seq;
  }
#ifdef RSI_UART_INTERFACE
  else if( get_portid(info) == UART_PORT_ID)
  {
    Adapter->uart_seq_num = info->snd_seq;
  }
#endif

  return retval;

out:
  RSI_DEBUG(RSI_ZONE_SPI_DBG,"an error occured in rsi_read_pkt_from_userspace:\n");
  return -2;
}

/*==============================================*/
/**
 * @fn          INT32 rsi_send_rsp_to_userspace(
 *                          PRSI_ADAPTER Adapter,
 *                          rspBuff)
 * @brief       Sends the response to user space over netlink
 *              socket
 * @param[in]   PRSI_ADAPTER Adapter, pointer to adapter
 * @param[in]   rspBuff, pointer to response buffer
 * @param[out]  none
 * @return      errCode
 *              -1 = FAIL
 *              0  = SUCCESS
 * @section description
 * This API is used to read the command from user over netlink
 * socket.
 */
#ifdef RSI_UART_INTERFACE
INT32 rsi_send_rsp_to_userspace(PRSI_ADAPTER Adapter, UINT8 *rspBuff, UINT8 to_uart)
{
#else
INT32 rsi_send_rsp_to_userspace(PRSI_ADAPTER Adapter, UINT8 *rspBuff)
{
#endif
  struct nlattr *na=NULL;
  struct sk_buff *skb = NULL;
  INT32 rc;
  void *msg_head=NULL;
  UINT16 rspBufLen, total_len;
  INT16 retval;
  UINT8 i=0;
#ifdef RSI_UART_INTERFACE
#if RSI_TCP_IP_BYPASS
 uint8 data_ack = 0;
#endif
#endif
  UINT8 queue_no;
  UINT32 seq_num,snd_pid =0 ;
#ifdef RSI_DEBUG_PRINT
  RSI_DEBUG(RSI_ZONE_SPI_DBG,"\nrsi_send_rsp_to_userspace:\n");
#endif
  /* send the response to user space */
  /* allocate some memory, since the size is not yet known use NLMSG_GOODSIZE*/
  rspBufLen = rsi_bytes2R_to_uint16(rspBuff);
  rspBufLen &= 0x0FFF;

  queue_no  = rspBuff[1] >> 4;
  total_len = RSI_DESC_LEN + rspBufLen;
#ifdef RSI_UART_INTERFACE
  if(to_uart == TO_PROTOCOL)
#endif
 {
  
#if RSI_TCP_IP_BYPASS
Adapter->tcp_ip_bypass = 1;
#endif
  if((Adapter->tcp_ip_bypass || (rspBuff[2] == 1)) && ((rspBuff[1] >> 4) == SND_DATA_Q) && (total_len > RSI_DESC_LEN))
  {
    if (rsi_indicate_packet(Adapter, &rspBuff[RSI_DESC_LEN], rspBufLen, rspBuff[(RSI_DESC_LEN - 1)]) != 0)
    {
      RSI_DEBUG(RSI_ZONE_ERROR, "rsi_indicate_packet: Error! Unsuccessful attempt at indicating\n");
      return RSI_STATUS_FAIL;
    }
    return 0;
  }

  }
  skb = rsi_genlmsg_new(NLMSG_GOODSIZE, GFP_KERNEL);
  if (skb == NULL)
  {
    retval = 1;
    goto out;
  }

  if(!Adapter)
  {
    goto out;
  }
 
#if KERNEL_VERSION_GREATER_THAN_EQUALS_2_6_(33)
#ifdef RSI_SPI_INTERFACE
  struct net *net = &init_net;
#endif
#ifdef RSI_USB_INTERFACE
  struct net *net = &init_net;
#endif
#ifdef RSI_UART_INTERFACE
  struct net *net = &init_net;
#endif
#ifdef RSI_SDIO_INTERFACE
  struct net *net = &init_net;
#endif
#else

#if  KERNEL_VERSION_GREATER_THAN_EQUALS_2_6_(32)
 struct net *net = &init_net;
#endif
#endif

#if  KERNEL_VERSION_GREATER_THAN_EQUALS_2_6_(32) 
if(!net)
  {
    goto out;
  }
#endif
  /* create the message headers */
  /* arguments of genlmsg_put:
     struct sk_buff *,
     INT32 (sending) pid,
     INT32 sequence number,
     struct genl_family *,
     INT32 flags,
     u8 command index (why do we need this?)
  */
#ifdef RSI_UART_INTERFACE
  if(to_uart == 1)
  {
    seq_num = Adapter->uart_seq_num;
    snd_pid = UART_PORT_ID;
  }
  else
#endif
  {
      seq_num = Adapter->wlan_seq_num;
      snd_pid = WLAN_PORT_ID;
  }
#if LINUX_VERSION_CODE == KERNEL_VERSION(2,6,18)
  msg_head = rsi_genlmsg_put(skb, 0, seq_num + 1, rsi_user_gnl_family.id, 0,0, RSI_USER_C_CMD,rsi_user_gnl_family.version);
#else
  msg_head = rsi_genlmsg_put(skb, 0, seq_num + 1, &rsi_user_gnl_family, 0, RSI_USER_C_CMD);
#endif
  if (msg_head == NULL) {
    retval = 2;
    rc = -ENOMEM;
    goto out;
  }
 /* add a RSI_USER_A_MSG attribute (actual value to be sent) */
  rc = rsi_nla_put(skb, RSI_USER_A_MSG, total_len, rspBuff);
  if (rc != 0)
  {
    retval = 3;
    goto out;
  }
#ifdef RSI_UART_INTERFACE
#if RSI_TCP_IP_BYPASS
  if ((to_uart == TO_UART_APP) && (queue_no == WLAN_DATA_TYPE) &&(!ack_flag))
  {
      rsi_lock_bh(&Adapter->uart_data_ack_flag_sem);
      ack_flag = 1;
      rsi_unlock_bh(&Adapter->uart_data_ack_flag_sem);
  }
  
  if ((to_uart == TO_PROTOCOL) && (queue_no == WLAN_MGMT_TYPE) && (skb->data[26] == 0xac) )
  {
  
    data_ack = 1;
    if(skb->data[36] == 0x3F)
     {
       rsi_lock_bh(&Adapter->uart_data_ack_flag_sem);
       ack_flag = 2;
       rsi_unlock_bh(&Adapter->uart_data_ack_flag_sem);
       rsi_Set_Event(&Adapter->DATA_ACKEvent);
     }
     else
     {
       rsi_lock_bh(&Adapter->uart_data_ack_flag_sem);
       ack_flag = 0;
       rsi_unlock_bh(&Adapter->uart_data_ack_flag_sem);
       rsi_Set_Event(&Adapter->DATA_ACKEvent);
     
     }
    goto out;
  }
#endif
#endif
  /* finalize the message */
  rsi_genlmsg_end(skb, msg_head);
#if  KERNEL_VERSION_GREATER_THAN_EQUALS_2_6_(32) 
  /* send the message back */
  rc = rsi_genlmsg_unicast(net,skb,snd_pid );
#else
  /* send the message back */
  rc = rsi_genlmsg_unicast(skb,snd_pid);

#endif
  if (rc != 0)
  {
    retval = 4;
    goto out;
  }

  return 0;

out:
#ifdef RSI_UART_INTERFACE
#if RSI_TCP_IP_BYPASS
  if(data_ack)
  {
    kfree_skb(skb);
  }
#endif
#endif
 
#ifdef RSI_DEBUG_PRINT
   RSI_DEBUG(RSI_ZONE_SPI_DBG,"an error occured in rsi_send_rsp_to_userspace : %d, rc : %d\n",retval,rc);
#endif
  
  return -1;
}


/* commands: mapping between the command enumeration and the actual function*/
struct genl_ops rsi_user_gnl_ops = {
  .cmd = RSI_USER_C_CMD,
  .flags = 0,
  .policy = rsi_user_genl_policy,
  .doit = rsi_read_req_from_userspace,
  .dumpit = NULL,
};

/*==============================================*/
/**
 * @fn          INT32 rsi_register_genl(void)
 * @brief       Registers genl family and operations
 * @param[in]   none
 * @param[out]  none
 * @return      errCode
 *              0  = SUCCESS
 *              else FAIL
 * @section description
 * This API is used to register genl ops and family.
 */
INT32 rsi_register_genl(void)
{
  INT32 rc;

#ifdef RSI_DEBUG_PRINT
  RSI_DEBUG(RSI_ZONE_SPI_DBG,"\nrsi_register_genl:\n");
#endif
  /*register new family*/
#if LINUX_VERSION_CODE > KERNEL_VERSION(3, 12, 34)
  rsi_user_gnl_family.ops = &rsi_user_gnl_ops;
  rsi_user_gnl_family.n_ops = 1;
#endif
  rc = rsi_genl_register_family(&rsi_user_gnl_family);
  if (rc != 0)
    return rc;

  /*register functions (commands) of the new family*/
#if LINUX_VERSION_CODE <= KERNEL_VERSION(3, 12, 34)
 rc = rsi_genl_register_ops(&rsi_user_gnl_family, &rsi_user_gnl_ops);
  if (rc != 0)
  {
    RSI_DEBUG(RSI_ZONE_SPI_DBG,"register ops: %i\n",rc);
    rsi_genl_unregister_family(&rsi_user_gnl_family);
    return rc;
  }
#endif
  return rc;
}

/*==============================================*/
/**
 * @fn          INT32 rsi_unregister_genl(void)
 * @brief       Unregisters genl family and operations
 * @param[in]   none
 * @param[out]  none
 * @return      errCode
 *              0  = SUCCESS
 *              else FAIL
 * @section description
 * This API is used to unregister genl related ops.
 */
INT32 rsi_unregister_genl(void)
{
  INT32 ret;

#ifdef RSI_DEBUG_PRINT
  RSI_DEBUG(RSI_ZONE_SPI_DBG,"\nrsi_unregister_genl:\n");
#endif

#if LINUX_VERSION_CODE <= KERNEL_VERSION(3, 12, 34)
  /*unregister the functions*/
  ret = rsi_genl_unregister_ops(&rsi_user_gnl_family, &rsi_user_gnl_ops);
  if(ret != 0)
  {
    RSI_DEBUG(RSI_ZONE_SPI_DBG,"unregister ops: %i\n",ret);
    return ret;
  }
#endif

  /*unregister the family*/
  ret = rsi_genl_unregister_family(&rsi_user_gnl_family);
  if(ret !=0)
  {
    RSI_DEBUG(RSI_ZONE_SPI_DBG,"unregister family %i\n",ret);
    return ret;
  }

  return ret;
}

/* $EOF */
/* Log */
