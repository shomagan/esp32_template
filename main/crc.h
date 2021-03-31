#ifndef CRC_H
#define CRC_H 1
 
/*add includes below */
#include "type_def.h"
/*add includes before */
#ifdef __cplusplus 
   extern "C" {
#endif
/*add functions and variable declarations below */
u8 check_summ16(vu8* buff, vu16 len);
u8 check_summ8(vu8* buff, vu16 len);
u8 check_crc16(vu8* buff, vu16 len);
u16 calc_summ16(vu8* buff, vu16 len);
u8 calc_summ8(vu8* buff, vu16 len);
u8 calc_xor8(vu8* buff, vu16 len);
u16 calc_crc16(vu8* buff, vu16 len);
u32 calc_crc_for_u32(u32* buff, u32 len);
int check_crc_for_u32(u32* space);
void crc_init(void);
void crc_deinit(void);
/*add functions and variable declarations before */
#ifdef __cplusplus
}
#endif
#endif //CRC_H
