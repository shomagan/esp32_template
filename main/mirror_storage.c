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
#include "os_type.h"
#include "regs.h"
#include "regs_description.h"
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
/**
 * @brief bkram_access_recalc_crc calculate crc and write to bkram(last 4 byte)
 * @return
 */
int mirror_recalc_crc(u8 *data, u32 size);
esp_err_t mirror_storage_init(void) {
   if (global_vars_mirror_mutex == NULL) {
      global_vars_mirror_mutex = semaphore_create_binary();
   }
   esp_err_t ret = nvs_flash_init();
   if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
      ESP_ERROR_CHECK(nvs_flash_erase());
      ret = nvs_flash_init();
   }
   return ret;
};

u32 mirror_access_get_size(){
    return (INTERNAL_FLASH_MIRROR_ITEM_SIZE);
}
int mirror_access_init_buffer(u16 table_ind){
   int res = 0;
   u32 num_of_regs = regs_description_list_get_num_of_regs(table_ind);
   u8 * data = regs_description_list_get_buffer(table_ind);
   int size = regs_description_list_get_saved_buffer_size(table_ind);
   for(u32 i = 0; i < num_of_regs && res == 0; i++){
      regs_template_t regs_template = {0};
      regs_template.table_ind = table_ind;
      regs_description_get_by_index_in_table(&regs_template, i);
      if (regs_description_flag_check(&regs_template, SAVING)) {
         if (data && (regs_template.saved_address + regs_template.size_in_bytes <= size)) {
            semaphore_take(global_vars_mirror_mutex, portMAX_DELAY);
            memcpy(&data[regs_template.saved_address], regs_template.p_value, regs_template.size_in_bytes);
            semaphore_release(global_vars_mirror_mutex);
         } else {
            res = -1;
         }
      }
   }
   mirror_recalc_crc(data, size);
   return res;
}
int mirror_access_write(regs_template_t *regs_template)
{
   int res = 0;
   if (regs_template && regs_description_flag_check(regs_template, SAVING)) {
      u8 * data = regs_description_list_get_buffer(regs_template->table_ind);
      int size = regs_description_list_get_saved_buffer_size(regs_template->table_ind);
      if (data && (regs_template->saved_address + regs_template->size_in_bytes <= size)) {
         semaphore_take(global_vars_mirror_mutex, portMAX_DELAY);
         if(memcmp(&data[regs_template->saved_address], regs_template->p_value, regs_template->size_in_bytes) != 0){
            semaphore_release(global_vars_mirror_mutex);
            main_printf(TAG, "mirror_access_write: mirror_space_is_changing %i", regs_template->guid );
            memcpy(&data[regs_template->saved_address], regs_template->p_value, regs_template->size_in_bytes);
            mirror_recalc_crc(data, size);
            u32 table_to_save = regs_global->vars.table_to_save;
            table_to_save |= BIT(regs_template->table_ind);
            regs_copy_safe(&regs_global->vars.table_to_save, &table_to_save, sizeof(table_to_save));
            main_printf(TAG, "set ASYNC_INIT_SET_VALUE_FROM_BKRAM_TO_FLASH");
            regs_global->vars.async_flags |= ASYNC_INIT_SET_VALUE_FROM_BKRAM_TO_FLASH;
            res = 1;
         }else{
            semaphore_release(global_vars_mirror_mutex);
         }
      } else {
         res = -1;
      }
   }
   return res;
}
int mirror_space_is_changing(regs_template_t *regs_template) {
   int res = 0;
   u8 * data = regs_description_list_get_buffer(regs_template->table_ind);
   int size = regs_description_list_get_saved_buffer_size(regs_template->table_ind);
   if (data && (regs_template->saved_address + regs_template->size_in_bytes <= size)) {
      semaphore_take(global_vars_mirror_mutex, portMAX_DELAY);
      if(memcmp(&data[regs_template->saved_address], regs_template->p_value, regs_template->size_in_bytes) != 0){
         res = 1;
      }
      semaphore_release(global_vars_mirror_mutex);
   } else {
      res = -1;
   }
   return res;
}

int mirror_access_read(regs_template_t *regs_template) {
   u8 *data = regs_description_list_get_buffer(regs_template->table_ind);
   int size = regs_description_list_get_saved_buffer_size(regs_template->table_ind);
   if (data && (regs_template->saved_address + regs_template->size_in_bytes <= size)) {
      semaphore_take(global_vars_mirror_mutex, portMAX_DELAY);
      memcpy(regs_template->p_value, &data[regs_template->saved_address], regs_template->size_in_bytes);
      semaphore_release(global_vars_mirror_mutex);
      return 0;
   } else {
      return -1;
   }
}

int mirror_recalc_crc(u8 * data, u32 size){
   if(data){
      semaphore_take(global_vars_mirror_mutex, portMAX_DELAY );
      u32 crc = calc_crc_for_u32((void*)data, (size-4)/4);
      memcpy(&data[size-sizeof(u32)], &crc, sizeof(u32));
      semaphore_release(global_vars_mirror_mutex);
      return 0;
   }else{
      return -1;
   }
}
void print_nvs_files(void) {
   /*finds all files in NVS and print out names*/
   nvs_iterator_t it = NULL;
   esp_err_t err = nvs_entry_find(NVS_DEFAULT_PART_NAME, NULL, NVS_TYPE_ANY, &it);
   while (err == ESP_OK) {
      nvs_entry_info_t info;
      nvs_entry_info(it, &info);
      main_printf(TAG, "namespace: '%s', key: '%s', type: %d", info.namespace_name, info.key, info.type);
      err = nvs_entry_next(&it);
   }
   nvs_release_iterator(it);
}

int internal_flash_save_mirror_to_flash(u16 table_ind) {
   int res = 0;

   const char *space_name = regs_description_list_get_space_name(table_ind);
   if(space_name){
      regs_global->vars.flash_write_number = (regs_global->vars.flash_write_number < 0xffffffff)
                                                ? (regs_global->vars.flash_write_number + 1)
                                                : regs_global->vars.flash_write_number;
      main_printf(TAG, "mirror is saving to flash");
      nvs_handle_t my_handle = {0};
      size_t size = regs_description_list_get_saved_buffer_size(table_ind);
      const u8 *data = regs_description_list_get_buffer(table_ind);
      semaphore_take(global_vars_mirror_mutex, portMAX_DELAY);
      res = nvs_open(space_name, NVS_READWRITE, &my_handle);
      if (res == ESP_OK) {
         u8 temp_mirror[size];
         res = nvs_get_blob(my_handle, mirror_key, temp_mirror, &size);
         if (memcmp(temp_mirror, data, size) != 0) {
            nvs_set_blob(my_handle, mirror_key, data, size);
            res = nvs_commit(my_handle);
            if (res >= 0) {
               main_printf(TAG, "mirror %s saved to flash", space_name);
            }
         } else {
            main_printf(TAG, "mirror is the same, no needs to save");
         }
         nvs_close(my_handle);
      } else {
         main_error_message(TAG, "Error opening NVS handle! Error code: %s , %d", space_name, res);
      }
      semaphore_release(global_vars_mirror_mutex);
   }
   return res;
}

int internal_flash_restore_mirror_from_flash(u16 table_ind) {
   int res = 0;
   const char *space_name = regs_description_list_get_space_name(table_ind);
   if(space_name){
      semaphore_take(global_vars_mirror_mutex, portMAX_DELAY);
      {
         nvs_handle_t my_handle = {0};
         main_printf(TAG, "mirror is getting from flash %s", space_name);
         res = nvs_open(space_name, NVS_READWRITE, &my_handle);
         if (res == ESP_OK) {
            size_t size = regs_description_list_get_saved_buffer_size(table_ind);
            u8 *data = regs_description_list_get_buffer(table_ind);
            res = nvs_get_blob(my_handle, mirror_key, data, &size);
            nvs_close(my_handle);
         }else{
            main_error_message(TAG, "Error opening NVS handle! Error code: %s , %d", space_name, res);
         }
      }
      semaphore_release(global_vars_mirror_mutex);
   }else{
      res = -1;
   }
   return res;
}

void preinit_table_vars(u16 table_ind) {
   regs_template_t regs_template = { 0 };
   regs_template.table_ind = (u16)table_ind;
   u8 * saved_space =  regs_description_list_get_buffer(table_ind);
   if (internal_flash_restore_mirror_from_flash(table_ind) == 0) {
      main_printf(TAG, "regs inited from saved space");
      u32 num_of_regs = regs_description_list_get_num_of_regs(table_ind);
      for (u32 i = 0; i < num_of_regs; i++) {
         if (regs_description_get_by_index_in_table(&regs_template, i) == 0) {
            if (regs_template.property & SAVING) {
               memcpy(regs_template.p_value, &saved_space[regs_template.saved_address],
                      regs_size_in_byte(regs_template.type) * regs_template.size);
            } else {
               if (regs_template.p_default == NULL) {
                  memset(regs_template.p_value, 0, regs_size_in_byte(regs_template.type) * regs_template.size);
               } else {
                  memcpy(regs_template.p_value, regs_template.p_default,
                         regs_size_in_byte(regs_template.type) * regs_template.size);
               }
            }
         }
      }
   } else {
      main_printf(TAG, "regs init by default, restore from flash failed");
      set_regs_def_values(table_ind);
      mirror_access_init_buffer(table_ind);
   }
}
#endif //MIRROR_STORAGE_C
