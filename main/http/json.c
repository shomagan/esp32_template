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
const char method_list[] = "HTTP/1.0 200 OK\r\nAccess-Control-Allow-Methods: GET, POST\r\nAccess-Control-Allow-Origin: *\r\nAccess-Control-Allow-Headers: Destination, Json \r\nAccess-Control-Max-Age: 86400\r\n\r\n\0";

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
static json_msg_t json_get_msg_type(char * data);
static int json_get_array_elem_nmb(char * array);
static int json_get_array_elements(char* json_array, u64 * buff, int num);
static operand_t json_get_elements(char* str_value, regs_template_t * reg_template);
void json_init_base(void){
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
char * extract_json_pack(char * headers){
    char *p;
    char *json_pack;

    /* Find Json header */
    /* Get a pointer to headers */
    //headers = hs->req->payload;
    for (u8 i = 0; i < 10; i++) {
        p = strstr(headers, "Json: ");
        if(p == 0){
            p = strstr(headers, "json: ");
        }else{
            break;
        }
        if(p == 0){
            p = strstr(headers, "JSON: ");
        }else{
            break;
        }
        if(p == 0){
            headers += (strlen(headers) + 1);
        }else{
            break;
        }
    }

    /* Find start of json_pack */
    json_pack = strchr(p, '{');
    p = strchr(json_pack, '}');
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
static int json_parse(json_pairs_t *json, char *json_pack){
    char *position;
    char *array_pointer;    // "["->"]"
    char *determiner;// ":"
    //json_pack[MAX_DINAMIC_JSON_FILE_LEN-1] = '\0'; //add edge for safety
    for(int i=0;i<LWIP_HTTPD_MAX_JSON_PAIRS;i++){
        json->param[i] = NULL;
        json->value[i] = NULL;
    }
    json->pairs_number = 0;
    if(*(json_pack + strlen(json_pack)-1) == '}'){
        *(json_pack + strlen(json_pack)-1) = '\0';
    }
    position = json_pack +1;
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
            if((array_pointer!=NULL)){
                while((*array_pointer!= ']') &&
                      ((array_pointer < determiner)||(determiner==NULL))){
                    if(*array_pointer == ','){
                        *array_pointer = ';';   // replace "," to ";" in array
                    }
                    array_pointer++;
                }
            }

            json->value[json->pairs_number] = position;
        } else {
            main_error_message(TAG, "Failed %d\n",__LINE__);
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
const char* get_json_handle(char *json_pack){
    json_pairs_t json_pairs;
    int params = json_parse(&json_pairs, json_pack);
    request_t request = NONE_REQUEST;
    get_t get_type = GET_NONE;
    u8 reg_num = 0;
    u64 reg_index[MAX_REGS_REQUEST_NUM] = {0};
    char temp[32] = {0};
    char *p_databuf;
    u64 val_u64 = 0;
    i8 temp_i8 = 0;
    s16 temp_i16 = 0;
    s32 temp_i32 = 0;
    s64 temp_i64 = 0;
    float val_float = 0.0f;
    double val_double = 0.0;
    regs_template_t reg_template = {0};

    if(params !=0){
        //read params
        for(u8 i = 0; i < params; i++){
            //delete " symbols before compare
            if(json_pairs.param[i]!=NULL && json_pairs.value[i]!=NULL){
                json_get_msg_type(json_pairs.param[i]);
                json_get_msg_type(json_pairs.value[i]);
                if(strcmp(json_pairs.param[i], "request") == 0){
                    if(strcmp(json_pairs.value[i], "os_param") == 0){
                        request = OS_PARAM_REQUEST;
                    }else if(strcmp(json_pairs.value[i], "task_param") == 0){
                        request = TASK_PARAM_REQUEST;
                    }else if(strcmp(json_pairs.value[i], "regs_num") == 0){
                        request = REGS_NUM_REQUEST;
                    }else if(strcmp(json_pairs.value[i], "client_regs_num") == 0){
                        request = CLIENT_REGS_NUM_REQUEST;
                    }else if(strcmp(json_pairs.value[i], "regs_set") == 0){
                        request = REGS_SET_REQUEST;
                    }else if(strcmp(json_pairs.value[i], "regs") == 0){
                        request = REGS_REQUEST;
                    }else if(strcmp(json_pairs.value[i], "set_string") == 0){
                        request = SET_STRING;
                    }
                }else if(strcmp(json_pairs.param[i], "reg_num") == 0){
                    reg_num = (u8)atoi(json_pairs.value[i]);
                }else if(strcmp(json_pairs.param[i], "get_type") == 0){
                    if(strcmp(json_pairs.value[i], "full") == 0){
                        get_type = GET_FULL;
                    }else if(strcmp(json_pairs.value[i], "only_value") == 0){
                        get_type = GET_VALUE;
                    }
                }else if((strcmp(json_pairs.param[i], "index") == 0) && (request == REGS_REQUEST || request == REGS_SET_REQUEST || request == SET_STRING)){
                    json_msg_t json_msg = json_get_msg_type(json_pairs.value[i]);
                    if(json_msg == NUMBER_JSON){
                        reg_index[0] = (u16)atoi(json_pairs.value[i]);
                    }else if (json_msg == ARRAY_JSON){
                        if(reg_num == json_get_array_elem_nmb(json_pairs.value[i])){
                            if(reg_num < MAX_REGS_REQUEST_NUM){
                                json_get_array_elements(json_pairs.value[i],reg_index,reg_num);
                            }
                        }else{
                            main_error_message(TAG, "Failed %s:%d\n",__FILE__,__LINE__);
                            break;
                        }
                    }
                }else if((strcmp(json_pairs.param[i], "name") == 0) && (request == REGS_REQUEST || request == REGS_SET_REQUEST)){
                    reg_template.name = json_pairs.value[i];
                    if(regs_description_get_by_name (&reg_template)==0){
                        reg_index[0] = reg_template.ind;
                    }else{
                        reg_index[0] = 0;
                    }
                }else if((strcmp(json_pairs.param[i], "value") == 0) && (request == REGS_REQUEST || request == REGS_SET_REQUEST ||
                                                                         request == SET_STRING)){
                    reg_template.ind = (u16)reg_index[0];
                    main_printf(TAG,"write reg %u",reg_template.ind);
                    if(regs_description_get_by_ind (&reg_template)==0){
                        if (request == SET_STRING){
                            char * element_start;
                            char * element_end;
                            u8 temp_buff[64] = {0};
                            u8 size = 0;
                            element_start = json_pairs.value[i];
                            while((*element_start) == '"'){
                                element_start++;
                            }
                            element_end = element_start;
                            while((*element_end) != '"' && (*element_end) != 0){
                                element_end++;
                                size++;
                                if (size>=64){
                                    break;
                                }
                            }
                            memcpy(temp_buff, element_start, size);
                            main_printf(TAG,"write reg %u",reg_template.ind);
                            regs_set_buffer(reg_template.p_value, temp_buff, reg_template.size_in_bytes);
                        }else {
                            char * element;
                            element = json_pairs.value[i];
                            while((*element == '"')||(*element == '[')){
                                element++;
                            }
                            for (u32 j = 0; j < reg_template.size; j++) {
                                operand_t value ;
                                value = json_get_elements(element, &reg_template);
                                regs_access_t reg;
                                reg.flag = reg_template.type;
                                memcpy(&reg.value,&value,8);
                                regs_set((void*)((u32)reg_template.p_value+regs_size_in_byte(reg_template.type)*j), reg);
                                element = strchr(element, ';');
                                if(element){
                                    element += 2;
                                }else{
                                    break;
                                }
                            }
                        }
                    }
                }
            }else{
                break;
            }
        }
        // check json_file busy flag
        u32 time = task_get_tick_count();
        while((dynamic_json_file.busy != JSON_OUT_FREE)&&
            (task_get_tick_count() < (time + JSON_RESPONSE_TIMEOUT))){
            task_delay_ms(1);
        }
        if(dynamic_json_file.busy != JSON_OUT_FREE){ //check timeout
            main_printf(TAG, "json_file busy timeout");
        }else{  //response building
            dynamic_json_file.busy = JSON_OUT_BUILD;

            p_databuf = dynamical_file_databuf;
            dynamic_json_file.file.len = 0;
            strcpy(p_databuf + dynamic_json_file.file.len, json_header);  //add header in file
            dynamic_json_file.file.len += strlen(json_header);
            dynamic_json_file.file.len+=sprintf(p_databuf + dynamic_json_file.file.len, "{");
            switch(request){

            case OS_PARAM_REQUEST:
                dynamic_json_file.file.len+=sprintf(p_databuf + dynamic_json_file.file.len, "\"current\":");
                itoa((int)0, temp,10);
                strcpy(p_databuf + dynamic_json_file.file.len, temp);
                dynamic_json_file.file.len += strlen(temp);
                *(p_databuf + dynamic_json_file.file.len) = ',';
                dynamic_json_file.file.len++;
                dynamic_json_file.file.len+=sprintf(p_databuf + dynamic_json_file.file.len, "\"main\":");
                itoa((int)0, temp,10);
                strcpy(p_databuf + dynamic_json_file.file.len, temp);
                dynamic_json_file.file.len += strlen(temp);
                *(p_databuf + dynamic_json_file.file.len) = ',';
                dynamic_json_file.file.len++;
                dynamic_json_file.file.len+=sprintf(p_databuf + dynamic_json_file.file.len, "\"os_ver\":");
                *(p_databuf + dynamic_json_file.file.len) = '[';
                dynamic_json_file.file.len++;
                for(u8 i = 0; i < FW_VERSION_SIZE; i++){
                    itoa(regs_global.vars.fw_version[i], temp, 10);
                    strcpy(p_databuf + dynamic_json_file.file.len, temp);
                    dynamic_json_file.file.len += strlen(temp);
                    *(p_databuf + dynamic_json_file.file.len) = ',';
                    dynamic_json_file.file.len++;
                }
                *(p_databuf + dynamic_json_file.file.len - 1) = ']';  //replace last "," symbol to "]"
                *(p_databuf + dynamic_json_file.file.len) = ',';
                dynamic_json_file.file.len++;
                break;
            case TASK_PARAM_REQUEST:
                //add actions
                break;
            case REGS_REQUEST:
                //index
                for (u8 i = 0; i < reg_num; i++) {
                    int res = 0;
                    dynamic_json_file.file.len+=sprintf(p_databuf + dynamic_json_file.file.len, " \"index\" :");
                    reg_template.ind = (u16)reg_index[i];
                    res = regs_description_get_by_ind(&reg_template);
                    itoa((int)reg_index[i], temp, 10);
                    strcpy(p_databuf + dynamic_json_file.file.len, temp);
                    dynamic_json_file.file.len += strlen(temp);
                    *(p_databuf + dynamic_json_file.file.len) = ',';
                    dynamic_json_file.file.len++;
                    if(get_type == GET_FULL){
                        if(res==0){
                            //name
                            dynamic_json_file.file.len+=sprintf(p_databuf + dynamic_json_file.file.len, " \"name\" :\"");
                            strcpy(p_databuf + dynamic_json_file.file.len, reg_template.name);
                            dynamic_json_file.file.len += strlen(reg_template.name);
                            //type
                            dynamic_json_file.file.len+=sprintf(p_databuf + dynamic_json_file.file.len, "\", \"type\" :");
                            itoa((int)reg_template.type, temp,10);
                            strcpy(p_databuf + dynamic_json_file.file.len, temp);
                            dynamic_json_file.file.len += strlen(temp);
                            *(p_databuf + dynamic_json_file.file.len) = ',';
                            dynamic_json_file.file.len++;
                            //byte_address
                            dynamic_json_file.file.len+=sprintf(p_databuf + dynamic_json_file.file.len, " \"byte_address\" :");
                            itoa((int)reg_template.guid, temp,10);
                            strcpy(p_databuf + dynamic_json_file.file.len, temp);
                            dynamic_json_file.file.len += strlen(temp);
                            *(p_databuf + dynamic_json_file.file.len) = ',';
                            dynamic_json_file.file.len++;
                            //flags
                            dynamic_json_file.file.len+=sprintf(p_databuf + dynamic_json_file.file.len, " \"flags\" :");
                            itoa((int)reg_template.property, temp,10);
                            strcpy(p_databuf + dynamic_json_file.file.len, temp);
                            dynamic_json_file.file.len += strlen(temp);
                            *(p_databuf + dynamic_json_file.file.len) = ',';
                            dynamic_json_file.file.len++;
                            //description
                            dynamic_json_file.file.len+=sprintf(p_databuf + dynamic_json_file.file.len, " \"description\" :\"");
                            strcpy(p_databuf + dynamic_json_file.file.len, reg_template.description);
                            dynamic_json_file.file.len += strlen(reg_template.description);
                            dynamic_json_file.file.len+=sprintf(p_databuf + dynamic_json_file.file.len, "\",");
                            //modbus_function
                            dynamic_json_file.file.len+=sprintf(p_databuf + dynamic_json_file.file.len, " \"modbus_function\" :%u,",(u8)RD_MDB_FUNCTION(reg_template.modbus_description));
                            //modbus_function
                            dynamic_json_file.file.len+=sprintf(p_databuf + dynamic_json_file.file.len, " \"modbus_address\" :%u,",(u16)RD_MDB_ADDRESS(reg_template.modbus_description));
                        }
                    }
                    //value
                    dynamic_json_file.file.len+=sprintf(p_databuf + dynamic_json_file.file.len, " \"value\" :");
                    if(res==0){
                        u16 temp_len = 0;
                        if(add_lwip_stats(&reg_template,p_databuf + dynamic_json_file.file.len,&temp_len)>0){
                            char *p = p_databuf + dynamic_json_file.file.len;
                            *p = '\"';
                            *(p + temp_len - 1) = '\"';
                            for(int i=0;i<temp_len;i++){
                                if((*p == '\n')||(*p == '\t')){
                                    *p = ' ';
                                }
                                p++;
                            }
                            dynamic_json_file.file.len += temp_len;
                            *(p_databuf + dynamic_json_file.file.len) = ',';
                            dynamic_json_file.file.len++;
                        }else{
                            if(reg_template.size > 1){
                                *(p_databuf + dynamic_json_file.file.len) = '[';
                                dynamic_json_file.file.len++;
                            }
                            semaphore_take(regs_access_mutex, portMAX_DELAY);{
                                for (u16 i = 0; i < reg_template.size; i++) {
                                    switch (reg_template.type){
                                    case U8_REGS_FLAG:
                                    case U16_REGS_FLAG:
                                    case U32_REGS_FLAG:
                                    case U64_REGS_FLAG:
                                    case TIME_REGS_FLAG:
                                        val_u64 = 0;
                                        memcpy(&val_u64,(reg_template.p_value + i*regs_size_in_byte(reg_template.type)),regs_size_in_byte(reg_template.type));
                                        dynamic_json_file.file.len+=sprintf(p_databuf + dynamic_json_file.file.len,"%llu,",val_u64);
                                        break;
                                    case I8_REGS_FLAG:
                                        temp_i8 = 0;
                                        memcpy(&temp_i8,(reg_template.p_value + i*regs_size_in_byte(reg_template.type)),regs_size_in_byte(reg_template.type));
                                        dynamic_json_file.file.len+=sprintf(p_databuf + dynamic_json_file.file.len,"%i,",temp_i8);
                                        break;
                                    case S16_REGS_FLAG:
                                        temp_i16 = 0;
                                        memcpy(&temp_i16,(reg_template.p_value + i*regs_size_in_byte(reg_template.type)),regs_size_in_byte(reg_template.type));
                                        dynamic_json_file.file.len+=sprintf(p_databuf + dynamic_json_file.file.len,"%i,",temp_i16);
                                        break;
                                    case S32_REGS_FLAG:
                                    case INT_REGS_FLAG:
                                        temp_i32 = 0;
                                        memcpy(&temp_i32,(reg_template.p_value + i*regs_size_in_byte(reg_template.type)),regs_size_in_byte(reg_template.type));
                                        dynamic_json_file.file.len+=sprintf(p_databuf + dynamic_json_file.file.len,"%li,",temp_i32);
                                        break;
                                    case S64_REGS_FLAG:
                                        temp_i64 = 0;
                                        memcpy(&temp_i64,(reg_template.p_value + i*regs_size_in_byte(reg_template.type)),regs_size_in_byte(reg_template.type));
                                        dynamic_json_file.file.len+=sprintf(p_databuf + dynamic_json_file.file.len,"%lli,",temp_i64);
                                        break;
                                    case FLOAT_REGS_FLAG:
                                        val_float = 0.0f;
                                        memcpy(&val_float,(reg_template.p_value + i*regs_size_in_byte(reg_template.type)),regs_size_in_byte(reg_template.type));
                                        if(isnanf(val_float) || isinff(val_float)){
                                            dynamic_json_file.file.len+=sprintf(p_databuf + dynamic_json_file.file.len,"\"%f\",",(double)val_float);
                                        }else{
                                            dynamic_json_file.file.len+=sprintf(p_databuf + dynamic_json_file.file.len,"%f,",(double)val_float);
                                        }
                                        break;
                                    case DOUBLE_REGS_FLAG:
                                        val_double = 0.0;
                                        memcpy(&val_double,(reg_template.p_value + i*regs_size_in_byte(reg_template.type)),regs_size_in_byte(reg_template.type));
                                        if(isnanf((float)val_double) || isinff((float)val_double)){
                                            dynamic_json_file.file.len+=sprintf(p_databuf + dynamic_json_file.file.len,"\"%f\",",val_double);
                                        }else{
                                            dynamic_json_file.file.len+=sprintf(p_databuf + dynamic_json_file.file.len,"%f,",val_double);
                                        }
                                        break;
                                    }
                                }
                            }semaphore_release(regs_access_mutex);
                            if(reg_template.size > 1){
                                *(p_databuf + dynamic_json_file.file.len - 1) = ']';  //replace last "," symbol to "]"
                                *(p_databuf + dynamic_json_file.file.len) = ',';
                                dynamic_json_file.file.len++;
                            }
                        }
                    }
                }
                break;
            case REGS_NUM_REQUEST:
                dynamic_json_file.file.len+=sprintf(p_databuf + dynamic_json_file.file.len, "\"regs_num\":");
                itoa((int)regs_global.vars.num_of_vars, temp,10);
                strcpy(p_databuf + dynamic_json_file.file.len, temp);
                dynamic_json_file.file.len += strlen(temp);
                *(p_databuf + dynamic_json_file.file.len) = ',';
                dynamic_json_file.file.len++;
                break;
            case CLIENT_REGS_NUM_REQUEST:
                dynamic_json_file.file.len+=sprintf(p_databuf + dynamic_json_file.file.len, "\"client_regs_num\":");
                itoa((int)regs_global.vars.client_num_of_vars, temp,10);
                strcpy(p_databuf + dynamic_json_file.file.len, temp);
                dynamic_json_file.file.len += strlen(temp);
                *(p_databuf + dynamic_json_file.file.len) = ',';
                dynamic_json_file.file.len++;
                break;
            case REGS_SET_REQUEST:
            case SET_STRING:
                dynamic_json_file.file.len+=sprintf(p_databuf + dynamic_json_file.file.len, "\"status\":\"ok\",");
                break;
            default:
                dynamic_json_file.file.len += sprintf(p_databuf + dynamic_json_file.file.len, "\"error\":\"invalid request value\",");
                break;
            }

            *(p_databuf + dynamic_json_file.file.len - 1) = '}';  //replace last "," symbol to "}"
            *(p_databuf + dynamic_json_file.file.len) = '\0';

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

static json_msg_t json_get_msg_type(char * data){
    json_msg_t result = ERROR_JSON;
    /* clear message from " symbols */
    char * p = data + strlen(data) - 1;
    if((*data == '\"') && (*p == '\"')){   //if first and last symbols is "
        p = data;
        do {
            *p = *(p+1);
            p++;
        }
        while(*p != '\"');
        p--;
        *p = '\0';
    }

    if((*data == '[') && (*p == ']')){      //if first and last symbol is '[' and ']'
        return ARRAY_JSON;
    }

    if(isdigit((int) *data)){
        if(strchr(data, '.')){
            result = FLOAT_JSON;
        }else{
            result = NUMBER_JSON;
        }
    }else {
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
static int json_get_array_elem_nmb(char * array){
    int nmb = 1;
    char * p = array;
    while(1){
        p = strchr(p, ';');
        if(p){
            nmb++;
            p++;
        }else{
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
static int json_get_array_elements(char* json_array, u64 * buff, int num){
    int res = 0;
    char * p;
    char * q;
    p = json_array;
    p = strchr(p, '[');
    p++;
    for (int i = 0; i < num; i++) {
        if (*p=='\"'){
            p++;
        }
        buff[i] = (u64)strtol(p,&p,10);
        q = strchr(p, ';');
        if(q==NULL){
            q = strchr(p, ',');
            if(q==NULL){
                break;
            }
        }
        p=q+1;
    }
    return res;
}

/**
 * @brief json_get_elements
 * @param str_value value in string type
 * @param reg_template register type
 * @return value in the corresponding register type
 * @ingroup Json
 *
 * This function converts a value from a string to the corresponding register type
 */
static operand_t json_get_elements(char* str_value, regs_template_t * reg_template){
    operand_t value;
    value.op_u64 = 0;
    switch (reg_template->type){
    case(I8_REGS_FLAG):
    case(S16_REGS_FLAG):
    case(S32_REGS_FLAG):
    case(S64_REGS_FLAG):
    case(INT_REGS_FLAG):
        value.op_s64 = strtol(str_value,0,10);
        break;
    case(U8_REGS_FLAG):
    case(U16_REGS_FLAG):
    case(U32_REGS_FLAG):
    case(TIME_REGS_FLAG):
        value.op_u32 = strtoul(str_value,0,10);
        break;
    case(U64_REGS_FLAG):
        value.op_u64 = strtoul(str_value,0,10);
        break;
    case(FLOAT_REGS_FLAG):
        value.op_f = (float)strtod(str_value,NULL);
        break;
    case(DOUBLE_REGS_FLAG):
        value.op_d = strtod(str_value,NULL);
        break;
    }
    return value;
}

#endif //JSON_C
