/**
 * @file wireless_control.h
 * @author Shoma Gane <shomagan@gmail.com>
 *         
 * @defgroup main
 * @ingroup main
 * @version 0.1 
 * @brief  TODO!!! write brief in 
 */
#ifndef WIRELESS_CONTROL_H
#define WIRELESS_CONTROL_H 1
 
/*add includes below */
#include "type_def.h"
#include "main_config.h"
#include "regs.h"
#include "common.h"

/*add includes before */
#ifdef __cplusplus 
   extern "C" {
#endif
/*add functions and variable declarations below */
extern task_handle_t wireless_control_handle_id;
extern void wireless_control_task(void *arg);

/*add functions and variable declarations before */
#ifdef __cplusplus
}
#endif
#endif /*WIRELESS_CONTROL_H*/
