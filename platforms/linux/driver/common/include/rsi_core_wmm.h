/*******************************************************************************
* @file  rsi_core_wmm.h
* @brief Contains WLAN WMM queue handling defines
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

#ifdef ENABLE_WMM_FEATURE
#ifndef __RSI_CORE_WMM_
#define __RSI_CORE_WMM_

#include "rsi_linux_common.h"
/*
 * WME/WMM support.
 */
/*General defines*/
#define ONEBOX_STATUS int
#define ONEBOX_STATUS_FAILURE -1
#define ONEBOX_STATUS_SUCCESS 0

#define INVALID_QUEUE              0xff

/*WMM defines*/
#define NUM_EDCA_QUEUES  4

#define WME_NUM_AC 4    /* 4 AC categories */
/* WME stream classes */
#define	WME_AC_BE	1		/* best effort */
#define	WME_AC_BK	0		/* background */
#define	WME_AC_VI	2		/* video */
#define	WME_AC_VO	3		/* voice */

#define QUEUE_DEPTH_BK 20
#define QUEUE_DEPTH_BE 140//40
#define QUEUE_DEPTH_VI 300//100
#define QUEUE_DEPTH_VO 600//300

#define NW_QUEUE_START_OFFSET 10	
#define NW_QUEUE_STOP_OFFSET	3

#define WMM_SHORT_SLOT_TIME 9
#define SIFS_DURATION   16

#define	IEEE80211_TXOP_TO_US(_txop)	((_txop)<<5)
#define	IEEE80211_US_TO_TXOP(_us)	((_us)>>5)

#define WME_AC_TO_TID(_ac) (       \
	((_ac) == WME_AC_VO) ? 6 : \
	((_ac) == WME_AC_VI) ? 5 : \
	((_ac) == WME_AC_BK) ? 1 : \
	0)

#define TID_TO_WME_AC(_tid) (      \
	((_tid) == 0 || (_tid) == 3) ? WME_AC_BE : \
	((_tid) < 3) ? WME_AC_BK : \
	((_tid) < 6) ? WME_AC_VI : \
	WME_AC_VO)

/*WMM enums*/
enum EDCA_QUEUE
{
	BK_Q_STA = 0,
	BE_Q_STA,
	VI_Q_STA,
	VO_Q_STA,
	BK_Q_AP,
	BE_Q_AP,
	VI_Q_AP,
	VO_Q_AP,
};

/*WMM Structures*/
struct wmeParams {
	UINT8		wmep_acm;
	UINT8		wmep_aifsn;
	UINT8		wmep_logcwmin;		/* log2(cwmin) */
	UINT8		wmep_logcwmax;		/* log2(cwmax) */
	UINT8		wmep_txopLimit;
	UINT8		wmep_noackPolicy;	/* 0 (ack), 1 (no ack) */
};

struct wmm_ac_beacon {
	UINT8 aifsn : 4;
	UINT8 acm : 1;
	UINT8 aci : 2;
	UINT8 res : 1;
	UINT8 cw_min : 4;
	UINT8 cw_max : 4;
	UINT16 tx_op;
};
struct wmm_params_fw {
	UINT16 aifsn;
	UINT16 cw_min;
	UINT16 cw_max;
	UINT16 tx_op;
};

struct chanAccParams {
	UINT8		cap_info;		/* version of the current set */
	struct wmeParams cap_wmeParams[WME_NUM_AC];
};

#endif
#endif
