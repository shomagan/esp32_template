/**
 * @file battery_state.c
 * @author Shoma Gane <shomagan@gmail.com>
 *         
 * @defgroup main
 * @ingroup main
 * @version 0.1 
 * @brief  TODO!!! write brief in 
 */
#ifndef BATTERY_STATE_C
#define BATTERY_STATE_C 1
 

#include "battery_state.h"
#include "esp_log.h"
#include "esp_check.h"
#include "regs_description.h"
                       
task_handle_t battery_state_handle_id = NULL;
static const char *TAG = "battery_state";
#define BATTERY_STATE_TASK_PERIOD (100u)
#if BATTERY_STATE
static int battery_state_init(void);
static int battery_state_deinit();

static int battery_state_init(){
   int result = 0;
   regs_global->vars.current_state[0] |= CS0_TASK_ACTIVE_BATTERY_STATE;
   return result;
}  
static int battery_state_deinit(){
   int result = 0;
   regs_global->vars.current_state[0] &= ~((u32)CS0_TASK_ACTIVE_BATTERY_STATE);
   return result;
}
void battery_state_task(void *arg){
   (void)(*arg);
   uint32_t signal_value;       
   battery_state_init();
   u64 task_counter = 0u;
   while(1){
      if(task_notify_wait(STOP_CHILD_PROCCES, &signal_value, BATTERY_STATE_TASK_PERIOD)==pdTRUE){
         /*by signal*/
         if (signal_value & STOP_CHILD_PROCCES){
            battery_state_deinit();
            task_delete(task_get_id());
         }
      }
      task_counter++;
   }
}
#endif //BATTERY_STATE
#endif /*BATTERY_STATE_C*/
