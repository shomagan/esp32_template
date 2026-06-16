
/*
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 * 3. The name of the author may not be used to endorse or promote products
 *    derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT
 * SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT
 * OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING
 * IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY
 * OF SUCH DAMAGE.
 *
 *
 * Author: Shoma Gane <shomagan@gmail.com>
 */
#ifndef LINK_FUNCTIONS_C
#define LINK_FUNCTIONS_C 1
#include "cmsis_os.h"
#include "main.h"
#include "link_functions.h"
#define OS_VERSION {0,0,0,1}
const link_functions_t link_functions SOFI_LINK_CODE = {
  .os_kernel_sys_tick = osKernelSysTick,
  .os_thread_create =  osThreadCreate,
  .os_thread_get_id =  osThreadGetId,
  .os_thread_get_id_by_name = os_thread_get_id_by_name,
  .os_thread_terminate =  osThreadTerminate,
  .os_thread_yield =  osThreadYield,
  .os_thread_set_priority =  osThreadSetPriority,
  .os_thread_get_priority =  osThreadGetPriority,
  .os_delay =  osDelay,
  .os_delay_until =  osDelayUntil,
  .os_signal_set =  osSignalSet,
  .os_signal_clear =  osSignalClear,
  .os_signal_wait =  osSignalWait,
  .os_mutex_create =  osMutexCreate,
  .os_mutex_wait =  osMutexWait,
  .os_mutex_release =  osMutexRelease,
  .os_mutex_delete =  osMutexDelete,
  .os_semaphore_create =  osSemaphoreCreate,
  .os_semaphore_wait =  osSemaphoreWait,
  .os_semaphore_release =  osSemaphoreRelease,
  .os_semaphore_delete =  osSemaphoreDelete,
  .os_pool_create =  osPoolCreate,
  .os_pool_alloc =  osPoolAlloc,
  .os_pool_c_alloc =  osPoolCAlloc,
  .os_pool_free =  osPoolFree,
  .os_pool_get_by_index = os_pool_get_by_index,
  .os_message_create =  osMessageCreate,
  .os_message_put =  osMessagePut,
  .os_message_get =  osMessageGet,
  .os_mail_create =  osMailCreate,
  .os_mail_alloc =  osMailAlloc,
  .os_mail_c_alloc =  osMailCAlloc,
  .os_mail_put =  osMailPut,
  .os_mail_get =  osMailGet,
  .os_mail_free =  osMailFree,
  .os_thread_get_state =  osThreadGetState,
  .os_thread_is_suspended =  osThreadIsSuspended,
  .os_thread_suspend =  osThreadSuspend,
  .os_thread_resume =  osThreadResume,
  .os_thread_suspend_all =  osThreadSuspendAll,
  .os_thread_resume_all =  osThreadResumeAll,
  .os_abort_delay =  osAbortDelay,
  .os_thread_list =  osThreadList,
  .os_message_peek =  osMessagePeek,
  .os_message_waiting =  osMessageWaiting,
  .os_message_available_space =  osMessageAvailableSpace,
  .os_message_delete =  osMessageDelete,
  .os_recursive_mutex_create =  osRecursiveMutexCreate,
  .os_recursive_mutex_release =  osRecursiveMutexRelease,
  .os_recursive_mutex_wait =  osRecursiveMutexWait,
  .os_semaphore_get_count =  osSemaphoreGetCount,
  .task_enter_critical =  task_enter_critical,
  .task_exit_critical =  task_exit_critical,
  .refresh_watchdog =  refresh_watchdog,
  .printf = debug_printf,
  .version = OS_VERSION,
};
#endif //LINK_FUNCTIONS_C
