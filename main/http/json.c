/**
 * @file    json.c
 * @brief   This file implements functions for work with json packs
 * @author  Shoma Gane <shomagan@gmail.com>
 * @author  Ayrat Girfanov <girfanov.ayrat@yandex.ru>
 * @version 0.1
 * @ingroup Json
 */

#ifndef JSON_C
#define JSON_C 1

/**
  * @defgroup Json
  * @brief Json packets description
  */

/** @addtogroup Json
  * @{
  */
#include "json.h"
#include "regs_description.h"
#include "regs.h"
#include "httpd_structs.h"
#include "httpd_ssi_cgi.h"
#include "os_type.h"
#include <math.h>
#include <stdarg.h>
#include <stdio.h>
static const char *TAG = "json";
/**
 * @}
 */

/**
 * @brief json_header
 * header for method GET
 * @ingroup Json
 */
const char json_header[] = "HTTP/1.0 200 OK\r\nServer:sofi_1.0.0\r\nAccess-Control-Allow-Origin: *\r\n\r\n";

/**
 * @brief method_list
 * list of methods (GET, POST)
 * @ingroup Json
 */
const char method_list[] = "HTTP/1.0 200 OK\r\nAccess-Control-Allow-Methods: GET, POST\r\nAccess-Control-Allow-Origin: "
                           "*\r\nAccess-Control-Allow-Headers: Destination, Json \r\nAccess-Control-Max-Age: 86400\r\n\r\n\0";

/**
 * @brief dynamical_file_databuf
 * buffer for collect request data
 * @ingroup Json
 */
char dynamical_file_databuf[MAX_DINAMIC_JSON_FILE_LEN];

/**
 * @brief json_cgi_request
 * @ingroup Json
 * @todo add brief
 */
const char json_cgi_request[] = "json_request";
/**
 * @brief dynamic_json_file
 * dynamic page with request data
 * @ingroup Json
 */
json_out_t dynamic_json_file;

static int json_parse(json_pairs_t *json, char *json_pack);
static json_msg_t json_get_msg_type(char *data);
static int json_get_array_elem_nmb(char *array);
static int json_get_array_elements(char *json_array, u64 *buff, int num);
static operand_t json_get_elements(char *str_value, regs_template_t *regs_template);
static int json_out_overflow = 0;

static void json_out_append(const char *format, ...) {
   int free_space = MAX_DINAMIC_JSON_FILE_LEN - dynamic_json_file.file.len;
   if (json_out_overflow || (free_space <= 1)) {
      json_out_overflow = 1;
      return;
   }
   va_list args;
   va_start(args, format);
   int written = vsnprintf(dynamical_file_databuf + dynamic_json_file.file.len, (size_t)free_space, format, args);
   va_end(args);
   if ((written < 0) || (written >= free_space)) {
      json_out_overflow = 1;
   } else {
      dynamic_json_file.file.len += written;
   }
}

static void json_out_replace_last(char symbol) {
   if (dynamic_json_file.file.len > 0) {
      dynamical_file_databuf[dynamic_json_file.file.len - 1] = symbol;
   }
}

void json_init_base(void) {
   dynamic_json_file.file.data = dynamical_file_databuf;
   dynamic_json_file.file.flags |= FS_FILE_FLAGS_HEADER_INCLUDED;
   dynamic_json_file.busy = JSON_OUT_FREE;
}

/**
 * @brief extract_json_pack
 * Find json pack in header
 * @param headers
 * @return pointer to first symbol of json pack
 * @ingroup Json
 */
char *extract_json_pack(char *headers) {
   char *p;
   char *json_pack;

   /* Find Json header */
   /* Get a pointer to headers */
   //headers = hs->req->payload;
   for (u8 i = 0; i < 10; i++) {
      p = strstr(headers, "Json: ");
      if (p == 0) {
         p = strstr(headers, "json: ");
      } else {
         break;
      }
      if (p == 0) {
         p = strstr(headers, "JSON: ");
      } else {
         break;
      }
      if (p == 0) {
         headers += (strlen(headers) + 1);
      } else {
         break;
      }
   }

   /* Find start of json_pack */
   if (p == NULL) {
      return NULL;
   }
   json_pack = strchr(p, '{');
   if (json_pack == NULL) {
      return NULL;
   }
   p = strchr(json_pack, '}');
   if (p == NULL) {
      return NULL;
   }
   p++;
   *p = '\0';
   return json_pack;
}

/**
 * @brief json_parce
 * @param json a pointer to variable to save json pairs
 * @param json_pack a pointer to first symbol of json pack
 * @return number of json pairs
 * @ingroup Json
 *
 * This functions find json pairs PARAM:VALUE in string and
 * save pointers to PARAMs and VALUEs in param "json"
 */
//{name:[1,1,2,3,4],name:2}
static int json_parse(json_pairs_t *json, char *json_pack) {
   char *position;
   char *array_pointer; // "["->"]"
   char *determiner; // ":"
   //json_pack[MAX_DINAMIC_JSON_FILE_LEN-1] = '\0'; //add edge for safety
   for (int i = 0; i < LWIP_HTTPD_MAX_JSON_PAIRS; i++) {
      json->param[i] = NULL;
      json->value[i] = NULL;
   }
   json->pairs_number = 0;
   if (*(json_pack + strlen(json_pack) - 1) == '}') {
      *(json_pack + strlen(json_pack) - 1) = '\0';
   }
   position = json_pack + 1;
   for (int loop = 0; (loop < MAX_JSON_PAIRS) && position; loop++) {
      /*save name */
      json->param[json->pairs_number] = position;
      /* Now find the ':' in the previous pair, replace it with '\0' and save
        * the parameter value string. */
      position = strchr(position, ':');
      if (position) {
         *position = '\0';
         position++;
         array_pointer = strchr(position, '['); //if json_pack have array
         determiner = strchr(position, ':');
         if ((array_pointer != NULL)) {
            while ((*array_pointer != ']') && ((array_pointer < determiner) || (determiner == NULL))) {
               if (*array_pointer == ',') {
                  *array_pointer = ';'; // replace "," to ";" in array
               }
               array_pointer++;
            }
         }

         json->value[json->pairs_number] = position;
      } else {
         main_error_message(TAG, "Failed %d\n", __LINE__);
         break;
      }
      json->pairs_number++;
      position = strchr(position, ',');
      if (position) {
         *position = '\0';
         position++;
      }
   }
   return json->pairs_number;
}


/**
 * @brief get_json_handle
 * @param json_pack - string with json pack
 * @return json_cgi_request
 * @ingroup Json
 *
 * This function recognize request type and create response message with requested data
 */
const char *get_json_handle(char *json_pack) {
   if (json_pack == NULL) {
      return NULL;
   }
   json_pairs_t json_pairs;
   int params = json_parse(&json_pairs, json_pack);
   request_t request = NONE_REQUEST;
   get_t get_type = GET_NONE;
   u8 reg_num = 0;
   u64 reg_index[MAX_REGS_REQUEST_NUM] = { 0 };
   char *p_databuf;
   u64 val_u64 = 0;
   i8 temp_i8 = 0;
   s16 temp_i16 = 0;
   s32 temp_i32 = 0;
   s64 temp_i64 = 0;
   float val_float = 0.0f;
   double val_double = 0.0;
   regs_template_t regs_template = { 0 };

   if (params != 0) {
      //read params
      for (u8 i = 0; i < params; i++) {
         //delete " symbols before compare
         if (json_pairs.param[i] != NULL && json_pairs.value[i] != NULL) {
            json_get_msg_type(json_pairs.param[i]);
            json_get_msg_type(json_pairs.value[i]);
            if (strcmp(json_pairs.param[i], "request") == 0) {
               if (strcmp(json_pairs.value[i], "os_param") == 0) {
                  request = OS_PARAM_REQUEST;
               } else if (strcmp(json_pairs.value[i], "task_param") == 0) {
                  request = TASK_PARAM_REQUEST;
               } else if (strcmp(json_pairs.value[i], "regs_num") == 0) {
                  request = REGS_NUM_REQUEST;
               } else if (strcmp(json_pairs.value[i], "user_task_regs_num") == 0) {
                  request = USER_TASK_REGS_NUM_REQUEST;
               } else if (strcmp(json_pairs.value[i], "regs_set") == 0) {
                  request = REGS_SET_REQUEST;
               } else if (strcmp(json_pairs.value[i], "regs") == 0) {
                  request = REGS_REQUEST;
               } else if (strcmp(json_pairs.value[i], "set_string") == 0) {
                  request = SET_STRING;
               }
            } else if (strcmp(json_pairs.param[i], "reg_num") == 0) {
               reg_num = (u8)atoi(json_pairs.value[i]);
               if (reg_num > MAX_REGS_REQUEST_NUM) {
                  reg_num = MAX_REGS_REQUEST_NUM;
               }
            } else if (strcmp(json_pairs.param[i], "get_type") == 0) {
               if (strcmp(json_pairs.value[i], "full") == 0) {
                  get_type = GET_FULL;
               } else if (strcmp(json_pairs.value[i], "only_value") == 0) {
                  get_type = GET_VALUE;
               }
            } else if ((strcmp(json_pairs.param[i], "index") == 0) &&
                       (request == REGS_REQUEST || request == REGS_SET_REQUEST || request == SET_STRING)) {
               json_msg_t json_msg = json_get_msg_type(json_pairs.value[i]);
               if (json_msg == NUMBER_JSON) {
                  reg_index[0] = (u16)atoi(json_pairs.value[i]);
               } else if (json_msg == ARRAY_JSON) {
                  if (reg_num == json_get_array_elem_nmb(json_pairs.value[i])) {
                     if (reg_num <= MAX_REGS_REQUEST_NUM) {
                        json_get_array_elements(json_pairs.value[i], reg_index, reg_num);
                     }
                  } else {
                     main_error_message(TAG, "Failed %s:%d\n", __FILE__, __LINE__);
                     break;
                  }
               }
            } else if ((strcmp(json_pairs.param[i], "name") == 0) && (request == REGS_REQUEST || request == REGS_SET_REQUEST)) {
               regs_template.name = json_pairs.value[i];
               int res = regs_description_get_index_by_name(&regs_template);
               if (res >= 0) {
                  reg_index[0] = res;
               } else {
                  reg_index[0] = 0;
               }
            } else if ((strcmp(json_pairs.param[i], "value") == 0) &&
                       (request == REGS_REQUEST || request == REGS_SET_REQUEST || request == SET_STRING)) {
               main_printf(TAG, "write reg %u", (u16)reg_index[0]);
               if (regs_description_get_by_index(&regs_template, reg_index[0]) == 0) {
                  if (request == SET_STRING) {
                     char *element_start;
                     char *element_end;
                     u8 temp_buff[64] = { 0 };
                     u8 size = 0;
                     element_start = json_pairs.value[i];
                     while ((*element_start) == '"') {
                        element_start++;
                     }
                     element_end = element_start;
                     while ((*element_end) != '"' && (*element_end) != 0) {
                        element_end++;
                        size++;
                        if (size >= 64) {
                           break;
                        }
                     }
                     memcpy(temp_buff, element_start, size);
                     main_printf(TAG, "write reg %u", (u16)reg_index[0]);
                     regs_set_buffer(regs_template.p_value, temp_buff, regs_template.size_in_bytes);
                  } else {
                     char *element;
                     element = json_pairs.value[i];
                     while ((*element == '"') || (*element == '[')) {
                        element++;
                     }
                     for (u32 j = 0; j < regs_template.size; j++) {
                        operand_t value;
                        value = json_get_elements(element, &regs_template);
                        regs_access_t reg;
                        reg.flag = regs_template.type;
                        memcpy(&reg.value, &value, 8);
                        regs_set((void *)((u32)regs_template.p_value + regs_size_in_byte(regs_template.type) * j), reg);
                        element = strchr(element, ';');
                        if (element) {
                           element++;
                           while (*element == ' ') {
                              element++;
                           }
                        } else {
                           break;
                        }
                     }
                  }
               }
            }
         } else {
            break;
         }
      }
      // check json_file busy flag
      u32 time = task_get_tick_count();
      while ((dynamic_json_file.busy != JSON_OUT_FREE) && (task_get_tick_count() < (time + JSON_RESPONSE_TIMEOUT))) {
         task_delay_ms(1);
      }
      if (dynamic_json_file.busy != JSON_OUT_FREE) { //check timeout
         main_printf(TAG, "json_file busy timeout");
      } else { //response building
         dynamic_json_file.busy = JSON_OUT_BUILD;

         p_databuf = dynamical_file_databuf;
         dynamic_json_file.file.len = 0;
         json_out_overflow = 0;
         json_out_append("%s{", json_header);
         switch (request) {
         case OS_PARAM_REQUEST:
            json_out_append("\"current\":%d,\"main\":%d,\"os_ver\":[", 0, 0);
            for (u8 i = 0; i < FW_VERSION_SIZE; i++) {
               json_out_append("%d,", (int)regs_global->vars.fw_version[i]);
            }
            json_out_replace_last(']');
            json_out_append(",");
            break;
         case TASK_PARAM_REQUEST:
            break;
         case REGS_REQUEST:
            for (u8 i = 0; i < reg_num; i++) {
               int res = regs_description_get_by_index(&regs_template, reg_index[i]);
               json_out_append(" \"index\" :%d,", (int)reg_index[i]);
               if ((get_type == GET_FULL) && (res == 0)) {
                  json_out_append(" \"name\" :\"%s\", \"type\" :%d, \"byte_address\" :%d, \"flags\" :%d,", regs_template.name,
                                  (int)regs_template.type, (int)regs_template.guid, (int)regs_template.property);
                  json_out_append(" \"description\" :\"%s\", \"modbus_function\" :%u, \"modbus_address\" :%u,",
                                  regs_template.description, (u8)RD_MDB_FUNCTION(regs_template.modbus_description),
                                  (u16)RD_MDB_ADDRESS(regs_template.modbus_description));
               }
               json_out_append(" \"value\" :");
               if (res == 0) {
                  u16 temp_len = 0;
                  if (add_lwip_stats(&regs_template, p_databuf + dynamic_json_file.file.len, &temp_len) > 0) {
                     char *p = p_databuf + dynamic_json_file.file.len;
                     *p = '\"';
                     *(p + temp_len - 1) = '\"';
                     for (int i = 0; i < temp_len; i++) {
                        if ((*p == '\n') || (*p == '\t')) {
                           *p = ' ';
                        }
                        p++;
                     }
                     dynamic_json_file.file.len += temp_len;
                     *(p_databuf + dynamic_json_file.file.len) = ',';
                     dynamic_json_file.file.len++;
                  } else {
                     if (regs_template.size > 1) {
                        json_out_append("[");
                     }
                     semaphore_take(regs_access_mutex, portMAX_DELAY);
                     {
                        for (u16 i = 0; i < regs_template.size; i++) {
                           switch (regs_template.type) {
                           case U8_REGS_FLAG:
                           case U16_REGS_FLAG:
                           case U32_REGS_FLAG:
                           case U64_REGS_FLAG:
                           case TIME_REGS_FLAG:
                              val_u64 = 0;
                              memcpy(&val_u64, (regs_template.p_value + i * regs_size_in_byte(regs_template.type)),
                                     regs_size_in_byte(regs_template.type));
                              json_out_append("%llu,", val_u64);
                              break;
                           case I8_REGS_FLAG:
                              temp_i8 = 0;
                              memcpy(&temp_i8, (regs_template.p_value + i * regs_size_in_byte(regs_template.type)),
                                     regs_size_in_byte(regs_template.type));
                              json_out_append("%i,", temp_i8);
                              break;
                           case S16_REGS_FLAG:
                              temp_i16 = 0;
                              memcpy(&temp_i16, (regs_template.p_value + i * regs_size_in_byte(regs_template.type)),
                                     regs_size_in_byte(regs_template.type));
                              json_out_append("%i,", temp_i16);
                              break;
                           case S32_REGS_FLAG:
                           case INT_REGS_FLAG:
                              temp_i32 = 0;
                              memcpy(&temp_i32, (regs_template.p_value + i * regs_size_in_byte(regs_template.type)),
                                     regs_size_in_byte(regs_template.type));
                              json_out_append("%li,", temp_i32);
                              break;
                           case S64_REGS_FLAG:
                              temp_i64 = 0;
                              memcpy(&temp_i64, (regs_template.p_value + i * regs_size_in_byte(regs_template.type)),
                                     regs_size_in_byte(regs_template.type));
                              json_out_append("%lli,", temp_i64);
                              break;
                           case FLOAT_REGS_FLAG:
                              val_float = 0.0f;
                              memcpy(&val_float, (regs_template.p_value + i * regs_size_in_byte(regs_template.type)),
                                     regs_size_in_byte(regs_template.type));
                              if (isnanf(val_float) || isinff(val_float)) {
                                 json_out_append("\"%f\",", (double)val_float);
                              } else {
                                 json_out_append("%f,", (double)val_float);
                              }
                              break;
                           case DOUBLE_REGS_FLAG:
                              val_double = 0.0;
                              memcpy(&val_double, (regs_template.p_value + i * regs_size_in_byte(regs_template.type)),
                                     regs_size_in_byte(regs_template.type));
                              if (isnanf((float)val_double) || isinff((float)val_double)) {
                                 json_out_append("\"%f\",", val_double);
                              } else {
                                 json_out_append("%f,", val_double);
                              }
                              break;
                           }
                        }
                     }
                     semaphore_release(regs_access_mutex);
                     if (regs_template.size > 1) {
                        json_out_replace_last(']');
                        json_out_append(",");
                     }
                  }
               }
            }
            break;
         case REGS_NUM_REQUEST:
            json_out_append("\"regs_num\":%d,", (int)regs_global->vars.num_of_vars);
            break;
         case USER_TASK_REGS_NUM_REQUEST:
            json_out_append("\"user_task_regs_num\":%d,", (int)regs_global->vars.user_task_num_of_vars);
            break;
         case REGS_SET_REQUEST:
         case SET_STRING:
            json_out_append("\"status\":\"ok\",");
            break;
         default:
            json_out_append("\"error\":\"invalid request value\",");
            break;
         }

         json_out_replace_last('}');
         if (json_out_overflow) {
            dynamic_json_file.file.len = 0;
            json_out_overflow = 0;
            json_out_append("%s{\"error\":\"response too large\"}", json_header);
         }
         dynamical_file_databuf[dynamic_json_file.file.len] = '\0';

         dynamic_json_file.file.index = dynamic_json_file.file.len;
         dynamic_json_file.busy = JSON_OUT_SENDING;
         return json_cgi_request;
      }
   }

   return NULL;
}


/**
* @brief json_get_msg_type
* @param data json parameter or value
* @return see @json_msg_t
* @ingroup Json
*
* Function remove " symbols in data and return type of data
*/

static json_msg_t json_get_msg_type(char *data) {
   json_msg_t result = ERROR_JSON;
   /* clear message from " symbols */
   char *p = data + strlen(data) - 1;
   if ((*data == '\"') && (*p == '\"')) { //if first and last symbols is "
      p = data;
      do {
         *p = *(p + 1);
         p++;
      } while (*p != '\"');
      p--;
      *p = '\0';
   }

   if ((*data == '[') && (*p == ']')) { //if first and last symbol is '[' and ']'
      return ARRAY_JSON;
   }

   if (isdigit((int)*data)) {
      if (strchr(data, '.')) {
         result = FLOAT_JSON;
      } else {
         result = NUMBER_JSON;
      }
   } else {
      result = TEXT_JSON;
   }

   return result;
}

/**
 * @brief json_get_massive_elem_nmb
 * @param array json massive such as [1,2,3]
 * @return number of elements
 * @ingroup Json
 */
static int json_get_array_elem_nmb(char *array) {
   int nmb = 1;
   char *p = array;
   while (1) {
      p = strchr(p, ';');
      if (p) {
         nmb++;
         p++;
      } else {
         break;
      }
   }
   return nmb;
}

/**
 * @brief json_get_array_elements
 * @param json_array json massive such as [1,2,3]
 * @param buff array to send elements as text
 * @param num number of elements
 * @ingroup Json
 *
 * This function convert array elements from string to long and save its into buff[]
 */
static int json_get_array_elements(char *json_array, u64 *buff, int num) {
   int res = 0;
   char *p;
   char *q;
   p = json_array;
   p = strchr(p, '[');
   p++;
   for (int i = 0; i < num; i++) {
      if (*p == '\"') {
         p++;
      }
      buff[i] = (u64)strtol(p, &p, 10);
      q = strchr(p, ';');
      if (q == NULL) {
         q = strchr(p, ',');
         if (q == NULL) {
            break;
         }
      }
      p = q + 1;
   }
   return res;
}

/**
 * @brief json_get_elements
 * @param str_value value in string type
 * @param regs_template register type
 * @return value in the corresponding register type
 * @ingroup Json
 *
 * This function converts a value from a string to the corresponding register type
 */
static operand_t json_get_elements(char *str_value, regs_template_t *regs_template) {
   operand_t value;
   value.op_u64 = 0;
   switch (regs_template->type) {
   case (I8_REGS_FLAG):
   case (S16_REGS_FLAG):
   case (S32_REGS_FLAG):
   case (S64_REGS_FLAG):
   case (INT_REGS_FLAG):
      value.op_s64 = strtoll(str_value, 0, 10);
      break;
   case (U8_REGS_FLAG):
   case (U16_REGS_FLAG):
   case (U32_REGS_FLAG):
   case (TIME_REGS_FLAG):
      value.op_u32 = strtoul(str_value, 0, 10);
      break;
   case (U64_REGS_FLAG):
      value.op_u64 = strtoull(str_value, 0, 10);
      break;
   case (FLOAT_REGS_FLAG):
      value.op_f = (float)strtod(str_value, NULL);
      break;
   case (DOUBLE_REGS_FLAG):
      value.op_d = strtod(str_value, NULL);
      break;
   }
   return value;
}

#endif //JSON_C
