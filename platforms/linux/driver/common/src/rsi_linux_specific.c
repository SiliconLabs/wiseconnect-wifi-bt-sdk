/*******************************************************************************
* @file  rsi_linux_specific.c
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
 * @file      rsi_linux_specific.c
 * @version   1.6
 * @date      2013-June-12
 *
 *
 *
 * @brief This contains all the linux specific code
 *
 * @section Description
 * This file contains the following functions.
 *    rsi_alloc_skb
 *    rsi_dump
 *    rsi_Init_Event
 *    rsi_Delete_Event
 *    rsi_Wait_Event
 *    rsi_Set_Event
 *    rsi_Reset_Event
 *    rsi_Kill_Thread
 *    rsi_Init_Thread
 *
 */


#include <net/genetlink.h>
#include <linux/moduleparam.h>
#include <linux/proc_fs.h>

#include "rsi_linux.h"
#include "rsi_nic.h"
#include <linux/sched.h>
#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,39)
#include <linux/kthread.h>
#endif

/* Globals */
UINT32 rsi_zone_enabled =  RSI_ZONE_ERROR |
  RSI_ZONE_INIT  |
  RSI_ZONE_SPI_DBG 
#ifdef RS9116
  | RSI_ZONE_RS9116_DBG 
#endif
  | RSI_ZONE_SDIO_DBG
  ;
/*==============================================*/
/**
 * @fn          struct sk_buff* rsi_alloc_skb(UINT32 Len)
 * @brief       allocates skb
 * @param[in]   UINT32 Len, length to be allocated
 * @param[out]  none
 * @return      struct sk_buff*, pointer to allocated socket buffer
 * @section description
 * This is used to allocate skb.
 */
struct sk_buff*
rsi_alloc_skb
(
  UINT32 Len
)
{

#ifdef RSI_DEBUG_PRINT
  RSI_DEBUG(RSI_ZONE_SPI_DBG,"\nrsi_alloc_skb:\n");
#endif
  return dev_alloc_skb(Len + 2); /*FIXME - CHECK DIS*/
}

/*==============================================*/
/**
 * @fn          VOID rsi_dump(INT32 zone, PVOID vdata, INT32 len)
 * @brief       Dumps the data through debugger
 * @param[in]   INT32 zone, Pointer to Adapter
 * @param[in]   PVOID vdata, Data to dump
 * @param[in]   INT32 len, length of the data to dump
 * @param[out]  none
 * @return      none
 * @section description
 * This function dumps the given data through the debugger
 */
VOID
rsi_dump
(
  INT32 zone,
  PVOID vdata,
  INT32 len
)
{
  INT32 ii;
#ifdef RSI_DEBUG_PRINT
  RSI_DEBUG(RSI_ZONE_SPI_DBG,"\nrsi_dump\n");
#endif
  UINT8 *data = vdata;
  for (ii = 0; ii < len; ii++)
  {
    if (!(ii % 16))
    {
      RSI_DEBUG(zone,"\n");
    }
    RSI_DEBUG(zone,"%02x ", data[ii]);
  }
  RSI_DEBUG(zone,"\n");
}

/*==============================================*/
/**
 * @fn          INT32 rsi_Init_Event(RSI_EVENT *pEvent)
 * @brief       Initializes event
 * @param[in]   RSI_EVENT *pEvent, Pointer to event
 * @return      0
 * @section description
 * This function initializes event.
 */
INT32 rsi_Init_Event(RSI_EVENT *pEvent)
{
#ifdef RSI_DEBUG_PRINT
  RSI_DEBUG(RSI_ZONE_SPI_DBG,"\nrsi_Init_Event:\n");
#endif
  RSI_DEBUG(RSI_ZONE_INFO, (TEXT("Initialising the event %p addr\n"),pEvent));

  atomic_set(&pEvent->eventCondition, 1);
  init_waitqueue_head(&pEvent->eventQueue);
  return 0;
}

/*==============================================*/
/**
 * @fn          INT32 rsi_Delete_Event(RSI_EVENT *pEvent)
 * @brief       Deletes event
 * @param[in]   RSI_EVENT *pEvent, Pointer to event
 * @return      0
 * @section description
 * This function Deletes event.
 */
INT32 rsi_Delete_Event (RSI_EVENT *pEvent)
{
  /**Dummy for Linux*/
#ifdef RSI_DEBUG_PRINT
  RSI_DEBUG(RSI_ZONE_SPI_DBG,"\nrsi_Delete_Event:\n");
#endif
  return 0;
}

/*==============================================*/
/**
 * @fn          INT32 rsi_Wait_Event(RSI_EVENT *pEvent, UINT32 timeOut)
 * @brief       Wait event
 * @param[in]   RSI_EVENT *pEvent, Pointer to event
 * @param[in]   UINT32 timeOut, Wait timeout
 * @return      0
 * @section description
 * This function handles the wait event functionality.
 */
INT32 rsi_Wait_Event(RSI_EVENT *pEvent, UINT32 timeOut)
{
  INT32 Status = 0;
#ifdef RSI_DEBUG_PRINT
  RSI_DEBUG(RSI_ZONE_SPI_DBG,"\nrsi_Wait_Event:\n");
#endif
  if (!timeOut)
  {
    wait_event_interruptible(pEvent->eventQueue,
                             (atomic_read(&pEvent->eventCondition) == 0));
  }
  else
  {
    Status = wait_event_interruptible_timeout(pEvent->eventQueue,
                                              (atomic_read(&pEvent->eventCondition) == 0),
                                              timeOut);
  } /* End if <condition> */

  return Status;
}

/*==============================================*/
/**
 * @fn          INT32 rsi_Set_Event(RSI_EVENT *pEvent)
 * @brief       Set event
 * @param[in]   RSI_EVENT *pEvent, Pointer to event
 * @return      none
 * @section description
 * This function handles the set event functionality.
 */
VOID rsi_Set_Event(RSI_EVENT *pEvent)
{
#ifdef RSI_DEBUG_PRINT
  RSI_DEBUG(RSI_ZONE_SPI_DBG,"\nrsi_Set_Event:\n");
#endif
  atomic_set(&pEvent->eventCondition, 0);
  wake_up_interruptible(&pEvent->eventQueue);
}

/*==============================================*/
/**
 * @fn          INT32 rsi_Reset_Event(RSI_EVENT *pEvent)
 * @brief       Reset event
 * @param[in]   RSI_EVENT *pEvent, Pointer to event
 * @return      VOID
 * @section description
 * This function handles the reset event functionality.
 */
INT32 rsi_Reset_Event(RSI_EVENT *pEvent)
{
#ifdef RSI_DEBUG_PRINT
  RSI_DEBUG(RSI_ZONE_SPI_DBG,"\nrsi_Reset_Event:\n");
#endif
  atomic_set(&pEvent->eventCondition,1);
}

/*==============================================*/
/**
 * @fn          INT32 rsi_Kill_Thread(PRSI_ADAPTER Adapter)
 * @brief       Kills the thread
 * @param[in]   PRSI_ADAPTER Adapter, Pointer to Adapter
 * @return      0
 * @section description
 * This function kills the thread
 */
INT32
rsi_Kill_Thread
(
  PRSI_ADAPTER Adapter
)
{
#ifdef RSI_DEBUG_PRINT
  RSI_DEBUG(RSI_ZONE_SPI_DBG,"\nrsi_Kill_Thread:\n");
#endif
  Adapter->transmit_thread_exit = 1;
  rsi_Set_Event(&Adapter->Event);
#ifdef RSI_UART_INTERFACE
#if RSI_TCP_IP_BYPASS
  rsi_Set_Event(&Adapter->DATA_ACKEvent);
#endif
#endif
  
  wait_for_completion(&Adapter->txThreadComplete);
  Adapter->txThreadId = 0;
  return 0;
}

/*==============================================*/
/**
 * @fn          INT32 rsi_Kill_Rx_Thread(struct rsi_thread *handle)
 * @brief       Kills the Rx thread
 * @param[in]   Pointer to rsi_thread
 * @return      0
 * @section description
 * This function kills Rx thread
 */
INT32
rsi_Kill_Rx_Thread
(
  struct rsi_thread *handle
)
{
#ifdef RSI_DEBUG_PRINT
  RSI_DEBUG(RSI_ZONE_SPI_DBG,"\nrsi_Kill_Rx_Thread:\n");
#endif
  if (atomic_read(&handle->thread_done) > 0)
	  return 0;
  atomic_inc(&handle->thread_done);
  rsi_Set_Event(&handle->event);
  return kthread_stop(handle->task);
}

/*==============================================*/
/**
 * @fn          INT32 rsi_Init_Thread(PRSI_ADAPTER Adapter)
 * @brief       Initializes thread
 * @param[in]   PRSI_ADAPTER Adapter, Pointer to Adapter
 * @return      0 on success, else failure
 * @section description
 * This function initializes event.
 */
uint8   name_p[20] = "XMIT-Thread";
INT32
rsi_Init_Thread
(
  PRSI_ADAPTER Adapter
)
{
#ifdef RSI_DEBUG_PRINT
  RSI_DEBUG(RSI_ZONE_SPI_DBG,"\nrsi_Init_Thread:\n");
#endif
  INT32 (*function) (PVOID pContext)=NULL;

  function = rsi_transmit_thread;

  Adapter->txThreadId = 0;
  Adapter->transmit_thread_exit=0;
  init_completion(&Adapter->txThreadComplete);
#if LINUX_VERSION_CODE < KERNEL_VERSION(2,6,39)
	Adapter->txThreadId =  kernel_thread(function,
                                       Adapter,
                                       CLONE_FS |CLONE_FILES);
#else
	Adapter->txThreadId = (int)kthread_run(function,Adapter,name_p);
#endif

  if (Adapter->txThreadId < 0)
  {
    RSI_DEBUG(RSI_ZONE_ERROR,(TEXT("rsi_probe: Unable to initialize thrd\n")));
    return RSI_STATUS_FAIL;
  }
  else
  {
    return RSI_STATUS_SUCCESS;
  } /* End if <condition> */
}

int rsi_create_kthread( PRSI_ADAPTER Adapter,
  struct rsi_thread *thread,
  void *func_ptr,
  u8 *name)
{
  init_completion(&thread->completion);
  atomic_set(&thread->thread_done, 0);
  thread->task = kthread_run(func_ptr, Adapter, "%s", name);
  if (IS_ERR(thread->task))
	  return (int)PTR_ERR(thread->task);

  return 0;
}

/* $EOF */
/* Log */
