/**
 * @file feeder.c
 * @author Shoma Gane <shomagan@gmail.com>
 *         
 * @defgroup feeder
 * @ingroup feeder
 * @version 0.1 
 * @brief  TODO!!! write brief in 
 */
#ifndef FEEDER_C
#define FEEDER_C 1
 
#include "feeder.h"
#include "pin_map.h"
#include "hal/gpio_hal.h"
#include "esp_log.h"
#include "esp_check.h"
#include "soc/rtc.h"
#include "driver/rmt.h"/*use rmt as step controller*/
#include <math.h>
#include "sleep_control.h"
#include "regs_description.h"
#include "mirror_storage.h"

#define FEEDER_TASK_PERIOD (1000u)
#define RMT_TX_CHANNEL RMT_CHANNEL_0
#define SPEED_STEPS 50u
#define MIN_STEPS_PER_SECOND (15*32)
#define MAX_STEPS_PER_SECOND (30*32)
#define AVERAGE_STEPS_PER_SECOND ((MIN_STEPS_PER_SECOND + MAX_STEPS_PER_SECOND)/2)
#define MIN_WORK_TIME_MINUTE 1
#define MAX_SLEEP_TIME_MINUTE 60
#define SLEEP_IS_ACTIVE 1


typedef struct {
   rmt_config_t *rmt_config;
   rmt_step_motor_running_status status;
   rmt_item32_t rmt_items_loop;
   uint32_t rmt_items_loop_count;
   rmt_item32_t rmt_items_speedup[SPEED_STEPS];
   rmt_item32_t rmt_items_speeddown[SPEED_STEPS];
   uint32_t rmt_items_smoothstep_count;
   semaphore_handle_t notify_semphr;/*we have a mutex here :)*/
} rmt_step_motor_t;

task_handle_t feeder_handle_id = NULL;
static const char *TAG = "feeder";


#if FEEDER
static int feeder_init(rmt_step_motor_t * rmt_step_motor);
static int feeder_deinit(rmt_step_motor_t * rmt_step_motor);
static inline float helper_smootherstep_clamp(float x, float lowerlimit, float upperlimit);
static float helper_smootherstep(float edge0, float edge1, float x);
static uint16_t helper_speed_to_duration(uint16_t speed);
static esp_err_t helper_fill_rmt_items(rmt_item32_t *items, uint32_t speed);
static esp_err_t rmt_step_motor_smoothstep(rmt_step_motor_t *rmt_handle, uint32_t n, uint32_t speed_min,uint32_t speed_max);
static IRAM_ATTR void rmt_tx_loop_intr(rmt_channel_t channel, void *args);
static esp_err_t step_motor_create_rmt(rmt_step_motor_t * rmt_step_motor);
static esp_err_t step_motor_delete_rmt(rmt_step_motor_t * rmt_step_motor);
static int run_feeder(rmt_step_motor_t * rmt_step_motor);
static int handle_sleeping(u32 minutes_of_the_day);
static int handle_feeding(u32 minutes_of_the_day, int * feeded_minute, rmt_step_motor_t * rmt_step_motor);
static esp_err_t rmt_step_motor_step_impl(rmt_step_motor_t *rmt_handle, uint32_t n, uint32_t speed);
static esp_err_t rmt_step_motor_step(rmt_step_motor_t *rmt_handle, uint32_t n, uint32_t speed);

static int feeder_init(rmt_step_motor_t * rmt_step_motor){
   int result = 0;
   regs_global->vars.current_state[0] |= CS0_TASK_ACTIVE_FEEDER;
   gpio_config_t io_conf = {};
   io_conf.intr_type = GPIO_INTR_DISABLE;
   io_conf.mode = GPIO_MODE_OUTPUT;
   io_conf.pin_bit_mask = (1ull<<GPIO_OUTPUT_STEP_MOTOR_EN)|
                        (1ull<<GPIO_OUTPUT_STEP_MOTOR_DIR0)|
                        /*(1ull<<GPIO_OUTPUT_STEP_MOTOR_STEP0)|*/
                        (1ull<<GPIO_OUTPUT_STEP_MOTOR_SLEEP);
                        /*(1ull<<GPIO_OUTPUT_STEP_MOTOR_STEP1);*/
   io_conf.pull_down_en = 0;
   io_conf.pull_up_en = 0;
   gpio_config(&io_conf);
   gpio_set_level(GPIO_OUTPUT_STEP_MOTOR_EN, 1);/*not active*/
   gpio_set_level(GPIO_OUTPUT_STEP_MOTOR_DIR0, 0);
   gpio_set_level(GPIO_OUTPUT_STEP_MOTOR_SLEEP, 0);
   ESP_ERROR_CHECK(step_motor_create_rmt(rmt_step_motor));
   return result;
}

static int feeder_deinit(rmt_step_motor_t * rmt_step_motor){
   int result = 0;
   gpio_config_t io_conf = {0};
   gpio_set_level(GPIO_OUTPUT_STEP_MOTOR_EN, 1);/*not active*/
   io_conf.intr_type = GPIO_INTR_DISABLE;
   io_conf.mode = GPIO_MODE_DISABLE;
   io_conf.pin_bit_mask = (1ull<<GPIO_OUTPUT_STEP_MOTOR_EN)|
                        (1ull<<GPIO_OUTPUT_STEP_MOTOR_DIR0)|
                        (1ull<<GPIO_OUTPUT_STEP_MOTOR_STEP0)|
                        (1ull<<GPIO_OUTPUT_STEP_MOTOR_SLEEP);
   io_conf.pull_down_en = 0;
   io_conf.pull_up_en = 0;
   gpio_config(&io_conf);
   regs_global->vars.current_state[0] &= ~((u32)CS0_TASK_ACTIVE_FEEDER);
   step_motor_delete_rmt(rmt_step_motor);
   return result;
}
void feeder_task(void *arg){
   (void)(*arg);
   uint32_t signal_value;
   u64 task_counter = 0u;
   rmt_config_t dev_config = RMT_DEFAULT_CONFIG_TX(GPIO_OUTPUT_STEP_MOTOR_STEP0, RMT_TX_CHANNEL);
   rmt_step_motor_t rmt_step_motor;
   rmt_step_motor.rmt_config = &dev_config;  
   feeder_init(&rmt_step_motor);
   task_delay_ms(1000);
   int feeded_minute = -1;
   while(1){
      if(task_notify_wait(FEEDER_TASK_STOP_CHILD_PROCCES, &signal_value, FEEDER_TASK_PERIOD)==pdTRUE){
         /*by signal*/
         if (signal_value & FEEDER_TASK_STOP_CHILD_PROCCES){
            feeder_deinit(&rmt_step_motor);
            task_delete(task_get_id());
         }
         if (signal_value & FEEDER_TASK_ONE_FEED_COMM){
            run_feeder(&rmt_step_motor);
         }
      }
      struct timeval tv;
      if (gettimeofday(&tv, NULL)!= 0) {
         main_error_message(TAG,"Failed to obtain time");
      }else{
         u32 minutes_of_the_day = tv.tv_sec/60;
         handle_feeding(minutes_of_the_day, &feeded_minute,&rmt_step_motor);
         handle_sleeping(minutes_of_the_day);
         main_debug(TAG,"sec of the day %u",tv.tv_sec);
      }
      task_counter++;
   }
}
static int handle_sleeping(u32 minutes_of_the_day){
   int result = 0;
   u32 interval_minute = feeder_reg->vars.feeder_interval;
   if (interval_minute > MIN_WORK_TIME_MINUTE){
      u32 time_without_feeding = minutes_of_the_day % interval_minute;
      if (time_without_feeding >= MIN_WORK_TIME_MINUTE){
         u32 sleep_time_minutes = interval_minute - time_without_feeding;
         sleep_time_minutes = sleep_time_minutes > MAX_SLEEP_TIME_MINUTE?MAX_SLEEP_TIME_MINUTE:sleep_time_minutes;
#if SLEEP_IS_ACTIVE
         if (sleep_time_minutes){
            u32 sleep_time_sec = sleep_time_minutes * 60;
            u32 prev_value = 0;
            u32 signal = (u32)SLEEP_TASK_DEEP_SLEEP_FOR_N_SEC | ((sleep_time_sec << 16u)&0xffff0000);
            task_notify_send(sleep_control_handle_id, signal, &prev_value);
         }
#endif
      }//do nothing,dont sleep
   }//do nothing,dont sleep
   return result; 
}
static int handle_feeding(u32 minutes_of_the_day, int * feeded_minute, rmt_step_motor_t * rmt_step_motor){
   int result = 0;
   if (minutes_of_the_day % feeder_reg->vars.feeder_interval == 0){
      if((*feeded_minute < 0) || (*feeded_minute != minutes_of_the_day)){
         result = run_feeder(rmt_step_motor); 
         if(result > 0){
            feeder_reg->vars.feeder_counter++;
            int index = regs_description_get_index_by_address(&feeder_reg->vars.feeder_counter);
            if (index>=0){
                regs_template_t regs_template;
                regs_template.ind =(u16)index;
                if(regs_description_get_by_ind(&regs_template)>=0){
                    if (mirror_access_write(&regs_template)>=0){
                        main_debug(TAG, "succes wrote feeder_counter to mirror");
                    }else{
                        main_debug(TAG, "unsucces writing feeder_counter to mirror");
                    }
                }
            }
         }
      }
      *feeded_minute = minutes_of_the_day;
   }
   return result;   
}
static int run_feeder(rmt_step_motor_t * rmt_step_motor){
   int result = 0;
   gpio_set_level(GPIO_OUTPUT_STEP_MOTOR_EN, 0);    /*active*/
   gpio_set_level(GPIO_OUTPUT_STEP_MOTOR_SLEEP, 1); /*active*/
   task_delay_ms(1);
   u32 steps = (u32)(feeder_reg->vars.feeder_time_sec * AVERAGE_STEPS_PER_SECOND);
   esp_err_t rmt_result = rmt_step_motor_smoothstep(rmt_step_motor, steps, MIN_STEPS_PER_SECOND, MAX_STEPS_PER_SECOND);
   task_delay_ms((uint32_t)(feeder_reg->vars.feeder_time_sec*1000));
   gpio_set_level(GPIO_OUTPUT_STEP_MOTOR_EN, 1);    /*not active*/
   gpio_set_level(GPIO_OUTPUT_STEP_MOTOR_SLEEP, 0); /*not active*/
   if (rmt_result == ESP_OK){
      if(feeder_reg->vars.feeder_time_sec > 0.0f){
         result = 1;
      }else{
         result = 0;
      }
   }else{
      result = -1;
   }
   return result;
}

static inline float helper_smootherstep_clamp(float x, float lowerlimit, float upperlimit)
{
   if (x < lowerlimit){
      x = lowerlimit;
   }
   if (x > upperlimit){
      x = upperlimit;
   }
   return x;
}

static float helper_smootherstep(float edge0, float edge1, float x)
{
   // Scale, and clamp x to 0..1 range
   x = helper_smootherstep_clamp((x - edge0) / (edge1 - edge0), 0.0, 1.0);
   // Evaluate polynomial
   return x * x * x * (x * (x * 6 - 15) + 10) * (edge1 - edge0) + edge0;
}

static uint16_t helper_speed_to_duration(uint16_t speed)
{
   return (uint16_t)round(1.0 * 1000 * 1000 / speed);
}

static esp_err_t helper_fill_rmt_items(rmt_item32_t *items, uint32_t speed)
{
   items->duration1 = 100;
   items->level1 = 1;
   items->level0 = 0;
   uint32_t delay_period = helper_speed_to_duration(speed);
   if (delay_period <= 100) {
      main_printf(TAG, "maximum rate reached, driver will generate another possible highest rate instead");
      items->duration0 = 100;
   }   else   {
      items->duration0 = delay_period;
   }
   return ESP_OK;
}


static esp_err_t rmt_step_motor_smoothstep(rmt_step_motor_t *rmt_handle, uint32_t n, uint32_t speed_min,
                                           uint32_t speed_max)
{
   esp_err_t ret = ESP_OK;
   if ((speed_min <= speed_max) && (n > SPEED_STEPS * 2) && (helper_speed_to_duration(speed_max) > 1)){
      if (ESP_OK == rmt_tx_stop(rmt_handle->rmt_config->channel)){
         // prepare speed tables
         for (int i = 0; i < SPEED_STEPS; ++i){
            helper_fill_rmt_items(&rmt_handle->rmt_items_speedup[i],
                                 (uint16_t)helper_smootherstep(
                                    (float)speed_min,
                                    (float)speed_max,
                                    (float)speed_min + ((float)i / (float)SPEED_STEPS) * (float)(speed_max - speed_min)));
         }
         for (int i = 0; i < SPEED_STEPS; ++i){
            helper_fill_rmt_items(&rmt_handle->rmt_items_speeddown[i],
                                 speed_max + speed_min - (uint16_t)helper_smootherstep((float)speed_min, (float)speed_max, (float)speed_min + ((float)i / (float)SPEED_STEPS) * (float)(speed_max - speed_min)));
         }
         rmt_handle->rmt_items_smoothstep_count = SPEED_STEPS;
         // prepare continuous phase rmt payload
         helper_fill_rmt_items(&rmt_handle->rmt_items_loop, speed_max);
         rmt_handle->rmt_items_loop_count = n - SPEED_STEPS * 2;
         // set status to be checked inside ISR
         rmt_handle->status = SMOOTH_SPEED_UP;
         // start transmitting
         ESP_ERROR_CHECK(rmt_write_items(rmt_handle->rmt_config->channel, rmt_handle->rmt_items_speedup, SPEED_STEPS, false));
         // waiting for transfer done
         semaphore_take(rmt_handle->notify_semphr, portMAX_DELAY);
      }   else   {
         main_error_message(TAG, "failed to stop rmt tx");
         ret = ESP_FAIL;
      }
   }
   return ret;
}

static IRAM_ATTR void rmt_tx_loop_intr(rmt_channel_t channel, void *args)
{
   rmt_step_motor_t *rmt_step_motor = (rmt_step_motor_t *)args;

   // smoothstep speedup stage finished
   if (rmt_step_motor->status == SMOOTH_SPEED_UP)
   {
      rmt_step_motor->status = SMOOTH_KEEP_SPEED;
      rmt_set_tx_loop_mode(rmt_step_motor->rmt_config->channel, true);
      rmt_enable_tx_loop_autostop(rmt_step_motor->rmt_config->channel, true);
      rmt_set_tx_intr_en(rmt_step_motor->rmt_config->channel, 0);
      // continue and configure loop count
   }

   if (rmt_step_motor->status == SMOOTH_KEEP_SPEED || rmt_step_motor->status == LIMITED_LOOP)
   {
      // loop count not 0, continuing looping
      if ((rmt_step_motor->rmt_items_loop_count) != 0)
      {
         if ((rmt_step_motor->rmt_items_loop_count) > 1023)
         {
            (rmt_step_motor->rmt_items_loop_count) -= 1023;
            rmt_set_tx_loop_count(rmt_step_motor->rmt_config->channel, 1023);
         }
         else
         {
            rmt_set_tx_loop_count(rmt_step_motor->rmt_config->channel, rmt_step_motor->rmt_items_loop_count);
            rmt_step_motor->rmt_items_loop_count = 0;
         }
         rmt_write_items(rmt_step_motor->rmt_config->channel, &rmt_step_motor->rmt_items_loop, 1, false);
         return;
      }
   }

   // smoothstep keep speed stage finished
   if (rmt_step_motor->status == SMOOTH_KEEP_SPEED)
   {
      rmt_step_motor->status = SMOOTH_SLOW_DOWN;
      rmt_set_tx_loop_mode(rmt_step_motor->rmt_config->channel, false);
      rmt_enable_tx_loop_autostop(rmt_step_motor->rmt_config->channel, false);
      rmt_set_tx_intr_en(rmt_step_motor->rmt_config->channel, 1);
      rmt_write_items(rmt_step_motor->rmt_config->channel, rmt_step_motor->rmt_items_speeddown, rmt_step_motor->rmt_items_smoothstep_count, false);
      return;
   }

   if (rmt_step_motor->status == LIMITED_LOOP || rmt_step_motor->status == SMOOTH_SLOW_DOWN)
   {
      rmt_step_motor->status = STOPPED;
      BaseType_t xHigherPriorityTaskWoken = pdFALSE;
      semaphore_release_ISR(rmt_step_motor->notify_semphr, &xHigherPriorityTaskWoken);
      /*if (xHigherPriorityTaskWoken == pdTRUE)
      {
         portYIELD_FROM_ISR();
      }*/
   }
}
/*all args preallocated*/
static esp_err_t step_motor_create_rmt(rmt_step_motor_t * rmt_step_motor)
{
   if (rmt_step_motor != NULL)
   {
      ESP_RETURN_ON_ERROR(rmt_config(rmt_step_motor->rmt_config), TAG, "Failed to configure RMT");
      ESP_RETURN_ON_ERROR(rmt_driver_install(rmt_step_motor->rmt_config->channel, 0, 0), TAG, "Failed to install RMT driver");
      semaphore_create_binary(rmt_step_motor->notify_semphr)
      // register tx end callback function, which got invoked when tx loop comes to the end
      rmt_register_tx_end_callback(rmt_tx_loop_intr, rmt_step_motor);
      return ESP_OK;
   }
   else
   {
      return ESP_ERR_INVALID_ARG;
   }
}
static esp_err_t step_motor_delete_rmt(rmt_step_motor_t * rmt_step_motor)
{
   semaphore_delete(rmt_step_motor->notify_semphr);
   if (ESP_OK!=rmt_driver_uninstall(rmt_step_motor->rmt_config->channel)){
      main_error_message(TAG, "Failed to uninstall RMT driver");
      return ESP_FAIL;
   }
   return ESP_OK;
}
#endif //FEEDER
#endif /*FEEDER_C*/
