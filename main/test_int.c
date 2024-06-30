/**
 * @file test_int.c
 * @author Shoma Gane <shomagan@gmail.com>
 *         
 * @defgroup main
 * @ingroup main
 * @version 0.1 
 * @brief  TODO!!! write brief in 
 */
#ifndef TEST_INT_C
#define TEST_INT_C 1
 

#include "test_int.h"
#include "esp_log.h"
#include "esp_check.h"
#include "regs_description.h"
                       
task_handle_t test_int_handle_id = NULL;
static const char *TAG = "test_int";
#define TEST_INT_TASK_PERIOD (2000u)
#if TEST_INT
static int test_int_init(void);
static int test_int_deinit();
static int test_int_regs_description(void);

static int test_int_init(){
   int result = 0;
   regs_global.vars.current_state[0] |= CS0_TASK_ACTIVE_TEST_INT;
   return result;
}  
static int test_int_deinit(){
   int result = 0;
   regs_global.vars.current_state[0] &= ~((u32)CS0_TASK_ACTIVE_TEST_INT);
   return result;
}
void test_int_task(void *arg){
   (void)(*arg);
   uint32_t signal_value;
   test_int_init();
   u64 task_counter = 0u;
   while(1){
      if(task_notify_wait(STOP_CHILD_PROCCES, &signal_value, TEST_INT_TASK_PERIOD)==pdTRUE){
         /*by signal*/
         if (signal_value & STOP_CHILD_PROCCES){
            test_int_deinit();
            task_delete(task_get_id());
         }
      }
      test_int_regs_description();
      task_counter++;
   }
}
static int test_int_regs_description(void){
   int result = RES_OK;
   regs_template_t regs_template;
   regs_template.guid = 0;
   result += regs_description_get_by_guid(&regs_template);
   if (regs_template.modbus_description != 0x30000){
      result += RES_ERROR;
      main_error_message(TAG,"Failed %s:%d\n",__FILE__,__LINE__);
   }
   regs_template.guid = 148;
   result += regs_description_get_by_guid(&regs_template);
   if (regs_template.ind != 22){
      result += RES_ERROR;
      main_error_message(TAG,"Failed %s:%d\n",__FILE__,__LINE__);
   }
   regs_template.guid = 630;
   result += regs_description_get_by_guid(&regs_template);
   if (regs_template.type != FLOAT_REGS_FLAG){
      result += RES_ERROR;
      main_error_message(TAG,"Failed %s:%d\n",__FILE__,__LINE__);
   }
   if(result == RES_OK){
      main_printf(TAG,"regs_description_get_by_guid passed %i",result);
   }
   return result;
}
#endif //TEST_INT
#endif /*TEST_INT_C*/
