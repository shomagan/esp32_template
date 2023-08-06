/**
 * @file modbus_tcp.h
 * @defgroup free_rtos\lwip\src\include\lwip\apps\
 * @ingroup free_rtos\lwip\src\include\lwip\apps\
 * @version 0.1 
 * @brief  TODO!!! write brief in 
 */

#ifndef MODBUS_TCP_H
#define MODBUS_TCP_H 1
 
/*add includes below */
#include "type_def.h"
#include "os_type.h"
#include "lwip/opt.h"
#include "lwip/err.h"
#include "lwip/tcpbase.h"
/*add includes before */
#ifdef __cplusplus 
   extern "C" {
#endif
#define MODBUS_TCP_MAX_PACKET_SIZE 255
#define LWIP_MODBUS_TCP_ABORT_ON_CLOSE_MEM_ERROR 1
#define MODBUS_TCP_USE_MEM_POOL 1
#define LWIP_MODBUS_TCP_KILL_OLD_ON_CONNECTIONS_EXCEEDED 1
#define LWIP_MODBUS_TCP_SUPPORT_11_KEEPALIVE 0
#define MODBUS_TCP_MAX_RETRIES_WITHOUT_RECV 40
#define MEMP_NUM_PARALLEL_MODBUS_TCP_CONNS 8
/** The poll delay is X*500ms */
#if !defined MODBUS_TCP_POLL_INTERVAL || defined __DOXYGEN__
#define MODBUS_TCP_POLL_INTERVAL                 4
#endif

#if !defined MODBUS_TCP_DEBUG
#define MODBUS_TCP_DEBUG LWIP_DBG_OFF
#endif
#if !defined MODBUS_TCP_PRIO || defined __DOXYGEN__
#define MODBUS_TCP_PRIO TCP_PRIO_NORMAL
#endif
#define MODBUS_TCP_PORT 502u


extern SemaphoreHandle_t modbus_tcp_mutex;
err_t modbus_tcp_init(void);
/*add functions and variable declarations before */
#ifdef __cplusplus
}
#endif
#endif //MODBUS_TCP_H
