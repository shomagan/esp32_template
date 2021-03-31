#ifndef MODBUS_H
#define MODBUS_H 1
 
/*add includes below */
#include "type_def.h"
#include "memory_handle.h"
/*add includes before */
#ifdef __cplusplus 
   extern "C" {
#endif
/*add functions and variable declarations below */
enum modbus_type{
   MODBUS_RTU_PACKET = 1,
   MODBUS_TCP_PACKET
};
#define MAX_MODBUS_PACKET 256
#define MODBUS_MAX_MODBUS_REGS_NUM 125
#define MODBUS_TCP_HEADER_SIZE 6u
#define ILLEGAL_FUNCTION 0x01
#define ILLEGAL_DATA_ADDRESS 0x02
#define ILLEGAL_DATA_VALUE 0x03
#define SLAVE_DEVICE_FAILURE 0x04
#define ACKNOWLEDGE 0x05
#define SLAVE_DEVICE_BUSY   0x06
#define NEGATIVE_ACKNOWLEDGE   0x07
#define MEMORY_PARITY_ERROR   0x08
#define GATEWAY_PATH_UNAVAILABLE   0x0A
#define GATEWAY_TARGET_DEVICE_FAILED_TO_RESPOND   0x0B
#define BEREMIZ_ARRAY_ADDRESS_SPACE_START 30000u
#define BEREMIZ_ADDRESS_SPACE_START       40000u
#define SELF_MDB_ADDRESS_SPACE_START      0u
#define MODBUS_BROADCAST_ADDRESS 255
#define MDB_COILS_RW 1
#define MDB_INPUT_DISCRETES_RO 2
#define MDB_HOLDING_REGS_RW 3
#define MDB_INPUT_REGS_RO 4
extern const os_pool_def_t pool_dinamic_addr_def;

extern os_pool_cb_t * dinamic_address;
u8 htons_buff(u16 *buff, u8 word_numm);
u16 modbus_crc16(u8* pckt, u16 len);
u8 modbus_crc16_check(u8* pckt,u16 lenght);
u8 modbus_packet_for_me(u8* pckt,u16 lenght);
u16 modbus_rtu_packet (u8* pckt,u16 length);
u16 modbus_tcp_packet (u8* pckt,u16 length);
u8 genenerate_error_packet(u8* pckt,u8 error_code);
u8 modbus_err_packet_type(u8 rtu_flag,u8* pckt,u8 error_code);
u8 it_modbus_simply_check(u8* pckt,u16 lenght);
int it_modbus_request_check(u8* buff,u16 length);
int it_modbus_responde_check(u8* buff,u16 length);
int it_modbus_tcp_full_check(u8* buff,u16 len);

/**
 * @brief make packet from parametrs
 * @param slave_address simply will add to packet
 * @param function simply will add to packet
 * @param start_addr will add to packet with htons
 * @param reg_num will add to packet with htons for functions - 1,2,3,4,15,16
 * @param data_to_write pointer data from to for writers functions 15,16
 * @param packet where packet will safe
 * @return len made packet
 * */
int modbus_make_packet (u8  slave_address,u8  function, u16 start_addr,
                         u16 reg_num, u8 * data_to_write, u8 * packet);

os_pool_cb_t * modbus_bind_address_space(u32 mdb_address,u8* space,u32 * len);
/**
 * @brief add address space for modbus
 * @param mdb_address modbus space
 * @param space pointer for bring  to modbus space
 * @param space_size size space
 * @param command - modbus command corresponding of request
 * @return zero all corrected
 * */
os_pool_cb_t * modbus_bind_address_space_by_command(u32 mdb_address,u8* space,u32 * len, u8 command);

/**
 * @brief make packet from parametrs
 * @param slave_address simply will add to packet
 * @param function simply will add to packet
 * @param start_addr will add to packet with htons
 * @param reg_num will add to packet with htons for functions - 1,2,3,4,15,16
 * @param data_to_write pointer data from to for writers functions 15,16
 * @param packet where packet will safe
 * @return len made packet
 * */
int modbus_make_packet (u8  slave_address,u8  function, u16 start_addr,
                         u16 reg_num, u8 * data_to_write, u8 * packet);

/**
 * @brief check if address in dinamic space
 * @param mdb_addr - address for checking
 * @param command  - mdb command
 * @return  1 - address in, \n
 *          0 - address out
 *          negative - error
 * @ingroup modbus
 */
int modbus_dinamic_addr_check(u32 mdb_or_coil_addr,u8 command,u16 item_number);
/**
 * @brief calc new address and pointer for reading and writing address
 * @param reg_address - pionter will rewrite data to
 * @return pointer to dinamic_address_space_t
 * @ingroup modbus
 * @todo add description
 */
void * modbus_dinamic_addr_get(int pool_id);
/*add functions and variable declarations before */
#ifdef __cplusplus
}
#endif
#endif //MODBUS_H
