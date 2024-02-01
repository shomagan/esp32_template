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
enum wireless_control_task_event{
   WIRELESS_TASK_STOP_CHILD_PROCCES          = (1<<0),   /*!< sended before kill external thread*/
   WIRELESS_TASK_STOP_WIFI = (1<<1),  /*!< disable WIFI - stop it*/
   WIRELESS_TASK_START_WIFI = (1<<2),  /*!< enable WIFI -start it if neccessery, register should be set first*/
   WIRELESS_TASK_RESET_WIFI_FOR_120_SEC = (1<<3),/*!< disable WIFI FOR 120 SEC*/
};

/*add functions and variable declarations below */
extern task_handle_t wireless_control_handle_id;
extern void wireless_control_task(void *arg);

/*add functions and variable declarations before */
#ifdef __cplusplus
}
#endif
#endif /*WIRELESS_CONTROL_H*/
