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
#include "os_type.h"
#include "mirror_storage.h"
#include <string.h> /* memset */
#include <stdlib.h> /* atoi */
#include <stdio.h>

static const u16 def_mdb_addr = 3;
static const u8 def_ip[4] = {192,168,1,232};
static const u8 def_netmask[4] = {255,255,255,0};
static const u8 def_gate[4] = {192,168,1,1};
static const u8 def_slip_ip[4] = {172,16,1,200};
static const u8 def_slip_netmask[4] = {255,255,255,0};
static const u8 def_slip_gate[4] = {172,16,1,232};

static const u16 def_uart_debug_sets = 0xC853;
static const u8 def_fw_version[FW_VERSION_SIZE] = FW_VERSION;
static const u16 def_num_of_vars = NUM_OF_SELF_VARS;
static const u8 def_board_ver = (u8)BOARD_VERSION;
static const u16 def_permission = ENABLE_HTTP_FLAG;
static const u8 def_wifi_name[WIFI_NAME_LEN] = "sofi_plc_net";               //!<"must be strong full filled", &save &def
static const u8 def_wifi_password[WIFI_PASSWORD_LEN] = "bricsofi";            //!<"must be strong more then 8 byte", &save &def
static const u8 def_wifi_router_name[WIFI_ROUTER_NAME_LEN] = "UPC0369423";
static const u8 def_wifi_router_password[WIFI_ROUTER_PASSWORD_LEN] = "taG2acmsvabz";
static const u16 def_wifi_setting = WIFI_AP_STA;               //!<"type of wifi and settings" &save &def
static const float def_test_pwm_value = 50.0f;
const u32 def_table_version = 0xae3e;
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

regs_description_t const regs_description[NUM_OF_SELF_VARS]={
{ &def_mdb_addr, NULL, NULL, (u8*)&regs_global.vars.mdb_addr, 16,"modbus address","mdb_addr", U16_REGS_FLAG, 0, 0, 0x30000, 1, 5, 0 },//!<"modbus address" &save &def
{ &def_ip, NULL, NULL, (u8*)&regs_global.vars.ip[0], 18,"device ip address, warning!!! changes can lead to lost connection","ip", U8_REGS_FLAG, 1, 2, 0x30001, 4, 5, 0 },//!<"device ip address, warning!!! changes can lead to lost connection" &save &def &crtcl
{ &def_netmask, NULL, NULL, (u8*)&regs_global.vars.netmask[0], 22,"netmask address for main wifi net","netmask", U8_REGS_FLAG, 2, 6, 0x30003, 4, 5, 0 },//!<"netmask address for main wifi net", &save , &def , &crtcl
{ &def_gate, NULL, NULL, (u8*)&regs_global.vars.gate[0], 26,"gateaway address, warning!!! changes can lead to lost connection ","gate", U8_REGS_FLAG, 3, 10, 0x30005, 4, 5, 0 },//!<"gateaway address, warning!!! changes can lead to lost connection " &save &def &crtcl
{ &def_slip_ip, NULL, NULL, (u8*)&regs_global.vars.slip_ip[0], 30,"ip address for local net","slip_ip", U8_REGS_FLAG, 4, 14, 0x30007, 4, 5, 0 },//!<"ip address for local net",&save , &def &crtcl
{ &def_slip_netmask, NULL, NULL, (u8*)&regs_global.vars.slip_netmask[0], 34,"netmask address for local net","slip_netmask", U8_REGS_FLAG, 5, 18, 0x30009, 4, 5, 0 },//!<"netmask address for local net", &save , &def , &crtcl
{ &def_slip_gate, NULL, NULL, (u8*)&regs_global.vars.slip_gate[0], 38,"gateaway address for local net","slip_gate", U8_REGS_FLAG, 6, 22, 0x3000b, 4, 5, 0 },//!<"gateaway address for local net", &save, &def &crtcl
{ &def_wifi_name, NULL, NULL, (u8*)&regs_global.vars.wifi_name[0], 42,"must be strong full filled","wifi_name", U8_REGS_FLAG, 7, 26, 0x3000d, 12, 5, 0 },//!<"must be strong full filled", &save &def &crtcl
{ &def_wifi_password, NULL, NULL, (u8*)&regs_global.vars.wifi_password[0], 54,"must be strong 8 byte","wifi_password", U8_REGS_FLAG, 8, 38, 0x30013, 8, 5, 0 },//!<"must be strong 8 byte", &save &def &crtcl
{ &def_wifi_router_name, NULL, NULL, (u8*)&regs_global.vars.wifi_router_name[0], 62,"must be ended by zero","wifi_router_name", U8_REGS_FLAG, 9, 46, 0x30017, 32, 5, 0 },//!<"must be ended by zero", &save &def &crtcl
{ &def_wifi_router_password, NULL, NULL, (u8*)&regs_global.vars.wifi_router_password[0], 94,"must be more or equal 8 byte","wifi_router_password", U8_REGS_FLAG, 10, 78, 0x30027, 32, 5, 0 },//!<"must be more or equal 8 byte", &save &def &crtcl
{ &def_wifi_setting, NULL, NULL, (u8*)&regs_global.vars.wifi_setting, 126,"type of wifi and settings","wifi_setting", U16_REGS_FLAG, 11, 110, 0x30037, 1, 5, 0 },//!<"type of wifi and settings" &save &def &crtcl
{ NULL, NULL, NULL, (u8*)&regs_global.vars.reset_num, 128,"number of reset","reset_num", U32_REGS_FLAG, 12, 112, 0x30038, 1, 5, 0 },//!<"number of reset" &save
{ NULL, NULL, NULL, (u8*)&regs_global.vars.wifi_state, 0,"wifi state","wifi_state", U16_REGS_FLAG, 13, 116, 0x3003a, 1, 3, 0 },//!<"wifi state" &ro
{ NULL, NULL, NULL, (u8*)&regs_global.vars.command, 0,"command register","command", U16_REGS_FLAG, 14, 118, 0x3003b, 1, 1, 0 },//!<"command register"
{ &def_uart_debug_sets, NULL, NULL, (u8*)&regs_global.vars.uart_debug_sets, 132,"settings debug uart speed,parity and stop bits, default 115200 ,parity - none, 1 stop bit","uart_debug_sets", U16_REGS_FLAG, 15, 120, 0x3003c, 1, 5, 0 },//!<"settings debug uart speed,parity and stop bits, default 115200 ,parity - none, 1 stop bit" &save &def
{ NULL, NULL, NULL, (u8*)&regs_global.vars.device_state, 0,"BIT(0) - interlock","device_state", U16_REGS_FLAG, 16, 122, 0x3003d, 1, 3, 0 },//!< "BIT(0) - interlock" &ro
{ NULL, NULL, NULL, (u8*)&regs_global.vars.unused_3, 0,"unused_3","unused_3", U16_REGS_FLAG, 17, 124, 0x3003e, 1, 3, 0 },//!<"" &ro
{ NULL, NULL, NULL, (u8*)&regs_global.vars.sys_tick_counter, 0,"tick in ms","sys_tick_counter", U64_REGS_FLAG, 18, 126, 0x3003f, 1, 3, 0 },//!<"tick in ms",&ro
{ NULL, NULL, NULL, (u8*)&regs_global.vars.mac_addr[0], 0,"ethernet mac address","mac_addr", U8_REGS_FLAG, 19, 134, 0x30043, 6, 3, 0 },//!<"ethernet mac address",&ro
{ NULL, NULL, NULL, (u8*)&regs_global.vars.flags_succ_init, 0,"success inited modules","flags_succ_init", U32_REGS_FLAG, 20, 140, 0x30046, 1, 3, 0 },//!<"success inited modules" &ro for init in task
{ NULL, NULL, NULL, (u8*)&regs_global.vars.cur_free_heap, 0,"in bytes","cur_free_heap", U32_REGS_FLAG, 21, 144, 0x30048, 1, 3, 0 },//!<"in bytes",&ro
{ NULL, NULL, NULL, (u8*)&regs_global.vars.min_free_heap, 0,"in bytes","min_free_heap", U32_REGS_FLAG, 22, 148, 0x3004a, 1, 3, 0 },//!<"in bytes",&ro
{ NULL, NULL, NULL, (u8*)&regs_global.vars.debug_info[0], 0,"reserved use for debug","debug_info", U8_REGS_FLAG, 23, 152, 0x3004c, 8, 3, 0 },//!<"reserved use for debug"&ro
{ &def_num_of_vars, NULL, NULL, (u8*)&regs_global.vars.num_of_vars, 0,"num_of_vars","num_of_vars", U16_REGS_FLAG, 24, 160, 0x30050, 1, 3, 0 },//!<"number of vars self + config(user)&ro &def
{ NULL, NULL, NULL, (u8*)&regs_global.vars.temperature_mcu, 0,"temperature mcu Celsius","temperature_mcu", FLOAT_REGS_FLAG, 25, 162, 0x30051, 1, 3, 0 },//!<"temperature mcu Celsius" &ro
{ &def_fw_version, NULL, NULL, (u8*)&regs_global.vars.fw_version[0], 0,"version like 0.1.1.0","fw_version", U8_REGS_FLAG, 26, 166, 0x30053, 4, 3, 0 },//!<"version like 0.1.1.0",&ro,&def
{ &def_board_ver, NULL, NULL, (u8*)&regs_global.vars.board_ver, 0,"board version","board_ver", U16_REGS_FLAG, 27, 170, 0x30055, 1, 3, 0 },//!< "board version", &ro, &def
{ NULL, NULL, NULL, (u8*)&regs_global.vars.firmware_hash, 0,"hash os","firmware_hash", U32_REGS_FLAG, 28, 172, 0x30056, 1, 3, 0 },//!< "hash os" &ro
{ NULL, NULL, NULL, (u8*)&regs_global.vars.system_error, 0,"system global error","system_error", U32_REGS_FLAG, 29, 176, 0x30058, 1, 3, 0 },//!< "system global error" &ro
{ &def_permission, NULL, NULL, (u8*)&regs_global.vars.permission, 0,"flags with permissions","permission", U16_REGS_FLAG, 30, 180, 0x3005a, 1, 3, 0 },//!<"flags with permissions" &ro &def
{ NULL, NULL, NULL, (u8*)&regs_global.vars.async_flags, 0,"async flags","async_flags", U32_REGS_FLAG, 31, 182, 0x3005b, 1, 3, 0 },//!< "async flags" &ro
{ NULL, NULL, NULL, (u8*)&regs_global.vars.slip_packet_counter, 0,"count all slip packet","slip_packet_counter", U64_REGS_FLAG, 32, 186, 0x3005d, 1, 1, 0 },//!< "count all slip packet"
{ NULL, NULL, NULL, (u8*)&regs_global.vars.ap_connections_number, 0,"number of connections","ap_connections_number", U16_REGS_FLAG, 33, 194, 0x30061, 1, 3, 0 },//!< "number of connections" &ro
{ NULL, NULL, NULL, (u8*)&regs_global.vars.sta_connect, 0,"sta connect state","sta_connect", U16_REGS_FLAG, 34, 196, 0x30062, 1, 3, 0 },//!< "sta connect state" &ro
{ &def_table_version, NULL, NULL, (u8*)&regs_global.vars.table_version, 134,"change value in def_table_version for drop all regs to default value","table_version", U32_REGS_FLAG, 35, 198, 0x30063, 1, 7, 0 },//!< "change value in def_table_version for drop all regs to default value" &ro &def &save
{ NULL, NULL, NULL, (u8*)&regs_global.vars.iirls_pairing_status, 0,"status","iirls_pairing_status", U16_REGS_FLAG, 36, 202, 0x30065, 1, 3, 0 },//!< "status" &ro
{ NULL, NULL, NULL, (u8*)&regs_global.vars.iirls_paired_ip[0], 0,"ip address of paired device","iirls_paired_ip", U8_REGS_FLAG, 37, 204, 0x30066, 4, 3, 0 },//!< "ip address of paired device" &ro
{ NULL, NULL, NULL, (u8*)&regs_global.vars.modbus_master_test_value, 0,"status","modbus_master_test_value", U16_REGS_FLAG, 38, 208, 0x30068, 1, 3, 0 },//!< "status" &ro
{ NULL, NULL, NULL, (u8*)&regs_global.vars.iirls_state, 138,"state","iirls_state", U16_REGS_FLAG, 39, 210, 0x30069, 1, 5, 0 },//!< "state" &save
{ NULL, NULL, NULL, (u8*)&regs_global.vars.paired_device_model_number[0], 0,"state","paired_device_model_number", U16_REGS_FLAG, 40, 212, 0x3006a, 40, 3, 0 },//!< "state" &ro
{ &def_slave_modbus_address, &def_min_slave_modbus_address, &def_max_slave_modbus_address, (u8*)&regs_global.vars.slave_modbus_address, 140,"slave modbus address","slave_modbus_address", U16_REGS_FLAG, 41, 292, 0x30092, 1, 197, 0 },//!< "slave modbus address" &def &save &max &min
{ NULL, NULL, NULL, (u8*)&regs_global.vars.modbus_master_comm_period_current, 0,"modbus_master_comm_period_current","modbus_master_comm_period_current", U32_REGS_FLAG, 42, 294, 0x30093, 1, 1, 0 },//!< ""
{ NULL, NULL, NULL, (u8*)&regs_global.vars.modbus_master_comm_period_max, 0,"modbus_master_comm_period_max","modbus_master_comm_period_max", U32_REGS_FLAG, 43, 298, 0x30095, 1, 1, 0 },//!< ""
{ NULL, NULL, NULL, (u8*)&regs_global.vars.modbus_master_comm_period_min, 0,"modbus_master_comm_period_min","modbus_master_comm_period_min", U32_REGS_FLAG, 44, 302, 0x30097, 1, 1, 0 },//!< ""
{ NULL, NULL, NULL, (u8*)&regs_global.vars.modbus_master_succ_transactions_number, 0,"modbus_master_succ_transactions_number","modbus_master_succ_transactions_number", U32_REGS_FLAG, 45, 306, 0x30099, 1, 1, 0 },//!< ""
{ NULL, NULL, NULL, (u8*)&regs_global.vars.modbus_master_error_transactions_number, 0,"modbus_master_error_transactions_number","modbus_master_error_transactions_number", U32_REGS_FLAG, 46, 310, 0x3009b, 1, 1, 0 },//!< ""
{ &def_test_pwm_value, NULL, NULL, (u8*)&regs_global_part1.vars.test_pwm_value, 142,"test pwm value [0;100]","test_pwm_value", FLOAT_REGS_FLAG, 47, 314, 0x303e8, 1, 5, 1 },//!<"test pwm value [0;100]" &def &save
{ &def_test_pwm_value, NULL, NULL, (u8*)&regs_global_part1.vars.touch_0, 0,"touch_0","touch_0", U16_REGS_FLAG, 48, 318, 0x303ea, 1, 1, 1 },
{ &def_test_pwm_value, NULL, NULL, (u8*)&regs_global_part1.vars.touch_1, 0,"touch_1","touch_1", U16_REGS_FLAG, 49, 320, 0x303eb, 1, 1, 1 },
{ &def_test_pwm_value, NULL, NULL, (u8*)&regs_global_part1.vars.touch_2, 0,"touch_2","touch_2", U16_REGS_FLAG, 50, 322, 0x303ec, 1, 1, 1 },
{ &def_test_pwm_value, NULL, NULL, (u8*)&regs_global_part1.vars.touch_3, 0,"touch_3","touch_3", U16_REGS_FLAG, 51, 324, 0x303ed, 1, 1, 1 },
{ NULL, NULL, NULL, (u8*)&regs_global_part1.vars.water_counter, 146,"di counter","water_counter", U64_REGS_FLAG, 52, 326, 0x303ee, 1, 7, 1 },//!< "di counter" &save &ro
{ &def_impulse_cost, NULL, NULL, (u8*)&regs_global_part1.vars.impulse_cost, 154,"cost of di impulse in liters","impulse_cost", FLOAT_REGS_FLAG, 53, 334, 0x303f2, 1, 5, 1 },//!< "cost of di impulse in liters" &save &def
{ NULL, NULL, NULL, (u8*)&regs_global_part1.vars.liters, 158,"liters calculated","liters", FLOAT_REGS_FLAG, 54, 338, 0x303f4, 1, 5, 1 },//!< "liters calculated" &save
{ &def_touch_1_count, NULL, NULL, (u8*)&regs_global_part1.vars.touch_1_count, 162,"ms ","touch_1_count", U32_REGS_FLAG, 55, 342, 0x303f6, 1, 5, 1 },//!< "ms " &save &def
{ &def_touch_2_count, NULL, NULL, (u8*)&regs_global_part1.vars.touch_2_count, 166,"ms ","touch_2_count", U32_REGS_FLAG, 56, 346, 0x303f8, 1, 5, 1 },//!< "ms " &save &def
{ &def_touch_3_count, NULL, NULL, (u8*)&regs_global_part1.vars.touch_3_count, 170,"ms ","touch_3_count", U32_REGS_FLAG, 57, 350, 0x303fa, 1, 5, 1 },//!< "ms " &save &def
{ &def_touch_1_liters, NULL, NULL, (u8*)&regs_global_part1.vars.touch_1_liters, 174,"ms ","touch_1_liters", FLOAT_REGS_FLAG, 58, 354, 0x303fc, 1, 5, 1 },//!< "ms " &save &def
{ &def_touch_2_liters, NULL, NULL, (u8*)&regs_global_part1.vars.touch_2_liters, 178,"ms ","touch_2_liters", FLOAT_REGS_FLAG, 59, 358, 0x303fe, 1, 5, 1 },//!< "ms " &save &def
{ &def_touch_3_liters, NULL, NULL, (u8*)&regs_global_part1.vars.touch_3_liters, 182,"ms ","touch_3_liters", FLOAT_REGS_FLAG, 60, 362, 0x30400, 1, 5, 1 },//!< "ms " &save &def
{ &def_touch_0_trshld, NULL, NULL, (u8*)&regs_global_part1.vars.touch_0_trshld, 186,"in percents","touch_0_trshld", FLOAT_REGS_FLAG, 61, 366, 0x30402, 1, 5, 1 },//!< "in percents" &save &def
{ &def_touch_1_trshld, NULL, NULL, (u8*)&regs_global_part1.vars.touch_1_trshld, 190,"in percents","touch_1_trshld", FLOAT_REGS_FLAG, 62, 370, 0x30404, 1, 5, 1 },//!< "in percents" &save &def
{ &def_touch_2_trshld, NULL, NULL, (u8*)&regs_global_part1.vars.touch_2_trshld, 194,"in percents","touch_2_trshld", FLOAT_REGS_FLAG, 63, 374, 0x30406, 1, 5, 1 },//!< "in percents" &save &def
{ &def_touch_3_trshld, NULL, NULL, (u8*)&regs_global_part1.vars.touch_3_trshld, 198,"in percents","touch_3_trshld", FLOAT_REGS_FLAG, 64, 378, 0x30408, 1, 5, 1 },//!< "in percents" &save &def
{ NULL, NULL, NULL, (u8*)&regs_global_part1.vars.filter_use, 202,"use inside filter fot touch sensors or not ","filter_use", U32_REGS_FLAG, 65, 382, 0x3040a, 1, 5, 1 },//!< "use inside filter fot touch sensors or not " &save
{ &def_touch_handle_period, NULL, NULL, (u8*)&regs_global_part1.vars.touch_handle_period, 206,"in ms period of handle touchs","touch_handle_period", U32_REGS_FLAG, 66, 386, 0x3040c, 1, 5, 1 },//!< "in ms period of handle touchs" &save &def
{ &def_by_time, NULL, NULL, (u8*)&regs_global_part1.vars.by_time, 210,"by time or counter","by_time", U32_REGS_FLAG, 67, 390, 0x3040e, 1, 5, 1 },//!< "by time or counter" &save &def
{ &def_servo_0, &def_min_servo_0, &def_max_servo_0, (u8*)&servo_control_part.vars.servo_0, 0,"servo pwm value [0;100]","servo_0", FLOAT_REGS_FLAG, 68, 394, 0x307d0, 1, 193, 2 },//!<"servo pwm value [0;100]" &def &min &max
{ &def_servo_1, &def_min_servo_1, &def_max_servo_1, (u8*)&servo_control_part.vars.servo_1, 0,"servo pwm value [0;100]","servo_1", FLOAT_REGS_FLAG, 69, 398, 0x307d2, 1, 193, 2 },//!<"servo pwm value [0;100]" &def &min &max
{ &def_servo_2, &def_min_servo_2, &def_max_servo_2, (u8*)&servo_control_part.vars.servo_2, 0,"servo pwm value [0;100]","servo_2", FLOAT_REGS_FLAG, 70, 402, 0x307d4, 1, 193, 2 },//!<"servo pwm value [0;100]" &def &min &max
{ &def_servo_3, &def_min_servo_3, &def_max_servo_3, (u8*)&servo_control_part.vars.servo_3, 0,"servo pwm value [0;100]","servo_3", FLOAT_REGS_FLAG, 71, 406, 0x307d6, 1, 193, 2 },//!<"servo pwm value [0;100]" &def &min &max
};
regs_description_t const regs_description_client[NUM_OF_CLIENT_VARS]={
{ &def_mdb_addr, NULL, NULL, (u8*)&client_part_0.vars.mdb_addr, 214,"modbus address","mdb_addr", U16_REGS_FLAG, 72, 410, 0x30000, 1, 5, 3 },//!<"modbus address" &save &def
{ NULL, NULL, NULL, (u8*)&client_part_0.vars.ip[0], 216,"device ip address, warning!!! changes can lead to lost connection","ip", U8_REGS_FLAG, 73, 412, 0x30001, 4, 5, 3 },//!<"device ip address, warning!!! changes can lead to lost connection" &save
{ NULL, NULL, NULL, (u8*)&client_part_0.vars.netmask[0], 220,"netmask address for main wifi net","netmask", U8_REGS_FLAG, 74, 416, 0x30003, 4, 5, 3 },//!<"netmask address for main wifi net", &save
{ NULL, NULL, NULL, (u8*)&client_part_0.vars.gate[0], 224,"gateaway address, warning!!! changes can lead to lost connection ","gate", U8_REGS_FLAG, 75, 420, 0x30005, 4, 5, 3 },//!<"gateaway address, warning!!! changes can lead to lost connection " &save
{ NULL, NULL, NULL, (u8*)&client_part_0.vars.slip_ip[0], 228,"ip address for local net","slip_ip", U8_REGS_FLAG, 76, 424, 0x30007, 4, 5, 3 },//!<"ip address for local net",&save ,
{ NULL, NULL, NULL, (u8*)&client_part_0.vars.slip_netmask[0], 232,"netmask address for local net","slip_netmask", U8_REGS_FLAG, 77, 428, 0x30009, 4, 5, 3 },//!<"netmask address for local net", &save ,
{ NULL, NULL, NULL, (u8*)&client_part_0.vars.slip_gate[0], 236,"gateaway address for local net","slip_gate", U8_REGS_FLAG, 78, 432, 0x3000b, 4, 5, 3 },//!<"gateaway address for local net", &save,
};
static const regs_description_t * regs_description_user =NULL;


/**
 * @brief regs_description_get_by_name gets full description by name
 * @param regs_template
 * @return
 */
int regs_description_get_by_name(regs_template_t * regs_template){
    int result;
    u32 name_size;
    u16 i;
    result = -1;
    name_size = strlen(regs_template->name);
    if(name_size > REGS_MAX_NAME_SIZE){
       name_size = REGS_MAX_NAME_SIZE;
    }
    const regs_description_t * description_temp =NULL;
    for (i=0 ; i<regs_global.vars.num_of_vars; i++){
        if (memcmp(regs_template->name, regs_description[i].name, name_size) == 0){
            description_temp = &regs_description[i];
            result = 0;
            break;
        }
    }
    if((description_temp != NULL) && (result==0)){
        memcpy(regs_template,description_temp,sizeof(regs_description_t));
        regs_template->size_in_bytes = regs_template->size * regs_size_in_byte(regs_template->type);
    }

    return result;
}
/**
 * @brief regs_description_get_by_ind get full description by ind
 * @param regs_template
 * @return
 */
int regs_description_get_by_ind(regs_template_t * regs_template){
    int result = 0;
    regs_global.vars.num_of_vars = (regs_global.vars.num_of_vars<NUM_OF_SELF_VARS)?NUM_OF_SELF_VARS:regs_global.vars.num_of_vars;
    if(regs_template->ind < regs_global.vars.num_of_vars){
        const regs_description_t * description_temp =NULL;
        description_temp = &regs_description[regs_template->ind];
        if(description_temp != NULL){
            memcpy(regs_template,description_temp,sizeof(regs_description_t));
            regs_template->size_in_bytes = regs_template->size * regs_size_in_byte(regs_template->type);
        }
    }else{
        result = -1;
    }
    return result;
}
/**
 * @brief regs_description_get_by_guid get full description by guid
 * @param regs_template
 * @return zero value if have found description
 */
int regs_description_get_by_guid(regs_template_t * regs_template){
    int result = -1;
    u16 i;
    const regs_description_t * description_temp =NULL;
    regs_global.vars.num_of_vars = (regs_global.vars.num_of_vars<NUM_OF_SELF_VARS)?NUM_OF_SELF_VARS:regs_global.vars.num_of_vars;
    for (i=0 ; i<regs_global.vars.num_of_vars ; i++){
        if (regs_template->guid == regs_description[i].guid){
            description_temp = &regs_description[i];
            result = 0;
            break;
        }
    }
    if((description_temp != NULL) && (result==0)){
        memcpy(regs_template,description_temp,sizeof(regs_description_t));
        regs_template->size_in_bytes = regs_template->size * regs_size_in_byte(regs_template->type);
    }
    return result;
}
/**
 * @brief regs_description_write_value write value by info used in regs_template_to_write
 * @param regs_template_to_write presetted struct
 * @param value pointer to value for write
 * @return zero value if writed
 */
int regs_description_write_value(regs_template_t * regs_template_to_write, const u8 * value){
    int res = 0;
    semaphore_take(regs_access_mutex, portMAX_DELAY );{
        if ((regs_template_to_write->p_value!=NULL)&&(value!=NULL)){
            u32 size_in_byte = regs_size_in_byte(regs_template_to_write->type);
            size_in_byte *= regs_template_to_write->size;
            memcpy(regs_template_to_write->p_value,value,size_in_byte);
            if (regs_template_to_write->property&SAVING){
                /*!< save to mirror @todo add handl for flash saved vars*/
                if ((regs_template_to_write->saved_address+size_in_byte)<=INTERNAL_FLASH_MIRROR_ITEM_SIZE){
                    mirror_access_write(regs_template_to_write);
                }
            }
        }else{
            res = -1;
        }
    }semaphore_release(regs_access_mutex);
    return res;
}

/**
 * @brief regs_description_add_user_vars udd users description space
 * @param user_description
 * @param num_of_user_vars
 * @return
 */
int regs_description_add_user_vars(const regs_description_t * user_description, u16 num_of_user_vars){
    int result = 0;
    regs_global.vars.num_of_vars = (regs_global.vars.num_of_vars<NUM_OF_SELF_VARS)?NUM_OF_SELF_VARS:regs_global.vars.num_of_vars;
    if ((num_of_user_vars+regs_global.vars.num_of_vars) < MAX_DESCRIPTIONS_REGS){
        regs_global.vars.num_of_vars = NUM_OF_SELF_VARS + num_of_user_vars;
        regs_description_user = user_description;
    }else{
        result = -1;
    }
    return result;
}
/**
 * @brief reg_is_writeable
 * @param reg_index
 * @return 0 if RO, and 1 if writeable
 * @ingroup regs
  * @todo check neccessary of this function
 */
u8 regs_description_is_writable (u16 reg_index){
    u8 result = 0;
    if (reg_index < NUM_OF_SELF_VARS){
        if(regs_description[reg_index].property & READ_ONLY){
            result = 0;
        }else{
            result = 1;
        }
    }
    return result;
}
/**
 * @brief regs_description_is_credential
 * @param reg_index
 * @return 1 if credential
 * @ingroup regs
  * @todo check neccessary of this function
 */
u8 regs_description_is_credential(u16 reg_index){
    u8 result = 0;
    if (reg_index < NUM_OF_SELF_VARS){
        if(regs_description[reg_index].property & CREDENTIAL_FLAG){
            result = 1;
        }else{
            result = 0;
        }
    }
    return result;
}
/**
 * @brief regs_description_is_saved
 * @param reg_index
 * @return 1 if saving
 * @ingroup regs
  * @todo check neccessary of this function
 */
u8 regs_description_is_saved(u16 reg_index){
    u8 result = 0;
    if (reg_index < NUM_OF_SELF_VARS){
        if(regs_description[reg_index].property & SAVING){
            result = 1;
        }else{
            result = 0;
        }
    }
    return result;
}
/**
 * @brief regs_description_flag_check
 * @param index
 * @param flag
 * @return  1 if flag exist
 */
u8 regs_description_flag_check (u16 index, u8 flag){
    u8 result = 0;
    if (index < NUM_OF_SELF_VARS){
        if(regs_description[index].property & flag){
            result = 1;
        }else{
            result = 0;
        }
    }
    return result;

}

/**
 * @brief regs_get_index_by_byte_address return index of regs by byte address
 * @param byte_address - byte number
 * @return
 */
int regs_description_get_index_by_guid(u32 byte_address){
    int result = -1;
    for (int i = 0;i<NUM_OF_SELF_VARS;i++){
        if ((byte_address>= regs_description[i].guid) &&
            (byte_address < (regs_description[i].guid+regs_size_in_byte(regs_description[i].type)*regs_description[i].size))){
            result = i;
            break;
        }
    }
    return result;
}

/**
 * @brief regs_get_index_by_byte_address return index of regs by byte address
 * @param byte_address - byte number
 * @return
 */
int regs_description_get_index_by_address(const void * address){
    int result = -1;
    for (int i = 0;i<NUM_OF_SELF_VARS;i++){
        if ((address >= (void*)regs_description[i].p_value) &&
            (address < (void*)(regs_description[i].p_value+regs_size_in_byte(regs_description[i].type)*regs_description[i].size))){
            result = i;
            break;
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
    for (int i = 0;i<NUM_OF_SELF_VARS;i++){
        u8 reg_mdb_function = RD_MDB_FUNCTION(regs_description[i].modbus_description);
        u16 reg_mdb_address = RD_MDB_ADDRESS(regs_description[i].modbus_description);
        u16 reg_size_in_word = (regs_size_in_byte(regs_description[i].type)*regs_description[i].size)/2;
        if (modbus_function == reg_mdb_function){
            if (modbus_address == reg_mdb_address){
                p_value = regs_description[i].p_value;
                break;
            }else if((modbus_address > reg_mdb_address) &&
                     (modbus_address < (reg_mdb_address + reg_size_in_word))){
                p_value = regs_description[i].p_value + (modbus_address - reg_mdb_address) * 2;
                break;
            }
        }
    }
    return p_value;
}
/**
 * @brief set_regs_def_values
 * @return 1 if self vars was changed
 */
int set_regs_def_values (void){
    int res = 0;
    regs_template_t reg_template = {0};
    for (u16 i=0; i<NUM_OF_SELF_VARS; i++){
        reg_template.ind = i;
        if(regs_description_get_by_ind(&reg_template)==0){
            if(reg_template.p_default == NULL){
                memset(reg_template.p_value,0,reg_template.size_in_bytes);
            }else{
                memcpy(reg_template.p_value,reg_template.p_default,reg_template.size_in_bytes);
            }
        }
    }
    return res;
}
/**
 * @brief return ind of savig address of register in bkram
 * @param reg_end_address - pointer to register
 * @return  ind of savig bkram address, \n
 *          -1 - reg description not find
 * @ingroup regs
 */
int end_of_saved_reg_addr(void * reg_end_address){
    u16 i;
    int result = -1;
    regs_template_t regs_template;
    for (i=0 ; i<regs_global.vars.num_of_vars; i++){
        regs_template.ind = i;
        regs_description_get_by_ind(&regs_template);
        if ((regs_template.property & SAVING) &&
            (reg_end_address == (regs_template.p_value + regs_template.size_in_bytes))){
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
    regs_template_t reg_template = {0};
    reg_template.ind = (u16)index;
    if (regs_description_get_by_ind(&reg_template)>=0){
        u8 reg_write_size = regs_size_in_byte(reg.flag);
        u32 shift = (u32)reg_address - (u32)reg_template.p_value;
        u8 regs_item_size = regs_size_in_byte(reg_template.type);
        u8 start_byte = shift % regs_item_size;/*0-7*/
        if ((u32)temp_data_buffering->reg_address != ((u32)reg_address - start_byte)){
            temp_data_buffering->reg_address = (void*)((u32)reg_address - start_byte);
            temp_data_buffering->byte_writed_flags = 0;
            temp_data_buffering->type = reg_template.type;
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
 * @return positive value if register is full filling
 */
int regs_check_temp_buffer(temp_data_buffering_t * temp_data_buffering, int index){
    int res = 0;
    regs_template_t reg_template = {0};
    reg_template.ind = (u16)index;
    if (regs_description_get_by_ind(&reg_template)>=0){
        u8 regs_item_size = regs_size_in_byte(reg_template.type);
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
