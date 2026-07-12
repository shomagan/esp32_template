#ifndef REGS_DESCRIPTION_USER_C
#define REGS_DESCRIPTION_USER_C 1

#include "regs_description.h"
#include "link_functions.h"
#include "scd41.h"
#include "mirror_storage.h"
static scd41_reg_t scd41_reg_storage = {{0}};
scd41_reg_t * const scd41_reg = &scd41_reg_storage;

#define NUM_OF_SCD41_REG_VARS 7
static u8 scd41_reg_saved_buf[110];
static const u32 scd41_reg_table_version = 0x0001;
static const char scd41_reg_space_name[] = "scd41_reg_t";
/*
static const float def_test_pwm_value = 0.0f;
static const float def_min_test_pwm_value = 0.0f;
static const float def_max_test_pwm_value = 100.0f;
static const float def_servo_0 = 10.0f;                   //!<"servo pwm value [0;100]" &def &save &min &max
static const float def_min_servo_0 = 0.0f;                   //!<"servo pwm value [0;100]" &def &save &min &max
static const float def_max_servo_0 = 100.0f;                   //!<"servo pwm value [0;100]" &def &save &min &max
static const float def_servo_1 = 10.0f;                   //!<"servo pwm value [0;100]" &def &save &min &max
static const float def_min_servo_1 = 0.0f;                   //!<"servo pwm value [0;100]" &def &save &min &max
static const float def_max_servo_1 = 100.0f;                   //!<"servo pwm value [0;100]" &def &save &min &max
static const float def_servo_2 = 10.0f;                   //!<"servo pwm value [0;100]" &def &save &min &max
static const float def_min_servo_2 = 0.0f;                   //!<"servo pwm value [0;100]" &def &save &min &max
static const float def_max_servo_2 = 100.0f;                   //!<"servo pwm value [0;100]" &def &save &min &max
static const float def_servo_3 = 10.0f;                   //!<"servo pwm value [0;100]" &def &save &min &max
static const float def_min_servo_3 = 0.0f;                   //!<"servo pwm value [0;100]" &def &save &min &max
static const float def_max_servo_3 = 100.0f;                  //!<"servo pwm value [0;100]" &def &save &min &max
static const float def_impulse_cost = 0.0005f;
static const u32 def_touch_1_count = 20000;                      //!< "ms or water counter long" &save &def
static const u32 def_touch_2_count = 100000;                      //!< "ms or water counter long" &save &def
static const u32 def_touch_3_count = 200000;                      //!< "ms or water counter long" &save &def
static const float def_touch_1_liters = 0.125f;                      //!< "ms " &save &def
static const float def_touch_2_liters = 1.75f;                      //!< "ms " &save &def
static const float def_touch_3_liters = 5.0f;                      //!< "ms " &save &def
static const u32 def_by_time = 0;                              //!< "by time or counter" &save &def
static const float def_touch_0_trshld = 0.85f;                      //!< "in percents" &save &def
static const float def_touch_1_trshld = 0.85f;                      //!< "in percents" &save &def
static const float def_touch_2_trshld = 0.85f;                      //!< "in percents" &save &def
static const float def_touch_3_trshld = 0.85f;                      //!< "in percents" &save &def
static const u32 def_touch_handle_period = 300;                    //!< "in ms period of handle touchs" &save &def

static const u16 def_feeder_interval = 721u;
static const u16 def_min_feeder_interval = 1u;
static const float def_feeder_time_sec = 0.5f;
static const float def_min_feeder_time_sec = 0.1f;
static const float def_max_feeder_time_sec = 60.0f;
static const u16 def_polisher_speed = 0;
static const u16 def_max_polisher_speed = 100;
static const u16 def_polisher_direction = 0;
static const u16 def_max_polisher_direction = 2;
static const u16 def_morse_unit_time_ms = 100;
static const u16 def_min_morse_unit_time_ms = 10;
static const u16 def_max_morse_unit_time_ms = 1000;
static const u16 def_morse_message_len = 17;
static const u16 def_min_morse_message_len = 1;
static const u16 def_max_morse_message_len = 32;
static const u16 def_morse_settings = 0;         //!<"morse settings bit0 - server"

static const u16 def_battery_min_level = 10;
static const u16 def_min_battery_min_level = 1;
static const u16 def_max_battery_min_level = 50;
*/

static regs_description_t const regs_description_scd41_reg[NUM_OF_SCD41_REG_VARS] = {
    { NULL, NULL, NULL, (u8*)&scd41_reg->vars.scd41_co2_level, 0,"CO2 level in ppm","scd41_co2_level", 0x14016000, 0x312c0, U16_REGS_FLAG, 1, 3, 11 }//!< "CO2 level in ppm" &ro
,
    { NULL, NULL, NULL, (u8*)&scd41_reg->vars.scd41_temperature, 0,"Temperature in Celsius","scd41_temperature", 0x14016001, 0x312c1, FLOAT_REGS_FLAG, 1, 3, 11 }//!< "Temperature in Celsius" &ro
,
    { NULL, NULL, NULL, (u8*)&scd41_reg->vars.scd41_humidity, 0,"Humidity in percentage","scd41_humidity", 0x14016002, 0x312c3, FLOAT_REGS_FLAG, 1, 3, 11 }//!< "Humidity in percentage" &ro
,
    { NULL, NULL, NULL, (u8*)&scd41_reg->vars.scd41_settings, 0,"Settings for SCD41 sensor","scd41_settings", 0x14016003, 0x312c5, U16_REGS_FLAG, 1, 5, 11 }//!< "Settings for SCD41 sensor" &save
,
    { &def_scd41_measur_interval, &def_min_scd41_measur_interval, &def_max_scd41_measur_interval, (u8*)&scd41_reg->vars.scd41_measur_interval, 2,"minutes between a measurement","scd41_measur_interval", 0x14016004, 0x312c6, U16_REGS_FLAG, 1, 197, 11 }//!< "minutes between a measurement" &save &def &min &max
,
    { NULL, NULL, NULL, (u8*)&scd41_reg->vars.scd41_index_in_array, 4,"Index in array","scd41_index_in_array", 0x14016005, 0x312c7, U16_REGS_FLAG, 1, 7, 11 }//!< "Index in array" &ro &save
,
    { NULL, NULL, NULL, (u8*)&scd41_reg->vars.scd41_co2_array[0], 6,"CO2 level in ppm","scd41_co2_array", 0x14016006, 0x312c8, U16_REGS_FLAG, 48, 7, 11 }//!< "CO2 level in ppm" &ro &save
,
};
const regs_description_list_t regs_table_scd41_reg = {
    .description = regs_description_scd41_reg,
    .num_of_regs = NUM_OF_SCD41_REG_VARS,
    .table_version = &scd41_reg_table_version,
    .space_name = scd41_reg_space_name,
    .saved_regs_buffer = scd41_reg_saved_buf,
    .saved_regs_buffer_size = sizeof(scd41_reg_saved_buf),
};

void user_tasks_register_regs(void) {
    int index = link_functions.regs_description_list_add_new(regs_table_scd41_reg);
    if(index >= 0){
        preinit_table_vars(index);//!< preinit global vars for new description
    }
}

#endif
