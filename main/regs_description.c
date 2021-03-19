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
#include "bkram_access.h"
#include "internal_flash.h"
#include <string.h> /* memset */
#include <stdlib.h> /* atoi */
#include <stdio.h>

static const u16 def_mdb_addr = 3;
static const u8 def_mdb_revers = 0;
static const u8 def_mdb_shift = 0;
static const u8 def_ip[4] = {192,168,1,232};
static const u8 def_netmask[4] = {255,255,255,0};
static const u8 def_gateaway[4] = {192,168,1,1};
static const u8 def_eth_speed = 0;
static const u8 def_eth_duplex = 0;
static const u16 def_uart_debug_sets = 0xC853;
static const u8 def_os_version[OS_VERSION_SIZE] = OS_VERSION;
static const u16 def_num_of_vars = NUM_OF_SELF_VARS;
static const u8 def_board_ver = (u8)BOARD_VERSION;
static const u32 def_pass_key = 0x879ABCA6;
static const int def_pwm_value = 100;
static const int def_tmr_t = 1000;
static const int def_frequency_sp = 4000;
static const u8 def_name[32] = {'a','b','c','d','e','f','g'};
static const u8 def_device_name[32] = {'i','s','i','m','f','w','4','0','0'};
static const u16 def_model_number = 0;
static const u16 def_device_type = 0;
static const u16 def_permission = ENABLE_HTTP_FLAG;

regs_description_t const regs_description[NUM_OF_SELF_VARS]={
{&def_mdb_addr,(u8*)&regs_global.vars.mdb_addr,16,"modbus address, main register for modbus polling (1 to 247)","mdb_addr",U16_REGS_FLAG,0,0,1,5},//!<"modbus address, main register for modbus polling (1 to 247)" &save &def
{&def_mdb_revers,(u8*)&regs_global.vars.mdb_revers,18,"reverse 3 and 4 function(unput and holding reg) (0 or 1)","mdb_revers",U8_REGS_FLAG,1,2,1,5},//!<"reverse 3 and 4 function(unput and holding reg) (0 or 1)" &save &def
{&def_mdb_shift,(u8*)&regs_global.vars.mdb_shift,19,"shift start address regs (0 or 1)","mdb_shift",U8_REGS_FLAG,2,3,1,5},//!<"shift start address regs (0 or 1)" &save &def
{&def_ip,(u8*)&regs_global.vars.ip[0],20,"device ip address, warning!!! changes can lead to lost connection","ip",U8_REGS_FLAG,3,4,4,5},//!<"device ip address, warning!!! changes can lead to lost connection" &save &def
{&def_netmask,(u8*)&regs_global.vars.netmask[0],24,"netmask address, warning!!! changes can lead to lost connection","netmask",U8_REGS_FLAG,4,8,4,5},//!<"netmask address, warning!!! changes can lead to lost connection" &save &def
{&def_gateaway,(u8*)&regs_global.vars.gateaway[0],28,"gateaway address, warning!!! changes can lead to lost connection ","gateaway",U8_REGS_FLAG,5,12,4,5},//!<"gateaway address, warning!!! changes can lead to lost connection " &save &def
{&def_eth_speed,(u8*)&regs_global.vars.eth_speed,32,"reserved for speed10-100mb, not available(0 - auto 10 - 10 100 - 100)","eth_speed",U8_REGS_FLAG,6,16,1,5},//!<"reserved for speed10-100mb, not available(0 - auto 10 - 10 100 - 100)" &save &def
{&def_eth_duplex,(u8*)&regs_global.vars.eth_duplex,33,"reserved for duplex(0 - auto 1 - HALF 2 - FULL)","eth_duplex",U8_REGS_FLAG,7,17,1,5},//!<"reserved for duplex(0 - auto 1 - HALF 2 - FULL)" &save &def
{NULL,(u8*)&regs_global.vars.command,0,"command register","command",U16_REGS_FLAG,8,18,1,1},//!<"command register"
{NULL,(u8*)&regs_global.vars.unix_time_sec,0,"unix time since the Epoch (00:00:00 UTC, January 1, 1970)","unix_time_sec",S32_REGS_FLAG,9,20,1,1},//!<"unix time since the Epoch (00:00:00 UTC, January 1, 1970)"
{NULL,(u8*)&regs_global.vars.time_hms,0,"struct for real time(0-hour,1-min,2-sec,3-sub_sec,4-week_day,6-month,7-date[1;31],8-year,9-year_day)","time_hms",U8_REGS_FLAG,10,24,10,1},//!<"struct for real time(0-hour,1-min,2-sec,3-sub_sec,4-week_day,6-month,7-date[1;31],8-year,9-year_day)"
{&def_uart_debug_sets,(u8*)&regs_global.vars.uart_debug_sets,34,"settings debug uart speed,parity and stop bits, default 115200 ,parity - none, 1 stop bit","uart_debug_sets",U16_REGS_FLAG,11,34,1,5},//!<"settings debug uart speed,parity and stop bits, default 115200 ,parity - none, 1 stop bit" &save &def
{NULL,(u8*)&regs_global.vars.main_test_blocks,0,"use for testing blocks","main_test_blocks",U32_REGS_FLAG,12,36,1,1},//!<"use for testing blocks"     Blocks[0...28] = {see main_test_block_t},CheckTest[29],StressTest[30],PerfTest[31];
{NULL,(u8*)&regs_global.vars.isimfw_enable,0,"isimfw400 on-off lamp","isimfw_enable",U16_REGS_FLAG,13,40,1,1},//!<"isimfw400 on-off lamp" when value is 0 parameters ac_voltage_set,dc_voltage_set,ac_current_set,dc_current_set don't affect on work
{&def_frequency_sp,(u8*)&regs_global.vars.frequency_sp,36,"isimfw400 freq sp [2000:10000](TIM4 PB6 PB7)","frequency_sp",S32_REGS_FLAG,14,42,1,5},//!<"isimfw400 freq sp [2000:10000](TIM4 PB6 PB7)" &def &save
{NULL,(u8*)&regs_global.vars.pause_sp,0,"isimfw400 value in microseconds, the maximum value is limited to the specified frequency","pause_sp",S32_REGS_FLAG,15,46,1,1},//!<"isimfw400 value in microseconds, the maximum value is limited to the specified frequency"
{NULL,(u8*)&regs_global.vars.ac_voltage_set,0,"isimfw400 setting ac volatge *10, ex.max value 45000 -> 4500 volts","ac_voltage_set",FLOAT_REGS_FLAG,16,50,1,1},//!<"isimfw400 setting ac volatge *10, ex.max value 45000 -> 4500 volts"
{NULL,(u8*)&regs_global.vars.dc_voltage_set,0,"isimfw400 setting dc volatge *10, ex.max value 30000 -> 3000 volts","dc_voltage_set",FLOAT_REGS_FLAG,17,54,1,1},//!<"isimfw400 setting dc volatge *10, ex.max value 30000 -> 3000 volts"
{NULL,(u8*)&regs_global.vars.ac_current_set,0,"isimfw400 setting ac current *1000, ex.max value 6000 -> 6ma","ac_current_set",FLOAT_REGS_FLAG,18,58,1,1},//!<"isimfw400 setting ac current *1000, ex.max value 6000 -> 6ma"
{NULL,(u8*)&regs_global.vars.dc_current_set,0,"isimfw400 setting dc current *1000, ex.max value 6000 -> 6ma","dc_current_set",FLOAT_REGS_FLAG,19,62,1,1},//!<"isimfw400 setting dc current *1000, ex.max value 6000 -> 6ma"
{&def_name,(u8*)&regs_global.vars.name[0],40,"additional name,for users","name",U8_REGS_FLAG,20,66,32,5},//!<"additional name,for users" &def &save
{NULL,(u8*)&regs_global.vars.reserv[0],0,"reserv","reserv",U16_REGS_FLAG,21,98,100,1},//!< reserved for writable registers
{&def_pwm_value,(u8*)&regs_global.vars.pwm_value,0,"isimfw400 pwm value default-100(TIM4 PB6 PB7)","pwm_value",S32_REGS_FLAG,22,298,1,3},//!<"isimfw400 pwm value default-100(TIM4 PB6 PB7)" &def &ro
{&def_tmr_t,(u8*)&regs_global.vars.tmr_t,0,"isimfw400 timer default-1000(TIM4 PB6 PB7)","tmr_t",S32_REGS_FLAG,23,302,1,3},//!<"isimfw400 timer default-1000(TIM4 PB6 PB7)" &def &ro
{NULL,(u8*)&regs_global.vars.dc1_pwm,0,"isimfw400 main PID outputs 1","dc1_pwm",S32_REGS_FLAG,24,306,1,3},//!<"isimfw400 main PID outputs 1" &ro
{NULL,(u8*)&regs_global.vars.dc2_pwm,0,"isimfw400 main PID outputs 2","dc2_pwm",S32_REGS_FLAG,25,310,1,3},//!<"isimfw400 main PID outputs 2" &ro
{NULL,(u8*)&regs_global.vars.pwm_limit,0,"isimfw400 max pwm value","pwm_limit",S32_REGS_FLAG,26,314,1,3},//!<"isimfw400 max pwm value" &ro
{NULL,(u8*)&regs_global.vars.enc_flag1,0,"isimfw400 reference to GPIOE-15,14","enc_flag1",U16_REGS_FLAG,27,318,1,3},//!<"isimfw400 reference to GPIOE-15,14" &ro
{NULL,(u8*)&regs_global.vars.enc_flag2,0,"isimfw400 reference to GPIOB-2,1","enc_flag2",U16_REGS_FLAG,28,320,1,3},//!<"isimfw400 reference to GPIOB-2,1" &ro
{NULL,(u8*)&regs_global.vars.enc_flag3,0,"isimfw400 reference to GPIOC-6,8","enc_flag3",U16_REGS_FLAG,29,322,1,3},//!<"isimfw400 reference to GPIOC-6,8" &ro
{NULL,(u8*)&regs_global.vars.enc_flag4,0,"isimfw400 reference to GPIOB-4,5","enc_flag4",U16_REGS_FLAG,30,324,1,3},//!<"isimfw400 reference to GPIOB-4,5" &ro
{NULL,(u8*)&regs_global.vars.enc_flag5,0,"isimfw400 reference to GPIOE-12,13","enc_flag5",U16_REGS_FLAG,31,326,1,3},//!<"isimfw400 reference to GPIOE-12,13" &ro
{NULL,(u8*)&regs_global.vars.ac_current_err,0,"isimfw400 err ac current *1000, ex. 6000 -> 6ma","ac_current_err",FLOAT_REGS_FLAG,32,328,1,3},//!<"isimfw400 err ac current *1000, ex. 6000 -> 6ma" &ro
{NULL,(u8*)&regs_global.vars.ac_current_integr,0,"isimfw400 integr ac current *1000, ex. 6000 -> 6ma","ac_current_integr",FLOAT_REGS_FLAG,33,332,1,3},//!<"isimfw400 integr ac current *1000, ex. 6000 -> 6ma" &ro
{NULL,(u8*)&regs_global.vars.dc_current_err,0,"isimfw400 err dc current *1000, ex. 6000 -> 6ma","dc_current_err",FLOAT_REGS_FLAG,34,336,1,3},//!<"isimfw400 err dc current *1000, ex. 6000 -> 6ma" &ro
{NULL,(u8*)&regs_global.vars.dc_current_integr,0,"isimfw400 integrl err dc current *1000, ex. 6000 -> 6ma","dc_current_integr",FLOAT_REGS_FLAG,35,340,1,3},//!<"isimfw400 integrl err dc current *1000, ex. 6000 -> 6ma" &ro
{NULL,(u8*)&regs_global.vars.dc_voltage_err,0,"isimfw400 err dc voltage *10, ex. 30000 -> 3000 volts","dc_voltage_err",FLOAT_REGS_FLAG,36,344,1,3},//!<"isimfw400 err dc voltage *10, ex. 30000 -> 3000 volts" &ro
{NULL,(u8*)&regs_global.vars.dc_voltage_integr,0,"isimfw400 integr err dc voltage *10, ex. 30000 -> 3000 volts","dc_voltage_integr",FLOAT_REGS_FLAG,37,348,1,3},//!<"isimfw400 integr err dc voltage *10, ex. 30000 -> 3000 volts" &ro
{NULL,(u8*)&regs_global.vars.ac_voltage_err,0,"isimfw400 err ac voltage *10, ex. 45000 -> 4500 volts ","ac_voltage_err",FLOAT_REGS_FLAG,38,352,1,3},//!<"isimfw400 err ac voltage *10, ex. 45000 -> 4500 volts " &ro
{NULL,(u8*)&regs_global.vars.ac_voltage_integr,0,"isimfw400 integr err ac voltage *10, ex. 45000 -> 4500 volts ","ac_voltage_integr",FLOAT_REGS_FLAG,39,356,1,3},//!<"isimfw400 integr err ac voltage *10, ex. 45000 -> 4500 volts " &ro
{NULL,(u8*)&regs_global.vars.ac_voltage_fact,0,"isimfw400 real ac voltage *10, ex. 45000 -> 4500 volts","ac_voltage_fact",FLOAT_REGS_FLAG,40,360,1,3},//!<"isimfw400 real ac voltage *10, ex. 45000 -> 4500 volts" &ro
{NULL,(u8*)&regs_global.vars.dc_voltage_fact,0,"isimfw400 real dc voltage *10, ex. 30000 -> 3000 volts","dc_voltage_fact",FLOAT_REGS_FLAG,41,364,1,3},//!<"isimfw400 real dc voltage *10, ex. 30000 -> 3000 volts" &ro
{NULL,(u8*)&regs_global.vars.ac_current_fact,0,"isimfw400 real ac current *1000, ex. 6000 -> 6ma","ac_current_fact",FLOAT_REGS_FLAG,42,368,1,3},//!<"isimfw400 real ac current *1000, ex. 6000 -> 6ma" &ro
{NULL,(u8*)&regs_global.vars.dc_current_fact,0,"isimfw400 real dc current *1000, ex. 6000 -> 6ma","dc_current_fact",FLOAT_REGS_FLAG,43,372,1,3},//!<"isimfw400 real dc current *1000, ex. 6000 -> 6ma" &ro
{NULL,(u8*)&regs_global.vars.adc1,0,"adc1 value in units [0:65535]","adc1",U16_REGS_FLAG,44,376,1,3},//!< "adc1 value in units [0:65535]" &ro
{NULL,(u8*)&regs_global.vars.adc2,0,"adc2 value in units [0:65535]","adc2",U16_REGS_FLAG,45,378,1,3},//!< "adc2 value in units [0:65535]" &ro
{NULL,(u8*)&regs_global.vars.adc3,0,"adc3 value in units [0:65535]","adc3",U16_REGS_FLAG,46,380,1,3},//!< "adc3 value in units [0:65535]" &ro
{NULL,(u8*)&regs_global.vars.adc4,0,"adc4 value in units [0:65535]","adc4",U16_REGS_FLAG,47,382,1,3},//!< "adc4 value in units [0:65535]" &ro
{NULL,(u8*)&regs_global.vars.adc5,0,"adc5 value in units [0:65535]","adc5",U16_REGS_FLAG,48,384,1,3},//!< "adc5 value in units [0:65535]" &ro
{NULL,(u8*)&regs_global.vars.device_state,0,"BIT(0) - interlock","device_state",U16_REGS_FLAG,49,386,1,3},//!< "BIT(0) - interlock" &ro
{NULL,(u8*)&regs_global.vars.unused_3,0,"unused_3","unused_3",U16_REGS_FLAG,50,388,1,3},//!<"" &ro
{NULL,(u8*)&regs_global.vars.last_reset,0,"reason of last system reset","last_reset",U16_REGS_FLAG,51,390,1,3},//!<"reason of last system reset",&ro, See rst_reason_t
{NULL,(u8*)&regs_global.vars.sys_tick_counter,0,"tick in ms","sys_tick_counter",U64_REGS_FLAG,52,392,1,3},//!<"tick in ms",&ro
{NULL,(u8*)&regs_global.vars.tick100us,0,"tick counter in 100us time","tick100us",U64_REGS_FLAG,53,400,1,3},//!<"tick counter in 100us time",&ro
{NULL,(u8*)&regs_global.vars.mac_addr[0],0,"ethernet mac address","mac_addr",U8_REGS_FLAG,54,408,6,3},//!<"ethernet mac address",&ro
{NULL,(u8*)&regs_global.vars.isol_pwr_state,0,"isolated power state","isol_pwr_state",U16_REGS_FLAG,55,414,1,3},//!<"isolated power state",&ro
{NULL,(u8*)&regs_global.vars.flags_task,0,"check for task created","flags_task",U32_REGS_FLAG,56,416,1,3},//!<"check for task created" &ro
{NULL,(u8*)&regs_global.vars.cnt_kernel,0,"kernel task count","cnt_kernel",U32_REGS_FLAG,57,420,1,3},//!<"kernel task count"&ro
{NULL,(u8*)&regs_global.vars.cnt_init,0,"init task count","cnt_init",U32_REGS_FLAG,58,424,1,3},//!<"init task count"&ro
{NULL,(u8*)&regs_global.vars.cnt_eth_input,0,"ethernet task count","cnt_eth_input",U32_REGS_FLAG,59,428,1,3},//!<"ethernet task count"&ro
{NULL,(u8*)&regs_global.vars.cnt_packet,0,"packet task count","cnt_packet",U32_REGS_FLAG,60,432,1,3},//!<"packet task count"&ro
{NULL,(u8*)&regs_global.vars.async_flags,0,"inited modules","async_flags",U32_REGS_FLAG,61,436,1,3},//!<"inited modules" &ro for init in task
{NULL,(u8*)&regs_global.vars.flags_succ_init,0,"success inited modules","flags_succ_init",U32_REGS_FLAG,62,440,1,3},//!<"success inited modules" &ro for init in task
{NULL,(u8*)&regs_global.vars.main_test_result,0,"test blocks results","main_test_result",U32_REGS_FLAG,63,444,1,3},//!<"test blocks results",&ro Results[0...31] = {see main_test_block_t, 1-OK, 0-failed};
{NULL,(u8*)&regs_global.vars.run_test,0,"running tests","run_test",U32_REGS_FLAG,64,448,1,3},//!<"running tests",&ro
{NULL,(u8*)&regs_global.vars.cur_free_heap,0,"in bytes","cur_free_heap",U32_REGS_FLAG,65,452,1,3},//!<"in bytes",&ro
{NULL,(u8*)&regs_global.vars.min_free_heap,0,"in bytes","min_free_heap",U32_REGS_FLAG,66,456,1,3},//!<"in bytes",&ro
{NULL,(u8*)&regs_global.vars.eth_reinit_cnt,0,"counter for ethernet_reinit","eth_reinit_cnt",U32_REGS_FLAG,67,460,1,3},//!<"counter for ethernet_reinit",&ro
{NULL,(u8*)&regs_global.vars.debug_info[0],0,"reserved use for debug","debug_info",U8_REGS_FLAG,68,464,8,3},//!<"reserved use for debug"&ro
{&def_num_of_vars,(u8*)&regs_global.vars.num_of_vars,0,"num_of_vars","num_of_vars",U16_REGS_FLAG,69,472,1,3},//!<"number of vars self + config(user)&ro &def
{NULL,(u8*)&regs_global.vars.current_os,0,"using os 1 or 2","current_os",U16_REGS_FLAG,70,474,1,3},//!<"using os 1 or 2"  &ro
{NULL,(u8*)&regs_global.vars.temperature_mcu,0,"temperature mcu Celsius","temperature_mcu",FLOAT_REGS_FLAG,71,476,1,3},//!<"temperature mcu Celsius" &ro
{NULL,(u8*)&regs_global.vars.temperature_out,0,"temperature on DS18b20","temperature_out",FLOAT_REGS_FLAG,72,480,1,3},//!<"temperature on DS18b20" &ro
{&def_os_version,(u8*)&regs_global.vars.os_version[0],0,"version like 0.1.1.0","os_version",U8_REGS_FLAG,73,484,4,3},//!<"version like 0.1.1.0",&ro,&def
{NULL,(u8*)&regs_global.vars.uniq_id[0],0,"uniq_id number","uniq_id",U8_REGS_FLAG,74,488,12,3},//!< "uniq_id number" , &ro
{&def_device_type,(u8*)&regs_global.vars.device_type,0,"type of device","device_type",U16_REGS_FLAG,75,500,1,3},//!< "type of device", &ro, &def
{&def_board_ver,(u8*)&regs_global.vars.board_ver,0,"board version","board_ver",U16_REGS_FLAG,76,502,1,3},//!< "board version", &ro, &def
{&def_device_name,(u8*)&regs_global.vars.device_name[0],0,"special device name prebuilded","device_name",U8_REGS_FLAG,77,504,32,19},//!< "special device name prebuilded" &def &ro &credential
{&def_model_number,(u8*)&regs_global.vars.model_number,0,"special model number prebuilded","model_number",U16_REGS_FLAG,78,536,1,19},//!< "special model number prebuilded" &def &ro &credential
{NULL,(u8*)&regs_global.vars.serial_number,0,"uniq serial number, writed only once","serial_number",U32_REGS_FLAG,79,538,1,19},//!<"uniq serial number, writed only once" &ro &credential
{NULL,(u8*)&regs_global.vars.gid,0,"GID","gid",U32_REGS_FLAG,80,542,1,19},//!<"GID" &ro &credential
{NULL,(u8*)&regs_global.vars.os_hash,0,"hash os","os_hash",U32_REGS_FLAG,81,546,1,3},//!< "hash os" &ro
{&def_pass_key,(u8*)&regs_global.vars.pass_key,0,"key for registers change","pass_key",U32_REGS_FLAG,82,550,1,3},//!< "key for registers change", &def, &ro
{NULL,(u8*)&regs_global.vars.phy1_bcr,0,"phy1_bcr","phy1_bcr",U16_REGS_FLAG,83,554,1,3},//!< debug only &ro
{NULL,(u8*)&regs_global.vars.phy2_bcr,0,"phy2_bcr","phy2_bcr",U16_REGS_FLAG,84,556,1,3},//!< debug only &ro
{NULL,(u8*)&regs_global.vars.phy1_isfr,0,"phy1_isfr","phy1_isfr",U16_REGS_FLAG,85,558,1,3},//!< debug only &ro
{NULL,(u8*)&regs_global.vars.phy2_isfr,0,"phy2_isfr","phy2_isfr",U16_REGS_FLAG,86,560,1,3},//!< debug only &ro
{NULL,(u8*)&regs_global.vars.phy1_scsr,0,"phy1_scsr","phy1_scsr",U16_REGS_FLAG,87,562,1,3},//!< debug only &ro
{NULL,(u8*)&regs_global.vars.phy2_scsr,0,"phy2_scsr","phy2_scsr",U16_REGS_FLAG,88,564,1,3},//!< debug only &ro
{NULL,(u8*)&regs_global.vars.phy1_id,0,"phy1_id","phy1_id",U32_REGS_FLAG,89,566,1,3},//!< debug only &ro
{NULL,(u8*)&regs_global.vars.phy2_id,0,"phy2_id","phy2_id",U32_REGS_FLAG,90,570,1,3},//!< debug only &ro
{NULL,(u8*)&regs_global.vars.ph1_link_status,0,"BIT(2)-Link is up,BIT(3) - AN capable,BIT(4) - FAR end Fault,BIT(5) - AN complete","ph1_link_status",U16_REGS_FLAG,91,574,1,3},//!<"BIT(2)-Link is up,BIT(3) - AN capable,BIT(4) - FAR end Fault,BIT(5) - AN complete" &ro
{NULL,(u8*)&regs_global.vars.ph2_link_status,0,"BIT(2)-Link is up,BIT(3) - AN capable,BIT(4) - FAR end Fault,BIT(5) - AN complete","ph2_link_status",U16_REGS_FLAG,92,576,1,3},//!<"BIT(2)-Link is up,BIT(3) - AN capable,BIT(4) - FAR end Fault,BIT(5) - AN complete" &ro
{NULL,(u8*)&regs_global.vars.ph1_link_partner,0,"BIT(5)-10Half,BIT(6)-10Full,BIT(7)-100Half,BIT(8)-100Full","ph1_link_partner",U16_REGS_FLAG,93,578,1,3},//!<"BIT(5)-10Half,BIT(6)-10Full,BIT(7)-100Half,BIT(8)-100Full" &ro
{NULL,(u8*)&regs_global.vars.ph2_link_partner,0,"BIT(5)-10Half,BIT(6)-10Full,BIT(7)-100Half,BIT(8)-100Full","ph2_link_partner",U16_REGS_FLAG,94,580,1,3},//!<"BIT(5)-10Half,BIT(6)-10Full,BIT(7)-100Half,BIT(8)-100Full" &ro
{NULL,(u8*)&regs_global.vars.system_error,0,"system global error","system_error",U32_REGS_FLAG,95,582,1,3},//!< "system global error" &ro
{NULL,(u8*)&regs_global.vars.monitor_period,0,"monitor period in ms","monitor_period",U32_REGS_FLAG,96,586,1,3},//!<"monitor period in ms" &ro
{NULL,(u8*)&regs_global.vars.total_tasks_time,0,"total_tasks_time","total_tasks_time",FLOAT_REGS_FLAG,97,590,1,3},//!<"sum of running times of tasks in %" &ro
{&def_permission,(u8*)&regs_global.vars.permission,0,"flags with permissions","permission",U16_REGS_FLAG,98,594,1,3},//!<"flags with permissions" &ro &def
{NULL,(u8*)&regs_global.vars.isimfw400_task_stack,0,"main control isimfw400 - PID, control PWM ,free stack in bytes","isimfw400_task_stack",U32_REGS_FLAG,99,596,1,3},//!<"main control isimfw400 - PID, control PWM ,free stack in bytes" &ro
{NULL,(u8*)&regs_global.vars.regs_event_task_stack,0,"regs event - regs changes handler, free stack size in bytes","regs_event_task_stack",U32_REGS_FLAG,100,600,1,3},//!<"regs event - regs changes handler, free stack size in bytes" &ro
{NULL,(u8*)&regs_global.vars.packet_task_stack,0,"packet handler, free stack size in bytes","packet_task_stack",U32_REGS_FLAG,101,604,1,3},//!<"packet handler, free stack size in bytes" &ro
{NULL,(u8*)&regs_global.vars.monitor_task_stack,0,"monitore for debug purpose, free stack size in bytes","monitor_task_stack",U32_REGS_FLAG,102,608,1,3},//!<"monitore for debug purpose, free stack size in bytes" &ro
{NULL,(u8*)&regs_global.vars.duty_task_stack,0,"duty for long time event handler, system led etc, free stack size in bytes","duty_task_stack",U32_REGS_FLAG,103,612,1,3},//!<"duty for long time event handler, system led etc, free stack size in bytes" &ro
{NULL,(u8*)&regs_global.vars.idle_task_stack,0,"idle processor time counter, free stack size in bytes","idle_task_stack",U32_REGS_FLAG,104,616,1,3},//!<"idle processor time counter, free stack size in bytes" &ro
{NULL,(u8*)&regs_global.vars.usb_cdc_task_stack,0,"usb cdc, free stack size in bytes","usb_cdc_task_stack",U32_REGS_FLAG,105,620,1,3},//!<"usb cdc, free stack size in bytes" &ro
{NULL,(u8*)&regs_global.vars.ethernetif_task_stack,0,"ethernet interface(MAC) input handling, free stack size in bytes","ethernetif_task_stack",U32_REGS_FLAG,106,624,1,3},//!<"ethernet interface(MAC) input handling, free stack size in bytes" &ro
{NULL,(u8*)&regs_global.vars.tcpip_task_stack,0,"lwip tcp_ip ip packet handling(TCP,UDP), free stack size in bytes","tcpip_task_stack",U32_REGS_FLAG,107,628,1,3},//!<"lwip tcp_ip ip packet handling(TCP,UDP), free stack size in bytes" &ro
{NULL,(u8*)&regs_global.vars.ethernetif_control_task_stack,0,"ethernet interface(MAC) state control, free stack size in bytes","ethernetif_control_task_stack",U32_REGS_FLAG,108,632,1,3},//!<"ethernet interface(MAC) state control, free stack size in bytes" &ro
{NULL,(u8*)&regs_global.vars.ds18b20_task_stack,0,"ds18b20 temperature handler, free stack size in bytes","ds18b20_task_stack",U32_REGS_FLAG,109,636,1,3},//!<"ds18b20 temperature handler, free stack size in bytes" &ro
{NULL,(u8*)&regs_global.vars.init_task_stack,0,"for asynchronous long processes of initialisation, free stack size in bytes","init_task_stack",U32_REGS_FLAG,110,640,1,3},//!<"for asynchronous long processes of initialisation, free stack size in bytes" &ro
{NULL,(u8*)&regs_global.vars.usb_handle_task_stack,0,"usb input packet handler, free stack size in bytes","usb_handle_task_stack",U32_REGS_FLAG,111,644,1,3},//!<"usb input packet handler, free stack size in bytes" &ro
{NULL,(u8*)&regs_global.vars.timers_task_stack,0,"freertos timers handler, free stack size in bytes","timers_task_stack",U32_REGS_FLAG,112,648,1,3},//!<"freertos timers handler, free stack size in bytes" &ro
{NULL,(u8*)&regs_global.vars.isimfw400_task_time,0,"main control isimfw400 - PID, control PWM ,percentage share in processor resource","isimfw400_task_time",FLOAT_REGS_FLAG,113,652,1,3},//!<"main control isimfw400 - PID, control PWM ,percentage share in processor resource" &ro
{NULL,(u8*)&regs_global.vars.regs_event_task_time,0,"regs event handler, percentage share in processor resource","regs_event_task_time",FLOAT_REGS_FLAG,114,656,1,3},//!<"regs event handler, percentage share in processor resource" &ro
{NULL,(u8*)&regs_global.vars.packet_task_time,0,"packet handler (modbus), percentage share in processor resource","packet_task_time",FLOAT_REGS_FLAG,115,660,1,3},//!<"packet handler (modbus), percentage share in processor resource" &ro
{NULL,(u8*)&regs_global.vars.monitor_task_time,0,"for debug purpose, percentage share in processor resource","monitor_task_time",FLOAT_REGS_FLAG,116,664,1,3},//!<"for debug purpose, percentage share in processor resource" &ro
{NULL,(u8*)&regs_global.vars.duty_task_time,0,"duty for long time event handler like system led etc,percentage share in processor resource","duty_task_time",FLOAT_REGS_FLAG,117,668,1,3},//!<"duty for long time event handler like system led etc,percentage share in processor resource" &ro
{NULL,(u8*)&regs_global.vars.idle_task_time,0,"idle processor time counter, percentage share in processor resource","idle_task_time",FLOAT_REGS_FLAG,118,672,1,3},//!<"idle processor time counter, percentage share in processor resource" &ro
{NULL,(u8*)&regs_global.vars.usb_cdc_task_time,0,"usb cdc, percentage share in processor resource","usb_cdc_task_time",FLOAT_REGS_FLAG,119,676,1,3},//!<"usb cdc, percentage share in processor resource" &ro
{NULL,(u8*)&regs_global.vars.ethernetif_task_time,0,"ethernet interface(MAC) input handling, percentage share in processor resource","ethernetif_task_time",FLOAT_REGS_FLAG,120,680,1,3},//!<"ethernet interface(MAC) input handling, percentage share in processor resource" &ro
{NULL,(u8*)&regs_global.vars.tcpip_task_time,0,"lwip tcp_ip ip packet handling(TCP,UDP), percentage share in processor resource","tcpip_task_time",FLOAT_REGS_FLAG,121,684,1,3},//!<"lwip tcp_ip ip packet handling(TCP,UDP), percentage share in processor resource" &ro
{NULL,(u8*)&regs_global.vars.ethernetif_control_task_time,0,"ethernet interface(MAC) state control, percentage share in processor resource","ethernetif_control_task_time",FLOAT_REGS_FLAG,122,688,1,3},//!<"ethernet interface(MAC) state control, percentage share in processor resource" &ro
{NULL,(u8*)&regs_global.vars.ds18b20_task_time,0,"ds18b20 temperature handler, percentage share in processor resource","ds18b20_task_time",FLOAT_REGS_FLAG,123,692,1,3},//!<"ds18b20 temperature handler, percentage share in processor resource" &ro
{NULL,(u8*)&regs_global.vars.init_task_time,0,"for asynchronous long processes of initialisation, percentage share in processor resource","init_task_time",FLOAT_REGS_FLAG,124,696,1,3},//!<"for asynchronous long processes of initialisation, percentage share in processor resource" &ro
{NULL,(u8*)&regs_global.vars.usb_handle_task_time,0,"usb input packet handler, percentage share in processor resource","usb_handle_task_time",FLOAT_REGS_FLAG,125,700,1,3},//!<"usb input packet handler, percentage share in processor resource" &ro
{NULL,(u8*)&regs_global.vars.timers_task_time,0,"freertos timers handler, percentage share in processor resource","timers_task_time",FLOAT_REGS_FLAG,126,704,1,3},//!<"freertos timers handler, percentage share in processor resource" &ro
{NULL,(u8*)&regs_global.vars.link,0,"lwip state","link",U16_REGS_FLAG,127,708,1,3},//!<"lwip state" &ro
{NULL,(u8*)&regs_global.vars.eth_arp,0,"lwip state","eth_arp",U16_REGS_FLAG,128,710,1,3},//!<"lwip state" &ro
{NULL,(u8*)&regs_global.vars.ip_frag,0,"lwip state","ip_frag",U16_REGS_FLAG,129,712,1,3},//!<"lwip state" &ro
{NULL,(u8*)&regs_global.vars.ip_proto,0,"lwip state","ip_proto",U16_REGS_FLAG,130,714,1,3},//!<"lwip state" &ro
{NULL,(u8*)&regs_global.vars.icmp,0,"lwip state","icmp",U16_REGS_FLAG,131,716,1,3},//!<"lwip state" &ro
{NULL,(u8*)&regs_global.vars.udp,0,"lwip state","udp",U16_REGS_FLAG,132,718,1,3},//!<"lwip state" &ro
{NULL,(u8*)&regs_global.vars.tcp,0,"lwip state","tcp",U16_REGS_FLAG,133,720,1,3},//!<"lwip state" &ro
{NULL,(u8*)&regs_global.vars.memp_udp_pool,0,"lwip state","memp_udp_pool",U16_REGS_FLAG,134,722,1,3},//!<"lwip state" &ro
{NULL,(u8*)&regs_global.vars.memp_tcp_pool,0,"lwip state","memp_tcp_pool",U16_REGS_FLAG,135,724,1,3},//!<"lwip state" &ro
{NULL,(u8*)&regs_global.vars.memp_listen_tcp,0,"lwip state","memp_listen_tcp",U16_REGS_FLAG,136,726,1,3},//!<"lwip state" &ro
{NULL,(u8*)&regs_global.vars.memp_seg_tcp,0,"lwip state","memp_seg_tcp",U16_REGS_FLAG,137,728,1,3},//!<"lwip state" &ro
{NULL,(u8*)&regs_global.vars.memp_altcp,0,"lwip state","memp_altcp",U16_REGS_FLAG,138,730,1,3},//!<"lwip state" &ro
{NULL,(u8*)&regs_global.vars.memp_reassdata,0,"lwip state","memp_reassdata",U16_REGS_FLAG,139,732,1,3},//!<"lwip state" &ro
{NULL,(u8*)&regs_global.vars.memp_frag_pbuf,0,"lwip state","memp_frag_pbuf",U16_REGS_FLAG,140,734,1,3},//!<"lwip state" &ro
{NULL,(u8*)&regs_global.vars.memp_net_buf,0,"lwip state","memp_net_buf",U16_REGS_FLAG,141,736,1,3},//!<"lwip state" &ro
{NULL,(u8*)&regs_global.vars.memp_net_conn,0,"lwip state","memp_net_conn",U16_REGS_FLAG,142,738,1,3},//!<"lwip state" &ro
{NULL,(u8*)&regs_global.vars.memp_tcpip_api,0,"lwip state","memp_tcpip_api",U16_REGS_FLAG,143,740,1,3},//!<"lwip state" &ro
{NULL,(u8*)&regs_global.vars.memp_tcpip_input,0,"lwip state","memp_tcpip_input",U16_REGS_FLAG,144,742,1,3},//!<"lwip state" &ro
{NULL,(u8*)&regs_global.vars.memp_sys_timeout,0,"lwip state","memp_sys_timeout",U16_REGS_FLAG,145,744,1,3},//!<"lwip state" &ro
{NULL,(u8*)&regs_global.vars.memp_pbuf_ref,0,"lwip state","memp_pbuf_ref",U16_REGS_FLAG,146,746,1,3},//!<"lwip state" &ro
{NULL,(u8*)&regs_global.vars.memp_pbuf_pool,0,"lwip state","memp_pbuf_pool",U16_REGS_FLAG,147,748,1,3},//!<"lwip state" &ro
{NULL,(u8*)&regs_global.vars.lwip_sys,0,"lwip state","lwip_sys",U16_REGS_FLAG,148,750,1,3},//!<"lwip state" &ro
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
    result = 1;
    name_size = strlen(regs_template->name);
    if(name_size > REGS_MAX_NAME_SIZE){
       name_size = REGS_MAX_NAME_SIZE;
    }
    regs_global.vars.num_of_vars = (regs_global.vars.num_of_vars<NUM_OF_SELF_VARS)?NUM_OF_SELF_VARS:regs_global.vars.num_of_vars;
    const regs_description_t * description_temp =NULL;
    for (i=0 ; i<regs_global.vars.num_of_vars; i++){
        if(i<NUM_OF_SELF_VARS){
            if (memcmp(regs_template->name, regs_description[i].name, name_size) == 0){
                description_temp = &regs_description[i];
                regs_template->ind = regs_description[i].ind;
                regs_template->p_value = REGS_GLOBAL_ADDRESS(description_temp->guid);
                result = 0;
                break;
            }
        }else if(regs_description_user!=NULL){
            if (memcmp(regs_template->name, regs_description_user[i-NUM_OF_SELF_VARS].name, name_size) == 0){
                description_temp = &regs_description_user[i-NUM_OF_SELF_VARS];
                regs_template->ind = regs_description_user[i-NUM_OF_SELF_VARS].ind+NUM_OF_SELF_VARS;
                regs_template->p_value = description_temp->p_value;
                result = 0;
                break;
            }
        }else{
            result =-2;
            break;
        }
    }
    if((description_temp != NULL) && (result==0)){
        regs_template->description = description_temp->description;
        regs_template->guid = description_temp->guid;
        regs_template->p_default = description_temp->p_default;
        regs_template->saved_address = description_temp->saved_address;
        regs_template->property = description_temp->property;
        regs_template->size = description_temp->size;
        regs_template->type = description_temp->type;
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
        if(regs_template->ind<NUM_OF_SELF_VARS){
            description_temp = &regs_description[regs_template->ind];
            regs_template->p_value = REGS_GLOBAL_ADDRESS(description_temp->guid);
        }else if(regs_description_user!=NULL){
            description_temp = &regs_description_user[regs_template->ind-NUM_OF_SELF_VARS];
            regs_template->p_value = description_temp->p_value;
        }else{
            result = -2;
        }
        if((description_temp != NULL) && (result==0)){
            regs_template->description = description_temp->description;
            regs_template->guid = description_temp->guid;
            regs_template->name = description_temp->name;
            regs_template->p_default = description_temp->p_default;
            regs_template->saved_address = description_temp->saved_address;
            regs_template->property = description_temp->property;
            regs_template->size = description_temp->size;
            regs_template->type = description_temp->type;
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
    int result = 1;
    u16 i;
    const regs_description_t * description_temp =NULL;
    regs_global.vars.num_of_vars = (regs_global.vars.num_of_vars<NUM_OF_SELF_VARS)?NUM_OF_SELF_VARS:regs_global.vars.num_of_vars;
    for (i=0 ; i<regs_global.vars.num_of_vars ; i++){
        if(i<NUM_OF_SELF_VARS){
            if (regs_template->guid == regs_description[i].guid){
                description_temp = &regs_description[i];
                regs_template->ind = regs_description[i].ind;
                regs_template->p_value = REGS_GLOBAL_ADDRESS(description_temp->guid);
                result = 0;
                break;
            }
        }else if(regs_description_user!=NULL){
            if (regs_template->guid == regs_description_user[i-NUM_OF_SELF_VARS].guid){
                description_temp = &regs_description_user[i-NUM_OF_SELF_VARS];
                regs_template->ind = regs_description_user[i-NUM_OF_SELF_VARS].ind + NUM_OF_SELF_VARS;
                regs_template->p_value = description_temp->p_value;
                result = 0;
                break;
            }
        }else{
            result = -2;
            break;
        }
    }
    if((description_temp != NULL) && (result==0)){
        regs_template->description = description_temp->description;
        regs_template->name = description_temp->name;
        regs_template->p_default = description_temp->p_default;
        regs_template->saved_address = description_temp->saved_address;
        regs_template->property = description_temp->property;
        regs_template->size = description_temp->size;
        regs_template->type = description_temp->type;
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
    osMutexWait(regs_access_mutex, portMAX_DELAY );{
        if ((regs_template_to_write->p_value!=NULL)&&(value!=NULL)){
            u32 size_in_byte = regs_size_in_byte(regs_template_to_write->type);
            size_in_byte *= regs_template_to_write->size;
            memcpy(regs_template_to_write->p_value,value,size_in_byte);
            if (regs_template_to_write->property&SAVING){
                /*!< save to mirror @todo add handl for flash saved vars*/
                if ((regs_template_to_write->saved_address+size_in_byte)<=BKSRAM_SIZE){
                    mirror_access_write((u16)regs_template_to_write->saved_address,value,(u16)size_in_byte);
                }
            }
        }else{
            res = -1;
            led_os_error_on(REGS_WRITE_ERROR);
        }
    }osMutexRelease(regs_access_mutex);
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
u8 regs_description_is_writeable (u16 reg_index){
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
 * @brief regs_get_index_by_byte_address return index of regs by byte address
 * @param byte_address - byte number
 * @return
 */
int regs_description_get_index_by_byte_address(u32 byte_address){
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

#endif //REGS_DESCRIPTION_C
