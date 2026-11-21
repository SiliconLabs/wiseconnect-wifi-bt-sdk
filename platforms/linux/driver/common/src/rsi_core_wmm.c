
/*******************************************************************************
* @file  rsi_core_wmm.c
* @brief Contains WLAN Queue handling functions 
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
 * @file rsi_core_wmm.c
 * @author 
 * @version 1.0
 *
 */

#ifdef ENABLE_WMM_FEATURE
#include "rsi_core_wmm.h"
#include "rsi_linux.h"
#include "rsi_nic.h"

/**
 * This function gets the contention values for the backoff procedure.
 *
 * @param Pointer to the channel acces params sturcture
 * @param Pointer to the driver's private structure.
 * @return none.
 */ 
void onebox_set_contention_vals(struct chanAccParams wme_wmeChanParams, PRSI_ADAPTER adapter)
{
  /*Fill the structure through network params: just after connection*/
	struct chanAccParams *wme_params = &wme_wmeChanParams;

	adapter->wme_org_q[VO_Q_STA] = (((wme_params->cap_wmeParams[WME_AC_VO].wmep_logcwmin / 2 ) + 
	                            (wme_params->cap_wmeParams[WME_AC_VO].wmep_aifsn)) * WMM_SHORT_SLOT_TIME + SIFS_DURATION);
	adapter->wme_org_q[VI_Q_STA] = (((wme_params->cap_wmeParams[WME_AC_VI].wmep_logcwmin / 2 ) + 
	                            (wme_params->cap_wmeParams[WME_AC_VI].wmep_aifsn)) * WMM_SHORT_SLOT_TIME + SIFS_DURATION);
	adapter->wme_org_q[BE_Q_STA] = (((wme_params->cap_wmeParams[WME_AC_BE].wmep_logcwmin / 2 ) + 
	                            (wme_params->cap_wmeParams[WME_AC_BE].wmep_aifsn-1)) * WMM_SHORT_SLOT_TIME + SIFS_DURATION);
	adapter->wme_org_q[BK_Q_STA] = (((wme_params->cap_wmeParams[WME_AC_BK].wmep_logcwmin / 2 ) + 
	                            (wme_params->cap_wmeParams[WME_AC_BK].wmep_aifsn)) * WMM_SHORT_SLOT_TIME + SIFS_DURATION);

	printk("In %s Line %d QUEUE WT are \n", __func__, __LINE__);
	printk("BE_Q %d \n",adapter->wme_org_q[BE_Q_STA] );
	printk("BK_Q %d \n",adapter->wme_org_q[BK_Q_STA] );
	printk("VI_Q %d \n",adapter->wme_org_q[VI_Q_STA] );
	printk("VO_Q %d \n",adapter->wme_org_q[VO_Q_STA] );

	rsi_memcpy(adapter->per_q_wt, adapter->wme_org_q, sizeof(adapter->per_q_wt));
	rsi_memset(adapter->pkt_contended, 0, sizeof(adapter->pkt_contended));
}

/**
 * This function determines the HAL queue from which packets has to be dequeued while transmission.
 *
 * @param Pointer to the driver's private structure .
 * @return ONEBOX_STATUS_SUCCESS on success else ONEBOX_STATUS_FAILURE.
 */
UINT8 core_determine_hal_queue(PRSI_ADAPTER adapter)
{
  UINT8 q_num = INVALID_QUEUE;
  UINT8 ii,min = 0;
  UINT8 fresh_contention;
  struct chanAccParams *wme_params_sta;

  wme_params_sta = &adapter->wme_wmeChanParams;

  {
    RSI_DEBUG(RSI_ZONE_DATA_SEND, TEXT("per q wt values in %d:  %d %d %d %d \n"), 
        __LINE__, adapter->per_q_wt[0], adapter->per_q_wt[1],
        adapter->per_q_wt[2], adapter->per_q_wt[3]);
    RSI_DEBUG(RSI_ZONE_DATA_SEND, TEXT("selected queue num and pkt cnt are : %d %d %d \n"),
        __LINE__, adapter->selected_qnum, adapter->pkt_cnt);
    if (adapter->pkt_cnt != 0)
    {
      adapter->pkt_cnt -= 1;
      return (adapter->selected_qnum);
    }

GET_QUEUE_NUM:
    q_num = 0;
    fresh_contention = 0;

    /* Selecting first valid contention value */
    for(ii = 0; ii < NUM_EDCA_QUEUES ; ii++)
    {
      if(adapter->pkt_contended[ii] && 
          ((rsi_queue_len(&adapter->list[ii])) != 0)) /* Check for contended value*/
      {
        min = adapter->per_q_wt[ii];
        q_num = ii;
        break;
      }
    }

    /* Selecting the queue with least back off */
    for(; ii < NUM_EDCA_QUEUES ; ii++) /* Start finding the least value from first valid value itself
                                        * Leave the value of ii as is  from previous loop */
    {
      if(adapter->pkt_contended[ii] && (adapter->per_q_wt[ii] < min) 
          && ((rsi_queue_len(&adapter->list[ii]) !=0))) /* Check only if contended */
      {
        min = adapter->per_q_wt[ii];
        q_num = ii;
      }
    }
    RSI_DEBUG(RSI_ZONE_DATA_SEND, TEXT("min =%d and qnum=%d\n"), min, q_num);

    /* Adjust the back off values for all queues again */
    adapter->pkt_contended[q_num] = 0; /* Reset the contention for the current queue so that it gets org value again if it has more packets */

    for(ii = 0; ii< NUM_EDCA_QUEUES; ii++)
    {
      if(rsi_queue_len(&adapter->list[ii]))/* Check for the need of contention */
      { 
        RSI_DEBUG(RSI_ZONE_DATA_SEND, TEXT("queue %d len %d\n"), 
            ii, rsi_queue_len(&adapter->list[ii]));
        if(adapter->pkt_contended[ii])
        {
          if(adapter->per_q_wt[ii] > min)
          { /* Subtracting the backoff value if already contended */
            adapter->per_q_wt[ii] -= min;
          }
          else /* This case occurs only two queues end up in having same back off value and is least */
          {
            adapter->per_q_wt[ii] = 0;
          }
        }
        else /* Fresh contention */
        {
          adapter->pkt_contended[ii] = 1;
          adapter->per_q_wt[ii] = adapter->wme_org_q[ii];
          fresh_contention = 1;
        }
      }
      else
      { /* No packets so no contention */
        adapter->per_q_wt[ii] = 0;
        adapter->pkt_contended[ii] = 0;
      }
    }
    RSI_DEBUG(RSI_ZONE_DATA_SEND, TEXT("per q values in %d:  %d %d %d %d \n"),
        __LINE__, adapter->per_q_wt[0], adapter->per_q_wt[1], adapter->per_q_wt[2], adapter->per_q_wt[3]);
    RSI_DEBUG(RSI_ZONE_DATA_SEND, TEXT("pkt contended val in %d:  %d %d %d %d \n\n"),
        __LINE__, adapter->pkt_contended[0], adapter->pkt_contended[1], 
        adapter->pkt_contended[2], adapter->pkt_contended[3]);
    if((rsi_queue_len(&adapter->list[q_num])) == 0)
    {
      /* If any queues are freshly contended and the selected queue doesn't have any packets
       * then get the queue number again with fresh values */
      if(fresh_contention)
      {
        goto GET_QUEUE_NUM;
      }
      q_num = INVALID_QUEUE;
      return q_num;
    }

    adapter->selected_qnum = q_num ;
    RSI_DEBUG(RSI_ZONE_DATA_SEND, TEXT("WMM::queue num after algo= %d \n"), q_num);
    RSI_DEBUG(RSI_ZONE_DATA_SEND, TEXT("queue %d len %d \n"), q_num, 
        rsi_queue_len(&adapter->list[q_num]));

    if ((adapter->selected_qnum == VO_Q_STA) || (adapter->selected_qnum == VI_Q_STA))
    {
      if(adapter->selected_qnum == VO_Q_STA) {
        if(wme_params_sta->cap_wmeParams[adapter->selected_qnum].wmep_acm)
          adapter->pkt_cnt = 1;
        else
          adapter->pkt_cnt = 6;
        //adapter->pkt_cnt = ((wme_params_sta->cap_wmeParams[adapter->selected_qnum].wmep_txopLimit << 5) / 150);
      }	else { 
        if(wme_params_sta->cap_wmeParams[adapter->selected_qnum].wmep_acm) {
          adapter->pkt_cnt = 1;
        } else {
          adapter->pkt_cnt = ((wme_params_sta->cap_wmeParams[adapter->selected_qnum].wmep_txopLimit << 5) / 800);
         if (adapter->wmmparamsfw[2].aifsn > adapter->wmmparamsfw[1].aifsn)
		adapter->pkt_cnt = 2;
        }
      }
      if((rsi_queue_len(&adapter->list[q_num])) <= adapter->pkt_cnt)
      {
        adapter->pkt_cnt = ((rsi_queue_len(&adapter->list[q_num])));
      }
      if(adapter->pkt_cnt != 0)
      {
        adapter->pkt_cnt -= 1;
      }
      else
      {
        adapter->pkt_cnt = 0;
      }
    }
    RSI_DEBUG(RSI_ZONE_DATA_SEND, TEXT("pkt_cnt and q_num are: %d %d \n"), adapter->pkt_cnt, q_num);
    return (q_num);
  }
  return ONEBOX_STATUS_FAILURE;
}

#if 1
void dbg_test_values(PRSI_ADAPTER adapter)
{
  RSI_DEBUG(RSI_ZONE_INIT,"INITIALIZING... WMM PARAMS FOR TEST \n");
  struct chanAccParams *wme_params_sta;
  wme_params_sta = &adapter->wme_wmeChanParams;
  wme_params_sta->cap_info = 0x1;
  /*BE*/
  wme_params_sta->cap_wmeParams[WME_AC_BE].wmep_acm = 0x0;
  wme_params_sta->cap_wmeParams[WME_AC_BE].wmep_aifsn = 0x3;
  wme_params_sta->cap_wmeParams[WME_AC_BE].wmep_logcwmax = 10;
  wme_params_sta->cap_wmeParams[WME_AC_BE].wmep_logcwmin = 4;
  wme_params_sta->cap_wmeParams[WME_AC_BE].wmep_txopLimit = 0;
  wme_params_sta->cap_wmeParams[WME_AC_BE].wmep_noackPolicy = 0;

  /*BK*/
  wme_params_sta->cap_wmeParams[WME_AC_BK].wmep_acm = 0x0;
  wme_params_sta->cap_wmeParams[WME_AC_BK].wmep_aifsn = 0x7;
  wme_params_sta->cap_wmeParams[WME_AC_BK].wmep_logcwmax = 10;
  wme_params_sta->cap_wmeParams[WME_AC_BK].wmep_logcwmin = 4;
  wme_params_sta->cap_wmeParams[WME_AC_BK].wmep_txopLimit = 0;
  wme_params_sta->cap_wmeParams[WME_AC_BK].wmep_noackPolicy = 0;

  /*VI*/
  wme_params_sta->cap_wmeParams[WME_AC_VI].wmep_acm = 0x0;
  wme_params_sta->cap_wmeParams[WME_AC_VI].wmep_aifsn = 0x2;
  wme_params_sta->cap_wmeParams[WME_AC_VI].wmep_logcwmax = 4;
  wme_params_sta->cap_wmeParams[WME_AC_VI].wmep_logcwmin = 3;
  wme_params_sta->cap_wmeParams[WME_AC_VI].wmep_txopLimit = 94;
  wme_params_sta->cap_wmeParams[WME_AC_VI].wmep_noackPolicy = 0;

  /*VO*/
  wme_params_sta->cap_wmeParams[WME_AC_VO].wmep_acm = 0x0;
  wme_params_sta->cap_wmeParams[WME_AC_VO].wmep_aifsn = 0x2;
  wme_params_sta->cap_wmeParams[WME_AC_VO].wmep_logcwmax = 3;
  wme_params_sta->cap_wmeParams[WME_AC_VO].wmep_logcwmin = 2;
  wme_params_sta->cap_wmeParams[WME_AC_VO].wmep_txopLimit = 47;
  wme_params_sta->cap_wmeParams[WME_AC_VO].wmep_noackPolicy = 0;
}

void update_wmm_params(PRSI_ADAPTER adapter)
{
  //RSI_DEBUG(RSI_ZONE_INIT,"UPDATING... WMM PARAMS FOR TEST \n");
  struct chanAccParams *wme_params_sta;
  wme_params_sta = &adapter->wme_wmeChanParams; 
  wme_params_sta->cap_info = 0x1;

  /*BE*/
  wme_params_sta->cap_wmeParams[WME_AC_BE].wmep_acm = 0x0;
  wme_params_sta->cap_wmeParams[WME_AC_BE].wmep_aifsn = adapter->wmmparamsfw[1].aifsn;
  wme_params_sta->cap_wmeParams[WME_AC_BE].wmep_logcwmax = adapter->wmmparamsfw[1].cw_max;
  wme_params_sta->cap_wmeParams[WME_AC_BE].wmep_logcwmin = adapter->wmmparamsfw[1].cw_min;
  wme_params_sta->cap_wmeParams[WME_AC_BE].wmep_txopLimit = adapter->wmmparamsfw[1].tx_op;
  wme_params_sta->cap_wmeParams[WME_AC_BE].wmep_noackPolicy = 0;

  /*BK*/
  wme_params_sta->cap_wmeParams[WME_AC_BK].wmep_acm = 0x0;
  wme_params_sta->cap_wmeParams[WME_AC_BK].wmep_aifsn = adapter->wmmparamsfw[0].aifsn;
  wme_params_sta->cap_wmeParams[WME_AC_BK].wmep_logcwmax = adapter->wmmparamsfw[0].cw_max;
  wme_params_sta->cap_wmeParams[WME_AC_BK].wmep_logcwmin = adapter->wmmparamsfw[0].cw_min;
  wme_params_sta->cap_wmeParams[WME_AC_BK].wmep_txopLimit = adapter->wmmparamsfw[0].tx_op;
  wme_params_sta->cap_wmeParams[WME_AC_BK].wmep_noackPolicy = 0;

  /*VI*/
  wme_params_sta->cap_wmeParams[WME_AC_VI].wmep_acm = 0x0;
  wme_params_sta->cap_wmeParams[WME_AC_VI].wmep_aifsn = adapter->wmmparamsfw[2].aifsn;
  wme_params_sta->cap_wmeParams[WME_AC_VI].wmep_logcwmax = adapter->wmmparamsfw[2].cw_max;
  wme_params_sta->cap_wmeParams[WME_AC_VI].wmep_logcwmin = adapter->wmmparamsfw[2].cw_min;
  wme_params_sta->cap_wmeParams[WME_AC_VI].wmep_txopLimit = adapter->wmmparamsfw[2].tx_op;
  wme_params_sta->cap_wmeParams[WME_AC_VI].wmep_noackPolicy = 0;

  /*VO*/
  wme_params_sta->cap_wmeParams[WME_AC_VO].wmep_acm = 0x0;
  wme_params_sta->cap_wmeParams[WME_AC_VO].wmep_aifsn = adapter->wmmparamsfw[3].aifsn;
  wme_params_sta->cap_wmeParams[WME_AC_VO].wmep_logcwmax = adapter->wmmparamsfw[3].cw_max;
  wme_params_sta->cap_wmeParams[WME_AC_VO].wmep_logcwmin = adapter->wmmparamsfw[3].cw_min;
  wme_params_sta->cap_wmeParams[WME_AC_VO].wmep_txopLimit = adapter->wmmparamsfw[3].tx_op;
  wme_params_sta->cap_wmeParams[WME_AC_VO].wmep_noackPolicy = 0;

  onebox_set_contention_vals(adapter->wme_wmeChanParams, adapter);
}
#endif
#endif
