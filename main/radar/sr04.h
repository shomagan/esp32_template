/**
 * @file sr04.hpp
 * @author Shoma Gane <shomagan@gmail.com>
 *
 * @defgroup main/radar
 * @ingroup main/radar
 * @version 0.1
 * @brief  itteraction with sr04 radar
 */
#ifndef SR04_HPP
#define SR04_HPP 1
#include "main_config.h"
#include "regs.h"
typedef enum{
    SR04_STATE_ACTIVE = BIT(0),
    SR04_STATE_ECHO = BIT(1),
} sr04_state_t;
/**
 * @brief sr04 measurements struct for distance control and time laps
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
/** #generator_use_description {"space_name" :"sr04_reg_t",  "address_space" :5, "modbus_type" :"server", "modbus_function" :"holding_registers", "register_start_address" :4100}*/
typedef union{
    struct MCU_PACK{
        // start regs struct
        u16 lap_state;         //!< "state sr04, bit0 - activated, bit1 - echo signal received" &ro
        float lap_distance;        //!< "current distance" &ro
        u64 lap;       //!< "when we have sharp change of a distance, save it " &ro
        u64 lap_paired_dev;    //!< "lap from paired device" &ro
        float distance_filtered;      //!< "current distance filterd" &ro
    }vars;
    u8 bytes[32]; //for full bksram copy
}sr04_reg_t;// #generator_use_description {"message":"end_struct"}
extern sr04_reg_t * const sr04_reg;

void sr04_task(void *arg);
extern task_handle_t sr04_handle_id;
extern u64 time_rising_edge;
extern u64 time_faling_edge;

typedef enum{
    ECHO_RISING_EDGE = (1<<7),
    ECHO_FALING_EDGE = (1<<8),
}sr04_edges_t;
typedef enum{
    SR04_STEP_TRIGGER = 1,
    SR04_STEP_RISING = 2,
    SR04_STEP_FALING = 3,
}sr04_step_t;


#endif //SR04_HPP
