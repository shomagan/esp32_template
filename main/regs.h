/**
 * @file    regs.h
 * @author  Shoma Gane <shomagan@gmail.com>
 * @author  Ayrat Girfanov <girfanov.ayrat@yandex.ru>
 * @ingroup regs
 * @version 0.1
 */
#ifndef REGS_H
#define REGS_H 1
#include "type_def.h"
#include "lwipopts.h"
#include "os_type.h"
#ifdef __cplusplus
   extern "C" {
#endif
extern semaphore_handle_t regs_access_mutex;
extern queue_handle_t regs_event_queue;
#ifndef BIT
#define BIT(x)  (1 << x)
#endif
/**
  * @addtogroup regs
  * @{
  */
#define GLOBAL_UNION_SIZE 256
/**
  * @}
  */

/**
  * @brief struct for real time
  * @ingroup regs
  */

typedef struct MCU_PACK{
    u8 hour;            /*!< RTC Time Hour [0;23]*/
    u8 min;             /*!< RTC Time Minutes [0;59]*/
    u8 sec;             /*!< RTC Time Seconds [0;59]*/
    u8 sub_sec;         /*!< RTC Sub Second register content [1 Sec / SecondFraction +1]*/
    u8 week_day;        /*!< RTC Date WeekDay*/
    u8 month;           /*!< RTC Date Month (in BCD format)*/
    u8 date;            /*!< RTC Date[1;31]*/
    u8 year;            /*!< RTC Date Year[0,99]*/
    u16 year_day;       /*!< RTC year of day */
}structured_time_t;
/**
 * @todo add enumeration description
 */
enum {
    NEED_TO_WRITE_RTC = 0   /*!< NEED_TO_WRITE_RTC */
};
/**
  * @brief struct contain flags of runned tasks
  * @ingroup regs
  */
typedef enum{
    TASK_CREATED_INIT                = BIT(0),/*!< init task must be work on*/
    TASK_CREATED_ETHERNET_INPUT      = BIT(1),/*!< ethernet_input task must be work on*/
    TASK_CREATED_PACKET              = BIT(2),/*!< packet task must be work on*/
    TASK_CREATED_TCP_IP              = BIT(3),/*!< tcp_ip task inited */
    TASK_CREATED_ISIMFW400           = BIT(4),/*!< main control task*/
    TASK_CREATED_DUTY                = BIT(5),/*!< main manager - LED RTC*/
    TASK_CREATED_MONITOR             = BIT(6),/*!< debug monitor task*/
    TASK_CREATED_REGS_EVENT          = BIT(7),/*!< regs event task*/
    TASK_CREATED_ETHERNET_COTROL     = BIT(8),/*!< ethernet control task must be work on*/
    TASK_CREATED_DS18                = BIT(9),/*!< ds18 task must be work on*/
} task_created_t;
/**
  * @brief structures for u32 current_state[4]; //!< "current state of proccess" &ro description above contain flags
  * @ingroup regs
  */
typedef enum{
    CS0_TASK_ACTIVE_INIT                = BIT(0),/*!< init task must be work on*/
    CS0_TASK_ACTIVE_ETHERNET_INPUT      = BIT(1),/*!< ethernet_input task must be work on*/
    CS0_TASK_ACTIVE_PACKET              = BIT(2),/*!< packet task must be work on*/
    CS0_TASK_ACTIVE_TCP_IP              = BIT(3),/*!< tcp_ip task inited */
    CS0_TASK_ACTIVE_ISIMFW400           = BIT(4),/*!< main control task*/
    CS0_TASK_ACTIVE_DUTY                = BIT(5),/*!< main manager - LED RTC*/
    CS0_TASK_ACTIVE_MONITOR             = BIT(6),/*!< debug monitor task*/
    CS0_TASK_ACTIVE_REGS_EVENT          = BIT(7),/*!< regs event task*/
    CS0_TASK_ACTIVE_ETHERNET_COTROL     = BIT(8),/*!< ethernet control task must be work on*/
    CS0_TASK_ACTIVE_DS18                = BIT(9),/*!< ds18 task must be work on*/
    CS0_TASK_ACTIVE_SR04                = BIT(10),/*!< SR04 task must be work on*/
    CS0_TASK_ACTIVE_STEP_MOTOR          = BIT(11),/*!< step motor task must be work on*/
    CS0_TASK_ACTIVE_SLEEP_CONTROL       = BIT(12),/*!< sleep motor task must be work on*/
    CS0_TASK_ACTIVE_WIRELESS_CONTROL    = BIT(13),/*!< wireless conrol task must be work on*/
    CS0_TASK_ACTIVE_FEEDER              = BIT(14),/*!< feeder task must be work on*/
    CS0_TASK_ACTIVE_POLISHER            = BIT(15),/*!< polisher task must be work on*/
    CS0_TASK_ACTIVE_TEST_INT            = BIT(16),/*!< test int task must be work on*/
    CS0_TASK_ACTIVE_MORSE               = BIT(17),/*!< morse task must be work on*/
    CS0_TASK_ACTIVE_BATTERY_STATE       = BIT(18),/*!< battery state task must be work on*/
    CS0_TASK_ACTIVE_HTTP_SOCK         = BIT(19),/*!< http socket task must be work on*/
    CS0_TASK_ACTIVE_SCD41               = BIT(20),/*!< SCD41 task must be work on*/
    CS0_TASK_ACTIVE_TELEGRAM            = BIT(21),/*!< telegram task must be work on*/
} current_state_0;
/**
  * @brief structures for u32 current_state[4]; //!< "current state of proccess" &ro description above contain flags
  * @ingroup regs
  */
typedef enum{
  CS1_SR04_FALINGEDGE_BEFORE_RISINGEDGE = BIT(0),/*!< SR04 consequnce problem*/
  CS1_SR04_FULL_SUBSEQUNCE_PROBLEM = BIT(1),/*!< SR04 consequnce problem*/
  CS1_SR04_RISINGEDGE_AFTER_FALINGEDGE = BIT(2),/*!< SR04 consequnce problem*/
} current_state_1;


/**
  * @brief struct contain flags of initialized  modules
  * @ingroup regs
  */
typedef enum {
       ASYNC_INIT_IP             = BIT(0), /*!< ip was init*/
       ASYNC_INIT_USB            = BIT(1), /*!< usb was init*/
       ASYNC_INIT_MODBUS_UDP              = BIT(2), /*!< modbus udp server was init*/
       ASYNC_INIT_MODBUS_TCP              = BIT(3), /*!< modbus tcp server was init*/
       ASYNC_INIT_HTTP                    = BIT(4), /*!< http server was init*/
       ASYNC_INIT_GET_VALUE_FROM_BKRAM    = BIT(5), /*!< use settings from bkram*/
       ASYNC_INIT_SET_VALUE_FROM_BKRAM_TO_FLASH    = BIT(6), /*!< use settings from bkram*/
       ASYNC_INIT_ERROR_OCCURED_WHILE_INIT    = BIT(7), /*!< error was wjile init*/
       ASYNC_INIT_UDP_BROADCAST              = BIT(8), /*!< udp broadcast */
} async_init_t;
/**
  * @brief struct contain flags of initialized  modules
  * @ingroup regs
  */
typedef enum{
   async_succes_ip                      = BIT(0), /*!< ip was init*/
   async_succes_usb                     = BIT(1), /*!< usb was init*/
   async_succes_modbus_udp              = BIT(2), /*!< modbus udp server was init*/
   async_succes_modbus_tcp              = BIT(3), /*!< modbus tcp server was init*/
   async_succes_http                    = BIT(4), /*!< http server was init*/
} async_init_success_t;
typedef enum{
   ENABLE_HTTP_FLAG             = BIT(0), /*!< http web pages was enabled*/
   ENABLE_CREDENTIAL_FLAG             = BIT(1), /*!< special http web pages was enabled*/
} permissions_t;

#define RISING 1    /** @todo check implementation */
#define FALLING 0   /** @todo check implementation */
/**
 * @todo check implementation of enum
 */
enum{
    REGS_SET_FLAG=0,
    REGS_GET_FLAG
};


/**
  * @brief global register types
  * @ingroup regs
  */
typedef union {
    float op_f;
    double op_d;
    int op_int;
    u8 op_u8;
    u16 op_u16;
    u32 op_u32;
    u64 op_u64;
    i8 op_i8;
    s16 op_i16;
    s32 op_s32;
    s64 op_s64;
} operand_t;

/**
  * @brief global register type flag
  * @ingroup regs
  */
typedef enum {
    U8_REGS_FLAG = (1<<0),/*!<unsigned char type(1 byte) */
    U16_REGS_FLAG = (1<<1),/*!<unsigned short type(2 byte) */
    U32_REGS_FLAG = (1<<2),/*!<unsigned int type(4 byte) */
    U64_REGS_FLAG = (1<<3),/*!<unsigned long long type(8 byte) */
    I8_REGS_FLAG = (1<<4),/*!<signed char type(1 byte) */
    S16_REGS_FLAG = (1<<5),/*!<signed short type(2 byte) */
    S32_REGS_FLAG = (1<<6),/*!<signed int type(4 byte) */
    S64_REGS_FLAG = (1<<7),/*!<signed long long type(8 byte) */
    FLOAT_REGS_FLAG = (1<<8),/*!<float type(4 byte) */
    DOUBLE_REGS_FLAG = (1<<9),/*!<double type(8 byte) */
    INT_REGS_FLAG = (1<<10),/*!<signed int type(4 byte) */
    TIME_REGS_FLAG = (1<<11)/*!<unsigned int type(4 byte) */
}regs_flag_t;

/**
  * @brief struct for regs access
  * @ingroup regs
  */
typedef struct {
    regs_flag_t flag;
    operand_t value;
} regs_access_t;
/**
  * @addtogroup
  * @todo determine which group to include
  * @{
  */
#define FW_VERSION_SIZE 4
#define FW_VERSION {0,10,0,0}
#define FW_VERSION_STR "0.10.0-beta.0"
#define FW_FIRMWARE_HASH "71-f0350c5d9e94bf11b37c89215dfb7c"
#define FW_HASH 0x00000000
#define REGS_MAX_NAME_SIZE 32
#define DEVICE_NAME_SIZE 40
#define BKRAM_BYTES_NUM 66
#define WRITE_REG_MASK 0xFFFF
#define WRITE_REG_CH_MASK 0xFFFF0000
#define WIFI_NAME_LEN 12
#define WIFI_PASSWORD_LEN 8
#define WIFI_ROUTER_NAME_LEN 32
#define WIFI_ROUTER_PASSWORD_LEN 32

#define BOARD_VERSION 0
#define USE_REGS_GLOBAL 1

/**
  * @}
  */
/** \union main_vars_t  
 * @brief main struct
 * name variables uses for generate name in description file and then in get value by name
 * and therefore use max size len name is 16 charackter \n
 * coment style :   "" - description, \n
 *                  &ro  - read only, \n
 *                  &def -> have const varibale with struct like def_name, \n
 *                  &save- will have saved in bkram, \n
 *                  &crtcl- restart after change value, \n
 *
 * @ingroup regs
 */
/** #generator_use_description {"space_name" :"main_vars_t", "modbus_type" :"server", "address_space" :0, "modbus_function" :"holding_registers", "register_start_address" :0}*/
typedef union{
    struct MCU_PACK{
        // start regs struct
        u16 mdb_addr;                   //!<"modbus address" &save &def
        u8 ip[4];                       //!<"device ip address, warning!!! changes can lead to lost connection" &save &def &crtcl
        u8 netmask[4];                  //!<"netmask address for main wifi net", &save , &def , &crtcl
        u8 gate[4];                 //!<"gateaway address, warning!!! changes can lead to lost connection " &save &def &crtcl
        u8 slip_ip[4];                       //!<"ip address for local net",&save , &def &crtcl
        u8 slip_netmask[4];                  //!<"netmask address for local net", &save , &def , &crtcl
        u8 slip_gate[4];                 //!<"gateaway address for local net", &save, &def &crtcl
        u8 wifi_name[WIFI_NAME_LEN];               //!<"must be strong full filled", &save &def &crtcl
        u8 wifi_password[WIFI_PASSWORD_LEN];            //!<"must be strong 8 byte", &save &def &crtcl
        u8 wifi_router_name[WIFI_ROUTER_NAME_LEN];               //!<"must be ended by zero", &save &def &crtcl
        u8 wifi_router_password[WIFI_ROUTER_PASSWORD_LEN];            //!<"must be more or equal 8 byte", &save &def &crtcl
        u16 wifi_setting;               //!<"type of wifi and settings" &save &def &crtcl
        u32 reset_num;                  //!<"number of reset" &save
        u16 wifi_state;                 //!<"wifi state" &ro
        u16 command;                    //!<"command register"
        u16 uart_debug_sets;            //!<"settings debug uart speed,parity and stop bits, default 115200 ,parity - none, 1 stop bit" &save &def
        u16 device_state;               //!< "BIT(0) - interlock" &ro
        u16 unused_3;                  //!<"" &ro
        u64 sys_tick_counter;           //!<"tick in ms",&ro
        u8  mac_addr[6];                //!<"ethernet mac address",&ro
        u32 flags_succ_init;   //!<"success inited modules" &ro for init in task
        u32 cur_free_heap;      //!<"in bytes",&ro
        u32 min_free_heap;      //!<"in bytes",&ro
        u8 debug_info[8];       //!<"reserved use for debug"&ro
        u16 num_of_vars;        //!<"number of vars self + config(user) &ro &def
        u16 client_num_of_vars;        //!<"number of client vars self" &ro &def        
        float temperature_mcu;  //!<"temperature mcu Celsius" &ro
        u8 fw_version[FW_VERSION_SIZE];      //!<"version like 0.1.1.0",&ro,&def
        u16 board_ver;              //!< "board version", &ro, &def
        u32 firmware_hash;                //!< "hash os" &ro
        u32 system_error;           //!< "system global error" &ro
        u16 permission;                 //!<"flags with permissions" &ro &def
        u32 async_flags;            //!< "async flags" &ro
        u64 slip_packet_counter;    //!< "count all slip packet"
        u16 ap_connections_number; //!< "number of connections" &ro
        u16 sta_connect; //!< "sta connect state" &ro
        u32 table_version;     //!< "change value in def_table_version for drop all regs to default value" &ro &def &save
        u16 iirls_pairing_status; //!< "status" &ro
        u8 iirls_paired_ip[4];              //!< "ip address of paired device" &ro
        u16 modbus_master_test_value; //!< "status" &ro
        u16 iirls_state;                //!< "state" &save
        u16 paired_device_model_number[40];//!< "state" &ro
        u16 slave_modbus_address;       //!< "slave modbus address" &def &save &max &min
        u32 modbus_master_comm_period_current; //!< ""
        u32 modbus_master_comm_period_max; //!< ""
        u32 modbus_master_comm_period_min; //!< ""
        u32 modbus_master_succ_transactions_number; //!< ""
        u32 modbus_master_error_transactions_number; //!< ""
        u16 i2c_display_address; //!< "address of display" &ro
        u8 sta_ip[4]; //!< "ip address of sta" &ro &save
        u32 live_time; //!< "live time in seconds" &ro
        s64 unix_time; //!< "unix time , not implemented" 
        u32 seconds_of_the_day; //!< "seconds of the day" 
        u32 flash_write_number; //!< "increments every flash write by an app" &ro &save
        u32 current_state[4]; //!< "current state of proccess" &ro
        u16 sleep_time;//!< "seconds,couple with SLEEP_TASK_DEEP_SLEEP_FOR_N_SEC"  &def &min
        s8  rssi_ap[2];//!< "wifi rssi of Access point" &ro
        s8  primary_channel_ap[2];//!< "wifi channel used" &ro
        s32 wake_up_cause;//!< "wake up reason" &ro
    }vars;
    u8 bytes[GLOBAL_UNION_SIZE]; //for full bksram copy
}main_vars_t;// #generator_use_description {"message":"end_struct"}
extern main_vars_t * const regs_global;
/**
 * @brief main struct
 * name variables uses for generate name in description file and then in get value by name
 * and therefore use max size len name is 16 charackter \n
 * coment style :   "" - description, \n
 *                  &ro  - read only, \n
 *                  &def -> have const varibale with struct like def_name, \n
 *                  &save- will have saved in bkram, \n
 *                  &crtcl- restart after change value, \n
 *
 * @ingroup regs
 */
/** #generator_use_description {"space_name" :"main_vars_part_1_t", "modbus_type" :"server", "address_space" :1, "modbus_function" :"holding_registers", "register_start_address" :1000}*/
typedef union{
    struct MCU_PACK{
        // start regs struct
        float test_pwm_value;                   //!<"test pwm value [0;100]" &def &save &min &max
        u16 touch_0;
        u16 touch_1;
        u16 touch_2;
        u16 touch_3;
        u64 water_counter;                      //!< "di counter" &save &ro
        float impulse_cost;                     //!< "cost of di impulse in liters" &save &def
        float liters;                           //!< "liters calculated" &save
        u32 touch_1_count;                      //!< "ms " &save &def
        u32 touch_2_count;                      //!< "ms " &save &def
        u32 touch_3_count;                      //!< "ms " &save &def
        float touch_1_liters;                      //!< "ms " &save &def
        float touch_2_liters;                      //!< "ms " &save &def
        float touch_3_liters;                      //!< "ms " &save &def
        float touch_0_trshld;                      //!< "in percents" &save &def
        float touch_1_trshld;                      //!< "in percents" &save &def
        float touch_2_trshld;                      //!< "in percents" &save &def
        float touch_3_trshld;                      //!< "in percents" &save &def
        u32 filter_use;                             //!< "use inside filter fot touch sensors or not " &save
        u32 touch_handle_period;                    //!< "in ms period of handle touchs" &save &def
        u32 by_time;                              //!< "by time or counter" &save &def
    }vars;
    u8 bytes[32]; //for full bksram copy
}main_vars_part_1_t;// #generator_use_description {"message":"end_struct"}
extern main_vars_part_1_t * const regs_global_part1;
/**
 * @brief main struct
 * name variables uses for generate name in description file and then in get value by name
 * and therefore use max size len name is 16 charackter \n
 * coment style :   "" - description, \n
 *                  &ro  - read only, \n
 *                  &def -> have const varibale with struct like def_name, \n
 *                  &save- will have saved in bkram, \n
 *                  &crtcl- restart after change value, \n
 *
 * @ingroup regs
 */
/** #generator_use_description {"space_name" :"servo_control_part_t",  "address_space" :2, "modbus_type" :"server", "modbus_function" :"holding_registers", "register_start_address" :2000}*/
typedef union{
    struct MCU_PACK{
        // start regs struct
        float servo_0;                   //!<"servo pwm value [0;100]" &def &min &max
        float servo_1;                   //!<"servo pwm value [0;100]" &def &min &max
        float servo_2;                   //!<"servo pwm value [0;100]" &def &min &max
        float servo_3;                   //!<"servo pwm value [0;100]" &def &min &max
    }vars;
    u8 bytes[32]; //for full bksram copy
}servo_control_part_t;// #generator_use_description {"message":"end_struct"}
extern servo_control_part_t * const servo_control_part;
/**
 * @brief main struct
 * name variables uses for generate name in description file and then in get value by name
 * and therefore use max size len name is 16 charackter \n
 * coment style :   "" - description, \n
 *                  &ro  - read only, \n
 *                  &def -> have const varibale with struct like def_name, \n
 *                  &save- will have saved in bkram, \n
 *                  &crtcl- restart after change value, \n
 *
 * @ingroup regs
 */
/** #generator_use_description {"space_name" :"di_control_t",  "address_space" :3, "modbus_type" :"server", "modbus_function" :"holding_registers", "register_start_address" :3000}*/
typedef union{
    struct MCU_PACK{
        // start regs struct
        u32 pin_state;                 //!<"current states of digital inputs"
    }vars;
    u8 bytes[4]; //for full bksram copy
}di_control_t;// #generator_use_description {"message":"end_struct"}
extern di_control_t * const di_control;
typedef enum{
  SYNC_STATE_ACTIVE = BIT(0),
  SYNC_STATE_SYNCRONIZED = BIT(1),
}sync_state_t;
/**
 * @brief time sync struct for read time from another device
 * name variables uses for generate name in description file and then in get value by name
 * and therefore use max size len name is 16 charackter \n
 * coment style :   "" - description, \n
 *                  &ro  - read only, \n
 *                  &def -> have const varibale with struct like def_name, \n
 *                  &save- will have saved in bkram, \n
 *                  &crtcl- restart after change value, \n
 *
 * @ingroup regs
 */
/** #generator_use_description {"space_name" :"sync_time_t",  "address_space" :4, "modbus_type" :"server", "modbus_function" :"holding_registers", "register_start_address" :4000}*/
typedef union{
    struct MCU_PACK{
        // start regs struct
        s32 sync_sys_tick_dev;         //!< "deviation between master and slave" &ro
        u64 sync_sys_tick_slave;        //!< "time read from slave" &ro
        u64 sync_sys_tick_master;       //!< "time read from master" &ro 
        u16 sync_average_time_ms;    //!< "average send receive time " &ro
        u16 sync_last_req_time_ms;       //!< "last send receive time " &ro
        u16 sync_active;                      //!< "activated measurement" &ro
    }vars;
    u8 bytes[32]; //for full bksram copy
}sync_time_regs_t;// #generator_use_description {"message":"end_struct"}
extern sync_time_regs_t * const sync_time_regs;
typedef enum{
    SR04_STATE_ACTIVE = BIT(0),
    SR04_STATE_ECHO = BIT(1),
} sr04_state_t;
/**
 * @brief sr04 measurements struct for distance control and time laps
 * name variables uses for generate name in description file and then in get value by name
 * and therefore use max size len name is 16 charackter \n
 * coment style :   "" - description, \n
 *                  &ro  - read only, \n
 *                  &def -> have const varibale with struct like def_name, \n
 *                  &save- will have saved in bkram, \n
 *                  &crtcl- restart after change value, \n
 *
 * @ingroup regs
 */
/** #generator_use_description {"space_name" :"sr04_reg_t",  "address_space" :5, "modbus_type" :"server", "modbus_function" :"holding_registers", "register_start_address" :4100}*/
typedef union{
    struct MCU_PACK{
        // start regs struct
        u16 lap_state;         //!< "state sr04, bit0 - activated, bit1 - echo signal received" &ro
        float lap_distance;        //!< "current distance" &ro
        u64 lap;       //!< "when we have sharp change of a distance, save it " &ro 
        u64 lap_paired_dev;    //!< "lap from paired device" &ro
        float distance_filtered;      //!< "current distance filterd" &ro
    }vars;
    u8 bytes[32]; //for full bksram copy
}sr04_reg_t;// #generator_use_description {"message":"end_struct"}
extern sr04_reg_t * const sr04_reg;

/**
 * @brief feeders struct for feed control
 * name variables uses for generate name in description file and then in get value by name
 * and therefore use max size len name is 16 charackter \n
 * coment style :   "" - description, \n
 *                  &ro  - read only, \n
 *                  &def -> have const varibale with struct like def_name, \n
 *                  &save- will have saved in bkram, \n
 *                  
 * @ingroup regs
 */
/** #generator_use_description {"space_name" :"feeder_reg_t",  "address_space" :6, "modbus_type" :"server", "modbus_function" :"holding_registers", "register_start_address" :4200}*/
typedef union{
    struct MCU_PACK{
        // start regs struct
        u32 feeder_counter;         //!<"how many time step motor started" &save &ro
        u16 feeder_interval;        //!< "minutes between a feeds" &save &def &min
        u16 feeder_reserv0;         //!< "reserved"
        float feeder_time_sec;       //!< "turn time is seconds" &save &def &min &max
    }vars;
    u32 bytes[4]; //for full bksram copy
}feeder_reg_t;// #generator_use_description {"message":"end_struct"}
extern feeder_reg_t * const feeder_reg;

/**
 * @brief polisher struct for speed control
 * name variables uses for generate name in description file and then in get value by name
 * and therefore use max size len name is 16 charackter \n
 * coment style :   "" - description, \n
 *                  &ro  - read only, \n
 *                  &def -> have const varibale with struct like def_name, \n
 *                  &save- will have saved in bkram, \n
 *                  
 * @ingroup regs
 */
/** #generator_use_description {"space_name" :"polisher_reg_t",  "address_space" :7, "modbus_type" :"server", "modbus_function" :"holding_registers", "register_start_address" :4300}*/
typedef union{
    struct MCU_PACK{
        // start regs struct
        u32 polisher_sec;         //!<"how many time in seconds it was active" &save &ro
        u32 polisher_last_sec;         //!<"how many time in seconds it was active last session" &ro
        u16 polisher_speed;        //!< "polisher_speed" &save &def &max
        u16 polisher_direction;         //!< "polisher_direction" &save &def &max 
    }vars;
    u32 bytes[4]; //for full bksram copy
}polisher_reg_t;// #generator_use_description {"message":"end_struct"}
extern polisher_reg_t * const polisher_reg;
/**   
 * @brief test struct for internal tests 
 * name variables uses for generate name in description file and then in get value by name
 * and therefore use max size len name is 16 charackter \n
 * coment style :   "" - description, \n
 *                  &ro  - read only, \n
 *                  &def -> have const varibale with struct like def_name, \n
 *                  &save- will have saved in bkram, \n
 *  @ingroup regs
 */
/** #generator_use_description {"space_name" :"test_int_reg_t",  "address_space" :8, "modbus_type" :"server", "modbus_function" :"holding_registers", "register_start_address" :4400}*/
typedef union{
    struct MCU_PACK{
        // start regs struct
        u32 test_int_state;         //!<"current state" &ro
        u32 test_int_command;         //!<"command" &ro
        u32 test_int_component;         //!<"component to be tested" 
        u32 test_int_type;         //!<"test type - check, stress, performance" 
        u32 test_int_result;         //!<"overall result" &ro
    }vars;
    u32 bytes[4]; //for full bksram copy
}test_int_reg_t;// #generator_use_description {"message":"end_struct"}
extern test_int_reg_t * const test_int_reg;
/**   
 * @brief struct for morse component 
 * name variables uses for generate name in description file and then in get value by name
 * and therefore use max size len name is 16 charackter \n
 * coment style :   "" - description, \n
 *                  &ro  - read only, \n
 *                  &def -> have const varibale with struct like def_name, \n
 *                  &save- will have saved in bkram, \n
 *  @ingroup regs
 */
/** #generator_use_description {"space_name" :"morse_reg_t",  "address_space" :9, "modbus_type" :"server", "modbus_function" :"holding_registers", "register_start_address" :4500}*/
typedef union{
    struct MCU_PACK{
        // start regs struct
        u16 morse_unit_time_ms;         //!<"morse time ms" &save &def &min &max
        u16 morse_message_len;         //!<"max morse message len" &save &def &min &max
        u16 morse_settings;         //!<"morse settings bit0 - server" &save &def
        u16 morse_message_position;         //!<"morse message position" &ro
        u8 morse_message[32];         //!<"morse message to broadcast" &ro 
        u8 morse_send[32];         //!<"morse message to broadcast" &ro
        u8 morse_line_1[32];         //!<"morse messages received " &ro 
        u8 morse_line_2[32];         //!<"morse messages received " &ro 
        u8 morse_line_3[32];         //!<"morse messages received " &ro 
        u8 morse_line_4[32];         //!<"morse messages received " &ro 
        u8 morse_line_5[32];         //!<"morse messages received " &ro 
        u32 morse_counter;         //!<"morse messages sent and received counter" &ro
    }vars;
    u32 bytes[256]; //for full bksram copy
}morse_reg_t;// #generator_use_description {"message":"end_struct"}
extern morse_reg_t * const morse_reg;
/**   
 * @brief struct for battery_state component 
 * name variables uses for generate name in description file and then in get value by name
 * and therefore use max size len name is 16 charackter \n
 * coment style :   "" - description, \n
 *                  &ro  - read only, \n
 *                  &def -> have const varibale with struct like def_name, \n
 *                  &save- will have saved in bkram, \n
 *  @ingroup regs
 */
/** #generator_use_description {"space_name" :"battery_state_reg_t",  "address_space" :10, "modbus_type" :"server", "modbus_function" :"holding_registers", "register_start_address" :4700}*/
typedef union{
    struct MCU_PACK{
        // start regs struct
        u16 battery_level;         //!<"0 - 100" &ro
        u16 battery_min_level;     //!<"min level to start" &save &def &min &max
        float battery_voltage;     //!<"for exmpl 4.2v" &ro
        u16 battery_settings;     //!<"bit 0 - activate level control" &save
    }vars;
    u32 bytes[32]; //for full bksram copy
}battery_state_reg_t;// #generator_use_description {"message":"end_struct"}
extern battery_state_reg_t * const battery_state_reg;

/**
 * @brief struct for SCD41 sensor component
 * name variables use for generating names in the description file and then in get value by name.
 * Therefore, use max size len name is 16 characters.
 * Comment style:
 *   "" - description,
 *   &ro  - read only,
 *   &def -> have const variable with struct like def_name,
 *   &save- will have saved in bkram,
 * @ingroup regs
 */
/** #generator_use_description {"space_name" :"scd41_reg_t",  "address_space" :11, "modbus_type" :"server", "modbus_function" :"holding_registers", "register_start_address" :4800}*/
typedef union {
    struct MCU_PACK {
        // start regs struct
        u16 scd41_co2_level;         //!< "CO2 level in ppm" &ro
        float scd41_temperature;     //!< "Temperature in Celsius" &ro
        float scd41_humidity;        //!< "Humidity in percentage" &ro
        u16 scd41_settings;    //!< "Settings for SCD41 sensor" &save
        u16 scd41_measur_interval;   //!< "minutes between a measurement" &save &def &min &max
    } vars;
    u32 bytes[32]; // for full bksram copy
} scd41_reg_t; // #generator_use_description {"message":"end_struct"}
extern scd41_reg_t * const scd41_reg;

/**
 * @brief struct for reading modbus data from another device maximume bytes - 240
 * name variables uses for generate name in description file and then in get value by name
 * and therefore use max size len name is 16 charackter \n
 * will be in struct regs_description_client
 * additional space modbus_address, specific space read_holding_registers
 * coment style :   "" - description, \n
 *                  &ro  - read only, \n
 *                  &def -> have const varibale with struct like def_name, \n
 *                  &save- will have saved in bkram, \n
 *                  regs_description \n
 *
 * @ingroup regs
 */
/** #generator_use_description {"space_name" :"client_part_0",  "address_space" :0, "modbus_type" :"client", "modbus_function" :"holding_registers", "modbus_address" :3,"register_start_address" :0}*/
typedef union{
    struct MCU_PACK{
        // start regs struct
        u16 cli_mdb_addr;                   //!<"modbus address" 
        u8 cli_ip[4];                       //!<"device ip address, warning!!! " 
        u8 cli_netmask[4];                  //!<"netmask address for main wifi net",
        u8 cli_gate[4];                     //!<"gateaway address, warning!!! " 
        u8 cli_slip_ip[4];                  //!<"ip address for local net",
        u8 cli_slip_netmask[4];             //!<"netmask address for local net", 
        u8 cli_slip_gate[4];                //!<"gateaway address for local net", 
    }vars;
    u8 bytes[32]; //for full bksram copy
}client_part_0_t;// #generator_use_description {"message":"end_struct"}
extern client_part_0_t * const client_part_0;
/**
 * @brief client part2 to read sys_tick_counter from salve
 * struct for reading modbus data from another device maximume bytes - 240
 * name variables uses for generate name in description file and then in get value by name
 * and therefore use max size len name is 16 charackter \n
 * will be in struct regs_description_client
 * additional space modbus_address, specific space read_holding_registers
 * coment style :   "" - description, \n
 *                  &ro  - read only, \n
 *                  &def -> have const varibale with struct like def_name, \n
 *                  &save- will have saved in bkram, \n
 *                  regs_description \n
 *
 * @ingroup regs
 */
/** #generator_use_description {"space_name" :"sync_time_client_t",  "address_space" :2, "modbus_type" :"client", "modbus_function" :"holding_registers", "modbus_address" :3,"register_start_address" :63}*/
typedef union{
    struct MCU_PACK{
        // start regs struct
        u64 sys_tick_slave;        //!< "time read from slave" &ro
    }vars;
    u8 bytes[8]; //for full bksram copy
}sync_time_client_t;// #generator_use_description {"message":"end_struct"}
extern sync_time_client_t * const sync_time_client;
/**
 * @brief struct for reading modbus data from another device maximume bytes - 240
 * name variables uses for generate name in description file and then in get value by name
 * and therefore use max size len name is 16 charackter \n
 * will be in struct regs_description_client
 * additional space modbus_address, specific space read_holding_registers
 * coment style :   "" - description, \n
 *                  &ro  - read only, \n
 *                  &def -> have const varibale with struct like def_name, \n
 *                  &save- will have saved in bkram, \n
 *                  regs_description \n
 *
 * @ingroup regs
 */
/** #generator_use_description {"space_name" :"client_part_1",  "address_space" :1, "modbus_type" :"client", "modbus_function" :"holding_registers", "modbus_address" :3,"register_start_address" :80}*/
typedef union{
    struct MCU_PACK{
        // start regs struct
        u16 cli_num_of_vars;        //!<"number of vars self + config(user) &ro 
        u16 cli_client_num_of_vars;        //!<"number of client vars self" &ro 
    }vars;
    u8 bytes[8]; //for full bksram copy
}client_part_1_t;// #generator_use_description {"message":"end_struct"}
extern client_part_1_t * const client_part_1;

/**
 * @brief time sync struct for read time from another device
 * name variables uses for generate name in description file and then in get value by name
 * and therefore use max size len name is 16 charackter \n
 * coment style :   "" - description, \n
 *                  &ro  - read only, \n
 *                  &def -> have const varibale with struct like def_name, \n
 *                  &save- will have saved in bkram, \n
 *                  &crtcl- restart after change value, \n
 *
 * @ingroup regs
 */
/** inside struct modbus_type shows us that this struct is for client(read data from client) or not 
 * #generator_use_description {"space_name" :"sync_data_client_t",  "address_space" :4, "modbus_type" :"client", "modbus_function" :"holding_registers", "modbus_address" :3, "register_start_address" :4000}*/
typedef union{
    struct MCU_PACK{
        // start regs struct
        s32 cli_sys_tick_dev;         //!< "deviation between master and slave" &ro
        u64 cli_sys_tick_slave;        //!< "time read from slave" &ro
        u64 cli_sys_tick_master;       //!< "time read from master" &ro 
        u16 cli_average_time_ms;    //!< "average send receive time " &ro
        u16 cli_last_req_time_ms;       //!< "last send receive time " &ro
        u16 cli_sync_state;                      //!< "activated measurement" &ro
    }vars;
    u8 bytes[32]; //for full bksram copy
}sync_data_client_t;// #generator_use_description {"message":"end_struct"}
extern sync_data_client_t * const sync_time_regs_from_client;
/**
 * @brief sr04 measurements struct for distance control and time laps
 * name variables uses for generate name in description file and then in get value by name
 * and therefore use max size len name is 16 charackter \n
 * coment style :   "" - description, \n
 *                  &ro  - read only, \n
 *                  &def -> have const varibale with struct like def_name, \n
 *                  &save- will have saved in bkram, \n
 *                  &crtcl- restart after change value, \n
 *
 * @ingroup regs
 */
/** #generator_use_description {"space_name" :"sr04_reg_client_t",  "address_space" :3, "modbus_type" :"client", "modbus_function" :"holding_registers", "modbus_address" :3, "register_start_address" :4100}*/
typedef union{
    struct MCU_PACK{
        // start regs struct
        u16 cli_state;         //!< "state sr04, bit0 - activated, bit1 - echo signal received" &ro
        float cli_distance;        //!< "current distance" &ro
        u64 cli_lap;       //!< "when we have sharp change of a distance, save it " &ro 
        u64 cli_lap_paired_dev;    //!< "lap from paired device" &ro
        float cli_distance_filtered;      //!< "current distance filterd" &ro
    }vars;
    u8 bytes[32]; //for full bksram copy
}sr04_reg_client_t;// #generator_use_description {"message":"end_struct"}
extern sr04_reg_client_t * const sr04_reg_client;

/**
 * @brief main struct to unite all generated structs and keep it in one place and sorted by address space
 */
/** #generator_use_description {"regs_main":"start_struct"} */
typedef struct{
    main_vars_t regs_global; //!< "main_vars_t"
    main_vars_part_1_t regs_global_part1; //!< "main_vars_part_1_t"
    servo_control_part_t servo_control_part; //!< "servo_control_part_t"
    di_control_t di_control; //!< "di_control_t"
    sync_time_regs_t sync_time_regs; //!< "sync_time_regs_t"
    sr04_reg_t sr04_reg; //!< "sr04_reg_t"
    feeder_reg_t feeder_reg; //!< "feeder_reg_t"
    polisher_reg_t polisher_reg; //!< "polisher_reg_t"
    test_int_reg_t test_int_reg; //!< "test_int_reg_t"
    morse_reg_t morse_reg; //!< "morse_reg_t"
    battery_state_reg_t battery_state_reg; //!< "battery_state_reg_t"
    scd41_reg_t scd41_reg; //!< "scd41_reg_t"
    client_part_0_t client_part_0; //!< "client_part_0_t"
    sync_time_client_t sync_time_client; //!< "sync_time_client_t"
    client_part_1_t client_part_1; //!< "client_part_1_t"
    sync_data_client_t sync_time_regs_from_client; //!< "sync_data_client_t"
    sr04_reg_client_t sr04_reg_client; //!< "sr04_reg_client_t"
}regs_main_t;// #generator_use_description {"message":"end_struct"}
extern regs_main_t regs_main;
/**
  * @brief registers for regs_event_handler
  * @ingroup regs
  * @todo check implementation
  */
typedef enum {
    none_reg = 0,
    change_ip_reg,
    eth_speed_reg,
    eth_duplex_reg,
    rtc_reg,
    rtc_unix_reg,
    main_test_blocks_reg,
    command_reg,
} write_regs_t;

/**
  * @brief register elements
  * @ingroup regs
  * @todo check implementation
  */
typedef enum {
    ch_none_reg =   0,
    ch_0_reg =      (1<<16),
    ch_1_reg =      (2<<16),
    ch_2_reg =      (3<<16),
    ch_3_reg =      (4<<16),
    ch_4_reg =      (5<<16),
    ch_5_reg =      (6<<16),
    ch_6_reg =      (7<<16),
    ch_7_reg =      (8<<16),
    ch_8_reg =      (9<<16),
    ch_9_reg =      (10<<16),
    ch_10_reg =     (11<<16),
    ch_11_reg =     (12<<16),
    ch_12_reg =     (13<<16),
    ch_13_reg =     (14<<16),
    ch_14_reg =     (15<<16),
    ch_15_reg =     (16<<16),
} write_regs_ch_t;

/**
  * @brief main command list
  * @ingroup regs
  */
typedef enum {
    NONE_COMM = 0,
    SAVE_CREDENTIAL_COMM = 0x51ff,          /*!< 20991 command to save credential to flash*/
    ENABLE_HTTP_COMM = 0x52ff,          /*!< 21247 enable http web page*/
    DISABLE_HTTP_COMM = 0x5300,          /*!< 21248 disable http web page*/
    ENABLE_SPECIAL_HTTP_COMM = 0x53ff,  /*!< 21503 enable special http web page*/
    DISABLE_SPECIAL_HTTP_COMM = 0x5400,  /*!< 21504 disable special http web page*/
    FLASH_ACCESS_CREDENTIAL_ERR_COMM = 0x54fe,   /*!< 0x54ff */
    FLASH_ACCESS_ERR_COMM = 0x54ff,   /*!< 0x54ff */
    SW_RESET_COMM = 0x5500,   /*!< 0x5500 software reset command*/
    SAVE_ALL_RETAINS_COMM  = 0x5501,    /*!< 0x5501 */
    SET_DEFAULT_VALUES_COMM = 0x5502,  /*!< 0x5502 set default value command(plc will reboot)*/
    CHANGE_OS_COMM = 0x5503,           /*!< 0x5503 Automatically return back*/
    RUN_OS1_COMM = 0x5504,             /*!< 0x5504 to main OS if not confirmed*/
    RUN_OS2_COMM = 0x5505,             /*!< 0x5505 during OS_CONFIRM_TIME*/
    CONFIRM_OS_COMM = 0x5506,          /*!< 0x5506 set current OS as main, command use after CHANGE_OS_COMM,RUN_OS1_COMM,RUN_OS2_COMM*/
    SET_MAIN_AND_RUN_OS1_COMM = 0x5507,/*!< 0x5507 run OS1 without confirmation*/
    SET_MAIN_AND_RUN_OS2_COMM = 0x5508,/*!< 0x5508 run OS2 without confirmation*/
    REMOTE_UPDATE_ENABLE_COMM = 0x5509, /*!< 0x5509 ignore BOOTKEY for download OS*/
    REMOTE_UPDATE_DISABLE_COMM = 0x550a,/*!< 0x550a check BOOTKEY for downloas OS*/
    LAN_RESET_COMM = 0x550b,            /*!< 0x550b check BOOTKEY for downloas OS*/
    MAKE_TEST_COMM = 0x550c,            /*!< 0x550c command for debug purpose*/
    DEEP_SLEEP_FOR_120_SEC_COMM = 0x550D,            /*!< 0x550d go to deep sleep*/
    RESET_WIFI_FOR_120_SEC_COMM = 0x550E,            /*!< 0x550e disables wifi*/
    DEEP_SLEEP_FOR_N_SEC_COMM = 0x550f,            /*!< 0x550d go to deep sleep for sleep_time seconds*/
    DEEP_SLEEP_COMM = 0x5510,            /*!< 0x550d go to deep sleep until external wake*/
    IIRLS_HANDSHAKE = 0x7000,   /*!< 28672 Issues a command for slave to perform "handshake" procedure.*/
    IIRLS_HANDSHAKE_CONFIRMATION = 0x7001,   /*!< 28673 Issues a command for slave to confirm successful "handshake".*/
    FEEDER_TASK_ONE_FEED_COMM = 0x8000, /*!< 32768 start feeder , only one cycle.*/
}main_command_t;
typedef enum{
    INTERLOCK_OK = BIT(0), /*!< when 3.3 v power is absent interlock dumped*/
}device_state_t;
/**
  * @brief reason of reset
  * @ingroup regs
  * @todo determine which group to include
  */
typedef enum {
    BOR_RST_REASON =    (1 << 0),
    PIN_RST_REASON =    (1 << 1),
    POR_RST_REASON =    (1 << 2),
    SOFT_RST_REASON =   (1 << 3),
    IWDG_RST_REASON =   (1 << 4),
    WWDG_RST_REASON =   (1 << 5),
    LPWR_RST_REASON =   (1 << 6),

    BOOT_KEY_DOWNLOAD_DISABLE =     (1 << 13),
    BOOT_KEY_REMOTE_UPDATE_ENABLE = (1 << 14),
}rst_reason_t;
typedef union{
    operand_t operand;
    u8 bytes[8];
}data_buffer_t;

typedef struct {
    data_buffer_t data;
    u8 byte_writed_flags;
    void * reg_address;
    u16 type;
    u16 index;
}temp_data_buffering_t;

typedef enum {
    WIFI_ACCESS_POINT = 1,
    WIFI_CLIENT       = 2,
    WIFI_AP_STA       = 3,
    WIFI_ESP32_CHANGED_ONLY_ACCESS_POINT = 4,
    WIFI_ESP32_CHANGED_ONLY_CLIENT       = 5,
    WIFI_ESP32_CHANGED_ONLY_AP_STA       = 6,
}wifi_setting_t;/*todo!!!rename to meso setting */

typedef enum {
    WIFI_SLIP_CONNECTED = (1<<0),
}wifi_state_t;/*todo!!!rename to meso setting */

/**
  * @addtogroup
  * @{
  * @todo determine which group to include
  */
#define DINAMIC_ADDRESS_SPACE_NUMM 30
#define ALL_MDB_CMD 0xff
/**
  * @}
  */

/**
  * @brief struct of dynamic address space
  * @ingroup regs
  * @todo determine which group to include
  */
typedef struct {
    u32 address;  /*!< modbus address 0 - 65535*/
    u8* data;     /*!< pointer to readdressing*/
    u32 len;      /*!< len of space */
    u8 command;   /*!< special modbus command or use 0xff for all commands*/
}dinamic_address_space_t;
//functions
/**
 * @brief regs_init
 * @return
 */
int regs_init(void );

void regs_event_handler (const void *pvParameters)FNCT_NO_RETURN;
int write_reg_to_mirror(void *main_reg);
int read_reg_from_bkram(void *main_reg);
u8 write_comm_to_bkp (main_command_t command);

/**
 * @brief regs_of_bkram_and_have_def_value
 * @param reg_address
 * @return  1 if vars in saved space and have def value in description
 */
int regs_saved_and_have_def_value(const void * reg_address);
int regs_set(void * reg_address,regs_access_t reg);
int regs_set_buffer(void * reg_address,u8* buffer_from,u16 byte_numm);
int regs_get(void * reg_address,regs_access_t* reg);
int regs_get_buffer(void * reg_address,u8* buffer_to,u16 byte_numm);
u8  regs_size_in_byte(regs_flag_t type);
int regs_write_internal(void * reg_address,regs_access_t reg);
/* @brief   copy data from global regs to local regs and vice versa
 * @param   reg_to - pointer to global or local regs
 * @param   reg_from - pointer to global or local regs
 * @param   size - size of regs for memcpy
 * @ingroup regs
 *
*/
void regs_copy_safe(void * reg_to,void * reg_from,u8 size);

#ifdef __cplusplus
}
#endif
#endif //REGS_H
