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
#define NUM_OF_SELF_VARS 90
#define NUM_OF_CLIENT_VARS 21
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
};//!< @property this item must be unchangable name beacose using in beremiz user generator

/**
  *@brief struct regs_description using for storage description about vars
  * */
typedef struct MCU_PACK{//struct for reg description generic from regs.h
  const void * p_default;//!<pointer to default value or to value for user desc vars
  const void * p_min_value;//!<pointer to min value
  const void * p_max_value;//!<pointer to max value
  u8* p_value;//!<pointer to value
  u32 saved_address;    //!<address in bkram or flash where data stored, for variable needs it
  char* description;  //!<full name pointer
  const char* name;  //!<short uniq name
  u16 type;   //!<variable type u8 u16 u32 float double
  u16 ind;    //!<index
  u32 guid;   //!<guid uniq for variable
  u32 modbus_description; //!<separate guid and modbus address @ref RD_MDB_ADDRESS and RD_MDB_FUNCTION
  u16 size;    //!<array size in type value 
  u16 property;      //!< @ref property (bit0 - writeable)
  u16 space_number;     //!< space number for own client, and modbus address for client
} regs_description_t;//!< @property this item must be unchangable name because using in beremiz user generator
/** @brief use it for field modbus_description in regs_description_t*/
#define RD_MDB_ADDRESS(desc) (desc&0x0000ffff)
/** @brief use it for field modbus_description in regs_description_t*/
#define RD_MDB_FUNCTION(desc) ((((u32)desc)>>16)&0x0000000f)
/** @brief use it for field space_number in regs_description_t*/
#define RD_MDB_CLIENT_SPACE_NUMBER(desc) ((((u16)desc)>>8)&0xff)
/** @brief use it for field space_number in regs_description_t*/
#define RD_MDB_CLIENT_MDB_ADDRESS(desc) (((u16)desc)&0xff)

/**
  * @brief struct regs_template_t use it for access to variable throuth regs access
  * */
typedef struct MCU_PACK{
  const u8* p_default;//!<pointer to default value
  const void * p_min_value;//!<pointer to min value
  const void * p_max_value;//!<pointer to max value
  u8* p_value;//!<pointer to value
  u32 saved_address;    //!<address in bkram or flash where data stored, for variable needs it
  char* description;  //!<full name pointer
  const char* name;  //!<short uniq name
  u16 type;   //!<variable type u8 u16 u32 float double
  u16 ind;    //!<index
  u32 guid;   //!<guid uniq for variable
  u32 modbus_description; //!<separate guid and modbus address + function
  u16 size;    //!<array size in type value
  u16 property;      //!<property
  u16 space_number;     //!< space number
  u16 size_in_bytes; //!<array size in bytes
} regs_template_t;//!< @property this item must be unchangable name because using in beremiz user generator
typedef enum{
    GUID_ADDRESS_MASK   =    0x000FFFFF,
    GUID_TYPE_MASK      =    0xfff00000,
    GUID_OS_HEAD        =    0x00000000,
    GUID_USER_HEAD      =    0x10000000,
    GUID_USER_MDB_FIELD_MASK =    0x0F000000,/*!< modbus field mask*/
    GUID_USER_IS_01_MD  =    0x01000000,/*!< its belong modbus coils areas 01*/
    GUID_USER_IS_02_MD  =    0x02000000,/*!< its belong modbus input descretes areas 02*/
    GUID_USER_IS_03_MD  =    0x04000000,/*!< its belong modbus holdings regs areas 03*/
    GUID_USER_IS_04_MD  =    0x08000000,/*!< its belong modbus input regs areas 04*/
}regs_description_guid;

extern regs_description_t const regs_description[];
extern regs_description_t const regs_description_user[];
extern const regs_description_t * regs_description_client;
extern const u16 def_table_id;
int regs_description_get_by_name(regs_template_t * regs_template);
int regs_description_get_by_ind(regs_template_t * regs_template);
int regs_description_get_by_guid(regs_template_t * regs_template);
int regs_description_add_user_vars(const regs_description_t * user_description, u16 num_of_user_vars);
u8 regs_description_is_writable (u16 reg_index);
u8 regs_description_is_credential(u16 reg_index);
/**
 * @brief regs_description_is_saved
 * @param reg_index
 * @return 1 if saving
 * @ingroup regs
  * @todo check neccessary of this function
 */
u8 regs_description_is_saved(u16 reg_index);

u8 regs_description_flag_check (u16 index, u8 flag);
/**
 * @brief regs_get_index_by_byte_address return index of regs by byte address
 * @param byte_address - byte number
 * @return
 */
int regs_description_get_index_by_address(const void * address);
/**
 * @brief regs_get_index_by_byte_address return index of regs by byte address
 * @param byte_address - byte number
 * @return
 */
int regs_description_get_index_by_guid(u32 guid);
/**
 * @brief regs_description_get_pointer_by_modbus
 * @param modbus_address [0;65635]
 * @param modbus_function {1,2,3,4}
 * @return  non NULL pointer if matched
 */
void * regs_description_get_pointer_by_modbus(u16 modbus_address, u8 modbus_function);
/**
 * @brief regs_description_flag_check
 * @param index
 * @param flag
 * @return  1 if flag exist
 */
u8 regs_description_flag_check (u16 index, u8 flag);
/**
 * @brief return ind of savig address of register in bkram
 * @param reg_end_address - pointer to register
 * @return  ind of savig bkram address, \n
 *          -1 - reg description not find
 * @ingroup regs
 */
int end_of_saved_reg_addr(void * reg_end_address);
/**
 * @brief regs_fill_temp_buffer - fillng temp buff
 * @param reg_address   - pointer to data where will write
 * @param reg   - write struct
 * @param temp_data_buffering - global temp buffer for writing data
 * @param index - index of register
 * @return
 */
int regs_fill_temp_buffer(void * reg_address,regs_access_t reg,temp_data_buffering_t * temp_data_buffering, int index);
/**
 * @brief regs_check_temp_buffer
 * @param temp_data_buffering
 * @param index
 * @return positive value if register is full filling
 */
int regs_check_temp_buffer(temp_data_buffering_t * temp_data_buffering, int index);
/**
 * @brief set_regs_def_values
 * @return 1 if self vars was changed
 */
int set_regs_def_values (void);
/**
 * @brief regs_clear_temp_buffer clining
 * @param temp_data_buffering
 * @param index
 * @return positive value if register is full filling
 */
int regs_clear_temp_buffer(temp_data_buffering_t * temp_data_buffering);
/*add functions and variable declarations before */
#ifdef __cplusplus
}
#endif
#endif //REGS_DESCRIPTION_H
