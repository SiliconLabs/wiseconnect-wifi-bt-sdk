/*******************************************************************************
* @file  rsi_linux_data.c
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
 * @file          rsi_linux_data.c
 * @version       1.6
 * @date          2013-June-12
 *
 *
 *
 * @brief This contains all the functions related to data tranfer
 * over net device.
 *
 * @section Description
 * This file contains the following functions.
 *    rsi_xmit
 *    rsi_transmit_thread
 *    rsi_indicate_packet
 *
 */

#include "rsi_linux.h"
#include "rsi_nic.h"
#include "rsi_api.h"
#ifdef ENABLE_WMM_FEATURE
#include "rsi_core_wmm.h"
#endif
#include <linux/sched.h>

/*==============================================*/
/**
 * @fn          INT32 rsi_xmit (struct sk_buff *skb,
 *                           struct net_device *dev)
 * @brief       To transmit data
 * @param[in]   struct sk_buff *skb,
 *              Pointer to the socket buffer structure
 * @param[in]   struct net_device *dev,
 *              Pointer to our network device structure
 * @param[out]  none
 * @return      errCode
 *              -ve = FAIL
 *              0  = SUCCESS
 * @section description
 * When a packet is sent by the upper layers, the transmit entry
 * point registered at the time of initialization will be called.
 * This function will que the skb and set the event.
 */
#ifdef RSI_UART_INTERFACE
#if RSI_TCP_IP_BYPASS
volatile extern int ack_flag;
#endif
#endif
#ifdef ENABLE_WMM_FEATURE
void onebox_set_contention_vals(struct chanAccParams wme_wmeChanParams, PRSI_ADAPTER adapter);
uint8 core_determine_hal_queue(PRSI_ADAPTER adapter);
#endif

INT32 rsi_xmit (struct sk_buff *skb, struct net_device *dev)
{
  PRSI_ADAPTER Adapter = rsi_getpriv (dev);
#ifdef ENABLE_WMM_FEATURE
 	UINT16 pkt_priority = BE_Q_STA;
	UINT8  tos;
#endif
  UINT32 qId;

#ifdef RSI_UART_INTERFACE
#if RSI_TCP_IP_BYPASS
  uint8 temp_ack = 0;
#endif
#endif
  struct ethhdr *ehdr = (struct ethhdr *)skb->data;
#ifdef RSI_DEBUG_PRINT
  RSI_DEBUG(RSI_ZONE_SPI_DBG,"\nrsi_xmit:\n");
#endif
  RSI_DEBUG (RSI_ZONE_DATA_SEND, "+rsi_xmit\n");

  /*Checking if data pkt is rcvd in open state or not */
  if (Adapter->FSM_STATE != FSM_OPEN)
  {
    RSI_DEBUG (RSI_ZONE_ERROR, "rsi_xmit: Not in open state ");
    goto fail;
  }

  /* Checking the size of the packet */
  if (skb->len > 1528)
  {
    RSI_DEBUG (RSI_ZONE_ERROR, "rsi_xmit: Big packet ");
    goto fail;
  }

  if(is_zero_ether_addr(ehdr->h_source))
  {
	     printk("Source address is Zero so dropping Pkt\n");
	     goto fail;
  }


#ifdef ENABLE_WMM_FEATURE

  if(!Adapter->dbg_test_values_loaded)
  {
    Adapter->dbg_test_values_loaded = 1;
    onebox_set_contention_vals(Adapter->wme_wmeChanParams,Adapter);
  }

  if (Adapter->total_pkts_qd > RSI_MAX_PKTS_QUEUED){
	  RSI_DEBUG (RSI_ZONE_DATA_SEND, "rsi_xmit: failed due to total pkt qd crossed RSI_MAX_PKTS_QUEUED\n");
	  //goto fail;
  }
  
  /* Incase of device encap pick the tos from the IP hdr */
  tos = skb->data[15] >> 5;
  Adapter->tid = tos; 
  Adapter->priority = TID_TO_WME_AC(tos);
if (Adapter->wmm_vi_acm)
  Adapter->priority = WME_AC_BE;
  switch (Adapter->priority) 
  {
    case WME_AC_VO:
      pkt_priority = VO_Q_STA;
      break;
    case WME_AC_VI:
      pkt_priority = VI_Q_STA;
      break;
    case WME_AC_BE:
      pkt_priority = BE_Q_STA;
      break;
    case WME_AC_BK:
      pkt_priority = BK_Q_STA;
      break;
    default:
      /* Should not come here */
      pkt_priority = BE_Q_STA;
  }
  qId = pkt_priority;
  RSI_DEBUG (RSI_ZONE_DATA_SEND, "rsi_xmit: qId of packet %d and tid %d\n", qId,Adapter->tid);

  rsi_lock_bh(&Adapter->lockqueue);
  Adapter->total_pkts_qd++;
  rsi_unlock_bh(&Adapter->lockqueue);

  /*Acquiring the lock on the skb queues */
  rsi_queue_tail (&Adapter->list[qId], skb);
  //rsi_queue_tail (&Adapter->list[0], skb);

  rsi_Set_Event(&Adapter->Event);
  if(rsi_queue_len(&Adapter->list[qId]) > (Adapter->max_q_len[qId]-NW_QUEUE_STOP_OFFSET) )
  {
	if(!__netif_subqueue_stopped(dev, qId))
	{
		RSI_DEBUG (RSI_ZONE_DATA_SEND, "stopped_tx_queue of qid:%d, tos:%d\n",qId,tos);
		netif_stop_subqueue(dev, qId);	
	}
		if (Adapter->wmm_vi_acm)
			netif_stop_subqueue(dev, VI_Q_STA);
  }
if (Adapter->wmmparamsfw[2].aifsn > Adapter->wmmparamsfw[1].aifsn){

  if(rsi_queue_len(&Adapter->list[WME_AC_BE]) > 5 )
	{
		netif_stop_subqueue(dev, WME_AC_VI);	
		netif_stop_subqueue(dev, WME_AC_VO);	
		netif_stop_subqueue(dev, WME_AC_BK);	
	}else if(rsi_queue_len(&Adapter->list[WME_AC_BK]) > 10 )
	{
		netif_stop_subqueue(dev, WME_AC_VO);	
		netif_stop_subqueue(dev, WME_AC_VI);
	}else if(rsi_queue_len(&Adapter->list[WME_AC_VO]) > 10 )
		netif_stop_subqueue(dev, WME_AC_VI);

} else {
  if(rsi_queue_len(&Adapter->list[WME_AC_VO]) > 15 )
	{
		netif_stop_subqueue(dev, WME_AC_VI);	
		netif_stop_subqueue(dev, WME_AC_BE);	
		netif_stop_subqueue(dev, WME_AC_BK);	
	}else if(rsi_queue_len(&Adapter->list[WME_AC_VI]) > 30 )
	{
		netif_stop_subqueue(dev, WME_AC_BE);	
		netif_stop_subqueue(dev, WME_AC_BK);
	}else if(rsi_queue_len(&Adapter->list[WME_AC_BE]) > 60 )
		netif_stop_subqueue(dev, WME_AC_BK);
}
  RSI_DEBUG (RSI_ZONE_DATA_SEND, "rsi_xmit: Signalled\n");
  return NETDEV_TX_OK;
#else
  qId = DATA_QUEUE;

  if (Adapter->total_pkts_qd > RSI_MAX_PKTS_QUEUED)
  {
#ifdef RSI_UART_INTERFACE
#if RSI_TCP_IP_BYPASS
    rsi_lock_bh(&Adapter->uart_data_ack_flag_sem);
    temp_ack = ack_flag;
    rsi_unlock_bh(&Adapter->uart_data_ack_flag_sem);
    if(!temp_ack)
#endif
#endif
    {

      rsi_Set_Event(&Adapter->Event);
    }

    goto fail;
#if 0    
    if (!rsi_netif_queue_stopped (Adapter->net_device))
    {
      rsi_netif_stop_queue (Adapter->net_device);
    }
#endif    
  }

  rsi_lock_bh(&Adapter->lockqueue);
  Adapter->total_pkts_qd++;
  rsi_unlock_bh(&Adapter->lockqueue);

  /*Acquiring the lock on the skb queues */
  rsi_queue_tail (&Adapter->list[qId - 1], skb);
#ifdef RSI_UART_INTERFACE
#if RSI_TCP_IP_BYPASS
  rsi_lock_bh(&Adapter->uart_data_ack_flag_sem);
  temp_ack = ack_flag;
  rsi_unlock_bh(&Adapter->uart_data_ack_flag_sem);
  if(!temp_ack)
#endif
#endif
  {
    
    rsi_Set_Event(&Adapter->Event);
  }
  RSI_DEBUG (RSI_ZONE_DATA_SEND, "rsi_xmit: Signalled\n");
  return NETDEV_TX_OK;
#endif
fail:
  RSI_DEBUG (RSI_ZONE_DATA_SEND, "-Dropped\n");
  Adapter->stats.tx_dropped++;
  rsi_kfree_skb (skb);
  return 0;
}


/*==============================================*/
/**
 * @fn          VOID rsi_transmit_thread (PVOID pContext)
 * @brief       Tranmit thread to write data onto card
 * @param[in]   PVOID pContext
 * @param[out]  none
 * @return      none
 * @section description
 * This thread dequeues the skb from the list and adds the
 * descriptor. Finally, it writes onto the card.
 */
INT32 rsi_transmit_thread (PVOID pContext)
{
  PRSI_ADAPTER Adapter = (PRSI_ADAPTER) pContext;
  UINT32 Len = 0, i;
  UINT32 q_num, que_end;
  UINT32 que_start = 0;
  INT32 status;
  UINT8 cmd_qnum = SND_DATA_Q;
  UINT8 desc[RSI_DESC_LEN];
  UINT8           int_status = 0;
#ifdef RSI_UART_INTERFACE
  UINT8 data[1600];
#endif
  UINT32 num_pkts = 0;
#ifdef RSI_UART_INTERFACE
#if RSI_TCP_IP_BYPASS
  uint32 temp_ack = 0;
#endif
#endif

#ifdef RSI_DEBUG_PRINT
  RSI_DEBUG(RSI_ZONE_SPI_DBG,"\nrsi_transmit_thread:\n");
#endif
  RSI_DEBUG (RSI_ZONE_DATA_SEND,
             "rsi_transmit_thread: XMIT thread started\n");

  set_user_nice(current,-5);
  
  #if LINUX_VERSION_CODE <= KERNEL_VERSION(3, 6, 11)
  daemonize("XMIT-Thread");
  #endif

#ifdef ENABLE_WMM_FEATURE 
  do
  {
    UINT8 InterruptStatus = 0;
    UINT8 tid = 0;
    int status;
    struct sk_buff *rcvskb;
    struct sk_buff *rcvskb_free;


    if (Adapter->BufferFull)
    {
      status = rsi_Wait_Event (&Adapter->Event, 2);
    }
    else
    {
      status = rsi_Wait_Event (&Adapter->Event, EVENT_WAIT_FOREVER);
    }

    rsi_Reset_Event (&Adapter->Event); 
    while(1)
    {
      q_num = core_determine_hal_queue(Adapter);
      if (Adapter->FSM_STATE != FSM_OPEN)
      {
        RSI_DEBUG(RSI_ZONE_DATA_SEND,(TEXT("transmit_thread: FSM is not in OPEN State\n")));
        break;
      }

      if (q_num == INVALID_QUEUE) 
      {
        RSI_DEBUG(RSI_ZONE_DATA_SEND,TEXT("\nqos_pro: No More Pkt Due to invalid queueno\n"));
        break;
      }
#ifdef RSI_SDIO_INTERFACE
      status = sdio_read_buffer_full();
      if (status < 0 ) {
	      break;
      }
#endif

      rsi_lock_bh(&Adapter->lockqueue);
      Adapter->total_pkts_qd--;
      rsi_unlock_bh(&Adapter->lockqueue);

      rcvskb = rsi_dequeue (&Adapter->list[q_num]);

      RSI_DEBUG (RSI_ZONE_DATA_SEND, "Deque frm %d\n", q_num);
      if (rcvskb == NULL)
      {
        RSI_DEBUG (RSI_ZONE_ERROR,
            "rsi_transmit_thread: ERROR!!!! skb NULL %d\n",
            q_num);
        RSI_DEBUG(RSI_ZONE_DATA_SEND,(TEXT("No packet queued and trying to deque packet\n")));
        break;
      }
      
      tid = WME_AC_TO_TID(q_num);
      rsi_memset (desc, 0, 16);
#ifdef RSI_LITTLE_ENDIAN
      *(UINT16 *)&desc[0] = (((rcvskb->len) | (SND_DATA_Q << 12)) & 0x7fff);
#else
      desc[0] = (UINT8)((UINT16)((rcvskb->len ) & 0x00ff));
      desc[1] = (UINT8)((UINT16)((((rcvskb->len )| (SND_DATA_Q << 4)) >> 8) & 0x00ff));
#endif
      /*Keep Tid at 14th byte upper nibble*/
      desc[14] |= (((tid & 0xf) << 4) | (q_num & 0xf));
      Len = rcvskb->len;

      RSI_DEBUG (RSI_ZONE_DATA_SEND, "Queue %d and Tid of the queue %d\n", q_num,tid);
      //need to send the data from application
      Adapter->txPkt_count++;
      do
      {
        //need to send the data from application
        rsi_down_interruptible(&Adapter->int_check_sem);
        status = rsi_execute_cmd(desc, rcvskb->data, Len);

        rsi_up_sem(&Adapter->int_check_sem);
        if (status == -2)
        {
          RSI_DEBUG (RSI_ZONE_ERROR, "\nstatus : %d\n",status);
          RSI_DEBUG (RSI_ZONE_ERROR,
              "trying data %d bytes, Pkt_Count:%d\n",
              Len, Adapter->txPkt_count);

        }
#ifdef RSI_SPI_INTERFACE
        if (status == -1)
        {
          rsi_queue_head(&Adapter->list[q_num], rcvskb);
          rsi_lock_bh(&Adapter->lockqueue);
          Adapter->total_pkts_qd++;
          rsi_unlock_bh(&Adapter->lockqueue);
          break;
        }
#endif
        if (status == 0)
        {
          break;
        }
      }while(1);

      if(status == -1)
        break;
      if (status != RSI_STATUS_SUCCESS)
      {
#ifdef RSI_DEBUG_PRINT
        RSI_DEBUG (RSI_ZONE_ERROR,
            //"rsi_transmit_thread: Dropped Pkt of %d bytes\n",Len);
                  "desc Pkt Count: %d\n",
                  Adapter->txPkt_count);
#endif
        rsi_kfree_skb (rcvskb);
        Adapter->stats.tx_dropped++;
        break;
      }


      RSI_DEBUG (RSI_ZONE_DATA_SEND,
          "rsi_transmit_thread: Pkt of %d bytes written on card\n",
          Len);
      rsi_kfree_skb (rcvskb);
      Adapter->stats.tx_packets++;
      Adapter->stats.tx_bytes += Len;

      if(Adapter->transmit_thread_exit)
      {
        rsi_lock_bh(&Adapter->lockqueue);
        while(Adapter->total_pkts_qd > 0)
        {
          rcvskb_free= rsi_dequeue (&Adapter->list[q_num]);
          Adapter->total_pkts_qd--;
          rsi_kfree_skb (rcvskb_free);
        }
        rsi_unlock_bh(&Adapter->lockqueue);
        break;
      }
      if(__netif_subqueue_stopped((void *)Adapter->net_device0, WME_AC_VO) && (rsi_queue_len(&Adapter->list[WME_AC_VO]) < 100))
	      netif_wake_subqueue((void *)Adapter->net_device0, WME_AC_VO);	
      if(rsi_queue_len(&Adapter->list[WME_AC_VO]) ==0 )
      {
	     // if(__netif_subqueue_stopped((void *)Adapter->net_device0, WME_AC_VI) && (rsi_queue_len(&Adapter->list[WME_AC_VI]) < 150))
	      if(__netif_subqueue_stopped((void *)Adapter->net_device0, WME_AC_VI) && ((!Adapter->wmm_vi_acm && (rsi_queue_len(&Adapter->list[WME_AC_VI]) < 150)) || (Adapter->wmm_vi_acm && (rsi_queue_len(&Adapter->list[WME_AC_BE]) < 25))))
		      netif_wake_subqueue((void *)Adapter->net_device0, WME_AC_VI);	
	      if(rsi_queue_len(&Adapter->list[WME_AC_VI]) < 5 )
	      {
		      if(__netif_subqueue_stopped((void *)Adapter->net_device0, WME_AC_BE) && (rsi_queue_len(&Adapter->list[WME_AC_BE]) < 25))
			      netif_wake_subqueue((void *)Adapter->net_device0, WME_AC_BE);

		      if(rsi_queue_len(&Adapter->list[WME_AC_BE]) < 15 )
		      {
			      if(__netif_subqueue_stopped((void *)Adapter->net_device0, WME_AC_BK) && (rsi_queue_len(&Adapter->list[WME_AC_BK]) < 10))
				      netif_wake_subqueue((void *)Adapter->net_device0, WME_AC_BK);
		      }
	      }
      }

    }
  } while (Adapter->transmit_thread_exit == 0);
#else

  do
  {
    UINT8 InterruptStatus = 0;
    struct sk_buff *rcvskb;
    struct sk_buff *rcvskb_free;
#ifndef RSI_UART_INTERFACE
    if (Adapter->BufferFull)
    {
      status = rsi_Wait_Event (&Adapter->Event, 10);
    }
    else
    {
      status = rsi_Wait_Event (&Adapter->Event, EVENT_WAIT_FOREVER);
    }
#endif
#ifdef RSI_UART_INTERFACE
#if RSI_TCP_IP_BYPASS

    rsi_lock_bh(&Adapter->lockqueue);
    temp_ack = Adapter->total_pkts_qd;
    rsi_unlock_bh(&Adapter->lockqueue);
    if(!temp_ack)
    {
    
      status = rsi_Wait_Event (&Adapter->Event, EVENT_WAIT_FOREVER);
    }
#else 
if (Adapter->BufferFull)
    {
      status = rsi_Wait_Event (&Adapter->Event, 10);
    }
    else
    {
      status = rsi_Wait_Event (&Adapter->Event, EVENT_WAIT_FOREVER);
    }
#endif
#endif

    rsi_Reset_Event (&Adapter->Event); 

    RSI_DEBUG (RSI_ZONE_DATA_SEND, "rsi_transmit_thread:event released\n");
	que_end = 1;
      if (Adapter->transmit_thread_exit )
    {
      RSI_DEBUG (RSI_ZONE_DATA_SEND,
                 "rsi_transmit_thread: Halt flag set\n");
      break;
    }
  que_loop:
    for (q_num = que_start; q_num < que_end; ++q_num)
    {
      UINT32 pkts_given = 0;
      RSI_DEBUG (RSI_ZONE_DATA_SEND, "rsi_transmit_thread: Q_NUM:%d\n",
                 q_num);

      if (Adapter->FSM_STATE != FSM_OPEN)
      {
        RSI_DEBUG(RSI_ZONE_DATA_SEND,(TEXT("ganges_transmit_thread: FSM is not in OPEN State\n")));
        break;
      }

      do
      {

        ++pkts_given;

        rsi_lock_bh(&Adapter->lockqueue);

        if (Adapter->total_pkts_qd > 0)
        {
          Adapter->total_pkts_qd--;
        }
        else 
        {
          rsi_unlock_bh(&Adapter->lockqueue);
          break;
        }
        rsi_unlock_bh(&Adapter->lockqueue);
        rcvskb = rsi_dequeue (&Adapter->list[q_num]);

        RSI_DEBUG (RSI_ZONE_DATA_SEND, "Deque frm%d\n", q_num);
        if (rcvskb == NULL)
        {
          RSI_DEBUG (RSI_ZONE_ERROR,
                     "rsi_transmit_thread: ERROR!!!! skb NULL %d\n",
                     q_num);
          break;
        }

        rsi_memset (desc, 0, 16);
        rsi_uint16_to_2bytes(desc, (((rcvskb->len) & 0xFFF) | (SND_DATA_Q << 12)));
        Len = rcvskb->len;
#ifdef RSI_DUAL_MODE
       if((desc[1] >> RSI_MGMT_PKT_TYPE) == RSI_DATA_PKT_TYPE)
       {
          desc[2] = RSI_FRAME_TYPE; //Here frame type 1 is assigned to desc[2] for bypass case
       }
#endif
        //need to send the data from application
        Adapter->txPkt_count++;
        do
        {
          //need to send the data from application
          rsi_down_interruptible(&Adapter->int_check_sem);
#ifndef RSI_UART_INTERFACE
          status = rsi_execute_cmd(desc, rcvskb->data, Len);
#else
          memcpy(data, desc, 16);
          memcpy(&data[16], rcvskb->data, Len);
          status = rsi_send_rsp_to_userspace(Adapter, data, TO_UART_APP);
#endif
          rsi_up_sem(&Adapter->int_check_sem);

#ifdef RSI_UART_INTERFACE
#if RSI_TCP_IP_BYPASS
          rsi_Wait_Event(&Adapter->DATA_ACKEvent, EVENT_WAIT_FOREVER);
          if(Adapter->data_ack_missed)
          {
            Adapter->data_ack_missed = 0; 
            rsi_lock_bh(&Adapter->lockqueue);
            while(Adapter->total_pkts_qd > 0)
            {
              rcvskb_free= rsi_dequeue (&Adapter->list[q_num]);
              Adapter->total_pkts_qd--;
              rsi_kfree_skb (rcvskb_free);
            }
            rsi_unlock_bh(&Adapter->lockqueue);
            rsi_Reset_Event (&Adapter->DATA_ACKEvent); 
            break;
          }
          if(Adapter->transmit_thread_exit)
            break;
          rsi_lock_bh(&Adapter->uart_data_ack_flag_sem);
          temp_ack = ack_flag;
          rsi_unlock_bh(&Adapter->uart_data_ack_flag_sem);
          if(temp_ack == 2)
          {
            rsi_lock_bh(&Adapter->uart_data_ack_flag_sem);
            ack_flag = 1;
            rsi_unlock_bh(&Adapter->uart_data_ack_flag_sem);
            continue;
          }
          rsi_Reset_Event (&Adapter->DATA_ACKEvent); 
#endif
#endif
          if (status == -2)
          {
            RSI_DEBUG (RSI_ZONE_ERROR, "\nstatus : %d\n",status);
            RSI_DEBUG (RSI_ZONE_ERROR,
                "trying data %d bytes, Pkt_Count:%d\n",
                Len, Adapter->txPkt_count);

          }
#ifdef RSI_SPI_INTERFACE
          if (status == -1)
          {
            rsi_queue_head(&Adapter->list[q_num], rcvskb);
            rsi_lock_bh(&Adapter->lockqueue);
            Adapter->total_pkts_qd++;
            rsi_unlock_bh(&Adapter->lockqueue);
            break;
          }
#endif
          if (status == 0)
          {
            break;
          }
        } while (1); 
        if(status == -1)
          break;
        if (status != RSI_STATUS_SUCCESS)
        {
#ifdef RSI_DEBUG_PRINT
          RSI_DEBUG (RSI_ZONE_ERROR,
              //"rsi_transmit_thread: Dropped Pkt of %d bytes\n",Len);
                    "desc Pkt Count: %d\n",
                    Adapter->txPkt_count);
#endif
          rsi_kfree_skb (rcvskb);
          Adapter->stats.tx_dropped++;
          break;
        }


        RSI_DEBUG (RSI_ZONE_DATA_SEND,
            "rsi_transmit_thread: Pkt of %d bytes written on card\n",
            Len);
        rsi_kfree_skb (rcvskb);
        Adapter->stats.tx_packets++;
        Adapter->stats.tx_bytes += Len;
#ifdef RSI_UART_INTERFACE
#if RSI_TCP_IP_BYPASS
        break;
#endif
#endif
        if(Adapter->transmit_thread_exit)
        {
          rsi_lock_bh(&Adapter->lockqueue);
          while(Adapter->total_pkts_qd > 0)
          {
            rcvskb_free= rsi_dequeue (&Adapter->list[q_num]);
            Adapter->total_pkts_qd--;
            rsi_kfree_skb (rcvskb_free);
          }
          rsi_unlock_bh(&Adapter->lockqueue);
          break;
        }
#if 0        
        if (Adapter->total_pkts_qd < RSI_MAX_PKTS_QUEUED)
        {
          if (rsi_netif_queue_stopped (Adapter->net_device) )
          {
            rsi_netif_start_queue(Adapter->net_device);
          }
        }
#endif        
      } while (Adapter->total_pkts_qd > 0);
    }

#if 0
    for (i = que_start; i < que_end; i++)
    {
      if (rsi_queue_len (&Adapter->list[i]) /* && (!Adapter->sleep_indcn) */ )
      {
        RSI_DEBUG (RSI_ZONE_DATA_SEND,
            "BFl: %d\nQue0:%d\nQue1:%d\nQue2:%d\nQue3:%d\n",
            Adapter->BufferFull,
            rsi_queue_len (&Adapter->list[0]),
            rsi_queue_len (&Adapter->list[1]),
            rsi_queue_len (&Adapter->list[2]),
            rsi_queue_len (&Adapter->list[3]));
        q_num = i;
        goto que_loop;
      }
    }
#endif
  } while (Adapter->transmit_thread_exit == 0);

#endif

  complete_and_exit(&Adapter->txThreadComplete, 0);

}

/*==============================================*/
/**
 * @fn          INT32 rsi_indicate_packet(PRSI_ADAPTER Adapter,
 *                                     UINT8 * DataRcvPacket,
 *                                     UINT32 pktLen)
 * @brief       Indicated the packet to upper layer
 * @param[in]   PRSI_ADAPTER Adapter,
 *              Pointer to the private data of the device
 * @param[in]   UINT8 *DataRcvPacket, Pointer to a packet
 * @param[in]   UINT32 pktLen, Length of the packet
 * @param[out]  none
 * @return      On success 0, else a negative number
 * @section description
 * This function copies the packet into a skb structure and
 * indicates to the upper layer
 */
INT32
rsi_indicate_packet
(PRSI_ADAPTER Adapter, UINT8 * DataRcvPacket, UINT32 pktLen, UINT8 sta_id)
{
  struct sk_buff *rxskb;
  INT32 i = 0;

#ifdef RSI_DEBUG_PRINT
  RSI_DEBUG(RSI_ZONE_SPI_DBG,"\nrsi_indicate_packet:\n");
#endif
#if RSI_CONCURRENT_MODE
  if ((sta_id == 0) && (Adapter->FSM_STATE != FSM_OPEN))
  {
    RSI_DEBUG (RSI_ZONE_ERROR, "rsi_indicate_packet: Station is not in open state\n");
    goto fail;
  }
  else if ((sta_id != 0) && (Adapter->FSM_STATE_AP1 != FSM_OPEN))
  {
    RSI_DEBUG (RSI_ZONE_ERROR, "rsi_indicate_packet: AP1 is not in open state\n");
    goto fail;
  }

#else
  if (Adapter->FSM_STATE != FSM_OPEN)
  {
    RSI_DEBUG (RSI_ZONE_ERROR, "rsi_indicate_packet: Not in open state\n");
    goto fail;
  }
#endif

  if (pktLen > 1536)
  {
    RSI_DEBUG (RSI_ZONE_ERROR, "rsi_indicate_packet: Big packet revd\n");
    goto fail;
  }

  if (pktLen < RSI_HEADER_SIZE)
  {
    RSI_DEBUG (RSI_ZONE_ERROR, "rsi_indicate_packet: Runt packet recvd\n");
    goto fail;
  }
  /*Allocate skb and copy the data into it */
  rxskb = rsi_alloc_skb (pktLen);
  if (!rxskb)
  {
    RSI_DEBUG (RSI_ZONE_ERROR,
               "rsi_indicate_packet: Low on memory, packet dropped\n");
    goto fail;
  }

  rsi_memcpy (rsi_skb_put (rxskb, pktLen), DataRcvPacket, pktLen);
  /*Filling the various other fields of skb */
#if RSI_CONCURRENT_MODE
  if(sta_id == 0)
  {
   rxskb->dev = Adapter->net_device0;
   rxskb->protocol = rsi_eth_type_trans (rxskb, Adapter->net_device0);
  }
  else
  {
    //! These station belongs to VAP 1 (which is for AP1)
   rxskb->dev = Adapter->net_device1;
   rxskb->protocol = rsi_eth_type_trans (rxskb, Adapter->net_device1);
  }
#else
   rxskb->dev = Adapter->net_device0;
   rxskb->protocol = rsi_eth_type_trans (rxskb, Adapter->net_device0);
#endif

  rxskb->ip_summed = CHECKSUM_NONE;

  rsi_netif_rx (rxskb);

  RSI_DEBUG (RSI_ZONE_DATA_RCV,
             "rsi_indicate_packet: Pkt of %d bytes indicated\n", pktLen);

  Adapter->stats.rx_packets++;
  Adapter->stats.rx_bytes += pktLen;
  return 0;

fail:

  Adapter->stats.rx_dropped++;
  return -1;
}

/* $EOF */
/* Log */
