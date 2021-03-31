/**
 * @file memory_handle.c
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
#ifndef MEMORY_HANDLE_C
#define MEMORY_HANDLE_C 1
#include "memory_handle.h" 
#include "string.h"
/*******************   Memory Pool Management Functions  ***********************/
/**
* @brief Create and Initialize a memory pool
* @param  pool_def      memory pool definition referenced with \ref osPool.
* @retval  memory pool ID for reference by other functions or NULL in case of error.
* @note   MUST REMAIN UNCHANGED: \b osPoolCreate shall be consistent in every CMSIS-RTOS.
*/
os_pool_cb_t * os_pool_create (const os_pool_def_t *pool_def){
    os_pool_cb_t * the_pool;
    int itemSize = 4 * ((pool_def->item_sz + 3) / 4);
    uint32_t i;
    /* First have to allocate memory for the pool control block. */
    the_pool = pvPortMalloc(sizeof(os_pool_cb_t));
    if (the_pool) {
        the_pool->pool_sz = pool_def->pool_sz;
        the_pool->item_sz = (u32)itemSize;
        the_pool->current_index = 0;
        /* Memory for markers */
        the_pool->markers = pvPortMalloc(pool_def->pool_sz);
        if (the_pool->markers) {
            /* Now allocate the pool itself. */
            the_pool->pool = pvPortMalloc(pool_def->pool_sz * (u32)itemSize);
            if (the_pool->pool) {
                for (i = 0; i < pool_def->pool_sz; i++) {
                    the_pool->markers[i] = 0;
                }
            } else {
                vPortFree(the_pool->markers);
                vPortFree(the_pool);
                the_pool = NULL;
            }
        } else {
            vPortFree(the_pool);
            the_pool = NULL;
        }
    }
    return the_pool;
}
/**
* @brief delete and freea memory pool
* @param  pool_id
* @retval  non zero value if error occured
*/
int os_pool_delete (os_pool_cb_t * pool_id){
    int res = 0;
    if (pool_id->pool){
        vPortFree(pool_id->pool);
    }else{
        res -= 1;
    }
    if (pool_id->markers){
        vPortFree(pool_id->markers);
    }else{
        res -= 1;
    }
    if (pool_id){
        vPortFree(pool_id);
    }else{
        res -= 1;
    }
    return res;
}
/**
* @brief Allocate a memory block from a memory pool
* @param pool_id       memory pool ID obtain referenced with \ref osPoolCreate.
* @retval  address of the allocated memory block or NULL in case of no memory available.
* @note   MUST REMAIN UNCHANGED: \b osPoolAlloc shall be consistent in every CMSIS-RTOS.
*/
void *os_pool_alloc (os_pool_cb_t * pool_id){
    void *p = NULL;
    uint32_t i;
    uint32_t index;
    for (i = 0; i < pool_id->pool_sz; i++) {
        index = pool_id->current_index + i;
        if (index >= pool_id->pool_sz) {
            index = 0;
        }
        if (pool_id->markers[index] == 0) {
            pool_id->markers[index] = 1;
            p = (void *)((uint32_t)(pool_id->pool) + (index * pool_id->item_sz));
            pool_id->current_index = index;
            break;
        }
    }
    return p;
}
/**
* @brief return pointer to pool by index number(item)
* @param pool_id  memory pool ID obtain referenced with \ref osPoolCreate.
* @param index  index by items(max size in pool_id).
* @retval  address of the allocated memory block or NULL in case of no memory available.
*
*/
void *os_pool_get_by_index(os_pool_cb_t * pool_id,u32 index){
    void *p = NULL;
    p = NULL;
    if (pool_id == NULL) {
        return p;
    }
    if((index < pool_id->pool_sz) &&
            (pool_id->markers[index] == 1)){
        p = (void *)((uint32_t)(pool_id->pool) + (index * pool_id->item_sz));
    }
    return p;
}
/**
* @brief return index corresspondet to pointer in pool
* @param pool_id  memory pool ID obtain referenced with \ref osPoolCreate.
* @param void * pointer
* @retval  address of the allocated memory block or NULL in case of no memory available.
*
*/
int os_pool_get_index_by_pointer(os_pool_cb_t * pool_id,void * pointer){
    int index;
    index = -1;
    if (pool_id == NULL) {
        return index;
    }
    if((pointer < (void*)pool_id) ||
            (pointer > (void*)((u32)pool_id + pool_id->item_sz * pool_id->pool_sz))){
        return index;
    }
    index = (int)(((u32)pointer - (u32)(pool_id->pool))/pool_id->item_sz);
    return index;
}

/**
* @brief Allocate a memory block from a memory pool and set memory block to zero
* @param  pool_id       memory pool ID obtain referenced with \ref osPoolCreate.
* @retval  address of the allocated memory block or NULL in case of no memory available.
* @note   MUST REMAIN UNCHANGED: \b osPoolCAlloc shall be consistent in every CMSIS-RTOS.
*/
void * os_pool_calloc (os_pool_cb_t * pool_id){
    void *p = os_pool_alloc(pool_id);
    if (p != NULL) {
        memset(p, 0, sizeof(pool_id->pool_sz));
    }
    return p;
}

/**
* @brief Return an allocated memory block back to a specific memory pool
* @param  pool_id       memory pool ID obtain referenced with \ref osPoolCreate.
* @param  block         address of the allocated memory block that is returned to the memory pool.
* @retval  status code that indicates the execution status of the function.
* @note   MUST REMAIN UNCHANGED: \b osPoolFree shall be consistent in every CMSIS-RTOS.
*/
int os_pool_free (os_pool_cb_t * pool_id, void *block){
    uint32_t index;
    if (pool_id == NULL) {
        return -1;
    }
    if (block == NULL) {
        return -2;
    }
    if (block < pool_id->pool) {
        return -3;
    }
    index = (uint32_t)block - (uint32_t)(pool_id->pool);
    if (index % pool_id->item_sz) {
        return -4;
    }
    index = index / pool_id->item_sz;
    if (index >= pool_id->pool_sz) {
        return -5;
    }
    pool_id->markers[index] = 0;
    return 0;
}

#endif //MEMORY_HANDLE_C
