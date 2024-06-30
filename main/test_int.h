/**
 * @file test_int.h
 * @author Shoma Gane <shomagan@gmail.com>
 *         
 * @defgroup main
 * @ingroup main
 * @version 0.1 
 * @brief  TODO!!! write brief in 
 */
#ifndef TEST_INT_H
#define TEST_INT_H 1
 
/*add includes below */
#include "type_def.h"
#include "main_config.h"
#include "regs.h"
#include "common.h"
#include "pin_map.h"

/*add includes before */
#ifdef __cplusplus 
   extern "C" {
#endif
/*add functions and variable declarations below */

extern task_handle_t test_int_handle_id;

#if TEST_INT

extern void test_int_task(void *arg);

#endif //TEST_INT


/*add functions and variable declarations before */
#ifdef __cplusplus
}
#endif
#endif /*TEST_INT_H*/
