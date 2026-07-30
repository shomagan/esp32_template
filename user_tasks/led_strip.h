/**
 * @file led_strip.h
 * @author Shoma Gane <shomagan@gmail.com>
 *
 * @defgroup main
 * @ingroup main
 * @version 0.1
 * @brief  TODO!!! write brief in
 */
#ifndef LED_STRIP_H
#define LED_STRIP_H 1

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
 * @brief led_strip registers
 * name variables uses for generate name in description file and then in get value by name
 * and therefore use max size len name is 16 charackter \n
 * coment style :   "" - description, \n
 *                  &ro  - read only, \n
 *                  &def -> have const varibale with struct like def_name, \n
 *                  &save- will have saved in bkram, \n
 * @ingroup regs
 */
/** #generator_use_description {"space_name" :"led_strip_reg_t",  "address_space" :12, "modbus_type" :"server", "modbus_function" :"holding_registers", "register_start_address" :4900}*/
typedef union {
   struct MCU_PACK {
      // start regs struct
      u32 table_version; //!< "table version, resets regs to defaults on mismatch" &ro &save
      u32 led_number; //!< "number of leds in strip" &save &def
      u32 led_brightness; //!< "brightness of leds in strip" &save &def
      u32 led_color; //!< "color of leds in strip" &save &def
   } vars;
   u32 bytes[4]; //for full bksram copy
}led_strip_reg_t; // #generator_use_description {"message":"end_struct"}
extern led_strip_reg_t * const led_strip_reg;

extern task_handle_t led_strip_handle_id;
#if LED_STRIP
extern void led_strip_task(void *arg);
#endif //LED_STRIP
/*add functions and variable declarations before */
#ifdef __cplusplus
}
#endif
#endif /*LED_STRIP_H*/
