/**
 * @file touch_handle.c
 * @author Shoma Gane <shomagan@gmail.com>
 *         Ayrat Girfanov <girfanov.ayrat@yandex.ru>
 * @defgroup ../main/
 * @ingroup ../main/
 * @version 0.1
 * @brief  TODO!!! write brief in
 */
#ifndef TOUCH_HANDLE_C
#define TOUCH_HANDLE_C 1
#include "common.h"
#include "touch_handle.h"
#include "regs.h"
#include "regs_description.h"
#if CONFIG_IDF_TARGET_ESP32
#include "driver/touch_pad.h"
#endif //CONFIG_IDF_TARGET_ESP32
#include "driver/gpio.h"
#include "mirror_storage.h"
#include "math.h"
#include "pin_map.h"
#include "interrupt.h"
#include "link_functions.h"
static const char *TAG = "touch_handle";

/*#generator_use_description {"user_task_regs":"start_struct"}*/
static touch_regs_t touch_regs_storage = {{0}};
touch_regs_t * const touch_regs = &touch_regs_storage;
#define NUM_OF_TOUCH_REGS_VARS 22
static u8 touch_regs_saved_buf[80];
static const char touch_regs_space_name[] = "touch_regs_t";
static regs_description_t const regs_description_touch_regs[NUM_OF_TOUCH_REGS_VARS] = {
    { &def_test_pwm_value, &def_min_test_pwm_value, &def_max_test_pwm_value, (u8*)&touch_regs->vars.test_pwm_value, 0,"test pwm value [0;100]","test_pwm_value", 0x14002000, 0x303e8, FLOAT_REGS_FLAG, 1, 197, 1 }//!<"test pwm value [0;100]" &def &save &min &max
,
    { &def_test_pwm_value, &def_min_test_pwm_value, &def_max_test_pwm_value, (u8*)&touch_regs->vars.touch_0, 0,"touch_0","touch_0", 0x14002001, 0x303ea, U16_REGS_FLAG, 1, 1, 1 }
,
    { &def_test_pwm_value, &def_min_test_pwm_value, &def_max_test_pwm_value, (u8*)&touch_regs->vars.touch_1, 0,"touch_1","touch_1", 0x14002002, 0x303eb, U16_REGS_FLAG, 1, 1, 1 }
,
    { &def_test_pwm_value, &def_min_test_pwm_value, &def_max_test_pwm_value, (u8*)&touch_regs->vars.touch_2, 0,"touch_2","touch_2", 0x14002003, 0x303ec, U16_REGS_FLAG, 1, 1, 1 }
,
    { &def_test_pwm_value, &def_min_test_pwm_value, &def_max_test_pwm_value, (u8*)&touch_regs->vars.touch_3, 0,"touch_3","touch_3", 0x14002004, 0x303ed, U16_REGS_FLAG, 1, 1, 1 }
,
    { NULL, NULL, NULL, (u8*)&touch_regs->vars.water_counter, 4,"di counter","water_counter", 0x14002005, 0x303ee, U64_REGS_FLAG, 1, 7, 1 }//!< "di counter" &save &ro
,
    { &def_impulse_cost, NULL, NULL, (u8*)&touch_regs->vars.impulse_cost, 12,"cost of di impulse in liters","impulse_cost", 0x14002006, 0x303f2, FLOAT_REGS_FLAG, 1, 5, 1 }//!< "cost of di impulse in liters" &save &def
,
    { NULL, NULL, NULL, (u8*)&touch_regs->vars.liters, 16,"liters calculated","liters", 0x14002007, 0x303f4, FLOAT_REGS_FLAG, 1, 5, 1 }//!< "liters calculated" &save
,
    { &def_touch_1_count, NULL, NULL, (u8*)&touch_regs->vars.touch_1_count, 20,"ms ","touch_1_count", 0x14002008, 0x303f6, U32_REGS_FLAG, 1, 5, 1 }//!< "ms " &save &def
,
    { &def_touch_2_count, NULL, NULL, (u8*)&touch_regs->vars.touch_2_count, 24,"ms ","touch_2_count", 0x14002009, 0x303f8, U32_REGS_FLAG, 1, 5, 1 }//!< "ms " &save &def
,
    { &def_touch_3_count, NULL, NULL, (u8*)&touch_regs->vars.touch_3_count, 28,"ms ","touch_3_count", 0x1400200a, 0x303fa, U32_REGS_FLAG, 1, 5, 1 }//!< "ms " &save &def
,
    { &def_touch_1_liters, NULL, NULL, (u8*)&touch_regs->vars.touch_1_liters, 32,"ms ","touch_1_liters", 0x1400200b, 0x303fc, FLOAT_REGS_FLAG, 1, 5, 1 }//!< "ms " &save &def
,
    { &def_touch_2_liters, NULL, NULL, (u8*)&touch_regs->vars.touch_2_liters, 36,"ms ","touch_2_liters", 0x1400200c, 0x303fe, FLOAT_REGS_FLAG, 1, 5, 1 }//!< "ms " &save &def
,
    { &def_touch_3_liters, NULL, NULL, (u8*)&touch_regs->vars.touch_3_liters, 40,"ms ","touch_3_liters", 0x1400200d, 0x30400, FLOAT_REGS_FLAG, 1, 5, 1 }//!< "ms " &save &def
,
    { &def_touch_0_trshld, NULL, NULL, (u8*)&touch_regs->vars.touch_0_trshld, 44,"in percents","touch_0_trshld", 0x1400200e, 0x30402, FLOAT_REGS_FLAG, 1, 5, 1 }//!< "in percents" &save &def
,
    { &def_touch_1_trshld, NULL, NULL, (u8*)&touch_regs->vars.touch_1_trshld, 48,"in percents","touch_1_trshld", 0x1400200f, 0x30404, FLOAT_REGS_FLAG, 1, 5, 1 }//!< "in percents" &save &def
,
    { &def_touch_2_trshld, NULL, NULL, (u8*)&touch_regs->vars.touch_2_trshld, 52,"in percents","touch_2_trshld", 0x14002010, 0x30406, FLOAT_REGS_FLAG, 1, 5, 1 }//!< "in percents" &save &def
,
    { &def_touch_3_trshld, NULL, NULL, (u8*)&touch_regs->vars.touch_3_trshld, 56,"in percents","touch_3_trshld", 0x14002011, 0x30408, FLOAT_REGS_FLAG, 1, 5, 1 }//!< "in percents" &save &def
,
    { NULL, NULL, NULL, (u8*)&touch_regs->vars.filter_use, 60,"use inside filter fot touch sensors or not ","filter_use", 0x14002012, 0x3040a, U32_REGS_FLAG, 1, 5, 1 }//!< "use inside filter fot touch sensors or not " &save
,
    { &def_touch_handle_period, NULL, NULL, (u8*)&touch_regs->vars.touch_handle_period, 64,"in ms period of handle touchs","touch_handle_period", 0x14002013, 0x3040c, U32_REGS_FLAG, 1, 5, 1 }//!< "in ms period of handle touchs" &save &def
,
    { &def_by_time, NULL, NULL, (u8*)&touch_regs->vars.by_time, 68,"by time or counter","by_time", 0x14002014, 0x3040e, U32_REGS_FLAG, 1, 5, 1 }//!< "by time or counter" &save &def
,
    { &def_table_version, NULL, NULL, (u8*)&touch_regs->vars.table_version, 72,"table version, resets regs to defaults on mismatch","table_version", 0x14002015, 0x30410, U32_REGS_FLAG, 1, 7, 1 }//!< "table version, resets regs to defaults on mismatch" &ro &save &def
,
};
const regs_description_list_t regs_table_touch_regs = {
    .description = regs_description_touch_regs,
    .num_of_regs = NUM_OF_TOUCH_REGS_VARS,
    .table_version = &def_table_version,
    .space_name = touch_regs_space_name,
    .saved_regs_buffer = touch_regs_saved_buf,
    .saved_regs_buffer_size = sizeof(touch_regs_saved_buf),
};
/*#generator_use_description {"user_task_regs":"end_struct"}*/

#define TOUCH_THRESH_NO_USE (0)
#define TOUCHPAD_FILTER_TOUCH_PERIOD (10)
#define MAX_ENABLED_PERIOD_MS 600000UL

#define THRESHOLD_PERCENT 0.9f
#define CONNECT_PERCENT 0.9f
#define UNCONNECT_PERCENT 0.25f
#define ESP_INTR_FLAG_DEFAULT 0
typedef enum {
   IDLE_STATE = 0,
   TOUCH_0_STATE = BIT(0),
   TOUCH_1_STATE = BIT(1),
   TOUCH_2_STATE = BIT(2),
   TOUCH_3_STATE = BIT(3),

} touch_states_t;
task_handle_t touch_task_handle;

#if TOUCH_HANDLE_ENABLE
static touch_states_t touch_states = IDLE_STATE;
static void touch_gpio_initialize(void);
static int touch_handle_init(void);
static int handle_touch_value(u16 touch_0, u16 touch_1, u16 touch_2, u16 touch_3);
static void touch_gpio_initialize(void) {
   gpio_config_t io_conf;
   //disable interrupt
   io_conf.intr_type = GPIO_INTR_DISABLE;
   //set as output mode
   io_conf.mode = GPIO_MODE_OUTPUT;
   //bit mask of the pins that you want to set,e.g.GPIO18/19
   io_conf.pin_bit_mask = 1ULL << DRIVE_PIN;
   //disable pull-down mode
   io_conf.pull_down_en = 0;
   //disable pull-up mode
   io_conf.pull_up_en = 0;
   //configure GPIO with the given settings
   gpio_config(&io_conf);
#if CONFIG_IDF_TARGET_ESP32
   touch_pad_config(GPIO_TOUCH_0, TOUCH_THRESH_NO_USE);
   touch_pad_config(GPIO_TOUCH_1, TOUCH_THRESH_NO_USE);
   touch_pad_config(GPIO_TOUCH_2, TOUCH_THRESH_NO_USE);
   touch_pad_config(GPIO_TOUCH_3, TOUCH_THRESH_NO_USE);
#endif //CONFIG_IDF_TARGET_ESP32
   //interrupt of rising edge
   io_conf.intr_type = GPIO_INTR_POSEDGE;
   //bit mask of the pins, use GPIO4/5 here
   io_conf.pin_bit_mask = GPIO_WATER_COUNTER_INPUT_MASK;
   //set as input mode
   io_conf.mode = GPIO_MODE_INPUT;
   //enable pull-up mode
   io_conf.pull_up_en = 0;
   gpio_config(&io_conf);
   gpio_set_intr_type(GPIO_WATER_COUNTER_INPUT, GPIO_INTR_POSEDGE);
   gpio_install_isr_service(ESP_INTR_FLAG_DEFAULT);
   //hook isr handler for specific gpio pin
   gpio_isr_handler_add(GPIO_WATER_COUNTER_INPUT, gpio_isr_handler, (void *)GPIO_WATER_COUNTER_INPUT);
}

static int touch_handle_init(void) {
   int table_ind = link_functions.regs_description_list_add_new(regs_table_touch_regs);
   link_functions.preinit_table_vars((u16)table_ind);
#if CONFIG_IDF_TARGET_ESP32
   ESP_ERROR_CHECK(touch_pad_init());
   touch_pad_set_voltage(TOUCH_HVOLT_2V7, TOUCH_LVOLT_0V5, TOUCH_HVOLT_ATTEN_1V);
   touch_gpio_initialize();
   if (touch_regs->vars.filter_use) {
      touch_pad_filter_start(TOUCHPAD_FILTER_TOUCH_PERIOD);
   }
#endif //CONFIG_IDF_TARGET_ESP32
   return 0;
}
/*@brief deinit touch pins*/
static int touch_handle_deinit(void) {
   return 0;
}
void touch_task(void *arg) {
   (void)(*arg);
   uint32_t signal_value;
   touch_handle_init();
   u32 enable_period = 0;
   float enable_period_float = touch_regs->vars.liters;
   u32 driver_enabled_tick_os = link_functions.os_kernel_sys_tick();
   float driver_enabled_liters = touch_regs->vars.liters;
   u8 drive_enable = 0;
   u64 ticks = 0;
   u16 touch_temp;
   uint16_t touch_value;
#if CONFIG_IDF_TARGET_ESP32
   u16 touch_0, touch_1, touch_2, touch_3;
   touch_pad_read_raw_data(GPIO_TOUCH_0, &touch_value);
   touch_pad_read_filtered(GPIO_TOUCH_0, &touch_temp);
   touch_regs->vars.touch_0 = touch_temp;
   touch_0 = touch_temp;
   touch_pad_read_raw_data(GPIO_TOUCH_1, &touch_value);
   touch_pad_read_filtered(GPIO_TOUCH_1, &touch_temp);
   touch_regs->vars.touch_1 = touch_temp;
   touch_1 = touch_temp;
   touch_pad_read_raw_data(GPIO_TOUCH_2, &touch_value);
   touch_pad_read_filtered(GPIO_TOUCH_2, &touch_temp);
   touch_regs->vars.touch_2 = touch_temp;
   touch_2 = touch_temp;
   touch_pad_read_raw_data(GPIO_TOUCH_3, &touch_value);
   touch_pad_read_filtered(GPIO_TOUCH_3, &touch_temp);
   touch_regs->vars.touch_3 = touch_temp;
   touch_3 = touch_temp;
#endif
   u32 filter_use = touch_regs->vars.filter_use;
   while (1) {
#if CONFIG_IDF_TARGET_ESP32
      if (touch_regs->vars.filter_use && !filter_use) {
         filter_use = touch_regs->vars.filter_use;
         touch_pad_filter_start(TOUCHPAD_FILTER_TOUCH_PERIOD);
      } else if (!touch_regs->vars.filter_use && filter_use) {
         filter_use = touch_regs->vars.filter_use;
         touch_pad_filter_stop();
      }
#endif //CONFIG_IDF_TARGET_ESP32
      if (link_functions.os_thread_signal_wait(STOP_CHILD_PROCCES | PACKET_RECEIVED, &signal_value, 10) == pdTRUE) {
         /*by signal*/
         if (signal_value & STOP_CHILD_PROCCES) {
            touch_handle_deinit();
            link_functions.os_thread_terminate(link_functions.os_thread_get_id());
         } else if (signal_value & PACKET_RECEIVED) {
         }
      }
#if CONFIG_IDF_TARGET_ESP32
      if (filter_use) {
         touch_pad_read_raw_data(GPIO_TOUCH_0, &touch_value);
         touch_pad_read_filtered(GPIO_TOUCH_0, &touch_temp);
         touch_regs->vars.touch_0 = touch_temp;
         touch_pad_read_raw_data(GPIO_TOUCH_1, &touch_value);
         touch_pad_read_filtered(GPIO_TOUCH_1, &touch_temp);
         touch_regs->vars.touch_1 = touch_temp;
         touch_pad_read_raw_data(GPIO_TOUCH_2, &touch_value);
         touch_pad_read_filtered(GPIO_TOUCH_2, &touch_temp);
         touch_regs->vars.touch_2 = touch_temp;
         touch_pad_read_raw_data(GPIO_TOUCH_3, &touch_value);
         touch_pad_read_filtered(GPIO_TOUCH_3, &touch_temp);
         touch_regs->vars.touch_3 = touch_temp;
      } else {
         touch_pad_read(GPIO_TOUCH_0, &touch_temp);
         touch_regs->vars.touch_0 = touch_temp;
         touch_pad_read(GPIO_TOUCH_1, &touch_temp);
         touch_regs->vars.touch_1 = touch_temp;
         touch_pad_read(GPIO_TOUCH_2, &touch_temp);
         touch_regs->vars.touch_2 = touch_temp;
         touch_pad_read(GPIO_TOUCH_3, &touch_temp);
         touch_regs->vars.touch_3 = touch_temp;
      }
      if (ticks % (touch_regs->vars.touch_handle_period / 10) == 0) {
         float part_0 = (float)touch_regs->vars.touch_0 / (float)touch_0;
         float part_1 = (float)touch_regs->vars.touch_1 / (float)touch_1;
         float part_2 = (float)touch_regs->vars.touch_2 / (float)touch_2;
         float part_3 = (float)touch_regs->vars.touch_3 / (float)touch_3;
         if (part_0 < touch_regs->vars.touch_0_trshld) {
            touch_states |= TOUCH_0_STATE;
         } else {
            touch_states &= ~TOUCH_0_STATE;
         }
         if (part_1 < touch_regs->vars.touch_1_trshld) {
            touch_states |= TOUCH_1_STATE;
         } else {
            touch_states &= ~TOUCH_1_STATE;
         }
         if (part_2 < touch_regs->vars.touch_2_trshld) {
            touch_states |= TOUCH_2_STATE;
         } else {
            touch_states &= ~TOUCH_2_STATE;
         }
         if (part_3 < touch_regs->vars.touch_3_trshld) {
            touch_states |= TOUCH_3_STATE;
         } else {
            touch_states &= ~TOUCH_3_STATE;
         }
         touch_0 = touch_regs->vars.touch_0;
         touch_1 = touch_regs->vars.touch_1;
         touch_2 = touch_regs->vars.touch_2;
         touch_3 = touch_regs->vars.touch_3;
      }
#endif
      if (ticks % 10 == 0) {
         touch_regs->vars.liters = touch_regs->vars.water_counter * touch_regs->vars.impulse_cost;
      }
      if (ticks % 360000 == 0) {
         regs_template_t regs_template = { 0 };
         regs_template.p_value = &touch_regs->vars.liters;
         int result = regs_description_get_by_address(&regs_template);
         if (result >= 0) {
            if (mirror_access_write(&regs_template) >= 0) {
               link_functions.os_log_info(TAG, "succes wrote liters to mirror");
            } else {
               link_functions.os_log_info(TAG, "unsucces writing liters to mirror");
            }
         }
         regs_template.p_value = &touch_regs->vars.water_counter;
         int result = regs_description_get_by_address(&regs_template);
         if (result >= 0) {
            if (mirror_access_write(&regs_template) >= 0) {
               link_functions.os_log_info(TAG, "succes wrote water_counter to mirror");
            } else {
               link_functions.os_log_info(TAG, "unsucces writing water_counter to mirror");
            }
         }
      }
   }
   if ((touch_states & TOUCH_1_STATE) && (drive_enable == 0)) {
      if (touch_regs->vars.by_time) {
         enable_period = touch_regs->vars.touch_1_count;
      } else {
         enable_period_float = touch_regs->vars.touch_1_liters;
      }
      driver_enabled_tick_os = link_functions.os_kernel_sys_tick();
      drive_enable = 1;
   }
   if ((touch_states & TOUCH_2_STATE) && (drive_enable == 0)) {
      if (touch_regs->vars.by_time) {
         enable_period = touch_regs->vars.touch_2_count;
      } else {
         enable_period_float = touch_regs->vars.touch_2_liters;
      }
      driver_enabled_liters = touch_regs->vars.liters;
      driver_enabled_tick_os = link_functions.os_kernel_sys_tick();
      drive_enable = 1;
   }
   if ((touch_states & TOUCH_3_STATE) && (drive_enable == 0)) {
      if (touch_regs->vars.by_time) {
         enable_period = touch_regs->vars.touch_3_count;
      } else {
         enable_period_float = touch_regs->vars.touch_3_liters;
      }
      driver_enabled_liters = touch_regs->vars.liters;
      driver_enabled_tick_os = link_functions.os_kernel_sys_tick();
      drive_enable = 1;
   }
   if ((touch_states & TOUCH_0_STATE) && (drive_enable == 1)) {
      if (touch_regs->vars.by_time) {
         enable_period = 0;
      }
      driver_enabled_liters = touch_regs->vars.liters;
      driver_enabled_tick_os = link_functions.os_kernel_sys_tick();
      drive_enable = 0;
   }

   if (drive_enable) {
      gpio_set_level(DRIVE_PIN, 1);
      if (touch_regs->vars.by_time) {
         if (link_functions.os_kernel_sys_tick() - driver_enabled_tick_os > enable_period) {
            drive_enable = 0;
         }
      } else {
         if (touch_regs->vars.liters - driver_enabled_liters > enable_period_float) {
            drive_enable = 0;
         }
      }

      if (link_functions.os_kernel_sys_tick() - driver_enabled_tick_os > MAX_ENABLED_PERIOD_MS) {
         drive_enable = 0;
      }
   } else {
      gpio_set_level(DRIVE_PIN, 0);
   }
   ticks++;
}
}
static int handle_touch_value(u16 touch_0, u16 touch_1, u16 touch_2, u16 touch_3) {
   int res = 0;
   return res;
}
#endif /*TOUCH_HANDLE_ENABLE*/
#endif //TOUCH_HANDLE
