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
#include "esp_wifi.h"
#include "esp_sleep.h"
#include "driver/rtc_io.h"
#include "common.h"
#include "wireless_control.h"

#define SLEEP_CONTROL_TASK_PERIOD_MS 100u
static const char *TAG = "sleep_control";
task_handle_t sleep_control_handle_id = NULL;
#if ENABLE_DEEP_SLEEP
/*set up wakeup source*/
#if CONFIG_IDF_TARGET_ESP32
static void rtc_setup_wakeup_pin(void);
#endif
#if CONFIG_IDF_TARGET_ESP32C3
static void rtc_setup_wakeup_timer(void);
#endif
static int check_deep_sleep_condition_by_pin (const u32 duty_task_period_ms);
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
   (void)(*arg);
   uint32_t signal_value;
   sleep_control_init();
   u32 prev_signal=0;
   u64 task_counter = 0;
   while(1){
      if(task_notify_wait(STOP_CHILD_PROCCES|SLEEP_TASK_DEEP_SLEEP_FOR_120_SEC, &signal_value, SLEEP_CONTROL_TASK_PERIOD_MS)==pdTRUE){
         /*by signal*/
         if (signal_value & STOP_CHILD_PROCCES){
            sleep_control_deinit();
            task_delete(task_get_id());
         }
         if (signal_value & SLEEP_TASK_DEEP_SLEEP_FOR_120_SEC){
            task_notify_send(wireless_control_handle_id, WIRELESS_TASK_STOP_WIFI,&prev_signal);
            task_delay_ms(100);
            rtc_setup_wakeup_timer();
            esp_deep_sleep_start();
         }
      }
#if ENABLE_DEEP_SLEEP
#if CONFIG_IDF_TARGET_ESP32
      if (check_deep_sleep_condition_by_pin (SLEEP_CONTROL_TASK_PERIOD_MS) > 0u)
      {
         rtc_setup_wakeup_pin();
         esp_deep_sleep_start();
      }
#endif
#endif //ENABLE_DEEP_SLEEP      
      task_counter++;
   }
}

/*set up wakeup source*/
#if CONFIG_IDF_TARGET_ESP32
static void rtc_setup_wakeup_pin(void){
    main_printf(TAG,"Enabling EXT0 wakeup on pin GPIO%d\n", EXT_WAKEUP_PIN);
    ESP_ERROR_CHECK(esp_sleep_enable_ext0_wakeup(EXT_WAKEUP_PIN, 1));
    // Configure pullup/downs via RTCIO to tie wakeup pins to inactive level during deepsleep.
    // EXT0 resides in the same power domain (RTC_PERIPH) as the RTC IO pullup/downs.
    // No need to keep that power domain explicitly, unlike EXT1.
    ESP_ERROR_CHECK(rtc_gpio_pullup_dis(EXT_WAKEUP_PIN));
    ESP_ERROR_CHECK(rtc_gpio_pulldown_en(EXT_WAKEUP_PIN));
    rtc_gpio_isolate(GPIO_NUM_12);
}
#endif
#if CONFIG_IDF_TARGET_ESP32C3
static void rtc_setup_wakeup_timer(void){
    const int wakeup_time_sec = 120;
    main_printf(TAG,"Enabling timer wakeup, %ds\n", wakeup_time_sec);
    esp_sleep_enable_timer_wakeup(wakeup_time_sec * 1000000);
}
#endif
static int check_deep_sleep_condition_by_pin (const u32 duty_task_period_ms){
   static u32 deep_sleep_counter;
   u32 deep_sleep_pin = 0;
#if CONFIG_IDF_TARGET_ESP32    
   deep_sleep_pin = gpio_get_level(EXT_WAKEUP_PIN);
#endif   
   if (deep_sleep_pin){
      deep_sleep_counter++;
   }else{
      deep_sleep_counter=0;
   }
   if (deep_sleep_counter>((5u/*sec*/*1000u)/duty_task_period_ms)){
      display_good_bye_message();
      vTaskDelay(2500 / portTICK_PERIOD_MS);
      return 1;
   }
   return 0;
}
static void display_good_bye_message(void){
   main_printf(TAG, "deep sleep trough pin state in sleep task");
#if SS1306_MODULE   
   char temp_buff[TEMP_BUFFER_SIZE] = {0u};
   sprintf(temp_buff,"going to sleep");
   u8g2_ClearBuffer(&u8g2);
   u8g2_SetFont(&u8g2, u8g2_font_9x15B_mf);
   u8g2_DrawStr(&u8g2, 0,22u, temp_buff);
   u8g2_SendBuffer(&u8g2);
#endif
}

#endif /*ENABLE_DEEP_SLEEP*/
#endif /*SLEEP_CONTROL_C*/
