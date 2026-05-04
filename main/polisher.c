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
#include "driver/rmt_tx.h"/*use rmt as step controller*/
#include "driver/rmt_encoder.h"
#include "driver/rmt_types.h"
#include <limits.h>
#include <math.h>
#include "sleep_control.h"
#include "regs_description.h"
#include "mirror_storage.h"


#define POLISHER_TASK_PERIOD (1000u)
#define RMT_RESOLUTION_HZ (1000000u)
#define SPEED_STEPS 50u

task_handle_t polisher_handle_id = NULL;
static const char *TAG = "polisher";
typedef enum {
    STOPPED = 0,
    UNLIMITED_LOOP,
    LIMITED_LOOP,
} rmt_step_motor_running_status;

typedef struct {
   rmt_channel_handle_t tx_chan;
   rmt_encoder_handle_t copy_encoder;
   rmt_step_motor_running_status status;
   rmt_symbol_word_t rmt_items_loop;
   uint32_t rmt_items_loop_count;
   uint32_t current_speed;
} rmt_step_motor_t;


#if POLISHER
static int polisher_init(rmt_step_motor_t * rmt_step_motor);
static int polisher_deinit(rmt_step_motor_t * rmt_step_motor);
static uint16_t helper_speed_to_duration(uint16_t speed);
static esp_err_t helper_fill_rmt_items(rmt_symbol_word_t *items, uint32_t speed);
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
   (void)arg;
   uint32_t signal_value;
   rmt_step_motor_t rmt_step_motor = {0};
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
   if (rmt_handle == NULL) {
      return ESP_ERR_INVALID_ARG;
   }
   if ((helper_speed_to_duration(speed) <= 1) || (n > INT_MAX)) {
      return ESP_ERR_INVALID_ARG;
   }

   helper_fill_rmt_items(&rmt_handle->rmt_items_loop, speed);
   rmt_handle->rmt_items_loop_count = n;
   rmt_handle->status = LIMITED_LOOP;
   rmt_handle->current_speed = speed;

   rmt_transmit_config_t tx_cfg = {
      .loop_count = (int)n,
   };
   ESP_RETURN_ON_ERROR(rmt_transmit(rmt_handle->tx_chan,
                                    rmt_handle->copy_encoder,
                                    &rmt_handle->rmt_items_loop,
                                    sizeof(rmt_handle->rmt_items_loop),
                                    &tx_cfg), TAG, "failed to transmit finite step loop");
   ESP_RETURN_ON_ERROR(rmt_tx_wait_all_done(rmt_handle->tx_chan, -1), TAG, "failed while waiting finite step loop");

   rmt_handle->status = STOPPED;
   return ESP_OK;
}
static esp_err_t rmt_step_motor_step(rmt_step_motor_t *rmt_handle, uint32_t n, uint32_t speed)
{
   esp_err_t result = ESP_FAIL;
   if (rmt_handle == NULL) {
      return ESP_ERR_INVALID_ARG;
   }

   if (n == UINT32_MAX)
   { // forever loop, non-blocking
      if (helper_speed_to_duration(speed) <= 1) {
         return ESP_ERR_INVALID_ARG;
      }
      if ((rmt_handle->status != UNLIMITED_LOOP) || (rmt_handle->current_speed != speed)) {
         ESP_RETURN_ON_ERROR(rmt_disable(rmt_handle->tx_chan), TAG, "failed to stop previous transmission");
         ESP_RETURN_ON_ERROR(rmt_enable(rmt_handle->tx_chan), TAG, "failed to re-enable channel");
         helper_fill_rmt_items(&rmt_handle->rmt_items_loop, speed);
         rmt_transmit_config_t tx_cfg = {
            .loop_count = -1,
         };
         ESP_RETURN_ON_ERROR(rmt_transmit(rmt_handle->tx_chan,
                                          rmt_handle->copy_encoder,
                                          &rmt_handle->rmt_items_loop,
                                          sizeof(rmt_handle->rmt_items_loop),
                                          &tx_cfg), TAG, "failed to start infinite loop transmission");
         rmt_handle->status = UNLIMITED_LOOP;
         rmt_handle->current_speed = speed;
      }
      result = ESP_OK;
   }
   else if (n == 0)
   { // break the forever loop
      if (rmt_handle->status == UNLIMITED_LOOP) {
         ESP_RETURN_ON_ERROR(rmt_disable(rmt_handle->tx_chan), TAG, "failed to disable channel");
         ESP_RETURN_ON_ERROR(rmt_enable(rmt_handle->tx_chan), TAG, "failed to re-enable channel after stop");
      }
      rmt_handle->status = STOPPED;
      rmt_handle->current_speed = 0;
      result = ESP_OK;
   }
   else
   { // normally move n steps
      if (rmt_handle->status == UNLIMITED_LOOP) {
         ESP_RETURN_ON_ERROR(rmt_disable(rmt_handle->tx_chan), TAG, "failed to disable channel before finite move");
         ESP_RETURN_ON_ERROR(rmt_enable(rmt_handle->tx_chan), TAG, "failed to re-enable channel before finite move");
         rmt_handle->status = STOPPED;
         rmt_handle->current_speed = 0;
      }
      if (helper_speed_to_duration(speed) > 1){
         result =  rmt_step_motor_step_impl(rmt_handle, n, speed);
      }
   }
   return result;
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
      main_printf(TAG, "maximum rate reached, driver will generate another possible highest rate instead");
      items->duration0 = 100;
   }   else   {
      items->duration0 = delay_period;
   }
   return ESP_OK;
}
/*all args preallocated*/
static esp_err_t step_motor_create_rmt(rmt_step_motor_t * rmt_step_motor)
{
   if (rmt_step_motor == NULL)
   {
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
      if (rmt_step_motor->status == UNLIMITED_LOOP) {
         (void)rmt_disable(rmt_step_motor->tx_chan);
      } else {
         (void)rmt_tx_wait_all_done(rmt_step_motor->tx_chan, -1);
         (void)rmt_disable(rmt_step_motor->tx_chan);
      }
   }

   if (rmt_step_motor->copy_encoder != NULL) {
      ESP_RETURN_ON_ERROR(rmt_del_encoder(rmt_step_motor->copy_encoder), TAG, "Failed to delete RMT encoder");
      rmt_step_motor->copy_encoder = NULL;
   }
   if (rmt_step_motor->tx_chan != NULL) {
      ESP_RETURN_ON_ERROR(rmt_del_channel(rmt_step_motor->tx_chan), TAG, "Failed to delete RMT channel");
      rmt_step_motor->tx_chan = NULL;
   }

   rmt_step_motor->status = STOPPED;
   rmt_step_motor->current_speed = 0;
   return ESP_OK;
}
#endif //POLISHER
#endif /*POLISHER_C*/
