/**
 * @file interrupt.c
 * @author Shoma Gane <shomagan@gmail.com>
 *         
 * @defgroup main/
 * @ingroup main/
 * @version 0.1 
 * @brief  TODO!!! write brief in 
 */
#ifndef INTERRUPT_C
#define INTERRUPT_C 1
#include "common.h"
#include "pin_map.h"
#include "touch_handle.h"
#include "driver/gpio.h"
#include "driver/timer.h"
#include "sr04.h"
/*count faling edge only if rising edge has changed , skip 0*/
u64 rising_edge_old = 0u;
void IRAM_ATTR gpio_isr_handler(void* arg){
    uint32_t gpio_num = (uint32_t) arg;
#if    TOUCH_HANDLE_ENABLE/*need to rename!!!*/
    if (gpio_num == GPIO_WATER_COUNTER_INPUT)
    {
        regs_global_part1->vars.water_counter++;
    }else
#endif
#if SR04_MODULE 
    if(gpio_num == DI_HANDLER_PIN12_INPUT){
        ui32 prev_signal=0;
        int  higher_priority_task_woken;
        if(gpio_get_level(DI_HANDLER_PIN12_INPUT)){
            time_rising_edge = timer_group_get_counter_value_in_isr(TIMER_GROUP_0, TIMER_0);
        }else{
            if (rising_edge_old != time_rising_edge){
                time_faling_edge = timer_group_get_counter_value_in_isr(TIMER_GROUP_0, TIMER_0);
                task_notify_send_isr_overwrite(sr04_handle_id,0,(uint32_t)ECHO_FALING_EDGE,&prev_signal,&higher_priority_task_woken);
            }
            rising_edge_old = time_rising_edge;
        }
    }
#endif
}


#endif //INTERRUPT_C
