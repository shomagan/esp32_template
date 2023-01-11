/**
 * @file    regs.c
 * @author  Shoma Gane <shomagan@gmail.com>
 * @author  Ayrat Girfanov <girfanov.ayrat@yandex.ru>
 * @ingroup regs
 * @version 0.1
 */

#ifndef REGS_C
#define REGS_C 1

/**
  * @defgroup regs
  * @brief Work with main vars regs
  */

#include "regs.h"
#include "regs_description.h"
#include "modbus.h"
#include "esp_log.h"
#include "common.h"
#include "mirror_storage.h"
#include <time.h>
#include <string.h>
#include "common.h"
#include "pwm_test.h"
#ifndef DEBUG
#define DEBUG           0   /** @warning DEBUG must be defined in CMake */
#endif
static const char *TAG = "regs_c";
main_vars_t regs_global;
main_vars_part_1_t regs_global_part1;
servo_control_part_t servo_control_part;
client_part_0_t client_part_0;
client_part_1_t client_part_1;
/**
 * @brief mutex for access to global regs
 * @ingroup regs
 */
semaphore_handle_t regs_access_mutex = NULL;
/**
 * @brief queue for write regs
 * @ingroup regs
 * @warning unused queue
 */
queue_handle_t regs_event_queue = NULL;
/**
 * @brief regs_global - global main struct in project
 * @ingroup regs
 */


static void * end_of_reg_addr(void * regs_global);
static void execute_main_command(u16 command);
static int regs_write_access(void * reg_address);
static int regs_write_value_check(temp_data_buffering_t * temp_data_buffering);
static int regs_read_access(void * byte_address);
static temp_data_buffering_t temp_data_buffering = {.data = {{0}},.byte_writed_flags = 0,.reg_address = NULL};
static int regs_hadle_sets(void * reg_address,regs_access_t reg,u16 reg_index);

static void execute_main_command(u16 command);

/**
 * @brief regs_init
 * @return
 */
int regs_init(){
    semaphore_create_binary(regs_access_mutex)
    return 0;
}
/**
 * @brief write new value in register
 * @param reg_address - in byte addressing
 * @param reg - struct for register access
 * @return  0 - OK, \n
 *          -1 - reg.flag error
 * @ingroup regs
 *
 * One writer or several reader \n
 * If we have writer dont read \n
 * If have reader dont write \n
 *
 * @warning special DO_CTRL register processing
 */
int regs_set(void * reg_address,regs_access_t reg){
    int result;
    result = 0;
    int writable_var_index = regs_write_access(reg_address);
    if (writable_var_index>=0){
        if (regs_fill_temp_buffer(reg_address,reg,&temp_data_buffering, writable_var_index)<0){

        }
        if (regs_check_temp_buffer(&temp_data_buffering, writable_var_index)>0){
            main_printf(TAG,"regs_set: regs_check_temp_buffer OK");
            if (regs_write_value_check(&temp_data_buffering)){
                semaphore_take(regs_access_mutex, portMAX_DELAY );{
                    memcpy(temp_data_buffering.reg_address, temp_data_buffering.data.bytes,
                           regs_size_in_byte(temp_data_buffering.type));
                }semaphore_release(regs_access_mutex);
            }else{
                result = -ILLEGAL_DATA_VALUE;
            }
            regs_clear_temp_buffer(&temp_data_buffering);
            regs_hadle_sets(reg_address, reg, (u16)writable_var_index);
        }
    }else{
        result = -ILLEGAL_DATA_ADDRESS;
    }
    return result;
}
/**
 * @brief regs_write_access
 * @param byte_address - byte address
 * @return more than 0 if enabled access, <0 if not 
 */
static int regs_write_access(void * reg_address){
    int res = -ILLEGAL_DATA_ADDRESS;
    int index = regs_description_get_index_by_address(reg_address);
    if (index>=0){
        if (regs_description_is_writable((u16)index)||
            (regs_description_is_credential((u16)index)==1 &&
             (regs_global.vars.permission & ENABLE_CREDENTIAL_FLAG))){
            res = index;
        }
    }else{
        res = -ILLEGAL_DATA_ADDRESS;
    }
    return res;
}
/**
 * @brief regs_write_value_check
 * @param temp_data_buffering - 
 * @return 
 */
static int regs_write_value_check(temp_data_buffering_t * temp_data_buffering){
    int result = 0;
    regs_template_t regs_template = {0};
    regs_template.ind = temp_data_buffering->index;
    if (regs_description_get_by_ind(&regs_template)>=0){
        u32 array_number = (u32)temp_data_buffering->reg_address - (u32)regs_template.p_value;
        array_number /= regs_size_in_byte(regs_template.type);
        result = 1;
        if (regs_template.p_max_value!=NULL){
            switch(regs_template.type){
            case U8_REGS_FLAG:
            {
                const u8 * max_value = (const void*)regs_template.p_max_value;
                if (temp_data_buffering->data.operand.op_u8 > max_value[array_number]){
                    temp_data_buffering->data.operand.op_u8 = max_value[array_number];
                }
            }
                break;
            case I8_REGS_FLAG:
            {
                const i8 * max_value = (const void*)regs_template.p_max_value;
                if (temp_data_buffering->data.operand.op_i8 > max_value[array_number]){
                    temp_data_buffering->data.operand.op_i8 = max_value[array_number];
                }
            }
                break;
            case U16_REGS_FLAG:
            {
                const u16 * max_value = (const void*)regs_template.p_max_value;
                if (temp_data_buffering->data.operand.op_u16 > max_value[array_number]){
                    temp_data_buffering->data.operand.op_u16 = max_value[array_number];
                }
            }
                break;
            case S16_REGS_FLAG:
            {
                const s16 * max_value = (const void*)regs_template.p_max_value;
                if (temp_data_buffering->data.operand.op_i16 > max_value[array_number]){
                    temp_data_buffering->data.operand.op_i16 = max_value[array_number];
                }
            }
                break;
            case TIME_REGS_FLAG:
            case U32_REGS_FLAG:
            {
                const u32 * max_value = (const void*)regs_template.p_max_value;
                if (temp_data_buffering->data.operand.op_u32 > max_value[array_number]){
                    temp_data_buffering->data.operand.op_u32 = max_value[array_number];
                }
            }
                break;
            case S32_REGS_FLAG:
            case INT_REGS_FLAG:
            {
                const s32 * max_value = (const void*)regs_template.p_max_value;
                if (temp_data_buffering->data.operand.op_s32 > max_value[array_number]){
                    temp_data_buffering->data.operand.op_s32 = max_value[array_number];
                }
            }
                break;
            case FLOAT_REGS_FLAG:
            {
                const float * max_value = (const void*)regs_template.p_max_value;
                if (temp_data_buffering->data.operand.op_f > max_value[array_number]){
                    temp_data_buffering->data.operand.op_f = max_value[array_number];
                }
            }
                break;
            case U64_REGS_FLAG:
            {
                const u64 * max_value = (const void*)regs_template.p_max_value;
                if (temp_data_buffering->data.operand.op_u64 > max_value[array_number]){
                    temp_data_buffering->data.operand.op_u64 = max_value[array_number];
                }
            }
                break;
            case S64_REGS_FLAG:
            {
                const s64 * max_value = (const void*)regs_template.p_max_value;
                if (temp_data_buffering->data.operand.op_s64 > max_value[array_number]){
                    temp_data_buffering->data.operand.op_s64 = max_value[array_number];
                }
            }
                break;
            case DOUBLE_REGS_FLAG:
            {
                const double * max_value = (const void*)regs_template.p_max_value;
                if (temp_data_buffering->data.operand.op_d > max_value[array_number]){
                    temp_data_buffering->data.operand.op_d = max_value[array_number];
                }
            }
                break;
            }

        }
        if (regs_template.p_min_value!=NULL){
            switch(regs_template.type){
            case U8_REGS_FLAG:
            {
                const u8 * min_value = (const void*)regs_template.p_min_value;
                if (temp_data_buffering->data.operand.op_u8 < min_value[array_number]){
                    temp_data_buffering->data.operand.op_u8 = min_value[array_number];
                }
            }
                break;
            case I8_REGS_FLAG:
            {
                const i8 * min_value = (const void*)regs_template.p_min_value;
                if (temp_data_buffering->data.operand.op_i8 < min_value[array_number]){
                    temp_data_buffering->data.operand.op_i8 = min_value[array_number];
                }
            }
                break;
            case U16_REGS_FLAG:
            {
                const u16 * min_value = (const void*)regs_template.p_min_value;
                if (temp_data_buffering->data.operand.op_u16 < min_value[array_number]){
                    temp_data_buffering->data.operand.op_u16 = min_value[array_number];
                }
            }
                break;
            case S16_REGS_FLAG:
            {
                const s16 * min_value = (const void*)regs_template.p_min_value;
                if (temp_data_buffering->data.operand.op_i16 < min_value[array_number]){
                    temp_data_buffering->data.operand.op_i16 = min_value[array_number];
                }
            }
                break;
            case TIME_REGS_FLAG:
            case U32_REGS_FLAG:
            {
                const u32 * min_value = (const void*)regs_template.p_min_value;
                if (temp_data_buffering->data.operand.op_u32 < min_value[array_number]){
                    temp_data_buffering->data.operand.op_u32 = min_value[array_number];
                }
            }
                break;
            case S32_REGS_FLAG:
            case INT_REGS_FLAG:
            {
                const s32 * min_value = (const void*)regs_template.p_min_value;
                if (temp_data_buffering->data.operand.op_s32 < min_value[array_number]){
                    temp_data_buffering->data.operand.op_s32 = min_value[array_number];
                }
            }
                break;
            case FLOAT_REGS_FLAG:
            {
                const float * min_value = (const void*)regs_template.p_min_value;
                if (temp_data_buffering->data.operand.op_f < min_value[array_number]){
                    temp_data_buffering->data.operand.op_f = min_value[array_number];
                }
            }
                break;
            case U64_REGS_FLAG:
            {
                const u64 * min_value = (const void*)regs_template.p_min_value;
                if (temp_data_buffering->data.operand.op_u64 < min_value[array_number]){
                    temp_data_buffering->data.operand.op_u64 = min_value[array_number];
                }
            }
                break;
            case S64_REGS_FLAG:
            {
                const s64 * min_value = (const void*)regs_template.p_min_value;
                if (temp_data_buffering->data.operand.op_s64 < min_value[array_number]){
                    temp_data_buffering->data.operand.op_s64 = min_value[array_number];
                }
            }
                break;
            case DOUBLE_REGS_FLAG:
            {
                const double * min_value = (const void*)regs_template.p_min_value;
                if (temp_data_buffering->data.operand.op_d < min_value[array_number]){
                    temp_data_buffering->data.operand.op_d = min_value[array_number];
                }
            }
                break;
            }
        }
        if (regs_template.p_value == regs_global.vars.wifi_name ||
            regs_template.p_value == regs_global.vars.wifi_password||
            regs_template.p_value == regs_global.vars.wifi_router_password||
            regs_template.p_value == regs_global.vars.wifi_router_name){
            if (is_ascii_symbol_or_digital((u8*)&temp_data_buffering->data.operand.op_u64, regs_size_in_byte(regs_template.type))==1){
                result = 1;
            }else{
                result = 0;
            }
        }
    }else{
        result = -1;
    }
    return result;
}
/**
 * @brief pull data from main vars to template reg
 * @param reg_address - in byte addressing
 * @param reg - pointer to value when we will set data
 * @return  0 - OK, \n
 *          -1 - reg.flag error
 * @ingroup regs
 * @warning in param reg you should set flag space
 * */
int regs_get(void * reg_address,regs_access_t* reg){
    int result;
    result = 0;
    reg->value.op_u64 = 0;
    if (regs_read_access(reg_address)==1){
        semaphore_take(regs_access_mutex, portMAX_DELAY);{
            memcpy(&reg->value.op_u64,reg_address,regs_size_in_byte(reg->flag));
        }semaphore_release(regs_access_mutex);
    }else{
        result = -ILLEGAL_DATA_ADDRESS;
    }

    return result;
}
/**
 * @brief regs_read_access
 * @param byte_address - byte address
 * @return 1 if enabled access, 0 if disabled , <0 if error occured
 */
static int regs_read_access(void * byte_address){
    int res = 0;
    int index = regs_description_get_index_by_address(byte_address);
    if (index>=0){
        res = 1;
    }else{
        res = -ILLEGAL_DATA_ADDRESS;
    }
    return res;
}

/**
 * @brief read vars in buffer
 * @param reg_address - in byte addressing
 * @param buffer_to - pointer to buffer for reading data
 * @param byte_numm - number of reading bytes
 * @return 0
 * @ingroup regs
 * @todo add asserts for byte_numm,buffer_to
 */
int regs_get_buffer(void * reg_address,u8* buffer_to,u16 byte_numm){
    int result;
    regs_access_t reg;
    reg.flag = U8_REGS_FLAG;
    result =0;
    for(u16 i=0; i<byte_numm; i++){
        result = regs_get((void*)((u32)reg_address+i),&reg);
        if(result!=0){
            break;
        }
        memcpy((u8*)(buffer_to+i),&reg.value.op_u8,1);
    }
    return result;
}

/**
 * @brief get register size in bytes
 * @param type - register type flag
 * @return register size in bytes
 * @ingroup regs
 */
u8 regs_size_in_byte(regs_flag_t type){
    u8 result = 0;
    switch(type){
    case U8_REGS_FLAG:
    case I8_REGS_FLAG:
        result = 1;
        break;
    case U16_REGS_FLAG:
    case S16_REGS_FLAG:
        result = 2;
        break;
    case U32_REGS_FLAG:
    case S32_REGS_FLAG:
    case INT_REGS_FLAG:
    case TIME_REGS_FLAG:
    case FLOAT_REGS_FLAG:
        result = 4;
        break;
    case U64_REGS_FLAG:
    case S64_REGS_FLAG:
    case DOUBLE_REGS_FLAG:
        result = 8;
        break;
    }
    return result;
}

/**
 * @brief write new values in register array
 * @param reg_address - first register address in byte addressing
 * @param buffer_from - pointer to reading buffer
 * @param byte_numm - number of bytes for writing
 * @return 0 - not changed, 1 - if value changed, less then zero if error
 * @ingroup regs
 * @todo add asserts for byte_numm,buffer_to
 */
int regs_set_buffer(void * reg_address,u8* buffer_from,u16 byte_numm){
    int result;
    regs_access_t reg;
    reg.flag = U8_REGS_FLAG;
    result =0;
    for(u16 i=0; i<byte_numm; i++){
        reg.value.op_u64 = 0;
        reg.value.op_u8 = *(u8*)(buffer_from+i);
        result = regs_set((void*)((u32)reg_address+i), reg);
        if(result!=0){
            break;
        }
    }
    return result;
}


/**
 * @brief perform an action or send a message to the regs_event_handler queue
 * @param reg_address - in byte addressing
 * @param reg - struct for register access
 * @return 0
 * @ingroup regs
 * @example rewrite rtc time or ethernet settings drive to write in peripherial hardware or change external setting
 */

static int regs_hadle_sets(void * reg_address,regs_access_t reg,u16 reg_index){
    int result;
    result = 0;
    void * end_addr = (void*)((u32)reg_address + regs_size_in_byte(reg.flag));
    if (regs_description_is_saved(reg_index)){/*regs saved in mirror*/
        main_printf(TAG,"\n register is saved %u reg_index", reg_index);
        regs_template_t reg_template;
        reg_template.ind = (u16)reg_index;
        if(regs_description_get_by_ind(&reg_template)==0){
            main_printf(TAG, "\n reg_template.saved_address %lu", reg_template.saved_address);
            mirror_access_write(&reg_template);
        }
    }else{
        main_printf(TAG,"\n register is not saved %u reg_index", reg_index);
    }
    /*special*/
    if ((end_addr == end_of_reg_addr(&regs_global.vars.ip))||
        (end_addr == end_of_reg_addr(&regs_global.vars.gate))||
        (end_addr == end_of_reg_addr(&regs_global.vars.netmask))){
        ESP_LOGI(TAG, "ip has changed");
    } else if (end_addr == end_of_reg_addr(&regs_global.vars.command)){
        execute_main_command(regs_global.vars.command);
    }else if(end_addr == end_of_reg_addr(&regs_global_part1.vars.test_pwm_value)){
        pwm_test_set(regs_global_part1.vars.test_pwm_value);
    }
    return result;
}



/**
 * @brief write register to mirror
 * @param reg - pointer to register
 * @return  0 - OK, \n
 *          -1 - reg description not find, \n
 *          -2 - register not saving
 * @ingroup regs
 * @warning in debug mode enabled checking after write to mirror
 */
int write_reg_to_mirror(void *reg){
    int result = 0;
    int index = regs_description_get_index_by_address(reg);
    if (index >= 0){
        regs_template_t reg_template = {0};
        reg_template.ind = (u16)index;
        if(regs_description_get_by_ind(&reg_template)==0){
            if(reg_template.property & SAVING){
                mirror_access_write(&reg_template);
            }else{
                result = -3;
            }
        }else {
            main_printf(TAG,"\nFailed %s:%d",__FILE__,__LINE__);
            result = -2;
        }
    }else{
        main_printf(TAG,"\nFailed %s:%d",__FILE__,__LINE__);
        result = -1;
    }
    return result;
}


/**
 * @brief restore register value from bkram
 * @param reg - pointer to register
 * @return  0 - OK, \n
 *          -1 - reg description not find, \n
 *          -2 - register not saving
 * @ingroup regs
 */
int read_reg_from_bkram(void *reg){
    int result = 0;
    int index = regs_description_get_index_by_address(reg);
    if (index >= 0){
        regs_template_t reg_template = {0};
        reg_template.ind = (u16)index;
        if(regs_description_get_by_ind(&reg_template)==0){
            if(reg_template.property & SAVING){
                mirror_access_read((u16)reg_template.saved_address, reg_template.p_value, (u8)reg_template.size_in_bytes);
            }else{
                result = -2;
            }
        }else {
            main_printf(TAG,"\nFailed %s:%d",__FILE__,__LINE__);
            result = -2;
        }
    }else{
        main_printf(TAG,"\nFailed %s:%d",__FILE__,__LINE__);
        result = -1;
    }
    return result;
}
/**
 * @brief return end of register address
 * @param reg - pointer to register
 * @return  end of register address, \n
 *          0 - reg description not find
 * @ingroup regs
 */
static void * end_of_reg_addr(void * reg){
    void * addr = NULL;
    regs_template_t reg_template = {0};
    int index = regs_description_get_index_by_address(reg);
    if(index >= 0){
        reg_template.ind = (u16)index;
        if (regs_description_get_by_ind(&reg_template)>=0){
            addr = reg_template.p_value + reg_template.size_in_bytes;
        }
    }
    return addr;
}

/**
 * @brief regs_of_bkram_and_have_def_value
 * @param reg_address
 * @return  1 if vars in saved space and have def value in description
 */
int regs_saved_and_have_def_value(const void * reg_address){
    int res = 0;
    regs_template_t regs_template;
    int index = regs_description_get_index_by_address(reg_address);
    if (index>=0){
        regs_template.ind = (u16)index;
        regs_description_get_by_ind(&regs_template);
        if (regs_template.property & SAVING) {
            if (regs_template.p_default) {    /*reg have default value*/
                res = 1;
            }
        }
    }
    return res;
}

/**
 * @brief execute command
 * @param command - command from command_list
 * @ingroup regs
 */
static void execute_main_command(u16 command){
    int res = 0;
    switch ((main_command_t)command){
    case SW_RESET_COMM:
        // add necessary actions before reset
        ESP_LOGI(TAG, "SW_RESET_COMM");
        ui32 prev_value = 0;
        main_printf(TAG,"task notify to prepare reset");
        task_notify_send(common_duty_task_handle,PREPARE_TO_RESET,&prev_value);
        break;
    case SAVE_ALL_RETAINS_COMM:
        ESP_LOGI(TAG, "SAVE_ALL_RETAINS_COMM");
        // add function
        break;
    case SET_DEFAULT_VALUES_COMM:
        ESP_LOGI(TAG, "SET_DEFAULT_VALUES_COMM");
        res = set_regs_def_values();
        if (res == 1){
            regs_access_t reg;
            reg.flag = U16_REGS_FLAG;
            reg.value.op_u16 = SW_RESET_COMM;
            regs_set(&regs_global.vars.command,reg);
        }
        break;

    case ENABLE_SPECIAL_HTTP_COMM:
        regs_global.vars.permission |= ENABLE_CREDENTIAL_FLAG;
        break;
    case DISABLE_SPECIAL_HTTP_COMM:
        regs_global.vars.permission &= ~ENABLE_CREDENTIAL_FLAG;
        break;
    case ENABLE_HTTP_COMM:
        regs_global.vars.permission |= ENABLE_HTTP_FLAG;
        break;
    case DISABLE_HTTP_COMM:
        regs_global.vars.permission &= ~ENABLE_HTTP_FLAG;
        break;
        break;
    default:
        break;
    }
}



#endif //REGS_C
