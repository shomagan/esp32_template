/**
 * @file scd41.c
 * @brief Implementation of SCD41 sensor task and driver.
 */

#ifndef SCD41_C
#define SCD41_C 1

#include "type_def.h"
#include "main_config.h"
#include "regs.h"
#include "common.h"
#include "pin_map.h"
#include "sleep_control.h"
#include "regs_description.h"
#include "mirror_storage.h"
#include "driver/i2c.h"
#include "scd41.h"
#include "telegram.h"

#define SCD41_TASK_PERIOD_MS 20000u
#define SCD41_I2C_ADDRESS 0x62
#define SCD41_CMD_START_MEASUREMENT 0x21B1
#define SCD41_CMD_READ_MEASUREMENT 0xEC05
#define SLEEP_IS_ACTIVE 1
#define MIN_WORK_TIME_MINUTE 2

static const char *TAG = "scd41";
task_handle_t scd41_task_handle = NULL;

static int scd41_task_init(void);
static int scd41_task_deinit(void);
static int handle_sleeping(u32 minutes_of_the_day);
static esp_err_t scd41_driver_init(void);
static esp_err_t scd41_start_measurement(void);
static esp_err_t scd41_read_measurement(uint16_t *co2_ppm, float *humidity, float *temperature);

static int scd41_task_init(void){
   int result = 0;
   regs_global->vars.current_state[0] |= CS0_TASK_ACTIVE_SCD41;
   scd41_driver_init(); // Initialize the SCD41 driver
   return result;
}

static int scd41_task_deinit(void){
   int result = 0;
   regs_global->vars.current_state[0] &= ~((u32)CS0_TASK_ACTIVE_SCD41);
   return result;
}

void scd41_task(void *arg){
   (void)(*arg);
   uint32_t signal_value;
   u64 task_counter = 0u;
   scd41_task_init();
   u8 message_sent = 0u;

   while (1)   {
      if (task_notify_wait(SLEEP_TASK_STOP_CHILD_PROCCES, &signal_value, SCD41_TASK_PERIOD_MS) == pdTRUE)      {
         if (signal_value & SLEEP_TASK_STOP_CHILD_PROCCES)         {
            scd41_task_deinit();
            task_delete(task_get_id());
         }
      }
      struct timeval tv;
      if (gettimeofday(&tv, NULL) != 0)      {
         main_error_message(TAG, "Failed to obtain time");
      } else {
         u32 minutes_of_the_day = tv.tv_sec / 60;
         uint16_t co2_level = 0;
         float humidity = 0.0f;
         float temperature = 0.0f;
         scd41_start_measurement();
         task_delay_ms(5000); // Wait for measurement to stabilize
         if (scd41_read_measurement(&co2_level, &humidity, &temperature) == ESP_OK)         {
            scd41_reg->vars.scd41_co2_level = co2_level;
            scd41_reg->vars.scd41_humidity = humidity;
            scd41_reg->vars.scd41_temperature = temperature;
            main_debug(TAG, "CO2 Level: %u ppm, Humidity: %.2f%%, Temperature: %.2f°C", co2_level, humidity, temperature);
            if(message_sent == 0u) {
               u16 reg_ind = (u16)regs_description_get_index_by_address(&scd41_reg->vars.scd41_co2_level);
               u8 reg_num = 3u; // Number of registers to send
               u32 signal = (u32)TELEGRAM_SEND_REG_BY_ID | ((reg_ind << 16u) & 0xffff0000) | ((reg_num << 8u) & 0xff00);
               u32 prev_value = 0;
               task_notify_send(telegram_handle_id, signal, &prev_value);
               message_sent = 1u; // Set flag to indicate message has been sent
            }
         } else {
            main_error_message(TAG, "Failed to read CO2 level");
         }
         handle_sleeping(minutes_of_the_day);
         main_debug(TAG, "sec of the day %u", tv.tv_sec);
      }

      task_counter++;
   }
}

static int handle_sleeping(u32 minutes_of_the_day){
   int result = 0;
   u32 interval_minute = scd41_reg->vars.scd41_measur_interval;
   if (interval_minute > MIN_WORK_TIME_MINUTE)   {
      u32 time_gap = minutes_of_the_day % interval_minute;
      if (time_gap >= MIN_WORK_TIME_MINUTE)      {
         u32 sleep_time_minutes = interval_minute - time_gap;
#if SLEEP_IS_ACTIVE
         if (sleep_time_minutes)         {
            u32 sleep_time_sec = sleep_time_minutes * 60;
            u32 prev_value = 0;
            u32 signal = (u32)SLEEP_TASK_DEEP_SLEEP_FOR_N_SEC | ((sleep_time_sec << 16u) & 0xffff0000);
            task_notify_send(sleep_control_handle_id, signal, &prev_value);
         }
#endif
      } // do nothing,dont sleep
   } // do nothing,dont sleep
   return result;
}

static esp_err_t scd41_driver_init(void){
   // Initialize I2C (configure pins, frequency, etc.)
   i2c_config_t conf = {
       .mode = I2C_MODE_MASTER,
       .sda_io_num = PIN_SDA,
       .scl_io_num = PIN_SCL,
       .sda_pullup_en = GPIO_PULLUP_ENABLE,
       .scl_pullup_en = GPIO_PULLUP_ENABLE,
       .master.clk_speed = 100000, // 100 kHz
   };
   i2c_param_config(I2C_NUM_0, &conf);
   i2c_driver_install(I2C_NUM_0, conf.mode, 0, 0, 0);
   return ESP_OK;
}

static esp_err_t scd41_start_measurement(void){
   uint8_t cmd[] = {(SCD41_CMD_START_MEASUREMENT >> 8) & 0xFF, SCD41_CMD_START_MEASUREMENT & 0xFF};
   i2c_cmd_handle_t handle = i2c_cmd_link_create();
   i2c_master_start(handle);
   i2c_master_write_byte(handle, (SCD41_I2C_ADDRESS << 1) | I2C_MASTER_WRITE, true);
   i2c_master_write(handle, cmd, sizeof(cmd), true);
   i2c_master_stop(handle);
   esp_err_t ret = i2c_master_cmd_begin(I2C_NUM_0, handle, pdMS_TO_TICKS(1000));
   i2c_cmd_link_delete(handle);
   return ret;
}

static esp_err_t scd41_read_measurement(uint16_t *co2_ppm, float *humidity, float *temperature) {
   uint8_t cmd[] = {(SCD41_CMD_READ_MEASUREMENT >> 8) & 0xFF, SCD41_CMD_READ_MEASUREMENT & 0xFF};
   uint8_t data[9]; // 6 bytes for CO2, temperature, humidity + 3 CRC bytes

   // Send read measurement command
   i2c_cmd_handle_t handle = i2c_cmd_link_create();
   i2c_master_start(handle);
   i2c_master_write_byte(handle, (SCD41_I2C_ADDRESS << 1) | I2C_MASTER_WRITE, true);
   i2c_master_write(handle, cmd, sizeof(cmd), true);
   i2c_master_stop(handle);
   esp_err_t ret = i2c_master_cmd_begin(I2C_NUM_0, handle, pdMS_TO_TICKS(1000));
   i2c_cmd_link_delete(handle);
   if (ret != ESP_OK)   {
      ESP_LOGE(TAG, "Failed to send read measurement command");
      return ret;
   }
   // Read measurement data
   handle = i2c_cmd_link_create();
   i2c_master_start(handle);
   i2c_master_write_byte(handle, (SCD41_I2C_ADDRESS << 1) | I2C_MASTER_READ, true);
   i2c_master_read(handle, data, sizeof(data), I2C_MASTER_LAST_NACK);
   i2c_master_stop(handle);
   ret = i2c_master_cmd_begin(I2C_NUM_0, handle, pdMS_TO_TICKS(1000));
   i2c_cmd_link_delete(handle);
   if (ret != ESP_OK)   {
      main_error_message(TAG, "Failed to read measurement data");
      return ret;
   }

   // Parse CO2 value (first two bytes)
   *co2_ppm = (data[0] << 8) | data[1];
   main_debug(TAG, "CO2 Level: %u ppm", *co2_ppm);

   uint16_t raw_humidity = (data[6] << 8) | data[7];
   *humidity = ((float)raw_humidity * 100) / 65535.0;

   uint16_t raw_temperature = (data[3] << 8) | data[4];
   *temperature = -45.0 + ((float)raw_temperature * 175.0) / 65535.0;

   return ESP_OK;
}

#endif /* SCD41_C */
