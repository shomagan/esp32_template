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
#include "main_init.h"
#include "regs_description.h"
#include "main.h"
#include "isimfw400_test.h"
#include "rtc.h"
#include "bkram_access.h"
#include "lwip.h"
#include "internal_flash.h"
#include "kernel_starter.h"
#include <time.h>
#include <string.h>
#include "lan.h"
#include "crc.h"
#include "internal_flash_test.h"
#ifndef DEBUG
#define DEBUG           0   /** @warning DEBUG must be defined in CMake */
#endif

/**
 * @brief mutex for access to global regs
 * @ingroup regs
 */
osMutexId regs_access_mutex;

/**
 * @brief queue for write regs
 * @ingroup regs
 * @warning unused queue
 */
osMessageQId regs_event_queue;

/**
 * @brief regs_global - global main struct in project
 * @ingroup regs
 */
main_vars_t regs_global;

static int regs_set_event(u32 reg_address,regs_access_t reg);
static u32 end_of_reg_addr(void *regs_global);
static int end_of_saved_reg_addr(u32 reg_end_address);
static u32 end_of_reg_element_addr(void *regs_global, u8 channel);
static void execute_main_command(u16 command);
static int regs_write_access(u32 reg_address);
static int regs_write_value_check(u32 byte_address,regs_access_t reg);
static int regs_read_access(u32 byte_address);
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
int regs_set(u32 reg_address,regs_access_t reg){
    int result;
    result = 0;

    if(reg_address < GLOBAL_UNION_SIZE){
        u8 * data = NULL;
        data = regs_global.bytes;
        if (regs_write_access(reg_address)==1){
            if (regs_write_value_check(reg_address,reg)){
                osMutexWait(regs_access_mutex, portMAX_DELAY );{
                    memcpy(&data[reg_address],&reg.value.op_u64,regs_size_in_byte(reg.flag));
                }
                osMutexRelease(regs_access_mutex);
                regs_set_event(reg_address, reg);
            }else{
                result = -ILLEGAL_DATA_VALUE;
            }
        }else{
            result = -ILLEGAL_DATA_ADDRESS;
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
static int regs_write_access(u32 byte_address){
    int res = 0;
    int index = regs_description_get_index_by_byte_address(byte_address);
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
static int regs_write_value_check(u32 byte_address,regs_access_t reg){
    int result = 0;
    if (byte_address == REGS_NUMBER(&regs_global.vars.mdb_addr)){
        if ((reg.value.op_u16 >=1) && (reg.value.op_u16 <=247)){
            result = 1;
        }else{
            result = 0;
        }
    }else if (byte_address == REGS_NUMBER(&regs_global.vars.mdb_revers)){
        if ((reg.value.op_u16 ==0) || (reg.value.op_u16 == 1)){
            result = 1;
        }else{
            result = 0;
        }
    }else if (byte_address == REGS_NUMBER(&regs_global.vars.mdb_shift)){
        if ((reg.value.op_u16 ==0) || (reg.value.op_u16 == 1)){
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
int regs_get(u32 reg_address,regs_access_t* reg){
    int result;
    result = 0;
    reg->value.op_u64 = 0;
    if(reg_address < GLOBAL_UNION_SIZE){
        u8 * data = NULL;
        data = regs_global.bytes;
        if (regs_read_access(reg_address)==1){
            osMutexWait(regs_access_mutex, portMAX_DELAY);{
                memcpy(&reg->value.op_u64,&data[reg_address],regs_size_in_byte(reg->flag));
            }osMutexRelease(regs_access_mutex);
        }else{
            result = -ILLEGAL_DATA_ADDRESS;
        }
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
static int regs_read_access(u32 byte_address){
    int res = 0;
    int index = regs_description_get_index_by_byte_address(byte_address);
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
int regs_get_buffer(u32 reg_address,u8* buffer_to,u16 byte_numm){
    int result;
    regs_access_t reg;
    reg.flag = U8_REGS_FLAG;
    result =0;
    for(u16 i=0; i<byte_numm; i++){
        result = regs_get(reg_address+i,&reg);
        if(result!=0){
            break;
        }
        *(u8*)(buffer_to+i) = reg.value.op_u8;
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
 * @return 0
 * @ingroup regs
 * @todo add asserts for byte_numm,buffer_to
 */
int regs_set_buffer(u32 reg_address,u8* buffer_from,u16 byte_numm){
    int result;
    regs_access_t reg;
    reg.flag = U8_REGS_FLAG;
    result =0;
    for(u16 i=0; i<byte_numm; i++){
        reg.value.op_u8 = *(u8*)(buffer_from+i);
        result = regs_set(reg_address+i,reg);
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
static int regs_set_event(u32 reg_address,regs_access_t reg){
    int result;
    result = 0;
    u32 msg = none_reg;
    u32 end_addr = reg_address + regs_size_in_byte(reg.flag);
    int ind = end_of_saved_reg_addr(end_addr);
    if (ind>=0){/*regs saved in mirror*/
        regs_template_t reg_template;
        reg_template.ind = (u16)ind;
        if(regs_description_get_by_ind(&reg_template)==0){
            mirror_access_write((u16)reg_template.saved_address, reg_template.p_value, (u16)reg_template.size_in_bytes);
        }
    }
    /*special*/
    if ((end_addr == end_of_reg_addr(&regs_global.vars.ip))||
               (end_addr == end_of_reg_addr(&regs_global.vars.gateaway))||
               (end_addr == end_of_reg_addr(&regs_global.vars.netmask))){
        msg = change_ip_reg;
    } else if (end_addr == end_of_reg_addr(&regs_global.vars.eth_speed)){
        msg = eth_speed_reg;
    } else if (end_addr == end_of_reg_addr(&regs_global.vars.eth_duplex)){
        msg = eth_duplex_reg;
    } else if (end_addr == end_of_reg_addr(&regs_global.vars.uart_debug_sets)){
        uart_setting_from_regs(DEBUG_UART_NUMBER);
    } else if (end_addr == end_of_reg_addr(&regs_global.vars.time_hms)){
        osSemaphoreWait(rtc_access_sem, portMAX_DELAY );
        msg = rtc_reg;
    } else if (end_addr == end_of_reg_addr(&regs_global.vars.unix_time_sec)){
        osSemaphoreWait(rtc_access_sem, portMAX_DELAY );
        msg = rtc_unix_reg;
    } else if (end_addr == end_of_reg_addr(&regs_global.vars.main_test_blocks)){
        msg = main_test_blocks_reg;
    } else if (end_addr == end_of_reg_addr(&regs_global.vars.command)){
        msg = command_reg;
    }

    if(msg){
        osMessagePut(regs_event_queue, msg, portMAX_DELAY) ;   //put msg to regs_event_queue
    }

    return result;
}

/**
 * @brief handler of regs event
 * @param pvParameters - unused
 * @ingroup regs
 */
void regs_event_handler (const void *pvParameters){
    (void)pvParameters;
    u32 msg;
    osEvent event;
    u8 channel = 0;
    main_test_handle = NULL;
    for (;;){
        event = osMessageGet(regs_event_queue, 10);
        if (event.status == osEventMessage){
            msg = event.value.v;
            channel = (u8)((msg & WRITE_REG_CH_MASK)>>16);
            switch(msg & WRITE_REG_MASK){
            case change_ip_reg:
                main_printf("ip has changed");
                ip4_addr_t ipaddr_temp;
                IP4_ADDR(&ipaddr_temp, regs_global.vars.ip[0], regs_global.vars.ip[1], regs_global.vars.ip[2], regs_global.vars.ip[3]);
                if(!ip4_addr_cmp(&ipaddr_temp,netif_ip4_addr(&gnetif))){
                    osDelay(100);
                    NVIC_SystemReset();
                }
                change_net_ip();
                break;
            case eth_speed_reg:
                write_reg_to_mirror(&regs_global.vars.eth_speed);
                //add function;
                break;
            case eth_duplex_reg:
                write_reg_to_mirror(&regs_global.vars.eth_duplex);
                //add function;
                break;
            case rtc_reg:
                if (rtc_set_time(&regs_global.vars.time_hms)==0){

                }else{
                    main_printf("rtc set time error");
                    led_os_error_on(RTC_SET_TIME_ERROR_MS);
                }
                osSemaphoreRelease(rtc_access_sem);
                break;
            case rtc_unix_reg:
            {   struct tm * tm_time_p;
                time_t timer;
                memcpy(&timer, &regs_global.vars.unix_time_sec, sizeof(time_t));
                tm_time_p = gmtime((void *)&timer);
                if(tm_time_p!=NULL){
                    regs_global.vars.time_hms.sec = (u8)tm_time_p->tm_sec;
                    regs_global.vars.time_hms.min = (u8)tm_time_p->tm_min;
                    regs_global.vars.time_hms.hour = (u8)tm_time_p->tm_hour;
                    regs_global.vars.time_hms.date = (u8)tm_time_p->tm_mday;
                    regs_global.vars.time_hms.month = (u8)tm_time_p->tm_mon + 1;
                    regs_global.vars.time_hms.year = (u8)(tm_time_p->tm_year-100);
                    regs_global.vars.time_hms.week_day = (u8)tm_time_p->tm_wday;
                    regs_global.vars.time_hms.year_day = (u16)tm_time_p->tm_yday;
                }
                if (rtc_set_time(&regs_global.vars.time_hms)==0){

                }else{
                    main_printf("rtc set time error");
                    led_os_error_on(RTC_SET_TIME_ERROR_MS);
                }
                osSemaphoreRelease(rtc_access_sem);
                break;
            }
            case main_test_blocks_reg:
#if MAIN_TEST_ENABLE
                if(main_test_handle == NULL){
                    osThreadDef(main_test_blocks,main_test, osPriorityHigh, 0, configMINIMAL_STACK_SIZE*6);
                    main_test_handle = osThreadCreate (osThread(main_test_blocks), NULL);
                }
#endif
                break;
            case command_reg:
                execute_main_command(regs_global.vars.command);
                break;
            default:
                break;
            }
        }
    }
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
    regs_template_t reg_template = {0};
    reg_template.guid = REGS_NUMBER(reg);
    if(regs_description_get_by_guid(&reg_template)!=0){
        main_printf("\nFailed %s:%d",__FILE__,__LINE__);
        led_os_error_on(REG_MIRROR_WRITE_ERROR_MS);
        result = -1;
    }else {
        if(reg_template.property & SAVING){
            mirror_access_write((u16)reg_template.saved_address, reg_template.p_value, (u16)reg_template.size_in_bytes);
        }else{
            result = -2;
        }
    }
    return result;
}

/**
 * @brief write element of register array to bkram
 * @param reg - pointer to register
 * @param element_num - registers array namber
 * @return  0 - OK, \n
 *          -1 - reg description not find, \n
 *          -2 - register not saving
 * @ingroup regs
 * @warning in debug mode enabled checking after write to bkram
 */
int write_reg_element_to_mirror(void *reg, u8 element_num){
    int result = 0;
    regs_template_t reg_template = {0};
    reg_template.guid = REGS_NUMBER(reg);
    u16 bkram_addr = 0;
    u8 len = 0;
    u8 shift = 0;
    if(regs_description_get_by_guid(&reg_template)){
        main_printf("Failed %s:%d\n",__FILE__,__LINE__);
        led_os_error_on(REG_MIRROR_WRITE_ERROR_MS);
        result = -1;
    }else{
        if(reg_template.property & SAVING){
            shift = element_num*regs_size_in_byte(reg_template.type);
            bkram_addr = (u16)reg_template.saved_address+shift;
            len = regs_size_in_byte(reg_template.type);
            task_enter_critical();
            mirror_access_write(bkram_addr, reg_template.p_value + shift, len);
            task_exit_critical();
        }else{
            result = -2;
        }
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
    regs_template_t reg_template = {0};
    reg_template.guid = REGS_NUMBER(reg);
    if(regs_description_get_by_guid(&reg_template)){
        main_printf("Failed %s:%d\n",__FILE__,__LINE__);
        led_os_error_on(REG_MIRROR_WRITE_ERROR_MS);
        result = -1;
    }else{
        if(reg_template.property & SAVING){
            task_enter_critical();
            mirror_access_read((u16)reg_template.saved_address, reg_template.p_value, (u8)reg_template.size_in_bytes);
            task_exit_critical();
        }else{
            result = -2;
        }
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
static u32 end_of_reg_addr(void *reg){
    u32 addr = 0;
    regs_template_t reg_template = {0};
    reg_template.guid = REGS_NUMBER(reg);
    if(regs_description_get_by_guid(&reg_template)!=0){
        return addr;
    }
    addr = REGS_NUMBER(reg + reg_template.size_in_bytes);
    return addr;
}

/**
 * @brief return ind of savig address of register in bkram
 * @param reg_end_address - pointer to register
 * @return  ind of savig bkram address, \n
 *          -1 - reg description not find
 * @ingroup regs
 */
static int end_of_saved_reg_addr(u32 reg_end_address){
    u16 i;
    int result = -1;
    regs_template_t regs_template;
    for (i=0 ; i<regs_global.vars.num_of_vars; i++){
        regs_template.ind = i;
        regs_description_get_by_ind(&regs_template);
        if ((regs_template.property & SAVING) &&
            (reg_end_address == (regs_template.guid + regs_template.size_in_bytes))){
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
int regs_saved_and_have_def_value(u32 reg_address){
    int res = 0;
    regs_template_t regs_template;
    for (u16 i=0 ; i< NUM_OF_SELF_VARS; i++){   /*check only self vars*/
        regs_template.ind = i;
        regs_description_get_by_ind(&regs_template);
        if (regs_template.property & SAVING) {
            if (regs_template.p_default) {    /*reg have default value*/
                if(reg_address >= (regs_template.guid & GUID_ADDRESS_MASK)){
                    if (reg_address < ((regs_template.guid & GUID_ADDRESS_MASK) + regs_template.size_in_bytes)){
                        res = 1;
                        break;
                    }
                }
            }
        }
    }
    return res;
}

/**
 * @brief return end of register element address
 * @param reg - pointer to register
 * @param channel - register element number
 * @return  end of register element address, \n
 *          0 - reg description not find
 * @ingroup regs
 */
static u32 end_of_reg_element_addr(void *reg, u8 channel){
    u32 addr = 0;
    regs_template_t reg_template = {0};
    reg_template.guid = REGS_NUMBER(reg);
    if(regs_description_get_by_guid(&reg_template)){
        return addr;
    }
    addr = REGS_NUMBER(reg + regs_size_in_byte(reg_template.type)*(channel+1));
    return addr;
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
        main_printf("SW_RESET_COMM");
        NVIC_SystemReset();
    case SAVE_ALL_RETAINS_COMM:
        main_printf("SAVE_ALL_RETAINS_COMM");
        // add function
        break;
    case SET_DEFAULT_VALUES_COMM:
        main_printf("SET_DEFAULT_VALUES_COMM");
        res = set_regs_def_values();
        if (res == 1){
            regs_access_t reg;
            reg.flag = U16_REGS_FLAG;
            reg.value.op_u16 = SW_RESET_COMM;
            regs_set(REGS_NUMBER(&regs_global.vars.command), reg);
        }
        break;
    case CHANGE_OS_COMM:
        main_printf("CHANGE_OS_COMM");
        if((regs_global.vars.current_os == FIRST_OS) && (check_os_crc(SECOND_OS))){
            write_comm_to_bkp(RUN_OS2_COMM);
            main_printf("os2 will be started");
            shutdown_os();
            start_os(SECOND_OS);
        }else if((regs_global.vars.current_os == SECOND_OS) && (check_os_crc(FIRST_OS))){
            write_comm_to_bkp(RUN_OS1_COMM);
            main_printf("os1 will be started");
            shutdown_os();
            start_os(FIRST_OS);
        }
        break;
    case RUN_OS1_COMM:
        main_printf("RUN_OS1_COMM");
        if((regs_global.vars.current_os == SECOND_OS) && (check_os_crc(FIRST_OS))){
            write_comm_to_bkp(command);
            main_printf("os1 will be started");
            shutdown_os();
            start_os(FIRST_OS);
        }
        break;
    case RUN_OS2_COMM:
        main_printf("RUN_OS2_COMM");
        if((regs_global.vars.current_os == FIRST_OS) && (check_os_crc(SECOND_OS))){
            write_comm_to_bkp(command);
            main_printf("os2 will be started");
            shutdown_os();
            start_os(SECOND_OS);
        }
        break;
    case CONFIRM_OS_COMM:
        main_printf("CONFIRM_OS_COMM");
        if(regs_global.vars.current_os == FIRST_OS){
            set_main_os(FIRST_OS);
        }else if(regs_global.vars.current_os == SECOND_OS){
            set_main_os(SECOND_OS);
        }
        break;
    case SET_MAIN_AND_RUN_OS1_COMM:
        main_printf("SET_MAIN_AND_RUN_OS1_COMM");
        if((regs_global.vars.current_os == SECOND_OS) && (check_os_crc(FIRST_OS))){
            write_comm_to_bkp(command);
            main_printf("os1 will be started");
            shutdown_os();
            start_os(FIRST_OS);
        }
        break;
    case SET_MAIN_AND_RUN_OS2_COMM:
        main_printf("SET_MAIN_AND_RUN_OS2_COMM");
        if((regs_global.vars.current_os == FIRST_OS) && (check_os_crc(SECOND_OS))){
            write_comm_to_bkp(command);
            main_printf("os2 will be started");
            shutdown_os();
            start_os(SECOND_OS);
        }
        break;
    case REMOTE_UPDATE_ENABLE_COMM:
        main_printf("REMOTE_UPDATE_ENABLE_COMM");
        osMutexWait(regs_access_mutex, portMAX_DELAY );
        regs_global.vars.last_reset |= BOOT_KEY_REMOTE_UPDATE_ENABLE;      // set virtual boot_key_jumper
        osMutexRelease(regs_access_mutex);
        break;
    case REMOTE_UPDATE_DISABLE_COMM:
        main_printf("REMOTE_UPDATE_DISABLE_COMM");
        osMutexWait(regs_access_mutex, portMAX_DELAY );
        regs_global.vars.last_reset &= ~BOOT_KEY_REMOTE_UPDATE_ENABLE;      // remove virtual boot_key_jumper
        osMutexRelease(regs_access_mutex);
        break;
    case LAN_RESET_COMM:
        main_printf("LAN_RESET_COMM");
        osSignalSet(ethernet_if_control_id,PHY_RESET_SIGNAL);
        break;
    case MAKE_TEST_COMM:
        {
            main_test_type_t test_type = CHECK_TEST;
            internal_flash_test(&test_type);
            regs_global.vars.main_test_result = 1;
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
    case SAVE_CREDENTIAL_COMM:
        internal_flash_credential_save_to_flash();
        break;
    default:
        break;
    }
}

/**
 * @brief save command to bkram
 * @param command - command from command_list
 * @return  true - OK, \n
 *          false - ERROR
 * @ingroup regs
 *
 * Using in OS control functions
 */
u8 write_comm_to_bkp (main_command_t command){
    u8 ret = TRUE;
    u32 temp;
    while(hrtc.State != HAL_RTC_STATE_READY){
        osDelay(1);
    }
    hrtc.State = HAL_RTC_STATE_BUSY;
    __HAL_LOCK(&hrtc);
    HAL_PWR_EnableBkUpAccess();
    __HAL_RTC_WRITEPROTECTION_DISABLE(&hrtc);
    RTC->BKP0R = command;
    temp = RTC->BKP0R;
    __HAL_RTC_WRITEPROTECTION_ENABLE(&hrtc);
    __HAL_UNLOCK(&hrtc);
    hrtc.State = HAL_RTC_STATE_READY;
    if(temp != (u32)command){
        ret = FALSE;
        main_error_message("\nFailed %s:%d\n",__FILE__,__LINE__);
    }
    return ret;
}

#endif //REGS_C
