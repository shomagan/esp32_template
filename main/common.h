/**
 * @file common.h
 * @author Shoma Gane <shomagan@gmail.com>
 *         Ayrat Girfanov <girfanov.ayrat@yandex.ru>
 * @defgroup ../main/
 * @ingroup ../main/
 * @version 0.1 
 * @brief  TODO!!! write brief in 
 */
#ifndef COMMON_H
#define COMMON_H 1
 
/*add includes below */
#include "type_def.h"
#include "os_type.h"
/*add includes before */
#ifdef __cplusplus 
   extern "C" {
#endif
#define TIME_FOR_PREPARE_RESET_MS 2000

/*add functions and variable declarations below */

   enum dinamic_task_event{
       STOP_CHILD_PROCCES          = (1<<0),   /*!< sended before kill external thread*/
       STOP_PROCESS_FOR_CHANGE_OS  = (1<<1),   /*!< sended before change os*/
       CAN_RX_EVENT                = (1<<2),   /*!< when can rx event */
       CAN_TX_EVENT                = (1<<3),   /*!< when can tx event */
       CAN_TIMER_EVENT             = (1<<4),   /*!< when can timer event */
       CAN_DICT_DEFAULT            = (1<<5),   /*!< return can dict to default*/
       PACKET_MAIL_SENDED          = (1<<6),   /*!< send mail through PM*/
       PACKET_RECEIVED             = (1<<7),   /*!< receved packet for PM*/
       SDO_TRANSMITHING            = (1<<8),   /*!< start transmith sdo parametr*/
       ETHERNET_RX_EVENT           = (1<<9),   /*!< we have received rx over ethernet*/
       SLIP_RX_EVENT               = (1<<10),   /*!< we have received rx over SLIP*/
       HARDWARE_DEINIT_EVENT       = (1<<11),   /*!< we have problem with hardware*/
       HAVE_DATA_TO_SEND           = (1<<12),   /*!< we have data to send*/
       SAVE_BKRAM_TO_FLASH         = (1<<13),   /*!< save bkram to flash command*/
       ARC_REMOVE_ALL              = (1<<14),   /*!< remove all arc in flash*/
       LOG_WRITE                   = (1<<15),   /*!< save log temp file to flash*/
       PACKET_TRANSMITION_CLOSE    = (1<<16),   /*!< sent last byte */
       WIFI_CONNECTED              = (1<<17),   /*!< wifi connected*/
       PREPARE_TO_RESET            = (1<<18),   /*!< controller will reset througth TIME_FOR_PREPARE_RESET_MS*/
       MODBUS_MASTER_COMMAND_COMPLETE_SUCCESSFULL = (1<<19),   /*!< command complete by modbas master thread*/
       MODBUS_MASTER_COMMAND_COMPLETE_ERROR = (1<<20),   /*!< command complete by modbas master thread*/
       MODBUS_MASTER_SET_0_SIGNAL         = (1<<21),   /*!< set first registers*/
       MODBUS_MASTER_SET_1_SIGNAL         = (1<<22),   /*!< set second part of registers*/
       MODBUS_MASTER_READ_0_SIGNAL         = (1<<23),   /*!< read set first part of registers*/
       MODBUS_MASTER_CLOSE_CONNECTION_SIGNAL         = (1<<24),   /*!< close connection*/
       MODBUS_MASTER_READ_1_SIGNAL         = (1<<25),   /*!< read set first part of registers*/
       MODBUS_MASTER_SET_2_SIGNAL         = (1<<26),   /*!< set thirв part of registers*/
       MODBUS_MASTER_PERIODIC_CONFIRMATION_SIGNAL = (1<<27),   /*!< set thirв part of registers*/
       //.............................................................................
       UNUSED_EVENT_1              = (1<<28),   /*!< we should not use this bit!!*/
       RAM_ADDRESS_IN_EVENT        = (1<<29),   /*!< instead event use ram address*/
       UNUSED_EVENT_2              = (1<<30),   /*!< we should not use this bit!!*/
       FLASH_ADDRESS_IN_EVENT      = (1<<31),   /*!< instead event use flash address*/
   };

extern task_handle_t common_duty_task_handle;
/**
 * @brief compare_float_value
 * @param a compare
 * @param b to
 * @param diff - max differ
 * @return 1 if a out of range b (b-diff:b+diff)
 */
u8 compare_float_value(float a,float b, float diff);
/**
 * @brief led_blink_on enable the led on ms
 * @param time_ms
 */
void led_blink_on(u16 time_ms);
/**
 * @brief led_blink_off disable the led if it was enableb
 */
void led_blink_off(void);
/**
 * @brief duty_task - do several common functions
 * @param pvParameters
 */
void common_duty_task(void *pvParameters );
/**
 * @brief common_deinit
 * @return
 */
int common_deinit(void);
/**
 * @brief is_ascii_symbol_or_digital
 * @param buff
 * @param len must be more then 0
 * @return 1 if consist if symbol or digital
 */
int is_ascii_symbol_or_digital(u8 * buff, u32 len);
/**
 * @brief memcmp_self
 * @param buff_0
 * @param buff_1
 * @param len
 * @return 0 -if not equal, 1 - if equal
 */
int memcmp_not_equal(const u8 * buff_0,const u8 * buff_1,u32 len);
/* 
 * @brief prepare_to_sleep - prepare to sleep, disable wifi bluetooth and setup wakeup
*/
#if ENABLE_DEEP_SLEEP
void prepare_to_sleep(void);
#endif


/*add functions and variable declarations before */
#ifdef __cplusplus
}
#endif
#endif //COMMON_H
