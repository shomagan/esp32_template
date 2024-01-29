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
enum sleep_task_event{
   SLEEP_TASK_STOP_CHILD_PROCCES          = (1<<0),   /*!< sended before kill external thread*/
   SLEEP_TASK_DEEP_SLEEP_FOR_120_SEC = (1<<1),  /*!< go to deep sleep*/
};
/*add functions and variable declarations below */
extern task_handle_t sleep_control_handle_id;
extern void sleep_control_task(void *arg);

/*add functions and variable declarations before */
#ifdef __cplusplus
}
#endif
#endif /*SLEEP_CONTROL_H*/
