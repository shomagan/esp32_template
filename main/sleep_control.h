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
#define WAKE_UP_START 0
#define WAKE_UP_CANCELED 1

enum sleep_task_event{
   SLEEP_TASK_STOP_CHILD_PROCCES          = (1<<0),   /*!< sended before kill external thread*/
   SLEEP_TASK_DEEP_SLEEP_FOR_120_SEC = (1<<1),  /*!< go to deep sleep 120*/
   SLEEP_TASK_DEEP_SLEEP_FOR_N_SEC = (1<<2),  /*!< go to deep sleep for N sec*/
   SLEEP_TASK_DEEP_SLEEP = (1<<3),  /*!< go to deep sleep until external wake up*/

   /*up to bit 15 ,16 to 31 are taken for sleep second time*/
   
};
/*add functions and variable declarations below */
extern task_handle_t sleep_control_handle_id;
extern void sleep_control_task(void *arg);


/*add functions and variable declarations before */
#ifdef __cplusplus
}
#endif
#endif /*SLEEP_CONTROL_H*/
