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
static const char *TAG = "modbus_tcp_client";
/**unlike modbus tcp server, client based on BSD sockets
  */


/**
 * @brief prepare_server_address
 * @param address
 * @param port
 * @param sockaddr
 * @return
 */

static u8 prepare_server_address(const u8 * address, int port, struct sockaddr_in* sockaddr);
static void activate_keep_alive(int sd);
static int handleset_wait_ready(unsigned int timeout_ms,int socket);
static int close_socket_connection(int * socket);
static inline int handle_requests_signal(int signal,int * socket);
static inline int handle_ip_address_changes(u8 * server_ip_arg,u8 * own_ip_arg);

static int calculate_average_from_prefetch_buffer(void);
static float calculate_sq_from_prefetch_buffer(int average);
static u8 get_slave_address(void);
static int handshake_procedure(u16 master_slave_state);
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
static inline int connection_proccess(u8 dest_ip[4],int * socket_id);
static inline int connect_to_address(u8 dest_ip[4]);
task_handle_t modbus_master_id = NULL;
static semaphore_handle_t modbus_tcp_client_access_mutex;
static int modbus_master_init(void){
    int res =0;
    semaphore_create_binary(modbus_tcp_client_access_mutex)
    return res;
}
static int modbus_master_deinit(void){
    int res =0;

    return res;
}
static u8 prefetch_adc_state = 0;
static u16 prefetch_adc_buffer[100];


static fd_set write_set;
static fd_set read_set;
static fd_set error_set;
static int errors_in_the_row=0;
static uc8 zero_array[4] = {0,0,0,0};
static struct sockaddr_in server_address;

static u32 lap_time = 0;

/**
 * @brief
 * @param argument unused
 */
FNCT_NO_RETURN void modbus_tcp_client_task( void  * argument ){
    uint32_t signal_value;
    int client_socket_fd =-1;
    ( void ) argument ;//unused because used only once at init proccess
    modbus_master_init();/*allocated memmory and clear state*/
    u16 master_slave_state;
    u8 server_ip[4] = {0};
    u8 own_ip[4] = {0};

    if(task_create(isimfw400_task, "touch_task", 2048, NULL, (tskIDLE_PRIORITY + 2), &isimfw400_task_handle) != pdTRUE){
        main_printf(TAG,"modbus tcp task inited success\n");
    }

    while(1){
        int event_is_signal = task_notify_wait(0xffffffff,&signal_value,500);

        if (handle_ip_address_changes(server_ip, own_ip)>0){
            close_socket_connection(&client_socket_fd);
        }
        if (memcmp(zero_array,server_ip,sizeof(server_ip))!=0){
            if (client_socket_fd<0){
                connection_proccess(server_ip,&client_socket_fd);
            }
        }else{
            if (client_socket_fd>=0){
                close_socket_connection(&client_socket_fd);
            }
            semaphore_take(regs_access_mutex, portMAX_DELAY );{
                master_slave_state = regs_global.vars.iirls_pairing_status;
            }semaphore_release(regs_access_mutex);
            handshake_procedure(master_slave_state);

        }
        if(event_is_signal!=pdTRUE){
            if (client_socket_fd>=0){
                int res = modbus_master_execute_request(&client_requests[TEST_REQUEST_TYPE],client_socket_fd);
                if (res>=0){
                    semaphore_take(regs_access_mutex, portMAX_DELAY );{
                    if (res>0){
                        regs_global.vars.modbus_master_test_value= client_requests[TEST_REQUEST_TYPE].coms_buffer[0];
                    }
                    errors_in_the_row=0;
                    }semaphore_release(regs_access_mutex);
                }else{
                    close_socket_connection(&client_socket_fd);
                    errors_in_the_row++;
                }
            }
        }else{
            if (client_socket_fd>=0){
                handle_requests_signal((int)signal_value,&client_socket_fd);
            }else{
                ui32 prev_value = 0;
                task_notify_send(isimfw400_task_handle, MODBUS_MASTER_COMMAND_COMPLETE_ERROR,&prev_value);
            }
        }
        if(errors_in_the_row>10){
            errors_in_the_row=0;
            if (client_socket_fd>=0){
                close_socket_connection(&client_socket_fd);
            }
        }
        if((event_is_signal==pdTRUE)&&(signal_value & MODBUS_MASTER_CLOSE_CONNECTION_SIGNAL)){
            memcpy(server_ip,zero_array,sizeof(in_addr_t));
            if (client_socket_fd>=0){
                close_socket_connection(&client_socket_fd);
            }
            task_delay_ms(250);
        }
        if((event_is_signal==pdTRUE)&&(signal_value & STOP_CHILD_PROCCES)){
            if (client_socket_fd>=0){
                close_socket_connection(&client_socket_fd);
            }
            modbus_master_deinit();
            task_delete(task_get_id());
        }
    }
}
static int handshake_procedure(u16 master_slave_state){
    int res = 0;
    switch(master_slave_state){
    case(IIRLS_NEVER_PAIRED):
            semaphore_take(regs_access_mutex, portMAX_DELAY );{
                regs_global.vars.iirls_pairing_status = IIRLS_SEARCHING;
            }semaphore_release(regs_access_mutex);
            clean_slave_table();
            start_prefetch_adc_buffer();
            lap_time = task_get_tick_count();
    break;
    case(IIRLS_PAIRED):
        /*do nothing becouse we never be here theoretecly*/
        main_error_message(TAG,"master slave state machine error state");
        semaphore_take(regs_access_mutex, portMAX_DELAY);{
            if (memcmp(regs_global.vars.iirls_paired_ip,zero_array,4)==0){
                regs_global.vars.iirls_pairing_status = IIRLS_NEVER_PAIRED;
                lap_time = task_get_tick_count();
            }
        }semaphore_release(regs_access_mutex);
    break;
    case(IIRLS_SEARCHING):
        if ((task_get_tick_count() - lap_time) > SEARCHING_TIME_MS ){
            if (get_ip_slaves_table_size()){
                semaphore_take(regs_access_mutex, portMAX_DELAY );{
                    regs_global.vars.iirls_pairing_status = IIRLS_PAIRING;
                }semaphore_release(regs_access_mutex);
            }else{
                semaphore_take(regs_access_mutex, portMAX_DELAY );{
                    regs_global.vars.iirls_pairing_status = IIRLS_NO_DEVICES_FOUND;
                }semaphore_release(regs_access_mutex);
            }
            lap_time = task_get_tick_count();
        }else{
            main_printf(TAG,"send advrtsmnt");
            udp_broadcast_advertisement();
        }
    break;
    case(IIRLS_PAIRING):{
        u16 slaves = get_ip_slaves_table_size();
        int average_dark = calculate_average_from_prefetch_buffer();
        float sq_dark = calculate_sq_from_prefetch_buffer(average_dark);
        u8 paired=0;
        client_requests[MODBUS_MASTER_COMMAND].slave_id = get_slave_address();
        for(u32 i=0;i<slaves;i++){
            client_requests[MODBUS_MASTER_COMMAND].coms_buffer[0] = IIRLS_HANDSHAKE;
            u8 itt=5;
            u8 slave_ip_address_current[4];
            int socket_id = -1;
            do{
                if(get_ip_slaves_by_number(slave_ip_address_current,(u16)i)>=0){
                    socket_id = connect_to_address(slave_ip_address_current);
                    itt--;
                }else{
                    itt = 0;
                }
            }while(socket_id<0 && itt);
            if (socket_id>=0){
                main_printf(TAG,"pairing - sending request");
                int res = modbus_master_execute_request(&client_requests[MODBUS_MASTER_COMMAND],socket_id);
                if (res > 0){
                    task_delay_ms(200);
                    for (u32 j=0;j<10;j++){
                        start_prefetch_adc_buffer();
                        int average_current = calculate_average_from_prefetch_buffer();
                        if (((average_current - average_dark) * (average_current - average_dark)) > (100.0f * sq_dark)){
                            semaphore_take(regs_access_mutex, portMAX_DELAY);{
                                memcpy(regs_global.vars.iirls_paired_ip,slave_ip_address_current,4);
                            }semaphore_release(regs_access_mutex);
                            paired=1;
                            break;
                        }
#if DEBUG_SOCKET
                        semaphore_take(regs_access_mutex, portMAX_DELAY);{
                            memcpy(regs_global.vars.iirls_paired_ip,slave_ip_address_current,4);
                        }semaphore_release(regs_access_mutex);
                        paired =1;
                        task_delay_ms(2000);
                        break;
#endif
                        task_delay_ms(200);
                    }
                    if (paired){
                        client_requests[MODBUS_MASTER_COMMAND].coms_buffer[0] = IIRLS_HANDSHAKE_CONFIRMATION;
                        itt = 5;
                        do{
                            res = modbus_master_execute_request(&client_requests[MODBUS_MASTER_COMMAND],socket_id);
                        }while(res<0 && itt--);
                    }
                }
                close_socket_connection(&socket_id);
                if (paired){
                    break;
                }
            }
        }
        if (paired){
            regs_global.vars.iirls_pairing_status = IIRLS_PAIRED;
            lap_time = task_get_tick_count();
        }else{
            regs_global.vars.iirls_pairing_status = IIRLS_FAILED_TO_PAIR;
            lap_time = task_get_tick_count();
        }
    }break;
    case(IIRLS_NO_DEVICES_FOUND):
        if ((task_get_tick_count() - lap_time) > SEARCHING_TIME_MS*5){
            semaphore_take(regs_access_mutex, portMAX_DELAY );{
                regs_global.vars.iirls_pairing_status = IIRLS_NEVER_PAIRED;
            }semaphore_release(regs_access_mutex);
            lap_time = task_get_tick_count();
        }
    break;
    case(IIRLS_FAILED_TO_PAIR):
        if ((task_get_tick_count() - lap_time) > SEARCHING_TIME_MS*5){
            semaphore_take(regs_access_mutex, portMAX_DELAY );{
                regs_global.vars.iirls_pairing_status = IIRLS_NEVER_PAIRED;
            }semaphore_release(regs_access_mutex);
            lap_time = task_get_tick_count();
        }
    break;
    default:
    break;
    }

    return res;
}
static inline int handle_requests_signal(int signal,int * client_socket_fd){
    int res =0;
    u8 temp_slave_modbus_address = get_slave_address();
    for(u32 i=0;i<NUMBER_OF_CLIENT_REQTS;i++){
        client_requests[i].slave_id = temp_slave_modbus_address;
    }
    ui32 prev_value = 0;
    if(signal & MODBUS_MASTER_SET_0_SIGNAL){
        /*write first sets of registers*/
        client_requests[MODBUS_MASTER_SET_0].coms_buffer[0] = 5;
        int res = modbus_master_execute_request(&client_requests[MODBUS_MASTER_SET_0],*client_socket_fd);
        if (res > 0){

            task_notify_send(isimfw400_task_handle, MODBUS_MASTER_COMMAND_COMPLETE_SUCCESSFULL,&prev_value);
            errors_in_the_row=0;
        }else if(res==0){
            task_notify_send(isimfw400_task_handle, MODBUS_MASTER_COMMAND_COMPLETE_ERROR,&prev_value);
            errors_in_the_row=0;
        }else{
            close_socket_connection(client_socket_fd);
            task_notify_send(isimfw400_task_handle, MODBUS_MASTER_COMMAND_COMPLETE_ERROR,&prev_value);
            errors_in_the_row++;
        }
    }else if(signal & MODBUS_MASTER_SET_1_SIGNAL){
        /*write seconde sets of registers*/
            client_requests[MODBUS_MASTER_SET_1].coms_buffer[0] = 1;
            int res = modbus_master_execute_request(&client_requests[MODBUS_MASTER_SET_1],*client_socket_fd);
            if (res > 0){
                task_notify_send(isimfw400_task_handle, MODBUS_MASTER_COMMAND_COMPLETE_SUCCESSFULL,&prev_value);
                errors_in_the_row=0;
            }else if(res==0){
                task_notify_send(isimfw400_task_handle, MODBUS_MASTER_COMMAND_COMPLETE_ERROR,&prev_value);
                errors_in_the_row=0;
            }else{
                close_socket_connection(client_socket_fd);
                task_notify_send(isimfw400_task_handle, MODBUS_MASTER_COMMAND_COMPLETE_ERROR,&prev_value);
                errors_in_the_row++;
            }
    }else if(signal & MODBUS_MASTER_READ_0_SIGNAL){
        /*write seconde sets of registers*/
        int res = modbus_master_execute_request(&client_requests[MODBUS_MASTER_READ_0],*client_socket_fd);
        if (res > 0){
            memcpy(slave_adc_buffer,client_requests[MODBUS_MASTER_READ_0].coms_buffer,IIRLSCALC_DATA_BUFFER_LENGTH*2);
            task_notify_send(isimfw400_task_handle, MODBUS_MASTER_COMMAND_COMPLETE_SUCCESSFULL,&prev_value);
            errors_in_the_row=0;
        }else if(res==0){
            task_notify_send(isimfw400_task_handle, MODBUS_MASTER_COMMAND_COMPLETE_ERROR,&prev_value);
            errors_in_the_row=0;
        }else{
            close_socket_connection(client_socket_fd);
            task_notify_send(isimfw400_task_handle, MODBUS_MASTER_COMMAND_COMPLETE_ERROR,&prev_value);
            errors_in_the_row++;
        }
    }else if(signal & MODBUS_MASTER_READ_1_SIGNAL){
        /*write seconde sets of registers*/
        int res = modbus_master_execute_request(&client_requests[MODBUS_MASTER_READ_1],*client_socket_fd);
        if (res > 0){
            semaphore_take(regs_access_mutex, portMAX_DELAY );{
                memcpy(regs_global.vars.paired_device_model_number,client_requests[MODBUS_MASTER_READ_1].coms_buffer,20*2);
            }semaphore_release(regs_access_mutex);
            task_notify_send(isimfw400_task_handle, MODBUS_MASTER_COMMAND_COMPLETE_SUCCESSFULL,&prev_value);
            errors_in_the_row=0;
        }else if(res==0){
            task_notify_send(isimfw400_task_handle, MODBUS_MASTER_COMMAND_COMPLETE_ERROR,&prev_value);
            errors_in_the_row=0;
        }else{
            close_socket_connection(client_socket_fd);
            task_notify_send(isimfw400_task_handle, MODBUS_MASTER_COMMAND_COMPLETE_ERROR,&prev_value);
            errors_in_the_row++;
        }
    }else if(signal & MODBUS_MASTER_SET_2_SIGNAL){
        /*write seconde sets of registers*/
        u16 slave_signal_buffer[8] = {1,2,3,4,5,6,7,8};
        memcpy(client_requests[MODBUS_MASTER_SET_2].coms_buffer,slave_signal_buffer,8*2);
        int res = modbus_master_execute_request(&client_requests[MODBUS_MASTER_SET_2],*client_socket_fd);
        if (res > 0){
            task_notify_send(isimfw400_task_handle, MODBUS_MASTER_COMMAND_COMPLETE_SUCCESSFULL,&prev_value);
            errors_in_the_row=0;
        }else if(res==0){
            task_notify_send(isimfw400_task_handle, MODBUS_MASTER_COMMAND_COMPLETE_ERROR,&prev_value);
            errors_in_the_row=0;
        }else{
            close_socket_connection(client_socket_fd);
            task_notify_send(isimfw400_task_handle, MODBUS_MASTER_COMMAND_COMPLETE_ERROR,&prev_value);
            errors_in_the_row++;
        }
    } else if(signal & MODBUS_MASTER_PERIODIC_CONFIRMATION_SIGNAL){
        /*write seconde sets of registers*/
        client_requests[MODBUS_MASTER_COMMAND].coms_buffer[0] = IIRLS_HANDSHAKE_CONFIRMATION;
        int res = modbus_master_execute_request(&client_requests[MODBUS_MASTER_COMMAND],*client_socket_fd);
        if (res > 0){
            task_notify_send(isimfw400_task_handle, MODBUS_MASTER_COMMAND_COMPLETE_SUCCESSFULL,&prev_value);
            errors_in_the_row=0;
        }else if(res==0){
            task_notify_send(isimfw400_task_handle, MODBUS_MASTER_COMMAND_COMPLETE_ERROR,&prev_value);
            errors_in_the_row=0;
        }else{
            close_socket_connection(client_socket_fd);
            task_notify_send(isimfw400_task_handle, MODBUS_MASTER_COMMAND_COMPLETE_ERROR,&prev_value);
            errors_in_the_row++;
        }
    }
    return res;
}

static int handleset_wait_ready(unsigned int timeout_ms,int socket){
   int result;
   struct timeval timeout;
   timeout.tv_sec = timeout_ms / 1000;
   timeout.tv_usec = (timeout_ms % 1000) * 1000;
   FD_ZERO(&read_set);
   FD_SET(socket, &read_set);
   result = select(socket + 1, &read_set, NULL, NULL, &timeout);

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
                              u16 recv_length,u32 timeout,int socket_id) {
    (void)channel;
    int res =0;
    /**/
    struct timeval tv;
    u8 id[2];
    id[0] = packet[0];
    id[1] = packet[1];
    FD_ZERO(&write_set);
    FD_SET(socket_id, &write_set);
    tv.tv_sec =0;
    tv.tv_usec=(long)timeout*1000;
    if(select(socket_id+1, NULL, &write_set, NULL, &tv)>0){
        res = send(socket_id, packet,send_length,0);
        if (res == send_length){
            u8 id_mismatch = 0;
            do{
                id_mismatch = 0;
                if(handleset_wait_ready(timeout,socket_id)==1){
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
        FD_ZERO(&error_set);
        FD_SET(socket_id, &error_set);
        tv.tv_sec =0;
        tv.tv_usec=0;
        if(select(socket_id+1, NULL, NULL, &error_set, &tv)>0){
            res =-1;
        }
    }
    return res;
}
static int close_socket_connection(int * socket){
    if (*socket!=-1){
        shutdown(*socket, SHUT_RDWR);
        close(*socket);
        main_printf(TAG,"socket - %i,closed",*socket);
        *socket =-1;
    }
    return 0;
}
#define MAX_NUMBER_OF_SLAVES 255
typedef struct MCU_PACK{
    in_addr_t ip;
    u8 modbus_id;
}slave_table_item_t;

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
    int res =0;
    semaphore_take(modbus_tcp_client_access_mutex, portMAX_DELAY );{
        for(u32 i=0;i<slaves_number;i++){
            if (memcmp(ip_addr,&slaves_ip_table[i].ip,sizeof(in_addr_t))==0){
                exist =1;
            }
        }
    }semaphore_release(modbus_tcp_client_access_mutex);
    if (!exist){
        if (slaves_number<MAX_NUMBER_OF_SLAVES){
            semaphore_take(modbus_tcp_client_access_mutex, portMAX_DELAY );{
                memcpy(&slaves_ip_table[slaves_number].ip,ip_addr,sizeof(in_addr_t));
                memcpy(&slaves_ip_table[slaves_number].modbus_id,&modbus_id,1);
                slaves_number++;
            }semaphore_release(modbus_tcp_client_access_mutex);
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
int get_ip_slaves_by_number(u8 * ip_address,u16 number){
    int res = 0;
    semaphore_take(modbus_tcp_client_access_mutex, portMAX_DELAY );{
        if (number < slaves_number){
            res = 1;
            memcpy(ip_address,&slaves_ip_table[number*sizeof(in_addr_t)],sizeof(in_addr_t));
        }else{
            res =-1;
        }
    }semaphore_release(modbus_tcp_client_access_mutex);
    return res;
}
int start_prefetch_adc_buffer(){
    semaphore_take(modbus_tcp_client_access_mutex, portMAX_DELAY );{
        prefetch_adc_state = 1;
    }semaphore_release(modbus_tcp_client_access_mutex);
    return 0;
}
int end_prefetch_adc_buffer(){
    semaphore_take(modbus_tcp_client_access_mutex, portMAX_DELAY );{
        prefetch_adc_state = 0;
    }semaphore_release(modbus_tcp_client_access_mutex);
    return 0;
}

u8 is_prefetching_state(){
    u8 res = 0;
    semaphore_take(modbus_tcp_client_access_mutex, portMAX_DELAY );{
        if(prefetch_adc_state){
            res = 1;
        }else{
            res = 0;
        }
    }semaphore_release(modbus_tcp_client_access_mutex);
    return res;
}
int prefill_data_to_buff(u16 position,u16 data){
    int res = 0;
    if (position<IIRLSCALC_DATA_BUFFER_LENGTH){
        semaphore_take(modbus_tcp_client_access_mutex, portMAX_DELAY );{
            prefetch_adc_buffer[position]=data;
        }semaphore_release(modbus_tcp_client_access_mutex);
    }else{
        res = -1;
    }
    return res;
}
static int calculate_average_from_prefetch_buffer(){
    u32 summ=0;
    for(u32 i=0;i<IIRLSCALC_DATA_BUFFER_LENGTH;i++){
        summ += prefetch_adc_buffer[i];
    }
    return summ/IIRLSCALC_DATA_BUFFER_LENGTH;
}
static float calculate_sq_from_prefetch_buffer(int average){
    float std_sq = 0.0f;
    for(u32 i=0;i<IIRLSCALC_DATA_BUFFER_LENGTH;i++){
        std_sq += (average - prefetch_adc_buffer[i]) * (average - prefetch_adc_buffer[i]);
    }
    return std_sq/IIRLSCALC_DATA_BUFFER_LENGTH;
}

static inline int handle_ip_address_changes(u8 * server_ip_arg,u8 * own_ip_arg){
    int res =0;
    semaphore_take(regs_access_mutex, portMAX_DELAY );{
        if(memcmp(regs_global.vars.iirls_paired_ip,server_ip_arg,sizeof(in_addr_t))!=0){
            /*server ip was changed*/
            memcpy(server_ip_arg,regs_global.vars.iirls_paired_ip,sizeof(in_addr_t));
            res =1;
        }
    }semaphore_release(regs_access_mutex);
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
static inline int connect_to_address(u8 dest_ip[4]){
    int socket_id=-1;
    if (!prepare_server_address(dest_ip, 502, &server_address)){
        main_printf(TAG,"error prepare server address");
    }else{
        socket_id = socket(AF_INET, SOCK_STREAM, 0);
        if (socket_id !=-1){
    #if CONFIG_ACTIVATE_TCP_KEEPALIVE == 1
            activate_keep_alive(socket_id);
    #endif
            main_printf(TAG,"Socket connect: %u.%u.%u.%u:%i\n", dest_ip[0],dest_ip[1],dest_ip[2],dest_ip[3], 502);
            FD_ZERO(&write_set);
            FD_SET(socket_id, &write_set);
            fcntl(socket_id, F_SETFL, O_NONBLOCK);
            int res;
            if (connect(socket_id, (struct sockaddr *) &server_address, sizeof(server_address)) < 0) {
                int err = errno;
                if(err == EINPROGRESS){
                    struct timeval tv;
                    tv.tv_sec = 0;
                    tv.tv_usec = 2000*1000;
                    /* select without waiting should fail */
                    u32 timer;
                    timer = task_get_tick_count();
                    FD_ZERO(&read_set);
                    FD_SET(socket_id, &read_set);
                    FD_ZERO(&error_set);
                    FD_SET(socket_id, &error_set);
                    res = select(socket_id+1, NULL, &write_set, NULL, &tv);
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
static inline int connection_proccess(u8 dest_ip[4],int * socket_id){
    static u8 connection_error_in_the_row = 0;
    if (!prepare_server_address(dest_ip, 502, &server_address)){
        main_printf(TAG,"error prepare server address");
    }
    *socket_id = socket(AF_INET, SOCK_STREAM, 0);
    if (*socket_id!=-1){
#if CONFIG_ACTIVATE_TCP_KEEPALIVE == 1
        activate_keep_alive(*socket_id);
#endif
        main_printf(TAG,"Socket connect: %u.%u.%u.%u:%i\n", dest_ip[0],dest_ip[1],dest_ip[2],dest_ip[3], 502);
        FD_ZERO(&write_set);
        FD_SET(*socket_id, &write_set);
        fcntl(*socket_id, F_SETFL, O_NONBLOCK);
        if (connect(*socket_id, (struct sockaddr *) &server_address, sizeof(server_address)) < 0) {
            int err = errno;
            if(err == EINPROGRESS){
                struct timeval tv;
                tv.tv_sec = 0;
                tv.tv_usec = 2000*1000;
                /* select without waiting should fail */
                u32 timer;
                timer = task_get_tick_count();
                FD_ZERO(&read_set);
                FD_SET(*socket_id, &read_set);
                FD_ZERO(&error_set);
                FD_SET(*socket_id, &error_set);
                int select_res = select(*socket_id+1, NULL, &write_set, NULL, &tv);
                main_printf(TAG,"select time - %lu",task_get_tick_count()-timer);
                if (select_res<=0){
                    main_error_message(TAG,"connection time out \n");
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
static u8 get_slave_address(){
    u8 temp_slave_modbus_address;
    semaphore_take(regs_access_mutex, portMAX_DELAY );{
        temp_slave_modbus_address = (u8)regs_global.vars.slave_modbus_address;
        temp_slave_modbus_address = temp_slave_modbus_address>247?3:temp_slave_modbus_address;
    }semaphore_release(regs_access_mutex);
    return temp_slave_modbus_address;
}
#endif //MODBUS_TCP_CLIENT_C
