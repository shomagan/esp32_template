/**
 * @file modbus_tcp_client.c
 * @author Shoma Gane <shomagan@gmail.com>
 *         
 * @defgroup src
 * @ingroup src
 * @version 0.1 
 * @brief  TODO!!! write brief in 
 */
#ifndef MODBUS_TCP_CLIENT_C
#define MODBUS_TCP_CLIENT_C 1

#include "regs.h"
#include "lwip/ip_addr.h"
#include "lwip/sockets.h"
#include "lwip/opt.h"
#include "lwip/sys.h"
#include "lwip/mem.h"
#include "lwip/netdb.h"
#include <stdio.h>
#include <string.h>
#include "modbus_tcp_client.h"
#include "udp_broadcast.h"
#include "iirls_handler.h"
#include "os_type.h"
#include "common.h"
#include "modbus.h"
#include <sys/socket.h>
#include <u8g2.h>
#include "u8g2_esp32_hal.h"
#ifndef DEBUG_SOCKET
#define DEBUG_SOCKET 1
#endif
#include "modbus_master_description.h"
#define SEARCHING_TIME_MS 7500
#define MAX_NUMBER_OF_REGS_BLOCKS 256
static const char *TAG = "modbus_tcp_client";

modbus_tcp_client_slave_connections_t modbus_tcp_client_slave_connections[MAX_NUMBER_OF_SLAVES_CONNECTIONS];
/** unlike modbus tcp server, client based on BSD sockets
  */
/**
 * @brief prepare_server_address
 * @param address
 * @param port
 * @param sockaddr
 * @return
 */
static u8 prepare_server_address(const u8 * address, int port, struct sockaddr_in* sockaddr);
/**
 * @brief get_ip_slaves_by_number
 * @param slave_table_item functuion will it fill if exist
 * @param number should be less then slaves_number
 * @return 1 if ip address was coppied to ip_address
 */
static int get_slaves_info_by_number(slave_table_item_t * slave_table_item, u16 number);
/**
 * @brief get_ip_address_by_modbus
 * @param slave_table_item
 * @param modbus_id - [0;250]
 * @return
 */
static int get_ip_address_by_modbus(slave_table_item_t * slave_table_item, u8 modbus_id);
static void activate_keep_alive(int sd);
static int handleset_wait_ready(unsigned int timeout_ms,int socket,file_desc_set_t  file_desc_set);
static int close_socket_connection(int * socket);
static inline int ip_addresses_were_changed(u8 * server_ip_arg,u8 * own_ip_arg,u8 modbus_id);
static FNCT_NO_RETURN void modbus_tcp_client_connection_task(void * argument);
/**
 * @brief connection_proccess
 *             if we have server ip address try connect to him
 *  bsd sockets steps
 *  addr_info
 *  socket
 *  connect
 *  send recv
 *  close
 *
 *
 * @return less then 0 if error occured
 */
static inline int connection_proccess(u8 dest_ip[4],int * socket_id,struct sockaddr_in * server_address,file_desc_set_t file_desc_set);
static inline int connect_to_address(u8 dest_ip[4],struct sockaddr_in * server_address,file_desc_set_t file_desc_set);
task_handle_t modbus_master_id = NULL;
static semaphore_handle_t modbus_tcp_client_access_mutex;
static u16 number_separate_client_spaces = 0;
static int modbus_master_init(void){
    int res =0;
    u16 space_number_prev = 0;
    semaphore_create_binary(modbus_tcp_client_access_mutex);
    number_separate_client_spaces = 0;
    char name[20] = "mdb_client_000";
    for (int i =0;(i < NUM_OF_CLIENT_VARS) && (number_separate_client_spaces < MAX_NUMBER_OF_SLAVES_CONNECTIONS);i++){
        if((regs_description_client[i].space_number != space_number_prev)||
            (i==(NUM_OF_CLIENT_VARS-1)) || (i==0)){
            int modbus_first = -1;
            int modbus_last = -1;
            int last_item_size = -1;
            if(i==0){
                modbus_tcp_client_slave_connections[0].first_regs_description = &regs_description_client[i];
            }else{
                if (regs_description_client[i].space_number != space_number_prev){
                    modbus_tcp_client_slave_connections[number_separate_client_spaces+1].first_regs_description = &regs_description_client[i];
                } 
                if(i==(NUM_OF_CLIENT_VARS-1)){
                    modbus_last = RD_MDB_ADDRESS(regs_description_client[i].modbus_description);
                    last_item_size = (regs_description_client[i].size*regs_size_in_byte(regs_description_client[i].type))/2;
                }else {
                    modbus_last = RD_MDB_ADDRESS(regs_description_client[i-1].modbus_description);
                    last_item_size = (regs_description_client[i-1].size*regs_size_in_byte(regs_description_client[i-1].type))/2;
                }
                //goes after an increment
                modbus_first = RD_MDB_ADDRESS(modbus_tcp_client_slave_connections[number_separate_client_spaces].first_regs_description->modbus_description);
                if ((modbus_first >= 0) && (modbus_last >= 0) &&  
                    (modbus_first <= modbus_last) && (modbus_last< 65535) && ((last_item_size+(modbus_last - modbus_first)) <= 65535) ){
                    modbus_tcp_client_slave_connections[number_separate_client_spaces].size_in_words = (modbus_last - modbus_first) + last_item_size;
                    name[11] = '0';
                    name[12] = '0';
                    name[13] = '0';
                    sprintf(name,"mdb_client_%i",number_separate_client_spaces);
                    res = task_create(modbus_tcp_client_connection_task, name, 4096, &modbus_tcp_client_slave_connections[number_separate_client_spaces], (tskIDLE_PRIORITY + 2), &modbus_tcp_client_slave_connections[number_separate_client_spaces].task_id);
                    if(res != pdTRUE){
                        main_error_message(TAG,"Failed %s:%d\n",__FILE__,__LINE__);
                    }
                    main_debug(TAG,"create task %s",name);
                }else{
                    main_error_message(TAG,"Failed %s:%d\n %u %u %u %lu",__FILE__,__LINE__,modbus_first,modbus_last,last_item_size,modbus_tcp_client_slave_connections[number_separate_client_spaces-1].size_in_words);
                }
                number_separate_client_spaces++;
            }
        }
        space_number_prev = regs_description_client[i].space_number;
    }
    return res;
}
static int modbus_master_deinit(void){
    int res =0;
    number_separate_client_spaces = 0;
    semaphore_delete(modbus_tcp_client_access_mutex);
    return res;
}

static uc8 zero_array[4] = {0,0,0,0};
static u32 lap_time = 0;
/**
 * @brief
 * @param argument unused
 */
FNCT_NO_RETURN void modbus_tcp_client_common_task( void  * argument ){
    uint32_t signal_value;
    ( void ) argument ;//unused because used only once at init proccess
    modbus_master_init();/*allocated memmory and clear state*/
    while(1){
        signal_value = 0;
        if(task_notify_wait(STOP_CHILD_PROCCES|PREPARE_TO_RESET, &signal_value, 1000)!=pdTRUE){
            main_debug(TAG,"modbus tcp client task,table size - %u,clients %u\n %u",get_ip_slaves_table_size(),
            number_separate_client_spaces,(uint32_t)&regs_description_client[0]);
        }else{
            if(signal_value & STOP_CHILD_PROCCES){
                /*todo add break consequence*/
                modbus_master_deinit();
            }
            if(signal_value & PREPARE_TO_RESET){
                /*todo add break consequence*/
            }
        }
    }
}
/**
 * @brief
 * @param argument unused
 */
FNCT_NO_RETURN void modbus_tcp_client_connection_task( void  * argument ){
    uint32_t signal_value;
    uint32_t counter = 0;
    int client_socket_fd =-1;
    modbus_tcp_client_slave_connections_t * slave_connection;
    u16 master_slave_state = 0;
    u8 server_ip[4] = {0};
    struct sockaddr_in server_address;
    u8 own_ip[4] = {0};
    int errors_in_the_row=0;
    int success_packet_transaction_number=0;
    file_desc_set_t file_desc_set;
    client_socket_fd =-1;
    master_slave_state = 0;
    slave_connection = argument;
    memset(own_ip,0,4);
    memset(server_ip,0,4);
    memset(&server_address,0,sizeof(struct sockaddr_in));
    while(1){
        int event_is_signal = task_notify_wait(MODBUS_MASTER_CLOSE_CONNECTION_SIGNAL|STOP_CHILD_PROCCES,&signal_value,40);
        if (slave_connection->first_regs_description==NULL){
            close_socket_connection(&client_socket_fd);
            slave_connection->task_id = NULL;
            task_delete(task_get_id());
        }
        if (ip_addresses_were_changed(server_ip, own_ip,RD_MDB_CLIENT_MDB_ADDRESS(slave_connection->first_regs_description->space_number))>0){
            close_socket_connection(&client_socket_fd);
        }
        if (memcmp(zero_array,server_ip,sizeof(server_ip))!=0){
            if (client_socket_fd<0){
                main_debug(TAG,"TCP_CLIENT trying to connnect to - %u.%u.%u.%u\n",server_ip[0],server_ip[1],server_ip[2],server_ip[3]);
                connection_proccess(server_ip,&client_socket_fd,&server_address,file_desc_set);
            }
        }else{
            if (client_socket_fd>=0){
                close_socket_connection(&client_socket_fd);
            }
        }
        if(event_is_signal!=pdTRUE){
            if (client_socket_fd>=0){
                const regs_description_t * regs_description_temp = slave_connection->first_regs_description;
                u16 space_number_prev ;
                //u16 reg_index ;
                client_request_t client_requests;
                space_number_prev = regs_description_temp->space_number;
                //reg_index = regs_description_temp->ind;
                client_requests.address = RD_MDB_ADDRESS(regs_description_temp->modbus_description);
                client_requests.slave_id = RD_MDB_CLIENT_MDB_ADDRESS(regs_description_temp->space_number);
                client_requests.channel = PACKET_CHANNEL_TCP;
                client_requests.retries = 2;
                client_requests.req_type = req_input;
                client_requests.error_code = 0;
                client_requests.prev_error = 0;
                client_requests.coms_buffer = (u16*)regs_description_temp->p_value;
                client_requests.mb_function = RD_MDB_FUNCTION(regs_description_temp->modbus_description);
                client_requests.resp_timeout = 1000;
                client_requests.plcv_buffer = NULL;
                client_requests.count = slave_connection->size_in_words;
                int res = modbus_master_execute_request(&client_requests,client_socket_fd,file_desc_set);
                if (res<0){
                    errors_in_the_row++;
                    slave_connection->failed_requests++;
                }else{
                    errors_in_the_row=0;
                    success_packet_transaction_number++;
                    slave_connection->success_requests++;
                }
            }
            counter++;
            if(counter%10==0){
                const regs_description_t * regs_description_temp = slave_connection->first_regs_description;
                main_debug(TAG,"TCP_CLIENT 0x%08x server_ip - %u.%u.%u.%u \n modbus_address - %lu %i %i \n address in memmory 0x%08x size in words %lu",\
                    regs_description_temp->space_number,server_ip[0],server_ip[1],server_ip[2],server_ip[3],\
                    RD_MDB_ADDRESS(regs_description_temp->modbus_description),errors_in_the_row,success_packet_transaction_number,\
                    (uint32_t)slave_connection->first_regs_description,slave_connection->size_in_words);
            }
        }
        if(errors_in_the_row>10){
            errors_in_the_row=0;
            if (client_socket_fd>=0){
                close_socket_connection(&client_socket_fd);
            }
            /*slave_connection->task_id = NULL;
            task_delete(task_get_id());*/
        }
        if((event_is_signal==pdTRUE)&&(signal_value & MODBUS_MASTER_CLOSE_CONNECTION_SIGNAL)){
            memcpy(server_ip,zero_array,sizeof(in_addr_t));
            close_socket_connection(&client_socket_fd);
            slave_connection->task_id = NULL;
            task_delete(task_get_id());
        }
        if((event_is_signal==pdTRUE)&&(signal_value & STOP_CHILD_PROCCES)){
            close_socket_connection(&client_socket_fd);
            slave_connection->task_id = NULL;
            task_delete(task_get_id());
        }
    }
}

static int handleset_wait_ready(unsigned int timeout_ms,int socket,file_desc_set_t  file_desc_set){
   int result;
   struct timeval timeout;
   timeout.tv_sec = timeout_ms / 1000;
   timeout.tv_usec = (timeout_ms % 1000) * 1000;
   FD_ZERO(&file_desc_set.read_set);
   FD_SET(socket, &file_desc_set.read_set);
   result = select(socket + 1, &file_desc_set.read_set, NULL, NULL, &timeout);
   return result;
}

static void activate_keep_alive(int sd){
#if defined SO_KEEPALIVE
    int optval;
    socklen_t optlen = sizeof(optval);
    optval = CONFIG_TCP_KEEPALIVE_IDLE;
    setsockopt(sd, SOL_SOCKET, SO_KEEPALIVE, &optval, optlen);
    optval = 1;
    //commented beacose we use MSG_NOSIGNAL in send function
    //setsockopt(sd, SOL_SOCKET, SO_NOSIGPIPE, &optval, optlen);
#if defined TCP_KEEPCNT
    optval = CONFIG_TCP_KEEPALIVE_IDLE;
    setsockopt(sd, IPPROTO_TCP, TCP_KEEPIDLE, &optval, optlen);
    optval = CONFIG_TCP_KEEPALIVE_INTERVAL;
    setsockopt(sd, IPPROTO_TCP, TCP_KEEPINTVL, &optval, optlen);
    optval = CONFIG_TCP_KEEPALIVE_CNT;
    setsockopt(sd, IPPROTO_TCP, TCP_KEEPCNT, &optval, optlen);
#endif /* TCP_KEEPCNT */
#endif /* SO_KEEPALIVE */
}

static u8 prepare_server_address(const u8 * address, int port, struct sockaddr_in* sockaddr){
    memset((char *) sockaddr , 0, sizeof(struct sockaddr_in));
    static ip_addr_t s_hostent_addr;
    static  ip_addr_t *s_phostent_addr[2];
    memcpy(&s_hostent_addr,address,sizeof(in_addr_t));
    s_phostent_addr[0] = &s_hostent_addr;
    s_phostent_addr[1] = NULL;
    sockaddr->sin_family = AF_INET;
    sockaddr->sin_port = htons((u16_t)port);
    sockaddr->sin_len = sizeof(struct sockaddr_in);
    memcpy((char *) &sockaddr->sin_addr.s_addr, (char *) s_phostent_addr[0], 4);
    return 1;
}



/* Execute a Query/Response suspended transaction between client and server */
/* returns: <0    -> ERROR: error codes
 *          >2    -> SUCCESS: frame length
 *
 */
int modbus_tcp_master_packet_transaction(u16 channel,u8  *packet,u16 send_length,
                              u16 recv_length,u32 timeout,int socket_id,file_desc_set_t file_desc_set) {
    (void)channel;
    int res =0;
    /**/
    struct timeval tv;
    u8 id[2];
    id[0] = packet[0];
    id[1] = packet[1];
    FD_ZERO(&file_desc_set.write_set);
    FD_SET(socket_id, &file_desc_set.write_set);
    tv.tv_sec =0;
    tv.tv_usec=(long)timeout*1000;
    if(select(socket_id+1, NULL, &file_desc_set.write_set, NULL, &tv)>0){
        res = send(socket_id, packet,send_length,0);
        if (res == send_length){
            u8 id_mismatch = 0;
            do{
                id_mismatch = 0;
                if(handleset_wait_ready(timeout,socket_id,file_desc_set)==1){
                    res = recv(socket_id, packet, (size_t)MODBUS_PACKET_LEN, MSG_DONTWAIT);
                    if(res>0){
                        if (id[0]!=packet[0] || id[1]!=packet[1]){
                            id_mismatch =1;
                        }
                    }
                }else{
                    res =-1;
                }
            }while(id_mismatch && (res>0));
        }else{
            res =-1;
        }
    }else{
        res =-1;
    }
    if (socket_id>=0){
        FD_ZERO(&file_desc_set.error_set);
        FD_SET(socket_id, &file_desc_set.error_set);
        tv.tv_sec =0;
        tv.tv_usec=0;
        if(select(socket_id+1, NULL, NULL, &file_desc_set.error_set, &tv)>0){
            res =-1;
        }
    }
    return res;
}
static int close_socket_connection(int * socket){
    if (*socket>=0){
        shutdown(*socket, SHUT_RDWR);
        close(*socket);
        main_printf(TAG,"socket - %i,closed",*socket);
        *socket =-1;
    }
    return 0;
}

#define SLAVE_TABLE_ITEM_SIZE (sizeof(slave_table_item_t))
#define SLAVE_TABLE_SIZE (SLAVE_TABLE_ITEM_SIZE*MAX_NUMBER_OF_SLAVES)
static slave_table_item_t slaves_ip_table[MAX_NUMBER_OF_SLAVES] = {{0,0}};
static u16 slaves_number = 0;
int clean_slave_table(void){
    semaphore_take(modbus_tcp_client_access_mutex, portMAX_DELAY );{
        memset(slaves_ip_table,0,SLAVE_TABLE_SIZE);
        slaves_number = 0;
    }semaphore_release(modbus_tcp_client_access_mutex);
    return 0;
}
int add_ip_to_slave_table(uc8 ip_addr[4], u8 modbus_id){
    u8 exist = 0;
    int res = 0;
    semaphore_take(modbus_tcp_client_access_mutex, portMAX_DELAY );{
        for(u32 i=0;i<slaves_number;i++){
            if (memcmp(ip_addr,&slaves_ip_table[i].ip,sizeof(in_addr_t))==0){
                exist =1;
            }
        }
    }semaphore_release(modbus_tcp_client_access_mutex);
    if (!exist){
        main_debug(TAG,"add ip to slave table modbus_id - %i,ip - %i.%i.%i.%i",modbus_id,ip_addr[0],ip_addr[1],ip_addr[2],ip_addr[3]);
        if (slaves_number<MAX_NUMBER_OF_SLAVES){
            semaphore_take(modbus_tcp_client_access_mutex, portMAX_DELAY );{
                memcpy(&slaves_ip_table[slaves_number].ip,ip_addr,sizeof(in_addr_t));
                memcpy(&slaves_ip_table[slaves_number].modbus_id,&modbus_id,1);
                slaves_number++;
            }semaphore_release(modbus_tcp_client_access_mutex);
            res = 1;
        }else{
            res = -1;
        }
    }
    return res;
}
u16 get_ip_slaves_table_size(void){
    u16 size=0;
    semaphore_take(modbus_tcp_client_access_mutex, portMAX_DELAY );{
        size = slaves_number;
    }semaphore_release(modbus_tcp_client_access_mutex);
    return size;
}
/**
 * @brief get_ip_slaves_by_number
 * @param slave_table_item functuion will it fill if exist
 * @param number should be less then slaves_number
 * @return 1 if ip address was coppied to ip_address
 */
static int get_slaves_info_by_number(slave_table_item_t * slave_table_item, u16 number){
    int res = 0;
    semaphore_take(modbus_tcp_client_access_mutex, portMAX_DELAY );{
        if (number < slaves_number){
            res = 1;
            memcpy(slave_table_item,&slaves_ip_table[number],sizeof(slave_table_item_t));
        }else{
            res =-1;
        }
    }semaphore_release(modbus_tcp_client_access_mutex);
    return res;
}
/**
 * @brief get_ip_address_by_modbus
 * @param slave_table_item
 * @param modbus_id - [0;250]
 * @return
 */
static int get_ip_address_by_modbus(slave_table_item_t * slave_table_item, u8 modbus_id){
    int result = 0;
    u16 table_size = get_ip_slaves_table_size();
    for(u16 i=0;i<table_size;i++){
        if (slaves_ip_table[i].modbus_id == modbus_id){
            result = 1;
            memcpy(slave_table_item,&slaves_ip_table[i],sizeof(slave_table_item_t));
        }
    }
    return result;
}

static inline int ip_addresses_were_changed(u8 * server_ip_arg,u8 * own_ip_arg,u8 modbus_id){
    int res =0;
    /*take server ip address and check*/
    slave_table_item_t slave_table_item;
    int result = get_ip_address_by_modbus(&slave_table_item,modbus_id);
    if(result>0){
        if(memcmp(&slave_table_item.ip,server_ip_arg,sizeof(in_addr_t))!=0){
            memcpy(server_ip_arg,&slave_table_item.ip,sizeof(in_addr_t));
            res =1;
        }
    }
    /*check own ip address*/
    semaphore_take(regs_access_mutex, portMAX_DELAY );{
        if(memcmp(regs_global.vars.ip,own_ip_arg,sizeof(in_addr_t))!=0){
            /*own ip was changed*/
            memcpy(own_ip_arg,regs_global.vars.ip,sizeof(in_addr_t));
            res =1;
        }
    }semaphore_release(regs_access_mutex);
    return res;
}
/**
 * @brief connection_proccess
 *             if we have server ip address try connect to him
 *  bsd sockets steps
 *  addr_info
 *  socket
 *  connect
 *  send recv
 *  close
 *
 *
 * @return less then 0 if error occured
 */
static inline int connect_to_address(u8 dest_ip[4],struct sockaddr_in * server_address,file_desc_set_t file_desc_set){
    int socket_id=-1;
    if (!prepare_server_address(dest_ip, 502, server_address)){
        main_printf(TAG,"error prepare server address");
    }else{
        socket_id = socket(AF_INET, SOCK_STREAM, 0);
        if (socket_id !=-1){
    #if CONFIG_ACTIVATE_TCP_KEEPALIVE == 1
            activate_keep_alive(socket_id);
    #endif
            main_printf(TAG,"Socket connect: %u.%u.%u.%u:%i\n", dest_ip[0],dest_ip[1],dest_ip[2],dest_ip[3], 502);
            fcntl(socket_id, F_SETFL, O_NONBLOCK);
            int res;
            if (connect(socket_id, (struct sockaddr *) server_address, sizeof(struct sockaddr_in)) < 0) {
                int err = errno;
                if(err == EINPROGRESS){
                    struct timeval tv;
                    tv.tv_sec = 0;
                    tv.tv_usec = 2000*1000;
                    /* select without waiting should fail */
                    u32 timer;
                    timer = task_get_tick_count();
                    FD_ZERO(&file_desc_set.write_set);
                    FD_SET(socket_id, &file_desc_set.write_set);
                    FD_ZERO(&file_desc_set.read_set);
                    FD_SET(socket_id, &file_desc_set.read_set);
                    FD_ZERO(&file_desc_set.error_set);
                    FD_SET(socket_id, &file_desc_set.error_set);
                    res = select(socket_id+1, &file_desc_set.read_set, &file_desc_set.write_set, &file_desc_set.error_set, &tv);
                    main_printf(TAG,"select time - %lu",task_get_tick_count()-timer);
                    if (res<=0){
                        main_error_message(TAG,"connection time out \n");
                        shutdown(socket_id, SHUT_RDWR);
                        close(socket_id);
                        main_printf(TAG,"socket - %i,closed",socket_id);
                        socket_id =-1;
                    }else{
                        main_printf(TAG,"connected to server");
                    }
                }else{
                    main_printf(TAG,"connection error");
                    shutdown(socket_id, SHUT_RDWR);
                    close(socket_id);
                    main_printf(TAG,"socket - %i,closed",socket_id);
                    socket_id =-1;
                }
            }else{
                main_printf(TAG,"connected to server without select");
            }
        }else{
            main_error_message(TAG,"socket create error");
        }
    }
    return socket_id;
}
/**
 * @brief connection_proccess
 *             if we have server ip address try connect to him
 *  bsd sockets steps
 *  addr_info
 *  socket
 *  connect
 *  send recv
 *  close
 *
 *
 * @return less then 0 if error occured
 */
static inline int connection_proccess(u8 dest_ip[4],int * socket_id,struct sockaddr_in * server_address,file_desc_set_t file_desc_set){
    static u8 connection_error_in_the_row = 0;
    if (!prepare_server_address(dest_ip, 502, server_address)){
        main_printf(TAG,"error prepare server address");
    }
    *socket_id = socket(AF_INET, SOCK_STREAM, 0);
    if (*socket_id!=-1){
#if CONFIG_ACTIVATE_TCP_KEEPALIVE == 1
        activate_keep_alive(*socket_id);
#endif
        main_printf(TAG,"Socket connect: %u.%u.%u.%u:%i %i\n", dest_ip[0],dest_ip[1],dest_ip[2],dest_ip[3], 502,*socket_id);
        FD_ZERO(&file_desc_set.write_set);
        FD_SET(*socket_id, &file_desc_set.write_set);
        fcntl(*socket_id, F_SETFL, O_NONBLOCK);
        if (connect(*socket_id, (struct sockaddr *) server_address, sizeof(struct sockaddr_in)) < 0) {
            int err = errno;
            if(err == EINPROGRESS){
                struct timeval tv;
                tv.tv_sec = 0;
                tv.tv_usec = 2000*1000;
                /* select without waiting should fail */
                u32 timer;
                timer = task_get_tick_count();
                FD_ZERO(&file_desc_set.read_set);
                FD_SET(*socket_id, &file_desc_set.read_set);
                FD_ZERO(&file_desc_set.error_set);
                FD_SET(*socket_id, &file_desc_set.error_set);
                int select_res = select(*socket_id+1, &file_desc_set.read_set, &file_desc_set.write_set, &file_desc_set.error_set, &tv);
                main_printf(TAG,"select time - %lu",task_get_tick_count()-timer);
                if (select_res<=0){
                    main_error_message(TAG,"connection time out \n");
                    close_socket_connection(socket_id);
                }else{
                    connection_error_in_the_row=0;
                    main_printf(TAG,"connected to server");
                }
            }else{
                main_printf(TAG,"connection error");
                if ((connection_error_in_the_row++)>5){
                    connection_error_in_the_row=0;
                    semaphore_take(regs_access_mutex, portMAX_DELAY );{
                        memset(regs_global.vars.iirls_paired_ip,0,sizeof(in_addr_t));
                        regs_global.vars.iirls_pairing_status = IIRLS_NEVER_PAIRED;
                    }semaphore_release(regs_access_mutex);
                    lap_time = task_get_tick_count();
                    memset(dest_ip,0,sizeof(in_addr_t));
                }
                close_socket_connection(socket_id);
            }
        }else{
            connection_error_in_the_row=0;
            main_printf(TAG,"connected to server without select");
        }
    }else{
        main_error_message(TAG,"socket create error");
    }
    return *socket_id;
}
#endif //MODBUS_TCP_CLIENT_C
