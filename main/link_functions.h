/**
 * @file link_functions.h
 * @author Shoma Gane <shomagan@gmail.com>
 * @defgroup free_rtos/inc
 * @ingroup free_rtos/inc
 * @version 0.1
 * @brief structure with function pointers to link os functions and other functions to user code without direct linking
 */
/*
 * Copyright (c) 2018
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
#ifndef LINK_FUNCTIONS_H
#define LINK_FUNCTIONS_H 1

#include "type_def.h"
#include "os_type.h"
#include "regs_description.h"
#include "memory_handle.h"
#define OS_VERSION_SIZE 4

#ifdef __cplusplus
extern "C" {
#endif

typedef struct MCU_PACK {
   u32 (*os_kernel_sys_tick)(void);
   u32 (*os_kernel_get_time_ms)(void);
   BaseType_t (*os_thread_create)(TaskFunction_t function, const char *name, uint32_t stack_size, void *param, UBaseType_t prio,
                                  TaskHandle_t *handler);
   TaskHandle_t (*os_thread_get_id)(void);
   TaskHandle_t (*os_thread_get_id_by_name)(const char *name);
   void (*os_thread_terminate)(TaskHandle_t handler);
   void (*os_thread_yield)(void);
   void (*os_thread_set_priority)(TaskHandle_t task, UBaseType_t prio);
   UBaseType_t (*os_thread_get_priority)(TaskHandle_t task);
   void (*os_thread_delay)(u32 ms);
   void (*os_thread_delay_until)(TickType_t *timer, u32 ms);
   BaseType_t (*os_thread_signal_set)(TaskHandle_t thread_id, uint32_t signal, uint32_t *prev_value);
   BaseType_t (*os_thread_signal_clear)(TaskHandle_t thread_id);
   BaseType_t (*os_thread_signal_wait)(uint32_t flags, uint32_t *signal, u32 ms);
   SemaphoreHandle_t (*os_mutex_create)(void);
   SemaphoreHandle_t (*os_semaphore_create)(UBaseType_t max_count, UBaseType_t initial_count);
   SemaphoreHandle_t (*os_semaphore_bin_create)(void);
   BaseType_t (*os_semaphore_wait)(SemaphoreHandle_t mutex, TickType_t time_ms);
   BaseType_t (*os_semaphore_release)(SemaphoreHandle_t mutex);
   void (*os_semaphore_delete)(SemaphoreHandle_t mutex);
   UBaseType_t (*os_semaphore_get_count)(SemaphoreHandle_t sem);
   os_pool_cb_t *(*os_pool_create)(const os_pool_def_t *pool_def);
   void *(*os_pool_alloc)(os_pool_cb_t *pool_id);
   void *(*os_pool_c_alloc)(os_pool_cb_t *pool_id);
   int (*os_pool_free)(os_pool_cb_t *pool_id, void *block);
   void *(*os_pool_get_by_index)(os_pool_cb_t *pool_id, u32 index);
   queue_handle_t (*os_message_create)(u32 queue_sz);
   BaseType_t (*os_message_put)(queue_handle_t queue, void *info, u32 ms);
   BaseType_t (*os_message_get)(queue_handle_t queue, void *info, u32 ms);
   eTaskState (*os_thread_get_state)(TaskHandle_t task);
   bool (*os_thread_is_suspended)(TaskHandle_t task);
   void (*os_thread_suspend)(TaskHandle_t task);
   void (*os_thread_resume)(TaskHandle_t task);
   void (*os_thread_suspend_all)(void);
   BaseType_t (*os_thread_resume_all)(void);
   BaseType_t (*os_abort_delay)(TaskHandle_t task);
   BaseType_t (*os_message_peek)(queue_handle_t queue, void *info, u32 ms);
   UBaseType_t (*os_message_waiting)(queue_handle_t queue);
   UBaseType_t (*os_message_available_space)(queue_handle_t queue);
   void (*os_message_delete)(queue_handle_t queue);
   SemaphoreHandle_t (*os_recursive_mutex_create)(void);
   BaseType_t (*os_recursive_mutex_release)(SemaphoreHandle_t mutex);
   BaseType_t (*os_recursive_mutex_wait)(SemaphoreHandle_t mutex, TickType_t time_ms);
   void (*task_enter_critical)(void);
   void (*task_exit_critical)(void);
   void (*refresh_watchdog)(void);
   int (*printf)(const char *format, ...);
   int (*os_log_debug)(const char *tag, const char *format, ...);
   int (*os_log_info)(const char *tag, const char *format, ...);
   int (*os_log_error)(const char *tag, const char *format, ...);
   int (*regs_description_list_add_new)(regs_description_list_t regs_table);
   void (*preinit_table_vars)(u16 table_ind);
   u8 version[OS_VERSION_SIZE];
} link_functions_t;
extern const link_functions_t link_functions;

#ifdef __cplusplus
}
#endif
#endif //LINK_FUNCTIONS_H
