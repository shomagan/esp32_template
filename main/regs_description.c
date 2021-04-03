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
static const u8 def_wifi_name[WIFI_NAME_LEN] = "bric_plc_net";               //!<"must be strong full filled", &save &def
static const u8 def_wifi_password[WIFI_PASSWORD_LEN] = "bricsofi";            //!<"must be strong more then 8 byte", &save &def
static const u8 def_wifi_router_name[WIFI_ROUTER_NAME_LEN] = "MikroTik-Jul";
static const u8 def_wifi_router_password[WIFI_ROUTER_PASSWORD_LEN] = "zyxel-206-rpu";
static const u16 def_wifi_setting = WIFI_AP_STA;               //!<"type of wifi and settings" &save &def
static const float def_test_pwm_value = 50.0;
const u32 def_table_version = 4;
regs_description_t const regs_description[NUM_OF_SELF_VARS]={
{&def_mdb_addr,(u8*)&regs_global.vars.mdb_addr,16,"modbus address","mdb_addr",U16_REGS_FLAG,0,0,0x30000,1,5},//!<"modbus address" &save &def
{&def_ip,(u8*)&regs_global.vars.ip[0],18,"device ip address, warning!!! changes can lead to lost connection","ip",U8_REGS_FLAG,1,2,0x30001,4,37},//!<"device ip address, warning!!! changes can lead to lost connection" &save &def &crtcl
{&def_netmask,(u8*)&regs_global.vars.netmask[0],22,"netmask address for main wifi net","netmask",U8_REGS_FLAG,2,6,0x30003,4,37},//!<"netmask address for main wifi net", &save , &def , &crtcl
{&def_gate,(u8*)&regs_global.vars.gate[0],26,"gateaway address, warning!!! changes can lead to lost connection ","gate",U8_REGS_FLAG,3,10,0x30005,4,37},//!<"gateaway address, warning!!! changes can lead to lost connection " &save &def &crtcl
{&def_slip_ip,(u8*)&regs_global.vars.slip_ip[0],30,"ip address for local net","slip_ip",U8_REGS_FLAG,4,14,0x30007,4,37},//!<"ip address for local net",&save , &def &crtcl
{&def_slip_netmask,(u8*)&regs_global.vars.slip_netmask[0],34,"netmask address for local net","slip_netmask",U8_REGS_FLAG,5,18,0x30009,4,37},//!<"netmask address for local net", &save , &def , &crtcl
{&def_slip_gate,(u8*)&regs_global.vars.slip_gate[0],38,"gateaway address for local net","slip_gate",U8_REGS_FLAG,6,22,0x3000b,4,37},//!<"gateaway address for local net", &save, &def &crtcl
{&def_wifi_name,(u8*)&regs_global.vars.wifi_name[0],42,"must be strong full filled","wifi_name",U8_REGS_FLAG,7,26,0x3000d,12,37},//!<"must be strong full filled", &save &def &crtcl
{&def_wifi_password,(u8*)&regs_global.vars.wifi_password[0],54,"must be strong 8 byte","wifi_password",U8_REGS_FLAG,8,38,0x30013,8,37},//!<"must be strong 8 byte", &save &def &crtcl
{&def_wifi_router_name,(u8*)&regs_global.vars.wifi_router_name[0],62,"must be ended by zero","wifi_router_name",U8_REGS_FLAG,9,46,0x30017,32,37},//!<"must be ended by zero", &save &def &crtcl
{&def_wifi_router_password,(u8*)&regs_global.vars.wifi_router_password[0],94,"must be more or equal 8 byte","wifi_router_password",U8_REGS_FLAG,10,78,0x30027,32,37},//!<"must be more or equal 8 byte", &save &def &crtcl
{&def_wifi_setting,(u8*)&regs_global.vars.wifi_setting,126,"type of wifi and settings","wifi_setting",U16_REGS_FLAG,11,110,0x30037,1,37},//!<"type of wifi and settings" &save &def &crtcl
{NULL,(u8*)&regs_global.vars.reset_num,128,"number of reset","reset_num",U32_REGS_FLAG,12,112,0x30038,1,5},//!<"number of reset" &save
{NULL,(u8*)&regs_global.vars.wifi_state,0,"wifi state","wifi_state",U16_REGS_FLAG,13,116,0x3003a,1,3},//!<"wifi state" &ro
{NULL,(u8*)&regs_global.vars.command,0,"command register","command",U16_REGS_FLAG,14,118,0x3003b,1,1},//!<"command register"
{&def_uart_debug_sets,(u8*)&regs_global.vars.uart_debug_sets,132,"settings debug uart speed,parity and stop bits, default 115200 ,parity - none, 1 stop bit","uart_debug_sets",U16_REGS_FLAG,15,120,0x3003c,1,5},//!<"settings debug uart speed,parity and stop bits, default 115200 ,parity - none, 1 stop bit" &save &def
{NULL,(u8*)&regs_global.vars.device_state,0,"BIT(0) - interlock","device_state",U16_REGS_FLAG,16,122,0x3003d,1,3},//!< "BIT(0) - interlock" &ro
{NULL,(u8*)&regs_global.vars.unused_3,0,"unused_3","unused_3",U16_REGS_FLAG,17,124,0x3003e,1,3},//!<"" &ro
{NULL,(u8*)&regs_global.vars.sys_tick_counter,0,"tick in ms","sys_tick_counter",U64_REGS_FLAG,18,126,0x3003f,1,3},//!<"tick in ms",&ro
{NULL,(u8*)&regs_global.vars.mac_addr[0],0,"ethernet mac address","mac_addr",U8_REGS_FLAG,19,134,0x30043,6,3},//!<"ethernet mac address",&ro
{NULL,(u8*)&regs_global.vars.flags_succ_init,0,"success inited modules","flags_succ_init",U32_REGS_FLAG,20,140,0x30046,1,3},//!<"success inited modules" &ro for init in task
{NULL,(u8*)&regs_global.vars.cur_free_heap,0,"in bytes","cur_free_heap",U32_REGS_FLAG,21,144,0x30048,1,3},//!<"in bytes",&ro
{NULL,(u8*)&regs_global.vars.min_free_heap,0,"in bytes","min_free_heap",U32_REGS_FLAG,22,148,0x3004a,1,3},//!<"in bytes",&ro
{NULL,(u8*)&regs_global.vars.debug_info[0],0,"reserved use for debug","debug_info",U8_REGS_FLAG,23,152,0x3004c,8,3},//!<"reserved use for debug"&ro
{&def_num_of_vars,(u8*)&regs_global.vars.num_of_vars,0,"num_of_vars","num_of_vars",U16_REGS_FLAG,24,160,0x30050,1,3},//!<"number of vars self + config(user)&ro &def
{NULL,(u8*)&regs_global.vars.temperature_mcu,0,"temperature mcu Celsius","temperature_mcu",FLOAT_REGS_FLAG,25,162,0x30051,1,3},//!<"temperature mcu Celsius" &ro
{&def_fw_version,(u8*)&regs_global.vars.fw_version[0],0,"version like 0.1.1.0","fw_version",U8_REGS_FLAG,26,166,0x30053,4,3},//!<"version like 0.1.1.0",&ro,&def
{&def_board_ver,(u8*)&regs_global.vars.board_ver,0,"board version","board_ver",U16_REGS_FLAG,27,170,0x30055,1,3},//!< "board version", &ro, &def
{NULL,(u8*)&regs_global.vars.firmware_hash,0,"hash os","firmware_hash",U32_REGS_FLAG,28,172,0x30056,1,3},//!< "hash os" &ro
{NULL,(u8*)&regs_global.vars.system_error,0,"system global error","system_error",U32_REGS_FLAG,29,176,0x30058,1,3},//!< "system global error" &ro
{&def_permission,(u8*)&regs_global.vars.permission,0,"flags with permissions","permission",U16_REGS_FLAG,30,180,0x3005a,1,3},//!<"flags with permissions" &ro &def
{NULL,(u8*)&regs_global.vars.async_flags,0,"async flags","async_flags",U32_REGS_FLAG,31,182,0x3005b,1,3},//!< "async flags" &ro
{NULL,(u8*)&regs_global.vars.slip_packet_counter,0,"count all slip packet","slip_packet_counter",U64_REGS_FLAG,32,186,0x3005d,1,1},//!< "count all slip packet"
{NULL,(u8*)&regs_global.vars.ap_connections_number,0,"number of connections","ap_connections_number",U16_REGS_FLAG,33,194,0x30061,1,3},//!< "number of connections" &ro
{NULL,(u8*)&regs_global.vars.sta_connect,0,"sta connect state","sta_connect",U16_REGS_FLAG,34,196,0x30062,1,3},//!< "sta connect state" &ro
{&def_table_version,(u8*)&regs_global.vars.table_version,134,"change value in def_table_version for drop all regs to default value","table_version",U32_REGS_FLAG,35,198,0x30063,1,7},//!< "change value in def_table_version for drop all regs to default value" &ro &def &save
{&def_test_pwm_value,(u8*)&regs_global_part1.vars.test_pwm_value,138,"test pwm value [0;100]","test_pwm_value",FLOAT_REGS_FLAG,36,202,0x303e8,1,5},//!<"test pwm value [0;100]" &def &save
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
                regs_template->p_value = description_temp->p_value;
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
        regs_template->modbus_description = description_temp->modbus_description;
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
            regs_template->p_value = description_temp->p_value;
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
            regs_template->modbus_description = description_temp->modbus_description;
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
                regs_template->p_value = description_temp->p_value;
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
        regs_template->modbus_description = description_temp->modbus_description;
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
u8 regs_description_is_writeable (u16 index){
    u8 result = 0;
    if (index < NUM_OF_SELF_VARS){
        if(regs_description[index].property & READ_ONLY){
            result = 0;
        }else{
            result = 1;
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
 * @brief regs_description_is_credential
 * @param reg_index
 * @return 1 if credential
 * @ingroup regs
  * @todo check neccessary of this function
 */
u8 regs_description_is_credential(u16 index){
    return regs_description_flag_check(index, CREDENTIAL_FLAG);
}
/**
 * @brief regs_get_index_by_byte_address return index of regs by byte address
 * @param byte_address - byte number
 * @return
 */
int regs_description_get_index_by_guid(u32 guid){
    int result = -1;
    for (int i = 0;i<NUM_OF_SELF_VARS;i++){
        if ((guid>= regs_description[i].guid) &&
            (guid < (regs_description[i].guid+regs_size_in_byte(regs_description[i].type)*regs_description[i].size))){
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
            }else if((modbus_address > reg_mdb_address) &&
                     (modbus_address < (reg_mdb_address + reg_size_in_word))){
                p_value = regs_description[i].p_value + (modbus_address - reg_mdb_address) * 2;
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
#endif //REGS_DESCRIPTION_C
