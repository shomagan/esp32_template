/**
 * @file feeder.h
 * @author Shoma Gane <shomagan@gmail.com>
 *         
 * @defgroup main
 * @ingroup main
 * @version 0.1 
 * @brief  TODO!!! write brief in 
 */
#ifndef FEEDER_H
#define FEEDER_H 1
 
/*add includes below */
#include "type_def.h"
#include "main_config.h"
#include "regs.h"
#include "common.h"

/*add includes before */
#ifdef __cplusplus 
   extern "C" {
#endif
enum feeder_task_event{
   FEEDER_TASK_STOP_CHILD_PROCCES = (1<<0),   /*!< sended before kill external thread*/
   FEEDER_TASK_ONE_FEED = (1<<1),  /*!< one cycle off feeding*/
};
typedef enum {
    STOPPED = 0,
    SMOOTH_SPEED_UP,
    SMOOTH_KEEP_SPEED,
    SMOOTH_SLOW_DOWN,
    UNLIMITED_LOOP,
    LIMITED_LOOP,
} rmt_step_motor_running_status;


/*add functions and variable declarations below */
extern task_handle_t feeder_handle_id;
#if FEEDER
extern void feeder_task(void *arg);
#endif
/*add functions and variable declarations before */
#ifdef __cplusplus
}
#endif
#endif /*FEEDER_H*/
