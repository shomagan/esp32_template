/**
 * @file modbus_tcp.c
 * @defgroup modbus_tcp_server
 * @ingroup apps
 * @version 0.1
 * @brief  modbus tcp protocol on 502 port 
 */

#ifndef MODBUS_TCP_C
#define MODBUS_TCP_C 1
#include "modbus_tcp.h"
#include "lwip/opt.h"
#include "lwip/init.h"
#include "lwip/debug.h"
#include "lwip/stats.h"
#include "lwip/def.h"

#if MODBUS_TCP_ENABLE

#include "lwip/tcp.h"
#include "lwip/timeouts.h"
#include "modbus.h"
#include <stdio.h>
#include <string.h>

#include "lwip/altcp.h"
#include "lwip/altcp_tcp.h"
#include "lwip/altcp_tls.h"
static const char *TAG = "modbus_tcp";
/*add functions and variable declarations below */
/* structure for maintaing connection infos to be passed as argument
to LwIP callbacks*/
struct modbus_tcp_state{
#if LWIP_MODBUS_TCP_KILL_OLD_ON_CONNECTIONS_EXCEEDED
    struct modbus_tcp_state *next;
#endif /* LWIP_HTTPD_KILL_OLD_ON_CONNECTIONS_EXCEEDED */
    u32 state;             /* current connection state */
    u32 time_to_live;     //time in poll while to live with out packet
    struct altcp_pcb *pcb;    /* pointer on the current tcp_pcb */
    u32 left;       /* Number of unsent bytes in buf. */
    u32 retries;
    u32 keepalive;
    u32 time_started;
    u16 len;
    u16 flags;
    u8 buff_received[MODBUS_TCP_MAX_PACKET_SIZE+1];   //buff for modbus packet in tcp
    u8 buff_send[MODBUS_TCP_MAX_PACKET_SIZE+1];   //buff for modbus packet in tcp
};
/** Allocate a struct modbus_tcp_state. */
#if MODBUS_TCP_USE_MEM_POOL
LWIP_MEMPOOL_DECLARE(MODBUS_TCP_STATE,     MEMP_NUM_PARALLEL_MODBUS_TCP_CONNS,     sizeof(struct modbus_tcp_state),     "MODBUS_TCP_STATE")
#define MODBUS_TCP_ALLOC_STATE() (struct modbus_tcp_state *)LWIP_MEMPOOL_ALLOC(MODBUS_TCP_STATE)
#define MODBUS_TCP_FREE_STATE(x) LWIP_MEMPOOL_FREE(MODBUS_TCP_STATE, (x))
#else /* MODBUS_TCP_USE_MEM_POOL */
#define MODBUS_TCP_ALLOC_STATE() (struct modbus_tcp_state *)mem_malloc(sizeof(struct modbus_tcp_state))
#define MODBUS_TCP_FREE_STATE(x) mem_free(x)
#endif
#ifdef DEBUG
    #undef DEBUG
#endif
#define DEBUG 0
#if DEBUG
    static u32 time_recv = 0;
    static u32 time_send = 0;
    u32 modbus_tcp_time_handl = 0;
#endif
static struct modbus_tcp_state *modbus_tcp_connections =NULL;
static void modbus_tcp_write_wrapped(void * p);
static err_t modbus_tcp_write_sofi(struct altcp_pcb *pcb, const void *ptr, u16 *length, u16 apiflags);

static err_t modbus_tcp_close_conn(struct altcp_pcb *pcb,\
                      struct modbus_tcp_state *modbus_tcp_connection);
static err_t modbus_tcp_close_or_abort_conn(struct altcp_pcb *pcb,\
                               struct modbus_tcp_state *modbus_tcp_connection,\
                               u8_t abort_conn);
static void modbus_tcp_remove_connection(struct modbus_tcp_state *connection);
static struct modbus_tcp_state * modbus_tcp_state_alloc(void);
static void modbus_tcp_state_init(struct modbus_tcp_state *connection);

static void modbus_tcp_err(void *arg, err_t err);
static err_t modbus_tcp_sent(void *arg, struct altcp_pcb *pcb, u16_t len);
static err_t modbus_tcp_poll(void *arg, struct altcp_pcb *pcb);
static err_t modbus_tcp_recv(void *arg, struct altcp_pcb *pcb,\
                             struct pbuf *p, err_t err);
#if LWIP_MODBUS_TCP_KILL_OLD_ON_CONNECTIONS_EXCEEDED
static void modbus_tcp_kill_oldest_connection(void);
#endif
semaphore_handle_t modbus_tcp_mutex = NULL;
/** Free a struct http_state.
 * Also frees the file data if dynamic.
 */
static void modbus_tcp_state_free(struct modbus_tcp_state *connection){
    if (connection != NULL){
        connection->state = 0;
        modbus_tcp_remove_connection(connection);
        MODBUS_TCP_FREE_STATE(connection);
    }
}

static void modbus_tcp_add_connection(struct modbus_tcp_state *connection){
    /* add the connection to the list */
    semaphore_take(modbus_tcp_mutex, portMAX_DELAY );
    connection->next = modbus_tcp_connections;
    modbus_tcp_connections = connection;
    semaphore_release(modbus_tcp_mutex);
}

static void modbus_tcp_remove_connection(struct modbus_tcp_state *connection){
    /* take the connection off the list */
    semaphore_take(modbus_tcp_mutex, portMAX_DELAY );
    if (modbus_tcp_connections) {
        if (modbus_tcp_connections == connection) {
            modbus_tcp_connections = connection->next;
        } else {
            struct modbus_tcp_state *last;
            //find and cut from chain
            for (last = modbus_tcp_connections; last->next != NULL; last = last->next) {
                if (last->next == connection) {
                    last->next = connection->next;
                    break;
                }
            }
        }
    }
    semaphore_release(modbus_tcp_mutex);
}
static struct modbus_tcp_state * modbus_tcp_state_alloc(void){
    struct modbus_tcp_state *ret = MODBUS_TCP_ALLOC_STATE();
#if LWIP_MODBUS_TCP_KILL_OLD_ON_CONNECTIONS_EXCEEDED
    if(ret == NULL){
        modbus_tcp_kill_oldest_connection();
        ret = MODBUS_TCP_ALLOC_STATE();
    }
#endif /* LWIP_MODBUS_TCP_KILL_OLD_ON_CONNECTIONS_EXCEEDED */
    if(ret != NULL){
        modbus_tcp_state_init(ret);
        modbus_tcp_add_connection(ret);
    }
    return ret;
}
/** Initialize a struct modbus_tcp_state.
 */
static void modbus_tcp_state_init(struct modbus_tcp_state *connection){
  /* Initialize the structure. */
    semaphore_take(modbus_tcp_mutex, portMAX_DELAY );
    memset(connection, 0, sizeof(struct modbus_tcp_state));
    semaphore_release(modbus_tcp_mutex);
}
/*@brief wrapping for use tcp call back message
 *
 * */
void modbus_tcp_write_wrapped(void * p){
    struct modbus_tcp_state * connection = p ;
    modbus_tcp_write_sofi(connection->pcb, connection->buff_send,\
                            &connection->len, connection->flags);
}

/** Call tcp_write() in a loop trying smaller and smaller length
 *
 * @param pcb altcp_pcb to send
 * @param ptr Data to send
 * @param length Length of data to send (in/out: on return, contains the
 *        amount of data sent)
 * @param apiflags directly passed to tcp_write
 * @return the return value of tcp_write
 */
err_t modbus_tcp_write_sofi(struct altcp_pcb *pcb, const void *ptr, u16 *length, u16 apiflags){
    u16_t len, max_len;
    err_t err;
    LWIP_ASSERT("length != NULL", length != NULL);
    if(pcb==NULL){
        return ERR_ABRT;
    }
    len = *length;
    if (len == 0) {
        return ERR_OK;
    }
#if DEBUG
    time_send = osKernelSysTick();
#endif
    /* We cannot send more data than space available in the send buffer. */
    max_len = altcp_sndbuf(pcb);
    if (max_len < len) {
        len = max_len;
    }
    do {
        LWIP_DEBUGF(MODBUS_TCP_DEBUG | LWIP_DBG_TRACE,\
                    ("Trying to send %d bytes\n", len));
        err = altcp_write(pcb, ptr, len, (u8)apiflags);
        if (err == ERR_MEM) {
            if ((altcp_sndbuf(pcb) == 0) ||
                (altcp_sndqueuelen(pcb) >= TCP_SND_QUEUELEN)) {
            /* no need to try smaller sizes */
                len = 1;
            } else {
                len /= 2;
            }
            LWIP_DEBUGF(MODBUS_TCP_DEBUG | LWIP_DBG_TRACE,\
                  ("Send failed, trying less (%d bytes)\n", len));
        }else{
            if(altcp_output(pcb)!=ERR_OK){
                main_error_message(TAG,"Failed %s:%d\n",__FILE__,__LINE__);
            }
        }
    } while ((err == ERR_MEM) && (len > 1));
    if (err == ERR_OK) {
        LWIP_DEBUGF(MODBUS_TCP_DEBUG| LWIP_DBG_TRACE, ("Sent %d bytes\n", len));
        *length = len;
    } else {
        LWIP_DEBUGF(MODBUS_TCP_DEBUG | LWIP_DBG_TRACE,\
                    ("Send failed with err %d (\"%s\")\n", err, lwip_strerr(err)));
        *length = 0;
    }
#if LWIP_MODBUS_TCP_SUPPORT_11_KEEPALIVE
    /* ensure nagle is normally enabled (only disabled for persistent connections
     when all data has been enqueued but the connection stays open for the next
     request */
    altcp_nagle_enable(pcb);
#endif
    return err;
}

/**
 * The pcb had an error and is already deallocated.
 * The argument might still be valid (if != NULL).
 */
static void modbus_tcp_err(void *arg, err_t err){
    struct modbus_tcp_state *connection= (struct modbus_tcp_state *)arg;
    LWIP_UNUSED_ARG(err);
    LWIP_DEBUGF(MODBUS_TCP_DEBUG, ("http_err: %s", lwip_strerr(err)));
    if (connection != NULL) {
        modbus_tcp_state_free(connection);
    }
}
/**
 * Data has been sent and acknowledged by the remote host.
 * This means that more data can be sent.
 */
static err_t modbus_tcp_sent(void *arg, struct altcp_pcb *pcb, u16_t len){
  struct modbus_tcp_state *connection = (struct modbus_tcp_state *)arg;
  LWIP_DEBUGF(MODBUS_TCP_DEBUG | LWIP_DBG_TRACE, ("modbus_tcp_sent %p\n", (void *)pcb));
  LWIP_UNUSED_ARG(len);
  if (connection == NULL){
    return ERR_OK;
  }
  semaphore_take(modbus_tcp_mutex, portMAX_DELAY );
  connection->retries = 0;
  semaphore_release(modbus_tcp_mutex);
  return ERR_OK;
}

/**
 * The poll function is called every 2nd second.
 * If there has been no data sent (which resets the retries) in 8 seconds, close.
 * If the last portion of a file has not been sent in 2 seconds, close.
 *
 * This could be increased, but we don't want to waste resources for bad connections.
 */
static err_t modbus_tcp_poll(void *arg, struct altcp_pcb *pcb){
    struct modbus_tcp_state *connection = (struct modbus_tcp_state *)arg;
    LWIP_DEBUGF(MODBUS_TCP_DEBUG | LWIP_DBG_TRACE, ("http_poll: pcb=%p hs=%p pcb_state=%s\n",
          (void *)pcb, (void *)connection, tcp_debug_state_str(altcp_dbg_get_tcp_state(pcb))));
    if (connection == NULL) {
        err_t closed;
        /* arg is null, close. */
        LWIP_DEBUGF(MODBUS_TCP_DEBUG, ("http_poll: arg is NULL, close\n"));
        closed = modbus_tcp_close_conn(pcb, NULL);
        LWIP_UNUSED_ARG(closed);
#if LWIP_MODBUS_TCP_ABORT_ON_CLOSE_MEM_ERROR
        if (closed == ERR_MEM) {
            altcp_abort(pcb);
            return ERR_ABRT;
        }
#endif /* LWIP_HTTPD_ABORT_ON_CLOSE_MEM_ERROR */
        return ERR_OK;
    } else {
        semaphore_take(modbus_tcp_mutex, portMAX_DELAY );
        connection->retries++;
        if (connection->retries >= MODBUS_TCP_MAX_RETRIES_WITHOUT_RECV) {
			connection->retries = 0;
            LWIP_DEBUGF(MODBUS_TCP_DEBUG, ("http_poll: too many retries, close\n"));
            semaphore_release(modbus_tcp_mutex);
            modbus_tcp_close_or_abort_conn(pcb, connection, 1);
            return ERR_OK;
        }
        semaphore_release(modbus_tcp_mutex);
    }
    return ERR_OK;
}

/**
 * Data has been received on this pcb.
 *
 */
static err_t modbus_tcp_recv(void *arg, struct altcp_pcb *pcb,\
                             struct pbuf *p, err_t err){
    struct modbus_tcp_state *connection = (struct modbus_tcp_state *)arg;
    LWIP_DEBUGF(MODBUS_TCP_DEBUG | LWIP_DBG_TRACE,\
    ("modbus_tcp_recv: pcb=%p pbuf=%p err=%s\n", (void *)pcb,\
    (void *)p, lwip_strerr(err)));
    if ((err != ERR_OK) || (p == NULL) || (connection == NULL)) {
        /* error or closed by other side? */
        if (p != NULL) {
            /* Inform TCP that we have taken the data. */
            altcp_recved(pcb, p->tot_len);
            pbuf_free(p);
        }
        if (connection == NULL) {
            /* this should not happen, only to be robust */
            LWIP_DEBUGF(MODBUS_TCP_DEBUG, ("Error, modbus_tcp_recv: hs is NULL, close\n"));
        }
        modbus_tcp_close_conn(pcb, connection);
        return ERR_OK;
    }
    /* Inform TCP that we have taken the data. */
    altcp_recved(pcb, p->tot_len);
    semaphore_take(modbus_tcp_mutex, portMAX_DELAY );
    connection->retries = 0;
    if(p->tot_len<MODBUS_TCP_MAX_PACKET_SIZE){
        struct pbuf *q;
        u32 len_temp=0;
        for (q = p;q!=NULL;q=q->next){
            memcpy(connection->buff_received + len_temp,q->payload,q->len);
            len_temp += q->len;
        }
        connection->len = p->tot_len;

#if DEBUG
        time_recv = osKernelSysTick();
#endif
        semaphore_release(modbus_tcp_mutex);
        u16 len_return=0;
        if(it_modbus_request_check(connection->buff_received,(u16)len_temp)==1){
            //rtu packet over tcp
            if(modbus_packet_for_me(connection->buff_received,(u16)len_temp)){
                len_return = modbus_rtu_packet(connection->buff_received,(u16)len_temp);
            }
        }else if(it_modbus_tcp_full_check(connection->buff_received,(u16)len_temp)==1){
            //modbus tcp packet
            if(modbus_packet_for_me(&connection->buff_received[MODBUS_TCP_HEADER_SIZE],(u16)len_temp)){
                len_return = modbus_tcp_packet(connection->buff_received,(u16)len_temp);
            }
        }else{
            len_return = modbus_err_packet_type(MODBUS_TCP_PACKET,\
                       connection->buff_received,ILLEGAL_FUNCTION);
        }
        semaphore_take(modbus_tcp_mutex, portMAX_DELAY);
        if(len_return){
            connection->len = len_return;
            connection->flags = TCP_WRITE_FLAG_COPY;
            memcpy(connection->buff_send,connection->buff_received,connection->len);
            semaphore_release(modbus_tcp_mutex);
            modbus_tcp_write_wrapped((void *)connection );
            semaphore_take(modbus_tcp_mutex, portMAX_DELAY);
        }
        semaphore_release(modbus_tcp_mutex);

        semaphore_take(modbus_tcp_mutex, portMAX_DELAY);
    }
    semaphore_release(modbus_tcp_mutex);
//todo receive packet and take out to packet manager
    pbuf_free(p);
    return ERR_OK;
}
/**
 * The connection shall be actively closed.
 * Reset the sent- and recv-callbacks.
 *
 * @param pcb the tcp pcb to reset callbacks
 * @param hs connection state to free
 */
static err_t modbus_tcp_close_conn(struct altcp_pcb *pcb,\
                      struct modbus_tcp_state *modbus_tcp_connection){
  return modbus_tcp_close_or_abort_conn(pcb, modbus_tcp_connection, 0);
}

/**
 * The connection shall be actively closed (using RST to close from fault states).
 * Reset the sent- and recv-callbacks.
 *
 * @param pcb the tcp pcb to reset callbacks
 * @param hs connection state to free
 */
static err_t modbus_tcp_close_or_abort_conn(struct altcp_pcb *pcb,\
                               struct modbus_tcp_state *connection,\
                               u8_t abort_conn){
    err_t err;
    LWIP_DEBUGF(MODBUS_TCP_DEBUG, ("Closing connection %p\n", (void *)pcb));
    altcp_arg(pcb, NULL);
    altcp_recv(pcb, NULL);
    altcp_err(pcb, NULL);
    altcp_poll(pcb, NULL, 0);
    altcp_sent(pcb, NULL);
    if (connection != NULL) {
        connection->state = 0;
        modbus_tcp_state_free(connection);
    }
    if (abort_conn) {
        altcp_abort(pcb);
        return ERR_OK;
    }
    err = altcp_close(pcb);
    if (err != ERR_OK) {
        LWIP_DEBUGF(MODBUS_TCP_DEBUG, ("Error %d closing %p\n", err, (void *)pcb));
        /* error closing, try again later in poll */
        altcp_poll(pcb, modbus_tcp_poll, MODBUS_TCP_POLL_INTERVAL);
    }
    return err;
}
#if LWIP_MODBUS_TCP_KILL_OLD_ON_CONNECTIONS_EXCEEDED
static void modbus_tcp_kill_oldest_connection(void){
  struct modbus_tcp_state * connection = modbus_tcp_connections;
  struct modbus_tcp_state * connection_free_next = NULL;
  while (connection && connection->next) {
    connection_free_next  = connection;
    LWIP_ASSERT("broken list", connection != connection->next);
    connection = connection->next;
  }
  if (connection_free_next != NULL) {
    LWIP_ASSERT("hs_free_next->next != NULL", connection_free_next->next != NULL);
    LWIP_ASSERT("hs_free_next->next->pcb != NULL", connection_free_next->next->pcb != NULL);
    /* send RST when killing a connection because of memory shortage */
    modbus_tcp_close_or_abort_conn(connection_free_next->next->pcb, connection_free_next->next, 1);
    /* this also unlinks the modbus_tcp_state from the list */
  }
}
#endif
/**
 * A new incoming connection has been accepted.
 */
static err_t modbus_tcp_accept(void *arg, struct altcp_pcb *pcb, err_t err){
    LWIP_UNUSED_ARG(err);
    LWIP_UNUSED_ARG(arg);
    struct modbus_tcp_state * connection;
    LWIP_DEBUGF(MODBUS_TCP_DEBUG, ("modbus_tcp_accept%p / %p\n", (void *)pcb, arg));
    if ((err != ERR_OK) || (pcb == NULL)){
        return ERR_VAL;
    }
    /* Set priority */
    altcp_setprio(pcb, MODBUS_TCP_PRIO);
    connection = modbus_tcp_state_alloc();
    if (connection == NULL) {
      LWIP_DEBUGF(MODBUS_TCP_DEBUG, ("MODBUS_TCP_DEBUG: Out of memory, RST\n"));
      return ERR_MEM;
    }
    connection->pcb = pcb;
    /* Tell TCP that this is the structure we wish to be passed for our
    callbacks. */
    altcp_arg(pcb, connection);
    /* Set up the various callback functions */
    altcp_recv(pcb, modbus_tcp_recv);
    altcp_err(pcb, modbus_tcp_err);
    altcp_poll(pcb, modbus_tcp_poll, MODBUS_TCP_POLL_INTERVAL);
    altcp_sent(pcb, modbus_tcp_sent);
    return ERR_OK;
}

static void modbus_tcp_init_pcb(struct altcp_pcb *pcb, u16_t port){
    err_t err;
    if (pcb) {
        altcp_setprio(pcb, MODBUS_TCP_PRIO);
        /* set SOF_REUSEADDR here to explicitly bind httpd to multiple interfaces */
        err = altcp_bind(pcb, IP_ANY_TYPE, port);
        LWIP_UNUSED_ARG(err); /* in case of LWIP_NOASSERT */
        LWIP_ASSERT("httpd_init: tcp_bind failed", err == ERR_OK);
        pcb = altcp_listen(pcb);
        LWIP_ASSERT("httpd_init: tcp_listen failed", pcb != NULL);
        altcp_accept(pcb, modbus_tcp_accept);
    }
}
/** @ingroup modbus_tcp_server
 * @brief Initialize modbus_tcp server.
 *
 */
err_t modbus_tcp_init(void){
    struct altcp_pcb *pcb;
#if MODBUS_TCP_USE_MEM_POOL
    LWIP_MEMPOOL_INIT(MODBUS_TCP_STATE);
#endif
    semaphore_create_binary(modbus_tcp_mutex)
    pcb = altcp_tcp_new_ip_type(IPADDR_TYPE_ANY);
    LWIP_ASSERT("modbus_tcp_init: tcp_new failed", pcb != NULL);
    modbus_tcp_init_pcb(pcb, MODBUS_TCP_PORT);
    return ERR_OK;
}
/** @ingroup modbus_tcp_server
 * Deinitialize ("turn off") modbus_tcp_server.
 */
void modbus_tcp_cleanup(void){
    semaphore_delete(modbus_tcp_mutex);
}

#endif //MODBUS_TCP_ENABLE
#endif //MODBUS_TCP_C
