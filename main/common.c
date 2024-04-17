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
 *
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
 * This file is part of the sofi.
 *
 * Author: Shoma Gane <shomagan@gmail.com>
 *         Ayrat Girfanov <girfanov.ayrat@yandex.ru>
 */
#ifndef COMMON_C
#define COMMON_C 1
/**
  * @defgroup common
  * @brief maintanance thing
  * 1 timer initialize
  * 2 led control
  */
#include <string.h>


#include "pin_map.h"
#include "driver/gpio.h"
#include "esp_log.h"
#include "slip_handle.h"
#include "regs.h"
#include "mirror_storage.h"
#include "esp_wifi.h"
#include "esp_sleep.h"
#include "pwm_test.h"
#include "main_config.h"
#include "common.h"
#include "modbus_tcp_client.h"
#include "wifi_slip_main.h"
#include "modbus_tcp_client.h"
#include "driver/timer.h"
#include "driver/rtc_io.h"
#include "time.h"
#if UDP_BROADCAST_ENABLE
#include "udp_broadcast.h"
#endif

#define DUTY_TASK_PERIOD_MS 100u
#define TEMP_BUFFER_SIZE 64u
#define TIMER_INTERVAL_SECONDS    1u
/*80.000.000 hz/800 = 100000 */
#define TIMER_DIVIDER         800u  //  Hardware timer clock divider
#define TIMER_SCALE           (TIMER_BASE_CLK / (TIMER_DIVIDER*200))  // convert counter value to seconds
#define BOX_SHIFT 40u

task_handle_t common_duty_task_handle;
task_handle_t modbus_master_id;
static task_handle_t common_timer_task_handle = NULL;
static const char *TAG = "common";
extern modbus_tcp_client_slave_connections_t modbus_tcp_client_slave_connections[];
static u16 led_os_blink_on_time = 0;
static int timer_init_state = 0;

static int example_tg_timer_init(int group, int timer, bool auto_reload);
static bool timer_group_isr_callback(void *args);
static void common_timer_task(void *pvParameters );
static void example_tg_timer_deinit(int group, int timer);
static int common_duty_init(void);
static void display_update(uint32_t task_tick);
static void udp_broabcast_update(uint32_t task_tick);
/**
 * @brief duty_task - do several common functions
 * @param pvParameters
 */
void common_duty_task(void *pvParameters ){
    /* Remove compiler warning about unused parameter. */
    (void) pvParameters;
    /* Init internal ADC service channels */
    /* Initialise xNextWakeTime - this only needs to be done once. */
    uint32_t prepare_time = 0u;
    uint32_t task_tick = 0u;
    uint32_t signal_value;
    u8 position = BOX_SHIFT;
    u8 const box_side = 10;
    if(common_duty_init()<0){
        led_blink_on(5000);
    }
    while(1){
        /* Place this task in the blocked state until it is time to run again. */
        signal_value = 0;
        if(task_notify_wait(STOP_CHILD_PROCCES|PREPARE_TO_RESET, &signal_value, DUTY_TASK_PERIOD_MS)!=pdTRUE){
            /*by timeout*/
            if(led_os_blink_on_time ){
                gpio_set_level(CONFIG_LED_BLINK_GPIO, 1u);
                led_os_blink_on_time = led_os_blink_on_time>DUTY_TASK_PERIOD_MS?led_os_blink_on_time-DUTY_TASK_PERIOD_MS:0;
            }else{
                gpio_set_level(CONFIG_LED_BLINK_GPIO, 0u);
            }
            if(((task_tick)%(1000u/DUTY_TASK_PERIOD_MS))==0u){
                // rtc time update start 
                semaphore_take(regs_access_mutex, portMAX_DELAY);{
                    regs_global.vars.live_time++;
                }semaphore_release(regs_access_mutex);
                led_blink_on(250u);
                display_update(task_tick);
                main_printf(TAG,"tick %u",task_tick);
                struct timeval tv;
                if (gettimeofday(&tv, NULL)!= 0) {
                    main_error_message(TAG,"Failed to obtain time");
                }else{
                    regs_global.vars.seconds_of_the_day = (u32)tv.tv_sec;
                    main_debug(TAG,"sec of the day %u",tv.tv_sec);
                }
                time_t now;
                now = time(0);
                regs_copy_safe(&regs_global.vars.unix_time,&now,sizeof(regs_global.vars.unix_time));
                main_printf(TAG,"unix time %u",regs_global.vars.unix_time&0xffffffff);
            }
            udp_broabcast_update(task_tick);

            regs_access_t async_flags;
            async_flags.flag = U64_REGS_FLAG;
            regs_get(&regs_global.vars.async_flags,&async_flags);
            if (async_flags.value.op_u64 & ASYNC_INIT_SET_VALUE_FROM_BKRAM_TO_FLASH){
                async_flags.value.op_u64 &= ~ASYNC_INIT_SET_VALUE_FROM_BKRAM_TO_FLASH;
                int result = regs_write_internal(&regs_global.vars.async_flags, async_flags);
                if (internal_flash_save_mirror_to_flash()!=0u){
                    main_printf(TAG, "Failed %d\n",__LINE__);
                }else{
                    main_printf(TAG, "bkram file saved to flash succes");
                }
            }
            if (prepare_time){
                if ((pdTICKS_TO_MS(task_get_tick_count()) - prepare_time ) > TIME_FOR_PREPARE_RESET_MS){
                    main_printf(TAG, "reset trough common duty task");
                    esp_restart();
                }
            }
        }else{
            /*by signal*/
            if (signal_value & STOP_CHILD_PROCCES){
                common_deinit();
            }else if(signal_value & PREPARE_TO_RESET){
                prepare_time = pdTICKS_TO_MS(task_get_tick_count());
            }
        }
        task_tick++;
    }
} 
/**
 * @brief common_init
 * + creates task for timer
 * + timer initialization
 * @return
 */
static int common_duty_init(void){
    int res = task_create(common_timer_task, "common_timer_task", 1500, NULL, (tskIDLE_PRIORITY + 2), &common_timer_task_handle);
    if (res != pdTRUE) {
        ESP_LOGE(TAG, "create slip to wifi flow control task failed");
        res = -1;
    }else{
        /*init timers*/
        timer_init_state = example_tg_timer_init(TIMER_GROUP_0, TIMER_0, false);
        if (timer_init_state <=0){
            res = -1;
        }else{
            res = 0;
        }
    }
    return res;
}

/**
 * @brief common_deinit
 * @return
 */
int common_deinit(){
    /*deinit timers*/
    if (timer_init_state >0){
        example_tg_timer_deinit(TIMER_GROUP_0, TIMER_0);
    }
    if (common_timer_task_handle!=NULL){
        task_delete(common_timer_task_handle);
    }
    return 0;
}
static void udp_broabcast_update(uint32_t task_tick){
#if UDP_BROADCAST_ENABLE && UDP_ADVERTISMENT_PERIOD
    if(((task_tick)%(UDP_ADVERTISMENT_PERIOD/DUTY_TASK_PERIOD_MS))==0u)
#if  UDP_BROADCAST_UDP_REQUEST_ENABLE 
    {   
        udp_broadcast_advertisement(UDP_BROADCAST_OPTION_UDP_REQUEST);
    }
#endif
    if(((task_tick)%(UDP_ADVERTISMENT_PERIOD/DUTY_TASK_PERIOD_MS))==1u){
#if UDP_BROADCAST_INFORMATION_ENABLE 
        udp_broadcast_advertisement(UDP_BROADCAST_OPTION_INFORMATION);
#endif
    }
#endif
}
static void display_update(uint32_t task_tick){
#if SS1306_MODULE
    s32 deviation;
    u64 slave_tick_calculated;
    u64 sys_tick_own;
    regs_copy_safe(&sys_tick_own,&regs_global.vars.sys_tick_counter,sizeof(deviation));
    regs_copy_safe(&deviation,&sync_time_regs.vars.sync_sys_tick_dev,sizeof(deviation));
    slave_tick_calculated = sys_tick_own - deviation;
    char temp_buff[TEMP_BUFFER_SIZE] = {0u};
    u16 sync_active_temp;
    regs_copy_safe(&sync_active_temp,&sync_time_regs.vars.sync_active,sizeof(sync_active_temp));
    u8g2_ClearBuffer(&u8g2);
#if DISPLAY_TIME_DIFF
    s32 time_div_slave = 0;
    s32 time_div_own = 0;
    u8 synchronized = 0;
    semaphore_take(regs_access_mutex, portMAX_DELAY);{
        time_div_slave = sync_time_regs_from_client.vars.cli_sys_tick_dev;
        time_div_own = sync_time_regs.vars.sync_sys_tick_dev;
    }semaphore_release(regs_access_mutex);    
    if ((time_div_slave != 0) && (time_div_own != 0) ){
        if ((time_div_slave + time_div_own) < 10){
            synchronized = 1u;
            u8g2_DrawBox(&u8g2, BOX_SHIFT, 0, box_side, box_side/2);
        }else{
            u8g2_DrawFrame(&u8g2, position, 0, box_side, box_side/2);
        }
    }else{
        u8g2_DrawFrame(&u8g2, position, 0, box_side, box_side/2);
    }
    if (position != BOX_SHIFT){
        position = BOX_SHIFT;
    }else{
        position = BOX_SHIFT + box_side * 2u;
    }
    u8g2_SetFont(&u8g2, u8g2_font_10x20_mf);
#if MAIN_CONFIG_WIFI_AP
    if (regs_global.vars.ap_connections_number){
        sprintf(temp_buff,"1c");
    }else{
        sprintf(temp_buff,"1");
    }
#elif MAIN_CONFIG_WIFI_NODE
    if (regs_global.vars.sta_connect){
        sprintf(temp_buff,"2c");
    }else{
        sprintf(temp_buff,"2");
    }
#endif
    u8g2_SetFont(&u8g2, u8g2_font_4x6_mf);
    u8g2_DrawStr(&u8g2, 0,6u, temp_buff);
    u64 lap_own;
    u64 lap_slave;
    int distance_own;
    int distance_slave;
    semaphore_take(regs_access_mutex, portMAX_DELAY);{
        lap_own = sr04_reg.vars.lap;
        lap_slave = sr04_reg_client.vars.cli_lap;
        distance_own = (int)sr04_reg.vars.distance_filtered;
        distance_slave = (int)sr04_reg_client.vars.cli_distance_filtered;
    }semaphore_release(regs_access_mutex);
    lap_own = lap_own - time_div_own;
    u64 div;
    if (lap_own > lap_slave){
        div = lap_own - lap_slave;
    }else{
        div = lap_slave - lap_own;
    }
    sprintf(temp_buff,"%llu ms",div);
    u8g2_SetFont(&u8g2, u8g2_font_9x15B_mf);
    u8g2_DrawStr(&u8g2, 0,18u, temp_buff);
    sprintf(temp_buff,"%i sm %i",distance_own,distance_slave);
    u8g2_DrawStr(&u8g2, 0,32u, temp_buff);
    u8g2_SendBuffer(&u8g2);
#else   // DISPLAY_TIME_DIFF
    semaphore_take(regs_access_mutex, portMAX_DELAY);{
        sprintf(temp_buff,"m %u;%u.%u.%u.%u;S-%u",regs_global.vars.mdb_addr,regs_global.vars.sta_ip[0],regs_global.vars.sta_ip[1],regs_global.vars.sta_ip[2],regs_global.vars.sta_ip[3],sync_active_temp & SYNC_STATE_SYNCRONIZED);
    }semaphore_release(regs_access_mutex);
    u8g2_SetFont(&u8g2, u8g2_font_5x8_tf);
    u8g2_DrawStr(&u8g2, 0,6u, temp_buff);
    memset(temp_buff,0,TEMP_BUFFER_SIZE);
    memset(temp_buff,0,TEMP_BUFFER_SIZE);
    u32 success_requests = 0;
    u32 failed_requests = 0;
    for (int i = 0;i < MAX_NUMBER_OF_SLAVES_CONNECTIONS;i++){
        success_requests += modbus_tcp_client_slave_connections[i].success_requests;
        failed_requests += modbus_tcp_client_slave_connections[i].failed_requests;
    }
    //sprintf(temp_buff,"OT%llu;SC%llu",sys_tick_own,slave_tick_calculated);
    sprintf(temp_buff,"%f %llu",sr04_reg.vars.lap_distance,sr04_reg.vars.lap);
    u8g2_DrawStr(&u8g2, 0,14u, temp_buff);
    semaphore_take(regs_access_mutex, portMAX_DELAY);{
    sprintf(temp_buff,"RC:%lu;P%lu;E%lu",regs_global.vars.reset_num,success_requests,failed_requests);
    }semaphore_release(regs_access_mutex);
    u8g2_DrawStr(&u8g2, 0,22u, temp_buff);
    s32 sync_sys_tick_dev_own;         //deviation between master and slave on our side
    s32 sync_sys_tick_dev_client;      //deviation between master and slave on client side
    regs_copy_safe(&sync_sys_tick_dev_own,&sync_time_regs.vars.sync_sys_tick_dev,sizeof(sync_sys_tick_dev_own));
    regs_copy_safe(&sync_sys_tick_dev_client,&sync_time_regs_from_client.vars.cli_sys_tick_dev,sizeof(sync_sys_tick_dev_client));
    sprintf(temp_buff,"dev %li %li",sync_sys_tick_dev_own,sync_sys_tick_dev_client);
    u8g2_DrawStr(&u8g2, 0,30u, temp_buff);
    u8g2_SendBuffer(&u8g2);
#endif  // DISPLAY_TIME_DIFF
#endif
}

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
    if(led_os_blink_on_time){led_os_blink_on_time = 0u;}
    return;
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
/**
 * @brief compare_float_value
 * @param a compare
 * @param b to
 * @param diff - max differ
 * @return 1 if a out of range b (b-diff:b+diff)
 */
u8 compare_float_value(float a,float b, float diff){
    u8 res = 0;
    if ((a>(b-diff))&&(a<(b+diff))){
        res = 1;
    }else{
        res = 0;
    }
    return res;
}
/**
 * @brief Initialize selected timer of timer group
 *
 * @param group Timer Group number, index from 0
 * @param timer timer ID, index from 0
 * @param auto_reload whether auto-reload on alarm event
 * @param timer_interval_sec interval of alarm
 */
static int example_tg_timer_init(int group, int timer, bool auto_reload){
    /* Select and initialize basic parameters of the timer */
    int result = 0;
    static timer_config_t config = {
        .divider = TIMER_DIVIDER,
        .counter_dir = TIMER_COUNT_UP,
        .counter_en = TIMER_PAUSE,
        .alarm_en = TIMER_ALARM_EN,
        .auto_reload = 0,
    }; // default clock source is APB
    config.auto_reload = auto_reload;
    timer_init(group, timer, &config);
    /* Timer's counter will initially start from value below.
v       Also, if auto_reload is set, this value will be automatically reload on alarm */
    timer_set_counter_value(group, timer, 0);
    /* Configure the alarm value and the interrupt on alarm. */
    timer_set_alarm_value(group, timer, (uint32_t)(TIMER_SCALE));
    timer_enable_intr(group, timer);
    timer_isr_callback_add(group, timer, timer_group_isr_callback, &config, 0);
    if ( ESP_OK == timer_start(group, timer)){
        result = 1;
    }else{
        result = -1;
    }
    return result;
}
/**
 * @brief DeInitialize selected timer of timer group
 *      stop and deinit
 * @param group Timer Group number, index from 0
 * @param timer timer ID, index from 0
 */
static void example_tg_timer_deinit(int group, int timer){
    timer_pause(group, timer);
    timer_deinit(group,timer);
}

static bool IRAM_ATTR timer_group_isr_callback(void *args){
    timer_config_t *info = (timer_config_t *) args;
    /* Prepare basic event data that will be then sent back to task */
    uint64_t timer_counter_value = timer_group_get_counter_value_in_isr(TIMER_GROUP_0, TIMER_0);
    if (!info->auto_reload) {
        timer_group_set_alarm_value_in_isr(TIMER_GROUP_0, TIMER_0, timer_counter_value + (uint32_t)(TIMER_SCALE));
    }
    if (common_timer_task_handle!=NULL){
        ui32 prev_signal=0;
        int  higher_priority_task_woken;
        task_notify_send_isr_overwrite(common_timer_task_handle,0,(uint32_t)timer_counter_value,&prev_signal,&higher_priority_task_woken);
    }
    return pdTRUE; // return whether we need to yield at the end of ISR
}
/**
 * @brief timer task - small task to controll timer - fast
 * @param pvParameters
 */
void common_timer_task(void *pvParameters ){
    /* Remove compiler warning about unused parameter. */
    (void) pvParameters;
    /* Init internal ADC service channels */
    /* Initialise xNextWakeTime - this only needs to be done once. */
    uint32_t task_tick = 0;
    uint32_t signal_value;
    while(1){
        /* Place this task in the blocked state until it is time to run again. */
        signal_value = 0;
        task_notify_wait(0, &signal_value, WAIT_MAX_DELAY);
        semaphore_take(regs_access_mutex, portMAX_DELAY);{
            u64_t sys_tick_counter;
            timer_get_counter_value(TIMER_GROUP_0, TIMER_0, &sys_tick_counter);
            sys_tick_counter /=100;
            memcpy(&regs_global.vars.sys_tick_counter,&sys_tick_counter,sizeof(sys_tick_counter));
        }semaphore_release(regs_access_mutex);
        task_tick++;
    }
}


#endif //COMMON_C
