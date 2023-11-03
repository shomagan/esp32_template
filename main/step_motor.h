/**
 * @file step_motor.h
 * @author Shoma Gane <shomagan@gmail.com>
 *         
 * @defgroup main
 * @ingroup main
 * @version 0.1 
 * @brief  TODO!!! write brief in 
 */
#ifndef STEP_MOTOR_H
#define STEP_MOTOR_H 1
 
/*add includes below */
#include "type_def.h"
#include "main_config.h"
#include "regs.h"
/*add includes before */
#ifdef __cplusplus 
   extern "C" {
#endif
/*add functions and variable declarations below */
extern task_handle_t step_motor_handle_id;
extern void step_motor_task(void *arg);
/*add functions and variable declarations before */
#ifdef __cplusplus
}
#endif
#endif //STEP_MOTOR_H
