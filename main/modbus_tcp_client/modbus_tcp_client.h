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
extern client_request_t client_requests[];
extern area_node_t area_nodes[];
/* Execute a Query/Response suspended transaction between client and server */
/* returns: <0    -> ERROR: error codes
 *          >2    -> SUCCESS: frame length
 *
 */
int modbus_tcp_master_packet_transaction(u16 channel,u8  *packet,u16 send_length,
                              u16 recv_length,u32 timeout,int socket);
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
int add_ip_to_slave_table(uc8 ip_addr[4]);
/**
 * @brief get_ip_slaves_table_size
 * @return number of stored slaves ip
 */
u16 get_ip_slaves_table_size(void);
/**
 * @brief get_ip_slaves_by_number
 * @param ip_address pointer where address will be coppied
 * @param number should be less then slaves_number
 * @return 1 if ip address was coppied to ip_address
 */
int get_ip_slaves_by_number(u8 * ip_address,u16 number);
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
FNCT_NO_RETURN void modbus_tcp_client_task(void * argument);
/*add functions and variable declarations before */
#ifdef __cplusplus
}
#endif
#endif //MODBUS_TCP_CLIENT_H
