/**
 * @file di_handler.h
 * @author Shoma Gane <shomagan@gmail.com>
 *         
 * @defgroup main/
 * @ingroup main/
 * @version 0.1 
 * @brief  TODO!!! write brief in 
 */
#ifndef DI_HANDLE_H
#define DI_HANDLE_H 1
 
/*add includes below */
#include "type_def.h"
#include "main_config.h"
#include "os_type.h"
/*add includes before */
#ifdef __cplusplus 
   extern "C" {
#endif
/*add functions and variable declarations below */
extern task_handle_t di_handle_id;
void di_handle_task(void *arg);
/*add functions and variable declarations before */
#ifdef __cplusplus
}
#endif
#endif //DI_HANDLER_H
