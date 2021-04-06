/**
 * @file common.c
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
#ifndef COMMON_C
#define COMMON_C 1
#include "common.h"

#include "pin_map.h"
#include "driver/gpio.h"
#include "esp_log.h"
#include "slip_handle.h"
#include "regs.h"
#include "mirror_storage.h"
#include "esp_wifi.h"

#define DUTY_TASK_PERIOD_MS 100
task_handle_t common_duty_task_handle;
static const char *TAG = "common";
/**
 * @brief duty_task - do several common functions
 * @param pvParameters
 */
static void common_duty_task(void *pvParameters );
/**
 * @brief common_init_gpio - init the common(system, like LED) use pins
 * @return
 */
static int common_init_gpio(void);

static u16 led_os_blink_on_time = 0;
/**
 * @brief led_blink_on enable the led on ms
 * @param time_ms
 */
void led_blink_on(u16 time_ms){
    if(!led_os_blink_on_time){led_os_blink_on_time = time_ms;}
    return;
}

/**
 * @brief led_blink_off disable the led if it was enableb
 */
void led_blink_off(){
    if(led_os_blink_on_time){led_os_blink_on_time = 0;}
    return;
}
/**
 * @brief common_init
 * @return
 */
int common_init(){
    common_init_gpio();
    return 0;
}
/**
 * @brief common_init_tasks - all task must starting here
 * @return
 */
int common_init_tasks(){
    int res=0;
    res = task_create(slip_flow_control_task, "wifi2slip_flow_ctl", 2048, NULL, (tskIDLE_PRIORITY + 2), &wifi_slip_config.slip_flow_control_handle);
    if (res != pdTRUE) {
        ESP_LOGE(TAG, "create wifi to slip flow control task failed");
    }
    res = task_create(common_duty_task, "common_duty_task", 2048, NULL, (tskIDLE_PRIORITY + 2), &common_duty_task_handle);
    if (res != pdTRUE) {
        ESP_LOGE(TAG, "create slip to wifi flow control task failed");
    }
    task_create(slip_handle_uart_rx_task, "slip_modem_uart_rx_task", SLIP_RX_TASK_STACK_SIZE, &wifi_slip_config, SLIP_RX_TASK_PRIORITY, &wifi_slip_config.uart_rx_task);
    return res;
}
/**
 * @brief common_deinit
 * @return
 */
int common_deinit(){
    return 0;
}

/**
 * @brief common_init_gpio - init the common(system, like LED) use pins
 * @return
 */
static int common_init_gpio(){
    gpio_config_t io_conf = {0};
    //disable interrupt
    io_conf.intr_type = GPIO_INTR_DISABLE;
    //set as output mode
    io_conf.mode = GPIO_MODE_OUTPUT;
    //bit mask of the pins that you want to set,e.g.GPIO18/19
    io_conf.pin_bit_mask = 1<<CONFIG_LED_BLINK_GPIO;
    //disable pull-down mode
    io_conf.pull_down_en = 0;
    //disable pull-up mode
    io_conf.pull_up_en = 0;
    //configure GPIO with the given settings
    gpio_config(&io_conf);
    return 0;
}
/**
 * @brief duty_task - do several common functions
 * @param pvParameters
 */
static void common_duty_task(void *pvParameters ){
    /* Remove compiler warning about unused parameter. */
    (void) pvParameters;
    /* Init internal ADC service channels */
    /* Initialise xNextWakeTime - this only needs to be done once. */
    ui32 prepare_time = 0;
    uint32_t task_tick = 0;
    uint32_t signal_value;
    for( ;; ){
        /* Place this task in the blocked state until it is time to run again. */
        signal_value = 0;
        if(task_notify_wait(STOP_CHILD_PROCCES|PREPARE_TO_RESET, &signal_value, DUTY_TASK_PERIOD_MS)!=pdTRUE){
            /*by timeout*/
            if(led_os_blink_on_time ){
                gpio_set_level(CONFIG_LED_BLINK_GPIO, 1);
                led_os_blink_on_time = led_os_blink_on_time>DUTY_TASK_PERIOD_MS?led_os_blink_on_time-DUTY_TASK_PERIOD_MS:0;
            }else{
                gpio_set_level(CONFIG_LED_BLINK_GPIO, 0);
            }
            if(((task_tick)%(1000/DUTY_TASK_PERIOD_MS))==0u){
                /* rtc time update start */
                led_blink_on(250);
            }
            if(((task_tick)%(5000/DUTY_TASK_PERIOD_MS))==0u){    // every 5 sec
                main_printf(TAG,"tick %u",task_tick);
            }
            if(((task_tick)%(60000/DUTY_TASK_PERIOD_MS))==0u){    // every 60 sec
                if(regs_global.vars.sta_connect==0){
                    main_printf(TAG, "try connect to sta");
                    esp_wifi_connect();
                }
            }
            if (regs_global.vars.async_flags & ASYNC_INIT_SET_VALUE_FROM_BKRAM_TO_FLASH){
                regs_global.vars.async_flags &= (u32)~ASYNC_INIT_SET_VALUE_FROM_BKRAM_TO_FLASH;
                if (internal_flash_save_mirror_to_flash()!=0){
                    main_printf(TAG, "Failed %d\n",__LINE__);
                }else{
                    main_printf(TAG, "bkram file saved to flash succes");
                }
            }
            if (prepare_time){
                if ((pdTICKS_TO_MS(task_get_tick_count()) - prepare_time ) > TIME_FOR_PREPARE_RESET_MS){
                    esp_restart();
                }
            }
        }else{
            /*by signal*/
            if (signal_value & STOP_CHILD_PROCCES){

            }else if(signal_value & PREPARE_TO_RESET){
                prepare_time = pdTICKS_TO_MS(task_get_tick_count());
            }
        }

        task_tick++;
    }
} 
/**
 * @brief is_ascii_symbol_or_digital
 * @param buff
 * @param len must be more then 0
 * @return 1 if consist if symbol or digital
 */
int is_ascii_symbol_or_digital(u8 * buff, u32 len){
    int res = 0;
    if (buff!=NULL){
        for (u32 i=0;i<len;i++){
            if ((buff[i]>=0x30 && buff[i]<=0x39)||
                (buff[i]>=0x41 && buff[i]<=0x5a)||
                (buff[i]>=0x61 && buff[i]<=0x7a)||
                 buff[i]==0x5f||buff[i]==0x2d){
                res = 1;
            }else{
                res = 0;
                break;
            }
        }
    }else{
        res = -1;
    }
    return res;
}
/**
 * @brief memcmp_self
 * @param buff_0
 * @param buff_1
 * @param len
 * @return 1 -if not equal, 0 - if equal
 */
int memcmp_not_equal(const u8 * buff_0,const u8 * buff_1,u32 len){
    if (buff_0!=NULL && buff_1!=NULL){
        for (u32 i=0;i<len;i++){
            if (buff_0[i]!=buff_1[i]){
                return 1;
            }
        }
        return 0;
    }else{
        return -1;
    }
}
#endif //COMMON_C
