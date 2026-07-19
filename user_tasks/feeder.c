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
#include "driver/rmt_tx.h"/*use rmt as step controller*/
#include "driver/rmt_encoder.h"
#include <math.h>
#include <stdlib.h>
#include "sleep_control.h"
#include "regs_description.h"
#include "mirror_storage.h"
#include "link_functions.h"

#define FEEDER_TASK_PERIOD (1000u)
#define RMT_RESOLUTION_HZ (1000000u)
#define SPEED_STEPS 50u
#define MIN_STEPS_PER_SECOND (15*32)
#define MAX_STEPS_PER_SECOND (30*32)
#define AVERAGE_STEPS_PER_SECOND ((MIN_STEPS_PER_SECOND + MAX_STEPS_PER_SECOND)/2)
#define MIN_WORK_TIME_MINUTE 1
#define MAX_SLEEP_TIME_MINUTE 240
#define SLEEP_IS_ACTIVE 1


typedef struct {
   rmt_channel_handle_t tx_chan;
   rmt_encoder_handle_t copy_encoder;
   rmt_symbol_word_t rmt_items_loop;
   uint32_t rmt_items_loop_count;
   rmt_symbol_word_t rmt_items_speedup[SPEED_STEPS];
   rmt_symbol_word_t rmt_items_speeddown[SPEED_STEPS];
   uint32_t rmt_items_smoothstep_count;
} rmt_step_motor_t;

task_handle_t feeder_handle_id = NULL;
static const char *TAG = "feeder";


#if FEEDER
static int feeder_init(rmt_step_motor_t * rmt_step_motor);
static int feeder_deinit(rmt_step_motor_t * rmt_step_motor);
static inline float helper_smootherstep_clamp(float x, float lowerlimit, float upperlimit);
static float helper_smootherstep(float edge0, float edge1, float x);
static uint16_t helper_speed_to_duration(uint16_t speed);
static esp_err_t helper_fill_rmt_items(rmt_symbol_word_t *items, uint32_t speed);
static esp_err_t rmt_step_motor_smoothstep(rmt_step_motor_t *rmt_handle, uint32_t n, uint32_t speed_min,uint32_t speed_max);
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
   (void)arg;
   uint32_t signal_value;
   u64 task_counter = 0u;
   rmt_step_motor_t rmt_step_motor = {0};
   feeder_init(&rmt_step_motor);
   link_functions.os_thread_delay(1000);
   int feeded_minute = -1;
   while(1){
      if(link_functions.os_thread_signal_wait(FEEDER_TASK_STOP_CHILD_PROCCES, &signal_value, FEEDER_TASK_PERIOD)==pdTRUE){
         /*by signal*/
         if (signal_value & FEEDER_TASK_STOP_CHILD_PROCCES){
            feeder_deinit(&rmt_step_motor);
            link_functions.os_thread_terminate(link_functions.os_thread_get_id());
         }
         if (signal_value & FEEDER_TASK_ONE_FEED_COMM){
            run_feeder(&rmt_step_motor);
         }
      }
      struct timeval tv;
      if (gettimeofday(&tv, NULL)!= 0) {
         link_functions.os_log_error(TAG,"Failed to obtain time");
      }else{
         u32 minutes_of_the_day = tv.tv_sec/60;
         handle_feeding(minutes_of_the_day, &feeded_minute,&rmt_step_motor);
         handle_sleeping(minutes_of_the_day);
         link_functions.os_log_debug(TAG,"sec of the day %u",tv.tv_sec);
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
            link_functions.os_thread_signal_set(sleep_control_handle_id, signal, &prev_value);
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
            regs_template_t regs_template = {0};
            regs_template.p_value = &feeder_reg->vars.feeder_counter;

            result = regs_description_get_by_address(&regs_template);
            if (result >= 0){
               if (mirror_access_write(&regs_template)>=0){
                  link_functions.os_log_debug(TAG, "succes wrote feeder_counter to mirror");
               }else{
                  link_functions.os_log_debug(TAG, "unsucces writing feeder_counter to mirror");
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
   link_functions.os_thread_delay(1);
   u32 steps = (u32)(feeder_reg->vars.feeder_time_sec * AVERAGE_STEPS_PER_SECOND);
   esp_err_t rmt_result = rmt_step_motor_smoothstep(rmt_step_motor, steps, MIN_STEPS_PER_SECOND, MAX_STEPS_PER_SECOND);
   link_functions.os_thread_delay((uint32_t)(feeder_reg->vars.feeder_time_sec*1000));
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

static esp_err_t helper_fill_rmt_items(rmt_symbol_word_t *items, uint32_t speed)
{
   items->duration1 = 100;
   items->level1 = 1;
   items->level0 = 0;
   uint32_t delay_period = helper_speed_to_duration(speed);
   if (delay_period <= 100) {
      link_functions.os_log_info(TAG, "maximum rate reached, driver will generate another possible highest rate instead");
      items->duration0 = 100;
   }   else   {
      items->duration0 = delay_period;
   }
   return ESP_OK;
}


static esp_err_t rmt_step_motor_smoothstep(rmt_step_motor_t *rmt_handle, uint32_t n, uint32_t speed_min,
                                           uint32_t speed_max)
{
   if (rmt_handle == NULL) {
      return ESP_ERR_INVALID_ARG;
   }
   if ((speed_min > speed_max) || (n <= SPEED_STEPS * 2) || (helper_speed_to_duration(speed_max) <= 1)) {
      return ESP_ERR_INVALID_ARG;
   }

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
   helper_fill_rmt_items(&rmt_handle->rmt_items_loop, speed_max);
   rmt_handle->rmt_items_loop_count = n - SPEED_STEPS * 2;

   rmt_transmit_config_t tx_cfg = {
      .loop_count = 0,
   };

   ESP_RETURN_ON_ERROR(rmt_transmit(rmt_handle->tx_chan,
                                    rmt_handle->copy_encoder,
                                    rmt_handle->rmt_items_speedup,
                                    sizeof(rmt_handle->rmt_items_speedup),
                                    &tx_cfg), TAG, "failed to transmit speed-up phase");
   ESP_RETURN_ON_ERROR(rmt_tx_wait_all_done(rmt_handle->tx_chan, -1), TAG, "failed while waiting speed-up phase");

   if (rmt_handle->rmt_items_loop_count > 0) {
      size_t loop_bytes = (size_t)rmt_handle->rmt_items_loop_count * sizeof(rmt_symbol_word_t);
      rmt_symbol_word_t *loop_items = (rmt_symbol_word_t *)calloc(rmt_handle->rmt_items_loop_count, sizeof(rmt_symbol_word_t));
      ESP_RETURN_ON_FALSE(loop_items != NULL, ESP_ERR_NO_MEM, TAG, "failed to allocate loop symbols");
      for (uint32_t i = 0; i < rmt_handle->rmt_items_loop_count; ++i) {
         loop_items[i] = rmt_handle->rmt_items_loop;
      }
      esp_err_t tx_ret = rmt_transmit(rmt_handle->tx_chan,
                                      rmt_handle->copy_encoder,
                                      loop_items,
                                      loop_bytes,
                                      &tx_cfg);
      if (tx_ret == ESP_OK) {
         tx_ret = rmt_tx_wait_all_done(rmt_handle->tx_chan, -1);
      }
      free(loop_items);
      ESP_RETURN_ON_ERROR(tx_ret, TAG, "failed to transmit constant-speed phase");
   }

   ESP_RETURN_ON_ERROR(rmt_transmit(rmt_handle->tx_chan,
                                    rmt_handle->copy_encoder,
                                    rmt_handle->rmt_items_speeddown,
                                    sizeof(rmt_handle->rmt_items_speeddown),
                                    &tx_cfg), TAG, "failed to transmit slow-down phase");
   ESP_RETURN_ON_ERROR(rmt_tx_wait_all_done(rmt_handle->tx_chan, -1), TAG, "failed while waiting slow-down phase");

   return ESP_OK;
}
/*all args preallocated*/
static esp_err_t step_motor_create_rmt(rmt_step_motor_t * rmt_step_motor)
{
   if (rmt_step_motor == NULL) {
      return ESP_ERR_INVALID_ARG;
   }

   rmt_tx_channel_config_t tx_chan_cfg = {
      .gpio_num = GPIO_OUTPUT_STEP_MOTOR_STEP0,
      .clk_src = RMT_CLK_SRC_DEFAULT,
      .resolution_hz = RMT_RESOLUTION_HZ,
      .mem_block_symbols = 64,
      .trans_queue_depth = 2,
      .flags.invert_out = false,
      .flags.with_dma = false,
   };
   ESP_RETURN_ON_ERROR(rmt_new_tx_channel(&tx_chan_cfg, &rmt_step_motor->tx_chan), TAG, "Failed to create RMT TX channel");

   rmt_copy_encoder_config_t encoder_cfg = {};
   esp_err_t ret = rmt_new_copy_encoder(&encoder_cfg, &rmt_step_motor->copy_encoder);
   if (ret != ESP_OK) {
      rmt_del_channel(rmt_step_motor->tx_chan);
      rmt_step_motor->tx_chan = NULL;
      return ret;
   }

   ret = rmt_enable(rmt_step_motor->tx_chan);
   if (ret != ESP_OK) {
      rmt_del_encoder(rmt_step_motor->copy_encoder);
      rmt_step_motor->copy_encoder = NULL;
      rmt_del_channel(rmt_step_motor->tx_chan);
      rmt_step_motor->tx_chan = NULL;
      return ret;
   }

   return ESP_OK;
}
static esp_err_t step_motor_delete_rmt(rmt_step_motor_t * rmt_step_motor)
{
   if (rmt_step_motor == NULL) {
      return ESP_ERR_INVALID_ARG;
   }

   if (rmt_step_motor->tx_chan != NULL) {
      (void)rmt_tx_wait_all_done(rmt_step_motor->tx_chan, -1);
      (void)rmt_disable(rmt_step_motor->tx_chan);
   }

   if (rmt_step_motor->copy_encoder != NULL) {
      ESP_RETURN_ON_ERROR(rmt_del_encoder(rmt_step_motor->copy_encoder), TAG, "Failed to delete RMT encoder");
      rmt_step_motor->copy_encoder = NULL;
   }
   if (rmt_step_motor->tx_chan != NULL) {
      ESP_RETURN_ON_ERROR(rmt_del_channel(rmt_step_motor->tx_chan), TAG, "Failed to delete RMT channel");
      rmt_step_motor->tx_chan = NULL;
   }

   return ESP_OK;
}
#endif //FEEDER
#endif /*FEEDER_C*/
