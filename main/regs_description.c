/**
 * @file regs_description.c
 * @author Shoma Gane <shomagan@gmail.com>
 *         Ayrat Girfanov <girfanov.ayrat@yandex.ru>
 * @defgroup regs
 * @ingroup regs
 * @version 0.1
 * @brief  TODO!!! write brief in
 */

#ifndef REGS_DESCRIPTION_C
#define REGS_DESCRIPTION_C 1

#include "regs_description.h"
#include "regs.h"
#include "os_type.h"
#include "mirror_storage.h"
#include "credentials.h"
#include <string.h> /* memset */
#include <stdlib.h> /* atoi */
#include <stdio.h>
#include <math.h>
#include "esp_log.h"
#include "esp_check.h"

static const regs_description_t * bin_search_guid(u32 guid, const regs_description_t * regs_array, const u16 size);
static const regs_description_t * bin_search_address(void * address, const regs_description_t * regs_array, const u16 size);

#define LIST_DESCRIPTIONS_SIZE 32 /*max number of descriptions, restricted by number of u32 bits of table_to_save register */
regs_description_list_t regs_description_list[LIST_DESCRIPTIONS_SIZE];/*array of descriptions of registers, each element is array of regs_template_t with num_of_vars size*/

static const char *TAG = "regs_description";

static const u16 def_mdb_addr = 14;
static const u8 def_ip[4] = {192,168,1,232};
static const u8 def_netmask[4] = {255,255,255,0};
static const u8 def_gate[4] = {192,168,1,1};
static const u8 def_slip_ip[4] = {172,16,1,200};
static const u8 def_slip_netmask[4] = {255,255,255,0};
static const u8 def_slip_gate[4] = {172,16,1,232};
static const u16 def_uart_debug_sets = 0xC853;/*what!?*/
static const u8 def_fw_version[FW_VERSION_SIZE] = FW_VERSION;
static const u16 def_num_of_vars = NUM_OF_SELF_VARS;
static const u16 def_client_num_of_vars = NUM_OF_CLIENT_VARS;
static const u8 def_board_ver = (u8)BOARD_VERSION;
static const u16 def_permission = ENABLE_HTTP_FLAG;
static const u8 def_wifi_name[WIFI_NAME_LEN] = "sofi_plc_air";               //!<"must be strong full filled", &save &def
static const u8 def_wifi_password[WIFI_PASSWORD_LEN] = "bricsofi";            //!<"must be strong more then 8 byte", &save &def
static const u8 def_wifi_router_name[WIFI_ROUTER_NAME_LEN] = WIFE_STATION_SECURE_NAME;
static const u8 def_wifi_router_password[WIFI_ROUTER_PASSWORD_LEN] = WIFE_STATION_SECURE_PASSWORD;
static const u16 def_wifi_setting = WIFI_CLIENT;               //!<"type of wifi and settings" &save &def
static const float def_test_pwm_value = 0.0f;
static const float def_min_test_pwm_value = 0.0f;
static const float def_max_test_pwm_value = 100.0f;
const u32 def_table_version = 0x001f;   //!<"current version, if changed, starting from scratch"
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
static const u16 def_slave_modbus_address = 3;
static const u16 def_min_slave_modbus_address = 1;
static const u16 def_max_slave_modbus_address = 247;

static const u16 def_feeder_interval = 721u;
static const u16 def_min_feeder_interval = 1u;
static const float def_feeder_time_sec = 0.5f;
static const float def_min_feeder_time_sec = 0.1f;
static const float def_max_feeder_time_sec = 60.0f;
static const u16 def_sleep_time = 25u;
static const u16 def_min_sleep_time = 1u;
static const u16 def_polisher_speed = 0;
static const u16 def_max_polisher_speed = 100;
static const u16 def_polisher_direction = 0;
static const u16 def_max_polisher_direction = 2;
static const u16 def_morse_unit_time_ms = 100;
static const u16 def_min_morse_unit_time_ms = 10;
static const u16 def_max_morse_unit_time_ms = 1000;
static const u16 def_morse_message_len = 17;
static const u16 def_min_morse_message_len = 1;
static const u16 def_max_morse_message_len = 32; /*morse_message_send and morse_message array size */
static const u16 def_morse_settings = 0;         //!<"morse settings bit0 - server"

static const u16 def_battery_min_level = 10;
static const u16 def_min_battery_min_level = 1;
static const u16 def_max_battery_min_level = 50;

static const u16 def_scd41_measur_interval = 1;
static const u16 def_min_scd41_measur_interval = 1;
static const u16 def_max_scd41_measur_interval = 120;

regs_description_t const regs_description[NUM_OF_SELF_VARS]={
{ &def_mdb_addr, NULL, NULL, (u8*)&regs_main.regs_global.vars.mdb_addr, 16,"modbus address","mdb_addr", U16_REGS_FLAG, 0, 0, 0x30000, 1, 5, 0 },//!<"modbus address" &save &def
{ &def_ip, NULL, NULL, (u8*)&regs_main.regs_global.vars.ip[0], 18,"device ip address, warning!!! changes can lead to lost connection","ip", U8_REGS_FLAG, 1, 2, 0x30001, 4, 5, 0 },//!<"device ip address, warning!!! changes can lead to lost connection" &save &def &crtcl
{ &def_netmask, NULL, NULL, (u8*)&regs_main.regs_global.vars.netmask[0], 22,"netmask address for main wifi net","netmask", U8_REGS_FLAG, 2, 6, 0x30003, 4, 5, 0 },//!<"netmask address for main wifi net", &save , &def , &crtcl
{ &def_gate, NULL, NULL, (u8*)&regs_main.regs_global.vars.gate[0], 26,"gateaway address, warning!!! changes can lead to lost connection ","gate", U8_REGS_FLAG, 3, 10, 0x30005, 4, 5, 0 },//!<"gateaway address, warning!!! changes can lead to lost connection " &save &def &crtcl
{ &def_slip_ip, NULL, NULL, (u8*)&regs_main.regs_global.vars.slip_ip[0], 30,"ip address for local net","slip_ip", U8_REGS_FLAG, 4, 14, 0x30007, 4, 5, 0 },//!<"ip address for local net",&save , &def &crtcl
{ &def_slip_netmask, NULL, NULL, (u8*)&regs_main.regs_global.vars.slip_netmask[0], 34,"netmask address for local net","slip_netmask", U8_REGS_FLAG, 5, 18, 0x30009, 4, 5, 0 },//!<"netmask address for local net", &save , &def , &crtcl
{ &def_slip_gate, NULL, NULL, (u8*)&regs_main.regs_global.vars.slip_gate[0], 38,"gateaway address for local net","slip_gate", U8_REGS_FLAG, 6, 22, 0x3000b, 4, 5, 0 },//!<"gateaway address for local net", &save, &def &crtcl
{ &def_wifi_name, NULL, NULL, (u8*)&regs_main.regs_global.vars.wifi_name[0], 42,"must be strong full filled","wifi_name", U8_REGS_FLAG, 7, 26, 0x3000d, 12, 5, 0 },//!<"must be strong full filled", &save &def &crtcl
{ &def_wifi_password, NULL, NULL, (u8*)&regs_main.regs_global.vars.wifi_password[0], 54,"must be strong 8 byte","wifi_password", U8_REGS_FLAG, 8, 38, 0x30013, 8, 5, 0 },//!<"must be strong 8 byte", &save &def &crtcl
{ &def_wifi_router_name, NULL, NULL, (u8*)&regs_main.regs_global.vars.wifi_router_name[0], 62,"must be ended by zero","wifi_router_name", U8_REGS_FLAG, 9, 46, 0x30017, 32, 5, 0 },//!<"must be ended by zero", &save &def &crtcl
{ &def_wifi_router_password, NULL, NULL, (u8*)&regs_main.regs_global.vars.wifi_router_password[0], 94,"must be more or equal 8 byte","wifi_router_password", U8_REGS_FLAG, 10, 78, 0x30027, 32, 5, 0 },//!<"must be more or equal 8 byte", &save &def &crtcl
{ &def_wifi_setting, NULL, NULL, (u8*)&regs_main.regs_global.vars.wifi_setting, 126,"type of wifi and settings","wifi_setting", U16_REGS_FLAG, 11, 110, 0x30037, 1, 5, 0 },//!<"type of wifi and settings" &save &def &crtcl
{ NULL, NULL, NULL, (u8*)&regs_main.regs_global.vars.reset_num, 128,"number of reset","reset_num", U32_REGS_FLAG, 12, 112, 0x30038, 1, 5, 0 },//!<"number of reset" &save
{ NULL, NULL, NULL, (u8*)&regs_main.regs_global.vars.wifi_state, 0,"wifi state","wifi_state", U16_REGS_FLAG, 13, 116, 0x3003a, 1, 3, 0 },//!<"wifi state" &ro
{ NULL, NULL, NULL, (u8*)&regs_main.regs_global.vars.command, 0,"command register","command", U16_REGS_FLAG, 14, 118, 0x3003b, 1, 1, 0 },//!<"command register"
{ &def_uart_debug_sets, NULL, NULL, (u8*)&regs_main.regs_global.vars.uart_debug_sets, 132,"settings debug uart speed,parity and stop bits, default 115200 ,parity - none, 1 stop bit","uart_debug_sets", U16_REGS_FLAG, 15, 120, 0x3003c, 1, 5, 0 },//!<"settings debug uart speed,parity and stop bits, default 115200 ,parity - none, 1 stop bit" &save &def
{ NULL, NULL, NULL, (u8*)&regs_main.regs_global.vars.device_state, 0,"BIT(0) - interlock","device_state", U16_REGS_FLAG, 16, 122, 0x3003d, 1, 3, 0 },//!< "BIT(0) - interlock" &ro
{ NULL, NULL, NULL, (u8*)&regs_main.regs_global.vars.unused_3, 0,"unused_3","unused_3", U16_REGS_FLAG, 17, 124, 0x3003e, 1, 3, 0 },//!<"" &ro
{ NULL, NULL, NULL, (u8*)&regs_main.regs_global.vars.sys_tick_counter, 0,"tick in ms","sys_tick_counter", U64_REGS_FLAG, 18, 126, 0x3003f, 1, 3, 0 },//!<"tick in ms",&ro
{ NULL, NULL, NULL, (u8*)&regs_main.regs_global.vars.mac_addr[0], 0,"ethernet mac address","mac_addr", U8_REGS_FLAG, 19, 134, 0x30043, 6, 3, 0 },//!<"ethernet mac address",&ro
{ NULL, NULL, NULL, (u8*)&regs_main.regs_global.vars.flags_succ_init, 0,"success inited modules","flags_succ_init", U32_REGS_FLAG, 20, 140, 0x30046, 1, 3, 0 },//!<"success inited modules" &ro for init in task
{ NULL, NULL, NULL, (u8*)&regs_main.regs_global.vars.cur_free_heap, 0,"in bytes","cur_free_heap", U32_REGS_FLAG, 21, 144, 0x30048, 1, 3, 0 },//!<"in bytes",&ro
{ NULL, NULL, NULL, (u8*)&regs_main.regs_global.vars.min_free_heap, 0,"in bytes","min_free_heap", U32_REGS_FLAG, 22, 148, 0x3004a, 1, 3, 0 },//!<"in bytes",&ro
{ NULL, NULL, NULL, (u8*)&regs_main.regs_global.vars.debug_info[0], 0,"reserved use for debug","debug_info", U8_REGS_FLAG, 23, 152, 0x3004c, 8, 3, 0 },//!<"reserved use for debug"&ro
{ &def_num_of_vars, NULL, NULL, (u8*)&regs_main.regs_global.vars.num_of_vars, 0,"num_of_vars","num_of_vars", U16_REGS_FLAG, 24, 160, 0x30050, 1, 3, 0 },//!<"number of vars self + config(user) &ro &def
{ &def_client_num_of_vars, NULL, NULL, (u8*)&regs_main.regs_global.vars.client_num_of_vars, 0,"number of client vars self","client_num_of_vars", U16_REGS_FLAG, 25, 162, 0x30051, 1, 3, 0 },//!<"number of client vars self" &ro &def
{ NULL, NULL, NULL, (u8*)&regs_main.regs_global.vars.temperature_mcu, 0,"temperature mcu Celsius","temperature_mcu", FLOAT_REGS_FLAG, 26, 164, 0x30052, 1, 3, 0 },//!<"temperature mcu Celsius" &ro
{ &def_fw_version, NULL, NULL, (u8*)&regs_main.regs_global.vars.fw_version[0], 0,"version like 0.1.1.0","fw_version", U8_REGS_FLAG, 27, 168, 0x30054, 4, 3, 0 },//!<"version like 0.1.1.0",&ro,&def
{ &def_board_ver, NULL, NULL, (u8*)&regs_main.regs_global.vars.board_ver, 0,"board version","board_ver", U16_REGS_FLAG, 28, 172, 0x30056, 1, 3, 0 },//!< "board version", &ro, &def
{ NULL, NULL, NULL, (u8*)&regs_main.regs_global.vars.firmware_hash, 0,"hash os","firmware_hash", U32_REGS_FLAG, 29, 174, 0x30057, 1, 3, 0 },//!< "hash os" &ro
{ NULL, NULL, NULL, (u8*)&regs_main.regs_global.vars.system_error, 0,"system global error","system_error", U32_REGS_FLAG, 30, 178, 0x30059, 1, 3, 0 },//!< "system global error" &ro
{ &def_permission, NULL, NULL, (u8*)&regs_main.regs_global.vars.permission, 0,"flags with permissions","permission", U16_REGS_FLAG, 31, 182, 0x3005b, 1, 3, 0 },//!<"flags with permissions" &ro &def
{ NULL, NULL, NULL, (u8*)&regs_main.regs_global.vars.async_flags, 0,"async flags","async_flags", U32_REGS_FLAG, 32, 184, 0x3005c, 1, 3, 0 },//!< "async flags" &ro
{ NULL, NULL, NULL, (u8*)&regs_main.regs_global.vars.slip_packet_counter, 0,"count all slip packet","slip_packet_counter", U64_REGS_FLAG, 33, 188, 0x3005e, 1, 1, 0 },//!< "count all slip packet"
{ NULL, NULL, NULL, (u8*)&regs_main.regs_global.vars.ap_connections_number, 0,"number of connections","ap_connections_number", U16_REGS_FLAG, 34, 196, 0x30062, 1, 3, 0 },//!< "number of connections" &ro
{ NULL, NULL, NULL, (u8*)&regs_main.regs_global.vars.sta_connect, 0,"sta connect state","sta_connect", U16_REGS_FLAG, 35, 198, 0x30063, 1, 3, 0 },//!< "sta connect state" &ro
{ &def_table_version, NULL, NULL, (u8*)&regs_main.regs_global.vars.table_version, 134,"change value in def_table_version for drop all regs to default value","table_version", U32_REGS_FLAG, 36, 200, 0x30064, 1, 7, 0 },//!< "change value in def_table_version for drop all regs to default value" &ro &def &save
{ NULL, NULL, NULL, (u8*)&regs_main.regs_global.vars.iirls_pairing_status, 0,"status","iirls_pairing_status", U16_REGS_FLAG, 37, 204, 0x30066, 1, 3, 0 },//!< "status" &ro
{ NULL, NULL, NULL, (u8*)&regs_main.regs_global.vars.iirls_paired_ip[0], 0,"ip address of paired device","iirls_paired_ip", U8_REGS_FLAG, 38, 206, 0x30067, 4, 3, 0 },//!< "ip address of paired device" &ro
{ NULL, NULL, NULL, (u8*)&regs_main.regs_global.vars.modbus_master_test_value, 0,"status","modbus_master_test_value", U16_REGS_FLAG, 39, 210, 0x30069, 1, 3, 0 },//!< "status" &ro
{ NULL, NULL, NULL, (u8*)&regs_main.regs_global.vars.iirls_state, 138,"state","iirls_state", U16_REGS_FLAG, 40, 212, 0x3006a, 1, 5, 0 },//!< "state" &save
{ NULL, NULL, NULL, (u8*)&regs_main.regs_global.vars.paired_device_model_number[0], 0,"state","paired_device_model_number", U16_REGS_FLAG, 41, 214, 0x3006b, 40, 3, 0 },//!< "state" &ro
{ &def_slave_modbus_address, &def_min_slave_modbus_address, &def_max_slave_modbus_address, (u8*)&regs_main.regs_global.vars.slave_modbus_address, 140,"slave modbus address","slave_modbus_address", U16_REGS_FLAG, 42, 294, 0x30093, 1, 197, 0 },//!< "slave modbus address" &def &save &max &min
{ NULL, NULL, NULL, (u8*)&regs_main.regs_global.vars.modbus_master_comm_period_current, 0,"modbus_master_comm_period_current","modbus_master_comm_period_current", U32_REGS_FLAG, 43, 296, 0x30094, 1, 1, 0 },//!< ""
{ NULL, NULL, NULL, (u8*)&regs_main.regs_global.vars.modbus_master_comm_period_max, 0,"modbus_master_comm_period_max","modbus_master_comm_period_max", U32_REGS_FLAG, 44, 300, 0x30096, 1, 1, 0 },//!< ""
{ NULL, NULL, NULL, (u8*)&regs_main.regs_global.vars.modbus_master_comm_period_min, 0,"modbus_master_comm_period_min","modbus_master_comm_period_min", U32_REGS_FLAG, 45, 304, 0x30098, 1, 1, 0 },//!< ""
{ NULL, NULL, NULL, (u8*)&regs_main.regs_global.vars.modbus_master_succ_transactions_number, 0,"modbus_master_succ_transactions_number","modbus_master_succ_transactions_number", U32_REGS_FLAG, 46, 308, 0x3009a, 1, 1, 0 },//!< ""
{ NULL, NULL, NULL, (u8*)&regs_main.regs_global.vars.modbus_master_error_transactions_number, 0,"modbus_master_error_transactions_number","modbus_master_error_transactions_number", U32_REGS_FLAG, 47, 312, 0x3009c, 1, 1, 0 },//!< ""
{ NULL, NULL, NULL, (u8*)&regs_main.regs_global.vars.i2c_display_address, 0,"address of display","i2c_display_address", U16_REGS_FLAG, 48, 316, 0x3009e, 1, 3, 0 },//!< "address of display" &ro
{ NULL, NULL, NULL, (u8*)&regs_main.regs_global.vars.sta_ip[0], 142,"ip address of sta","sta_ip", U8_REGS_FLAG, 49, 318, 0x3009f, 4, 7, 0 },//!< "ip address of sta" &ro &save
{ NULL, NULL, NULL, (u8*)&regs_main.regs_global.vars.live_time, 0,"live time in seconds","live_time", U32_REGS_FLAG, 50, 322, 0x300a1, 1, 3, 0 },//!< "live time in seconds" &ro
{ NULL, NULL, NULL, (u8*)&regs_main.regs_global.vars.unix_time, 0,"unix time , not implemented","unix_time", S64_REGS_FLAG, 51, 326, 0x300a3, 1, 1, 0 },//!< "unix time , not implemented"
{ NULL, NULL, NULL, (u8*)&regs_main.regs_global.vars.seconds_of_the_day, 0,"seconds of the day","seconds_of_the_day", U32_REGS_FLAG, 52, 334, 0x300a7, 1, 1, 0 },//!< "seconds of the day"
{ NULL, NULL, NULL, (u8*)&regs_main.regs_global.vars.flash_write_number, 146,"increments every flash write by an app","flash_write_number", U32_REGS_FLAG, 53, 338, 0x300a9, 1, 7, 0 },//!< "increments every flash write by an app" &ro &save
{ NULL, NULL, NULL, (u8*)&regs_main.regs_global.vars.current_state[0], 0,"current state of proccess","current_state", U32_REGS_FLAG, 54, 342, 0x300ab, 4, 3, 0 },//!< "current state of proccess" &ro
{ &def_sleep_time, &def_min_sleep_time, NULL, (u8*)&regs_main.regs_global.vars.sleep_time, 0,"seconds,couple with SLEEP_TASK_DEEP_SLEEP_FOR_N_SEC","sleep_time", U16_REGS_FLAG, 55, 358, 0x300b3, 1, 65, 0 },//!< "seconds,couple with SLEEP_TASK_DEEP_SLEEP_FOR_N_SEC"  &def &min
{ NULL, NULL, NULL, (u8*)&regs_main.regs_global.vars.rssi_ap[0], 0,"wifi rssi of Access point","rssi_ap", I8_REGS_FLAG, 56, 360, 0x300b4, 2, 3, 0 },//!< "wifi rssi of Access point" &ro
{ NULL, NULL, NULL, (u8*)&regs_main.regs_global.vars.primary_channel_ap[0], 0,"wifi channel used","primary_channel_ap", I8_REGS_FLAG, 57, 362, 0x300b5, 2, 3, 0 },//!< "wifi channel used" &ro
{ NULL, NULL, NULL, (u8*)&regs_main.regs_global.vars.wake_up_cause, 0,"wake up reason","wake_up_cause", S32_REGS_FLAG, 58, 364, 0x300b6, 1, 3, 0 },//!< "wake up reason" &ro
{ &def_test_pwm_value, &def_min_test_pwm_value, &def_max_test_pwm_value, (u8*)&regs_main.touch_regs.vars.test_pwm_value, 150,"test pwm value [0;100]","test_pwm_value", FLOAT_REGS_FLAG, 59, 368, 0x303e8, 1, 197, 1 },//!<"test pwm value [0;100]" &def &save &min &max
{ &def_test_pwm_value, &def_min_test_pwm_value, &def_max_test_pwm_value, (u8*)&regs_main.touch_regs.vars.touch_0, 0,"touch_0","touch_0", U16_REGS_FLAG, 60, 372, 0x303ea, 1, 1, 1 },
{ &def_test_pwm_value, &def_min_test_pwm_value, &def_max_test_pwm_value, (u8*)&regs_main.touch_regs.vars.touch_1, 0,"touch_1","touch_1", U16_REGS_FLAG, 61, 374, 0x303eb, 1, 1, 1 },
{ &def_test_pwm_value, &def_min_test_pwm_value, &def_max_test_pwm_value, (u8*)&regs_main.touch_regs.vars.touch_2, 0,"touch_2","touch_2", U16_REGS_FLAG, 62, 376, 0x303ec, 1, 1, 1 },
{ &def_test_pwm_value, &def_min_test_pwm_value, &def_max_test_pwm_value, (u8*)&regs_main.touch_regs.vars.touch_3, 0,"touch_3","touch_3", U16_REGS_FLAG, 63, 378, 0x303ed, 1, 1, 1 },
{ NULL, NULL, NULL, (u8*)&regs_main.touch_regs.vars.water_counter, 154,"di counter","water_counter", U64_REGS_FLAG, 64, 380, 0x303ee, 1, 7, 1 },//!< "di counter" &save &ro
{ &def_impulse_cost, NULL, NULL, (u8*)&regs_main.touch_regs.vars.impulse_cost, 162,"cost of di impulse in liters","impulse_cost", FLOAT_REGS_FLAG, 65, 388, 0x303f2, 1, 5, 1 },//!< "cost of di impulse in liters" &save &def
{ NULL, NULL, NULL, (u8*)&regs_main.touch_regs.vars.liters, 166,"liters calculated","liters", FLOAT_REGS_FLAG, 66, 392, 0x303f4, 1, 5, 1 },//!< "liters calculated" &save
{ &def_touch_1_count, NULL, NULL, (u8*)&regs_main.touch_regs.vars.touch_1_count, 170,"ms ","touch_1_count", U32_REGS_FLAG, 67, 396, 0x303f6, 1, 5, 1 },//!< "ms " &save &def
{ &def_touch_2_count, NULL, NULL, (u8*)&regs_main.touch_regs.vars.touch_2_count, 174,"ms ","touch_2_count", U32_REGS_FLAG, 68, 400, 0x303f8, 1, 5, 1 },//!< "ms " &save &def
{ &def_touch_3_count, NULL, NULL, (u8*)&regs_main.touch_regs.vars.touch_3_count, 178,"ms ","touch_3_count", U32_REGS_FLAG, 69, 404, 0x303fa, 1, 5, 1 },//!< "ms " &save &def
{ &def_touch_1_liters, NULL, NULL, (u8*)&regs_main.touch_regs.vars.touch_1_liters, 182,"ms ","touch_1_liters", FLOAT_REGS_FLAG, 70, 408, 0x303fc, 1, 5, 1 },//!< "ms " &save &def
{ &def_touch_2_liters, NULL, NULL, (u8*)&regs_main.touch_regs.vars.touch_2_liters, 186,"ms ","touch_2_liters", FLOAT_REGS_FLAG, 71, 412, 0x303fe, 1, 5, 1 },//!< "ms " &save &def
{ &def_touch_3_liters, NULL, NULL, (u8*)&regs_main.touch_regs.vars.touch_3_liters, 190,"ms ","touch_3_liters", FLOAT_REGS_FLAG, 72, 416, 0x30400, 1, 5, 1 },//!< "ms " &save &def
{ &def_touch_0_trshld, NULL, NULL, (u8*)&regs_main.touch_regs.vars.touch_0_trshld, 194,"in percents","touch_0_trshld", FLOAT_REGS_FLAG, 73, 420, 0x30402, 1, 5, 1 },//!< "in percents" &save &def
{ &def_touch_1_trshld, NULL, NULL, (u8*)&regs_main.touch_regs.vars.touch_1_trshld, 198,"in percents","touch_1_trshld", FLOAT_REGS_FLAG, 74, 424, 0x30404, 1, 5, 1 },//!< "in percents" &save &def
{ &def_touch_2_trshld, NULL, NULL, (u8*)&regs_main.touch_regs.vars.touch_2_trshld, 202,"in percents","touch_2_trshld", FLOAT_REGS_FLAG, 75, 428, 0x30406, 1, 5, 1 },//!< "in percents" &save &def
{ &def_touch_3_trshld, NULL, NULL, (u8*)&regs_main.touch_regs.vars.touch_3_trshld, 206,"in percents","touch_3_trshld", FLOAT_REGS_FLAG, 76, 432, 0x30408, 1, 5, 1 },//!< "in percents" &save &def
{ NULL, NULL, NULL, (u8*)&regs_main.touch_regs.vars.filter_use, 210,"use inside filter fot touch sensors or not ","filter_use", U32_REGS_FLAG, 77, 436, 0x3040a, 1, 5, 1 },//!< "use inside filter fot touch sensors or not " &save
{ &def_touch_handle_period, NULL, NULL, (u8*)&regs_main.touch_regs.vars.touch_handle_period, 214,"in ms period of handle touchs","touch_handle_period", U32_REGS_FLAG, 78, 440, 0x3040c, 1, 5, 1 },//!< "in ms period of handle touchs" &save &def
{ &def_by_time, NULL, NULL, (u8*)&regs_main.touch_regs.vars.by_time, 218,"by time or counter","by_time", U32_REGS_FLAG, 79, 444, 0x3040e, 1, 5, 1 },//!< "by time or counter" &save &def
{ &def_servo_0, &def_min_servo_0, &def_max_servo_0, (u8*)&regs_main.servo_control_part.vars.servo_0, 0,"servo pwm value [0;100]","servo_0", FLOAT_REGS_FLAG, 80, 448, 0x307d0, 1, 193, 2 },//!<"servo pwm value [0;100]" &def &min &max
{ &def_servo_1, &def_min_servo_1, &def_max_servo_1, (u8*)&regs_main.servo_control_part.vars.servo_1, 0,"servo pwm value [0;100]","servo_1", FLOAT_REGS_FLAG, 81, 452, 0x307d2, 1, 193, 2 },//!<"servo pwm value [0;100]" &def &min &max
{ &def_servo_2, &def_min_servo_2, &def_max_servo_2, (u8*)&regs_main.servo_control_part.vars.servo_2, 0,"servo pwm value [0;100]","servo_2", FLOAT_REGS_FLAG, 82, 456, 0x307d4, 1, 193, 2 },//!<"servo pwm value [0;100]" &def &min &max
{ &def_servo_3, &def_min_servo_3, &def_max_servo_3, (u8*)&regs_main.servo_control_part.vars.servo_3, 0,"servo pwm value [0;100]","servo_3", FLOAT_REGS_FLAG, 83, 460, 0x307d6, 1, 193, 2 },//!<"servo pwm value [0;100]" &def &min &max
{ NULL, NULL, NULL, (u8*)&regs_main.di_control.vars.pin_state, 0,"current states of digital inputs","pin_state", U32_REGS_FLAG, 84, 464, 0x30bb8, 1, 1, 3 },//!<"current states of digital inputs"
{ NULL, NULL, NULL, (u8*)&regs_main.sync_time_regs.vars.sync_sys_tick_dev, 0,"deviation between master and slave","sync_sys_tick_dev", S32_REGS_FLAG, 85, 468, 0x30fa0, 1, 3, 4 },//!< "deviation between master and slave" &ro
{ NULL, NULL, NULL, (u8*)&regs_main.sync_time_regs.vars.sync_sys_tick_slave, 0,"time read from slave","sync_sys_tick_slave", U64_REGS_FLAG, 86, 472, 0x30fa2, 1, 3, 4 },//!< "time read from slave" &ro
{ NULL, NULL, NULL, (u8*)&regs_main.sync_time_regs.vars.sync_sys_tick_master, 0,"time read from master","sync_sys_tick_master", U64_REGS_FLAG, 87, 480, 0x30fa6, 1, 3, 4 },//!< "time read from master" &ro
{ NULL, NULL, NULL, (u8*)&regs_main.sync_time_regs.vars.sync_average_time_ms, 0,"average send receive time ","sync_average_time_ms", U16_REGS_FLAG, 88, 488, 0x30faa, 1, 3, 4 },//!< "average send receive time " &ro
{ NULL, NULL, NULL, (u8*)&regs_main.sync_time_regs.vars.sync_last_req_time_ms, 0,"last send receive time ","sync_last_req_time_ms", U16_REGS_FLAG, 89, 490, 0x30fab, 1, 3, 4 },//!< "last send receive time " &ro
{ NULL, NULL, NULL, (u8*)&regs_main.sync_time_regs.vars.sync_active, 0,"activated measurement","sync_active", U16_REGS_FLAG, 90, 492, 0x30fac, 1, 3, 4 },//!< "activated measurement" &ro
{ NULL, NULL, NULL, (u8*)&regs_main.sr04_reg.vars.lap_state, 0,"state sr04, bit0 - activated, bit1 - echo signal received","lap_state", U16_REGS_FLAG, 91, 494, 0x31004, 1, 3, 5 },//!< "state sr04, bit0 - activated, bit1 - echo signal received" &ro
{ NULL, NULL, NULL, (u8*)&regs_main.sr04_reg.vars.lap_distance, 0,"current distance","lap_distance", FLOAT_REGS_FLAG, 92, 496, 0x31005, 1, 3, 5 },//!< "current distance" &ro
{ NULL, NULL, NULL, (u8*)&regs_main.sr04_reg.vars.lap, 0,"when we have sharp change of a distance, save it ","lap", U64_REGS_FLAG, 93, 500, 0x31007, 1, 3, 5 },//!< "when we have sharp change of a distance, save it " &ro
{ NULL, NULL, NULL, (u8*)&regs_main.sr04_reg.vars.lap_paired_dev, 0,"lap from paired device","lap_paired_dev", U64_REGS_FLAG, 94, 508, 0x3100b, 1, 3, 5 },//!< "lap from paired device" &ro
{ NULL, NULL, NULL, (u8*)&regs_main.sr04_reg.vars.distance_filtered, 0,"current distance filterd","distance_filtered", FLOAT_REGS_FLAG, 95, 516, 0x3100f, 1, 3, 5 },//!< "current distance filterd" &ro
{ NULL, NULL, NULL, (u8*)&regs_main.feeder_reg.vars.feeder_counter, 222,"how many time step motor started","feeder_counter", U32_REGS_FLAG, 96, 520, 0x31068, 1, 7, 6 },//!<"how many time step motor started" &save &ro
{ &def_feeder_interval, &def_min_feeder_interval, NULL, (u8*)&regs_main.feeder_reg.vars.feeder_interval, 226,"minutes between a feeds","feeder_interval", U16_REGS_FLAG, 97, 524, 0x3106a, 1, 69, 6 },//!< "minutes between a feeds" &save &def &min
{ NULL, NULL, NULL, (u8*)&regs_main.feeder_reg.vars.feeder_reserv0, 0,"reserved","feeder_reserv0", U16_REGS_FLAG, 98, 526, 0x3106b, 1, 1, 6 },//!< "reserved"
{ &def_feeder_time_sec, &def_min_feeder_time_sec, &def_max_feeder_time_sec, (u8*)&regs_main.feeder_reg.vars.feeder_time_sec, 228,"turn time is seconds","feeder_time_sec", FLOAT_REGS_FLAG, 99, 528, 0x3106c, 1, 197, 6 },//!< "turn time is seconds" &save &def &min &max
{ NULL, NULL, NULL, (u8*)&regs_main.polisher_reg.vars.polisher_sec, 232,"how many time in seconds it was active","polisher_sec", U32_REGS_FLAG, 100, 532, 0x310cc, 1, 7, 7 },//!<"how many time in seconds it was active" &save &ro
{ NULL, NULL, NULL, (u8*)&regs_main.polisher_reg.vars.polisher_last_sec, 0,"how many time in seconds it was active last session","polisher_last_sec", U32_REGS_FLAG, 101, 536, 0x310ce, 1, 3, 7 },//!<"how many time in seconds it was active last session" &ro
{ &def_polisher_speed, NULL, &def_max_polisher_speed, (u8*)&regs_main.polisher_reg.vars.polisher_speed, 236,"polisher_speed","polisher_speed", U16_REGS_FLAG, 102, 540, 0x310d0, 1, 133, 7 },//!< "polisher_speed" &save &def &max
{ &def_polisher_direction, NULL, &def_max_polisher_direction, (u8*)&regs_main.polisher_reg.vars.polisher_direction, 238,"polisher_direction","polisher_direction", U16_REGS_FLAG, 103, 542, 0x310d1, 1, 133, 7 },//!< "polisher_direction" &save &def &max
{ NULL, NULL, NULL, (u8*)&regs_main.test_int_reg.vars.test_int_state, 0,"current state","test_int_state", U32_REGS_FLAG, 104, 544, 0x31130, 1, 3, 8 },//!<"current state" &ro
{ NULL, NULL, NULL, (u8*)&regs_main.test_int_reg.vars.test_int_command, 0,"command","test_int_command", U32_REGS_FLAG, 105, 548, 0x31132, 1, 3, 8 },//!<"command" &ro
{ NULL, NULL, NULL, (u8*)&regs_main.test_int_reg.vars.test_int_component, 0,"component to be tested","test_int_component", U32_REGS_FLAG, 106, 552, 0x31134, 1, 1, 8 },//!<"component to be tested"
{ NULL, NULL, NULL, (u8*)&regs_main.test_int_reg.vars.test_int_type, 0,"test type - check, stress, performance","test_int_type", U32_REGS_FLAG, 107, 556, 0x31136, 1, 1, 8 },//!<"test type - check, stress, performance"
{ NULL, NULL, NULL, (u8*)&regs_main.test_int_reg.vars.test_int_result, 0,"overall result","test_int_result", U32_REGS_FLAG, 108, 560, 0x31138, 1, 3, 8 },//!<"overall result" &ro
{ &def_morse_unit_time_ms, &def_min_morse_unit_time_ms, &def_max_morse_unit_time_ms, (u8*)&regs_main.morse_reg.vars.morse_unit_time_ms, 240,"morse time ms","morse_unit_time_ms", U16_REGS_FLAG, 109, 564, 0x31194, 1, 197, 9 },//!<"morse time ms" &save &def &min &max
{ &def_morse_message_len, &def_min_morse_message_len, &def_max_morse_message_len, (u8*)&regs_main.morse_reg.vars.morse_message_len, 242,"max morse message len","morse_message_len", U16_REGS_FLAG, 110, 566, 0x31195, 1, 197, 9 },//!<"max morse message len" &save &def &min &max
{ &def_morse_settings, NULL, NULL, (u8*)&regs_main.morse_reg.vars.morse_settings, 244,"morse settings bit0 - server","morse_settings", U16_REGS_FLAG, 111, 568, 0x31196, 1, 5, 9 },//!<"morse settings bit0 - server" &save &def
{ NULL, NULL, NULL, (u8*)&regs_main.morse_reg.vars.morse_message_position, 0,"morse message position","morse_message_position", U16_REGS_FLAG, 112, 570, 0x31197, 1, 3, 9 },//!<"morse message position" &ro
{ NULL, NULL, NULL, (u8*)&regs_main.morse_reg.vars.morse_message[0], 0,"morse message to broadcast","morse_message", U8_REGS_FLAG, 113, 572, 0x31198, 32, 3, 9 },//!<"morse message to broadcast" &ro
{ NULL, NULL, NULL, (u8*)&regs_main.morse_reg.vars.morse_send[0], 0,"morse message to broadcast","morse_send", U8_REGS_FLAG, 114, 604, 0x311a8, 32, 3, 9 },//!<"morse message to broadcast" &ro
{ NULL, NULL, NULL, (u8*)&regs_main.morse_reg.vars.morse_line_1[0], 0,"morse messages received ","morse_line_1", U8_REGS_FLAG, 115, 636, 0x311b8, 32, 3, 9 },//!<"morse messages received " &ro
{ NULL, NULL, NULL, (u8*)&regs_main.morse_reg.vars.morse_line_2[0], 0,"morse messages received ","morse_line_2", U8_REGS_FLAG, 116, 668, 0x311c8, 32, 3, 9 },//!<"morse messages received " &ro
{ NULL, NULL, NULL, (u8*)&regs_main.morse_reg.vars.morse_line_3[0], 0,"morse messages received ","morse_line_3", U8_REGS_FLAG, 117, 700, 0x311d8, 32, 3, 9 },//!<"morse messages received " &ro
{ NULL, NULL, NULL, (u8*)&regs_main.morse_reg.vars.morse_line_4[0], 0,"morse messages received ","morse_line_4", U8_REGS_FLAG, 118, 732, 0x311e8, 32, 3, 9 },//!<"morse messages received " &ro
{ NULL, NULL, NULL, (u8*)&regs_main.morse_reg.vars.morse_line_5[0], 0,"morse messages received ","morse_line_5", U8_REGS_FLAG, 119, 764, 0x311f8, 32, 3, 9 },//!<"morse messages received " &ro
{ NULL, NULL, NULL, (u8*)&regs_main.morse_reg.vars.morse_counter, 0,"morse messages sent and received counter","morse_counter", U32_REGS_FLAG, 120, 796, 0x31208, 1, 3, 9 },//!<"morse messages sent and received counter" &ro
{ NULL, NULL, NULL, (u8*)&regs_main.battery_state_reg.vars.battery_level, 0,"0 - 100","battery_level", U16_REGS_FLAG, 121, 800, 0x3125c, 1, 3, 10 },//!<"0 - 100" &ro
{ &def_battery_min_level, &def_min_battery_min_level, &def_max_battery_min_level, (u8*)&regs_main.battery_state_reg.vars.battery_min_level, 246,"min level to start","battery_min_level", U16_REGS_FLAG, 122, 802, 0x3125d, 1, 197, 10 },//!<"min level to start" &save &def &min &max
{ NULL, NULL, NULL, (u8*)&regs_main.battery_state_reg.vars.battery_voltage, 0,"for exmpl 4.2v","battery_voltage", FLOAT_REGS_FLAG, 123, 804, 0x3125e, 1, 3, 10 },//!<"for exmpl 4.2v" &ro
{ NULL, NULL, NULL, (u8*)&regs_main.battery_state_reg.vars.battery_settings, 248,"bit 0 - activate level control","battery_settings", U16_REGS_FLAG, 124, 808, 0x31260, 1, 5, 10 },//!<"bit 0 - activate level control" &save
{ NULL, NULL, NULL, (u8*)&regs_main.scd41_reg.vars.scd41_co2_level, 0,"CO2 level in ppm","scd41_co2_level", U16_REGS_FLAG, 125, 810, 0x312c0, 1, 3, 11 },//!< "CO2 level in ppm" &ro
{ NULL, NULL, NULL, (u8*)&regs_main.scd41_reg.vars.scd41_temperature, 0,"Temperature in Celsius","scd41_temperature", FLOAT_REGS_FLAG, 126, 812, 0x312c1, 1, 3, 11 },//!< "Temperature in Celsius" &ro
{ NULL, NULL, NULL, (u8*)&regs_main.scd41_reg.vars.scd41_humidity, 0,"Humidity in percentage","scd41_humidity", FLOAT_REGS_FLAG, 127, 816, 0x312c3, 1, 3, 11 },//!< "Humidity in percentage" &ro
{ NULL, NULL, NULL, (u8*)&regs_main.scd41_reg.vars.scd41_settings, 250,"Settings for SCD41 sensor","scd41_settings", U16_REGS_FLAG, 128, 820, 0x312c5, 1, 5, 11 },//!< "Settings for SCD41 sensor" &save
{ &def_scd41_measur_interval, &def_min_scd41_measur_interval, &def_max_scd41_measur_interval, (u8*)&regs_main.scd41_reg.vars.scd41_measur_interval, 252,"minutes between a measurement","scd41_measur_interval", U16_REGS_FLAG, 129, 822, 0x312c6, 1, 197, 11 },//!< "minutes between a measurement" &save &def &min &max
{ NULL, NULL, NULL, (u8*)&regs_main.scd41_reg.vars.scd41_index_in_array, 254,"Index in array","scd41_index_in_array", U16_REGS_FLAG, 130, 824, 0x312c7, 1, 7, 11 },//!< "Index in array" &ro &save
{ NULL, NULL, NULL, (u8*)&regs_main.scd41_reg.vars.scd41_co2_array[0], 256,"CO2 level in ppm","scd41_co2_array", U16_REGS_FLAG, 131, 826, 0x312c8, 48, 7, 11 },//!< "CO2 level in ppm" &ro &save
};
regs_description_t const regs_description_user[NUM_OF_CLIENT_VARS]={
{ NULL, NULL, NULL, (u8*)&regs_main.client_part_0.vars.cli_mdb_addr, 0,"modbus address","cli_mdb_addr", U16_REGS_FLAG, 132, 922, 0x30000, 1, 1, 0xc03 },//!<"modbus address"
{ NULL, NULL, NULL, (u8*)&regs_main.client_part_0.vars.cli_ip[0], 0,"device ip address, warning!!! ","cli_ip", U8_REGS_FLAG, 133, 924, 0x30001, 4, 1, 0xc03 },//!<"device ip address, warning!!! "
{ NULL, NULL, NULL, (u8*)&regs_main.client_part_0.vars.cli_netmask[0], 0,"netmask address for main wifi net","cli_netmask", U8_REGS_FLAG, 134, 928, 0x30003, 4, 1, 0xc03 },//!<"netmask address for main wifi net",
{ NULL, NULL, NULL, (u8*)&regs_main.client_part_0.vars.cli_gate[0], 0,"gateaway address, warning!!! ","cli_gate", U8_REGS_FLAG, 135, 932, 0x30005, 4, 1, 0xc03 },//!<"gateaway address, warning!!! "
{ NULL, NULL, NULL, (u8*)&regs_main.client_part_0.vars.cli_slip_ip[0], 0,"ip address for local net","cli_slip_ip", U8_REGS_FLAG, 136, 936, 0x30007, 4, 1, 0xc03 },//!<"ip address for local net",
{ NULL, NULL, NULL, (u8*)&regs_main.client_part_0.vars.cli_slip_netmask[0], 0,"netmask address for local net","cli_slip_netmask", U8_REGS_FLAG, 137, 940, 0x30009, 4, 1, 0xc03 },//!<"netmask address for local net",
{ NULL, NULL, NULL, (u8*)&regs_main.client_part_0.vars.cli_slip_gate[0], 0,"gateaway address for local net","cli_slip_gate", U8_REGS_FLAG, 138, 944, 0x3000b, 4, 1, 0xc03 },//!<"gateaway address for local net",
{ NULL, NULL, NULL, (u8*)&regs_main.sync_time_client.vars.sys_tick_slave, 0,"time read from slave","sys_tick_slave", U64_REGS_FLAG, 139, 948, 0x3003f, 1, 3, 0xd03 },//!< "time read from slave" &ro
{ NULL, NULL, NULL, (u8*)&regs_main.client_part_1.vars.cli_num_of_vars, 0,"cli_num_of_vars","cli_num_of_vars", U16_REGS_FLAG, 140, 956, 0x30050, 1, 3, 0xe03 },//!<"number of vars self + config(user) &ro
{ NULL, NULL, NULL, (u8*)&regs_main.client_part_1.vars.cli_client_num_of_vars, 0,"number of client vars self","cli_client_num_of_vars", U16_REGS_FLAG, 141, 958, 0x30051, 1, 3, 0xe03 },//!<"number of client vars self" &ro
{ NULL, NULL, NULL, (u8*)&regs_main.sync_time_regs_from_client.vars.cli_sys_tick_dev, 0,"deviation between master and slave","cli_sys_tick_dev", S32_REGS_FLAG, 142, 960, 0x30fa0, 1, 3, 0xf03 },//!< "deviation between master and slave" &ro
{ NULL, NULL, NULL, (u8*)&regs_main.sync_time_regs_from_client.vars.cli_sys_tick_slave, 0,"time read from slave","cli_sys_tick_slave", U64_REGS_FLAG, 143, 964, 0x30fa2, 1, 3, 0xf03 },//!< "time read from slave" &ro
{ NULL, NULL, NULL, (u8*)&regs_main.sync_time_regs_from_client.vars.cli_sys_tick_master, 0,"time read from master","cli_sys_tick_master", U64_REGS_FLAG, 144, 972, 0x30fa6, 1, 3, 0xf03 },//!< "time read from master" &ro
{ NULL, NULL, NULL, (u8*)&regs_main.sync_time_regs_from_client.vars.cli_average_time_ms, 0,"average send receive time ","cli_average_time_ms", U16_REGS_FLAG, 145, 980, 0x30faa, 1, 3, 0xf03 },//!< "average send receive time " &ro
{ NULL, NULL, NULL, (u8*)&regs_main.sync_time_regs_from_client.vars.cli_last_req_time_ms, 0,"last send receive time ","cli_last_req_time_ms", U16_REGS_FLAG, 146, 982, 0x30fab, 1, 3, 0xf03 },//!< "last send receive time " &ro
{ NULL, NULL, NULL, (u8*)&regs_main.sync_time_regs_from_client.vars.cli_sync_state, 0,"activated measurement","cli_sync_state", U16_REGS_FLAG, 147, 984, 0x30fac, 1, 3, 0xf03 },//!< "activated measurement" &ro
{ NULL, NULL, NULL, (u8*)&regs_main.sr04_reg_client.vars.cli_state, 0,"state sr04, bit0 - activated, bit1 - echo signal received","cli_state", U16_REGS_FLAG, 148, 986, 0x31004, 1, 3, 0x1003 },//!< "state sr04, bit0 - activated, bit1 - echo signal received" &ro
{ NULL, NULL, NULL, (u8*)&regs_main.sr04_reg_client.vars.cli_distance, 0,"current distance","cli_distance", FLOAT_REGS_FLAG, 149, 988, 0x31005, 1, 3, 0x1003 },//!< "current distance" &ro
{ NULL, NULL, NULL, (u8*)&regs_main.sr04_reg_client.vars.cli_lap, 0,"when we have sharp change of a distance, save it ","cli_lap", U64_REGS_FLAG, 150, 992, 0x31007, 1, 3, 0x1003 },//!< "when we have sharp change of a distance, save it " &ro
{ NULL, NULL, NULL, (u8*)&regs_main.sr04_reg_client.vars.cli_lap_paired_dev, 0,"lap from paired device","cli_lap_paired_dev", U64_REGS_FLAG, 151, 1000, 0x3100b, 1, 3, 0x1003 },//!< "lap from paired device" &ro
{ NULL, NULL, NULL, (u8*)&regs_main.sr04_reg_client.vars.cli_distance_filtered, 0,"current distance filtered","cli_distance_filtered", FLOAT_REGS_FLAG, 152, 1008, 0x3100f, 1, 3, 0x1003 },//!< "current distance filtered" &ro
};

const regs_description_t * regs_description_client = regs_description_user;
static u32 num_of_list_descriptions = 0;

int regs_description_list_add_new(regs_description_list_t regs_table){
   int res = 0;
   if ((regs_table.description != NULL) && (regs_table.num_of_regs > 0) && (num_of_list_descriptions < LIST_DESCRIPTIONS_SIZE)){
      regs_description_list[num_of_list_descriptions] = regs_table;//!< add new description to list
      num_of_list_descriptions++;
   }else{
      res = -2;//!< wrong input params or list full
   }
   return res;
}

const char *regs_description_list_get_space_name(u8 ind){
   const char * result = NULL;
   if (ind < num_of_list_descriptions){
      result = regs_description_list[ind].space_name;
   }
   return result;
}

u32 regs_description_list_get_num_of_regs(u8 ind){
   u32 result = 0;
   if (ind < num_of_list_descriptions){
      result = regs_description_list[ind].num_of_regs;
   }
   return result;
}

u8 * regs_description_list_get_buffer(u8 ind){
   u8 * result = NULL;
   if (ind < num_of_list_descriptions){
      result = (u8*)regs_description_list[ind].saved_regs_buffer;
   }
   return result;
}
size_t regs_description_list_get_saved_buffer_size(u8 ind){
   size_t result = 0;
   if (ind < num_of_list_descriptions){
      result = regs_description_list[ind].saved_regs_buffer_size;
   }
   return result;
}
int regs_description_get_by_index(regs_template_t * regs_template, u8 ind){
   int result = -1;
   if ((regs_template->table_ind < num_of_list_descriptions) && (regs_template != NULL)){
      const regs_description_t * reg_desc = &regs_description_list[regs_template->table_ind].description[ind];
      if(reg_desc != NULL){
         memcpy(regs_template, reg_desc, sizeof(regs_description_t));
         regs_template->size_in_bytes = regs_template->size * regs_size_in_byte(regs_template->type);
         result = 0;
      }
   }
   return result;
}
int regs_description_get_by_name(regs_template_t *regs_template) {
   int result = -1;
   u32 name_size = strlen(regs_template->name);
   regs_template->table_ind = 0;

   if (name_size > REGS_MAX_NAME_SIZE) {
      name_size = REGS_MAX_NAME_SIZE;
   }
   for (int i = 0; i < LIST_DESCRIPTIONS_SIZE; i++) {
      if (regs_description_list[i].description != NULL) {
         const regs_description_t *reg_desc = &regs_description_list[i].description[0]; //!< take first element of list for search
         for (int j = 0; j < regs_description_list[i].num_of_regs; j++) {
            if ((name_size > 0) && (reg_desc[j].name[name_size] == '\0') &&
                (memcmp(regs_template->name, reg_desc[j].name, name_size) == 0)) {
               memcpy(regs_template, &reg_desc[j], sizeof(regs_description_t));
               regs_template->size_in_bytes = regs_template->size * regs_size_in_byte(regs_template->type);
               regs_template->table_ind = i;
               result = 0;
               break;
            }
         }
         if (result == 0) {
            break; //!< if we have found description, we have no need to search in other lists
         }
      } else {
         break; //!< if we have empty description, we have no more descriptions in list
      }
   }
   return result;
}

int regs_description_get_by_guid(regs_template_t * regs_template){
   int result = -1;
   regs_template->table_ind = 0;
   for (int i = 0; i < LIST_DESCRIPTIONS_SIZE; i++){
      if (regs_description_list[i].description != NULL){
         const regs_description_t * reg_desc = bin_search_guid(regs_template->guid,
                                                                  regs_description_list[i].description,
                                                                  regs_description_list[i].num_of_regs);
         if (reg_desc != NULL){
               memcpy(regs_template, reg_desc, sizeof(regs_description_t));
               regs_template->size_in_bytes = regs_template->size * regs_size_in_byte(regs_template->type);
               regs_template->table_ind = i;
               result = 0;
               break;
         }
      }else{
         break;//!< no more descriptions in list
      }
   }
   return result;
}


static const regs_description_t * bin_search_guid(u32 guid, const regs_description_t * regs_array, const u16 size){
   const regs_description_t * result = NULL;
   int left = 0;
   int right = size-1;
   int current = (left + right)/2;
   if(regs_array!= NULL){
      while (left <= right){
         current = (left + right)/2;
         if (guid == regs_array[current].guid){
               result = &regs_array[current];
               break;
         }else if (guid > regs_array[current].guid){
               left = current + 1;
         }else {
               right =current - 1;
         }
      }
   }
   return result;
}

int regs_description_get_by_address(regs_template_t * regs_template){
   int result = -1;
   regs_template->table_ind = 0;
   for (int i = 0; i < LIST_DESCRIPTIONS_SIZE; i++){
      if (regs_description_list[i].description != NULL){
         const regs_description_t * reg_desc = bin_search_address(regs_template->p_value,
                                                                  regs_description_list[i].description,
                                                                  regs_description_list[i].num_of_regs);
         if (reg_desc != NULL){
            memcpy(regs_template, reg_desc, sizeof(regs_description_t));
            regs_template->size_in_bytes = regs_template->size * regs_size_in_byte(regs_template->type);
            regs_template->table_ind = i;
            result = 0;
            break;
         }
      }else{
         break;//!< no more descriptions in list
      }
   }
   return result;
}

static const regs_description_t * bin_search_address(void * address, const regs_description_t * regs_array, const u16 size){
   const regs_description_t * result = NULL;
   int left = 0;
   int right = size-1;
   int current = (left + right)/2;
   if(regs_array!= NULL){
      while (left <= right){
         current = (left + right)/2;
         if ((address >= (void*)regs_array[current].p_value) &&
            (address < (void*)(regs_array[current].p_value+regs_size_in_byte(regs_array[current].type)*regs_array[current].size))){
            result = &regs_array[current];
            break;
         }else if (address > (void*)regs_array[current].p_value){
            left = current + 1;
         }else {
            right =current - 1;
         }
      }
   }
   return result;
}
int regs_description_write_value(regs_template_t * regs_template, const u8 * value){
   int res = 0;
   semaphore_take(regs_access_mutex, portMAX_DELAY );{
      if ((regs_template->p_value != NULL) && (value != NULL)){
         memcpy(regs_template->p_value,value,regs_template->size_in_bytes);
         if (regs_template->property & SAVING){
            mirror_access_write(regs_template);
         }
      }else{
         res = -1;
      }
   }semaphore_release(regs_access_mutex);
   return res;
}
int regs_description_write_value_by_address(const void * address, const u8 * value){
   int res = -1;
   regs_template_t regs_template;
   regs_template.p_value = (void *)address;
   if(regs_description_get_by_address(&regs_template)==0){
      res = regs_description_write_value(&regs_template, value);
   }
   return res;
}

u8 regs_description_is_writable (regs_template_t *regs_template){
   return !regs_description_flag_check(regs_template, READ_ONLY);
}

u8 regs_description_is_credential(regs_template_t *regs_template){
   return regs_description_flag_check(regs_template, CREDENTIAL_FLAG);
}

u8 regs_description_flag_check (regs_template_t *regs_template, u8 flag){
    u8 result = 0;
    if (regs_template != NULL){
        if(regs_template->property & flag){
            result = 1;
        }else{
            result = 0;
        }
    }
    return result;
}

u32 regs_description_get_regs_string_value(u16 reg_id,u8 reg_num, char * buffer, u32 buffer_size){
   u32 result = 0;
   u64 val_u64 = 0;
   s32 temp_i32 = 0;
   s16 temp_i16 = 0;
   i8 temp_i8 = 0;
   s64 temp_i64 = 0;
   float val_float = 0.0f;
   double val_double = 0.0;
   regs_template_t regs_template = {0};
   regs_template.ind = reg_id;
   u32 pos = 0;
   char temp_buf[96] = {0};
   u32 temp_buf_len = 0;
   if (buffer != NULL || buffer_size != 0){
      for(u32 i=0; i<reg_num; i++){
         if (regs_description_get_by_ind(&regs_template) == 0){
            temp_buf_len+=sprintf(temp_buf,"%s: ",regs_template.name);
            for (u16 i = 0; i < regs_template.size; i++) {
               switch (regs_template.type){
               case U8_REGS_FLAG:
               case U16_REGS_FLAG:
               case U32_REGS_FLAG:
               case U64_REGS_FLAG:
               case TIME_REGS_FLAG:
                   val_u64 = 0;
                   memcpy(&val_u64,(regs_template.p_value + i*regs_size_in_byte(regs_template.type)),regs_size_in_byte(regs_template.type));
                   temp_buf_len+=sprintf(&temp_buf[temp_buf_len],"%llu,",val_u64);
                   break;
               case I8_REGS_FLAG:
                   temp_i8 = 0;
                   memcpy(&temp_i8,(regs_template.p_value + i*regs_size_in_byte(regs_template.type)),regs_size_in_byte(regs_template.type));
                   temp_buf_len+=sprintf(&temp_buf[temp_buf_len],"%i,",temp_i8);
                   break;
               case S16_REGS_FLAG:
                   temp_i16 = 0;
                   memcpy(&temp_i16,(regs_template.p_value + i*regs_size_in_byte(regs_template.type)),regs_size_in_byte(regs_template.type));
                   temp_buf_len+=sprintf(&temp_buf[temp_buf_len],"%i,",temp_i16);
                   break;
               case S32_REGS_FLAG:
               case INT_REGS_FLAG:
                   temp_i32 = 0;
                   memcpy(&temp_i32,(regs_template.p_value + i*regs_size_in_byte(regs_template.type)),regs_size_in_byte(regs_template.type));
                   temp_buf_len+=sprintf(&temp_buf[temp_buf_len],"%li,",temp_i32);
                   break;
               case S64_REGS_FLAG:
                   temp_i64 = 0;
                   memcpy(&temp_i64,(regs_template.p_value + i*regs_size_in_byte(regs_template.type)),regs_size_in_byte(regs_template.type));
                   temp_buf_len+=sprintf(&temp_buf[temp_buf_len],"%lli,",temp_i64);
                   break;
               case FLOAT_REGS_FLAG:
                   val_float = 0.0f;
                   memcpy(&val_float,(regs_template.p_value + i*regs_size_in_byte(regs_template.type)),regs_size_in_byte(regs_template.type));
                   if(isnanf(val_float) || isinff(val_float)){
                     temp_buf_len+=sprintf(&temp_buf[temp_buf_len],"\"%f\",",(double)val_float);
                   }else{
                     temp_buf_len+=sprintf(&temp_buf[temp_buf_len],"%f,",(double)val_float);
                   }
                   break;
               case DOUBLE_REGS_FLAG:
                   val_double = 0.0;
                   memcpy(&val_double,(regs_template.p_value + i*regs_size_in_byte(regs_template.type)),regs_size_in_byte(regs_template.type));
                   if(isnanf((float)val_double) || isinff((float)val_double)){
                     temp_buf_len+=sprintf(&temp_buf[temp_buf_len],"\"%f\",",val_double);
                   }else{
                     temp_buf_len+=sprintf(&temp_buf[temp_buf_len],"%f,",val_double);
                   }
                   break;
               }
               if (temp_buf_len > 0u){
                  if (pos + temp_buf_len < buffer_size){
                     memcpy(buffer + pos, temp_buf, temp_buf_len);
                     pos += temp_buf_len;
                     temp_buf_len = 0;
                     result = pos;
                  }else{
                     result = pos;
                     return result; //buffer overflow
                  }
               }
           }
         }else{
            break;
         }
         regs_template.ind++;
      }

   }
   return result;
}
/**
 * @brief regs_description_get_pointer_by_modbus
 * @param modbus_address [0;65635]
 * @param modbus_function {1,2,3,4}
 * @return  non NULL pointer if matched
 */
void * regs_description_get_pointer_by_modbus(u16 modbus_address, u8 modbus_function){
    void * p_value= NULL;
    int current;
    int left = 0;
    int right = NUM_OF_SELF_VARS-1;
    while (left <= right){  //binary search
        current = (left + right)/2;
        u8 reg_mdb_function = RD_MDB_FUNCTION(regs_description[current].modbus_description);
        u16 reg_mdb_address = RD_MDB_ADDRESS(regs_description[current].modbus_description);
        u16 reg_size_in_word = (regs_size_in_byte(regs_description[current].type)*regs_description[current].size)/2;
        if ((modbus_function == reg_mdb_function) &&
            ((modbus_address == reg_mdb_address)||((modbus_address > reg_mdb_address) &&
                     (modbus_address < (reg_mdb_address + reg_size_in_word))))){
            if (modbus_address == reg_mdb_address){
                p_value = regs_description[current].p_value;
            }else {
                p_value = regs_description[current].p_value + (modbus_address - reg_mdb_address) * 2;
            }
            break;
        }else if (modbus_address > reg_mdb_address){
            left = current + 1;
        }else {
            right =current - 1;
        }
    }
    return p_value;
}
int set_regs_def_values(u16 table_ind) {
   int res = 0;
   regs_template_t regs_template = { 0 };
   regs_template.table_ind = table_ind;
   u32 num_of_regs = regs_description_list_get_num_of_regs(table_ind);
   for (u32 i = 0; i < num_of_regs; i++) {
      if (regs_description_get_by_index(&regs_template, i) == 0) {
         if (regs_template.p_default == NULL) {
            memset(regs_template.p_value, 0, regs_template.size_in_bytes);
         } else {
            memcpy(regs_template.p_value, regs_template.p_default, regs_template.size_in_bytes);
         }
      }
   }
   return res;
}
int end_of_saved_reg_addr(void *reg_end_address) {
   u16 i;
   int result = -1;
   regs_template_t regs_template;
   for (i = 0; i < NUM_OF_SELF_VARS + NUM_OF_CLIENT_VARS; i++) {
      regs_template.ind = i;
      regs_description_get_by_ind(&regs_template);
      if ((regs_template.property & SAVING) && (reg_end_address == (regs_template.p_value + regs_template.size_in_bytes))) {
         result = i;
         break;
      }
   }
   return result;
}
/**
 * @brief regs_fill_temp_buffer - fillng temp buff
 * @param reg_address   - pointer to data where will write
 * @param reg   - write struct
 * @param temp_data_buffering - global temp buffer for writing data
 * @param index - index of register
 * @return
 */
int regs_fill_temp_buffer(void * reg_address,regs_access_t reg,temp_data_buffering_t * temp_data_buffering, int index){
    int res = 0;
    regs_template_t regs_template = {0};
    regs_template.ind = (u16)index;
    if (regs_description_get_by_ind(&regs_template)>=0){
        u8 reg_write_size = regs_size_in_byte(reg.flag);
        u32 shift = (u32)reg_address - (u32)regs_template.p_value;
        u8 regs_item_size = regs_size_in_byte(regs_template.type);
        u8 start_byte = shift % regs_item_size;/*0-7*/
        if ((u32)temp_data_buffering->reg_address != ((u32)reg_address - start_byte)){
            temp_data_buffering->reg_address = (void*)((u32)reg_address - start_byte);
            temp_data_buffering->byte_writed_flags = 0;
            temp_data_buffering->type = regs_template.type;
            temp_data_buffering->index = (u16)index;
            memset(temp_data_buffering->data.bytes,0,8);
        }
        reg_write_size = reg_write_size>(regs_item_size-start_byte)?(regs_item_size-start_byte):reg_write_size;
        memcpy(&temp_data_buffering->data.bytes[start_byte],&reg.value,reg_write_size);
        for(u32 i=start_byte;i<(reg_write_size + start_byte);i++){
            temp_data_buffering->byte_writed_flags |= BIT(i);
        }
        res = reg_write_size;
    }else{
        res =-1;
    }
    return res;
}
/**
 * @brief regs_check_temp_buffer
 * @param temp_data_buffering
 * @param index
 * @return positive value if register is full filled
 */
int regs_check_temp_buffer(temp_data_buffering_t * temp_data_buffering, int index){
    int res = 0;
    regs_template_t regs_template = {0};
    regs_template.ind = (u16)index;
    if (regs_description_get_by_ind(&regs_template)>=0){
        u8 regs_item_size = regs_size_in_byte(regs_template.type);
        res = 1;
        for(u32 i=0;i<regs_item_size;i++){
            if ((temp_data_buffering->byte_writed_flags & BIT(i)) == 0){
                res = 0;
                break;
            }
        }
    }else{
        res =-1;
    }
    return res;
}
/**
 * @brief regs_clear_temp_buffer clining
 * @param temp_data_buffering
 * @param index
 * @return positive value if register is full filling
 */
int regs_clear_temp_buffer(temp_data_buffering_t * temp_data_buffering){
    int res = 0;
    if (temp_data_buffering!=NULL){
        temp_data_buffering->reg_address = NULL;
        temp_data_buffering->byte_writed_flags = 0;
        temp_data_buffering->type = 0;
        temp_data_buffering->index = 0xffff;
        memset(temp_data_buffering->data.bytes,0,8);
    }
    return res;
}
#endif //REGS_DESCRIPTION_C
