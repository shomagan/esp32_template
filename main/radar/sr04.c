/**
 * @file sr04.cpp
 * @author Shoma Gane <shomagan@gmail.com>
 *         
 * @defgroup main/radar
 * @ingroup main/radar
 * @version 0.1 
 * @brief  itteraction with sr04 radar
 */
#ifndef SR04_CPP
#define SR04_CPP 1
#include "common.h"
#include "regs.h"
#include "pin_map.h"
#include "sr04.h"
#include "driver/gpio.h"
#include "interrupt.h"
#include "driver/timer.h"
#include "os_type.h"
#define ESP_INTR_FLAG_DEFAULT 0
task_handle_t sr04_handle_id = NULL;
static portMUX_TYPE sr04_mux = portMUX_INITIALIZER_UNLOCKED;
/*regs used in interrupts start*/
u64 time_rising_edge = 0;
u64 time_faling_edge = 0;
/*regs used in interrupts end*/
#if SR04_MODULE
static void copy_regs(void);
static int sr04_init(void);
static int sr04_deinit();
static void sr04_trigger();
static u8 distance_is_in_range(float first,float second,float range);
static void copy_regs(){
    /*regs_copy_safe(&sr04_reg->vars.state,&state,sizeof(sr04_reg->vars.state));
    regs_copy_safe(&sr04_reg->vars.distance,&distance,sizeof(sr04_reg->vars.distance));
    regs_copy_safe(&sr04_reg->vars.lap,&lap,sizeof(sr04_reg->vars.lap));
    regs_copy_safe(&sr04_reg->vars.lap_paired_dev,&lap_paired_dev,sizeof(sr04_reg->vars.lap_paired_dev));*/
}

static int sr04_init(){
    int result = 0;
    gpio_config_t io_conf;
    io_conf.intr_type = GPIO_INTR_DISABLE;
    io_conf.mode = GPIO_MODE_OUTPUT;
    io_conf.pin_bit_mask = 1ULL<<DI_HANDLER_PIN13_INPUT;
    io_conf.pull_down_en = 0;
    io_conf.pull_up_en = 0;
    gpio_config(&io_conf);
    gpio_set_level(DI_HANDLER_PIN13_INPUT, 0);
    io_conf.intr_type = GPIO_INTR_POSEDGE;
    io_conf.mode = GPIO_MODE_INPUT;
    io_conf.pin_bit_mask = 1ULL<<DI_HANDLER_PIN12_INPUT;
    io_conf.pull_down_en = 0;
    io_conf.pull_up_en = 0;
    gpio_config(&io_conf);
    //change gpio intrrupt type for one pin
    gpio_set_intr_type(DI_HANDLER_PIN12_INPUT, GPIO_INTR_ANYEDGE);
    //install gpio isr service
    gpio_install_isr_service(ESP_INTR_FLAG_DEFAULT);
    //hook isr handler for specific gpio pin
    gpio_isr_handler_add(DI_HANDLER_PIN12_INPUT, gpio_isr_handler, (void*) DI_HANDLER_PIN12_INPUT);
    return result;
}
/*GPIO deinit*/
static int sr04_deinit(){
    int result = 0;
    gpio_config_t io_conf;
    //remove isr handler for gpio number.
    gpio_isr_handler_remove(DI_HANDLER_PIN12_INPUT);
    io_conf.intr_type = GPIO_INTR_DISABLE;
    io_conf.mode = GPIO_MODE_DISABLE;
    io_conf.pin_bit_mask = 1ULL<<DI_HANDLER_PIN12_INPUT;
    io_conf.pull_down_en = 0;
    io_conf.pull_up_en = 0;
    gpio_config(&io_conf);
    io_conf.pin_bit_mask = 1ULL<<DI_HANDLER_PIN13_INPUT;
    gpio_config(&io_conf);
    return result;
}

void sr04_task(void *arg){
    (void)(*arg);
    uint32_t signal_value;
    u16 state = 0;
    float distance = 0.0f;
    float preview_distance = 0.0f;
    u64 lap;
    u64 lap_paired_dev;
    sr04_init();
    u64 task_counter = 0;
    state |= BIT(SR04_STATE_ACTIVE);
    sr04_step_t sr04_step = 0;
    regs_global->vars.current_state[0] |= CS0_TASK_ACTIVE_SR04;
    while(1){
        if(task_notify_wait(STOP_CHILD_PROCCES|ECHO_FALING_EDGE, &signal_value, 5u)==pdTRUE){
            /*by signal*/
            if (signal_value & STOP_CHILD_PROCCES){
                regs_global->vars.current_state[0] &= ~((u32)CS0_TASK_ACTIVE_SR04);
                sr04_deinit();
                task_delete(task_get_id());
            }else if (signal_value & ECHO_FALING_EDGE){
                sr04_step = SR04_STEP_FALING;
                os_enter_critical(&sr04_mux);
                distance = (float)(time_faling_edge*10 - time_rising_edge*10) / 58.0f;
                os_exit_critical(&sr04_mux);
                state |= SR04_STATE_ECHO;
                regs_copy_safe(&sr04_reg->vars.lap_state,&state,sizeof(sr04_reg->vars.lap_state));                    
                if (distance < 15.5f){
                    regs_copy_safe(&lap,&regs_global->vars.sys_tick_counter,sizeof(regs_global->vars.sys_tick_counter));
                    regs_copy_safe(&sr04_reg->vars.lap,&lap,sizeof(sr04_reg->vars.lap));
                    regs_copy_safe(&sr04_reg->vars.lap_distance,&distance,sizeof(sr04_reg->vars.lap_distance));
                }
                if (distance_is_in_range(distance,preview_distance,1.0f)){
                    regs_copy_safe(&sr04_reg->vars.distance_filtered,&distance,sizeof(sr04_reg->vars.distance_filtered));
                }
                preview_distance = distance;
            }
        }else{
            sr04_step = SR04_STEP_TRIGGER;
            sr04_trigger();
        }
        task_counter++;
    }
}
static void sr04_trigger(){
    gpio_set_level(DI_HANDLER_PIN13_INPUT, 1);
    u64 temp_time, temp_time2;
    int i =10000u;
    timer_get_counter_value(TIMER_GROUP_0, TIMER_0, &temp_time);
    temp_time2 = temp_time;
    while(i && ((temp_time2 - temp_time) > 2u)){
        timer_get_counter_value(TIMER_GROUP_0, TIMER_0, &temp_time2);
        i--;
    };
    gpio_set_level(DI_HANDLER_PIN13_INPUT, 0);
}
static u8 distance_is_in_range(float first,float second,float range){
    u8 result = 0u;
    if (first > second){
        if ((first - second) < range){
            result = 1u;
        }
    }else{
        if ((second - first) < range){
            result = 1u;
        }
    }
    return result;
}
#endif/*SR04_MODULE*/
#endif //SR04_CPP
