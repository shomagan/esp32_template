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
/*add functions and variable declarations below */

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


/**
 * @brief feeders struct for feed control
 * name variables uses for generate name in description file and then in get value by name
 * and therefore use max size len name is 16 charackter \n
 * coment style :   "" - description, \n
 *                  &ro  - read only, \n
 *                  &def -> have const varibale with struct like def_name, \n
 *                  &save- will have saved in bkram, \n
 *
 * @ingroup regs
 */
/** #generator_use_description {"space_name" :"feeder_reg_t",  "address_space" :6, "modbus_type" :"server", "modbus_function" :"holding_registers", "register_start_address" :4200}*/
typedef union{
    struct MCU_PACK{
        // start regs struct
        u32 feeder_counter;         //!<"how many time step motor started" &save &ro
        u16 feeder_interval;        //!< "minutes between a feeds" &save &def &min
        u16 feeder_reserv0;         //!< "reserved"
        float feeder_time_sec;       //!< "turn time is seconds" &save &def &min &max
    }vars;
    u32 bytes[4]; //for full bksram copy
}feeder_reg_t;// #generator_use_description {"message":"end_struct"}
extern feeder_reg_t * const feeder_reg;

extern task_handle_t feeder_handle_id;
#if FEEDER
extern void feeder_task(void *arg);
#endif
/*add functions and variable declarations before */
#ifdef __cplusplus
}
#endif
#endif /*FEEDER_H*/
