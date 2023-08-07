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
