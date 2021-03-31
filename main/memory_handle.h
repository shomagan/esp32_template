/**
 * @file memory_handle.h
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
#ifndef MEMORY_HANDLE_H
#define MEMORY_HANDLE_H 1

/*add includes below */
#include "type_def.h"
#include "os_type.h"

/*add includes before */
#ifdef __cplusplus 
extern "C" {
#endif
/*add functions and variable declarations below */
typedef struct os_pool_cb {
    void *pool;
    uint8_t *markers;
    uint32_t pool_sz;
    uint32_t item_sz;
    uint32_t current_index;
} os_pool_cb_t;

/// Definition structure for memory block allocation.
/// \note CAN BE CHANGED: \b os_pool_def is implementation specific in every CMSIS-RTOS.
typedef struct os_pool_def  {
    uint32_t                 pool_sz;    ///< number of items (elements) in the pool
    uint32_t                 item_sz;    ///< size of an item
    void                       *pool;    ///< pointer to memory for pool
} os_pool_def_t;
os_pool_cb_t * os_pool_create (const os_pool_def_t *pool_def);
void *os_pool_alloc (os_pool_cb_t * pool_id);
void *os_pool_get_by_index(os_pool_cb_t * pool_id,u32 index);
int os_pool_get_index_by_pointer(os_pool_cb_t * pool_id,void * pointer);
void * os_pool_calloc (os_pool_cb_t * pool_id);
int os_pool_free (os_pool_cb_t * pool_id, void *block);
/*add functions and variable declarations before */
#ifdef __cplusplus
}
#endif
#endif //MEMORY_HANDLE_H
