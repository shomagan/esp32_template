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
  * @brief Work with vars regs goal is to have an access to internal registers for outcome interface such as modbus, http, mqtt etc.
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
#include "sleep_control.h"
#include "wireless_control.h"
#include "feeder.h"
#ifndef DEBUG
#define DEBUG           0   /** @warning DEBUG must be defined in CMake */
#endif
static const char *TAG = "regs_c";
/*global objects mapped in modbus and http memory*/
regs_main_t regs_main;
/*pointers to be used as a reference to global registers
usef in main regs_description structure */
/*#generator_use_description {"regs_pointers":"start_struct"}*/
main_vars_t * const regs_global = &regs_main.regs_global; //!< "main_vars_t"
touch_regs_t * const touch_regs = &regs_main.touch_regs; //!< "touch_regs_t"
servo_control_part_t * const servo_control_part = &regs_main.servo_control_part; //!< "servo_control_part_t"
di_control_t * const di_control = &regs_main.di_control; //!< "di_control_t"
sync_time_regs_t * const sync_time_regs = &regs_main.sync_time_regs; //!< "sync_time_regs_t"
sr04_reg_t * const sr04_reg = &regs_main.sr04_reg; //!< "sr04_reg_t"
feeder_reg_t * const feeder_reg = &regs_main.feeder_reg; //!< "feeder_reg_t"
polisher_reg_t * const polisher_reg = &regs_main.polisher_reg; //!< "polisher_reg_t"
test_int_reg_t * const test_int_reg = &regs_main.test_int_reg; //!< "test_int_reg_t"
morse_reg_t * const morse_reg = &regs_main.morse_reg; //!< "morse_reg_t"
battery_state_reg_t * const battery_state_reg = &regs_main.battery_state_reg; //!< "battery_state_reg_t"
scd41_reg_t * const scd41_reg = &regs_main.scd41_reg; //!< "scd41_reg_t"
client_part_0_t * const client_part_0 = &regs_main.client_part_0; //!< "client_part_0_t"
sync_time_client_t * const sync_time_client = &regs_main.sync_time_client; //!< "sync_time_client_t"
client_part_1_t * const client_part_1 = &regs_main.client_part_1; //!< "client_part_1_t"
sync_data_client_t * const sync_time_regs_from_client = &regs_main.sync_time_regs_from_client; //!< "sync_data_client_t"
sr04_reg_client_t * const sr04_reg_client = &regs_main.sr04_reg_client; //!< "sr04_reg_client_t"
/*#generator_use_description {"regs_pointers":"end_struct"}*/
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
static int regs_write_access(regs_template_t *regs_template);
static int regs_write_value_check(temp_data_buffering_t * temp_data_buffering, regs_template_t *regs_template);
static temp_data_buffering_t temp_data_buffering = {.data = {{0}},.byte_writed_flags = 0,.reg_address = NULL};
static int regs_hadle_sets(regs_access_t reg, regs_template_t * regs_template);
static void execute_main_command(u16 command);
const char * const global_space_name = "GLOBAL_SPACE";

const regs_description_list_t regs_table_global = {
    .description = regs_description_global,
    .num_of_regs = NUM_OF_SELF_VARS,
    .table_version = &def_table_version,
    .space_name = global_space_name,
    .saved_regs_buffer = global_vars_mirror,
    .saved_regs_buffer_size = INTERNAL_FLASH_MIRROR_ITEM_SIZE
};

int regs_init() {
   semaphore_create_binary(regs_access_mutex);
   return regs_description_list_add_new(regs_table_global);
}

int regs_set(void *reg_address, regs_access_t reg) {
   int result;
   result = 0;
   regs_template_t regs_template;
   regs_template.p_value = reg_address;
   result = regs_write_access(&regs_template); /*regs template filled */
   if (result > 0) {
      if (regs_fill_temp_buffer(&regs_template, reg, &temp_data_buffering) < 0) {
         result = -ILLEGAL_DATA_VALUE;
      }
      if (regs_check_temp_buffer(&temp_data_buffering, &regs_template) > 0) {
         main_printf(TAG, "regs_set: regs_check_temp_buffer OK");
         if (regs_write_value_check(&temp_data_buffering, &regs_template) > 0) {
            semaphore_take(regs_access_mutex, portMAX_DELAY);
            {
               memcpy(temp_data_buffering.reg_address, temp_data_buffering.data.bytes,
                      regs_size_in_byte(temp_data_buffering.type));
            }
            semaphore_release(regs_access_mutex);
         } else {
            result = -ILLEGAL_DATA_VALUE;
         }
         regs_clear_temp_buffer(&temp_data_buffering);
         regs_hadle_sets(reg, &regs_template);
      }
   } else {
      main_error_message(TAG, "regs_set: no write access");
      result = -ILLEGAL_DATA_ADDRESS;
   }
   return result;
}

int regs_write_internal(void *reg_address, regs_access_t reg) {
   int result = 0;
   semaphore_take(regs_access_mutex, portMAX_DELAY);
   {
      memcpy(reg_address, &reg.value.op_u64, regs_size_in_byte(reg.flag));
   }
   semaphore_release(regs_access_mutex);
   return result;
}

static int regs_write_access(regs_template_t *regs_template) {
   int res = -ILLEGAL_DATA_ADDRESS;
   if (regs_template != NULL) {
      if (regs_description_get_by_address(regs_template) >= 0) {
         if (regs_description_is_writable(regs_template) ||
             (regs_description_is_credential(regs_template) == 1 && (regs_global->vars.permission & ENABLE_CREDENTIAL_FLAG))) {
            res = TRUE;
         }
      }
   }
   return res;
}

static int regs_write_value_check(temp_data_buffering_t * temp_data_buffering, regs_template_t *regs_template){
    int result = 0;
    if (regs_template != NULL && regs_template->p_value != NULL && temp_data_buffering != NULL){
        u32 array_number = (u32)temp_data_buffering->reg_address - (u32)regs_template->p_value;
        array_number /= regs_size_in_byte(regs_template->type);
        result = 1;
        if (regs_template->p_max_value!=NULL){
            switch(regs_template->type){
            case U8_REGS_FLAG:
            {
                const u8 * max_value = (const void*)regs_template->p_max_value;
                if (temp_data_buffering->data.operand.op_u8 > max_value[array_number]){
                    temp_data_buffering->data.operand.op_u8 = max_value[array_number];
                }
            }
                break;
            case I8_REGS_FLAG:
            {
                const i8 * max_value = (const void*)regs_template->p_max_value;
                if (temp_data_buffering->data.operand.op_i8 > max_value[array_number]){
                    temp_data_buffering->data.operand.op_i8 = max_value[array_number];
                }
            }
                break;
            case U16_REGS_FLAG:
            {
                const u16 * max_value = (const void*)regs_template->p_max_value;
                if (temp_data_buffering->data.operand.op_u16 > max_value[array_number]){
                    temp_data_buffering->data.operand.op_u16 = max_value[array_number];
                }
            }
                break;
            case S16_REGS_FLAG:
            {
                const s16 * max_value = (const void*)regs_template->p_max_value;
                if (temp_data_buffering->data.operand.op_i16 > max_value[array_number]){
                    temp_data_buffering->data.operand.op_i16 = max_value[array_number];
                }
            }
                break;
            case TIME_REGS_FLAG:
            case U32_REGS_FLAG:
            {
                const u32 * max_value = (const void*)regs_template->p_max_value;
                if (temp_data_buffering->data.operand.op_u32 > max_value[array_number]){
                    temp_data_buffering->data.operand.op_u32 = max_value[array_number];
                }
            }
                break;
            case S32_REGS_FLAG:
            case INT_REGS_FLAG:
            {
                const s32 * max_value = (const void*)regs_template->p_max_value;
                if (temp_data_buffering->data.operand.op_s32 > max_value[array_number]){
                    temp_data_buffering->data.operand.op_s32 = max_value[array_number];
                }
            }
                break;
            case FLOAT_REGS_FLAG:
            {
                const float * max_value = (const void*)regs_template->p_max_value;
                if (temp_data_buffering->data.operand.op_f > max_value[array_number]){
                    temp_data_buffering->data.operand.op_f = max_value[array_number];
                }
            }
                break;
            case U64_REGS_FLAG:
            {
                const u64 * max_value = (const void*)regs_template->p_max_value;
                if (temp_data_buffering->data.operand.op_u64 > max_value[array_number]){
                    temp_data_buffering->data.operand.op_u64 = max_value[array_number];
                }
            }
                break;
            case S64_REGS_FLAG:
            {
                const s64 * max_value = (const void*)regs_template->p_max_value;
                if (temp_data_buffering->data.operand.op_s64 > max_value[array_number]){
                    temp_data_buffering->data.operand.op_s64 = max_value[array_number];
                }
            }
                break;
            case DOUBLE_REGS_FLAG:
            {
                const double * max_value = (const void*)regs_template->p_max_value;
                if (temp_data_buffering->data.operand.op_d > max_value[array_number]){
                    temp_data_buffering->data.operand.op_d = max_value[array_number];
                }
            }
                break;
            }

        }
        if (regs_template->p_min_value!=NULL){
            switch(regs_template->type){
            case U8_REGS_FLAG:
            {
                const u8 * min_value = (const void*)regs_template->p_min_value;
                if (temp_data_buffering->data.operand.op_u8 < min_value[array_number]){
                    temp_data_buffering->data.operand.op_u8 = min_value[array_number];
                }
            }
                break;
            case I8_REGS_FLAG:
            {
                const i8 * min_value = (const void*)regs_template->p_min_value;
                if (temp_data_buffering->data.operand.op_i8 < min_value[array_number]){
                    temp_data_buffering->data.operand.op_i8 = min_value[array_number];
                }
            }
                break;
            case U16_REGS_FLAG:
            {
                const u16 * min_value = (const void*)regs_template->p_min_value;
                if (temp_data_buffering->data.operand.op_u16 < min_value[array_number]){
                    temp_data_buffering->data.operand.op_u16 = min_value[array_number];
                }
            }
                break;
            case S16_REGS_FLAG:
            {
                const s16 * min_value = (const void*)regs_template->p_min_value;
                if (temp_data_buffering->data.operand.op_i16 < min_value[array_number]){
                    temp_data_buffering->data.operand.op_i16 = min_value[array_number];
                }
            }
                break;
            case TIME_REGS_FLAG:
            case U32_REGS_FLAG:
            {
                const u32 * min_value = (const void*)regs_template->p_min_value;
                if (temp_data_buffering->data.operand.op_u32 < min_value[array_number]){
                    temp_data_buffering->data.operand.op_u32 = min_value[array_number];
                }
            }
                break;
            case S32_REGS_FLAG:
            case INT_REGS_FLAG:
            {
                const s32 * min_value = (const void*)regs_template->p_min_value;
                if (temp_data_buffering->data.operand.op_s32 < min_value[array_number]){
                    temp_data_buffering->data.operand.op_s32 = min_value[array_number];
                }
            }
                break;
            case FLOAT_REGS_FLAG:
            {
                const float * min_value = (const void*)regs_template->p_min_value;
                if (temp_data_buffering->data.operand.op_f < min_value[array_number]){
                    temp_data_buffering->data.operand.op_f = min_value[array_number];
                }
            }
                break;
            case U64_REGS_FLAG:
            {
                const u64 * min_value = (const void*)regs_template->p_min_value;
                if (temp_data_buffering->data.operand.op_u64 < min_value[array_number]){
                    temp_data_buffering->data.operand.op_u64 = min_value[array_number];
                }
            }
                break;
            case S64_REGS_FLAG:
            {
                const s64 * min_value = (const void*)regs_template->p_min_value;
                if (temp_data_buffering->data.operand.op_s64 < min_value[array_number]){
                    temp_data_buffering->data.operand.op_s64 = min_value[array_number];
                }
            }
                break;
            case DOUBLE_REGS_FLAG:
            {
                const double * min_value = (const void*)regs_template->p_min_value;
                if (temp_data_buffering->data.operand.op_d < min_value[array_number]){
                    temp_data_buffering->data.operand.op_d = min_value[array_number];
                }
            }
                break;
            }
        }
        if (regs_template->p_value == regs_global->vars.wifi_name ||
            regs_template->p_value == regs_global->vars.wifi_password||
            regs_template->p_value == regs_global->vars.wifi_router_password||
            regs_template->p_value == regs_global->vars.wifi_router_name){
            if (is_ascii_symbol_or_digital((u8*)&temp_data_buffering->data.operand.op_u64, regs_size_in_byte(regs_template->type))==1){
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

void regs_copy_safe(void *reg_to, void *reg_from, u32 size) {
   semaphore_take(regs_access_mutex, portMAX_DELAY);
   {
      memcpy(reg_to, reg_from, size);
   }
   semaphore_release(regs_access_mutex);
}

int regs_get(void *reg_address, regs_access_t *reg) {
   int result = -1;
   if(reg != NULL && reg_address != NULL){
      reg->value.op_u64 = 0;
      semaphore_take(regs_access_mutex, portMAX_DELAY);
      memcpy(&reg->value.op_u64, reg_address, regs_size_in_byte(reg->flag));
      semaphore_release(regs_access_mutex);
   }
   return result;
}

int regs_get_buffer(void * reg_address,u8* buffer_to,u16 byte_num){
    int result = -1;
    if(reg_address != NULL && buffer_to != NULL && byte_num != 0){
        memcpy(buffer_to,reg_address,byte_num);

        result = 0;
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
 *    only to be used by connection interfaces (modbus, http etc)
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
static int regs_hadle_sets(regs_access_t reg, regs_template_t * regs_template){
   int result;
   result = 0;
   if(regs_template != NULL || regs_template->p_value != NULL){
      void * end_addr = (void*)((u32)regs_template->p_value + regs_size_in_byte(reg.flag));
      if (regs_description_flag_check(regs_template, SAVING)){/*regs saved in mirror*/
         main_printf(TAG,"\n register is saved %u reg_index", reg_index);
         main_printf(TAG, "\n regs_template.saved_address %lu", regs_template.saved_address);
         mirror_access_write(&regs_template);
      }else{
         main_printf(TAG,"\n register is not saved %u reg_index", reg_index);
      }
      /*special - hmmm*/
      if ((end_addr == end_of_reg_addr(&regs_global->vars.ip))||
         (end_addr == end_of_reg_addr(&regs_global->vars.gate))||
         (end_addr == end_of_reg_addr(&regs_global->vars.netmask))){
         ESP_LOGI(TAG, "ip has changed");
      } else if (end_addr == end_of_reg_addr(&regs_global->vars.command)){
         execute_main_command(regs_global->vars.command);
      }else if(end_addr == end_of_reg_addr(&touch_regs->vars.test_pwm_value)){
         pwm_test_set(touch_regs->vars.test_pwm_value);
      }
      return result;

   }
}

int write_reg_to_mirror(void *p_to_value) {
   int result = 0;
   regs_template_t regs_template = { 0 };
   regs_template.p_value = p_to_value;
   result = regs_description_get_by_address(&regs_template);
   if (result >= 0) {
      mirror_access_write(&regs_template);
   } else {
      main_printf(TAG, "\nFailed %s:%d", __FILE__, __LINE__);
      result = -1;
   }
   return result;
}
int read_reg_from_bkram(void *p_to_value) {
   int result = 0;
   regs_template_t regs_template = { 0 };
   regs_template.p_value = p_to_value;
   result = regs_description_get_by_address(&regs_template);
   if (result >= 0) {
      if (regs_template.property & SAVING) {
         mirror_access_read((u16)regs_template.saved_address, regs_template.p_value, (u8)regs_template.size_in_bytes);
      }
   } else {
      main_printf(TAG, "\nFailed %s:%d", __FILE__, __LINE__);
      result = -1;
   }
   return result;
}

static void *end_of_reg_addr(void *p_to_value) {
   void *addr = NULL;
   regs_template_t regs_template = { 0 };
   regs_template.p_value = p_to_value;
   int result = regs_description_get_by_address(&regs_template);
   if (result >= 0) {
      addr = regs_template.p_value + regs_template.size_in_bytes;
   }
   return addr;
}

int regs_saved_and_have_def_value(const void *p_to_value) {
   int res = 0;
   regs_template_t regs_template = { 0 };
   regs_template.p_value = (void *)p_to_value;
   res = regs_description_get_by_address(&regs_template);
   if (res >= 0) {
      if (regs_template.property & SAVING) {
         if (regs_template.p_default) { /*reg have default value*/
            res = 1;
         }
      }
   }
   return res;
}

static void execute_main_command(u16 command){
    int res = 0;
    uint32_t prev_value = 0;
    main_printf(TAG,"execute a command %u",command);
    switch ((main_command_t)command){
    case SW_RESET_COMM:
        // add necessary actions before reset
        ESP_LOGI(TAG, "SW_RESET_COMM");
        main_printf(TAG,"task notify to prepare reset");
        task_notify_send(common_duty_task_handle,PREPARE_TO_RESET,&prev_value);
        break;
    case SAVE_ALL_RETAINS_COMM:
        ESP_LOGI(TAG, "SAVE_ALL_RETAINS_COMM");
        // add function
        break;
    case SET_DEFAULT_VALUES_COMM:
        ESP_LOGI(TAG, "SET_DEFAULT_VALUES_COMM");
        res = set_regs_def_values(regs_global->vars.table_id);
        if (res == 1){
            main_printf(TAG,"task notify to prepare reset");
            task_notify_send(common_duty_task_handle,PREPARE_TO_RESET,&prev_value);
        }
        break;
    case ENABLE_SPECIAL_HTTP_COMM:
        regs_global->vars.permission |= ENABLE_CREDENTIAL_FLAG;
        break;
    case DISABLE_SPECIAL_HTTP_COMM:
        regs_global->vars.permission &= ~ENABLE_CREDENTIAL_FLAG;
        break;
    case ENABLE_HTTP_COMM:
        regs_global->vars.permission |= ENABLE_HTTP_FLAG;
        break;
    case DISABLE_HTTP_COMM:
        regs_global->vars.permission &= ~ENABLE_HTTP_FLAG;
        break;
    case DEEP_SLEEP_FOR_120_SEC_COMM:
        task_notify_send(sleep_control_handle_id,SLEEP_TASK_DEEP_SLEEP_FOR_120_SEC,&prev_value);
        break;
    case DEEP_SLEEP_FOR_N_SEC_COMM:
    {
        u32 signal = (u32)SLEEP_TASK_DEEP_SLEEP_FOR_N_SEC | (((u32)regs_global->vars.sleep_time << 16u)&0xffff0000);
        task_notify_send(sleep_control_handle_id, signal, &prev_value);
        break;
    }
    case DEEP_SLEEP_COMM:
        task_notify_send(sleep_control_handle_id,SLEEP_TASK_DEEP_SLEEP,&prev_value);
        break;
    case RESET_WIFI_FOR_120_SEC_COMM:
        task_notify_send(wireless_control_handle_id,WIRELESS_TASK_RESET_WIFI_FOR_120_SEC,&prev_value);
        break;
    case FEEDER_TASK_ONE_FEED_COMM:
        task_notify_send(feeder_handle_id,FEEDER_TASK_ONE_FEED_COMM,&prev_value);
        break;
    default:
        break;
    }
}
#endif //REGS_C
