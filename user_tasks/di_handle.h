/**
 * @file di_handler.h
 * @author Shoma Gane <shomagan@gmail.com>
 *
 * @defgroup main/
 * @ingroup main/
 * @version 0.1
 * @brief  TODO!!! write brief in
 */
#ifndef DI_HANDLE_H
#define DI_HANDLE_H 1

/*add includes below */
#include "type_def.h"
#include "main_config.h"
#include "os_type.h"
/*add includes before */
#ifdef __cplusplus
   extern "C" {
#endif
/*add functions and variable declarations below */
/**
 * @brief main struct
 * name variables uses for generate name in description file and then in get value by name
 * and therefore use max size len name is 16 charackter \n
 * coment style :   "" - description, \n
 *                  &ro  - read only, \n
 *                  &def -> have const varibale with struct like def_name, \n
 *                  &save- will have saved in bkram, \n
 *                  &crtcl- restart after change value, \n
 *
 * @ingroup regs
 */
/** #generator_use_description {"space_name" :"di_control_t",  "address_space" :3, "modbus_type" :"server", "modbus_function" :"holding_registers", "register_start_address" :3000}*/
typedef union{
    struct MCU_PACK{
        // start regs struct
        u32 pin_state;                 //!<"current states of digital inputs"
        u32 table_version;         //!< "table version, resets regs to defaults on mismatch" &ro &save &def
    }vars;
    u8 bytes[4]; //for full bksram copy
}di_control_t;// #generator_use_description {"message":"end_struct"}
extern di_control_t * const di_control;

extern task_handle_t di_handle_id;
#if DI_HANDLING_ENABLE
void di_handle_task(void *arg);
#endif
/*add functions and variable declarations before */
#ifdef __cplusplus
}
#endif
#endif //DI_HANDLER_H
