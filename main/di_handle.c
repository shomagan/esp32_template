/**
 * @file di_handler.c
 * @author Shoma Gane <shomagan@gmail.com>
 *         
 * @defgroup main/
 * @ingroup main/
 * @version 0.1 
 * @brief  TODO!!! write brief in 
 */
#ifndef DI_HANDLER_C
#define DI_HANDLER_C 1
#include "di_handle.h"
#include "common.h"
#include "regs.h"
#include "pin_map.h"
#include "driver/gpio.h"
#define PIN_STATE_DEBUG 0
task_handle_t di_handle_id = NULL;
#if DI_HANDLING_ENABLE
/*GPIO init*/
int di_handle_init(){
    int result = 0;
    gpio_config_t io_conf;
    io_conf.intr_type = GPIO_INTR_DISABLE;
    io_conf.mode = GPIO_MODE_INPUT;
    io_conf.pin_bit_mask = 1ULL<<DI_HANDLER_PIN12_INPUT;
    io_conf.pull_down_en = 0;
    io_conf.pull_up_en = 1;
    gpio_config(&io_conf);
    io_conf.pin_bit_mask = 1ULL<<DI_HANDLER_PIN13_INPUT;
    gpio_config(&io_conf);
    io_conf.pin_bit_mask = 1ULL<<DI_HANDLER_PIN14_INPUT;
    gpio_config(&io_conf);
    io_conf.pin_bit_mask = 1ULL<<DI_HANDLER_PIN27_INPUT;
    gpio_config(&io_conf);
    io_conf.pin_bit_mask = 1ULL<<DI_HANDLER_PIN26_INPUT;
    gpio_config(&io_conf);
    io_conf.pin_bit_mask = 1ULL<<DI_HANDLER_PIN23_INPUT;
    gpio_config(&io_conf);
    io_conf.pin_bit_mask = 1ULL<<DI_HANDLER_PIN32_INPUT;
    gpio_config(&io_conf);
    io_conf.pin_bit_mask = 1ULL<<DI_HANDLER_PIN15_INPUT;
    gpio_config(&io_conf);
    io_conf.pin_bit_mask = 1ULL<<DI_HANDLER_PIN18_INPUT;
    gpio_config(&io_conf);
    io_conf.pin_bit_mask = 1ULL<<DI_HANDLER_PIN4_INPUT_RESERV;
    gpio_config(&io_conf);
    io_conf.pin_bit_mask = 1ULL<<DI_HANDLER_PIN5_INPUT_RESERV;
    gpio_config(&io_conf);
    return result;
}
/*GPIO deinit*/
int di_handle_deinit(){
    int result = 0;
    gpio_config_t io_conf;
    io_conf.intr_type = GPIO_INTR_DISABLE;
    io_conf.mode = GPIO_INTR_DISABLE;
    io_conf.pin_bit_mask = 1ULL<<DI_HANDLER_PIN12_INPUT;
    io_conf.pull_down_en = 0;
    io_conf.pull_up_en = 0;
    gpio_config(&io_conf);
    io_conf.pin_bit_mask = 1ULL<<DI_HANDLER_PIN13_INPUT;
    gpio_config(&io_conf);
    io_conf.pin_bit_mask = 1ULL<<DI_HANDLER_PIN14_INPUT;
    gpio_config(&io_conf);
    io_conf.pin_bit_mask = 1ULL<<DI_HANDLER_PIN27_INPUT;
    gpio_config(&io_conf);
    io_conf.pin_bit_mask = 1ULL<<DI_HANDLER_PIN26_INPUT;
    gpio_config(&io_conf);
    io_conf.pin_bit_mask = 1ULL<<DI_HANDLER_PIN23_INPUT;
    gpio_config(&io_conf);
    io_conf.pin_bit_mask = 1ULL<<DI_HANDLER_PIN32_INPUT;
    gpio_config(&io_conf);
    io_conf.pin_bit_mask = 1ULL<<DI_HANDLER_PIN15_INPUT;
    gpio_config(&io_conf);
    io_conf.pin_bit_mask = 1ULL<<DI_HANDLER_PIN18_INPUT;
    gpio_config(&io_conf);
    io_conf.pin_bit_mask = 1ULL<<DI_HANDLER_PIN4_INPUT_RESERV;
    gpio_config(&io_conf);
    io_conf.pin_bit_mask = 1ULL<<DI_HANDLER_PIN5_INPUT_RESERV;
    gpio_config(&io_conf);

    return result;
}

void di_handle_task(void *arg){
    (void)(*arg);
    uint32_t signal_value;
    di_handle_init();
    u32 pin_states = 0;
    u64 task_counter = 0;
    while(1){
        if(task_notify_wait(STOP_CHILD_PROCCES|PACKET_RECEIVED, &signal_value, 100)==pdTRUE){
            /*by signal*/
            if (signal_value & STOP_CHILD_PROCCES){
                di_handle_deinit();
                task_delete(task_get_id());
            }else if(signal_value & PACKET_RECEIVED){

            }
        }
        pin_states = gpio_get_level(DI_HANDLER_PIN12_INPUT)|
                (gpio_get_level(DI_HANDLER_PIN13_INPUT)<<1u)|
                (gpio_get_level(DI_HANDLER_PIN14_INPUT)<<2u)|
                (gpio_get_level(DI_HANDLER_PIN27_INPUT)<<3u)|
                (gpio_get_level(DI_HANDLER_PIN26_INPUT)<<4u)|
                (gpio_get_level(DI_HANDLER_PIN23_INPUT)<<5u)|
                (gpio_get_level(DI_HANDLER_PIN32_INPUT)<<6u)|
                (gpio_get_level(DI_HANDLER_PIN15_INPUT)<<7u)|
                (gpio_get_level(DI_HANDLER_PIN18_INPUT)<<8u)|
                (gpio_get_level(DI_HANDLER_PIN4_INPUT_RESERV)<<9u)|
                (gpio_get_level(DI_HANDLER_PIN5_INPUT_RESERV)<<10u);
        pin_states = (~pin_states) & 0xFFF;
        semaphore_take(regs_access_mutex, portMAX_DELAY);{
#if PIN_STATE_DEBUG            
        static u32 pin_states_debug = 0xfffu;
        if (task_counter % 100 == 0u){
            if(pin_states_debug==0u){
                pin_states_debug = 0xfffu;
            }else{
                pin_states_debug = pin_states_debug>>1;
            }
            
        }                
        di_control->vars.pin_state = pin_states_debug;
#else
        di_control->vars.pin_state = pin_states;
#endif        
        }semaphore_release(regs_access_mutex);
        task_counter++;
    }
}
#endif /*DI_HANDLING_ENABLE*/
#endif //DI_HANDLER_C
