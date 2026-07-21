/**
 * @file os_type.h
 * @author Shoma Gane <shomagan@gmail.com>
 *         Ayrat Girfanov <girfanov.ayrat@yandex.ru>
 * @defgroup ../main/
 * @ingroup ../main/
 * @version 0.1
 * @brief  TODO!!! write brief in
 */
/*
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
 * This file is part of the sofi PLC.
 *
 * Author: Shoma Gane <shomagan@gmail.com>
 *         Ayrat Girfanov <girfanov.ayrat@yandex.ru>
 */
#ifndef OS_TYPE_H
#define OS_TYPE_H 1

/*add includes below */
#include "type_def.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "freertos/semphr.h"
#include "esp_log.h"
#include "esp_task_wdt.h"
#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>

/*add includes before */
#ifdef __cplusplus
extern "C" {
#endif
/*add functions and variable declarations below */
#define main_debug ESP_LOGD
#define main_printf ESP_LOGI
#define main_error_message ESP_LOGE
#define task_handle_t TaskHandle_t
static inline void task_delay_until_ms(TickType_t *timer, u32 ms) {
   vTaskDelayUntil(timer, pdMS_TO_TICKS(ms));
}
static inline void task_delay_ms(u32 ms) {
   vTaskDelay(ms / portTICK_PERIOD_MS);
}
static inline TickType_t task_get_tick_count(void) {
   return xTaskGetTickCount();
}
static inline u32 task_get_time_ms(void) {
   return pdTICKS_TO_MS(task_get_tick_count());
}

#define mutex_handle_t SemaphoreHandle_t
static inline SemaphoreHandle_t mutex_create(void) {
   return xSemaphoreCreateMutex();
}
#define semaphore_handle_t SemaphoreHandle_t
static inline SemaphoreHandle_t semaphore_create(UBaseType_t max_count, UBaseType_t initial_count) {
   return xSemaphoreCreateCounting(max_count, initial_count);
}
static inline SemaphoreHandle_t semaphore_create_binary(void) {
   return xSemaphoreCreateBinary();
}
static inline void semaphore_delete(SemaphoreHandle_t mutex) {
   vSemaphoreDelete(mutex);
}
static inline BaseType_t semaphore_take(SemaphoreHandle_t mutex, TickType_t time_ms) {
   return mutex != NULL ? xSemaphoreTake(mutex, time_ms) : pdFALSE;
}
static inline BaseType_t semaphore_release(SemaphoreHandle_t x) {
   return x != NULL ? xSemaphoreGive(x) : pdFALSE;
}
#define semaphore_release_ISR(sem, res) xSemaphoreGiveFromISR(sem, res)
static inline UBaseType_t os_semaphore_get_count(SemaphoreHandle_t sem) {
   return uxSemaphoreGetCount(sem);
}
static inline SemaphoreHandle_t os_recursive_mutex_create(void) {
   return xSemaphoreCreateRecursiveMutex();
}
static inline BaseType_t os_recursive_mutex_release(SemaphoreHandle_t mutex) {
   return xSemaphoreGiveRecursive(mutex);
}
static inline BaseType_t os_recursive_mutex_wait(SemaphoreHandle_t mutex, TickType_t time_ms) {
   return xSemaphoreTakeRecursive(mutex, time_ms);
}

#define queue_handle_t xQueueHandle
#define queue_create(queue_len, item_len) xQueueCreate(queue_len, item_len)
#define queue_receive(queue, msg, ms) xQueueReceive(queue, msg, pdMS_TO_TICKS(ms))
#define queue_send(queue, msg, ms) xQueueSend(queue, msg, pdMS_TO_TICKS(ms))
/* generic pointer-sized message queue, backing os_message_* in link_functions_t */
static inline queue_handle_t os_message_create(u32 queue_sz) {
   return xQueueCreate(queue_sz, sizeof(void *));
}
static inline BaseType_t os_message_put(queue_handle_t queue, void *info, u32 ms) {
   return xQueueSend(queue, &info, pdMS_TO_TICKS(ms));
}
static inline BaseType_t os_message_get(queue_handle_t queue, void *info, u32 ms) {
   return xQueueReceive(queue, info, pdMS_TO_TICKS(ms));
}
static inline BaseType_t os_message_peek(queue_handle_t queue, void *info, u32 ms) {
   return xQueuePeek(queue, info, pdMS_TO_TICKS(ms));
}
static inline UBaseType_t os_message_waiting(queue_handle_t queue) {
   return uxQueueMessagesWaiting(queue);
}
static inline UBaseType_t os_message_available_space(queue_handle_t queue) {
   return uxQueueSpacesAvailable(queue);
}
static inline void os_message_delete(queue_handle_t queue) {
   vQueueDelete(queue);
}
static inline BaseType_t task_create(TaskFunction_t function, const char *name, uint32_t stack_size, void *param, UBaseType_t prio,
                                     TaskHandle_t *handler) {
   return xTaskCreate(function, name, stack_size, param, prio, handler);
}
static inline void task_delete(TaskHandle_t handler) {
   vTaskDelete(handler);
}
#define task_get_id xTaskGetCurrentTaskHandle
static inline TaskHandle_t task_get_handle(const char *name) {
   return xTaskGetHandle(name);
}
static inline void task_set_priority(TaskHandle_t task, UBaseType_t prio) {
   vTaskPrioritySet(task, prio);
}
static inline UBaseType_t task_get_priority(TaskHandle_t task) {
   return uxTaskPriorityGet(task);
}
static inline void os_yield(void) {
   taskYIELD();
}
static inline eTaskState os_thread_get_state(TaskHandle_t task) {
   return eTaskGetState(task);
}
static inline bool os_thread_is_suspended(TaskHandle_t task) {
   return eTaskGetState(task) == eSuspended;
}
static inline void os_thread_suspend(TaskHandle_t task) {
   vTaskSuspend(task);
}
static inline void os_thread_resume(TaskHandle_t task) {
   vTaskResume(task);
}
static inline void os_thread_suspend_all(void) {
   vTaskSuspendAll();
}
static inline BaseType_t os_thread_resume_all(void) {
   return xTaskResumeAll();
}
static inline BaseType_t os_abort_delay(TaskHandle_t task) {
   return xTaskAbortDelay(task);
}
/* BaseType_t xTaskNotifyWait( uint32_t ulBitsToClearOnEntry,
                                uint32_t ulBitsToClearOnExit,
                                uint32_t * pulNotificationValue,
                                TickType_t xTicksToWait )   */
static inline BaseType_t task_notify_wait(uint32_t flags, uint32_t *signal, u32 ms) {
   return xTaskNotifyWait(0, flags, signal, ms / portTICK_PERIOD_MS);
}
static inline BaseType_t task_notify_send(TaskHandle_t thread_id, uint32_t signal, uint32_t *prev_value) {
   if (thread_id == NULL) {
      return pdFALSE;
   }
   return xTaskGenericNotify(thread_id, 0, signal, eSetBits, prev_value);
}
/* BaseType_t xTaskGenericNotifyFromISR( TaskHandle_t xTaskToNotify,
             UBaseType_t uxIndexToNotify,
             uint32_t ulValue,
             eNotifyAction eAction,
             uint32_t * pulPreviousNotificationValue,
             BaseType_t * pxHigherPriorityTaskWoken )*/
#define task_notify_send_isr_bits(thread_id, index, signal, prev_value, higher_priority_task_woken)                               \
   xTaskGenericNotifyFromISR(thread_id, index, (uint32_t)signal, eSetBits, prev_value, higher_priority_task_woken)
#define task_notify_send_isr_overwrite(thread_id, index, signal, prev_value, higher_priority_task_woken)                          \
   xTaskGenericNotifyFromISR(thread_id, index, (uint32_t)signal, eSetValueWithOverwrite, prev_value, higher_priority_task_woken)
static inline BaseType_t task_notify_state_clear(TaskHandle_t thread_id) {
   return xTaskNotifyStateClear(thread_id);
}
#define os_enter_critical(mutex) portENTER_CRITICAL(mutex)
#define os_exit_critical(mutex) portEXIT_CRITICAL(mutex)
#define WAIT_MAX_DELAY portMAX_DELAY
static inline int os_printf(const char *format, ...) {
   va_list args;
   va_start(args, format);
   int ret = vprintf(format, args);
   va_end(args);
   return ret;
}
static inline void refresh_watchdog(void) {
   esp_task_wdt_reset();
}
/* tagged logging, backed by esp_log_writev (no color/timestamp decoration, but
 * respects per-tag level filtering) so user_tasks can log without depending on
 * the ESP_LOG* macros directly */
static inline int os_log_debug(const char *tag, const char *format, ...) {
   va_list args;
   va_start(args, format);
   esp_log_writev(ESP_LOG_DEBUG, tag, format, args);
   va_end(args);
   return 0;
}
static inline int os_log_info(const char *tag, const char *format, ...) {
   va_list args;
   va_start(args, format);
   esp_log_writev(ESP_LOG_INFO, tag, format, args);
   va_end(args);
   return 0;
}
static inline int os_log_error(const char *tag, const char *format, ...) {
   va_list args;
   va_start(args, format);
   esp_log_writev(ESP_LOG_ERROR, tag, format, args);
   va_end(args);
   return 0;
}
/*add functions and variable declarations before */
#ifdef __cplusplus
}
#endif
#endif //OS_TYPE_H
