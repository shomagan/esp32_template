/**
 * @file polisher.c
 * @author Shoma Gane <shomagan@gmail.com>
 *         
 * @defgroup main
 * @ingroup main
 * @version 0.1 
 * @brief  TODO!!! write brief in 
 */
#ifndef POLISHER_C
#define POLISHER_C 1

#include "polisher.h"
#include "pin_map.h"
#include "hal/gpio_hal.h"
#include "esp_log.h"
#include "esp_check.h"
#include "soc/rtc.h"
#include "driver/rmt.h"/*use rmt as step controller*/

#include "hal/rmt_types.h"
#include <math.h>
#include "sleep_control.h"
#include "regs_description.h"
#include "mirror_storage.h"


#define POLISHER_TASK_PERIOD (1000u)
#define RMT_TX_CHANNEL RMT_CHANNEL_0
#define SPEED_STEPS 50u

task_handle_t polisher_handle_id = NULL;
static const char *TAG = "polisher";
typedef enum {
    STOPPED = 0,
    SMOOTH_SPEED_UP,
    SMOOTH_KEEP_SPEED,
    SMOOTH_SLOW_DOWN,
    UNLIMITED_LOOP,
    LIMITED_LOOP,
} rmt_step_motor_running_status;

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


#if POLISHER
static int polisher_init(rmt_step_motor_t * rmt_step_motor);
static int polisher_deinit(rmt_step_motor_t * rmt_step_motor);
static inline float helper_smootherstep_clamp(float x, float lowerlimit, float upperlimit);
static float helper_smootherstep(float edge0, float edge1, float x);
static uint16_t helper_speed_to_duration(uint16_t speed);
static esp_err_t helper_fill_rmt_items(rmt_item32_t *items, uint32_t speed);
static esp_err_t rmt_step_motor_smoothstep(rmt_step_motor_t *rmt_handle, uint32_t n, uint32_t speed_min,uint32_t speed_max);
static IRAM_ATTR void rmt_tx_loop_intr(rmt_channel_t channel, void *args);
static esp_err_t step_motor_create_rmt(rmt_step_motor_t * rmt_step_motor);
static esp_err_t step_motor_delete_rmt(rmt_step_motor_t * rmt_step_motor);
static esp_err_t rmt_step_motor_step_impl(rmt_step_motor_t *rmt_handle, uint32_t n, uint32_t speed);
static esp_err_t rmt_step_motor_step(rmt_step_motor_t *rmt_handle, uint32_t n, uint32_t speed);


static int polisher_init(rmt_step_motor_t * rmt_step_motor){
   int result = 0;
   regs_global->vars.current_state[0] |= CS0_TASK_ACTIVE_POLISHER;
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
static int polisher_deinit(rmt_step_motor_t * rmt_step_motor){
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
   step_motor_delete_rmt(rmt_step_motor);
   regs_global->vars.current_state[0] &= ~((u32)CS0_TASK_ACTIVE_POLISHER);
   return result;
}
void polisher_task(void *arg){
   (void)(*arg);
   uint32_t signal_value;
   rmt_config_t dev_config = RMT_DEFAULT_CONFIG_TX(GPIO_OUTPUT_STEP_MOTOR_STEP0, RMT_TX_CHANNEL);
   rmt_step_motor_t rmt_step_motor;
   rmt_step_motor.rmt_config = &dev_config;     
   polisher_init(&rmt_step_motor);
   u64 task_counter = 0u;
   u16 polisher_speed;        //!< "speed" 
   u16 polisher_direction;    //!< "reserved"
   while(1){
      if(task_notify_wait(STOP_CHILD_PROCCES, &signal_value, POLISHER_TASK_PERIOD)==pdTRUE){
         /*by signal*/
         if (signal_value & STOP_CHILD_PROCCES){
            polisher_deinit(&rmt_step_motor);
            task_delete(task_get_id());
         }
      }
      regs_copy_safe(&polisher_speed,&polisher_reg->vars.polisher_speed,sizeof(polisher_speed));
      regs_copy_safe(&polisher_direction,&polisher_reg->vars.polisher_direction,sizeof(polisher_direction));
      esp_err_t step_result;
      if (polisher_speed){
         if (polisher_direction){
            gpio_set_level(GPIO_OUTPUT_STEP_MOTOR_DIR0, TRUE);
         }else{
            gpio_set_level(GPIO_OUTPUT_STEP_MOTOR_DIR0, FALSE);
         }
         gpio_set_level(GPIO_OUTPUT_STEP_MOTOR_EN, 0);    /*active*/
         gpio_set_level(GPIO_OUTPUT_STEP_MOTOR_SLEEP, 1); /*active*/
         task_delay_ms(1);
         step_result = rmt_step_motor_step(&rmt_step_motor, UINT32_MAX, polisher_speed);
         polisher_reg->vars.polisher_sec += 1;
         polisher_reg->vars.polisher_last_sec += 1;
      }else{
         gpio_set_level(GPIO_OUTPUT_STEP_MOTOR_EN, 1);    /*not active*/
         gpio_set_level(GPIO_OUTPUT_STEP_MOTOR_SLEEP, 0); /*not active*/
         step_result = rmt_step_motor_step(&rmt_step_motor, 0, polisher_speed);
      }
      if (step_result != ESP_OK){
         main_error_message(TAG,"rmt_step_motor_step problem");
      }
      task_counter++;
   }
}
// assume n != 0 and speed is within considerable range
static esp_err_t rmt_step_motor_step_impl(rmt_step_motor_t *rmt_handle, uint32_t n, uint32_t speed)
{
   rmt_set_tx_loop_mode(rmt_handle->rmt_config->channel, true);
   rmt_enable_tx_loop_autostop(rmt_handle->rmt_config->channel, true);
   rmt_handle->rmt_items_loop_count = n;
   if ((rmt_handle->rmt_items_loop_count) > 1023)
   {
      (rmt_handle->rmt_items_loop_count) -= 1023;
      rmt_set_tx_loop_count(rmt_handle->rmt_config->channel, 1023);
   }
   else
   {
      rmt_set_tx_loop_count(rmt_handle->rmt_config->channel, rmt_handle->rmt_items_loop_count);
      rmt_handle->rmt_items_loop_count = 0;
   }
   helper_fill_rmt_items(&rmt_handle->rmt_items_loop, speed);
   rmt_handle->status = LIMITED_LOOP;
   rmt_write_items(rmt_handle->rmt_config->channel, &rmt_handle->rmt_items_loop, 1, false);
   semaphore_take(rmt_handle->notify_semphr, portMAX_DELAY);
   return ESP_OK;
}
static esp_err_t rmt_step_motor_step(rmt_step_motor_t *rmt_handle, uint32_t n, uint32_t speed)
{
   esp_err_t result = ESP_FAIL;
   if (ESP_OK == rmt_tx_stop(rmt_handle->rmt_config->channel))
   {
      if (n == UINT32_MAX)
      { // forever loop, non-blocking
         rmt_set_tx_loop_count(rmt_handle->rmt_config->channel, 0);
         rmt_enable_tx_loop_autostop(rmt_handle->rmt_config->channel, false);
         rmt_set_tx_loop_mode(rmt_handle->rmt_config->channel, true);
         helper_fill_rmt_items(&rmt_handle->rmt_items_loop, speed);
         rmt_handle->status = UNLIMITED_LOOP;
         ESP_ERROR_CHECK(rmt_write_items(rmt_handle->rmt_config->channel, &rmt_handle->rmt_items_loop, 1, false));
         result = ESP_OK;
      }
      else if (n == 0)
      { // break the forever loop
         rmt_handle->status = STOPPED;
         rmt_tx_stop(rmt_handle->rmt_config->channel);
         ESP_ERROR_CHECK(rmt_set_tx_loop_mode(rmt_handle->rmt_config->channel, false));
         result = ESP_OK;
      }
      else
      { // normally move n steps
         if (helper_speed_to_duration(speed) > 1){
            result =  rmt_step_motor_step_impl(rmt_handle, n, speed);
         }
      }
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
#endif //POLISHER
#endif /*POLISHER_C*/
