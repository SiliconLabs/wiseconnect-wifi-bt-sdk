/*******************************************************************************
* @file  rsi_os_wrapper.c
* @brief Linux OS wrapper implementation for RSI driver
*******************************************************************************
* # License
* <b>Copyright 2025 Silicon Laboratories Inc. www.silabs.com</b>
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

#include "rsi_driver.h"
#ifdef RSI_WITH_OS
#ifdef LINUX_PLATFORM
/**
 * Include files
 * */
#include <pthread.h>
#include <semaphore.h>
#include <time.h>
#include <errno.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/time.h>
#include "rsi_wlan_non_rom.h"

extern rsi_socket_info_non_rom_t *rsi_socket_pool_non_rom;

// Check if RSI_NUMBER_OF_SOCKETS is defined, otherwise use a safe default
#ifndef RSI_NUMBER_OF_SOCKETS
#define RSI_NUMBER_OF_SOCKETS 10  // Safe default if not defined
#endif

// Internal structures for Linux OS wrapper
typedef struct {
  pthread_mutex_t mutex;
  uint8_t initialized;
} linux_mutex_t;

typedef struct {
  sem_t sem;
  uint8_t initialized;
} linux_semaphore_t;

typedef struct {
  pthread_t thread;
  volatile uint8_t running;      // Atomic flag for task running state
  volatile uint8_t should_exit;  // Flag for graceful shutdown
  volatile uint8_t suspended;    // Flag for task suspension
  pthread_cond_t suspend_cond;   // Condition variable for suspend/resume
  pthread_mutex_t suspend_mutex; // Mutex for suspend/resume synchronization
  rsi_task_function_t task_function;
  void *parameters;
  sem_t notify_sem;
  uint32_t notification_value;
  uint32_t notification_count;  // Counter for notification take semantics
  pthread_mutex_t notify_mutex;
} linux_task_t;

// Tick count for Linux (1ms = 1 tick)
#define LINUX_TICK_PER_MS 1

// Thread-local storage for current task notification
// Forward declaration - will be defined after linux_task_t
static __thread linux_task_t *current_task_tls = NULL;

/** @addtogroup LINUX
* @{
*/
/*==============================================*/
/**
 * @fn          rsi_reg_flags_t rsi_critical_section_entry()
 * @brief       Enter critical section
 * @param[in]   None
 * @return      Interrupt status before entering critical section
 *
 */
// Global critical section mutex (shared between entry and exit)
// Use recursive mutex to prevent deadlocks when same thread enters critical section multiple times
static pthread_mutex_t critical_mutex;
static pthread_once_t critical_mutex_once = PTHREAD_ONCE_INIT;

// Initialize critical section mutex (thread-safe, called once)
static void init_critical_mutex_once(void)
{
  pthread_mutexattr_t attr;
  pthread_mutexattr_init(&attr);
  pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_RECURSIVE);
  pthread_mutex_init(&critical_mutex, &attr);
  pthread_mutexattr_destroy(&attr);
}

rsi_reg_flags_t rsi_critical_section_entry()
{
  // For Linux, we use a recursive mutex for critical sections
  // This prevents deadlocks when the same thread enters critical section multiple times
  rsi_reg_flags_t xflags = 0;
  
  // Initialize mutex once (thread-safe)
  pthread_once(&critical_mutex_once, init_critical_mutex_once);
  
  // Lock the critical section mutex (recursive, so same thread can lock multiple times)
  pthread_mutex_lock(&critical_mutex);
  
  // Return stored interrupt status (not used in Linux)
  return (xflags);
}

/*==============================================*/
/**
 * @fn         rsi_critical_section_exit(rsi_reg_flags_t xflags)
 * @brief      Exit critical section by restoring interrupts.
 * @param[in]  xflags - Interrupt status to restore interrupt on exit from critical section
 * @return     Void
 *
 *
 *
 */

void rsi_critical_section_exit(rsi_reg_flags_t xflags)
{
  // This statement is added only to resolve compilation warning, value is unchanged
  UNUSED_PARAMETER(xflags);
  
  // Unlock the critical section mutex
  int ret = pthread_mutex_unlock(&critical_mutex);
  // Note: In production, consider logging or handling unlock failures
  (void)ret;  // Suppress unused variable warning
}

/*==============================================*/
/**
 * @fn           rsi_error_t rsi_mutex_create(rsi_mutex_handle_t *mutex)
 * @brief        Create and initialize the mutex 
 * @param[in]    mutex - Mutex handle pointer
 * @return       0              - Success \n
 * @return          Negative Value - Failure
 *
 *
 */

rsi_error_t rsi_mutex_create(rsi_mutex_handle_t *mutex)
{
  linux_mutex_t *p_mutex = NULL;
  
  if (mutex == NULL) {
    return RSI_ERROR_IN_OS_OPERATION;
  }

  // Allocate memory for Linux mutex structure
  p_mutex = (linux_mutex_t *)malloc(sizeof(linux_mutex_t));
  if (p_mutex == NULL) {
    return RSI_ERROR_IN_OS_OPERATION;
  }

  // Initialize pthread mutex
  if (pthread_mutex_init(&(p_mutex->mutex), NULL) != 0) {
    free(p_mutex);
    return RSI_ERROR_IN_OS_OPERATION;
  }

  p_mutex->initialized = 1;
  *mutex = (rsi_mutex_handle_t)p_mutex;

  return RSI_ERROR_NONE;
}

/*==============================================*/
/**
 * @fn           rsi_error_t rsi_mutex_lock(volatile rsi_mutex_handle_t *mutex)
 * @brief        Lock the mutex 
 * @param[in]    mutex - Mutex handle pointer  
 * @return       0              - Success \n
 * @return       Negative Value - Failure    
 *
 */
rsi_error_t rsi_mutex_lock(volatile rsi_mutex_handle_t *mutex)
{
  linux_mutex_t *p_mutex = NULL;
  
  if (mutex == NULL || *mutex == NULL) {
    return RSI_ERROR_INVALID_PARAM;
  }

  p_mutex = (linux_mutex_t *)(*mutex);
  
  if (!p_mutex->initialized) {
    return RSI_ERROR_INVALID_PARAM;
  }

  // Lock the mutex (blocking)
  if (pthread_mutex_lock(&(p_mutex->mutex)) != 0) {
    return RSI_ERROR_IN_OS_OPERATION;
  }

  return RSI_ERROR_NONE;
}

/*==============================================*/
/**
 * @fn           rsi_error_t rsi_mutex_lock_from_isr(volatile rsi_mutex_handle_t *mutex)
 * @brief        Lock the mutex from ISR context
 * @param[in]    mutex - Mutex handle pointer  
 * @return       0              - Success \n
 * @return       Negative Value - Failure   
 *
 */
void rsi_mutex_lock_from_isr(volatile rsi_mutex_handle_t *mutex)
{
  linux_mutex_t *p_mutex = NULL;
  
  if (mutex == NULL || *mutex == NULL) {
    return;
  }

  p_mutex = (linux_mutex_t *)(*mutex);
  
  if (!p_mutex->initialized) {
    return;
  }

  // Try lock (non-blocking) - in ISR context we try lock
  pthread_mutex_trylock(&(p_mutex->mutex));
}

/*==============================================*/
/**
 * @fn           rsi_error_t rsi_mutex_unlock(volatile rsi_mutex_handle_t *mutex)
 * @brief        Unlock the mutex 
 * @param[in]    mutex - Mutex handle pointer  
 * @return       0              - Success \n 
 * @return       Negative Value - Failure  
 *
 */
rsi_error_t rsi_mutex_unlock(volatile rsi_mutex_handle_t *mutex)
{
  linux_mutex_t *p_mutex = NULL;
  
  if (mutex == NULL || *mutex == NULL) {
    return RSI_ERROR_INVALID_PARAM;
  }

  p_mutex = (linux_mutex_t *)(*mutex);
  
  if (!p_mutex->initialized) {
    return RSI_ERROR_INVALID_PARAM;
  }

  if (pthread_mutex_unlock(&(p_mutex->mutex)) != 0) {
    return RSI_ERROR_IN_OS_OPERATION;
  }

  return RSI_ERROR_NONE;
}

/*==============================================*/
/**
 * @fn           rsi_error_t rsi_mutex_unlock_from_isr(volatile rsi_mutex_handle_t *mutex)
 * @brief        Unlock the mutex from ISR context
 * @param[in]    mutex - Mutex handle pointer  
 * @return       None
 *
 */
void rsi_mutex_unlock_from_isr(volatile rsi_mutex_handle_t *mutex)
{
  linux_mutex_t *p_mutex = NULL;
  
  if (mutex == NULL || *mutex == NULL) {
    return;
  }

  p_mutex = (linux_mutex_t *)(*mutex);
  
  if (!p_mutex->initialized) {
    return;
  }

  // Unlock from ISR context
  pthread_mutex_unlock(&(p_mutex->mutex));
}

/*==============================================*/
/**
 * @fn           rsi_error_t rsi_mutex_destroy(rsi_mutex_handle_t *mutex)
 * @brief        Destroy the mutex 
 * @param[in]    mutex - Mutex handle pointer  
 * @return       0              - Success \n
 * @return       Negative Value - Failure
 *
 */
rsi_error_t rsi_mutex_destroy(rsi_mutex_handle_t *mutex)
{
  linux_mutex_t *p_mutex = NULL;
  
  if (mutex == NULL || *mutex == NULL) {
    return RSI_ERROR_INVALID_PARAM;
  }

  p_mutex = (linux_mutex_t *)(*mutex);
  
  if (!p_mutex->initialized) {
    return RSI_ERROR_INVALID_PARAM;
  }

  // Mark as uninitialized and clear handle before destroying to prevent race conditions
  p_mutex->initialized = 0;
  *mutex = NULL;
  
  pthread_mutex_destroy(&(p_mutex->mutex));
  free(p_mutex);

  return RSI_ERROR_NONE;
}

/*==============================================*/
/**
 * @fn           rsi_error_t rsi_semaphore_create(rsi_semaphore_handle_t *semaphore,uint32_t count)
 * @brief        Create and initialize the semaphore instance 
 * @param[in]    semaphore - Semaphore handle pointer  
 * @param[in]    count - Resource count   
 * @return       0              - Success \n 
 * @return              Negative Value - Failure
 *
 */
rsi_error_t rsi_semaphore_create(rsi_semaphore_handle_t *semaphore, uint32_t count)
{
  linux_semaphore_t *p_semaphore = NULL;
  int ret;

  UNUSED_PARAMETER(count); // Binary semaphore is used, count is not used

  if (semaphore == NULL) {
    return RSI_ERROR_IN_OS_OPERATION;
  }

  // Allocate memory for Linux semaphore structure
  p_semaphore = (linux_semaphore_t *)malloc(sizeof(linux_semaphore_t));
  if (p_semaphore == NULL) {
    return RSI_ERROR_IN_OS_OPERATION;
  }

  // Initialize semaphore (binary semaphore, initial value 0)
  ret = sem_init(&(p_semaphore->sem), 0, 0);
  if (ret != 0) {
    free(p_semaphore);
    return RSI_ERROR_IN_OS_OPERATION;
  }

  p_semaphore->initialized = 1;
  *semaphore = (rsi_semaphore_handle_t)p_semaphore;

  return RSI_ERROR_NONE;
}

/*==============================================*/
/**
 * @fn           rsi_error_t rsi_semaphore_destroy(rsi_semaphore_handle_t *semaphore)
 * @brief        Destroy the semaphore instance
 * @param[in]    semaphore - Semaphore handle pointer  
 * @return       0              - Success \n
 * @return              Negative Value - Failure
 */
rsi_error_t rsi_semaphore_destroy(rsi_semaphore_handle_t *semaphore)
{
  linux_semaphore_t *p_semaphore = NULL;
  
  if (semaphore == NULL || *semaphore == NULL) {
    return RSI_ERROR_INVALID_PARAM;
  }

  p_semaphore = (linux_semaphore_t *)(*semaphore);
  
  if (!p_semaphore->initialized) {
    return RSI_ERROR_INVALID_PARAM;
  }

  sem_destroy(&(p_semaphore->sem));
  p_semaphore->initialized = 0;
  free(p_semaphore);
  *semaphore = NULL;

  return RSI_ERROR_NONE;
}

/*==============================================*/
/**
 * @fn           rsi_error_t rsi_semaphore_check_and_destroy(rsi_semaphore_handle_t *semaphore)
 * @brief        Check whether the semaphore is created and destroy, if created.
 * @param[in]    semaphore - Semaphore handle pointer  
 * @return       0              - Success \n
 * @return       Negative Value - Failure  
 *
 */

rsi_error_t rsi_semaphore_check_and_destroy(rsi_semaphore_handle_t *semaphore)
{
  linux_semaphore_t *p_semaphore = NULL;
  
  if (semaphore == NULL || *semaphore == NULL) {
    return RSI_ERROR_NONE; // returning success since semaphore does not exists
  }

  p_semaphore = (linux_semaphore_t *)(*semaphore);
  
  if (!p_semaphore->initialized) {
    return RSI_ERROR_NONE; // returning success since semaphore does not exists
  }

  sem_destroy(&(p_semaphore->sem));
  p_semaphore->initialized = 0;
  free(p_semaphore);
  *semaphore = NULL;

  return RSI_ERROR_NONE;
}

/*==============================================*/
/**
 * @fn          rsi_error_t rsi_semaphore_wait(rsi_semaphore_handle_t *semaphore, uint32_t timeout_ms ) 
 * @brief       Wait for semaphore
 * @param[in]   semaphore - Semaphore handle pointer  
 * @param[in]   time_ms - Maximum time to wait to acquire semaphore. If timeout_ms is 0 then wait till semaphore is acquired.
 * @return      0              - Success \n 
 * @return      Negative Value - Failure 
 *
 */

rsi_error_t rsi_semaphore_wait(rsi_semaphore_handle_t *semaphore, uint32_t timeout_ms)
{
  linux_semaphore_t *p_semaphore = NULL;
  struct timespec timeout;
  int ret;

  if (semaphore == NULL || *semaphore == NULL) {
    return RSI_ERROR_INVALID_PARAM;
  }

  p_semaphore = (linux_semaphore_t *)(*semaphore);
  
  if (!p_semaphore->initialized) {
    return RSI_ERROR_INVALID_PARAM;
  }

  if (timeout_ms == 0) {
    // Wait indefinitely - retry on EINTR
    do {
      ret = sem_wait(&(p_semaphore->sem));
    } while (ret != 0 && errno == EINTR);
    
    if (ret != 0) {
      return RSI_ERROR_IN_OS_OPERATION;
    }
  } else {
    // Wait with timeout
    // Note: sem_timedwait on Linux uses CLOCK_REALTIME internally.
    // If system clock is adjusted, timeout behavior may be affected.
    // For more reliable timeouts, consider using CLOCK_MONOTONIC with
    // a custom timeout mechanism, but sem_timedwait requires CLOCK_REALTIME.
    clock_gettime(CLOCK_REALTIME, &timeout);
    timeout.tv_sec += timeout_ms / 1000;
    timeout.tv_nsec += (timeout_ms % 1000) * 1000000;
    if (timeout.tv_nsec >= 1000000000) {
      timeout.tv_sec += 1;
      timeout.tv_nsec -= 1000000000;
    }

    // Retry on EINTR - use same absolute timeout
    // Save errno before condition check to avoid race conditions
    do {
      ret = sem_timedwait(&(p_semaphore->sem), &timeout);
      if (ret != 0) {
        int saved_errno = errno;
        if (saved_errno != EINTR) {
          errno = saved_errno;  // Restore errno for error checking
          break;
        }
      }
    } while (ret != 0);
    
    if (ret != 0) {
      if (errno == ETIMEDOUT) {
        return RSI_ERROR_TIMEOUT;
      }
      return RSI_ERROR_IN_OS_OPERATION;
    }
  }

  return RSI_ERROR_NONE;
}

/*==============================================*/
/**
 * @fn          rsi_error_t rsi_semaphore_post(rsi_semaphore_handle_t *semaphore) 
 * @brief       Release semaphore, which is acquired
 * @param[in]   semaphore - Semaphore handle pointer  
 * @return      0              - Success \n 
 * @return      Negative Value - Failure 
 *
 */

rsi_error_t rsi_semaphore_post(rsi_semaphore_handle_t *semaphore)
{
  linux_semaphore_t *p_semaphore = NULL;
  
  if (semaphore == NULL || *semaphore == NULL) {
    return RSI_ERROR_INVALID_PARAM;
  }

  p_semaphore = (linux_semaphore_t *)(*semaphore);
  
  if (!p_semaphore->initialized) {
    return RSI_ERROR_INVALID_PARAM;
  }

  if (sem_post(&(p_semaphore->sem)) != 0) {
    return RSI_ERROR_IN_OS_OPERATION;
  }

  return RSI_ERROR_NONE;
}

/*====================================================*/
/**
 * @fn          rsi_error_t rsi_semaphore_post_from_isr(rsi_semaphore_handle_t *semaphore)
 * @brief       Release semaphore, which is acquired from ISR context
 * @param[in]   semphore - Semaphore handle pointer 
 * @return      0              - Success \n 
 * @return      Negative Value - Failure 
 *             
 */

rsi_error_t rsi_semaphore_post_from_isr(rsi_semaphore_handle_t *semaphore)
{
  linux_semaphore_t *p_semaphore = NULL;
  int ret; 
  if (semaphore == NULL || *semaphore == NULL) {
    return RSI_ERROR_INVALID_PARAM;
  }

  p_semaphore = (linux_semaphore_t *)(*semaphore);
  
  if (!p_semaphore->initialized) {
    return RSI_ERROR_INVALID_PARAM;
  }

  // Post from ISR context (same as normal post in Linux)
  // sem_post is async-signal-safe and can be called from signal handlers
  // Use memory barrier to ensure visibility of the semaphore state
  __sync_synchronize();
  
  ret = sem_post(&(p_semaphore->sem));
  if (ret != 0) {
    // sem_post can fail with EINVAL (invalid semaphore) or EOVERFLOW (value would exceed max)
    // Check errno only when sem_post fails
    if (errno == EOVERFLOW) {
      // Semaphore was incremented despite overflow, treat as success
      return RSI_ERROR_NONE;
    }
    // Other errors (EINVAL, etc.)
    return RSI_ERROR_IN_OS_OPERATION;
  }
  
  // Ensure the post operation is complete before returning
  __sync_synchronize();

  return RSI_ERROR_NONE;
}

/*==============================================*/
/*
 * @fn          rsi_error_t rsi_semaphore_reset(rsi_semaphore_handle_t *semaphore) 
 * @brief       Reset the semaphore
 * @param[in]   semphore - Semaphore handle pointer  
 * @return      0              - Success \n 
 * @return      Negative Value - Failure
 *
 */
rsi_error_t rsi_semaphore_reset(rsi_semaphore_handle_t *semaphore)
{
  linux_semaphore_t *p_semaphore = NULL;
  int value;
  uint32_t max_iterations = 1000;  // Prevent infinite loop
  uint32_t iteration = 0;
  
  if (semaphore == NULL || *semaphore == NULL) {
    return RSI_ERROR_INVALID_PARAM;
  }

  p_semaphore = (linux_semaphore_t *)(*semaphore);
  
  if (!p_semaphore->initialized) {
    return RSI_ERROR_INVALID_PARAM;
  }

  // Reset semaphore by waiting until it's 0, with iteration limit to prevent infinite loop
  // This handles the case where another thread continuously posts to the semaphore
  int getvalue_ret;
  while (iteration < max_iterations) {
    getvalue_ret = sem_getvalue(&(p_semaphore->sem), &value);
    if (getvalue_ret != 0 || value <= 0) {
      // sem_getvalue failed or semaphore is already 0
      break;
    }
    // Use trywait to avoid blocking indefinitely
    int trywait_ret = sem_trywait(&(p_semaphore->sem));
    if (trywait_ret != 0) {
      // Check errno only when trywait fails
      if (errno == EAGAIN) {
        // Semaphore is 0, we're done
        break;
      }
      // Other errors (EINVAL, etc.) - break to avoid infinite loop
      break;
    }
    iteration++;
  }

  return RSI_ERROR_NONE;
}

/*==============================================*/
/**
 * @fn           rsi_error_t rsi_task_create( rsi_task_function_t task_function,uint8_t *task_name,
                 uint32_t stack_size, void *parameters,
                 uint32_t task_priority,rsi_task_handle_t  *task_handle)
 * @brief        Create OS task/thread 
 * @param[in]    task_function - Pointer to function to be executed by created thread. \n
 * @param[in]    task_name     - Name of the created task  \n
 * @param[in]    stack_size    - Stack size given to the created task \n  
 * @param[in]    parameters    - Pointer to the parameters to be passed to task function \n
 * @param[in]    task_priority - Task priority \n
 * @param[in]    task_handle   - Task handle/instance created \n
 * @return       0              - Success \n
 * @return       Negative Value - Failure
 *
 */

// Thread wrapper function
static void *linux_task_wrapper(void *arg)
{
  linux_task_t *task_info = (linux_task_t *)arg;
  
  // Set thread-local storage to current task
  current_task_tls = task_info;
  
  if (task_info != NULL && task_info->task_function != NULL) {
    // Check for suspension before and during task execution
    pthread_mutex_lock(&(task_info->suspend_mutex));
    while (task_info->suspended && !task_info->should_exit) {
      pthread_cond_wait(&(task_info->suspend_cond), &(task_info->suspend_mutex));
    }
    pthread_mutex_unlock(&(task_info->suspend_mutex));
    
    if (!task_info->should_exit) {
      task_info->task_function(task_info->parameters);
    }
  }
  
  if (task_info != NULL) {
    task_info->running = 0;
    task_info->should_exit = 1;
  }
  
  // Clear thread-local storage
  current_task_tls = NULL;
  
  return NULL;
}

rsi_error_t rsi_task_create(rsi_task_function_t task_function,
                            uint8_t *task_name,
                            uint32_t stack_size,
                            void *parameters,
                            uint32_t task_priority,
                            rsi_task_handle_t *task_handle)
{
  linux_task_t *task_info = NULL;
  pthread_attr_t attr;
  int ret;
  struct sched_param sched_param;

#ifdef SAPIS_BT_STACK_ON_HOST
  task_priority = 7;
#endif

  if (task_function == NULL || task_handle == NULL) {
    return RSI_ERROR_INVALID_PARAM;
  }

  // Allocate memory for task structure
  task_info = (linux_task_t *)malloc(sizeof(linux_task_t));
  if (task_info == NULL) {
    return RSI_ERROR_IN_OS_OPERATION;
  }

  task_info->task_function = task_function;
  task_info->parameters = parameters;
  task_info->running = 1;
  task_info->should_exit = 0;
  task_info->suspended = 0;
  task_info->notification_value = 0;
  task_info->notification_count = 0;

  // Initialize suspend condition variable and mutex
  if (pthread_cond_init(&(task_info->suspend_cond), NULL) != 0) {
    free(task_info);
    return RSI_ERROR_IN_OS_OPERATION;
  }

  if (pthread_mutex_init(&(task_info->suspend_mutex), NULL) != 0) {
    pthread_cond_destroy(&(task_info->suspend_cond));
    free(task_info);
    return RSI_ERROR_IN_OS_OPERATION;
  }

  // Initialize notification semaphore
  if (sem_init(&(task_info->notify_sem), 0, 0) != 0) {
    free(task_info);
    return RSI_ERROR_IN_OS_OPERATION;
  }

  // Initialize notification mutex
  if (pthread_mutex_init(&(task_info->notify_mutex), NULL) != 0) {
    sem_destroy(&(task_info->notify_sem));
    free(task_info);
    return RSI_ERROR_IN_OS_OPERATION;
  }

  // Initialize thread attributes
  pthread_attr_init(&attr);

  // Set stack size
  if (stack_size > 0) {
    pthread_attr_setstacksize(&attr, stack_size);
  }

  // Set thread as detached or joinable (we use joinable for now)
  pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);

  // Set thread priority (Linux uses SCHED_OTHER, SCHED_FIFO, or SCHED_RR)
  // For normal priority threads, we use SCHED_OTHER
  if (task_priority > 50) {
    // High priority thread
    pthread_attr_setschedpolicy(&attr, SCHED_FIFO);
    sched_param.sched_priority = task_priority;
    pthread_attr_setschedparam(&attr, &sched_param);
    pthread_attr_setinheritsched(&attr, PTHREAD_EXPLICIT_SCHED);
  }

  // Create the thread
  ret = pthread_create(&(task_info->thread), &attr, linux_task_wrapper, (void *)task_info);
  pthread_attr_destroy(&attr);

  if (ret != 0) {
    // Clean up resources if thread creation fails
    pthread_mutex_destroy(&(task_info->suspend_mutex));
    pthread_cond_destroy(&(task_info->suspend_cond));
    pthread_mutex_destroy(&(task_info->notify_mutex));
    sem_destroy(&(task_info->notify_sem));
    free(task_info);
    return RSI_ERROR_IN_OS_OPERATION;
  }

  *task_handle = (rsi_task_handle_t)task_info;

  UNUSED_PARAMETER(task_name); // Task name is not used in pthread

  return RSI_ERROR_NONE;
}

/*==============================================*/
/**
 * @fn          void rsi_task_destroy(rsi_task_handle_t *task_handle)
 * @brief       Destroy a task created with rsi_task_create().
 * @param[in]   task_handle - Pass NULL to self-delete the calling task.
 *                            Pass the handle value returned by
 *                            rsi_task_create() to delete that task.
 * @return      Void
 *
 */

void rsi_task_destroy(rsi_task_handle_t *task_handle)
{
  linux_task_t *task = NULL;
  int ret;

  if (task_handle == NULL) {
    task = current_task_tls;
    if (task == NULL) {
      return;
    }

    pthread_detach(task->thread);

    sem_destroy(&(task->notify_sem));
    pthread_mutex_destroy(&(task->notify_mutex));
    pthread_cond_destroy(&(task->suspend_cond));
    pthread_mutex_destroy(&(task->suspend_mutex));

    current_task_tls = NULL;
    free(task);

    pthread_exit(NULL);
  }

  task = (linux_task_t *)task_handle;

  pthread_mutex_lock(&(task->suspend_mutex));
  task->should_exit = 1;
  task->suspended   = 0;
  pthread_cond_broadcast(&(task->suspend_cond));
  pthread_mutex_unlock(&(task->suspend_mutex));
  sem_post(&(task->notify_sem));

  if (pthread_equal(task->thread, pthread_self())) {
    pthread_detach(task->thread);
    return;
  }

  ret = pthread_join(task->thread, NULL);
  (void)ret;

  sem_destroy(&(task->notify_sem));
  pthread_mutex_destroy(&(task->notify_mutex));
  pthread_cond_destroy(&(task->suspend_cond));
  pthread_mutex_destroy(&(task->suspend_mutex));

  free(task);
}

/*==============================================*/
/**
 * @fn          void rsi_task_delete(rsi_task_handle_t *task_handle)
 * @brief       Delete the task created (alias for rsi_task_destroy)
 * @param[in]   task_handle - Task handle/instance to be deleted
 * @return      Void
 *
 */
void rsi_task_delete(rsi_task_handle_t *task_handle)
{
  rsi_task_destroy(task_handle);
}

/*==============================================*/
/**
 * @fn          void rsi_os_task_delay(uint32_t timeout_ms)
 * @brief       Induce required delay in milli seconds
 * @param[in]   timeout_ms - Expected delay in milli seconds
 * @return      Void 
 */
void rsi_os_task_delay(uint32_t timeout_ms)
{
  linux_task_t *current_task = current_task_tls;
  
  // Check for suspension before and during delay
  // This allows tasks to be suspended/resumed during delays
  if (current_task != NULL) {
    pthread_mutex_lock(&(current_task->suspend_mutex));
    while (current_task->suspended && !current_task->should_exit) {
      // If suspended, wait on condition variable (with timeout if needed)
      if (timeout_ms > 0) {
        struct timespec timeout_ts;
        clock_gettime(CLOCK_REALTIME, &timeout_ts);
        timeout_ts.tv_sec += timeout_ms / 1000;
        timeout_ts.tv_nsec += (timeout_ms % 1000) * 1000000;
        if (timeout_ts.tv_nsec >= 1000000000) {
          timeout_ts.tv_sec += 1;
          timeout_ts.tv_nsec -= 1000000000;
        }
        pthread_cond_timedwait(&(current_task->suspend_cond), &(current_task->suspend_mutex), &timeout_ts);
        break;  // Timeout or resumed
      } else {
        pthread_cond_wait(&(current_task->suspend_cond), &(current_task->suspend_mutex));
      }
    }
    pthread_mutex_unlock(&(current_task->suspend_mutex));
    
    // If not suspended or resumed, perform the delay
    if (!current_task->suspended && !current_task->should_exit) {
      // Convert milliseconds to microseconds for usleep
      usleep(timeout_ms * 1000);
    }
  } else {
    // No current task, just perform delay
    usleep(timeout_ms * 1000);
  }
}

/*==============================================*/
/**
 * @fn           void rsi_start_os_scheduler()
 * @brief        Schedule the tasks created
 * @param[in]    None
 * @return       Void  
 */
void rsi_start_os_scheduler()
{
  // In Linux, threads run automatically, so we just wait for all threads to finish
  // This function should be called from main and will wait indefinitely
  while (1) {
    sleep(1);
  }
}

/*==============================================*/
/**
 * @fn           void rsi_setup_timer_os_interrupt()
 * @brief        Setup the systick timer to generate the tick interrupts at the required frequency
 * @param[in]    None
 * @return       void
 */
void rsi_setup_timer_os_interrupt()
{
  // In Linux, we don't need to setup timer interrupts
  // The system timer is already available
}

/*==============================================*/
/**
 * @fn          void rsi_set_os_errno(int32_t error)
 * @brief       Sets the OS error .
 * @param[in]   error - Error
 * @return      Void 
 */
void rsi_set_os_errno(int32_t error)
{
  UNUSED_PARAMETER(error); // This statement is added only to resolve compilation warning, value is unchanged
  // In Linux, errno is set automatically by system calls
}

/*==============================================*/
/**
 * @fn          rsi_base_type_t rsi_task_notify_wait(uint32_t ulBitsToClearOnEntry, uint32_t ulBitsToClearOnExit, uint32_t *pulNotificationValue, uint32_t timeout)
 * @brief       Allow a task to wait with optional timeout
 * @param[in]   ulBitsToClearOnEntry - Bits set here will be cleared in the task's notification value on entry to the function.
 * @param[in]   ulBitsToClearOnExit  - Bits to clear on exit 
 * @param[in]   pulNotificationValue - Used to pass out the task notification value. Optional parameter
 * @param[in]   timeout              - Maximum amount of time the calling task should remain in blocked state (in milliseconds)
 * @return      0              - Success \n
 * @return      Negative Value - Failure
 */
rsi_base_type_t rsi_task_notify_wait(uint32_t ulBitsToClearOnEntry,
                                     uint32_t ulBitsToClearOnExit,
                                     uint32_t *pulNotificationValue,
                                     uint32_t timeout)
{
  linux_task_t *current_task = current_task_tls;
  struct timespec timeout_ts;
  int ret;

  UNUSED_PARAMETER(ulBitsToClearOnExit);  // Will be used after wait

  // If we have a current task, use its notification mechanism
  if (current_task != NULL) {
    // Clear bits on entry as per FreeRTOS semantics
    if (ulBitsToClearOnEntry != 0) {
      pthread_mutex_lock(&(current_task->notify_mutex));
      current_task->notification_value &= ~ulBitsToClearOnEntry;
      pthread_mutex_unlock(&(current_task->notify_mutex));
    }

    // Wait for notification with timeout
    if (timeout == 0) {
      // Wait indefinitely - retry on EINTR
      do {
        ret = sem_wait(&(current_task->notify_sem));
      } while (ret != 0 && errno == EINTR);
    } else {
      // Wait with timeout
      // Note: sem_timedwait on Linux uses CLOCK_REALTIME internally.
      // If system clock is adjusted, timeout behavior may be affected.
      clock_gettime(CLOCK_REALTIME, &timeout_ts);
      timeout_ts.tv_sec += timeout / 1000;
      timeout_ts.tv_nsec += (timeout % 1000) * 1000000;
      if (timeout_ts.tv_nsec >= 1000000000) {
        timeout_ts.tv_sec += 1;
        timeout_ts.tv_nsec -= 1000000000;
      }

      // Retry on EINTR - use same absolute timeout
      // Save errno before condition check to avoid race conditions
      do {
        ret = sem_timedwait(&(current_task->notify_sem), &timeout_ts);
        if (ret != 0) {
          int saved_errno = errno;
          if (saved_errno != EINTR) {
            errno = saved_errno;  // Restore errno for error checking
            break;
          }
        }
      } while (ret != 0);

      if (ret != 0) {
        if (errno == ETIMEDOUT) {
          return 0; // Timeout
        }
        return 0; // Error (return 0 for failure)
      }
    }

    // Return notification value if requested
    pthread_mutex_lock(&(current_task->notify_mutex));
    if (pulNotificationValue != NULL) {
      *pulNotificationValue = current_task->notification_value;
    }
    if (ulBitsToClearOnExit) {
      current_task->notification_value &= ~ulBitsToClearOnExit;
    }
    pthread_mutex_unlock(&(current_task->notify_mutex));

    return 1; // Success (return 1 like FreeRTOS pdTRUE)
  } else {
    // Fallback to global notification (for cases where TLS is not set)
    // Note: These static semaphores are never destroyed, but they persist for the
    // lifetime of the application. For proper cleanup, consider using atexit() or
    // application-level cleanup routines.
    static rsi_semaphore_handle_t notify_sem = NULL;
    static uint32_t notification_value = 0;
    static pthread_mutex_t notify_global_mutex = PTHREAD_MUTEX_INITIALIZER;
    rsi_error_t ret_err;

    // Create notification semaphore if not exists
    if (notify_sem == NULL) {
      pthread_mutex_lock(&notify_global_mutex);
      if (notify_sem == NULL) {
        ret_err = rsi_semaphore_create(&notify_sem, 0);
        if (ret_err != RSI_ERROR_NONE) {
          pthread_mutex_unlock(&notify_global_mutex);
          return -1;
        }
      }
      pthread_mutex_unlock(&notify_global_mutex);
    }

    // Clear bits on entry as per FreeRTOS semantics
    if (ulBitsToClearOnEntry != 0) {
      pthread_mutex_lock(&notify_global_mutex);
      notification_value &= ~ulBitsToClearOnEntry;
      pthread_mutex_unlock(&notify_global_mutex);
    }

    // Wait for notification with timeout
    if (timeout == 0) {
      ret_err = rsi_semaphore_wait(&notify_sem, 0);
    } else {
      ret_err = rsi_semaphore_wait(&notify_sem, timeout);
    }

    if (ret_err != RSI_ERROR_NONE) {
      if (ret_err == RSI_ERROR_TIMEOUT) {
        return 0; // Timeout
      }
      return 0; // Error (return 0 for failure)
    }

    // Return notification value if requested
    pthread_mutex_lock(&notify_global_mutex);
    if (pulNotificationValue != NULL) {
      *pulNotificationValue = notification_value;
    }
    if (ulBitsToClearOnExit) {
      notification_value &= ~ulBitsToClearOnExit;
    }
    pthread_mutex_unlock(&notify_global_mutex);

    return 1; // Success (return 1 like FreeRTOS pdTRUE)
  }
}

/*==============================================*/
/**
 * @fn          rsi_base_type_t rsi_task_notify(rsi_task_handle_t xTaskToNotify, uint32_t ulValue)
 * @brief       Notify a task
 * @param[in]   xTaskToNotify - The handle of the task to which the notification is being sent
 * @param[in]   ulValue       - How ulValue is used is dependent on the eNotifyAction value
 * @return      0  - Success \n
 * @return      Non-Zero Value - Failure
 */
rsi_base_type_t rsi_task_notify(rsi_task_handle_t xTaskToNotify, uint32_t ulValue)
{
  linux_task_t *task = NULL;
  // Note: These static semaphores are never destroyed, but they persist for the
  // lifetime of the application. For proper cleanup, consider using atexit() or
  // application-level cleanup routines.
  static rsi_semaphore_handle_t notify_sem = NULL;
  static uint32_t notification_value = 0;
  static pthread_mutex_t notify_global_mutex = PTHREAD_MUTEX_INITIALIZER;
  rsi_error_t ret;

  if (xTaskToNotify != NULL) {
    // Use per-task notification
    task = (linux_task_t *)xTaskToNotify;
    
    // Lock notification mutex
    pthread_mutex_lock(&(task->notify_mutex));
    
    // Set notification value (OR operation to set bits)
    task->notification_value |= ulValue;
    // Increment notification counter for notify_take semantics
    task->notification_count++;
    
    // Unlock notification mutex
    pthread_mutex_unlock(&(task->notify_mutex));
    
    // Post notification semaphore
    if (sem_post(&(task->notify_sem)) != 0) {
      return -1;
    }
    
    return 0; // Success
  } else {
    // Use global notification (for backward compatibility)
    pthread_mutex_lock(&notify_global_mutex);
    
    // Create notification semaphore if not exists
    if (notify_sem == NULL) {
      ret = rsi_semaphore_create(&notify_sem, 0);
      if (ret != RSI_ERROR_NONE) {
        pthread_mutex_unlock(&notify_global_mutex);
        return -1;
      }
    }
    
    // Set notification value
    notification_value |= ulValue;
    // Note: Global notification doesn't support counter semantics
    // For counter support, use per-task notifications
    
    // Post notification
    ret = rsi_semaphore_post(&notify_sem);
    pthread_mutex_unlock(&notify_global_mutex);
    
    if (ret != RSI_ERROR_NONE) {
      return -1;
    }
    
    return 0; // Success
  }
}

/*==============================================*/
/**
 * @fn          rsi_base_type_t rsi_task_notify_from_isr(rsi_task_handle_t xTaskToNotify, uint32_t ulValue, rsi_base_type_t *pxHigherPriorityTaskWoken )
 * @brief       Notify a task from ISR context
 * @param[in]	xTaskToNotify			- The handle of the task to which the notification is being sent
 * @param[in]	ulValue       			- How ulValue is used is dependent on the eNotifyAction value
 * @param[in]	pxHigherPriorityTaskWoken	- Sets,if sending the notification caused a task to unblock, and the unblocked task has a priority higher than the currently running task.
 * @return      0  - Success \n
 * @return      Non-Zero Value - Failure
 */
rsi_base_type_t rsi_task_notify_from_isr(rsi_task_handle_t xTaskToNotify,
                                         uint32_t ulValue,
                                         rsi_base_type_t *pxHigherPriorityTaskWoken)
{
  // Same as rsi_task_notify for Linux
  if (pxHigherPriorityTaskWoken != NULL) {
    *pxHigherPriorityTaskWoken = 0;
  }

  return rsi_task_notify(xTaskToNotify, ulValue);
}

/*==============================================*/
/**
 * @fn          uint32_t rsi_os_task_notify_take( BaseType_t xClearCountOnExit, TickType_t xTicksToWait )
 * @brief       Allow a task in wait in blocked state for its notification value
 * @param[in]   xClearCountOnExit - Based on this value calling task notification value will be decremented/zero
 * @param[in]   xTicksToWait      - Maximum amount of time calling task should remain in blocked state (in ticks, 1 tick = 1ms)
 * @return      Notification value received
 */
uint32_t rsi_os_task_notify_take(rsi_base_type_t xClearCountOnExit, uint32_t xTicksToWait)
{
  // Convert ticks to milliseconds
  uint32_t timeout_ms = xTicksToWait;
  linux_task_t *current_task = current_task_tls;
  uint32_t count = 0;

  // Use per-task notification if available (preferred)
  if (current_task != NULL) {
    // Wait for notification with timeout
    rsi_error_t ret = rsi_semaphore_wait(&(current_task->notify_sem), timeout_ms);
    if (ret != RSI_ERROR_NONE) {
      return 0;  // Timeout or error
    }

    // Get and update notification count (FreeRTOS ulTaskNotifyTake semantics)
    pthread_mutex_lock(&(current_task->notify_mutex));
    count = current_task->notification_count;
    if (xClearCountOnExit) {
      // Clear count (decrement to 0)
      current_task->notification_count = 0;
    } else {
      // Decrement count by 1
      if (current_task->notification_count > 0) {
        current_task->notification_count--;
      }
    }
    pthread_mutex_unlock(&(current_task->notify_mutex));

    return count;
  }

  // Fallback to global notification (for backward compatibility)
  // Note: These static semaphores are never destroyed, but they persist for the
  // lifetime of the application. For proper cleanup, consider using atexit() or
  // application-level cleanup routines.
  static rsi_semaphore_handle_t notify_sem = NULL;
  static uint32_t notification_count = 0;
  static pthread_mutex_t notify_global_mutex = PTHREAD_MUTEX_INITIALIZER;
  rsi_error_t ret;

  // Create notification semaphore if not exists
  pthread_mutex_lock(&notify_global_mutex);
  if (notify_sem == NULL) {
    ret = rsi_semaphore_create(&notify_sem, 0);
    if (ret != RSI_ERROR_NONE) {
      pthread_mutex_unlock(&notify_global_mutex);
      return 0;
    }
  }
  pthread_mutex_unlock(&notify_global_mutex);

  // Wait for notification
  ret = rsi_semaphore_wait(&notify_sem, timeout_ms);
  if (ret != RSI_ERROR_NONE) {
    return 0;
  }

  // Get and update notification count (FreeRTOS ulTaskNotifyTake semantics)
  pthread_mutex_lock(&notify_global_mutex);
  count = notification_count;
  if (xClearCountOnExit) {
    // Clear count (decrement to 0)
    notification_count = 0;
  } else {
    // Decrement count by 1
    if (notification_count > 0) {
      notification_count--;
    }
  }
  pthread_mutex_unlock(&notify_global_mutex);

  return count; // Return notification count
}

/*==============================================*/
/**
 * @fn          BaseType_t rsi_os_task_notify_give(rsi_task_handle_t xTaskToNotify)
 * @brief       Notify to a task
 * @param[in]   xTaskToNotify - Task handle to notify
 * @return      Task notification value - Success\n
 * @return      -1 - Failure
 */
rsi_base_type_t rsi_os_task_notify_give(rsi_task_handle_t xTaskToNotify)
{
  // Same as rsi_task_notify for Linux
  return rsi_task_notify(xTaskToNotify, 1);
}

#ifdef RSI_WLAN_ENABLE

/*==============================================*/
/**
 * @fn          int32_t rsi_get_error(int32_t sockID)
 * @brief       Get WLAN status
 * @param[in]   sockID - Socket ID
 * @return      WLAN status
 */
/// @private
int32_t rsi_get_error(int32_t sockID)
{
  // Validate socket ID to prevent array bounds violation
  if (sockID < 0 || sockID >= RSI_NUMBER_OF_SOCKETS || rsi_socket_pool_non_rom == NULL) {
    return RSI_SOCK_ERROR;
  }
  
  if (rsi_socket_pool_non_rom[sockID].socket_status == 0xFF87
      || rsi_socket_pool_non_rom[sockID].socket_status == 0xBB38) {
    return RSI_ERROR_ECONNREFUSED;
  } else if (rsi_socket_pool_non_rom[sockID].socket_status == 0x00D2) {
    return RSI_ERROR_ECONNABORTED;
  } else if (rsi_socket_pool_non_rom[sockID].socket_status == 0xFF6C
             || rsi_socket_pool_non_rom[sockID].socket_status == 0xFF6A
             || rsi_socket_pool_non_rom[sockID].socket_status == RSI_ERROR_RESPONSE_TIMEOUT
             || rsi_socket_pool_non_rom[sockID].socket_status == 0xBBD6) {
    return RSI_ERROR_ETIMEDOUT;
  } else if (rsi_socket_pool_non_rom[sockID].socket_status == 0x0021
             || rsi_socket_pool_non_rom[sockID].socket_status == 0xFF82
             || rsi_socket_pool_non_rom[sockID].socket_status == 0xFF74
             || rsi_socket_pool_non_rom[sockID].socket_status == 0xBB27
             || rsi_socket_pool_non_rom[sockID].socket_status == 0xFF80
             || rsi_socket_pool_non_rom[sockID].socket_status == 0x00D1) {
    return RSI_ERROR_EACCES;
  } else if (rsi_socket_pool_non_rom[sockID].socket_status == 0x00D3
             || rsi_socket_pool_non_rom[sockID].socket_status == 0xFFFE) {
    return RSI_ERROR_EPERM;
  } else if (rsi_socket_pool_non_rom[sockID].socket_status == 0xFF7E
             || rsi_socket_pool_non_rom[sockID].socket_status == 0xBBED
             || rsi_socket_pool_non_rom[sockID].socket_status == 0x003E
             || rsi_socket_pool_non_rom[sockID].socket_status == 0x0030
             || rsi_socket_pool_non_rom[sockID].socket_status == 0xBB3E) {
    return RSI_ERROR_EMSGSIZE;
  } else if (rsi_socket_pool_non_rom[sockID].socket_status == 0xFF86) {
    return RSI_ERROR_EBADF;
  } else if ((rsi_socket_pool_non_rom[sockID].socket_status == 0x0015)
             || (rsi_socket_pool_non_rom[sockID].socket_status == 0x0025)) {
    return RSI_ERROR_EAGAIN;
  } else if (rsi_socket_pool_non_rom[sockID].socket_status == 0xBB22
             || rsi_socket_pool_non_rom[sockID].socket_status == 0xBB42) {
    return RSI_ERROR_EISCONN;
  } else if (rsi_socket_pool_non_rom[sockID].socket_status == 0xFFFF
             || rsi_socket_pool_non_rom[sockID].socket_status == 0xBB50) {
    return RSI_ERROR_ENOTCONN;
  } else if (rsi_socket_pool_non_rom[sockID].socket_status == 0xBB33
             || rsi_socket_pool_non_rom[sockID].socket_status == 0xBB23
             || rsi_socket_pool_non_rom[sockID].socket_status == 0xBB45
             || rsi_socket_pool_non_rom[sockID].socket_status == 0xBB46
             || rsi_socket_pool_non_rom[sockID].socket_status == 0xBB50) {
    return RSI_ERROR_EADDRINUSE;
  } else if (rsi_socket_pool_non_rom[sockID].socket_status == 0xBB36) {
    return RSI_ERROR_ENOTSOCK;
  } else if (rsi_socket_pool_non_rom[sockID].socket_status == RSI_ERROR_PKT_ALLOCATION_FAILURE) {
    return RSI_ERROR_ENOMEM;
  } else
    return RSI_SOCK_ERROR;
}

#endif

/*==============================================*/
/**
 * @fn          void *rsi_malloc (uint32_t size)
 * @brief       Allocate memory from the buffer which is maintained by OS.	
 * @param[in]   size - Required bytes in size
 * @return      Void pointer to allocated memory
 */
void *rsi_malloc(uint32_t size)
{
  if (size == 0) {
    return NULL;
  } else {
    return malloc(size);
  }
}

/*==============================================*/
/**
 * @fn          void rsi_free (void *ptr)
 * @brief       Free the memory pointed by 'ptr'
 * @param[in]   ptr - starting address of the memory to be freed
 * @return      Void
 */
void rsi_free(void *ptr)
{
  if (ptr == NULL) {
    // do nothing
  } else {
    free(ptr);
  }
}
/*==============================================*/
/**
 * @fn          void rsi_vport_enter_critical(void)
 * @brief       Enter into critical section 
 * @param[in]   void 
 * @return      Void  
 */
void rsi_vport_enter_critical(void)
{
  // Initialize mutex once (thread-safe)
  pthread_once(&critical_mutex_once, init_critical_mutex_once);
  // Use the same critical section mutex as rsi_critical_section_entry
  pthread_mutex_lock(&critical_mutex);
}

/*==============================================*/
/**
 * @fn         void rsi_vport_exit_critical(void)
 * @brief       Exit critical section 
 * @param[in]   void 
 * @return      Void 
 */

void rsi_vport_exit_critical(void)
{
  // Initialize mutex once (thread-safe) - defensive check
  pthread_once(&critical_mutex_once, init_critical_mutex_once);
  // Use the same critical section mutex as rsi_critical_section_exit
  pthread_mutex_unlock(&critical_mutex);
}

/*==============================================*/
/**
 * @fn          void rsi_task_suspend(rsi_task_handle_t *task_handle)
 * @brief       Suspend a task
 * @param[in]   task_handle - Task handle to be suspended
 * @return      Void 
 */
void rsi_task_suspend(rsi_task_handle_t *task_handle)
{
  linux_task_t *task = NULL;
  
  if (task_handle == NULL || *task_handle == NULL) {
    return;
  }

  task = (linux_task_t *)(*task_handle);
  
  // Suspend the task using condition variable
  // This matches FreeRTOS vTaskSuspend behavior
  // Note: In Linux, true thread suspension requires the task to periodically
  // check the suspended flag. The task wrapper checks before starting,
  // and tasks should call OS functions (like rsi_os_task_delay) that check
  // for suspension to ensure proper suspend behavior.
  pthread_mutex_lock(&(task->suspend_mutex));
  task->suspended = 1;
  pthread_mutex_unlock(&(task->suspend_mutex));
}

/*==============================================*/
/**
 * @fn          void rsi_task_resume(rsi_task_handle_t *task_handle)
 * @brief       Resume a suspended task
 * @param[in]   task_handle - Task handle to be resumed
 * @return      Void 
 */
void rsi_task_resume(rsi_task_handle_t *task_handle)
{
  linux_task_t *task = NULL;
  
  if (task_handle == NULL || *task_handle == NULL) {
    return;
  }

  task = (linux_task_t *)(*task_handle);
  
  // Resume the task by clearing suspend flag and signaling condition variable
  // This matches FreeRTOS vTaskResume behavior
  pthread_mutex_lock(&(task->suspend_mutex));
  task->suspended = 0;
  pthread_cond_broadcast(&(task->suspend_cond));
  pthread_mutex_unlock(&(task->suspend_mutex));
}

/*==============================================*/
/**
 * @fn          void rsi_wireless_driver_task_create(void)
 * @brief       Create wireless driver task (empty implementation - application should implement)
 * @param[in]   None
 * @return      Void
 */
void rsi_wireless_driver_task_create(void)
{
  // This function is typically implemented in the application code
  // For Linux, it's up to the application to create the driver task if needed
  // Empty implementation here
}

#endif
#endif
/** @} */
