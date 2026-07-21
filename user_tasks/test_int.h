/**
 * @file test_int.h
 * @author Shoma Gane <shomagan@gmail.com>
 *
 * @defgroup main
 * @ingroup main
 * @version 0.1
 * @brief  TODO!!! write brief in
 */
#ifndef TEST_INT_H
#define TEST_INT_H 1

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
 * @brief test struct for internal tests
 * name variables uses for generate name in description file and then in get value by name
 * and therefore use max size len name is 16 charackter \n
 * coment style :   "" - description, \n
 *                  &ro  - read only, \n
 *                  &def -> have const varibale with struct like def_name, \n
 *                  &save- will have saved in bkram, \n
 *  @ingroup regs
 */
/** #generator_use_description {"space_name" :"test_int_reg_t",  "address_space" :8, "modbus_type" :"server", "modbus_function" :"holding_registers", "register_start_address" :4400}*/
typedef union{
    struct MCU_PACK{
        // start regs struct
        u32 test_int_state;         //!<"current state" &ro
        u32 test_int_command;         //!<"command" &ro
        u32 test_int_component;         //!<"component to be tested"
        u32 test_int_type;         //!<"test type - check, stress, performance"
        u32 test_int_result;         //!<"overall result" &ro
        u32 table_version;         //!< "table version, resets regs to defaults on mismatch" &ro &save &def
    }vars;
    u32 bytes[4]; //for full bksram copy
}test_int_reg_t;// #generator_use_description {"message":"end_struct"}
extern test_int_reg_t * const test_int_reg;

extern task_handle_t test_int_handle_id;

#if TEST_INT

extern void test_int_task(void *arg);

#endif //TEST_INT


/*add functions and variable declarations before */
#ifdef __cplusplus
}
#endif
#endif /*TEST_INT_H*/
