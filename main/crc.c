/**
 * @file    crc.c
 * @author  Shoma Gane <shomagan@gmail.com>
 * @author  Ayrat Girfanov <girfanov.ayrat@yandex.ru>
 * @ingroup crc
 * @version 0.1
 */

#ifndef CRC_C
#define CRC_C 1

/**
  * @defgroup crc
  * @brief Calculating and checking of CRC
  */


#include "crc.h"
#include "regs.h"
#include "esp_crc.h"
#include "lwip/def.h"
/**
  * @brief control check summ16
  * @param buff - array with last two byte for summ
  * @param len - lenght packet with two summ byte
  * @return 1 - OK, \n
  *         0 - ERROR
  * @ingroup crc
  */
u8 check_summ16(vu8* buff, vu16 len){
    vu16 sum=0;
    for(u16 i=0;i<(len-2);i++) sum=sum+buff[i];
    if(sum==(buff[len-2]+((u16)(buff[len-1]<<8)))){ return 1;}
    else{ return 0;}
}

/**
  * @brief control check summ8
  * @param buff - array with last byte for summ
  * @param len - lenght packet with summ byte
  * @return 1 - OK, \n
  *         0 - ERROR
  * @ingroup crc
  */
u8 check_summ8(vu8* buff, vu16 len){
    vu8 sum=0;
    for(u16 i=0;i<(len-1);i++) sum=sum+buff[i];
    if(sum==(buff[len-1])){ return 1;}
    else{ return 0;}
}

/**
  * @brief control check crc16
  * @param buff - array with las two byte for summ
  * @param len - lenght packet with two crc byte
  * @return 1 - OK, \n
  *         0 - ERROR
  * @ingroup crc
  */
u8 check_crc16(vu8* buff, vu16 len){
    u16 crc=0;
    u16 k=0;
    while(k<(len-2)){
        u8 i=8;
        crc=crc^(u16)(((u16)buff[k++])<<8);
        do{
            if(crc & 0x8000){ crc=(u16)(crc<<1)^0x1021;}
            else{ crc=(u16)(crc<<1);}
        } while(--i);
    }
    if(crc==(buff[len-2]+((u16)(buff[len-1]<<8)))){ return 1;}
    else{ return 0;}
}

/**
  * @brief calculation summ16 for buffer
  * @param buff - array with data
  * @param len - lenght data buffer without two summ byte
  * @return summ16
  * @ingroup crc
  */
u16 calc_summ16(vu8* buff, vu16 len){
    u16 summ=0;
    for(u16 i=0;i<len;i++){ summ=summ+buff[i];}
    return summ;
}

/**
  * @brief calculation summ8 for buffer
  * @param buff - array with data
  * @param len - lenght data buffer without summ byte
  * @return summ8
  * @ingroup crc
  */
u8 calc_summ8(vu8* buff, vu16 len){
    u8 summ=0;
    for(u16 i=0;i<len;i++){ summ=summ+buff[i];}
    return summ;
}

/**
  * @brief calculation crc16 for buffer
  * @param buff - array with data
  * @param len - lenght data buffer without two crc byte
  * @return crc16
  * @ingroup crc
  */
u16 calc_crc16(vu8* buff, vu16 len){
    vu16 k=0;
    vu16 crc=0;
    while(k<len){
        u8 i=8;
        crc=crc^(u16)((u16)buff[k++]<<8);
        do{
            if(crc & 0x8000){ crc=(u16)(crc<<1)^0x1021;}
            else{ crc=(u16)(crc<<1);}
        } while (--i);
    }
    return crc;
}

/**
  * @brief calculation xor8 for buffer
  * @param buff - array with data
  * @param len - lenght data buffer without lrc byte
  * @return xor8
  * @ingroup crc
  */
u8 calc_xor8(vu8* buff, vu16 len){
    vu16 i = 0;
    vu8 xor = 0;
    while(i<len){
        xor ^= buff[i];
        i++;
    }
    return xor;
}



/**
  * @brief calculation crc32 for buffer with apparat peripherial use
  * @param buff - array with data
  * @param len - lenght u32 data value buffer without crc
  * @return crc32
  * @ingroup crc
  */
u32 calc_crc_for_u32(u32* buff, u32 len){
    return esp_crc32_le(0, (uint8_t const *)buff, len*4);
}

/**
 * @brief calculation crc32 for buffer with apparat peripherial use
 * @param space
 * @return
 * @ingroup crc
 * @todo add annotation
 */
int check_crc_for_u32(u32* space){
    int result;
    u32 in_file_len,in_file_crc,calc_crc;
    result = 0;
    u32 * temp;
    in_file_len = htonl(*space);
    if(in_file_len>4){
        calc_crc = calc_crc_for_u32((u32*)space, (in_file_len-4)/4);
        temp = (u32*)((u32)space + in_file_len - 4);
        in_file_crc = htonl(*temp);
        if (calc_crc==in_file_crc){
            result = 1;
        }
    }
    return result;
}



#endif //CRC_C
