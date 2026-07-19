
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
#include "link_functions.h"
#include "regs_description.h"
#include "memory_handle.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "freertos/queue.h"
#include <stdio.h>

#define OS_VERSION {0,0,0,1}

/* task_enter_critical/task_exit_critical must share one spinlock across the whole
 * app, so unlike the rest of link_functions they can't be static inline in os_type.h
 * (each translation unit would get its own copy of the lock). Defined here instead,
 * next to their only user. */
static portMUX_TYPE link_functions_critical_mux = portMUX_INITIALIZER_UNLOCKED;
static void task_enter_critical(void) {
    portENTER_CRITICAL(&link_functions_critical_mux);
}
static void task_exit_critical(void) {
    portEXIT_CRITICAL(&link_functions_critical_mux);
}

const link_functions_t link_functions = {
  .os_kernel_sys_tick = task_get_tick_count,
  .os_kernel_get_time_ms = task_get_time_ms,
  .os_thread_create =  task_create,
  .os_thread_get_id =  task_get_id,
  .os_thread_get_id_by_name = task_get_handle,
  .os_thread_terminate =  task_delete,
  .os_thread_yield =  os_yield,
  .os_thread_set_priority =  task_set_priority,
  .os_thread_get_priority =  task_get_priority,
  .os_thread_delay =  task_delay_ms,
  .os_thread_delay_until =  task_delay_until_ms,
  .os_thread_signal_set =  task_notify_send,
  .os_thread_signal_clear =  task_notify_state_clear,
  .os_thread_signal_wait =  task_notify_wait,
  .os_mutex_create =  mutex_create,
  .os_semaphore_create =  semaphore_create,
  .os_semaphore_bin_create =  semaphore_create_binary,
  .os_semaphore_wait =  semaphore_take,
  .os_semaphore_release =  semaphore_release,
  .os_semaphore_delete =  semaphore_delete,
  .os_semaphore_get_count =  os_semaphore_get_count,
  .os_pool_create =  os_pool_create,
  .os_pool_alloc =  os_pool_alloc,
  .os_pool_c_alloc =  os_pool_calloc,
  .os_pool_free =  os_pool_free,
  .os_pool_get_by_index = os_pool_get_by_index,
  .os_message_create =  os_message_create,
  .os_message_put =  os_message_put,
  .os_message_get =  os_message_get,
  .os_thread_get_state =  os_thread_get_state,
  .os_thread_is_suspended =  os_thread_is_suspended,
  .os_thread_suspend =  os_thread_suspend,
  .os_thread_resume =  os_thread_resume,
  .os_thread_suspend_all =  os_thread_suspend_all,
  .os_thread_resume_all =  os_thread_resume_all,
  .os_abort_delay =  os_abort_delay,
  .os_message_peek =  os_message_peek,
  .os_message_waiting =  os_message_waiting,
  .os_message_available_space =  os_message_available_space,
  .os_message_delete =  os_message_delete,
  .os_recursive_mutex_create =  os_recursive_mutex_create,
  .os_recursive_mutex_release =  os_recursive_mutex_release,
  .os_recursive_mutex_wait =  os_recursive_mutex_wait,
  .task_enter_critical =  task_enter_critical,
  .task_exit_critical =  task_exit_critical,
  .refresh_watchdog =  refresh_watchdog,
  .printf = os_printf,
  .os_log_debug = os_log_debug,
  .os_log_info = os_log_info,
  .os_log_error = os_log_error,
  .regs_description_list_add_new = regs_description_list_add_new,
  .version = OS_VERSION
};
#endif //LINK_FUNCTIONS_C
