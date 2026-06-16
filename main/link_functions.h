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

/*add includes below */
#include "type_def.h"
#include "main_debug.h"
#include "main_config.h"
#include "cmsis_os.h"
#include "task_control.h"
#include "internal_flash.h"
#define OS_VERSION_SIZE 4
/*add includes before */
#ifdef __cplusplus
   extern "C" {
#endif

typedef struct MCU_PACK {
    uint32_t (*os_kernel_sys_tick) (void);
    osThreadId (*os_thread_create) (const osThreadDef_t *thread_def, void *argument);
    osThreadId (*os_thread_get_id) (void);
    osThreadId (*os_thread_get_id_by_name) (const char *name);
    osStatus (*os_thread_terminate) (osThreadId thread_id);
    osStatus (*os_thread_yield) (void);
    osStatus (*os_thread_set_priority) (osThreadId thread_id, osPriority priority);
    osPriority (*os_thread_get_priority) (osThreadId thread_id);
    osStatus (*os_delay) (uint32_t millisec);
    osStatus (*os_delay_until) (uint32_t *PreviousWakeTime, uint32_t millisec);
    int32_t (*os_signal_set) (osThreadId thread_id, int32_t signals);
    int32_t (*os_signal_clear) (osThreadId thread_id, int32_t signals);
    osEvent (*os_signal_wait) (int32_t signals, uint32_t millisec);
    osMutexId (*os_mutex_create) (const osMutexDef_t *mutex_def);
    osStatus (*os_mutex_wait) (osMutexId mutex_id, uint32_t millisec);
    osStatus (*os_mutex_release) (osMutexId mutex_id);
    osStatus (*os_mutex_delete) (osMutexId mutex_id);
    osSemaphoreId (*os_semaphore_create) (const osSemaphoreDef_t *semaphore_def, int32_t count);
    int32_t (*os_semaphore_wait) (osSemaphoreId semaphore_id, uint32_t millisec);
    osStatus (*os_semaphore_release) (osSemaphoreId semaphore_id);
    osStatus (*os_semaphore_delete) (osSemaphoreId semaphore_id);
    osPoolId (*os_pool_create) (const osPoolDef_t *pool_def);
    void * (*os_pool_alloc) (osPoolId pool_id);
    void * (*os_pool_c_alloc) (osPoolId pool_id);
    osStatus (*os_pool_free) (osPoolId pool_id, void *block);
    void * (*os_pool_get_by_index)(osPoolId pool_id,u32 index);
    osMessageQId (*os_message_create) (const osMessageQDef_t *queue_def, osThreadId thread_id);
    osStatus (*os_message_put) (osMessageQId queue_id, uint32_t info, uint32_t millisec);
    osEvent (*os_message_get) (osMessageQId queue_id, uint32_t millisec);
    osMailQId (*os_mail_create) (osMailQDef_t *queue_def, osThreadId thread_id);
    void * (*os_mail_alloc) (osMailQId queue_id, uint32_t millisec);
    void * (*os_mail_c_alloc) (osMailQId queue_id, uint32_t millisec);
    osStatus (*os_mail_put) (osMailQId queue_id, void *mail);
    osEvent (*os_mail_get) (osMailQId queue_id, uint32_t millisec);
    osStatus (*os_mail_free) (osMailQId queue_id, void *mail);
    osThreadState (*os_thread_get_state)(osThreadId thread_id);
    osStatus (*os_thread_is_suspended)(osThreadId thread_id);
    osStatus (*os_thread_suspend) (osThreadId thread_id);
    osStatus (*os_thread_resume) (osThreadId thread_id);
    osStatus (*os_thread_suspend_all) (void);
    osStatus (*os_thread_resume_all) (void);
    osStatus (*os_abort_delay) (osThreadId thread_id);
    osStatus (*os_thread_list) (uint8_t *buffer);
    osEvent (*os_message_peek) (osMessageQId queue_id, uint32_t millisec);
    uint32_t (*os_message_waiting) (osMessageQId queue_id);
    uint32_t (*os_message_available_space)(osMessageQId queue_id);
    osStatus (*os_message_delete) (osMessageQId queue_id);
    osMutexId (*os_recursive_mutex_create) (const osMutexDef_t *mutex_def);
    osStatus (*os_recursive_mutex_release) (osMutexId mutex_id);
    osStatus (*os_recursive_mutex_wait) (osMutexId mutex_id, uint32_t millisec);
    uint32_t (*os_semaphore_get_count) (osSemaphoreId semaphore_id);
    void (*task_enter_critical)(void);
    void (*task_exit_critical)(void);
    void (*refresh_watchdog)(void);
    int (*printf)(const char * format, ...);
    u8 version[OS_VERSION_SIZE];
} link_functions_t;
extern  const link_functions_t link_functions;

/*add functions and variable declarations before */
#ifdef __cplusplus
}
#endif
#endif //LINK_FUNCTIONS_H
