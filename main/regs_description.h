/**
 * @file regs_description.h
 * @author Shoma Gane <shomagan@gmail.com>
 *         Ayrat Girfanov <girfanov.ayrat@yandex.ru>
 * @defgroup free_rtos/inc
 * @ingroup free_rtos/inc
 * @version 0.1
 * @brief  TODO!!! write brief in
 */

#ifndef REGS_DESCRIPTION_H
#define REGS_DESCRIPTION_H 1

/*add includes below */
#include "type_def.h"
#include "regs.h"
/*add includes before */
#ifdef __cplusplus
   extern "C" {
#endif
/*add functions and variable declarations below */
#define  MAX_DESCRIPTION_SIZE 30
#define  MAX_ARRAY_SIZE 256
#define  MAX_DESCRIPTIONS_REGS  2048
#define NUM_OF_SELF_VARS 61
#define NUM_OF_CLIENT_VARS 0
extern const u32 def_table_version;

enum property{
    SELF      = 1<<0,
    READ_ONLY = 1<<1,
    SAVING    = 1<<2,
    USER_VARS = 1<<3,
    CREDENTIAL_FLAG   = 1<<4,
    CRITICAL_OPTION_FLAG   = 1<<5, /*!<critical need to restart*/
    HAVE_MIN_VALUE_FLAG = 1<<6,
    HAVE_MAX_VALUE_FLAG = 1<<7,
};

/** @brief struct regs_description using for storage description about vars */
typedef struct MCU_PACK{//struct for reg description generic from regs.h
  const void * p_default;//!<pointer to default value or to value for user desc vars
  const void * p_min_value;//!<pointer to min value
  const void * p_max_value;//!<pointer to max value
  u8* p_value;//!<pointer to value
  u32 saved_address;    //!<address in bkram or flash where data stored, for variable needs it
  char* description;  //!<full name pointer
  const char* name;  //!<short uniq name
  u32 guid;   //!<guid uniq for variable
  u32 modbus_description; //!<separate guid and modbus address @ref RD_MDB_ADDRESS and RD_MDB_FUNCTION
  u16 type;   //!<variable type u8 u16 u32 float double
  u16 size;    //!<array size in type value
  u16 property;      //!< @ref property (bit0 - writeable)
  u16 space_number;     //!< space number for own client, and modbus address for client
} regs_description_t;

/** @brief use it for field modbus_description in regs_description_t*/
#define RD_MDB_ADDRESS(desc) (desc&0x0000ffff)
/** @brief use it for field modbus_description in regs_description_t*/
#define RD_MDB_FUNCTION(desc) ((((u32)desc)>>16)&0x0000000f)
/** @brief use it for field space_number in regs_description_t*/
#define RD_MDB_CLIENT_SPACE_NUMBER(desc) ((((u16)desc)>>8)&0xff)
/** @brief use it for field space_number in regs_description_t*/
#define RD_MDB_CLIENT_MDB_ADDRESS(desc) (((u16)desc)&0xff)

/** @brief struct regs_template_t use it for access to variable through regs access */
typedef struct MCU_PACK {
   const u8 *p_default; //!<pointer to default value
   const void *p_min_value; //!<pointer to min value
   const void *p_max_value; //!<pointer to max value
   u8 *p_value; //!<pointer to value
   u32 saved_address; //!<address in bkram or flash where data stored, for variable needs it
   char *description; //!<full name pointer
   const char *name; //!<short uniq name
   u32 guid; //!<guid uniq for variable
   u32 modbus_description; //!<separate guid and modbus address + function
   u16 type; //!<variable type u8 u16 u32 float double
   u16 size; //!<array size in type value
   u16 property; //!<property
   u16 space_number; //!< space number
   u16 size_in_bytes; //!<array size in bytes
   u16 table_ind; //!<index of description table from regs_description_list
} regs_template_t;

/**
 * @brief struct that makes possible to keep dynamic range of regs_description_t arrays
 */
typedef struct{
   const regs_description_t * description;   /*pointer to array of regs_description_t*/
   u32 num_of_regs;                          /*number of registers*/
   const u32 * table_version;                /*version of description table to check in saved memory*/
   const char * space_name;                  /*should start with SOFI_ always, space name for user vars, and for own client*/
   u8 * saved_regs_buffer;                   /*pointer to buffer for save vars, size should be calculated by generator, 32 bit aligned*/
   u32 saved_regs_buffer_size;               /*size of buffer for save vars, should be calculated by generator, 32 bit aligned*/
} regs_description_list_t;

typedef enum{
    GUID_ADDRESS_MASK   =    0x000FFFFF,
    GUID_TYPE_MASK      =    0xfff00000,
    GUID_OS_HEAD        =    0x00000000,
    GUID_USER_HEAD      =    0x10000000,
    GUID_USER_MDB_FIELD_MASK =    0x0F000000,/*!< modbus field mask*/
    GUID_USER_IS_01_MD  =    0x01000000,/*!< its belong modbus coils areas 01*/
    GUID_USER_IS_02_MD  =    0x02000000,/*!< its belong modbus input discretes areas 02*/
    GUID_USER_IS_03_MD  =    0x04000000,/*!< its belong modbus holdings regs areas 03*/
    GUID_USER_IS_04_MD  =    0x08000000,/*!< its belong modbus input regs areas 04*/
}regs_description_guid;

extern regs_description_t const regs_description_global[];
extern const u16 def_table_id;
int regs_description_list_add_new(regs_description_list_t regs_table);
u8 * regs_description_list_get_buffer(u16 ind);
size_t regs_description_list_get_saved_buffer_size(u16 ind);
const char * regs_description_list_get_space_name(u16 table_ind);
u32 regs_description_list_get_num_of_regs(u16 table_ind);
int regs_description_get_index_by_name(regs_template_t *regs_template);
int regs_description_get_by_index(regs_template_t * regs_template, u32 index);
int regs_description_get_by_index_in_table(regs_template_t * regs_template, u32 ind);
int regs_description_get_by_address(regs_template_t * regs_template);
int regs_description_get_by_name(regs_template_t * regs_template);
int regs_description_get_by_guid(regs_template_t * regs_template);
int regs_description_add_user_vars(const regs_description_t * user_description, u16 num_of_user_vars);
u8 regs_description_is_writable (regs_template_t *regs_template);
u8 regs_description_is_credential(regs_template_t *regs_template);
u8 regs_description_flag_check (regs_template_t *regs_template, u8 flag);
void * regs_description_get_pointer_by_modbus(u16 modbus_address, u8 modbus_function);
int regs_fill_temp_buffer(regs_template_t *regs_template, regs_access_t reg, temp_data_buffering_t * temp_data_buffering, void *access_address);
int regs_check_temp_buffer(temp_data_buffering_t * temp_data_buffering, regs_template_t * regs_template);
int set_regs_def_values (u16 table_ind);
int regs_clear_temp_buffer(temp_data_buffering_t * temp_data_buffering);
u32 regs_description_get_regs_string_value(regs_template_t *regs_template, u8 reg_num, char *buffer, u32 buffer_size);
int regs_description_write_value_by_address(const void * address, const u8 * value);
int regs_description_get_index_by_address(const void * address);
/*add functions and variable declarations before */
#ifdef __cplusplus
}
#endif
#endif //REGS_DESCRIPTION_H
