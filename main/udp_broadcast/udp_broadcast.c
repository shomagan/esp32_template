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
#define MODBUS_FIELD_SIZE sizeof(MODBUS_FIELD)
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

static struct pbuf * udp_broadcast_send_pbuff;
static u32 add_string_from_regs(u8 * dest, u8 * from,u32 max_size){
    u32 size = strnlen((char*)from, max_size);
    memcpy(dest,from,size);
    return size;
}
static void udp_broadcast_server_recv(void *arg, struct udp_pcb *upcb,struct pbuf *p,\
                               const ip_addr_t *addr, u16_t port){
    LWIP_UNUSED_ARG(arg);
    LWIP_UNUSED_ARG(upcb);
    static char answer_buff[UDP_BROADCAST_MAX_PACKET_SIZE];
    char receive_buff[UDP_BROADCAST_MAX_PACKET_SIZE];
    int len = 0;
    int receive_len = p->tot_len;
    receive_len = receive_len>UDP_BROADCAST_MAX_PACKET_SIZE?UDP_BROADCAST_MAX_PACKET_SIZE:receive_len;
    pbuf_copy_partial(p, receive_buff, receive_len, 0);
    if (strncmp(ADVERTISMENT_REQUEST, &receive_buff[0], sizeof(ADVERTISMENT_REQUEST))==0){
        len += sprintf(answer_buff,"{\"modbus_address\": %u,",regs_global.vars.mdb_addr);
        len += sprintf(&answer_buff[len],"\"name\": \"chili\",");
        len += sprintf(&answer_buff[len],"\"serial\": \"0000\",");
        len += sprintf(&answer_buff[len],"\"model\": \"son\",");
        len += sprintf(&answer_buff[len],"\"firmware\": \"0.0.1\",");
        len += sprintf(&answer_buff[len],",\"ip\": \"%u.%u.%u.%u\",",regs_global.vars.ip[0],regs_global.vars.ip[1],
                       regs_global.vars.ip[2],regs_global.vars.ip[3]);
        len += sprintf(&answer_buff[len],"\"netmask\": [%u.%u.%u.%u],",regs_global.vars.netmask[0],regs_global.vars.netmask[1],
                regs_global.vars.netmask[2],regs_global.vars.netmask[3]);
        len += sprintf(&answer_buff[len],"\"gateway\": [%u.%u.%u.%u],",regs_global.vars.gate[0],regs_global.vars.gate[1],
                regs_global.vars.gate[2],regs_global.vars.gate[3]);
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
            u8 modbus_id = (u8)atoi(&receive_buff[position_modbus_id+MODBUS_FIELD_SIZE  +3]);
            if(add_ip_to_slave_table((uc8*)addr,modbus_id)){
                main_printf(TAG, "new modbus device was found\n");
            }
            u8 ip_address_temp[4];
            memcpy(ip_address_temp,addr,4);
            main_printf(TAG, "we received advertisment from slave %u.%u.%u.%u id %u",ip_address_temp[0],ip_address_temp[1],
                    ip_address_temp[2],ip_address_temp[3],modbus_id);
        }
    }
    if (len){
        udp_broadcast_send_pbuff = pbuf_alloc(PBUF_TRANSPORT, (u16)len, PBUF_RAM);
        if (udp_broadcast_send_pbuff != NULL) {
            pbuf_take(udp_broadcast_send_pbuff,answer_buff,(u16)len);
            udp_sendto(udp_broadcast_pcb, udp_broadcast_send_pbuff, addr, port);
            pbuf_free(udp_broadcast_send_pbuff);
        }
    }
    pbuf_free(p);
}
int udp_broadcast_advertisement(void){
    static char temp_buff[UDP_BROADCAST_MAX_PACKET_SIZE];
    int res = 0;
    if(udp_broadcast_pcb!=NULL){
        int len = 0;
        len += sprintf(temp_buff,ADVERTISMENT_REQUEST);
        res = udp_connect(udp_broadcast_pcb, IP_ADDR_ANY, UDP_BROADCAST_SELF_PORT);
        if (res == ERR_OK){
            udp_broadcast_send_pbuff = pbuf_alloc(PBUF_TRANSPORT, (u16)len, PBUF_RAM);
            if (udp_broadcast_send_pbuff != NULL) {
                pbuf_take(udp_broadcast_send_pbuff,temp_buff,(u16)len);
                udp_sendto(udp_broadcast_pcb, udp_broadcast_send_pbuff, IP_ADDR_BROADCAST, UDP_BROADCAST_SELF_PORT);
                pbuf_free(udp_broadcast_send_pbuff);
            }else{
                res = ERR_BUF;
            }
            udp_disconnect(udp_broadcast_pcb);
        }
    }else{
        res = ERR_CONN;
    }
    return res;
}

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

#endif //UDP_BROADCAST_ENABLE
#endif //LWIP_UDP
#endif  //UDP_BROADCAST_C
