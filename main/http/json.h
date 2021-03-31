/**
 * @file    json.h
 * @author  Shoma Gane <shomagan@gmail.com>
 * @author  Ayrat Girfanov <girfanov.ayrat@yandex.ru>
 * @ingroup Json
 * @version 0.1
 */

#ifndef JSON_H
#define JSON_H 1
 
/*add includes below */
#include "type_def.h"
#include "lwip/apps/fs.h"

#include <string.h>
#include <stdlib.h>
#include <stdio.h>

/**
  * @addtogroup Json
  * @{
  */
#define LWIP_HTTPD_MAX_JSON_PAIRS   20
#define MAX_REGS_REQUEST_NUM 10
#define MAX_JSON_PARAM_LEN 20
#define MAX_JSON_VALUE_LEN 200
#define MAX_JSON_PAIRS 10
#define MAX_DINAMIC_JSON_FILE_LEN 512
#define ARC_ELEMENT_NUMBER 2
#define JSON_RESPONSE_TIMEOUT 1000
/**
  * @}
  */

/*add includes before */
#ifdef __cplusplus 
   extern "C" {
#endif
/*add functions and variable declarations below */


/**
  * @brief json_msg_t
  * @ingroup Json
  *
  * Types of value in json pack
  */
typedef enum{
   ERROR_JSON = 0,
   TEXT_JSON,
   NUMBER_JSON,
   FLOAT_JSON,
   ARRAY_JSON,
}json_msg_t;

/**
  * @brief json_pairs_t
  * @ingroup Json
  *
  * Struct for storage json pack pairs
  */
typedef struct{
    char * param[LWIP_HTTPD_MAX_JSON_PAIRS];
    char * value[LWIP_HTTPD_MAX_JSON_PAIRS];
    int pairs_number;
}json_pairs_t;

/**
  * @brief get_t
  * @ingroup Json
  *
  * Types of register value request
  * {GET_FULL - response contain full information about requested register,
  * GET_VALUE - response contain only value of requested register
  * }
  */
typedef enum{
   GET_NONE = 0,
   GET_FULL,
   GET_VALUE,
}get_t;

/**
  * @brief request_t
  * @ingroup Json
  *
  * Types of json requests
  * {OS_PARAM_REQUEST - response contain information about current OS, main OS and OS_version,
  * TASK_PARAM_REQUEST - not implemented yet,
  * REGS_REQUEST - response contain information about requested register (full or only value),
  * REGS_NUM_REQUEST - response contain number of registers including user_task vars,
  * REGS_SET_REQUEST - request contain a register id and new value for change it
  * }
  */
typedef enum{
   NONE_REQUEST = 0,
   OS_PARAM_REQUEST,
   TASK_PARAM_REQUEST,
   REGS_REQUEST,
   REGS_NUM_REQUEST,
   REGS_SET_REQUEST,
   ARC_GET_REQUEST,
   LOG_GET_REQUEST,
   SET_STRING
}request_t;

/**
  * @brief json_out_st
  * @ingroup Json
  *
  * State of dynamic_json_file
  * {JSON_OUT_FREE - dynamic_json_file not in use,
  * JSON_OUT_BUILD - dynamic_json_file during recording,
  * JSON_OUT_SENDING - dynamic_json_file during sending
  * }
  */
typedef enum{
   JSON_OUT_FREE = 0,
   JSON_OUT_BUILD,
   JSON_OUT_SENDING,
}json_out_st;

/**
  * @brief json_out_t
  * @ingroup Json
  *
  * Struct for dynamic_json_file and it state
  */
typedef struct{
   struct fs_file file;
   json_out_st busy;
}json_out_t;

extern const char json_header[];
extern const char method_list[];
extern char dynamical_file_databuf[];
extern json_out_t dynamic_json_file;
extern const char json_cgi_request[];

void json_init_base(void);
const char* get_json_handle(char *json_pack);
char * extract_json_pack(char * headers);

/*add functions and variable declarations before */
#ifdef __cplusplus
}
#endif
#endif //JSON_H
