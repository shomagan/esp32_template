/**
 * @file iirls_handler.h
 * @author Shoma Gane <shomagan@gmail.com>
 *         
 * @defgroup inc
 * @ingroup inc
 * @version 0.1 
 * @brief  TODO!!! write brief in 
 */
#ifndef IIRLS_HANDLER_H
#define IIRLS_HANDLER_H 1
 
/*add includes below */
#include "type_def.h"
#include "os_type.h"
#include "main_config.h"

/*add includes before */
#ifdef __cplusplus 
   extern "C" {
#endif

   #define IIRLSCALC_DATA_BUFFER_LENGTH (100)

typedef enum{
   SENSITIVITY_DEVIDED10=0,
   SENSITIVITY_DEVIDED100=1,
   SENSITIVITY_MAX=2,
}iirls_sensitivity_t;
typedef enum{
   IIRLS_NEVER_PAIRED = 0,/*!< Applicable for both MASTER and SLAVE. Device was never paired
                               since its power on*/
   IIRLS_PAIRED = 1,/*!< Applicable for both MASTER and SLAVE. Device is successfully paired
                         with another IIRLS device (other half either SLAVE or MASTER).*/
   IIRLS_SEARCHING = 2,/*!< Applicable only for MASTER. Scanning network and searching for a slave
                            devices*/
   IIRLS_PAIRING = 3,/*!< Applicable for both SLAVE and MASTER. For master - modbus/tcp
                          communication established and "IIRLS handshake" command sent to a
                          slave and observing adc change. For slave - received "IIRLS handshake"
                          command and updating channel duty cycle values.*/
   IIRLS_NO_DEVICES_FOUND = 4,/*!< Applicable only for MASTER. After network scan (UDP broadcast) no
                                   SLAVE devices were found for pairing*/
   IIRLS_FAILED_TO_PAIR = 5 /*!< Applicable for both SLAVE and MASTER. For master - after finding
                                 at least 1 device after network scan (UDP broadcast) and none of the
                                 SLAVE devices confirmed pairing. For slave - after receiving "IIRLS
                                 handshake" and not receiving "IIRLS handshake confirmation" command
                                 before timeout*/
}iirls_pairing_status_t;
typedef enum{
   DEVICE_HVEL1 = 0x00,
   DEVICE_HVEL2 = 0x02,
   DEVICE_IIRLS_MASTER = 0x0A,
   DEVICE_IIRLS_SLAVE = 0x0E,
}device_types_t;
/*add functions and variable declarations below */
extern task_handle_t isimfw400_task_handle;
extern uint16_t ch1_duty_cycle;
extern uint16_t ch2_duty_cycle;
extern uint16_t ch3_duty_cycle;
extern uint16_t ch4_duty_cycle;
extern uint16_t ch5_duty_cycle;
extern uint16_t ch6_duty_cycle;
extern u16 slave_adc_buffer[];

int iirls_handler_main(void);
void isimfw400_task(void *pvParameters );
/*add functions and variable declarations before */
#ifdef __cplusplus
}
#endif
#endif //IIRLS_HANDLER_H
