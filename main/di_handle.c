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
task_handle_t di_handle_id = NULL;
/*GPIO init*/
int di_handle_init(){
    int result = 0;
    return result;
}
/*GPIO deinit*/
int di_handle_deinit(){
    int result = 0;
    return result;
}

void di_handle_task(void *arg){
    (void)(*arg);
    uint32_t signal_value;
    di_handle_init();

    while(1){
        if(task_notify_wait(STOP_CHILD_PROCCES|PACKET_RECEIVED, &signal_value, 100)==pdTRUE){
            /*by signal*/
            if (signal_value & STOP_CHILD_PROCCES){
                di_handle_deinit();
                task_delete(task_get_id());
            }else if(signal_value & PACKET_RECEIVED){

            }
        }
    }
}
#endif //DI_HANDLER_C
