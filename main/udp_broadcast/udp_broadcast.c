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
#define IIRLS_NAME "IIRLS-PCBS-1.0.0"
/**
 * @brief udp_broadcast use for bradcast advertisment
 *
 */
#if LWIP_UDP
#if UDP_BROADCAST_ENABLE
#include "lwip/udp.h"
#include "lwip/timeouts.h"
/**
answer example
{
    "pcbs": "HVEL1-PCBS-4.0.0",
    "name": "HVEL",
    "serial": "HVEL10000",
    "model": "HVEL1-0123456789ABCD",
    "firmware": "1.0.0-alpha.1",
    "firmwareMetaData": "0-g123456789ABCDEF",
    "gid": "G00000R00",
    "mech": "HVEL1-MECH-1.0.0",
    "icbls": "HVEL1-ICBLS-1.0.0",
    "ecbls": "HVEL1-ECBLS-1.0.0",
    "ethernet": {
        "lan": {
        "ip": "192.168.1.100",
        "netmask": "255.255.255.0",
        "gateway": "192.168.1.1",
        "dhcp": true
        },
        "service": {
        "ip": "192.168.1.100",
        "netmask": "255.255.255.0",
        "gateway": "192.168.1.1"
        }
    },
    "communications": [
        {
        "type": "Modbus/TCP",
        "port": 502,
        "unitId": 3
        },
        {
        "type": "XML/TCP",
        "port": 65550
        }
    ]
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
    int len = 0;
    char *buff = (char*)p->payload;
    if(ip_addr_isbroadcast(ip_current_dest_addr(), ip_current_netif())){
        len += sprintf(answer_buff,"{\"pcbs\": \"IIRLS-PCBS-1.0.0\"");
        len += sprintf(&answer_buff[len],",\"ethernet\": {\"lan\": {\"ip\": [%u.%u.%u.%u],",regs_global.vars.ip[0],regs_global.vars.ip[1],
                       regs_global.vars.ip[2],regs_global.vars.ip[3]);
        len += sprintf(&answer_buff[len],"\"netmask\": [%u.%u.%u.%u],",regs_global.vars.netmask[0],regs_global.vars.netmask[1],
                regs_global.vars.netmask[2],regs_global.vars.netmask[3]);
        len += sprintf(&answer_buff[len],"\"gateway\": [%u.%u.%u.%u],",regs_global.vars.gate[0],regs_global.vars.gate[1],
                regs_global.vars.gate[2],regs_global.vars.gate[3]);
        len += sprintf(&answer_buff[len],"}},\"communications\": [{\"type\": \"Modbus/TCP\",\"port\": 502,\"unitID\": %u}]}",regs_global.vars.mdb_addr);
    }else{
        /*master slave communication advertisment*/
        if (strncmp(IIRLS_NAME, &buff[10], 5)==0){
            add_ip_to_slave_table((uc8*)addr);
            u8 ip_address_temp[4];
            memcpy(ip_address_temp,addr,4);
            main_printf("udp","we received advertisment from slave %u.%u.%u.%u",ip_address_temp[0],
                    ip_address_temp[1],ip_address_temp[2],ip_address_temp[3]);
            len += sprintf(answer_buff,"master confirmation");
        }
        /*master slave communication advertisment end*/
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
    char temp_buff[UDP_BROADCAST_MAX_PACKET_SIZE];
    int res = 0;
    if(udp_broadcast_pcb!=NULL){
        int len = 0;
        len += sprintf(temp_buff,"advertisement");
        res = udp_connect(udp_broadcast_pcb, IP_ADDR_ANY, UDP_BROADCAST_SELF_PORT);
        if (res == ERR_OK){
            udp_broadcast_send_pbuff = pbuf_alloc(PBUF_TRANSPORT, (u16)len, PBUF_RAM);
            if (udp_broadcast_send_pbuff != NULL) {
                memcpy(udp_broadcast_send_pbuff->payload,temp_buff,(u16)len);
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
