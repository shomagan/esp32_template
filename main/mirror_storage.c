/**
 * @file mirror_storage.c
 * @author Shoma Gane <shomagan@gmail.com>
 *         Ayrat Girfanov <girfanov.ayrat@yandex.ru>
 * @defgroup ../main/
 * @ingroup ../main/
 * @version 0.1 
 * @brief  TODO!!! write brief in 
 */
/*
 * Copyright (c) 2018 Snema Service
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 * 3. The name of the author may not be used to endorse or promote products
 *    derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT
 * SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT
 * OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING
 * IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY
 * OF SUCH DAMAGE.
 *
 * This file is part of the sofi PLC.
 *
 * Author: Shoma Gane <shomagan@gmail.com>
 *         Ayrat Girfanov <girfanov.ayrat@yandex.ru>
 */
#ifndef MIRROR_STORAGE_C
#define MIRROR_STORAGE_C 1
#include "mirror_storage.h"
#include "nvs_flash.h"
#include "crc.h"
#include "esp_log.h"
#include <string.h>
#include "common.h"
static const char *TAG = "mirror_storage";
static const char *mirror_key = "mirror_key";
semaphore_handle_t global_vars_mirror_mutex = NULL;
u8 global_vars_mirror[INTERNAL_FLASH_MIRROR_ITEM_SIZE];
int mirror_storage_init(){
    int res = 0;
    semaphore_create_binary(global_vars_mirror_mutex)
    return res;
}
/**
 * @brief size of bkram
 * @return size bkram space availble in byte
 * @ingroup bkram
 * */
u32 mirror_access_get_size(){
    return (INTERNAL_FLASH_MIRROR_ITEM_SIZE);
}

/**
 * @brief write data to bkram
 * @param address - bkram start address
 * @param data - pointer to data
 * @param size - data size in bytes
 * @return  1 - writed
 *          0 - successfully, \n
 *          -1 -error
 * @ingroup bkram
 */
int mirror_access_write(regs_template_t * regs_template){
    int res = 0;
    if(mirror_space_is_changing((u16)regs_template->saved_address,regs_template->p_value,regs_template->size_in_bytes) == 1){
        main_printf(TAG, "mirror_access_write: mirror_space_is_changing");
        if(regs_template->saved_address+regs_template->size_in_bytes<= mirror_access_get_size()){
            if (global_vars_mirror_mutex!=NULL){
                semaphore_take(global_vars_mirror_mutex, portMAX_DELAY );
            }
            memcpy(&global_vars_mirror[regs_template->saved_address],regs_template->p_value,regs_template->size_in_bytes);
            if (global_vars_mirror_mutex!=NULL){
                semaphore_release(global_vars_mirror_mutex);
            }
            res = 1;
        }else{
            res = -1;
        }
        mirror_recalc_crc();
        if (regs_description_flag_check(regs_template->ind, SAVING)){
            main_printf(TAG,"set ASYNC_INIT_SET_VALUE_FROM_BKRAM_TO_FLASH");
            regs_global.vars.async_flags |= ASYNC_INIT_SET_VALUE_FROM_BKRAM_TO_FLASH;
        }
    }
    return  res;
}
/**
 * @brief bkram_space_is_changing
 * @param address
 * @param data
 * @param size
 * @return 1 if change 0 - not, less then zero if error
 */
int mirror_space_is_changing(u16 address,const u8* data,u16 size){
    int res = 0;
    if(address+size <= mirror_access_get_size()){
        u16 i = 0;
        if (global_vars_mirror_mutex!=NULL){
            semaphore_take(global_vars_mirror_mutex, portMAX_DELAY );
        }
        for(i = 0; i < size; i++){
            if (global_vars_mirror[address+i] != data[i]){
                res = 1;
                break;
            }
        }
        if (global_vars_mirror_mutex!=NULL){
            semaphore_release(global_vars_mirror_mutex);
        }
    }else{
        res = -1;
    }
    return res;
}

/**
 * @brief read data from bkram
 * @param address - bkram start address
 * @param data - pointer to where the read data will be written
 * @param size - data size in bytes
 * @return  0 - successfully, \n
 *          -1 - out of bkram address range
 * @ingroup bkram
 */
int mirror_access_read(u16 address,u8* data,u16 size){
    if(address+size <= mirror_access_get_size()){
        if (global_vars_mirror_mutex!=NULL){
            semaphore_take(global_vars_mirror_mutex, portMAX_DELAY );
        }
        for(u16 i = 0; i < size; i++){
            data[i] = global_vars_mirror[address+i];
        }
        if (global_vars_mirror_mutex!=NULL){
            semaphore_release(global_vars_mirror_mutex);
        }
        return 0;
    }else{
        return  -1;
    }
}
/**
 * @brief bkram_access_recalc_crc calculate crc and write to bkram(last 4 byte)
 * @return
 */
int mirror_recalc_crc(void){
    if (global_vars_mirror_mutex!=NULL){
        semaphore_take(global_vars_mirror_mutex, portMAX_DELAY );
    }
    u32 crc = calc_crc_for_u32((void*)&global_vars_mirror[0], (INTERNAL_FLASH_MIRROR_ITEM_SIZE-4)/4);
    for(u16 i = 0; i < sizeof(u32); i++){
        global_vars_mirror[INTERNAL_FLASH_MIRROR_ITEM_SIZE-sizeof(u32)+i] = (u8)(crc>>(i*8));
    }
    if (global_vars_mirror_mutex!=NULL){
        semaphore_release(global_vars_mirror_mutex);
    }
    return 0;
}
/**
 * @brief internal_flash_save_bkram_to_flash
 * @return zero if bkram saved to flash succed
 */
int internal_flash_save_mirror_to_flash(void){
    int res = 0;
    if (global_vars_mirror_mutex!=NULL){
        semaphore_take(global_vars_mirror_mutex, portMAX_DELAY );
    }
    {
        regs_global.vars.flash_write_number = (regs_global.vars.flash_write_number<0xffffffff)?(regs_global.vars.flash_write_number+1):regs_global.vars.flash_write_number;
        nvs_handle_t my_handle;
        res = nvs_open("storage", NVS_READWRITE, &my_handle);
        if (res == ESP_OK) {
            size_t len = INTERNAL_FLASH_MIRROR_ITEM_SIZE;
            u8 temp_mirror[INTERNAL_FLASH_MIRROR_ITEM_SIZE];
            res = nvs_get_blob(my_handle,mirror_key,temp_mirror,&len);
            if(memcmp(temp_mirror, global_vars_mirror, INTERNAL_FLASH_MIRROR_ITEM_SIZE)!=0){
                nvs_set_blob(my_handle,mirror_key,global_vars_mirror,INTERNAL_FLASH_MIRROR_ITEM_SIZE);
                res = nvs_commit(my_handle);
                if (res >= 0){
                    main_printf(TAG, "mirror saved to flash");
                }
            }else{
                main_printf(TAG, "mirror is the same, no needs to save");
            }
            nvs_close(my_handle);
        }
    }
    if (global_vars_mirror_mutex!=NULL){
        semaphore_release(global_vars_mirror_mutex);
    }
    return res;
}
/**
 * @brief internal_flash_restore_bkram_from_flash
 * @return
 */
int internal_flash_restore_mirror_from_flash(void){
    int res = 0;
    if (global_vars_mirror_mutex!=NULL){
        semaphore_take(global_vars_mirror_mutex, portMAX_DELAY );
    }
    {
        nvs_handle_t my_handle;
        res = nvs_open("storage", NVS_READWRITE, &my_handle);
        if (res == ESP_OK) {
            size_t len = INTERNAL_FLASH_MIRROR_ITEM_SIZE;
            res = nvs_get_blob(my_handle,mirror_key,global_vars_mirror,&len);
            nvs_close(my_handle);
        }
    }
    if (global_vars_mirror_mutex!=NULL){
        semaphore_release(global_vars_mirror_mutex);
    }
    return res;
}

void preinit_global_vars(){
    regs_template_t reg_template = {0};
    if(internal_flash_restore_mirror_from_flash()==0){
        main_printf(TAG, "regs inited from saved space");
        for (u16 i=0; i<NUM_OF_SELF_VARS; i++){
            reg_template.ind = i;
            if(regs_description_get_by_ind(&reg_template)==0){
                if(reg_template.property & SAVING){
                    memcpy(reg_template.p_value, &global_vars_mirror[reg_template.saved_address], regs_size_in_byte(reg_template.type)*reg_template.size);
                }else{
                    if(reg_template.p_default == NULL){
                        memset(reg_template.p_value,0,regs_size_in_byte(reg_template.type)*reg_template.size);
                    }else{
                        memcpy(reg_template.p_value,reg_template.p_default,regs_size_in_byte(reg_template.type)*reg_template.size);
                    }
                }
            }
        }
    }else{
        main_printf(TAG, "regs inited by default");
        set_regs_def_values();
        for (u16 i=0; i<NUM_OF_SELF_VARS; i++){
            reg_template.ind = i;
            if(regs_description_get_by_ind(&reg_template)==0){
                if(reg_template.property & SAVING){
                    mirror_access_write(&reg_template);
                }
            }
        }
    }
    regs_global.vars.reset_num++;
    if ((is_ascii_symbol_or_digital((u8*)&regs_global.vars.wifi_name,WIFI_NAME_LEN)<=0)||
        (is_ascii_symbol_or_digital((u8*)&regs_global.vars.wifi_password,WIFI_PASSWORD_LEN)<=0)||
        (strlen((char*)&regs_global.vars.wifi_router_name)==0)||
        (strlen((char*)&regs_global.vars.wifi_router_password)==0)||
            (regs_global.vars.table_version!=def_table_version)){
        set_regs_def_values();
        for (u16 i=0; i<NUM_OF_SELF_VARS; i++){
            reg_template.ind = i;
            if(regs_description_get_by_ind(&reg_template)==0){
                if(reg_template.property & SAVING){
                    mirror_access_write(&reg_template);
                }
            }
        }
    }
    int index = regs_description_get_index_by_address(&regs_global.vars.reset_num);
    if (index>=0){
        reg_template.ind = (u16)index;
        if (regs_description_get_by_ind(&reg_template)==0){
            mirror_access_write(&reg_template);
        }
    }
}
#endif //MIRROR_STORAGE_C
