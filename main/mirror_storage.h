/**
 * @file mirror_storage.h
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
#ifndef MIRROR_STORAGE_H
#define MIRROR_STORAGE_H 1
 
/*add includes below */
#include "type_def.h"
#include "os_type.h"
#include "main_config.h"
#include "regs_description.h"
/*add includes before */
#ifdef __cplusplus 
   extern "C" {
#endif
   #define FLASH_WRITE_PAGE_SIZE_IN_BYTE 32
/*add functions and variable declarations below */
extern semaphore_handle_t global_vars_mirror_mutex;
extern u8 global_vars_mirror[];
int mirror_storage_init(void);
   /**
    * @brief size of bkram
    * @return size bkram space availble in byte
    * @ingroup bkram
    * */
u32 mirror_access_get_size(void);
/**
 * @brief write data to bkram
 * @param address - bkram start address
 * @param data - pointer to data
 * @param size - data size in bytes
 * @return  0 - successfully, \n
 *          -1 - out of bkram address range
 * @ingroup bkram
 */
int mirror_access_write(regs_template_t * regs_template);
/**
 * @brief bkram_space_is_changing
 * @param address
 * @param data
 * @param size
 * @return 1 if change 0 - not, less then zero if error
 */
int mirror_space_is_changing(u16 address,const u8* data,u16 size);
/**
 * @brief read data from bkram
 * @param address - bkram start address
 * @param data - pointer to where the read data will be written
 * @param size - data size in bytes
 * @return  0 - successfully, \n
 *          -1 - out of bkram address range
 * @ingroup bkram
 */
int mirror_access_read(u16 address,u8* data,u16 size);
/**
 * @brief bkram_access_recalc_crc calculate crc and write to bkram(last 4 byte)
 * @return
 */
int mirror_recalc_crc(void);
/**
 * @brief internal_flash_save_bkram_to_flash
 * @return zero if bkram saved to flash succed
 */
int internal_flash_save_mirror_to_flash(void);
/**
 * @brief internal_flash_restore_bkram_from_flash
 * @return
 */
int internal_flash_restore_mirror_from_flash(void);

void preinit_global_vars(void);

/*add functions and variable declarations before */

#ifdef __cplusplus
}
#endif
#endif //MIRROR_STORAGE_H
