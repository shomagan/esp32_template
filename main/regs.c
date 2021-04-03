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
main_vars_t regs_global;



static void * end_of_reg_addr(void * regs_global);
static int end_of_saved_reg_addr(void * reg_end_address);
static void execute_main_command(u16 command);
static int regs_write_access(void * reg_address);
static int regs_write_value_check(void * byte_address,regs_access_t reg);
static int regs_read_access(void * byte_address);

static int regs_hadle_sets(void * reg_address,regs_access_t reg);

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
 * @return   1 - if value changed
 *           0 - OK, \n
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
    if (regs_write_access(reg_address)==1){
        if (regs_write_value_check(reg_address,reg)){
            semaphore_take(regs_access_mutex, portMAX_DELAY );{
                result = memcmp_not_equal(reg_address, (u8*)&reg.value.op_u64, regs_size_in_byte(reg.flag));
                memcpy(reg_address, &reg.value.op_u64, regs_size_in_byte(reg.flag));
            }
            semaphore_release(regs_access_mutex);
            regs_hadle_sets(reg_address, reg);
        }else{
            result = -ILLEGAL_DATA_VALUE;
        }
    }else{
        result = -ILLEGAL_DATA_ADDRESS;
    }
    return result;
}
/**
 * @brief regs_write_access
 * @param byte_address - byte address
 * @return 1 if enabled access, 0 if disabled , <0 if error occured
 */
static int regs_write_access(void * reg_address){
    int res = 0;
    int index = regs_description_get_index_by_address(reg_address);
    if (index>=0){
        if (regs_description_is_writeable((u16)index)||
                (regs_description_is_credential((u16)index)==1 && (regs_global.vars.permission & ENABLE_CREDENTIAL_FLAG))){
            res = 1;
        }
    }else{
        res = -ILLEGAL_DATA_ADDRESS;
    }
    return res;
}
static int regs_write_value_check(void * reg_address,regs_access_t reg){
    int result = 0;
    int index = regs_description_get_index_by_address(reg_address);
    if (index>=0){
        regs_template_t reg_template;
        reg_template.ind = (u16)index;
        if(regs_description_get_by_ind(&reg_template)==0){
            if (reg_template.p_value == regs_global.vars.wifi_name ||
                reg_template.p_value == regs_global.vars.wifi_password||
                reg_template.p_value == regs_global.vars.wifi_router_password||
                reg_template.p_value == regs_global.vars.wifi_router_name){
                if (is_ascii_symbol_or_digital((u8*)&reg.value.op_u64, regs_size_in_byte(reg.flag))==1){
                    result = 1;
                }else{
                    result = 0;
                }
            }
        }
    }
    if (reg_address == (void*)(&regs_global.vars.mdb_addr)){
        if ((reg.value.op_u16 >=1) && (reg.value.op_u16 <=247)){
            result = 1;
        }else{
            result = 0;
        }
    }else{
        result = 1;
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
        reg.value.op_u8 = *(u8*)(buffer_from+i);
        int set_result =  regs_set((void*)((u32)reg_address+i),reg);
        if(set_result < 0){
            result = set_result;
            break;
        }else if (set_result){
            result = set_result;
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

static int regs_hadle_sets(void * reg_address,regs_access_t reg){
    int result;
    result = 0;
    void * end_addr = (void*)((u32)reg_address + regs_size_in_byte(reg.flag));
    int ind = end_of_saved_reg_addr(end_addr);

    if (ind>=0){/*regs saved in mirror*/
        regs_template_t reg_template;
        reg_template.ind = (u16)ind;

        if(regs_description_get_by_ind(&reg_template)==0){

            if (mirror_access_write(&reg_template)==1){
                    /*reg was writed */
                /*if (regs_description_flag_check((u16)ind, CRITICAL_OPTION_FLAG)){
                    ui32 prev_value = 0;
                    main_printf(TAG,"task notify to prepare reset");
                    task_notify_send(common_duty_task_handle,PREPARE_TO_RESET,&prev_value);
                }*/

            }
        }
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
 * @brief return ind of savig address of register in bkram
 * @param reg_end_address - pointer to register
 * @return  ind of savig bkram address, \n
 *          -1 - reg description not find
 * @ingroup regs
 */
static int end_of_saved_reg_addr(void * reg_end_address){
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
