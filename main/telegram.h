/**
 * @file telegram.h
 * @author Shoma Gane <shomagan@gmail.com>
 *         
 * @defgroup main
 * @ingroup main
 * @version 0.1 
 * @brief  TODO!!! write brief in 
 */
#ifndef TELEGRAM_H
#define TELEGRAM_H 1
 
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
enum telegram_task_event{
   TELEGRAM_SEND_REG_BY_ID     = (1<<1),   /*!< send register by id to telegram*/
};

extern task_handle_t telegram_handle_id;
extern void telegram_task(void *arg);

/*add functions and variable declarations before */
#ifdef __cplusplus
}
#endif
#endif /*TELEGRAM_H*/
