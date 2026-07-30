/**
 * @file led_strip.c
 * @author Shoma Gane <shomagan@gmail.com>
 *
 * @defgroup main
 * @ingroup main
 * @version 0.1
 * @brief  TODO!!! write brief in
 */
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
#define LED_STRIP_TASK_PERIOD (10u)
#define LED_STRIP_MAX_LEDS (50u)
#define LED_STRIP_SPI_CLOCK_HZ (4000000u)
/*sk9822/apa102 led strip control, driven over hw spi (data+clock, no chip select)*/
static const u32 def_table_version = 1u;
static const u32 def_led_number = 30u;
static const u32 def_led_brightness = 50u;
static const u32 def_led_color = 0x00FF00u; //red green blue

/*#generator_use_description {"user_task_regs":"start_struct"}*/
static led_strip_reg_t led_strip_reg_storage = {{0}};
led_strip_reg_t * const led_strip_reg = &led_strip_reg_storage;
#define NUM_OF_LED_STRIP_REG_VARS 4
static u8 led_strip_reg_saved_buf[20];
static const char led_strip_reg_space_name[] = "led_strip_reg_t";
static regs_description_t const regs_description_led_strip_reg[NUM_OF_LED_STRIP_REG_VARS] = {
    { NULL, NULL, NULL, (u8*)&led_strip_reg->vars.table_version, 0,"table version, resets regs to defaults on mismatch","table_version", 0x14018000, 0x31324, U32_REGS_FLAG, 1, 7, 12 }//!< "table version, resets regs to defaults on mismatch" &ro &save
,
    { &def_led_number, NULL, NULL, (u8*)&led_strip_reg->vars.led_number, 4,"number of leds in strip","led_number", 0x14018001, 0x31326, U32_REGS_FLAG, 1, 5, 12 }//!< "number of leds in strip" &save &def
,
    { &def_led_brightness, NULL, NULL, (u8*)&led_strip_reg->vars.led_brightness, 8,"brightness of leds in strip","led_brightness", 0x14018002, 0x31328, U32_REGS_FLAG, 1, 5, 12 }//!< "brightness of leds in strip" &save &def
,
    { &def_led_color, NULL, NULL, (u8*)&led_strip_reg->vars.led_color, 12,"color of leds in strip","led_color", 0x14018003, 0x3132a, U32_REGS_FLAG, 1, 5, 12 }//!< "color of leds in strip" &save &def
,
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

static void led_strip_send(u32 led_number, u32 brightness_pct, u32 color) {
   static u8 frame_buf[LED_STRIP_FRAME_BUF_LEN] __attribute__((aligned(4)));
   if (led_strip_spi == NULL) {
      return;
   }
   if (led_number > LED_STRIP_MAX_LEDS) {
      led_number = LED_STRIP_MAX_LEDS;
   }
   if (brightness_pct > 100u) {
      brightness_pct = 100u;
   }
   u8 brightness_5bit = (u8)((brightness_pct * 31u) / 100u);
   u8 red = (u8)(color >> 16);
   u8 green = (u8)(color >> 8);
   u8 blue = (u8)(color);
   u32 end_frame_len = (led_number + 15u) / 16u;
   u32 total_len = LED_STRIP_START_FRAME_LEN + led_number * 4u + end_frame_len;
   memset(frame_buf, 0x00, LED_STRIP_START_FRAME_LEN);
   for (u32 i = 0; i < led_number; i++) {
      u8 *led = &frame_buf[LED_STRIP_START_FRAME_LEN + i * 4u];
      led[0] = 0xE0u | brightness_5bit;
      led[1] = blue;
      led[2] = green;
      led[3] = red;
   }
   memset(&frame_buf[LED_STRIP_START_FRAME_LEN + led_number * 4u], 0x00, end_frame_len);

   spi_transaction_t trans = { 0 };
   trans.length = total_len * 8u;
   trans.tx_buffer = frame_buf;
   spi_device_transmit(led_strip_spi, &trans);
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
   u32 last_brightness = 0xFFFFFFFFu;
   u32 last_color = 0xFFFFFFFFu;
   while (1) {
      if (link_functions.os_thread_signal_wait(STOP_CHILD_PROCCES, &signal_value, LED_STRIP_TASK_PERIOD) == pdTRUE) {
         /*by signal*/
         if (signal_value & STOP_CHILD_PROCCES) {
            led_strip_deinit();
            link_functions.os_thread_terminate(link_functions.os_thread_get_id());
         }
      }
      if (led_strip_reg->vars.led_number != last_led_number || led_strip_reg->vars.led_brightness != last_brightness ||
          led_strip_reg->vars.led_color != last_color) {
         last_led_number = led_strip_reg->vars.led_number;
         last_brightness = led_strip_reg->vars.led_brightness;
         last_color = led_strip_reg->vars.led_color;
         main_printf(TAG, "update: %u leds, brightness %u%%, color 0x%06X", (unsigned)last_led_number,
                     (unsigned)last_brightness, (unsigned)last_color);
#if CONFIG_IDF_TARGET_ESP32C3
         led_strip_send(last_led_number, last_brightness, last_color);
#endif //CONFIG_IDF_TARGET_ESP32C3
      }
   }
}
#endif //LED_STRIP
#endif /*LED_STRIP_C*/
