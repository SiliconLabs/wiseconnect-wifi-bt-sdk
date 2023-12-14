

/*******************************************************************************
* @file  rsi_common_utils.c
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
 * @file    rsi_utils.c
 * @version 0.1
 * @date    5 Aug 2021
 *
 *
 *
 *  @brief : This file contains utility functions required by different demo applications
 *
 *  @section Description  This file contains different utility functions required by different demo applications
 */

/*=======================================================================*/
//   ! INCLUDES
/*=======================================================================*/
#include <rsi_common_app.h>
#include "rsi_driver.h"
#include "rsi_common_utils.h"
#include "assert.h"
#ifdef MXRT_595s
#include "sl_hal_mcu_uart.h"

extern rsi_semaphore_handle_t ui_task_sem;

/*==============================================*/
/**
 * @fn         sl_handle_received_data_wrapper
 * @brief      handle the received data through uart.
 * @param[in]  sl_flexcomm_uart_cb, structure that contains uart specific parameters.
 * @return     int32_t
 *              0   = not disable uart, so that data will come till the condition met
 *              1   = disable the uart till user process ring buffer
 * @section description
 * This function is used to handle received data through uart.
 */
int32_t sl_handle_received_data_wrapper(sl_flexcomm_uart_cb_t *sl_flexcomm_uart_cb)
{
  //! sending same byte back to the uart terminal
  sl_flexcomm_uart_cb->base->FIFOWR = sl_flexcomm_uart_cb->demoRingBuffer[sl_flexcomm_uart_cb->rxIndex - 1];
  //!hfp use-case
  if (sl_flexcomm_uart_cb->demoRingBuffer[sl_flexcomm_uart_cb->rxIndex - 1] == '\n') {
    sl_flexcomm_uart_cb->rxIndex = 0;
#ifdef RSI_WITH_OS
    rsi_semaphore_post_from_isr(&ui_task_sem);
#endif
    return 1;
  } else {
    return 0;
  }
}
#endif
/*==============================================*/
/**
 * @fn         rsi_init_event_parameters
 * @brief      initializes the event parameters.
 * @param[in]  async_event_map, ative events bitmapmap.
 * @param[in]  async_event_map_mask, masked events bitmap.
 * @param[in]  task_sem, Semaphore to the task.
 * @param[in]  evevnt_map_mutex, Mutex for accesing event_map.
 * @return     none.
 * @section description
 * This function is used during application task intialilzation
 */
void rsi_app_init_event_parameters(uint32_t *async_event_map,
                                   uint32_t *async_event_map_mask,
                                   rsi_semaphore_handle_t *task_sem,
                                   rsi_mutex_handle_t *event_map_mutex)
{
  assert(async_event_map);
  assert(async_event_map_mask);

  async_event_map[0] = 0;
  async_event_map[1] = 0;

  if (async_event_map_mask != NULL) {
    async_event_map_mask[0] = 0;
    async_event_map_mask[1] = 0;
  }

  if (task_sem != NULL) {
    rsi_semaphore_create(task_sem, 0);
  }

  if (event_map_mutex != NULL) {
    rsi_mutex_create(event_map_mutex);
  }
  return;
}

/*==============================================*/
/**
 * @fn         rsi_deinit_event_parameters
 * @brief      deinitializes the event parameters.
 * @param[in]  task_sem, Semaphore to the event.
 * @param[in]  event_map_mutex, Mutex for accesing event_map.
 * @return     none.
 * @section description
 * This function is used during application task deinitialization
 */
void rsi_app_deinit_event_parameters(rsi_semaphore_handle_t *task_sem, rsi_mutex_handle_t *event_map_mutex)
{
  if (task_sem != NULL) {
    rsi_semaphore_destroy(task_sem);
  }

  if (event_map_mutex != NULL) {
    rsi_mutex_destroy(event_map_mutex);
  }
  return;
}

/**
 * @fn         rsi_app_set_event
 * @brief      sets the specific event.
 * @param[in]  event_num, specific event number.
 * @param[in]  async_event_map, active events bitmap.
 * @param[in]  task_sem, Semaphore to the event.
 * @param[in]  event_map_mutex, Mutex for accesing event_map.
 * @return     none.
 * @section description
 * This function is used to set/raise the specific event.
 */
void rsi_app_set_event(uint32_t event_num,
                       uint32_t *async_event_map,
                       rsi_semaphore_handle_t *task_sem,
                       rsi_mutex_handle_t *event_map_mutex)
{
  //! Take Mutex Lock
  if (event_map_mutex != NULL) {
    rsi_mutex_lock(event_map_mutex);
  }
  //! Set Actual Event
  if (event_num <= 31) {
    async_event_map[0] |= BIT(event_num);
  } else {
    async_event_map[1] |= BIT((event_num - 32));
  }
  //! Release the Mutex Lock
  if (event_map_mutex != NULL) {
    rsi_mutex_unlock(event_map_mutex);
  }
  //! Post semaphore to kick start the task again
  if (task_sem != NULL) {
    rsi_semaphore_post(task_sem);
  }
  return;
}

/**
 * @fn         rsi_app_set_event_from_isr
 * @brief      sets the specific event from interrupt context.
 * @param[in]  event_num, specific event number.
 * @param[in]  async_event_map, active events bitmap.
 * @param[in]  task_sem, Semaphore to the event.
 * @param[in]  event_map_mutex, Mutex for accesing event_map.
 * @return     none.
 * @section description
 * This function is used to set/raise the specific event.
 */
void rsi_app_set_event_from_isr(uint32_t event_num,
                                uint32_t *async_event_map,
                                rsi_semaphore_handle_t *task_sem,
                                rsi_mutex_handle_t *event_map_mutex)
{
  //! Take Mutex Lock
  if (event_map_mutex != NULL) {
#ifdef RSI_WITH_OS
    rsi_mutex_lock_from_isr(event_map_mutex);
#endif
  }
  //! Set Actual Event
  if (event_num <= 31) {
    async_event_map[0] |= BIT(event_num);
  } else {
    async_event_map[1] |= BIT((event_num - 32));
  }
  if (event_map_mutex != NULL) {
#ifdef RSI_WITH_OS
    rsi_mutex_unlock_from_isr(event_map_mutex);
#endif
  }
  //! Release Mutex Lock
  if (task_sem != NULL) {
    rsi_semaphore_post_from_isr(task_sem);
  }
  return;
}
/**
 * @fn         rsi_app_clear_event
 * @brief      clears specific event.
 * @param[in]  event_num, specific event number.
 * @param[in]  async_event_map, active events bitmap.
 * @param[in]  event_map_mutex, Mutex for accesing event_map.
 * @return     none.
 * @section description
 * This function is used to set/raise the specific event.
 */
/*==============================================*/
void rsi_app_clear_event(uint32_t event_num, uint32_t *async_event_map, rsi_mutex_handle_t *event_map_mutex)
{
  if (event_map_mutex != NULL) {
    rsi_mutex_lock(event_map_mutex);
  }

  if (event_num <= 31) {
    async_event_map[0] &= ~BIT(event_num);
  } else {
    async_event_map[1] &= ~BIT((event_num - 32));
  }

  if (event_map_mutex != NULL) {
    rsi_mutex_unlock(event_map_mutex);
  }
  return;
}
/**
 * @fn         rsi_app_mask_event
 * @brief      masks the specific event.
 * @param[in]  event_num, specific event number.
 * @param[in]  async_event_map_mask, active events bitmap.
 * @param[in]  event_map_mutex, Mutex for accesing event_map.
 * @return     none.
 * @section description
 * This function is used to set/raise the specific event.
 */
void rsi_app_mask_event(uint32_t event_num, uint32_t *async_event_map_mask, rsi_mutex_handle_t *event_map_mutex)
{
  if (event_map_mutex != NULL) {
    rsi_mutex_lock(event_map_mutex);
  }
  if (event_num <= 31) {
    async_event_map_mask[0] |= BIT(event_num);
  } else {
    async_event_map_mask[1] |= BIT((event_num - 32));
  }

  if (event_map_mutex != NULL) {
    rsi_mutex_unlock(event_map_mutex);
  }
  return;
}

/**
 * @fn         rsi_app_unmask_event_from_isr
 * @brief      unmasks the specific event from interrupt context.
 * @param[in]  event_num, specific event number.
 * @param[in]  async_event_map_mask, active events bitmap.
 * @param[in]  task_sem, Semaphore to the event.
 * @param[in]  event_map_mutex, Mutex for accesing event_map.
 * @return     none.
 * @section description
 * This function is used to set/raise the specific event.
 */
void rsi_app_unmask_event_from_isr(uint32_t event_num,
                                   uint32_t *async_event_map_mask,
                                   rsi_semaphore_handle_t *task_sem,
                                   rsi_mutex_handle_t *event_map_mutex)
{
  if (event_map_mutex != NULL) {
#ifdef RSI_WITH_OS
    rsi_mutex_lock_from_isr(event_map_mutex);
#endif
  }
  if (event_num <= 31) {
    async_event_map_mask[0] &= ~BIT(event_num);
  } else {
    async_event_map_mask[1] &= ~BIT((event_num - 32));
  }

  if (event_map_mutex != NULL) {
#ifdef RSI_WITH_OS
    rsi_mutex_unlock_from_isr(event_map_mutex);
#endif
  }
  if (task_sem != NULL) {
    rsi_semaphore_post_from_isr(task_sem);
  }
  return;
}
/**
 * @fn         rsi_app_unmask_event
 * @brief      unmasks specific event.
 * @param[in]  event_num, specific event number.
 * @param[in]  async_event_map_mask, active events mask bitmap.
 * @param[in]  event_map_mutex, Mutex for accesing event_map.
 * @param[in]  event_map_mutex, Mutex for accesing event_map.
 * @return     none.
 * @section description
 * This function is used to set/raise the specific event.
 */
/*==============================================*/
void rsi_app_unmask_event(uint32_t event_num,
                          uint32_t *async_event_map_mask,
                          rsi_semaphore_handle_t *task_sem,
                          rsi_mutex_handle_t *event_map_mutex)
{
  if (event_map_mutex != NULL) {
    rsi_mutex_lock(event_map_mutex);
  }

  if (event_num <= 31) {
    async_event_map_mask[0] &= ~BIT(event_num);
  } else {
    async_event_map_mask[1] &= ~BIT((event_num - 32));
  }

  if (event_map_mutex != NULL) {
    rsi_mutex_unlock(event_map_mutex);
  }
  if (task_sem != NULL) {
    rsi_semaphore_post(task_sem);
  }
  return;
}
/*==============================================*/
/**
 * @fn         rsi_app_get_event
 * @brief      returns the first set and unmasked event based on priority
 * @param[in]  async_event_map, bitmap for events set
 * @param[in]  async_event_map_mask, bitmap for maksed events
 * @return     int32_t
 *             > 0  = event number
 *             -1   = not received any event
 * @section description
 * This function returns the highest priority event among all the set events
 */
int32_t rsi_app_get_event(uint32_t *async_event_map, uint32_t *async_event_map_mask)
{
  uint32_t ix;

  for (ix = 0; ix < 64; ix++) {
    if (ix <= 31) {
      if ((async_event_map[0] & ~(async_event_map_mask[0])) & (1 << ix)) {
        return ix;
      }
    } else {
      if ((async_event_map[1] & ~(async_event_map_mask[1])) & (1 << (ix - 32))) {
        return ix;
      }
    }
  }

  return (RSI_FAILURE);
}

/*==============================================*/
/**
 * @fn          void rsi_app_enqueue_pkt(rsi_app_queue_t *queue,rsi_app_pkt_t *pkt)
 * @brief       Enqueues packet into queue
 * @param[in]   queue , pointer to queue cb 
 * @param[in]   pkt , packet pointer which needs to be queued 
 * @param[out]  None
 * @return      None
 *
 * @section description
 * This function enqueues packet into queue
 *
 */

void rsi_app_enqueue_pkt(rsi_app_queue_t *queue, rsi_app_pkt_t *pkt)
{

  //! take lock on queue
  rsi_mutex_lock(&queue->queue_mutex);

  //! check queue is empty
  if (!queue->pkt_cnt) {
    //! if empty then add packet as first packet (head & tail point to first packet)
    queue->head = queue->tail = pkt;
  } else {
    //! if not empty append the packet to list at tail
    queue->tail->next = pkt;

    //! Make packet as tail
    queue->tail = pkt;
  }

  //! increment packet pending count
  queue->pkt_cnt++;

  //! Unlock the queue
  rsi_mutex_unlock(&queue->queue_mutex);
}
/*==============================================*/
/**
 * @fn          void rsi_app_enqueue_pkt_from_isr(rsi_app_queue_t *queue,rsi_app_pkt_t *pkt)
 * @brief       Enqueues packet into queue from ISR context
 * @param[in]   queue , pointer to queue cb 
 * @param[in]   pkt , packet pointer which needs to be queued 
 * @param[out]  None
 * @return      None
 *
 * @section description
 * This function enqueues packet into queue
 *
 */

void rsi_app_enqueue_pkt_from_isr(rsi_app_queue_t *queue, rsi_app_pkt_t *pkt)
{

  //! take lock on queue
#ifdef RSI_WITH_OS
  rsi_mutex_lock_from_isr(&queue->queue_mutex);
#endif
  //! check queue is empty
  if (!queue->pkt_cnt) {
    //! if empty then add packet as first packet (head & tail point to first packet)
    queue->head = queue->tail = pkt;
  } else {
    //! if not empty append the packet to list at tail
    queue->tail->next = pkt;

    //! Make packet as tail
    queue->tail = pkt;
  }

  //! increment packet pending count
  queue->pkt_cnt++;

  //! Unlock the queue
#ifdef RSI_WITH_OS
  rsi_mutex_unlock_from_isr(&queue->queue_mutex);
#endif
}
/*==============================================*/
/**
 * @fn          void rsi_app_enqueue_pkt_with_mutex(app_queue_t *queue, rsi_app_pkt_t *pkt, rsi_mutex_handle_t *queue_mutex)
 * @brief       Enqueues packet into queue
 * @param[in]   queue , pointer to queue cb 
 * @param[in]   pkt , packet pointer which needs to be queued 
 * @param[in]   queue_mutex , mutex pointer which needs to protect queue 
 * @param[out]  None
 * @return      None
 *
 * @section description
 * This function enqueues packet into queue
 *
 */

void rsi_app_enqueue_pkt_with_mutex(app_queue_t *queue, rsi_app_pkt_t *pkt, rsi_mutex_handle_t *queue_mutex)
{

  //! take lock on queue
  rsi_mutex_lock(queue_mutex);

  //! check queue is empty
  if (!queue->pkt_cnt) {
    //! if empty then add packet as first packet (head & tail point to first packet)
    queue->head = queue->tail = pkt;
  } else {
    //! if not empty append the packet to list at tail
    queue->tail->next = pkt;

    //! Make packet as tail
    queue->tail = pkt;
  }

  //! increment packet pending count
  queue->pkt_cnt++;

  //! Unlock the queue
  rsi_mutex_unlock(queue_mutex);
}
/*==============================================*/
/**
 * @fn          rsi_app_pkt_t *rsi_app_dequeue_pkt(rsi_app_queue_t *queue)
 * @brief       Dequeues packet from queue
 * @param[in]   queue , queue pointer from which packet needs to be dequeued 
 * @param[out]  None
 * @return      None
 *
 *
 * @section description
 * This function dequeues packet from queue
 *
 */
rsi_app_pkt_t *rsi_app_dequeue_pkt(rsi_app_queue_t *queue)
{
  rsi_app_pkt_t *pkt;

  //! lock the mutex
  rsi_mutex_lock(&queue->queue_mutex);

  //! check queue is empty
  if (!queue->pkt_cnt) {

    rsi_mutex_unlock(&queue->queue_mutex);
    //! return NULL if queue is empty
    return NULL;
  }

  //! dequeue the packet from queue
  pkt = queue->head;

  //! update the queue head and decrement pending count
  queue->head = queue->head->next;

  //! Decrease pending packet count
  queue->pkt_cnt--;

  //! if pending count is zero, then reset head and tail
  if (!queue->pkt_cnt) {
    queue->head = NULL;
    queue->tail = NULL;
  }

  //! Unlock the mutex
  rsi_mutex_unlock(&queue->queue_mutex);

  //! return the packet
  return pkt;
}
/*==============================================*/
/**
 * @fn          rsi_app_pkt_t *rsi_app_dequeue_pkt_from_isr(rsi_app_queue_t *queue)
 * @brief       Dequeues packet from queue from ISR context
 * @param[in]   queue , queue pointer from which packet needs to be dequeued 
 * @param[out]  None
 * @return      None
 *
 *
 * @section description
 * This function dequeues packet from queue
 *
 */

rsi_app_pkt_t *rsi_app_dequeue_pkt_from_isr(rsi_app_queue_t *queue)
{
  rsi_app_pkt_t *pkt;

  //! lock the mutex
#ifdef RSI_WITH_OS
  rsi_mutex_lock_from_isr(&queue->queue_mutex);
#endif
  //! check queue is empty
  if (!queue->pkt_cnt) {
    //! Unlock the mutex
#ifdef RSI_WITH_OS
    rsi_mutex_unlock_from_isr(&queue->queue_mutex);
#endif
    //! return NULL if queue is empty
    return NULL;
  }

  //! dequeue the packet from queue
  pkt = queue->head;

  //! update the queue head and decrement pending count
  queue->head = queue->head->next;

  //! Decrease pending packet count
  queue->pkt_cnt--;

  //! if pending count is zero, then reset head and tail
  if (!queue->pkt_cnt) {
    queue->head = NULL;
    queue->tail = NULL;
  }

  //! Unlock the mutex
#ifdef RSI_WITH_OS
  rsi_mutex_unlock_from_isr(&queue->queue_mutex);
#endif
  //! return the packet
  return pkt;
}

/*==============================================*/
/**
 * @fn          rsi_app_dequeue_pkt_with_mutex(app_queue_t *queue, rsi_mutex_handle_t *queue_mutex)
 * @brief       Dequeues packet from queue
 * @param[in]   queue , queue pointer from which packet needs to be dequeued 
 * @param[out]  None
 * @return      None
 *
 *
 * @section description
 * This function dequeues packet from queue
 *
 */
rsi_app_pkt_t *rsi_app_dequeue_pkt_with_mutex(app_queue_t *queue, rsi_mutex_handle_t *queue_mutex)
{
  rsi_app_pkt_t *pkt;

  //! lock the mutex
  rsi_mutex_lock(queue_mutex);

  //! check queue is empty
  if (!queue->pkt_cnt) {

    rsi_mutex_unlock(queue_mutex);
    //! return NULL if queue is empty
    return NULL;
  }

  //! dequeue the packet from queue
  pkt = queue->head;

  //! update the queue head and decrement pending count
  queue->head = queue->head->next;

  //! Decrease pending packet count
  queue->pkt_cnt--;

  //! if pending count is zero, then reset head and tail
  if (!queue->pkt_cnt) {
    queue->head = NULL;
    queue->tail = NULL;
  }

  //! Unlock the mutex
  rsi_mutex_unlock(queue_mutex);

  //! return the packet
  return pkt;
}
void rsi_app_init_queue(rsi_app_queue_t *queue)
{
  rsi_mutex_create(&queue->queue_mutex);
}
void _assert(uint8_t *msg)
{
  LOG_PRINT("\r\n %s \n", msg);
  while (1)
    ;
}

void rsi_free_memory(void *ptr)
{
  if (ptr == NULL) {
    // do nothing
  } else {
#ifdef RSI_WITH_OS
    vPortFree(ptr);
#else
    _assert("OS Handling required");
#endif
  }
}