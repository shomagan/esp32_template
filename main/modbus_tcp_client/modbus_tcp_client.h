/**
 * @file modbus_tcp_client.h
 * @author Shoma Gane <shomagan@gmail.com>
 *
 * @defgroup inc
 * @ingroup inc
 * @version 0.1
 * @brief  TODO!!! write brief in
 */
#ifndef MODBUS_TCP_CLIENT_H
#define MODBUS_TCP_CLIENT_H 1

/*add includes below */
#include "type_def.h"
#include "os_type.h"
#include "main_config.h"
#include "regs_description.h"

/*add includes before */
#ifdef __cplusplus
   extern "C" {
#endif
/*add functions and variable declarations below */
/**
* Set the maximum number of client connections
*/
#define CONFIG_CS104_MAX_CLIENT_CONNECTIONS 1
/* activate TCP keep alive mechanism. 1 -> activate */
#define CONFIG_ACTIVATE_TCP_KEEPALIVE 1
/* time (in s) between last message and first keepalive message */
#define CONFIG_TCP_KEEPALIVE_IDLE 5
/* time between subsequent keepalive messages if no ack received */
#define CONFIG_TCP_KEEPALIVE_INTERVAL 2
/* number of not missing keepalive responses until socket is considered dead */
#define CONFIG_TCP_KEEPALIVE_CNT 2
#define PORT_FAILURE   -101
#define INTERNAL_ERROR -102
#define TIMEOUT        -103
#define INVALID_FRAME  -104
#define MODBUS_ERROR   -105
#define DEF_REQ_SEND_RETRIES 0
#define MODBUS_THREAD_SIZE 512
#define MODBUS_AREA_COUNT   4
#define MDB_REQTS_COUNT 1
#define MODBUS_MAX_WORD_NUM 112
#define MAX_NUMBER_OF_SLAVES_CONNECTIONS 20

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

typedef enum {
   optimize_speed,
   optimize_size
} optimization_t;

typedef enum {
   naf_ascii,
   naf_rtu,
   naf_tcp,
} node_addr_family_t;

typedef struct MCU_PACK{
   const char *host;
   const char *service;
   int         close_on_silence;
} node_addr_tcp_t;

typedef struct MCU_PACK {
   u16 channel;
   int         baud;       /* plain baud rate, eg 2400; zero for the default 9600 */
   int         parity;     /* 0 for none, 1 for odd, 2 for even                   */
   int         data_bits;
   int         stop_bits;
   int         ignore_echo; /* 1 => ignore echo; 0 => do not ignore echo */
} node_addr_rtu_t;

typedef node_addr_rtu_t node_addr_ascii_t;

typedef union {
   node_addr_ascii_t ascii;
   node_addr_rtu_t   rtu;
   node_addr_tcp_t   tcp;
} node_addr_common_t;

typedef struct MCU_PACK{
   node_addr_family_t  naf;
   node_addr_common_t  addr;
} node_addr_t;


typedef struct MCU_PACK{
   const char *location;
   u8		slave_id;
   node_addr_t	node_address;
   int		mb_nd;      // modbus library node used for this server
   int		init_state; // store how far along the server's initialization has progressed
   task_handle_t	thread_id;  // thread handling this server
} server_node_t;


// Used by the Modbus client node
typedef struct MCU_PACK{
   const char *location;
   node_addr_t	node_address;
   int		mb_nd;
   int		init_state; // store how far along the client's initialization has progressed
   u32		comm_period;
   int		prev_error; // error code of the last printed error message (0 when no error)
   task_handle_t	thread_id;  // thread handling all communication with this client
} client_node_t;


// Used by the Modbus client plugin
typedef enum {
   req_input,
   req_output,
   no_request		/* just for tests to quickly disable a request */
} iotype_t;

#define REQ_BUF_SIZE 64
typedef struct MCU_PACK{
   u16 channel;
   u8		slave_id;
   iotype_t	req_type;
   u8		mb_function;
   u16		address;
   u16		count;
   int		retries;
   u8		error_code; // modbus error code (if any) of current request
   int		prev_error; // error code of the last printed error message (0 when no error)
   u32 resp_timeout;
   // buffer used to store located PLC variables
   u16	* plcv_buffer;
   // buffer used to store data coming from / going to server
   u16 * coms_buffer;
} client_request_t;

typedef struct MCU_PACK{
   u16 * function;
   u16 * slave_address;
   u16 * regs_number;
   u16 * start_address;
   u16 * enable;
   u16 * error_counter;
   u16 * success_counter;
   u16 * reserv;
} modbus_request_state_t;
typedef struct{
   u8 channel_from;
   u8 channel_to;
   u8 mdb_address;
   u8 res;
} route_node_t;
typedef struct{
   u8 command;
   u16 regs_number;
   u16 mdb_or_coil_address;
   u8 * data;
   u8 location0;
   u8 location1;
} area_node_t;

#define MAX_NUMBER_OF_SLAVES 256
typedef struct {
    in_addr_t ip;
    u8 modbus_id;
}slave_table_item_t;

typedef struct {
    task_handle_t task_id;
    const regs_description_t * first_regs_description;
    u32 size_in_words;
    u32 failed_requests;
    u32 success_requests;
}modbus_tcp_client_slave_connections_t;
typedef struct{
    fd_set write_set;
    fd_set read_set;
    fd_set error_set;
}file_desc_set_t;


extern client_request_t client_requests[];
extern area_node_t area_nodes[];
/* Execute a Query/Response suspended transaction between client and server */
/* returns: <0    -> ERROR: error codes
 *          >2    -> SUCCESS: frame length
 *
 */
int modbus_tcp_master_packet_transaction(u16 channel,u8  *packet,u16 send_length,
                              u16 recv_length,u32 timeout,int socket,file_desc_set_t fds);
/**
 * @brief clean_slave_table cleal full slave table
 * @return 0 if all was passed good
 */
int clean_slave_table(void);
/**
 * @brief add_ip_to_slave_table add ip address to slave table
 * @param ip_addr
 * @return
 */
int add_ip_to_slave_table(uc8 ip_addr[4], u8 modbus_id);
/**
 * @brief get_ip_slaves_table_size
 * @return number of stored slaves ip
 */
u16 get_ip_slaves_table_size(void);
/**
 * @brief need_to_prefill_data
 * @return 1 if need to make prefetch
 */
u8 is_prefetching_state(void);
/**
 * @brief end_prefetch_adc_buffer command to end prefetch buffer
 * @return
 */
int end_prefetch_adc_buffer(void);
/**
 * @brief prefill_data_to_buff when we are searching devices we fill adc to buffer for
 * dark state
 * @param position in buffer
 * @param data - adc data
 * @return
 */
int prefill_data_to_buff(u16 position,u16 data);
/**
 * @brief start_prefetch_adc_buffer start proccess of copy data to prefetch buffer
 *
 *  IIRLS_MASTER use it only inside modbus_tcp_client task for start preparing data to buffer
 *
 *  IIRLS_SLAVE use it for start handshake process and set up duty cycles to 2500 on 1 second
 * @return
 */
int start_prefetch_adc_buffer(void);
extern task_handle_t modbus_master_id;
FNCT_NO_RETURN void modbus_tcp_client_common_task(void * argument);
/*add functions and variable declarations before */
#ifdef __cplusplus
}
#endif
#endif //MODBUS_TCP_CLIENT_H
