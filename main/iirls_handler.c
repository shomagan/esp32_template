/**
 * @file iirls_handler.c
 * @author Shoma Gane <shomagan@gmail.com>
 *         
 * @defgroup main
 * @ingroup main
 * @version 0.1 
 * @brief  TODO!!! write brief in 
 */
#ifndef IIRLS_HANDLER_C
#define IIRLS_HANDLER_C 1
#include "type_def.h"
#include "os_type.h"
#include "iirls_handler.h"
#include "modbus_tcp_client.h"
#include "regs.h"
#include "common.h"
#include <string.h>
#include "wifi_slip_main.h"

task_handle_t isimfw400_task_handle;
static const char *TAG = "iirls_task";
u16 slave_adc_buffer[100];
/**
 * @brief duty_task
 * @param pvParameters
 */
void isimfw400_task(void *pvParameters ){
    (void)pvParameters;
    uint32_t task_tick = 0;
    task_tick = xTaskGetTickCount();
    u16 init_time=0;
    const int device_config = DEVICE_IIRLS_MASTER;
    ui32 prev_signal=0;
    uint32_t signal_value;
    int event_is_signal;
    int itt = 0;
    u8 first_attempt = 1;
    for( ;; ){
        {
            if(itt%1000==0){
                /*u8g2_ClearBuffer(&u8g2);
                u8g2_SetFont(&u8g2, u8g2_font_ncenB08_tr);
                char temp[30];
                sprintf(temp,"tx/rx - %lu",regs_global.vars.modbus_master_succ_transactions_number);
                u8g2_DrawStr(&u8g2, 2,10,temp);
                sprintf(temp,"err - %lu",regs_global.vars.modbus_master_error_transactions_number);
                u8g2_DrawStr(&u8g2, 2,25,temp);
                u8g2_SendBuffer(&u8g2);*/
            }
            /**/
            ///reading analog RAW data, i need to see RAW for calibration
            static u8 prefetch_started = 0;
            u8 end_of_cycle = 0;
            if(is_prefetching_state() ){
                if (prefetch_started){
                    if (init_time >= IIRLSCALC_DATA_BUFFER_LENGTH){
                        end_prefetch_adc_buffer();
                        prefetch_started = 0;
                        init_time = 0;
                    }else{
                        init_time++;
                    }
                }else{
                    prefetch_started = 1;
                    init_time = 0;
                }
            }else{
                //real input parameters calibration and data format coefficients.
                if (regs_global.vars.iirls_state){
                    if (init_time==0){
                        if(device_config == DEVICE_IIRLS_MASTER){
                            /*start procedure*/
                            // 1. set readout period
                            u8 paired;
                            memset(slave_adc_buffer,0,IIRLSCALC_DATA_BUFFER_LENGTH*2);
                            semaphore_take(regs_access_mutex, portMAX_DELAY );{
                                paired = (u8)regs_global.vars.iirls_pairing_status;
                            }semaphore_release(regs_access_mutex);
                            if (paired!=IIRLS_PAIRED){
                                continue;
                            }
                            if(first_attempt){
                                first_attempt=0;
                                main_printf(TAG,"tcp comm receive info");
                                task_notify_send(modbus_master_id, MODBUS_MASTER_READ_1_SIGNAL,&prev_signal);
                                signal_value = 0;
                                event_is_signal = task_notify_wait(STOP_CHILD_PROCCES|MODBUS_MASTER_COMMAND_COMPLETE_SUCCESSFULL|MODBUS_MASTER_COMMAND_COMPLETE_ERROR,&signal_value,250);
                            }
                            //1.1. set state
                            task_notify_send(modbus_master_id, MODBUS_MASTER_PERIODIC_CONFIRMATION_SIGNAL,&prev_signal);
                            signal_value = 0;
                            event_is_signal = task_notify_wait(STOP_CHILD_PROCCES|MODBUS_MASTER_COMMAND_COMPLETE_SUCCESSFULL|MODBUS_MASTER_COMMAND_COMPLETE_ERROR,&signal_value,250);
                            task_notify_send(modbus_master_id, MODBUS_MASTER_SET_0_SIGNAL,&prev_signal);
                            signal_value = 0;
                            event_is_signal = task_notify_wait(STOP_CHILD_PROCCES|MODBUS_MASTER_COMMAND_COMPLETE_SUCCESSFULL|MODBUS_MASTER_COMMAND_COMPLETE_ERROR,&signal_value,250);
                            if(event_is_signal != pdTRUE){
                                main_error_message(TAG,"Failed %s:%d\n",__FILE__,__LINE__);
                                continue;
                            }else if(signal_value & MODBUS_MASTER_COMMAND_COMPLETE_ERROR){
                                main_error_message(TAG,"Failed %s:%d\n",__FILE__,__LINE__);
                                continue;
                            }
                            // 2. enable
                            task_notify_send(modbus_master_id, MODBUS_MASTER_SET_1_SIGNAL,&prev_signal);
                            signal_value = 0;
                            event_is_signal = task_notify_wait(STOP_CHILD_PROCCES|MODBUS_MASTER_COMMAND_COMPLETE_SUCCESSFULL|MODBUS_MASTER_COMMAND_COMPLETE_ERROR,&signal_value,250);
                            if(event_is_signal != pdTRUE){
                                main_error_message(TAG,"Failed %s:%d\n",__FILE__,__LINE__);
                                continue;
                            }else if(signal_value & MODBUS_MASTER_COMMAND_COMPLETE_ERROR){
                                main_error_message(TAG,"Failed %s:%d\n",__FILE__,__LINE__);
                                continue;
                            }
                        }
                    }
                    if ((++init_time)>=IIRLSCALC_DATA_BUFFER_LENGTH){
                        init_time=0;
                       //memcpy(iirls_real_time_data_end.vars.iirls_receiver2_data,adc1_buffer,IIRLSCALC_DATA_BUFFER_LENGTH*2);
                        if(device_config == DEVICE_IIRLS_SLAVE){
                            regs_global.vars.iirls_state = 0;
                        }else{
                            //step 3 read adc buffer from slave
                            task_notify_send(modbus_master_id, MODBUS_MASTER_READ_0_SIGNAL,&prev_signal);
                            signal_value = 0;
                            event_is_signal = task_notify_wait(STOP_CHILD_PROCCES|MODBUS_MASTER_COMMAND_COMPLETE_SUCCESSFULL|MODBUS_MASTER_COMMAND_COMPLETE_ERROR,&signal_value,250);
                            if(event_is_signal != pdTRUE){
                                main_error_message(TAG,"Failed %s:%d\n",__FILE__,__LINE__);
                                continue;
                            }else if(signal_value & MODBUS_MASTER_COMMAND_COMPLETE_ERROR){
                                main_error_message(TAG,"Failed %s:%d\n",__FILE__,__LINE__);
                                continue;
                            }
                            end_of_cycle =1;
                        }
                    }
                }else{
                    init_time = 0;
                }
                if ((device_config == DEVICE_IIRLS_MASTER) && (init_time == 0)) {
                    if (end_of_cycle){
                        end_of_cycle = 0;
                        task_notify_send(modbus_master_id, MODBUS_MASTER_SET_2_SIGNAL,&prev_signal);
                        signal_value = 0;
                        event_is_signal = task_notify_wait(STOP_CHILD_PROCCES|MODBUS_MASTER_COMMAND_COMPLETE_SUCCESSFULL|MODBUS_MASTER_COMMAND_COMPLETE_ERROR,&signal_value,250);
                        if(event_is_signal != pdTRUE){
                            main_error_message(TAG,"Failed %s:%d\n",__FILE__,__LINE__);
                            continue;
                        }else if(signal_value & MODBUS_MASTER_COMMAND_COMPLETE_ERROR){
                            main_error_message(TAG,"Failed %s:%d\n",__FILE__,__LINE__);
                            continue;
                        }
                    }

                }
            }
        }
        itt++;
        vTaskDelayUntil(&task_tick, 1);
    }
}

#endif //IIRLS_HANDLER_C
