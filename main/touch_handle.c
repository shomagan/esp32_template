/**
 * @file pwm_test.c
 * @author Shoma Gane <shomagan@gmail.com>
 *         Ayrat Girfanov <girfanov.ayrat@yandex.ru>
 * @defgroup ../main/
 * @ingroup ../main/
 * @version 0.1 
 * @brief  TODO!!! write brief in 
 */
#ifndef TOUCH_HANDLE_C
#define TOUCH_HANDLE_C 1
#include "common.h"
#include "touch_handle.h"
#include "regs.h"
#include "regs_description.h"
#include "driver/touch_pad.h"
#include "driver/gpio.h"
#include "mirror_storage.h"
#include "math.h"
static const char *TAG = "touch_handle";
static void touch_gpio_initialize(void);
static int touch_handle_init(void);
static int handle_touch_value(u16 touch_0,u16 touch_1,u16 touch_2,u16 touch_3);
#define TOUCH_0 0
#define TOUCH_1 7
#define TOUCH_2 8
#define TOUCH_3 9
#define DRIVE_PIN 25
#define GPIO_INPUT_IO_0     23
#define GPIO_INPUT_PIN_SEL  (1ULL<<GPIO_INPUT_IO_0)
#define TOUCH_THRESH_NO_USE   (0)
#define TOUCHPAD_FILTER_TOUCH_PERIOD (10)
#define MAX_ENABLED_PERIOD_MS 600000UL

#define THRESHOLD_PERCENT 0.9f
#define CONNECT_PERCENT 0.9f
#define UNCONNECT_PERCENT 0.25f
#define ESP_INTR_FLAG_DEFAULT 0
typedef enum{
    IDLE_STATE = 0,
    TOUCH_0_STATE = BIT(0),
    TOUCH_1_STATE = BIT(1),
    TOUCH_2_STATE = BIT(2),
    TOUCH_3_STATE = BIT(3),

}touch_states_t;

static void IRAM_ATTR gpio_isr_handler(void* arg){
    uint32_t gpio_num = (uint32_t) arg;
    if (gpio_num == GPIO_INPUT_IO_0){
        regs_global_part1.vars.water_counter++;
    }
}

static touch_states_t touch_states = IDLE_STATE;
task_handle_t touch_task_handle;
static void touch_gpio_initialize(void){
    gpio_config_t io_conf;
    //disable interrupt
    io_conf.intr_type = GPIO_INTR_DISABLE;
    //set as output mode
    io_conf.mode = GPIO_MODE_OUTPUT;
    //bit mask of the pins that you want to set,e.g.GPIO18/19
    io_conf.pin_bit_mask = 1ULL<<DRIVE_PIN;
    //disable pull-down mode
    io_conf.pull_down_en = 0;
    //disable pull-up mode
    io_conf.pull_up_en = 0;
    //configure GPIO with the given settings
    gpio_config(&io_conf);
    touch_pad_config(TOUCH_0, TOUCH_THRESH_NO_USE);
    touch_pad_config(TOUCH_1, TOUCH_THRESH_NO_USE);
    touch_pad_config(TOUCH_2, TOUCH_THRESH_NO_USE);
    touch_pad_config(TOUCH_3, TOUCH_THRESH_NO_USE);
    //interrupt of rising edge
    io_conf.intr_type = GPIO_INTR_POSEDGE;
    //bit mask of the pins, use GPIO4/5 here
    io_conf.pin_bit_mask = GPIO_INPUT_PIN_SEL;
    //set as input mode
    io_conf.mode = GPIO_MODE_INPUT;
    //enable pull-up mode
    io_conf.pull_up_en = 0;
    gpio_config(&io_conf);
    gpio_set_intr_type(GPIO_INPUT_IO_0, GPIO_INTR_POSEDGE);
    gpio_install_isr_service(ESP_INTR_FLAG_DEFAULT);
    //hook isr handler for specific gpio pin
    gpio_isr_handler_add(GPIO_INPUT_IO_0, gpio_isr_handler, (void*) GPIO_INPUT_IO_0);


}

int touch_handle_init(void){
    ESP_ERROR_CHECK(touch_pad_init());
    touch_pad_set_voltage(TOUCH_HVOLT_2V7, TOUCH_LVOLT_0V5, TOUCH_HVOLT_ATTEN_1V);
    touch_gpio_initialize();
    if (regs_global_part1.vars.filter_use){
        touch_pad_filter_start(TOUCHPAD_FILTER_TOUCH_PERIOD);
    }
    return 0;
}
void touch_task(void *arg){
    (void) (*arg);
    uint32_t signal_value;
    touch_handle_init();
    u32 enable_period=0;
    float enable_period_float = regs_global_part1.vars.liters;
    u32 driver_enabled_tick_os = task_get_tick_count();
    float driver_enabled_liters = regs_global_part1.vars.liters;
    u8 drive_enable = 0;
    u64 ticks=0;
    u16 touch_0,touch_1,touch_2,touch_3;
    u16 touch_temp;
    uint16_t touch_value;
    touch_pad_read_raw_data(TOUCH_0, &touch_value);
    touch_pad_read_filtered(TOUCH_0, &touch_temp);
    regs_global_part1.vars.touch_0 = touch_temp;
    touch_0 = touch_temp;
    touch_pad_read_raw_data(TOUCH_1, &touch_value);
    touch_pad_read_filtered(TOUCH_1, &touch_temp);
    regs_global_part1.vars.touch_1 = touch_temp;
    touch_1 = touch_temp;
    touch_pad_read_raw_data(TOUCH_2, &touch_value);
    touch_pad_read_filtered(TOUCH_2, &touch_temp);
    regs_global_part1.vars.touch_2 = touch_temp;
    touch_2 = touch_temp;
    touch_pad_read_raw_data(TOUCH_3, &touch_value);
    touch_pad_read_filtered(TOUCH_3, &touch_temp);
    regs_global_part1.vars.touch_3 = touch_temp;
    touch_3 = touch_temp;
    u32 filter_use = regs_global_part1.vars.filter_use;
    while(1){
        if (regs_global_part1.vars.filter_use && !filter_use){
            filter_use = regs_global_part1.vars.filter_use;
            touch_pad_filter_start(TOUCHPAD_FILTER_TOUCH_PERIOD);
        }else if (!regs_global_part1.vars.filter_use && filter_use){
            filter_use = regs_global_part1.vars.filter_use;
            touch_pad_filter_stop();
        }
        if(task_notify_wait(STOP_CHILD_PROCCES|PACKET_RECEIVED, &signal_value, 10)==pdTRUE){
            /*by signal*/
            if (signal_value & STOP_CHILD_PROCCES){
            }else if(signal_value & PACKET_RECEIVED){
            }
        }
        if (filter_use){
            touch_pad_read_raw_data(TOUCH_0, &touch_value);
            touch_pad_read_filtered(TOUCH_0, &touch_temp);
            regs_global_part1.vars.touch_0 = touch_temp;
            touch_pad_read_raw_data(TOUCH_1, &touch_value);
            touch_pad_read_filtered(TOUCH_1, &touch_temp);
            regs_global_part1.vars.touch_1 = touch_temp;
            touch_pad_read_raw_data(TOUCH_2, &touch_value);
            touch_pad_read_filtered(TOUCH_2, &touch_temp);
            regs_global_part1.vars.touch_2 = touch_temp;
            touch_pad_read_raw_data(TOUCH_3, &touch_value);
            touch_pad_read_filtered(TOUCH_3, &touch_temp);
            regs_global_part1.vars.touch_3 = touch_temp;
        }else{
            touch_pad_read(TOUCH_0, &touch_temp);
            regs_global_part1.vars.touch_0 = touch_temp;
            touch_pad_read(TOUCH_1, &touch_temp);
            regs_global_part1.vars.touch_1 = touch_temp;
            touch_pad_read(TOUCH_2, &touch_temp);
            regs_global_part1.vars.touch_2 = touch_temp;
            touch_pad_read(TOUCH_3, &touch_temp);
            regs_global_part1.vars.touch_3 = touch_temp;
        }
        if(ticks%(regs_global_part1.vars.touch_handle_period/10)==0){
            float part_0 = (float)regs_global_part1.vars.touch_0/(float)touch_0;
            float part_1 = (float)regs_global_part1.vars.touch_1/(float)touch_1;
            float part_2 = (float)regs_global_part1.vars.touch_2/(float)touch_2;
            float part_3 = (float)regs_global_part1.vars.touch_3/(float)touch_3;
            if (part_0<regs_global_part1.vars.touch_0_trshld){
                touch_states |= TOUCH_0_STATE;
            }else{
                touch_states &= ~TOUCH_0_STATE;
            }
            if (part_1<regs_global_part1.vars.touch_1_trshld){
                touch_states |= TOUCH_1_STATE;
            }else{
                touch_states &= ~TOUCH_1_STATE;
            }
            if (part_2<regs_global_part1.vars.touch_2_trshld){
                touch_states |= TOUCH_2_STATE;
            }else{
                touch_states &= ~TOUCH_2_STATE;
            }
            if (part_3<regs_global_part1.vars.touch_3_trshld){
                touch_states |= TOUCH_3_STATE;
            }else{
                touch_states &= ~TOUCH_3_STATE;
            }
            touch_0 = regs_global_part1.vars.touch_0;
            touch_1 = regs_global_part1.vars.touch_1;
            touch_2 = regs_global_part1.vars.touch_2;
            touch_3 = regs_global_part1.vars.touch_3;
        }

        if(ticks%10==0){
            regs_global_part1.vars.liters = regs_global_part1.vars.water_counter*regs_global_part1.vars.impulse_cost;
        }
        if(ticks%360000==0){
            int index = regs_description_get_index_by_address(&regs_global_part1.vars.liters);
            if (index>=0){
                regs_template_t regs_template;
                regs_template.ind =(u16)index;
                if(regs_description_get_by_ind(&regs_template)>=0){
                    if (mirror_access_write(&regs_template)>=0){
                        main_printf(TAG, "succes wrote liters to mirror");
                    }else{
                        main_printf(TAG, "unsucces writing liters to mirror");
                    }
                }
            }
            index = regs_description_get_index_by_address(&regs_global_part1.vars.water_counter);
            if (index>=0){
                regs_template_t regs_template;
                regs_template.ind =(u16)index;
                if(regs_description_get_by_ind(&regs_template)>=0){
                    if (mirror_access_write(&regs_template)>=0){
                        main_printf(TAG, "succes wrote liters to mirror");
                    }else{
                        main_printf(TAG, "unsucces writing liters to mirror");
                    }
                }
            }

        }
        if ((touch_states & TOUCH_1_STATE) &&(drive_enable==0)){
            if(regs_global_part1.vars.by_time){
                enable_period = regs_global_part1.vars.touch_1_count;
            }else{
                enable_period_float = regs_global_part1.vars.touch_1_liters;
            }
            driver_enabled_tick_os = task_get_tick_count();
            drive_enable = 1;
        }
        if ((touch_states & TOUCH_2_STATE) &&(drive_enable==0)){
            if(regs_global_part1.vars.by_time){
                enable_period = regs_global_part1.vars.touch_2_count;
            }else{
                enable_period_float = regs_global_part1.vars.touch_2_liters;
            }
            driver_enabled_liters = regs_global_part1.vars.liters;
            driver_enabled_tick_os = task_get_tick_count();
            drive_enable = 1;
        }
        if ((touch_states & TOUCH_3_STATE) &&(drive_enable==0)){
            if(regs_global_part1.vars.by_time){
                enable_period = regs_global_part1.vars.touch_3_count;
            }else{
                enable_period_float = regs_global_part1.vars.touch_3_liters;
            }
            driver_enabled_liters = regs_global_part1.vars.liters;
            driver_enabled_tick_os = task_get_tick_count();
            drive_enable = 1;
        }
        if ((touch_states & TOUCH_0_STATE) &&(drive_enable==1)){
            if(regs_global_part1.vars.by_time){
                enable_period = 0;
            }
            driver_enabled_liters = regs_global_part1.vars.liters;
            driver_enabled_tick_os = task_get_tick_count();
            drive_enable = 0;
        }

        if (drive_enable){
            gpio_set_level(DRIVE_PIN, 1);
            if(regs_global_part1.vars.by_time){
                if (task_get_tick_count()-driver_enabled_tick_os>enable_period){
                    drive_enable = 0;
                }
            }else{
                if (regs_global_part1.vars.liters-driver_enabled_liters>enable_period_float){
                    drive_enable = 0;
                }
            }

            if (task_get_tick_count()-driver_enabled_tick_os>MAX_ENABLED_PERIOD_MS){
                drive_enable = 0;
            }
        }else{
            gpio_set_level(DRIVE_PIN, 0);
        }
        ticks++;
    }
}
static int handle_touch_value(u16 touch_0,u16 touch_1,u16 touch_2,u16 touch_3){
    int res=0;
    return res;
}

#endif //TOUCH_HANDLE
