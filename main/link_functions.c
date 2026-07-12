
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
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "freertos/queue.h"
#include <stdio.h>

#define OS_VERSION {0,0,0,1}

static portMUX_TYPE s_critical_mux = portMUX_INITIALIZER_UNLOCKED;

static void _task_delay_ms(u32 ms) { vTaskDelay(pdMS_TO_TICKS(ms)); }
static u32 _task_get_tick_count(void) { return (u32)xTaskGetTickCount(); }
static void _task_enter_critical(void) { taskENTER_CRITICAL(&s_critical_mux); }
static void _task_exit_critical(void) { taskEXIT_CRITICAL(&s_critical_mux); }
static SemaphoreHandle_t _semaphore_create_mutex(void) { return xSemaphoreCreateMutex(); }
static BaseType_t _semaphore_take(SemaphoreHandle_t m, TickType_t t) { return xSemaphoreTake(m, t); }
static BaseType_t _semaphore_release(SemaphoreHandle_t m) { return xSemaphoreGive(m); }
static QueueHandle_t _queue_create(u32 len, u32 sz) { return xQueueCreate(len, sz); }
static BaseType_t _queue_send(QueueHandle_t q, const void *i, TickType_t t) { return xQueueSend(q, i, t); }
static BaseType_t _queue_receive(QueueHandle_t q, void *i, TickType_t t) { return xQueueReceive(q, i, t); }

const link_functions_t link_functions = {
    .task_delay_ms = _task_delay_ms,
    .task_get_tick_count = _task_get_tick_count,
    .task_enter_critical = _task_enter_critical,
    .task_exit_critical = _task_exit_critical,
    .semaphore_create_mutex = _semaphore_create_mutex,
    .semaphore_take = _semaphore_take,
    .semaphore_release = _semaphore_release,
    .queue_create = _queue_create,
    .queue_send = _queue_send,
    .queue_receive = _queue_receive,
    .regs_description_list_add_new = regs_description_list_add_new,
    .printf = printf,
    .version = OS_VERSION,
};
#endif //LINK_FUNCTIONS_C
