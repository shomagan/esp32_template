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
 * Copyright (c) 2018 Snema Service
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

/*add includes before */
#ifdef __cplusplus 
   extern "C" {
#endif
/*add functions and variable declarations below */
#define main_debug ESP_LOGI
#define main_printf ESP_LOGI
#define main_error_message ESP_LOGE
#define task_handle_t TaskHandle_t
#define task_delay_until_ms(timer,ms)   \
   vTaskDelayUntil(timer, pdMS_TO_TICKS(ms))
#define task_delay_ms(ms)   vTaskDelay(ms/portTICK_PERIOD_MS)
#define task_get_tick_count() xTaskGetTickCount()
#define semaphore_handle_t SemaphoreHandle_t
#define semaphore_create_binary vSemaphoreCreateBinary
#define semaphore_delete     vSemaphoreDelete
#define semaphore_take(mutex,time_ms) xSemaphoreTake(mutex,time_ms)
#define semaphore_release(x) xSemaphoreGive(x)

#define queue_handle_t   xQueueHandle
#define queue_create(queue_len, item_len) xQueueCreate(queue_len, item_len)
#define queue_receive(queue, msg, ms)\
    xQueueReceive(queue, msg, pdMS_TO_TICKS(ms))
#define queue_send(queue, msg, ms) \
    xQueueSend(queue, msg, pdMS_TO_TICKS(ms))
#define task_create(function, name, stack_size, param, prio, handler)\
    xTaskCreate(function, name, stack_size, param, prio, handler)
#define task_delete(handler) vTaskDelete(handler)
#define task_get_id   xTaskGetCurrentTaskHandle
#define os_yield() taskYIELD()
#define task_notify_wait(flags,signal,ms) xTaskNotifyWait(0,flags,signal,ms/portTICK_PERIOD_MS)

#define task_notify_send(thread_id,signal,prev_value) xTaskGenericNotify( thread_id, 0, (uint32_t)signal, eSetBits, prev_value)
#define os_enter_critical(mutex) portENTER_CRITICAL(mutex)
#define os_exit_critical(mutex) portEXIT_CRITICAL(mutex)

/*add functions and variable declarations before */
#ifdef __cplusplus
}
#endif
#endif //OS_TYPE_H
