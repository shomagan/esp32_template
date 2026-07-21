/**
 * @file polisher.h
 * @author Shoma Gane <shomagan@gmail.com>
 *
 * @defgroup main
 * @ingroup main
 * @version 0.1
 * @brief  TODO!!! write brief in
 */
#ifndef POLISHER_H
#define POLISHER_H 1

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
/**
 * @brief polisher struct for speed control
 * name variables uses for generate name in description file and then in get value by name
 * and therefore use max size len name is 16 charackter \n
 * coment style :   "" - description, \n
 *                  &ro  - read only, \n
 *                  &def -> have const varibale with struct like def_name, \n
 *                  &save- will have saved in bkram, \n
 *
 * @ingroup regs
 */
/** #generator_use_description {"space_name" :"polisher_reg_t",  "address_space" :7, "modbus_type" :"server", "modbus_function" :"holding_registers", "register_start_address" :4300}*/
typedef union{
    struct MCU_PACK{
        // start regs struct
        u32 polisher_sec;         //!<"how many time in seconds it was active" &save &ro
        u32 polisher_last_sec;         //!<"how many time in seconds it was active last session" &ro
        u16 polisher_speed;        //!< "polisher_speed" &save &def &max
        u16 polisher_direction;         //!< "polisher_direction" &save &def &max
        u32 table_version;         //!< "table version, resets regs to defaults on mismatch" &ro &save &def
    }vars;
    u32 bytes[4]; //for full bksram copy
}polisher_reg_t;// #generator_use_description {"message":"end_struct"}
extern polisher_reg_t * const polisher_reg;

extern task_handle_t polisher_handle_id;

#if POLISHER

extern void polisher_task(void *arg);

#endif //POLISHER


/*add functions and variable declarations before */
#ifdef __cplusplus
}
#endif
#endif /*POLISHER_H*/
