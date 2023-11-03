/**
 * @file step_motor.c
 * @author Shoma Gane <shomagan@gmail.com>
 *         
 * @defgroup main
 * @ingroup main
 * @version 0.1 
 * @brief  TODO!!! write brief in 
 */
#ifndef STEP_MOTOR_C
#define STEP_MOTOR_C 1
#include "common.h"
#include "step_motor.h"
#include "driver/gpio.h"
#include "pin_map.h"
#include "os_type.h"

task_handle_t step_motor_handle_id = NULL;
static int step_motor_init(void);
static int step_motor_deinit();

static int step_motor_init(){
    int result = 0;
    gpio_config_t io_conf = {};
    io_conf.intr_type = GPIO_INTR_DISABLE;
    io_conf.mode = GPIO_MODE_OUTPUT;
    io_conf.pin_bit_mask = (1ull<<GPIO_OUTPUT_STEP_MOTOR_EN)|
                           (1ull<<GPIO_OUTPUT_STEP_MOTOR_DIR0)|
                           (1ull<<GPIO_OUTPUT_STEP_MOTOR_STEP0)|
                           (1ull<<GPIO_OUTPUT_STEP_MOTOR_DIR1)|
                           (1ull<<GPIO_OUTPUT_STEP_MOTOR_STEP1);
    io_conf.pull_down_en = 0;
    io_conf.pull_up_en = 0;
    gpio_config(&io_conf);
    gpio_set_level(GPIO_OUTPUT_STEP_MOTOR_EN, 1);/*not active*/
    regs_global.vars.current_state[0] |= CS0_TASK_ACTIVE_STEP_MOTOR;
    return result;
}
/*GPIO deinit*/
static int step_motor_deinit(){
    int result = 0;
    gpio_config_t io_conf = {};
    gpio_set_level(GPIO_OUTPUT_STEP_MOTOR_EN, 1);/*not active*/
    io_conf.intr_type = GPIO_INTR_DISABLE;
    io_conf.mode = GPIO_MODE_DISABLE;
    io_conf.pin_bit_mask = (1ull<<GPIO_OUTPUT_STEP_MOTOR_EN)|
                           (1ull<<GPIO_OUTPUT_STEP_MOTOR_DIR0)|
                           (1ull<<GPIO_OUTPUT_STEP_MOTOR_STEP0)|
                           (1ull<<GPIO_OUTPUT_STEP_MOTOR_DIR1)|
                           (1ull<<GPIO_OUTPUT_STEP_MOTOR_STEP1);
    io_conf.pull_down_en = 0;
    io_conf.pull_up_en = 0;
    gpio_config(&io_conf);
    regs_global.vars.current_state[0] &= ~((u32)CS0_TASK_ACTIVE_STEP_MOTOR);
    return result;
}

void step_motor_task(void *arg){
    (void)(*arg);
    uint32_t signal_value;
    step_motor_init();
    u64 task_counter = 0;
    gpio_set_level(GPIO_OUTPUT_STEP_MOTOR_EN, 0u);/*active*/
    gpio_set_level(GPIO_OUTPUT_STEP_MOTOR_DIR0, 1u);
    gpio_set_level(GPIO_OUTPUT_STEP_MOTOR_DIR1, 0u);
    while(1){
        if(task_notify_wait(STOP_CHILD_PROCCES, &signal_value, 5u)==pdTRUE){
            /*by signal*/
            if (signal_value & STOP_CHILD_PROCCES){
                step_motor_deinit();
                task_delete(task_get_id());
            }
        }
        task_counter++;
        if (task_counter%2==0){
            gpio_set_level(GPIO_OUTPUT_STEP_MOTOR_STEP0, 0u);
            gpio_set_level(GPIO_OUTPUT_STEP_MOTOR_STEP1, 0u);
        }else{
            gpio_set_level(GPIO_OUTPUT_STEP_MOTOR_STEP0, 1u);
            gpio_set_level(GPIO_OUTPUT_STEP_MOTOR_STEP1, 1u);
        }
    }
}
 
#endif //STEP_MOTOR_C
