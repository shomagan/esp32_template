/**
 * @file interrupt.h
 * @author Shoma Gane <shomagan@gmail.com>
 *         
 * @defgroup main/
 * @ingroup main/
 * @version 0.1 
 * @brief  TODO!!! write brief in 
 */
#ifndef INTERRUPT_H
#define INTERRUPT_H 1
 
/*add includes below */
#include "type_def.h"
#include "main_config.h"

/*add includes before */
#ifdef __cplusplus 
   extern "C" {
#endif
/*add functions and variable declarations below */
void IRAM_ATTR gpio_isr_handler(void* arg);
/*add functions and variable declarations before */
#ifdef __cplusplus
}
#endif
#endif //INTERRUPT_H
