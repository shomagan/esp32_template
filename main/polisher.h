/**
 * @file polisher.h
 * @author Shoma Gane <shomagan@gmail.com>
 *         
 * @defgroup main
 * @ingroup main
 * @version 0.1 
 * @brief  TODO!!! write brief in 
 */
#ifndef POLISHER_H
#define POLISHER_H 1
 
/*add includes below */
#include "type_def.h"
#include "main_config.h"
#include "regs.h"
#include "common.h"

/*add includes before */
#ifdef __cplusplus 
   extern "C" {
#endif
/*add functions and variable declarations below */

extern task_handle_t polisher_handle_id;

#if POLISHER

extern void polisher_task(void *arg);

#endif //POLISHER


/*add functions and variable declarations before */
#ifdef __cplusplus
}
#endif
#endif /*POLISHER_H*/
