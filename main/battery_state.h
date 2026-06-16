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
/**
 * @brief struct for battery_state component
 * name variables uses for generate name in description file and then in get value by name
 * and therefore use max size len name is 16 charackter \n
 * coment style :   "" - description, \n
 *                  &ro  - read only, \n
 *                  &def -> have const varibale with struct like def_name, \n
 *                  &save- will have saved in bkram, \n
 *  @ingroup regs
 */
/** #generator_use_description {"space_name" :"battery_state_reg_t",  "address_space" :10, "modbus_type" :"server", "modbus_function" :"holding_registers", "register_start_address" :4700}*/
typedef union{
    struct MCU_PACK{
        // start regs struct
        u16 battery_level;         //!<"0 - 100" &ro
        u16 battery_min_level;     //!<"min level to start" &save &def &min &max
        float battery_voltage;     //!<"for exmpl 4.2v" &ro
        u16 battery_settings;     //!<"bit 0 - activate level control" &save
    }vars;
    u32 bytes[32]; //for full bksram copy
}battery_state_reg_t;// #generator_use_description {"message":"end_struct"}
extern battery_state_reg_t * const battery_state_reg;

extern task_handle_t battery_state_handle_id;

#if BATTERY_STATE

extern void battery_state_task(void *arg);

#endif //BATTERY_STATE


/*add functions and variable declarations before */
#ifdef __cplusplus
}
#endif
#endif /*BATTERY_STATE_H*/
