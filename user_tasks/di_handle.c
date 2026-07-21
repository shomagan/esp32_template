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
#include "link_functions.h"
#define PIN_STATE_DEBUG 0
task_handle_t di_handle_id = NULL;

/*#generator_use_description {"user_task_regs":"start_struct"}*/
static di_control_t di_control_storage = {{0}};
di_control_t * const di_control = &di_control_storage;
#define NUM_OF_DI_CONTROL_VARS 2
static u8 di_control_saved_buf[8];
static const char di_control_space_name[] = "di_control_t";
static regs_description_t const regs_description_di_control[NUM_OF_DI_CONTROL_VARS] = {
    { NULL, NULL, NULL, (u8*)&di_control->vars.pin_state, 0,"current states of digital inputs","pin_state", 0x14006000, 0x30bb8, U32_REGS_FLAG, 1, 1, 3 }//!<"current states of digital inputs"
,
    { &def_table_version, NULL, NULL, (u8*)&di_control->vars.table_version, 0,"table version, resets regs to defaults on mismatch","table_version", 0x14006001, 0x30bba, U32_REGS_FLAG, 1, 7, 3 }//!< "table version, resets regs to defaults on mismatch" &ro &save &def
,
};
const regs_description_list_t regs_table_di_control = {
    .description = regs_description_di_control,
    .num_of_regs = NUM_OF_DI_CONTROL_VARS,
    .table_version = &def_table_version,
    .space_name = di_control_space_name,
    .saved_regs_buffer = di_control_saved_buf,
    .saved_regs_buffer_size = sizeof(di_control_saved_buf),
};
/*#generator_use_description {"user_task_regs":"end_struct"}*/

#if DI_HANDLING_ENABLE
/*GPIO init*/
int di_handle_init(){
    int result = 0;
    int table_ind = link_functions.regs_description_list_add_new(regs_table_di_control);
    link_functions.preinit_table_vars((u16)table_ind);
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
    (void)arg;
    uint32_t signal_value;
    di_handle_init();
    u32 pin_states = 0;
    u64 task_counter = 0;
    while(1){
        if(link_functions.os_thread_signal_wait(STOP_CHILD_PROCCES|PACKET_RECEIVED, &signal_value, 100)==pdTRUE){
            /*by signal*/
            if (signal_value & STOP_CHILD_PROCCES){
                di_handle_deinit();
                link_functions.os_thread_terminate(link_functions.os_thread_get_id());
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
        link_functions.os_semaphore_wait(regs_access_mutex, portMAX_DELAY);{
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
        }link_functions.os_semaphore_release(regs_access_mutex);
        task_counter++;
    }
}
#endif /*DI_HANDLING_ENABLE*/
#endif //DI_HANDLER_C
