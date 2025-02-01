/**
 * @file battery_state.h
 * @author Shoma Gane <shomagan@gmail.com>
 *         
 * @defgroup main
 * @ingroup main
 * @version 0.1 
 * @brief  TODO!!! write brief in 
 */
#ifndef BATTERY_STATE_H
#define BATTERY_STATE_H 1
 
/*add includes below */
#include "type_def.h"
#include "main_config.h"
#include "regs.h"
#include "common.h"
#include "pin_map.h"

/*add includes before */
#ifdef __cplusplus 
   extern "C" {
#endif
/*add functions and variable declarations below */

extern task_handle_t battery_state_handle_id;

#if BATTERY_STATE

extern void battery_state_task(void *arg);

#endif //BATTERY_STATE


/*add functions and variable declarations before */
#ifdef __cplusplus
}
#endif
#endif /*BATTERY_STATE_H*/
