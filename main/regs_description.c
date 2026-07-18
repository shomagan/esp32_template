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
static regs_description_list_t regs_description_list[LIST_DESCRIPTIONS_SIZE];/*array of descriptions of registers, each element is array of regs_template_t with num_of_vars size*/

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
static const u16 def_num_of_vars = NUM_OF_MAIN_VARS;
static const u8 def_board_ver = (u8)BOARD_VERSION;
static const u16 def_permission = ENABLE_HTTP_FLAG;
static const u8 def_wifi_name[WIFI_NAME_LEN] = "sofi_plc_air";               //!<"must be strong full filled", &save &def
static const u8 def_wifi_password[WIFI_PASSWORD_LEN] = "bricsofi";            //!<"must be strong more then 8 byte", &save &def
static const u8 def_wifi_router_name[WIFI_ROUTER_NAME_LEN] = WIFE_STATION_SECURE_NAME;
static const u8 def_wifi_router_password[WIFI_ROUTER_PASSWORD_LEN] = WIFE_STATION_SECURE_PASSWORD;
static const u16 def_wifi_setting = WIFI_CLIENT;               //!<"type of wifi and settings" &save &def
const u32 def_table_version = 0x001f;   //!<"current version, if changed, starting from scratch"
static const u16 def_slave_modbus_address = 3;
static const u16 def_min_slave_modbus_address = 1;
static const u16 def_max_slave_modbus_address = 247;
static const u16 def_sleep_time = 25u;
static const u16 def_min_sleep_time = 1u;

regs_description_t const regs_description_global[NUM_OF_MAIN_VARS]={
{ &def_mdb_addr, NULL, NULL, (u8*)&regs_main.regs_global.vars.mdb_addr, 16,"modbus address","mdb_addr", 0x300000, 0x30000, U16_REGS_FLAG, 1, 5, 0 },//!<"modbus address" &save &def
{ &def_ip, NULL, NULL, (u8*)&regs_main.regs_global.vars.ip[0], 18,"device ip address, warning!!! changes can lead to lost connection","ip", 0x300001, 0x30001, U8_REGS_FLAG, 4, 5, 0 },//!<"device ip address, warning!!! changes can lead to lost connection" &save &def &crtcl
{ &def_netmask, NULL, NULL, (u8*)&regs_main.regs_global.vars.netmask[0], 22,"netmask address for main wifi net","netmask", 0x300002, 0x30003, U8_REGS_FLAG, 4, 5, 0 },//!<"netmask address for main wifi net", &save , &def , &crtcl
{ &def_gate, NULL, NULL, (u8*)&regs_main.regs_global.vars.gate[0], 26,"gateaway address, warning!!! changes can lead to lost connection ","gate", 0x300003, 0x30005, U8_REGS_FLAG, 4, 5, 0 },//!<"gateaway address, warning!!! changes can lead to lost connection " &save &def &crtcl
{ &def_slip_ip, NULL, NULL, (u8*)&regs_main.regs_global.vars.slip_ip[0], 30,"ip address for local net","slip_ip", 0x300004, 0x30007, U8_REGS_FLAG, 4, 5, 0 },//!<"ip address for local net",&save , &def &crtcl
{ &def_slip_netmask, NULL, NULL, (u8*)&regs_main.regs_global.vars.slip_netmask[0], 34,"netmask address for local net","slip_netmask", 0x300005, 0x30009, U8_REGS_FLAG, 4, 5, 0 },//!<"netmask address for local net", &save , &def , &crtcl
{ &def_slip_gate, NULL, NULL, (u8*)&regs_main.regs_global.vars.slip_gate[0], 38,"gateaway address for local net","slip_gate", 0x300006, 0x3000b, U8_REGS_FLAG, 4, 5, 0 },//!<"gateaway address for local net", &save, &def &crtcl
{ &def_wifi_name, NULL, NULL, (u8*)&regs_main.regs_global.vars.wifi_name[0], 42,"must be strong full filled","wifi_name", 0x300007, 0x3000d, U8_REGS_FLAG, 13, 5, 0 },//!<"must be strong full filled", &save &def &crtcl
{ &def_wifi_password, NULL, NULL, (u8*)&regs_main.regs_global.vars.wifi_password[0], 55,"must be strong 8 byte","wifi_password", 0x300008, 0x30013, U8_REGS_FLAG, 9, 5, 0 },//!<"must be strong 8 byte", &save &def &crtcl
{ &def_wifi_router_name, NULL, NULL, (u8*)&regs_main.regs_global.vars.wifi_router_name[0], 64,"must be ended by zero","wifi_router_name", 0x300009, 0x30018, U8_REGS_FLAG, 32, 5, 0 },//!<"must be ended by zero", &save &def &crtcl
{ &def_wifi_router_password, NULL, NULL, (u8*)&regs_main.regs_global.vars.wifi_router_password[0], 96,"must be more or equal 8 byte","wifi_router_password", 0x30000a, 0x30028, U8_REGS_FLAG, 32, 5, 0 },//!<"must be more or equal 8 byte", &save &def &crtcl
{ &def_wifi_setting, NULL, NULL, (u8*)&regs_main.regs_global.vars.wifi_setting, 128,"type of wifi and settings","wifi_setting", 0x30000b, 0x30038, U16_REGS_FLAG, 1, 5, 0 },//!<"type of wifi and settings" &save &def &crtcl
{ NULL, NULL, NULL, (u8*)&regs_main.regs_global.vars.reset_num, 130,"number of reset","reset_num", 0x30000c, 0x30039, U32_REGS_FLAG, 1, 5, 0 },//!<"number of reset" &save
{ NULL, NULL, NULL, (u8*)&regs_main.regs_global.vars.wifi_state, 0,"wifi state","wifi_state", 0x30000d, 0x3003b, U16_REGS_FLAG, 1, 3, 0 },//!<"wifi state" &ro
{ NULL, NULL, NULL, (u8*)&regs_main.regs_global.vars.command, 0,"command register","command", 0x30000e, 0x3003c, U16_REGS_FLAG, 1, 1, 0 },//!<"command register"
{ &def_uart_debug_sets, NULL, NULL, (u8*)&regs_main.regs_global.vars.uart_debug_sets, 134,"settings debug uart speed,parity and stop bits, default 115200 ,parity - none, 1 stop bit","uart_debug_sets", 0x30000f, 0x3003d, U16_REGS_FLAG, 1, 5, 0 },//!<"settings debug uart speed,parity and stop bits, default 115200 ,parity - none, 1 stop bit" &save &def
{ NULL, NULL, NULL, (u8*)&regs_main.regs_global.vars.device_state, 0,"BIT(0) - interlock","device_state", 0x300010, 0x3003e, U16_REGS_FLAG, 1, 3, 0 },//!< "BIT(0) - interlock" &ro
{ NULL, NULL, NULL, (u8*)&regs_main.regs_global.vars.unused_3, 0,"unused_3","unused_3", 0x300011, 0x3003f, U16_REGS_FLAG, 1, 3, 0 },//!<"" &ro
{ NULL, NULL, NULL, (u8*)&regs_main.regs_global.vars.sys_tick_counter, 0,"tick in ms","sys_tick_counter", 0x300012, 0x30040, U64_REGS_FLAG, 1, 3, 0 },//!<"tick in ms",&ro
{ NULL, NULL, NULL, (u8*)&regs_main.regs_global.vars.mac_addr[0], 0,"ethernet mac address","mac_addr", 0x300013, 0x30044, U8_REGS_FLAG, 6, 3, 0 },//!<"ethernet mac address",&ro
{ NULL, NULL, NULL, (u8*)&regs_main.regs_global.vars.flags_succ_init, 0,"success inited modules","flags_succ_init", 0x300014, 0x30047, U32_REGS_FLAG, 1, 3, 0 },//!<"success inited modules" &ro for init in task
{ NULL, NULL, NULL, (u8*)&regs_main.regs_global.vars.cur_free_heap, 0,"in bytes","cur_free_heap", 0x300015, 0x30049, U32_REGS_FLAG, 1, 3, 0 },//!<"in bytes",&ro
{ NULL, NULL, NULL, (u8*)&regs_main.regs_global.vars.min_free_heap, 0,"in bytes","min_free_heap", 0x300016, 0x3004b, U32_REGS_FLAG, 1, 3, 0 },//!<"in bytes",&ro
{ NULL, NULL, NULL, (u8*)&regs_main.regs_global.vars.debug_info[0], 0,"reserved use for debug","debug_info", 0x300017, 0x3004d, U8_REGS_FLAG, 8, 3, 0 },//!<"reserved use for debug"&ro
{ &def_num_of_vars, NULL, NULL, (u8*)&regs_main.regs_global.vars.num_of_vars, 0,"num_of_vars","num_of_vars", 0x300018, 0x30051, U16_REGS_FLAG, 1, 3, 0 },//!<"number of vars self + config(user) &ro &def
{ NULL, NULL, NULL, (u8*)&regs_main.regs_global.vars.user_task_num_of_vars, 0,"number of client vars self","user_task_num_of_vars", 0x300019, 0x30052, U16_REGS_FLAG, 1, 3, 0 },//!<"number of client vars self" &ro
{ NULL, NULL, NULL, (u8*)&regs_main.regs_global.vars.temperature_mcu, 0,"temperature mcu Celsius","temperature_mcu", 0x30001a, 0x30053, FLOAT_REGS_FLAG, 1, 3, 0 },//!<"temperature mcu Celsius" &ro
{ &def_fw_version, NULL, NULL, (u8*)&regs_main.regs_global.vars.fw_version[0], 0,"version like 0.1.1.0","fw_version", 0x30001b, 0x30055, U8_REGS_FLAG, 4, 3, 0 },//!<"version like 0.1.1.0",&ro,&def
{ &def_board_ver, NULL, NULL, (u8*)&regs_main.regs_global.vars.board_ver, 0,"board version","board_ver", 0x30001c, 0x30057, U16_REGS_FLAG, 1, 3, 0 },//!< "board version", &ro, &def
{ NULL, NULL, NULL, (u8*)&regs_main.regs_global.vars.firmware_hash, 0,"hash os","firmware_hash", 0x30001d, 0x30058, U32_REGS_FLAG, 1, 3, 0 },//!< "hash os" &ro
{ NULL, NULL, NULL, (u8*)&regs_main.regs_global.vars.system_error, 0,"system global error","system_error", 0x30001e, 0x3005a, U32_REGS_FLAG, 1, 3, 0 },//!< "system global error" &ro
{ &def_permission, NULL, NULL, (u8*)&regs_main.regs_global.vars.permission, 0,"flags with permissions","permission", 0x30001f, 0x3005c, U16_REGS_FLAG, 1, 3, 0 },//!<"flags with permissions" &ro &def
{ NULL, NULL, NULL, (u8*)&regs_main.regs_global.vars.async_flags, 0,"async flags","async_flags", 0x300020, 0x3005d, U32_REGS_FLAG, 1, 3, 0 },//!< "async flags" &ro
{ NULL, NULL, NULL, (u8*)&regs_main.regs_global.vars.table_to_save, 0,"bit field of tables to save up to 32, defined to zero","table_to_save", 0x300021, 0x3005f, U32_REGS_FLAG, 1, 3, 0 },//!< "bit field of tables to save up to 32, defined to zero" &ro
{ NULL, NULL, NULL, (u8*)&regs_main.regs_global.vars.slip_packet_counter, 0,"count all slip packet","slip_packet_counter", 0x300022, 0x30061, U64_REGS_FLAG, 1, 1, 0 },//!< "count all slip packet"
{ NULL, NULL, NULL, (u8*)&regs_main.regs_global.vars.ap_connections_number, 0,"number of connections","ap_connections_number", 0x300023, 0x30065, U16_REGS_FLAG, 1, 3, 0 },//!< "number of connections" &ro
{ NULL, NULL, NULL, (u8*)&regs_main.regs_global.vars.sta_connect, 0,"sta connect state","sta_connect", 0x300024, 0x30066, U16_REGS_FLAG, 1, 3, 0 },//!< "sta connect state" &ro
{ NULL, NULL, NULL, (u8*)&regs_main.regs_global.vars.iirls_pairing_status, 0,"status","iirls_pairing_status", 0x300025, 0x30067, U16_REGS_FLAG, 1, 3, 0 },//!< "status" &ro
{ NULL, NULL, NULL, (u8*)&regs_main.regs_global.vars.iirls_paired_ip[0], 0,"ip address of paired device","iirls_paired_ip", 0x300026, 0x30068, U8_REGS_FLAG, 4, 3, 0 },//!< "ip address of paired device" &ro
{ NULL, NULL, NULL, (u8*)&regs_main.regs_global.vars.modbus_master_test_value, 0,"status","modbus_master_test_value", 0x300027, 0x3006a, U16_REGS_FLAG, 1, 3, 0 },//!< "status" &ro
{ NULL, NULL, NULL, (u8*)&regs_main.regs_global.vars.iirls_state, 136,"state","iirls_state", 0x300028, 0x3006b, U16_REGS_FLAG, 1, 5, 0 },//!< "state" &save
{ NULL, NULL, NULL, (u8*)&regs_main.regs_global.vars.paired_device_model_number[0], 0,"state","paired_device_model_number", 0x300029, 0x3006c, U16_REGS_FLAG, 40, 3, 0 },//!< "state" &ro
{ &def_slave_modbus_address, &def_min_slave_modbus_address, &def_max_slave_modbus_address, (u8*)&regs_main.regs_global.vars.slave_modbus_address, 138,"slave modbus address","slave_modbus_address", 0x30002a, 0x30094, U16_REGS_FLAG, 1, 197, 0 },//!< "slave modbus address" &def &save &max &min
{ NULL, NULL, NULL, (u8*)&regs_main.regs_global.vars.modbus_master_comm_period_current, 0,"modbus_master_comm_period_current","modbus_master_comm_period_current", 0x30002b, 0x30095, U32_REGS_FLAG, 1, 1, 0 },//!< ""
{ NULL, NULL, NULL, (u8*)&regs_main.regs_global.vars.modbus_master_comm_period_max, 0,"modbus_master_comm_period_max","modbus_master_comm_period_max", 0x30002c, 0x30097, U32_REGS_FLAG, 1, 1, 0 },//!< ""
{ NULL, NULL, NULL, (u8*)&regs_main.regs_global.vars.modbus_master_comm_period_min, 0,"modbus_master_comm_period_min","modbus_master_comm_period_min", 0x30002d, 0x30099, U32_REGS_FLAG, 1, 1, 0 },//!< ""
{ NULL, NULL, NULL, (u8*)&regs_main.regs_global.vars.modbus_master_succ_transactions_number, 0,"modbus_master_succ_transactions_number","modbus_master_succ_transactions_number", 0x30002e, 0x3009b, U32_REGS_FLAG, 1, 1, 0 },//!< ""
{ NULL, NULL, NULL, (u8*)&regs_main.regs_global.vars.modbus_master_error_transactions_number, 0,"modbus_master_error_transactions_number","modbus_master_error_transactions_number", 0x30002f, 0x3009d, U32_REGS_FLAG, 1, 1, 0 },//!< ""
{ NULL, NULL, NULL, (u8*)&regs_main.regs_global.vars.i2c_display_address, 0,"address of display","i2c_display_address", 0x300030, 0x3009f, U16_REGS_FLAG, 1, 3, 0 },//!< "address of display" &ro
{ NULL, NULL, NULL, (u8*)&regs_main.regs_global.vars.sta_ip[0], 140,"ip address of sta","sta_ip", 0x300031, 0x300a0, U8_REGS_FLAG, 4, 7, 0 },//!< "ip address of sta" &ro &save
{ NULL, NULL, NULL, (u8*)&regs_main.regs_global.vars.live_time, 0,"live time in seconds","live_time", 0x300032, 0x300a2, U32_REGS_FLAG, 1, 3, 0 },//!< "live time in seconds" &ro
{ NULL, NULL, NULL, (u8*)&regs_main.regs_global.vars.unix_time, 0,"unix time , not implemented","unix_time", 0x300033, 0x300a4, S64_REGS_FLAG, 1, 1, 0 },//!< "unix time , not implemented"
{ NULL, NULL, NULL, (u8*)&regs_main.regs_global.vars.seconds_of_the_day, 0,"seconds of the day","seconds_of_the_day", 0x300034, 0x300a8, U32_REGS_FLAG, 1, 1, 0 },//!< "seconds of the day"
{ NULL, NULL, NULL, (u8*)&regs_main.regs_global.vars.flash_write_number, 144,"increments every flash write by an app","flash_write_number", 0x300035, 0x300aa, U32_REGS_FLAG, 1, 7, 0 },//!< "increments every flash write by an app" &ro &save
{ NULL, NULL, NULL, (u8*)&regs_main.regs_global.vars.current_state[0], 0,"current state of proccess","current_state", 0x300036, 0x300ac, U32_REGS_FLAG, 4, 3, 0 },//!< "current state of proccess" &ro
{ &def_sleep_time, &def_min_sleep_time, NULL, (u8*)&regs_main.regs_global.vars.sleep_time, 0,"seconds,couple with SLEEP_TASK_DEEP_SLEEP_FOR_N_SEC","sleep_time", 0x300037, 0x300b4, U16_REGS_FLAG, 1, 65, 0 },//!< "seconds,couple with SLEEP_TASK_DEEP_SLEEP_FOR_N_SEC"  &def &min
{ NULL, NULL, NULL, (u8*)&regs_main.regs_global.vars.rssi_ap[0], 0,"wifi rssi of Access point","rssi_ap", 0x300038, 0x300b5, I8_REGS_FLAG, 2, 3, 0 },//!< "wifi rssi of Access point" &ro
{ NULL, NULL, NULL, (u8*)&regs_main.regs_global.vars.primary_channel_ap[0], 0,"wifi channel used","primary_channel_ap", 0x300039, 0x300b6, I8_REGS_FLAG, 2, 3, 0 },//!< "wifi channel used" &ro
{ NULL, NULL, NULL, (u8*)&regs_main.regs_global.vars.wake_up_cause, 0,"wake up reason","wake_up_cause", 0x30003a, 0x300b7, S32_REGS_FLAG, 1, 3, 0 },//!< "wake up reason" &ro
{ &def_table_version, NULL, NULL, (u8*)&regs_main.regs_global.vars.table_version, 148,"version of main table change value in def_table_version for drop all regs to default value","table_version", 0x30003b, 0x300b9, U32_REGS_FLAG, 1, 7, 0 },//!< "version of main table change value in def_table_version for drop all regs to default value" &ro &def &save
{ NULL, NULL, NULL, (u8*)&regs_main.regs_global.vars.table_id, 0,"support variable for SET_DEFAULT_VALUES_COMM","table_id", 0x30003c, 0x300bb, U16_REGS_FLAG, 1, 1, 0 },//!< "support variable for SET_DEFAULT_VALUES_COMM"
};


static u32 num_of_list_descriptions = 0;
static u16 num_user_tasks_vars = 0;

int regs_description_list_add_new(regs_description_list_t regs_table){
   int index = 0;
   if ((regs_table.description != NULL) && (regs_table.num_of_regs > 0) && (num_of_list_descriptions < LIST_DESCRIPTIONS_SIZE)){
      regs_description_list[num_of_list_descriptions] = regs_table; /*add new description to list*/
      if(num_of_list_descriptions){
         num_user_tasks_vars += regs_description_list[num_of_list_descriptions].num_of_regs;
         regs_copy_safe(&regs_main.regs_global.vars.user_task_num_of_vars, &num_user_tasks_vars, sizeof(num_user_tasks_vars));
      }
      index = num_of_list_descriptions;
      num_of_list_descriptions++;
   }else{
      index = -1;//!< wrong input params or list full
   }
   return index;
}
regs_description_t * regs_description_list_get_description(u16 table_ind){
   regs_description_t * result = NULL;
   if (table_ind < num_of_list_descriptions){
      result = (regs_description_t *)regs_description_list[table_ind].description;
   }
   return result;
}

const char *regs_description_list_get_space_name(u16 table_ind){
   const char * result = NULL;
   if (table_ind < num_of_list_descriptions){
      result = regs_description_list[table_ind].space_name;
   }
   return result;
}

u32 regs_description_list_get_num_of_regs(u16 table_ind){
   u32 result = 0;
   if (table_ind < num_of_list_descriptions){
      result = regs_description_list[table_ind].num_of_regs;
   }
   return result;
}

u8 * regs_description_list_get_buffer(u16 table_ind){
   u8 * result = NULL;
   if (table_ind < num_of_list_descriptions){
      result = (u8*)regs_description_list[table_ind].saved_regs_buffer;
   }
   return result;
}
size_t regs_description_list_get_saved_buffer_size(u16 table_ind){
   size_t result = 0;
   if (table_ind < num_of_list_descriptions){
      result = regs_description_list[table_ind].saved_regs_buffer_size;
   }
   return result;
}
int regs_description_get_by_index(regs_template_t * regs_template, u32 index){
   int result = -1;
   u32 max_register_index = 0;
   u32 max_register_index_prev = 0;
   for(int i = 0; i < num_of_list_descriptions; i++){
      max_register_index += regs_description_list[i].num_of_regs;
      if(index < max_register_index){
         regs_template->table_ind = i;
         result = regs_description_get_by_index_in_table(regs_template, index - max_register_index_prev);
         break;
      }
      max_register_index_prev = max_register_index;
   }
   return result;
}
int regs_description_get_by_index_in_table(regs_template_t * regs_template, u32 ind){
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

int regs_description_get_index_by_name(regs_template_t *regs_template) {
   int result = -1;
   u32 name_size = strlen(regs_template->name);
   u32 index = 0;
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
               result = index;
               break;
            }
            index++;
         }
         if (result >= 0) {
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
   if ((regs_template->p_value != NULL) && (value != NULL)){
      semaphore_take(regs_access_mutex, portMAX_DELAY );{
      memcpy(regs_template->p_value,value,regs_template->size_in_bytes);
      }semaphore_release(regs_access_mutex);
      if (regs_template->property & SAVING){
         mirror_access_write(regs_template);
      }
   }else{
      res = -1;
   }
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

int regs_description_get_index_by_address(const void * address){
   int global_index = -1;
   u32 base_index = 0;
   for (int i = 0; i < (int)num_of_list_descriptions; i++){
      const regs_description_t *regs_array = regs_description_list[i].description;
      u16 num_regs = regs_description_list[i].num_of_regs;
      for (int j = 0; j < num_regs; j++){
         const u8 *reg_start = regs_array[j].p_value;
         const u8 *reg_end   = reg_start + regs_size_in_byte(regs_array[j].type) * regs_array[j].size;
         if (((const u8*)address >= reg_start) && ((const u8*)address < reg_end)){
            global_index = (int)(base_index + (u32)j);
            break;
         }
      }
      if (global_index >= 0) break;
      base_index += num_regs;
   }
   return global_index;
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

u32 regs_description_get_regs_string_value(regs_template_t *regs_template, u8 reg_num, char *buffer, u32 buffer_size) {
   u32 result = 0;
   u64 val_u64 = 0;
   s32 temp_i32 = 0;
   s16 temp_i16 = 0;
   i8 temp_i8 = 0;
   s64 temp_i64 = 0;
   float val_float = 0.0f;
   double val_double = 0.0;
   u32 pos = 0;
   char temp_buf[128] = { 0 };
   u32 temp_buf_len = 0;
   if (buffer != NULL || buffer_size != 0) {
      temp_buf_len += sprintf(temp_buf, "%s: ", regs_template->name);
      for (u16 i = 0; i < regs_template->size; i++) {
         switch (regs_template->type) {
         case U8_REGS_FLAG:
         case U16_REGS_FLAG:
         case U32_REGS_FLAG:
         case U64_REGS_FLAG:
         case TIME_REGS_FLAG:
            val_u64 = 0;
            memcpy(&val_u64, (regs_template->p_value + i * regs_size_in_byte(regs_template->type)),
                   regs_size_in_byte(regs_template->type));
            temp_buf_len += sprintf(&temp_buf[temp_buf_len], "%llu,", val_u64);
            break;
         case I8_REGS_FLAG:
            temp_i8 = 0;
            memcpy(&temp_i8, (regs_template->p_value + i * regs_size_in_byte(regs_template->type)),
                   regs_size_in_byte(regs_template->type));
            temp_buf_len += sprintf(&temp_buf[temp_buf_len], "%i,", temp_i8);
            break;
         case S16_REGS_FLAG:
            temp_i16 = 0;
            memcpy(&temp_i16, (regs_template->p_value + i * regs_size_in_byte(regs_template->type)),
                   regs_size_in_byte(regs_template->type));
            temp_buf_len += sprintf(&temp_buf[temp_buf_len], "%i,", temp_i16);
            break;
         case S32_REGS_FLAG:
         case INT_REGS_FLAG:
            temp_i32 = 0;
            memcpy(&temp_i32, (regs_template->p_value + i * regs_size_in_byte(regs_template->type)),
                   regs_size_in_byte(regs_template->type));
            temp_buf_len += sprintf(&temp_buf[temp_buf_len], "%li,", temp_i32);
            break;
         case S64_REGS_FLAG:
            temp_i64 = 0;
            memcpy(&temp_i64, (regs_template->p_value + i * regs_size_in_byte(regs_template->type)),
                   regs_size_in_byte(regs_template->type));
            temp_buf_len += sprintf(&temp_buf[temp_buf_len], "%lli,", temp_i64);
            break;
         case FLOAT_REGS_FLAG:
            val_float = 0.0f;
            memcpy(&val_float, (regs_template->p_value + i * regs_size_in_byte(regs_template->type)),
                   regs_size_in_byte(regs_template->type));
            if (isnanf(val_float) || isinff(val_float)) {
               temp_buf_len += sprintf(&temp_buf[temp_buf_len], "\"%f\",", (double)val_float);
            } else {
               temp_buf_len += sprintf(&temp_buf[temp_buf_len], "%f,", (double)val_float);
            }
            break;
         case DOUBLE_REGS_FLAG:
            val_double = 0.0;
            memcpy(&val_double, (regs_template->p_value + i * regs_size_in_byte(regs_template->type)),
                   regs_size_in_byte(regs_template->type));
            if (isnanf((float)val_double) || isinff((float)val_double)) {
               temp_buf_len += sprintf(&temp_buf[temp_buf_len], "\"%f\",", val_double);
            } else {
               temp_buf_len += sprintf(&temp_buf[temp_buf_len], "%f,", val_double);
            }
            break;
         }
      }
      if(temp_buf_len < buffer_size){
         memcpy(buffer, temp_buf, temp_buf_len);
         result = temp_buf_len;
      }
   }
   return result;
}

static void * regs_description_get_pointer_by_modbus_in_table(u16 modbus_address, u8 modbus_function, u16 table_ind){
    void * p_value= NULL;
    int current;
    int left = 0;
    int right;
    regs_description_t * regs_description = regs_description_list_get_description(table_ind);
    right = regs_description_list_get_num_of_regs(table_ind)-1;
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

void * regs_description_get_pointer_by_modbus(u16 modbus_address, u8 modbus_function){
    void * p_value= NULL;
    /*search for all tables */
    for(u32 i = 0; i < num_of_list_descriptions; i++){
        p_value = regs_description_get_pointer_by_modbus_in_table(modbus_address, modbus_function, i);
        if (p_value != NULL){
            break;
        }
    }
    return p_value;
}

int set_regs_def_values(u16 table_ind) {
   int res = 0;
   regs_template_t regs_template = { 0 };
   regs_template.table_ind = table_ind;
   u32 num_of_regs = regs_description_list_get_num_of_regs(table_ind);
   main_printf(TAG,"set_regs_def_values: table_ind=%d, num_of_regs=%d\n", table_ind, num_of_regs);
   for (u32 i = 0; i < num_of_regs; i++) {
      if (regs_description_get_by_index_in_table(&regs_template, i) == 0) {
         if (regs_template.p_default == NULL) {
            memset(regs_template.p_value, 0, regs_template.size_in_bytes);
         } else {
            memcpy(regs_template.p_value, regs_template.p_default, regs_template.size_in_bytes);
         }
      }
   }
   return res;
}

int regs_fill_temp_buffer(regs_template_t *regs_template, regs_access_t reg, temp_data_buffering_t * temp_data_buffering, void *access_address){
    int res = 0;
    if (regs_template != NULL && regs_template->p_value != NULL && temp_data_buffering != NULL){
        u8 reg_write_size = regs_size_in_byte(reg.flag);
        u32 shift = (u32)access_address - (u32)regs_template->p_value;
        u8 regs_item_size = regs_size_in_byte(regs_template->type);
        u8 start_byte = shift % regs_item_size;/*0-7*/
        if ((u32)temp_data_buffering->reg_address != ((u32)access_address - start_byte)){
            temp_data_buffering->reg_address = (void*)((u32)access_address - start_byte);
            temp_data_buffering->byte_writed_flags = 0;
            temp_data_buffering->type = regs_template->type;
            memset(temp_data_buffering->data.bytes,0,8);
        }
        reg_write_size = reg_write_size>(regs_item_size-start_byte)?(regs_item_size-start_byte):reg_write_size;
        memcpy(&temp_data_buffering->data.bytes[start_byte], &reg.value, reg_write_size);
        for(u32 i=start_byte;i<(reg_write_size + start_byte);i++){
            temp_data_buffering->byte_writed_flags |= BIT(i);
        }
        res = reg_write_size;
    }else{
        res =-1;
    }
    return res;
}

int regs_check_temp_buffer(temp_data_buffering_t * temp_data_buffering, regs_template_t * regs_template){
    int res = 0;
    if (regs_template != NULL && regs_template->p_value != NULL && temp_data_buffering != NULL){
        u8 regs_item_size = regs_size_in_byte(regs_template->type);
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

int regs_clear_temp_buffer(temp_data_buffering_t * temp_data_buffering){
    int res = 0;
    if (temp_data_buffering!=NULL){
        temp_data_buffering->reg_address = NULL;
        temp_data_buffering->byte_writed_flags = 0;
        temp_data_buffering->type = 0;
        temp_data_buffering->byte_writed_flags = 0;
        memset(temp_data_buffering->data.bytes,0,8);
    }
    return res;
}
#endif //REGS_DESCRIPTION_C
