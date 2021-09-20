/**
 * @file modbus_master_description.h
 * @author Shoma Gane <shomagan@gmail.com>
 *         
 * @defgroup inc
 * @ingroup inc
 * @version 0.1 
 * @brief  TODO!!! write brief in 
 */
#ifndef MODBUS_MASTER_DESCRIPTION_H
#define MODBUS_MASTER_DESCRIPTION_H 1
 
/*add includes below */
#include "type_def.h"
#include "main_config.h"
#include "modbus_tcp/modbus_tcp.h"
/*add includes before */
#ifdef __cplusplus 
   extern "C" {
#endif
/*add functions and variable declarations below */
/* The total number of nodes, needed to support _all_ instances of the modbus plugin */
#define TOTAL_TCPNODE_COUNT       1
#define TOTAL_RTUNODE_COUNT       0// not relized
#define TOTAL_ASCNODE_COUNT       0// not relized
#define TOTAL_ROUT_NODE_COUNT     0// not relized
#define TOTAL_MODBUS_AREA_COUNT   0// not relized
/* Values for instance 1 of the modbus plugin */
#define MAX_NUMBER_OF_TCPCLIENTS  0
#define NUMBER_OF_TCPSERVER_NODES 0// not used
#define NUMBER_OF_TCPCLIENT_NODES 1
#define NUMBER_OF_TCPCLIENT_REQTS 7

#define NUMBER_OF_RTUSERVER_NODES 0// not relized
#define NUMBER_OF_RTUCLIENT_NODES 0// not relized
#define NUMBER_OF_RTUCLIENT_REQTS 0// not relized

#define NUMBER_OF_ASCIISERVER_NODES 0// not relized
#define NUMBER_OF_ASCIICLIENT_NODES 0// not relized
#define NUMBER_OF_ASCIICLIENT_REQTS 0// not relized

#define NUMBER_OF_SERVER_NODES (NUMBER_OF_TCPSERVER_NODES + \
                                NUMBER_OF_RTUSERVER_NODES + \
                               NUMBER_OF_ASCIISERVER_NODES)

#define NUMBER_OF_CLIENT_NODES (NUMBER_OF_TCPCLIENT_NODES + \
                                NUMBER_OF_RTUCLIENT_NODES + \
                                NUMBER_OF_ASCIICLIENT_NODES)

#define NUMBER_OF_CLIENT_REQTS (NUMBER_OF_TCPCLIENT_REQTS + \
                                NUMBER_OF_RTUCLIENT_REQTS + \
                                NUMBER_OF_ASCIICLIENT_REQTS)

#define MAX_READ_BITS 254
#define MAX_WORD_NUM 127
#define MAX_PACKET_LEN (MAX_WORD_NUM*2 + 7)

#define COILS_01 1
#define INPUT_DISCRETES_02 2
#define HOLDING_REGISTERS_03 3
#define INPUT_REGISTERS_04 4
typedef enum{
   TEST_REQUEST_TYPE=0,
   MODBUS_MASTER_SET_0=1,
   MODBUS_MASTER_SET_1=2,
   MODBUS_MASTER_READ_0=3,
   MODBUS_MASTER_READ_1=4,
   MODBUS_MASTER_SET_2=5,
   MODBUS_MASTER_COMMAND=6,
}request_type;

static u16 com_buffer_req_0[1] = {0};
static u16 com_buffer_req_1[1] = {0};
static u16 com_buffer_req_2[1] = {0};
static u16 com_buffer_req_3[100] = {0};
static u16 com_buffer_req_4[20] = {0};
static u16 com_buffer_req_5[8] = {0};
static u16 com_buffer_req_6[10] = {0};

client_request_t	client_requests[NUMBER_OF_CLIENT_REQTS] = {
{PACKET_CHANNEL_TCP/*channel*/, 3/*slave_id*/, req_input/*req_type*/, 3/*mb_function*/, 20101 /*first reg address*/,
1/*reg number*/,DEF_REQ_SEND_RETRIES/*retries*/, 0 /* error_code */, 0 /* prev_code */, 200/* timeout */,
NULL, com_buffer_req_0},
{PACKET_CHANNEL_TCP/*channel*/, 3/*slave_id*/, req_input/*req_type*/, 6/*mb_function*/, 63521 /*first reg address*/,
1/*reg number*/,DEF_REQ_SEND_RETRIES/*retries*/, 0 /* error_code */, 0 /* prev_code */, 200/* timeout */,
NULL, com_buffer_req_1},
{PACKET_CHANNEL_TCP/*channel*/, 3/*slave_id*/, req_input/*req_type*/, 6/*mb_function*/, 20028 /*first reg address*/,
1/*reg number*/,DEF_REQ_SEND_RETRIES/*retries*/, 0 /* error_code */, 0 /* prev_code */, 200/* timeout */,
NULL, com_buffer_req_2},
{PACKET_CHANNEL_TCP/*channel*/, 3/*slave_id*/, req_input/*req_type*/, 3/*mb_function*/, 31500 /*first reg address*/,
100/*reg number*/,DEF_REQ_SEND_RETRIES/*retries*/, 0 /* error_code */, 0 /* prev_code */, 200/* timeout */,
NULL, com_buffer_req_3},
{PACKET_CHANNEL_TCP/*channel*/, 3/*slave_id*/, req_input/*req_type*/, 3/*mb_function*/, 20 /*first reg address*/,
20/*reg number*/,DEF_REQ_SEND_RETRIES/*retries*/, 0 /* error_code */, 0 /* prev_code */, 200/* timeout */,
NULL, com_buffer_req_4},
{PACKET_CHANNEL_TCP/*channel*/, 3/*slave_id*/, req_input/*req_type*/, 16/*mb_function*/, 63502 /*first reg address*/,
8/*reg number*/,DEF_REQ_SEND_RETRIES/*retries*/, 0 /* error_code */, 0 /* prev_code */, 200/* timeout */,
NULL, com_buffer_req_5},
{PACKET_CHANNEL_TCP/*channel*/, 3/*slave_id*/, req_input/*req_type*/, 6/*mb_function*/, 20100/*first reg address*/,
1/*reg number*/,DEF_REQ_SEND_RETRIES/*retries*/, 0 /* error_code */, 0 /* prev_code */, 200/* timeout */,
NULL, com_buffer_req_6},
};


/*add functions and variable declarations before */
#ifdef __cplusplus
}
#endif
#endif //MODBUS_MASTER_DESCRIPTION_H
