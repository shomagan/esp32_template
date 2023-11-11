/**
 * @file sleep_control.h
 * @author Shoma Gane <shomagan@gmail.com>
 *         
 * @defgroup main
 * @ingroup main
 * @version 0.1 
 * @brief  TODO!!! write brief in 
 */
#ifndef SLEEP_CONTROL_H
#define SLEEP_CONTROL_H 1
 
/*add includes below */
#include "type_def.h"
#include "main_config.h"
#include "regs.h"

/*add includes before */
#ifdef __cplusplus 
   extern "C" {
#endif
/*add functions and variable declarations below */
extern task_handle_t sleep_control_handle_id;
extern void sleep_control_task(void *arg);

/*add functions and variable declarations before */
#ifdef __cplusplus
}
#endif
#endif /*SLEEP_CONTROL_H*/
