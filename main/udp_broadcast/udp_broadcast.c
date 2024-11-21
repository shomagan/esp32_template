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
#include "credentials.h"
#if MORSE
#include "morse.h"
#endif
#include "common.h"
/**
 * @brief udp_broadcast use for bradcast advertisment
 *
 */
#if LWIP_UDP
#if UDP_BROADCAST_ENABLE
#include "lwip/udp.h"
#include "lwip/timeouts.h"
#if SOC_HMAC_SUPPORTED
    #include "esp_efuse.h"
    #include "esp_efuse_table.h"

    #include "esp_hmac.h"
#endif
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
#if SOC_HMAC_SUPPORTED
    const uint8_t key_data[32] = hmac_key_32;
    esp_efuse_write_key(EFUSE_BLK_KEY4, ESP_EFUSE_KEY_PURPOSE_HMAC_UP, key_data, sizeof(key_data));
#endif
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
        len += sprintf(&answer_buff[len],"{\"device_name\": \"%s\",", DEVICE_NAME);
        len += sprintf(&answer_buff[len],"\"modbus_address\": %u,",regs_global->vars.mdb_addr);
        len += sprintf(&answer_buff[len],"\"personal_name\": \"chili\",");
        len += sprintf(&answer_buff[len],"\"serial\": \"0000\",");
        len += sprintf(&answer_buff[len],"\"model\": \"son\",");
        len += sprintf(&answer_buff[len],"\"firmware\": \"0.0.1\",");
        len += sprintf(&answer_buff[len],"\"ip\": [%u,%u,%u,%u],",regs_global->vars.ip[0],regs_global->vars.ip[1],
                       regs_global->vars.ip[2],regs_global->vars.ip[3]);
        len += sprintf(&answer_buff[len],"\"netmask\": [%u,%u,%u,%u],",regs_global->vars.netmask[0],regs_global->vars.netmask[1],
                regs_global->vars.netmask[2],regs_global->vars.netmask[3]);
        len += sprintf(&answer_buff[len],"\"gateway\": [%u,%u,%u,%u],",regs_global->vars.gate[0],regs_global->vars.gate[1],
                regs_global->vars.gate[2],regs_global->vars.gate[3]);
        len += sprintf(&answer_buff[len],"\"port\": 502}");
    }else {
        int position = -1;
        for (u16 i=0;i<receive_len-MODBUS_FIELD_SIZE;i++){
            if (strncmp(MODBUS_FIELD,&receive_buff[i],MODBUS_FIELD_SIZE) == 0){
                position = i;
                break;
            }
        }
        if (position>=0){
#if MODBUS_MASTER_ENABLE
            u8 modbus_id = (u8)atoi(&receive_buff[position + MODBUS_FIELD_SIZE  +3]);            
            if(add_ip_to_slave_table((uc8*)addr,modbus_id)){
                main_printf(TAG, "new modbus device found\n");
            }
#endif            
        }
#if MORSE
        u8 message_valid = 1;
        u8 last_symbol_position = receive_len - 2; 
#if SOC_HMAC_SUPPORTED
        if(receive_len > 32){
            uint8_t hmac[32];
            esp_hmac_calculate(HMAC_KEY4, receive_buff, receive_len-32, hmac);
            if(memcmp(&receive_buff[receive_len-32], hmac, 32) != 0){
                message_valid = 0;
            }
            last_symbol_position = receive_len - 32 - 2;
        }
#endif /* SOC_HMAC_SUPPORTED */
        const char * param_message = "message";
        u8 str_size = strlen(param_message);
        uint32_t prev_value = 0;
        task_notify_send(common_duty_task_handle,UDB_BROADCAST_MSG_RECEIVED,&prev_value);
        if(message_valid){
            position = -1;
            for (u16 i=0;i<receive_len - str_size;i++){
                if (strncmp(param_message,&receive_buff[i],str_size) == 0){
                    position = i + str_size + 4;/*remove ": "*/
                    break;
                }
            }
            if(position > 0 && position < receive_len){
                u8 message_len = last_symbol_position - position;/*remove " */
                if(message_len > 0){
                    u8 ip_address_temp[4];
                    memcpy(ip_address_temp,addr,4);
                    receive_buff[position-4] = (u8)(ip_address_temp[3]/100) + 0x30;
                    receive_buff[position-3] = (u8)((ip_address_temp[3]%100)/10) + 0x30;
                    receive_buff[position-2] = (u8)(ip_address_temp[3]%10) + 0x30;
                    receive_buff[position-1] = ':';
                    if(message_is_new((u8*)&receive_buff[position-4],message_len+4)){
                        semaphore_take(regs_access_mutex, portMAX_DELAY);{
                        u8 line = morse_reg->vars.morse_counter % DISPLAY_LINES_NUM;
                        u8 * line_pointer = get_pointer_to_line(line);
                        if (line_pointer != NULL) {
                            memset(line_pointer, 0, MAX_MORSE_MESSAGE_LEN);
                            memcpy(line_pointer, &receive_buff[position-4], message_len+4);
                        }
                        morse_reg->vars.morse_counter++;
                        }semaphore_release(regs_access_mutex);
                    }
                }
            }
        }
#endif /* MORSE */
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
            len += sprintf(&temp_buff[len],"{\"device_name\": \"%s\"", DEVICE_NAME);
#if MORSE
            len += sprintf(&temp_buff[len],",\"message\": \"");
            u8 current_message_len = (u8)strlen((char*)&morse_reg->vars.morse_send[0]);
            regs_copy_safe(&temp_buff[len], &morse_reg->vars.morse_send[0], current_message_len);
            len += current_message_len;
            len += sprintf(&temp_buff[len],"\"");
#elif FEEDER
#elif POLISHER
#elif TEST_INT
#elif STEP_MOTOR
#elif DI_HANDLING_ENABLE
            len += get_di_info(&temp_buff[len]);
#endif
            len += sprintf(&temp_buff[len],"}");
#if SOC_HMAC_SUPPORTED
            uint8_t hmac[32];
            esp_hmac_calculate(HMAC_KEY4, temp_buff, len, hmac);
            memcpy(&temp_buff[len], hmac, 32);
            len += 32;
#endif /*SOC_HMAC_SUPPORTED*/

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
