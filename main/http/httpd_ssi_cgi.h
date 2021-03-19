/**
 * @file httpd_ssi_cgi.h
 * @author Shoma Gane <shomagan@gmail.com>
 *         Ayrat Girfanov <girfanov.ayrat@yandex.ru>
 * @defgroup free_rtos\lwip\src\apps\http
 * @ingroup free_rtos\lwip\src\apps\http
 * @version 0.1 
 * @brief  TODO!!! write brief in 
 */

#ifndef HTTPD_SSI_CGI_H
#define HTTPD_SSI_CGI_H 1
 
/*add includes below */
#include "type_def.h"
#include "main_debug.h"
#include "main_config.h"
#include "regs_description.h"

/*add includes before */
#ifdef __cplusplus 
   extern "C" {
#endif
/*add functions and variable declarations below */
u16 add_lwip_stats(regs_template_t*  regs_template,char* pc_insert,u16* sh);

/*add functions and variable declarations before */
#ifdef __cplusplus
}
#endif
#endif //HTTPD_SSI_CGI_H
