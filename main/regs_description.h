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
#define NUM_OF_SELF_VARS 149

enum property{
    SELF      = 1<<0,
    READ_ONLY = 1<<1,
    SAVING    = 1<<2,
    USER_VARS = 1<<3,
    CREDENTIAL_FLAG   = 1<<4,
};//!< @property this item must be unchangable name beacose using in beremiz user generator

/**
  *@brief struct regs_description using for storage description about vars
  * */
typedef struct MCU_PACK{//struct for reg description generic from regs.h
  const void * p_default;//!<pointer to default value or to value for user desc vars
  u8* p_value;//!<pointer to value
  u32 saved_address;    //!<address in bkram or flash where data stored, for variable needs it
  char* description;  //!<full name pointer
  char* name;  //!<short uniq name 
  u16 type;   //!<variable type u8 u16 u32 float double
  u16 ind;    //!<index
  u32 guid;   //!<guid uniq for variable
  u16 size;    //!<array size in type value 
  u8 property;      //!< @ref property (bit0 - writeable)
} regs_description_t;//!< @property this item must be unchangable name because using in beremiz user generator
/**
  * @brief struct regs_template_t use it for access to variable throuth regs access
  * */
typedef struct MCU_PACK{
  const u8* p_default;//!<pointer to default value
  u32 saved_address;    //!<address in bkram or flash where data stored, for variable needs it
  u8* p_value;//!<pointer to value
  char* description;  //!<full name pointer
  const char* name;  //!<short uniq name
  regs_flag_t type;   //!<variable type u8 u16 u32 float double
  u16 ind;    //!<index
  u32 guid;   //!<guid uniq for variable
  u16 size;    //!<array size in type value
  u16 size_in_bytes; //!<array size in bytes
  u8 property;      //!<property
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
int regs_description_get_by_name(regs_template_t * regs_template) MCU_ROOT_CODE;
int regs_description_get_by_ind(regs_template_t * regs_template) MCU_ROOT_CODE;
int regs_description_get_by_guid(regs_template_t * regs_template) MCU_ROOT_CODE;
int regs_description_add_user_vars(const regs_description_t * user_description, u16 num_of_user_vars)MCU_ROOT_CODE;
u8 regs_description_is_writeable (u16 reg_index);
u8 regs_description_is_credential(u16 reg_index);
int regs_description_get_index_by_byte_address(u32 byte_address);
/*add functions and variable declarations before */
#ifdef __cplusplus
}
#endif
#endif //REGS_DESCRIPTION_H
