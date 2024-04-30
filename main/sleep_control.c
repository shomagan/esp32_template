/**
 * @file sleep_control.c
 * @author Shoma Gane <shomagan@gmail.com>
 *         
 * @defgroup main
 * @ingroup main
 * @version 0.1 
 * @brief  TODO!!! write brief in 
 */
#ifndef SLEEP_CONTROL_C
#define SLEEP_CONTROL_C 1
#include "sleep_control.h"
#include "os_type.h"
#include "pin_map.h"
#include "esp_wifi.h"
#include "esp_sleep.h"
#include "driver/rtc_io.h"
#include "common.h"
#include "wireless_control.h"

#define SLEEP_CONTROL_TASK_PERIOD_MS 100u
#define ALIVE_DEFAULT_S 50u
#define ALIVE_LONG_S 300u
static const char *TAG = "sleep_control";
task_handle_t sleep_control_handle_id = NULL;
#if ENABLE_DEEP_SLEEP
/*set up wakeup source*/
static void rtc_setup_wakeup_pin(void);
static int rtc_setup_wakeup_timer(u16 sec);
static int wake_up_control(esp_sleep_wakeup_cause_t esp_sleep_wakeup_cause);
static void display_good_bye_message(void);
static int sleep_control_init(void);
static int sleep_control_deinit();

static int sleep_control_init(){
   int result = 0;
   regs_global.vars.current_state[0] |= CS0_TASK_ACTIVE_SLEEP_CONTROL;
   return result;
}  
static int sleep_control_deinit(){
   int result = 0;
   regs_global.vars.current_state[0] &= ~((u32)CS0_TASK_ACTIVE_SLEEP_CONTROL);
   return result;
}
void sleep_control_task(void *arg){
   esp_sleep_wakeup_cause_t * esp_sleep_wakeup_cause =  (esp_sleep_wakeup_cause_t *)(arg);
   uint32_t signal_value=0;
   uint32_t signal_in_process=0;
   u16 sleep_time = 0;
   sleep_control_init();
   u32 prev_signal=0;
   u64 task_counter=0;
   u16 alive_init_value = (ALIVE_DEFAULT_S*1000) / SLEEP_CONTROL_TASK_PERIOD_MS;
   u16 alive_timer;
   TickType_t task_timer;
   regs_global.vars.wake_up_cause = *esp_sleep_wakeup_cause;
   if (wake_up_control(*esp_sleep_wakeup_cause)==WAKE_UP_END){
      alive_timer = (ALIVE_DEFAULT_S*1000) / SLEEP_CONTROL_TASK_PERIOD_MS;
   }else{
      alive_timer = (ALIVE_LONG_S*1000) / SLEEP_CONTROL_TASK_PERIOD_MS;
      main_printf(TAG,"start config mode");
   }
   while(1){
      signal_value = 0u;
      task_delay_until_ms(&task_timer,SLEEP_CONTROL_TASK_PERIOD_MS);
      task_notify_wait(0xffffffff, &signal_value, 1/*ms*/);
      /*by signal*/
      if (signal_value & STOP_CHILD_PROCCES){
         sleep_control_deinit();
         task_delete(task_get_id());
      }
      if (signal_value & SLEEP_TASK_DEEP_SLEEP_FOR_120_SEC){
         main_printf(TAG,"SLEEP_TASK_DEEP_SLEEP_FOR_120_SEC");
         signal_in_process |= SLEEP_TASK_DEEP_SLEEP_FOR_120_SEC;
         sleep_time = 120u;
      }
      if (signal_value & SLEEP_TASK_DEEP_SLEEP_FOR_N_SEC){
         
         signal_in_process |= SLEEP_TASK_DEEP_SLEEP_FOR_N_SEC;
         sleep_time = (signal_value >> 16u) & 0x0000ffff;
         main_printf(TAG,"SLEEP_TASK_DEEP_SLEEP_FOR_N_SEC %u", sleep_time);
      }
      if (alive_timer==0){
         if (signal_in_process & SLEEP_TASK_DEEP_SLEEP_FOR_N_SEC){
            signal_in_process &= ~((uint32_t)SLEEP_TASK_DEEP_SLEEP_FOR_N_SEC);
         }else  if (signal_in_process & SLEEP_TASK_DEEP_SLEEP_FOR_120_SEC){
            signal_in_process &= ~((uint32_t)SLEEP_TASK_DEEP_SLEEP_FOR_120_SEC);
         }
         if (sleep_time){
            task_notify_send(wireless_control_handle_id, WIRELESS_TASK_STOP_WIFI,&prev_signal);
            main_printf(TAG,"set ASYNC_INIT_SET_VALUE_FROM_BKRAM_TO_FLASH");
            regs_global.vars.async_flags |= ASYNC_INIT_SET_VALUE_FROM_BKRAM_TO_FLASH;
            task_delay_ms(300u);
            if (ESP_OK != rtc_setup_wakeup(sleep_time)){
               main_error_message(TAG,"wake up seting problem");  
            }
            display_good_bye_message();
            esp_deep_sleep_start();
         }
      }else{
         alive_timer--;
         if (alive_timer%10==0u){
            main_printf(TAG,"non sleep timer %d ",alive_timer);
         }
      }
      if (1u==gpio_get_level(EXT_WAKEUP_PIN)){
         main_printf(TAG,"prolongation");
         alive_timer = alive_init_value;/*prolongation*/
      }
      task_counter++;
   }
}
int rtc_setup_wakeup(u16 seconds){
   rtc_setup_wakeup_pin();
   return rtc_setup_wakeup_timer(seconds);
}
/*set up wakeup source*/
static void rtc_setup_wakeup_pin(void){
    main_printf(TAG,"Enabling EXT0 wakeup on pin GPIO%d\n", EXT_WAKEUP_PIN);
#if CONFIG_IDF_TARGET_ESP32    
    ESP_ERROR_CHECK(esp_sleep_enable_ext0_wakeup(EXT_WAKEUP_PIN, 1u));
    // Configure pullup/downs via RTCIO to tie wakeup pins to inactive level during deepsleep.
    // EXT0 resides in the same power domain (RTC_PERIPH) as the RTC IO pullup/downs.
    // No need to keep that power domain explicitly, unlike EXT1.
    ESP_ERROR_CHECK(rtc_gpio_pullup_dis(EXT_WAKEUP_PIN));
    ESP_ERROR_CHECK(rtc_gpio_pulldown_en(EXT_WAKEUP_PIN));
#else
    const gpio_config_t config = {
        .pin_bit_mask = BIT(EXT_WAKEUP_PIN),
        .mode = GPIO_MODE_INPUT,
        .pull_up_en = 0,
        .pull_down_en = 1,
        .intr_type = GPIO_INTR_DISABLE
    };
    ESP_ERROR_CHECK(gpio_config(&config));
    ESP_ERROR_CHECK(esp_deep_sleep_enable_gpio_wakeup(BIT(EXT_WAKEUP_PIN), 1u));
#endif    
}
static int rtc_setup_wakeup_timer(u16 seconds){
   u64 useconds = seconds;
   useconds *= 1000000u;
   main_printf(TAG,"Enabling timer wakeup, %ds\n", seconds);
   return esp_sleep_enable_timer_wakeup(useconds);
}

static void display_good_bye_message(void){
   main_printf(TAG, "deep sleep in sleep task");
#if SS1306_MODULE   
   char temp_buff[TEMP_BUFFER_SIZE] = {0u};
   sprintf(temp_buff,"going to sleep");
   u8g2_ClearBuffer(&u8g2);
   u8g2_SetFont(&u8g2, u8g2_font_9x15B_mf);
   u8g2_DrawStr(&u8g2, 0,22u, temp_buff);
   u8g2_SendBuffer(&u8g2);
#endif
}
/*
* @brief wake_up_control - control wake up
* @return   
*/
static int wake_up_control(esp_sleep_wakeup_cause_t  esp_sleep_wakeup_cause){
   int result = WAKE_UP_END;
   u32 deep_sleep_pin = 0;
   if (ESP_SLEEP_WAKEUP_EXT0 == esp_sleep_wakeup_cause){
      task_delay_ms(5000 / portTICK_PERIOD_MS);
      deep_sleep_pin = gpio_get_level(EXT_WAKEUP_PIN);
      if (0u==deep_sleep_pin){/*dont wake up if pressed less than 1 sec*/
         result = WAKE_UP_START;
      }
   }else if (ESP_SLEEP_WAKEUP_UNDEFINED == esp_sleep_wakeup_cause){
      result = WAKE_UP_START;
   }
   return result;
}

#endif /*ENABLE_DEEP_SLEEP*/
#endif /*SLEEP_CONTROL_C*/
