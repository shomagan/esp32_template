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
#include "link_functions.h"

task_handle_t test_int_handle_id = NULL;
static const char *TAG = "test_int";

/*#generator_use_description {"user_task_regs":"start_struct"}*/
static test_int_reg_t test_int_reg_storage = {{0}};
test_int_reg_t * const test_int_reg = &test_int_reg_storage;
#define NUM_OF_TEST_INT_REG_VARS 6
static u8 test_int_reg_saved_buf[8];
static const char test_int_reg_space_name[] = "test_int_reg_t";
static regs_description_t const regs_description_test_int_reg[NUM_OF_TEST_INT_REG_VARS] = {
    { NULL, NULL, NULL, (u8*)&test_int_reg->vars.test_int_state, 0,"current state","test_int_state", 0x14010000, 0x31130, U32_REGS_FLAG, 1, 3, 8 }//!<"current state" &ro
,
    { NULL, NULL, NULL, (u8*)&test_int_reg->vars.test_int_command, 0,"command","test_int_command", 0x14010001, 0x31132, U32_REGS_FLAG, 1, 3, 8 }//!<"command" &ro
,
    { NULL, NULL, NULL, (u8*)&test_int_reg->vars.test_int_component, 0,"component to be tested","test_int_component", 0x14010002, 0x31134, U32_REGS_FLAG, 1, 1, 8 }//!<"component to be tested"
,
    { NULL, NULL, NULL, (u8*)&test_int_reg->vars.test_int_type, 0,"test type - check, stress, performance","test_int_type", 0x14010003, 0x31136, U32_REGS_FLAG, 1, 1, 8 }//!<"test type - check, stress, performance"
,
    { NULL, NULL, NULL, (u8*)&test_int_reg->vars.test_int_result, 0,"overall result","test_int_result", 0x14010004, 0x31138, U32_REGS_FLAG, 1, 3, 8 }//!<"overall result" &ro
,
    { &def_table_version, NULL, NULL, (u8*)&test_int_reg->vars.table_version, 0,"table version, resets regs to defaults on mismatch","table_version", 0x14010005, 0x3113a, U32_REGS_FLAG, 1, 7, 8 }//!< "table version, resets regs to defaults on mismatch" &ro &save &def
,
};
const regs_description_list_t regs_table_test_int_reg = {
    .description = regs_description_test_int_reg,
    .num_of_regs = NUM_OF_TEST_INT_REG_VARS,
    .table_version = &def_table_version,
    .space_name = test_int_reg_space_name,
    .saved_regs_buffer = test_int_reg_saved_buf,
    .saved_regs_buffer_size = sizeof(test_int_reg_saved_buf),
};
/*#generator_use_description {"user_task_regs":"end_struct"}*/

#define TEST_INT_TASK_PERIOD (2000u)
#if TEST_INT
static int test_int_init(void);
static int test_int_deinit();
static int test_int_regs_description(void);

static int test_int_init(){
   int result = 0;
   regs_global->vars.current_state[0] |= CS0_TASK_ACTIVE_TEST_INT;
   int table_ind = link_functions.regs_description_list_add_new(regs_table_test_int_reg);
   link_functions.preinit_table_vars((u16)table_ind);
   return result;
}
static int test_int_deinit(){
   int result = 0;
   regs_global->vars.current_state[0] &= ~((u32)CS0_TASK_ACTIVE_TEST_INT);
   return result;
}
void test_int_task(void *arg){
   (void)arg;
   uint32_t signal_value;
   test_int_init();
   u64 task_counter = 0u;
   while(1){
      if(link_functions.os_thread_signal_wait(STOP_CHILD_PROCCES, &signal_value, TEST_INT_TASK_PERIOD)==pdTRUE){
         /*by signal*/
         if (signal_value & STOP_CHILD_PROCCES){
            test_int_deinit();
            link_functions.os_thread_terminate(link_functions.os_thread_get_id());
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

   return result;
}
#endif //TEST_INT
#endif /*TEST_INT_C*/
