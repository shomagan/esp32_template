/**
 * @file led_strip.c
 * @author Shoma Gane <shomagan@gmail.com>
 *
 * @defgroup main
 * @ingroup main
 * @version 0.1
 * @brief  TODO!!! write brief in
 */
#include "os_type.h"
#include "regs.h"
#ifndef LED_STRIP_C
#define LED_STRIP_C 1

#include "led_strip.h"
#include "regs_description.h"
#include "link_functions.h"
#include "pin_map.h"
#include <string.h>
#if CONFIG_IDF_TARGET_ESP32C3
#include "driver/spi_master.h"
#endif

task_handle_t led_strip_handle_id = NULL;
static const char *TAG = "led_strip";
#define LED_STRIP_TASK_PERIOD (1u)
#define LED_STRIP_MAX_LEDS (300u)
#define LED_STRIP_SPI_CLOCK_HZ (4000000u)
enum{
   LED_MODE_RUN,
   LED_MODE_FULL,
   LED_MODE_BRIM
};
/*sk9822/apa102 led strip control, driven over hw spi (data+clock, no chip select)*/
static const u32 def_table_version = 9u;
static const u32 def_led_number = 30u;
static const u32 def_led_brightness = 50u;
static const u32 def_led_color = 0x00FF00u; //red green blue
static const u32 def_delay = 50u;
static const u32 def_min_delay = 1u;
static const u32 def_smooth_number = 5;
static const u32 def_mode = LED_MODE_BRIM; //0 run, 1 full
/*#generator_use_description {"user_task_regs":"start_struct"}*/
static led_strip_reg_t led_strip_reg_storage = {{0}};
led_strip_reg_t * const led_strip_reg = &led_strip_reg_storage;
#define NUM_OF_LED_STRIP_REG_VARS 7
static u8 led_strip_reg_saved_buf[32];
static const char led_strip_reg_space_name[] = "led_strip_reg_t";
static regs_description_t const regs_description_led_strip_reg[NUM_OF_LED_STRIP_REG_VARS] = {
   { &def_mode, NULL, NULL, (u8*)&led_strip_reg->vars.mode, 0,"led strip mode, 1 run, 2 full","mode", 0x14018000, 0x31324, U32_REGS_FLAG, 1, 5, 12 },//!< "led strip mode, 1 run, 2 full" &save &def
   { &def_led_number, NULL, NULL, (u8*)&led_strip_reg->vars.led_number, 4,"number of leds in strip","led_number", 0x14018001, 0x31326, U32_REGS_FLAG, 1, 5, 12 },//!< "number of leds in strip" &save &def
   { &def_led_brightness, NULL, NULL, (u8*)&led_strip_reg->vars.led_brightness, 8,"brightness of leds in strip","led_brightness", 0x14018002, 0x31328, U32_REGS_FLAG, 1, 5, 12 },//!< "brightness of leds in strip" &save &def
   { &def_led_color, NULL, NULL, (u8*)&led_strip_reg->vars.led_color, 12,"color of leds in strip","led_color", 0x14018003, 0x3132a, U32_REGS_FLAG, 1, 5, 12 },//!< "color of leds in strip" &save &def
   { &def_delay, &def_min_delay, NULL, (u8*)&led_strip_reg->vars.delay, 16,"delay between led updates in ms","delay", 0x14018004, 0x3132c, U32_REGS_FLAG, 1, 69, 12 },//!< "delay between led updates in ms" &save &def &min
   { &def_smooth_number, NULL, NULL, (u8*)&led_strip_reg->vars.smooth_number, 20,"number of neighbor leds","smooth_number", 0x14018005, 0x3132e, U32_REGS_FLAG, 1, 5, 12 },//!< "number of neighbor leds" &save &def
   { &def_table_version, NULL, NULL, (u8*)&led_strip_reg->vars.table_version, 24,"table version, resets regs to defaults on mismatch","table_version", 0x14018006, 0x31330, U32_REGS_FLAG, 1, 7, 12 },//!< "table version, resets regs to defaults on mismatch" &ro &def &save
};
const regs_description_list_t regs_table_led_strip_reg = {
   .description = regs_description_led_strip_reg,
   .num_of_regs = NUM_OF_LED_STRIP_REG_VARS,
   .table_version = &def_table_version,
   .space_name = led_strip_reg_space_name,
   .saved_regs_buffer = led_strip_reg_saved_buf,
   .saved_regs_buffer_size = sizeof(led_strip_reg_saved_buf),
};
/*#generator_use_description {"user_task_regs":"end_struct"}*/

#if LED_STRIP
static int led_strip_init(void);
static int led_strip_deinit(void);

#if CONFIG_IDF_TARGET_ESP32C3
static spi_device_handle_t led_strip_spi = NULL;

/*apa102/sk9822 frame: 4x 0x00 start, then per-led (0xE0|5bit brightness),B,G,R, then >=led_number/2 bits of 1 as end frame*/
#define LED_STRIP_START_FRAME_LEN (4u)
#define LED_STRIP_END_FRAME_LEN ((LED_STRIP_MAX_LEDS + 15u) / 16u)
#define LED_STRIP_FRAME_BUF_LEN (LED_STRIP_START_FRAME_LEN + LED_STRIP_MAX_LEDS * 4u + LED_STRIP_END_FRAME_LEN)

static void led_strip_send_run(u32 led_number, u32 last_led_number, u32 brightness_pct, u32 color, u8 smooth_number) {
   static u8 frame_buf[LED_STRIP_FRAME_BUF_LEN];
   if (led_strip_spi != NULL) {
      u8 red = (u8)(color >> 16);
      u8 green = (u8)(color >> 8);
      u8 blue = (u8)(color);
      led_number = led_number > LED_STRIP_MAX_LEDS - 1? LED_STRIP_MAX_LEDS - 1:led_number;
      last_led_number = last_led_number > LED_STRIP_MAX_LEDS - 1? LED_STRIP_MAX_LEDS-1:last_led_number;
      brightness_pct = brightness_pct > 100u? 100: brightness_pct;
      u8 brightness_5bit = (u8)((brightness_pct * 31u) / 100u);
      u32 end_frame_len = ((last_led_number+1) + 15u) / 16u;
      u32 total_len = LED_STRIP_START_FRAME_LEN + (last_led_number+1) * 4u + end_frame_len;
      memset(frame_buf, 0x00, LED_STRIP_START_FRAME_LEN);
      for (u32 i = 0; i <= last_led_number; i++) {
         u8 *led = &frame_buf[LED_STRIP_START_FRAME_LEN + i * 4u];
         led[0] = 0xE0u;
         u32 min_led = led_number >= smooth_number ? led_number - smooth_number : last_led_number - (smooth_number - led_number);
         u32 max_led = led_number + smooth_number <= last_led_number ? led_number + smooth_number: smooth_number - (last_led_number - led_number);
         if(((i >= min_led) && (i <= max_led))||
            ((max_led < min_led) && ((i >= min_led) || (i<=max_led)))) {
            u16 diff = 0;
            if(max_led >= min_led){
               diff = (i >= led_number)?(i- led_number):(led_number-i);
            }else{
               if(i > min_led){
                  diff = (i - min_led) >= smooth_number ? (i - min_led) - smooth_number: smooth_number - (i - min_led);
               }else{
                  diff = (max_led - i) >= smooth_number ? (max_led - i) - smooth_number: smooth_number - (max_led - i);
               }
            }
            u8 brightness_5bit_smoothed = brightness_5bit - (diff * brightness_5bit)/smooth_number;
            led[0] |= brightness_5bit_smoothed;
         }
         led[1] = blue;
         led[2] = green;
         led[3] = red;
      }
      memset(&frame_buf[LED_STRIP_START_FRAME_LEN + (last_led_number+1) * 4u], 0x00, end_frame_len);
      spi_transaction_t trans = { 0 };
      trans.length = total_len * 8u;
      trans.tx_buffer = frame_buf;
      spi_device_transmit(led_strip_spi, &trans);
   }
}
static void led_strip_send_full(u32 last_led_number, u32 brightness_pct, u32 color) {
   static u8 frame_buf[LED_STRIP_FRAME_BUF_LEN];
   if (led_strip_spi != NULL) {
      u8 red = (u8)(color >> 16);
      u8 green = (u8)(color >> 8);
      u8 blue = (u8)(color);
      last_led_number = last_led_number > LED_STRIP_MAX_LEDS - 1? LED_STRIP_MAX_LEDS-1:last_led_number;
      brightness_pct = brightness_pct > 100u? 100: brightness_pct;
      u8 brightness_5bit = (u8)((brightness_pct * 31u) / 100u);
      u32 end_frame_len = ((last_led_number+1) + 15u) / 16u;
      u32 total_len = LED_STRIP_START_FRAME_LEN + (last_led_number+1) * 4u + end_frame_len;
      memset(frame_buf, 0x00, LED_STRIP_START_FRAME_LEN);
      for (u32 i = 0; i <= last_led_number; i++) {
         u8 *led = &frame_buf[LED_STRIP_START_FRAME_LEN + i * 4u];
         led[0] = 0xE0u;
         led[0] |= brightness_5bit;
         led[1] = blue;
         led[2] = green;
         led[3] = red;
      }
      memset(&frame_buf[LED_STRIP_START_FRAME_LEN + (last_led_number+1) * 4u], 0x00, end_frame_len);
      spi_transaction_t trans = { 0 };
      trans.length = total_len * 8u;
      trans.tx_buffer = frame_buf;
      spi_device_transmit(led_strip_spi, &trans);
   }
}
static void led_strip_send_brim(u32 last_led_number, u32 brightness_pct, u32 color) {
   static u8 frame_buf[LED_STRIP_FRAME_BUF_LEN];
   static u8 color_part = 0;
   if (led_strip_spi != NULL) {
      u8 red = (u8)(color >> 16);
      u8 green = (u8)(color >> 8);
      u8 blue = (u8)(color);
      if(red){
         red = color_part % red;
      }
      if(green){
         green = color_part % green;
      }
      if(blue){
         blue = color_part % blue;
      }

      last_led_number = last_led_number > LED_STRIP_MAX_LEDS - 1? LED_STRIP_MAX_LEDS-1:last_led_number;
      brightness_pct = brightness_pct > 100u? 100: brightness_pct;
      u8 brightness_5bit = (u8)((brightness_pct * 31u) / 100u);
      u32 end_frame_len = ((last_led_number+1) + 15u) / 16u;
      u32 total_len = LED_STRIP_START_FRAME_LEN + (last_led_number+1) * 4u + end_frame_len;
      memset(frame_buf, 0x00, LED_STRIP_START_FRAME_LEN);
      for (u32 i = 0; i <= last_led_number; i++) {
         u8 *led = &frame_buf[LED_STRIP_START_FRAME_LEN + i * 4u];
         led[0] = 0xE0u;
         led[0] |= brightness_5bit;
         led[1] = blue;
         led[2] = green;
         led[3] = red;
      }
      memset(&frame_buf[LED_STRIP_START_FRAME_LEN + (last_led_number+1) * 4u], 0x00, end_frame_len);
      spi_transaction_t trans = { 0 };
      trans.length = total_len * 8u;
      trans.tx_buffer = frame_buf;
      spi_device_transmit(led_strip_spi, &trans);
   }
   color_part++;
}

#endif //CONFIG_IDF_TARGET_ESP32C3

static int led_strip_init(void) {
   int result = 0;
   int table_ind = link_functions.regs_description_list_add_new(regs_table_led_strip_reg);
   link_functions.preinit_table_vars((u16)table_ind);
#if CONFIG_IDF_TARGET_ESP32C3
   spi_bus_config_t bus_config = { 0 };
   bus_config.mosi_io_num = GPIO_LED_STRIP_DATA;
   bus_config.sclk_io_num = GPIO_LED_STRIP_CLOCK;
   bus_config.miso_io_num = -1;
   bus_config.quadwp_io_num = -1;
   bus_config.quadhd_io_num = -1;
   bus_config.max_transfer_sz = LED_STRIP_FRAME_BUF_LEN;
   ESP_ERROR_CHECK(spi_bus_initialize(SPI2_HOST, &bus_config, SPI_DMA_CH_AUTO));

   spi_device_interface_config_t dev_config = { 0 };
   dev_config.mode = 0;
   dev_config.clock_speed_hz = LED_STRIP_SPI_CLOCK_HZ;
   dev_config.spics_io_num = -1;
   dev_config.queue_size = 1;
   ESP_ERROR_CHECK(spi_bus_add_device(SPI2_HOST, &dev_config, &led_strip_spi));
#endif //CONFIG_IDF_TARGET_ESP32C3
   return result;
}
static int led_strip_deinit(void) {
   int result = 0;
   return result;
}
void led_strip_task(void *arg) {
   (void)arg;
   uint32_t signal_value;
   led_strip_init();
   u32 last_led_number = 0xFFFFFFFFu;
   u32 brightness = 0xFFFFFFFFu;
   u32 color = 0xFFFFFFFFu;
   u16 led_number = 0;
   u32 delay = 1;
   u32 smooth_number = 5;
   u32 mode = 1;
   u32 xoring = 0; /*heh*/
   while (1) {
      if (link_functions.os_thread_signal_wait(STOP_CHILD_PROCCES, &signal_value, delay) == pdTRUE) {
         /*by signal*/
         if (signal_value & STOP_CHILD_PROCCES) {
            led_strip_deinit();
            link_functions.os_thread_terminate(link_functions.os_thread_get_id());
         }
      }
      regs_copy_safe(&smooth_number, &led_strip_reg->vars.smooth_number, sizeof(smooth_number));
      regs_copy_safe(&delay, &led_strip_reg->vars.delay, sizeof(delay));
      regs_copy_safe(&last_led_number, &led_strip_reg->vars.led_number, sizeof(last_led_number));
      regs_copy_safe(&brightness, &led_strip_reg->vars.led_brightness, sizeof(brightness));
      regs_copy_safe(&color, &led_strip_reg->vars.led_color, sizeof(color));
      regs_copy_safe(&mode, &led_strip_reg->vars.mode, sizeof(mode));
#if CONFIG_IDF_TARGET_ESP32C3
      switch(mode){
         case LED_MODE_RUN:
            led_strip_send_run(led_number, last_led_number, brightness, color, smooth_number);
            led_number = ++led_number > last_led_number? 0:led_number;
         break;
         case LED_MODE_FULL:
            {
               xoring ^= last_led_number + brightness + color + mode;
               if(xoring){
                  led_strip_send_full(last_led_number, brightness, color);
               }
               xoring = last_led_number + brightness + color + mode;
            }
         break;
         case LED_MODE_BRIM:
            led_strip_send_brim(last_led_number, brightness, color);
         break;

         default:

      }
#endif //CONFIG_IDF_TARGET_ESP32C3
   }
}
#endif //LED_STRIP
#endif /*LED_STRIP_C*/
