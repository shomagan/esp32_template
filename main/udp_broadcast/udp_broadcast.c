#ifndef UDP_BROADCAST_C
#define UDP_BROADCAST_C 1

#include "udp_broadcast.h"
#include "regs.h"
#include "lwip/opt.h"
#include "lwip/init.h"
#include "lwip/debug.h"
#include "lwip/stats.h"
#include "lwip/def.h"
#include <stdio.h>
#include <string.h>
#include "modbus_tcp_client.h"
#include "os_type.h"
/**
 * @brief udp_broadcast use for bradcast advertisment
 *
 */
#if LWIP_UDP
#if UDP_BROADCAST_ENABLE
#include "lwip/udp.h"
#include "lwip/timeouts.h"
#define ADVERTISMENT_REQUEST "UDP_REQUEST"
#define MODBUS_FIELD "modbus_address"
#define MODBUS_FIELD_SIZE (sizeof(MODBUS_FIELD)-1)
static const char *TAG = "udp_broadcast";
/**
answer example
{
    "modbus_address": 3,
    "name": "HVEL",
    "serial": "HVEL10000",
    "model": "HVEL1-0123456789ABCD",
    "firmware": "1.0.0-alpha.1",
    "ip": "192.168.1.100",
    "netmask": "255.255.255.0",
    "gateway": "192.168.1.1",
    "dhcp": false,
    "modbus_port": 502
}
*/

static struct udp_pcb * udp_broadcast_pcb;
static void udp_broadcast_server_recv(void *arg, struct udp_pcb *upcb,struct pbuf *p,\
                               const ip_addr_t *addr, u16_t port);
static u32 add_string_from_regs(u8 * dest, u8 * from,u32 max_size);
static int get_di_info(char * temp_buff);


int udp_broadcast_init(){
    int res = 0;
    /* LWIP_ASSERT_CORE_LOCKED(); is checked by udp_new() */
    udp_broadcast_pcb = udp_new_ip_type(IPADDR_TYPE_ANY);
    if (udp_broadcast_pcb == NULL) {
        return ERR_MEM;
    }
    ip_set_option(udp_broadcast_pcb, SOF_BROADCAST);
    res = udp_bind(udp_broadcast_pcb, IP_ANY_TYPE, UDP_BROADCAST_SELF_PORT);

    if (res != ERR_OK) {
        udp_remove(udp_broadcast_pcb);
        return res;
    }
    udp_recv(udp_broadcast_pcb, udp_broadcast_server_recv, NULL);
    return res;
}
int udp_broadcast_deinit(){
    int res = 0;
    if(udp_broadcast_pcb!=NULL){
        udp_remove(udp_broadcast_pcb);
    }
    return res;
}

static u32 add_string_from_regs(u8 * dest, u8 * from,u32 max_size){
    u32 size = strnlen((char*)from, max_size);
    memcpy(dest,from,size);
    return size;
}

static void udp_broadcast_server_recv(void *arg, struct udp_pcb *upcb,struct pbuf *p,\
                               const ip_addr_t *addr, u16_t port){
    LWIP_UNUSED_ARG(arg);
    LWIP_UNUSED_ARG(upcb);
    static char answer_buff[UDP_BROADCAST_MAX_PACKET_SIZE+1] = {0};
    char receive_buff[UDP_BROADCAST_MAX_PACKET_SIZE+1] = {0};
    int len = 0;
    int receive_len = p->tot_len;
    receive_len = receive_len>UDP_BROADCAST_MAX_PACKET_SIZE?UDP_BROADCAST_MAX_PACKET_SIZE:receive_len;
    pbuf_copy_partial(p, receive_buff, receive_len, 0);
    if (strncmp(ADVERTISMENT_REQUEST, &receive_buff[0], sizeof(ADVERTISMENT_REQUEST))==0){
        len += sprintf(answer_buff,"{\"device_name\": \"%s\",", DEVICE_NAME);
        len += sprintf(answer_buff,"{\"modbus_address\": %u,",regs_global->vars.mdb_addr);
        len += sprintf(&answer_buff[len],"\"name\": \"chili\",");
        len += sprintf(&answer_buff[len],"\"serial\": \"0000\",");
        len += sprintf(&answer_buff[len],"\"model\": \"son\",");
        len += sprintf(&answer_buff[len],"\"firmware\": \"0.0.1\",");
        len += sprintf(&answer_buff[len],",\"ip\": \"%u.%u.%u.%u\",",regs_global->vars.ip[0],regs_global->vars.ip[1],
                       regs_global->vars.ip[2],regs_global->vars.ip[3]);
        len += sprintf(&answer_buff[len],"\"netmask\": [%u.%u.%u.%u],",regs_global->vars.netmask[0],regs_global->vars.netmask[1],
                regs_global->vars.netmask[2],regs_global->vars.netmask[3]);
        len += sprintf(&answer_buff[len],"\"gateway\": [%u.%u.%u.%u],",regs_global->vars.gate[0],regs_global->vars.gate[1],
                regs_global->vars.gate[2],regs_global->vars.gate[3]);
        len += sprintf(&answer_buff[len],",\"port\": 502,}");
    }else {
        int position_modbus_id = -1;
        for (u16 i=0;i<receive_len-MODBUS_FIELD_SIZE;i++){
            if (strncmp(MODBUS_FIELD,&receive_buff[i],MODBUS_FIELD_SIZE) == 0){
                position_modbus_id = i;
                break;
            }
        }
        if (position_modbus_id>=0){
#if MODBUS_MASTER_ENABLE
            u8 modbus_id = (u8)atoi(&receive_buff[position_modbus_id+MODBUS_FIELD_SIZE  +3]);            
            if(add_ip_to_slave_table((uc8*)addr,modbus_id)){
                main_printf(TAG, "new modbus device found\n");
            }
#endif            
            u8 ip_address_temp[4];
            memcpy(ip_address_temp,addr,4);
        }
    }
    if (len){
        struct pbuf * udp_broadcast_send_pbuff;
        udp_broadcast_send_pbuff = pbuf_alloc(PBUF_TRANSPORT, (u16)len, PBUF_RAM);
        if (udp_broadcast_send_pbuff != NULL) {
            pbuf_take(udp_broadcast_send_pbuff,answer_buff,(u16)len);
            udp_sendto(udp_broadcast_pcb, udp_broadcast_send_pbuff, addr, port);
            pbuf_free(udp_broadcast_send_pbuff);
        }
    }
    pbuf_free(p);
}
int udp_broadcast_advertisement(udp_broadcast_option_t option){
    static char temp_buff[UDP_BROADCAST_MAX_PACKET_SIZE];
    int res = 0;
#define     DI_COUNT 16
    if(udp_broadcast_pcb!=NULL){
        int len = 0;
        if(option == UDP_BROADCAST_OPTION_INFORMATION){
            len += sprintf(&temp_buff[len],"{\"device_name\": \"%s\",", DEVICE_NAME);
#if MORSE
#elif FEEDER
#elif POLISHER
#elif TEST_INT
#elif STEP_MOTOR
#elif DI_HANDLING_ENABLE
            len += get_di_info(&temp_buff[len]);
#endif
            len += sprintf(&temp_buff[len],"}");
        }else{
            len += sprintf(temp_buff,ADVERTISMENT_REQUEST);
        }
        struct pbuf * udp_broadcast_send_pbuff;
        udp_broadcast_send_pbuff = pbuf_alloc(PBUF_TRANSPORT, (u16)len, PBUF_RAM);
        if (udp_broadcast_send_pbuff != NULL) {
            pbuf_take(udp_broadcast_send_pbuff,temp_buff,(u16)len);
            udp_sendto(udp_broadcast_pcb, udp_broadcast_send_pbuff, IP_ADDR_BROADCAST, UDP_BROADCAST_SELF_PORT);
            pbuf_free(udp_broadcast_send_pbuff);
        }else{
            res = ERR_BUF;
        }
    }else{
        res = ERR_CONN;
    }
    return res;
}
static int get_di_info(char * temp_buff){
    int len = 0;
    if(temp_buff == NULL){
        return 0;
    }else{
        u32 pin_states = 0;
        pin_states = di_control->vars.pin_state;
        for (u8 i =0;i<DI_COUNT;i++){
            len += sprintf(&temp_buff[len],"\"di_state_%u\": ",i);
            if (pin_states & (1<<i)){
                len += sprintf(&temp_buff[len],"1");
            }else{
                len += sprintf(&temp_buff[len],"0");
            }
            if(i<(DI_COUNT-1)){
                len += sprintf(&temp_buff[len],",");
            }
        }
    }
    return len;
}
#endif //UDP_BROADCAST_ENABLE
#endif //LWIP_UDP
#endif  //UDP_BROADCAST_C
