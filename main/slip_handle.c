// Copyright 2020 Espressif Systems (Shanghai) PTE LTD
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
#ifndef SLIP_HANDLE_C
#define SLIP_HANDLE_C
#include "string.h"
#include "slip_handle.h"
#include "esp_netif.h"
#include "esp_netif_slip.h"
#include "esp_event.h"
#include "esp_log.h"
#include "lwip/opt.h"
#include "lwip/sio.h"
#include "lwip/def.h"
#include "lwip/pbuf.h"
#include "lwip/sys.h"
#include "lwip/stats.h"
#include "lwip/snmp.h"
#include "lwip/ip.h"
#include "lwipopts.h"
#include "regs.h"
#include "regs_description.h"
#include "crc.h"
#include "common.h"
struct netif slipif;

static const char *TAG = "slip_handle";
static semaphore_handle_t flow_control_slip_mutex = NULL;
static flow_control_memory_t flow_control_slip_memory[SLIP_FLOW_CONTROL_QUEUE_LENGTH];
static err_t slip_netif_init(struct netif *netif);
static struct pbuf* slip_handle_if_input(uint8_t* buff,uint16_t length);
static err_t slip_modem_transmit(struct netif *netif,struct pbuf *p,const ip4_addr_t *ipaddr);
queue_handle_t slip_flow_control_queue = NULL;
static esp_err_t slip_add_to_send_queue(uint8_t *buffer, uint16_t len);
static int slip_handle_request_parameters(void);

//static esp_err_t pkt_slip2wifi(uint8_t *buffer, uint16_t len);
static bool rx_filter(uint8_t *data, uint32_t len);
// Internal handler called on driver start

static esp_err_t slip_add_to_send_queue(uint8_t *buffer, uint16_t len){
    esp_err_t ret = ESP_OK;
    if (len < WIFI_SLIP_MAX_IP_PACKET){
        flow_control_memory_t * buffer_number = slip_get_buffer();
        if (buffer_number!=NULL){
            slip_fill_buffer(buffer_number, buffer, len);
            flow_control_msg_t msg = {
                .packet = buffer_number->packet,
                .length = &buffer_number->length
            };
            if(queue_send(slip_flow_control_queue, &msg, SLIP_FLOW_CONTROL_QUEUE_TIMEOUT_MS) != pdTRUE){
                ESP_LOGE(TAG, "send flow control message failed or timeout");
                ret = ESP_FAIL;
                buffer_number->length = 0;
            }
        }
    }
    return ret;
}
static ip4_addr_t local_ipaddr;
static ip4_addr_t local_netmask;
static ip4_addr_t local_gw;
extern err_t tcpip_input(struct pbuf *p, struct netif *inp);
esp_err_t esp_slip_init(slip_handle_config_t *slip_modem){
    ESP_LOGD(TAG, "%s: Starting SLIP modem (modem %p)", __func__, (void*)slip_modem);
    // Build configuration
    uart_config_t uart_config = {
        .baud_rate = slip_modem->uart_baud,
        .data_bits = UART_DATA_8_BITS,
        .parity = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
    };
    // Initialise uart
    ESP_ERROR_CHECK(uart_param_config(slip_modem->uart_dev, &uart_config));
    // Set UART pins
    ESP_ERROR_CHECK(uart_set_pin(slip_modem->uart_dev, slip_modem->uart_tx_pin, slip_modem->uart_rx_pin, 0, 0));
    // Install UART driver
    ESP_ERROR_CHECK(uart_driver_install(slip_modem->uart_dev, SLIP_HANDLE_RX_BUFFER_LEN, SLIP_HANDLE_RX_BUFFER_LEN, 10, &slip_modem->uart_queue, 0));
    main_printf(TAG,"inited uart with parameters: port - %d,rate - %d,pin_tx - %d, pin_rx - %d"
                ,slip_modem->uart_dev,slip_modem->uart_baud,slip_modem->uart_tx_pin,slip_modem->uart_rx_pin);
    // Start slip RX task
    slip_modem->running = true;
    semaphore_create_binary(flow_control_slip_mutex)
    slip_flow_control_queue = queue_create(SLIP_FLOW_CONTROL_QUEUE_LENGTH, sizeof(flow_control_msg_t));
    if (!slip_flow_control_queue) {
        ESP_LOGE(TAG, "create wifi to slip flow control queue failed");
    }
    return ESP_OK;
}
esp_err_t slip_modem_deinit(slip_handle_config_t *slip_handle){
    // Stop uart rx task
    if(slip_handle->slip_flow_control_handle!=NULL){
        task_delete(slip_handle->slip_flow_control_handle);
    }
    if(slip_handle->uart_rx_task!=NULL){
        task_delete(slip_handle->uart_rx_task);
    }
    semaphore_delete(flow_control_slip_mutex);
    // Delete driver
    uart_driver_delete(slip_handle->uart_dev);
    // Free slip interface
    free(slip_handle);
    return ESP_OK;
}

esp_err_t slip_modem_transmit_raw(void *buffer, size_t len){
    ESP_LOG_BUFFER_HEXDUMP(TAG, buffer, (uint16_t)len, ESP_LOG_DEBUG);
    int32_t res = slip_add_to_send_queue((u8 *)buffer, (u16)len);
    if (res < 0) {
        // Handle errors
        ESP_LOGE(TAG, "%s: uart_write_bytes error %i", __func__, res);
        return ESP_FAIL;
    }
    return ESP_OK;
}


err_t slip_modem_transmit(struct netif *netif,struct pbuf *p,const ip4_addr_t *ipaddr){
    int res;
    struct pbuf *q;
    static u8 transmit_temp_buff[SLIP_MAX_RAW_PACKET];
    uint32_t out_len = 0;
    /* Send pbuf out on the serial I/O device. */
    /* Start with packet delimiter. */
    uint8_t c;
    transmit_temp_buff[out_len++] = SLIP_END;
    for (q = p;q!=NULL;q=q->next){
        for (uint16_t i = 0; (i < q->len)&&(out_len<(SLIP_MAX_RAW_PACKET-1)); i++) {
            c = ((u8_t *)q->payload)[i];
            switch (c) {
            case SLIP_END:
                /* need to escape this byte (0xC0 -> 0xDB, 0xDC) */
                transmit_temp_buff[out_len++] = SLIP_ESC;
                transmit_temp_buff[out_len++] = SLIP_ESC_END;
                break;
              case SLIP_ESC:
                transmit_temp_buff[out_len++] = SLIP_ESC;
                transmit_temp_buff[out_len++] = SLIP_ESC_ESC;
                break;
              default:
                /* normal byte - no need for escaping */
                transmit_temp_buff[out_len++] = c;
                break;
            }
        }
    }
    /* End with packet delimiter. */
    transmit_temp_buff[out_len++] = SLIP_END;
    res = slip_add_to_send_queue((u8 *)transmit_temp_buff, (u16)out_len);
    if (res < 0) {
        // Handle errors
        ESP_LOGE(TAG, "%s: uart_write_bytes error %i", __func__, res);
    }
    return (err_t)res;
}
static int slip_handle_request_parameters(void){
    u8 buf_temp[30];
    main_printf(TAG,"slip send request to connect");
    buf_temp[0] = SLIP_END;
    buf_temp[1] = '?';
    buf_temp[2] = 'I';
    buf_temp[3] = SLIP_END;
    slip_modem_transmit_raw(buf_temp, 4);
    return 0;
}
/**
 * @brief slip_rx_filter filters incoming commands from the slip interface
 * this implementation is designed for use with contiki slip devices
 * @param ctx
 * @param data
 * @param len
 * @return
 */
static bool rx_filter(uint8_t *data, uint32_t len){
    (void)(len);
    bool res = 0;
    u8 buf_temp[30];
    if ((strncmp("!S", (char*)data,2)==0)&&(len==2)){
        slip_handle_request_parameters();
        res = 1;
    }else if((strncmp("!I", (char*)data,2)==0)){
        u8 handle_len = 2;
        u8 ip_local[4];
        u8 ip_local_mask[4];
        u8 ip_local_gate[4];
        u8 wifi_name[WIFI_NAME_LEN];
        u8 wifi_pass[WIFI_PASSWORD_LEN];
        u8 wifi_router_name[WIFI_ROUTER_NAME_LEN];
        u8 wifi_router_pass[WIFI_ROUTER_PASSWORD_LEN];
        u16 wifi_setting=0;
        u16 crc_sended=0;
        ip_local[0] = data[handle_len++];
        ip_local[1] = data[handle_len++];
        ip_local[2] = data[handle_len++];
        ip_local[3] = data[handle_len++];
        ip_local_mask[0] = data[handle_len++];
        ip_local_mask[1] = data[handle_len++];
        ip_local_mask[2] = data[handle_len++];
        ip_local_mask[3] = data[handle_len++];
        ip_local_gate[0] = data[handle_len++];
        ip_local_gate[1] = data[handle_len++];
        ip_local_gate[2] = data[handle_len++];
        ip_local_gate[3] = data[handle_len++];

        for (u8 i=0;i<WIFI_NAME_LEN;i++){
            wifi_name[i] = data[handle_len++];
        }
        for (u8 i=0;i<WIFI_PASSWORD_LEN;i++){
            wifi_pass[i] = data[handle_len++];
        }
        for (u8 i=0;i<WIFI_ROUTER_NAME_LEN;i++){
            wifi_router_name[i] = data[handle_len++];
        }
        for (u8 i=0;i<WIFI_ROUTER_PASSWORD_LEN;i++){
            wifi_router_pass[i] = data[handle_len++];
        }
        wifi_setting = data[handle_len++];
        wifi_setting |= (u16)data[handle_len++]<<8;
        crc_sended = data[handle_len++];
        crc_sended |= (u16)data[handle_len++]<<8;
        if(check_crc16(data,handle_len)){
            int writed=0;
            writed = regs_set_buffer(&regs_global.vars.wifi_setting,(u8*)&wifi_setting,sizeof(wifi_setting))==1?1:writed ;
            if (wifi_setting==WIFI_ACCESS_POINT || wifi_setting==WIFI_AP_STA){
                writed = regs_set_buffer(regs_global.vars.wifi_name,wifi_name,WIFI_NAME_LEN)==1?1:writed ;
                writed = regs_set_buffer(regs_global.vars.wifi_password,wifi_pass,WIFI_PASSWORD_LEN)==1?1:writed ;
            }
            writed = regs_set_buffer(regs_global.vars.slip_ip,ip_local_gate,4)==1?1:writed ;
            writed = regs_set_buffer(regs_global.vars.slip_netmask,ip_local_mask,4)==1?1:writed;
            writed = regs_set_buffer(regs_global.vars.slip_gate,ip_local,4)==1?1:writed ;
            if (wifi_setting==WIFI_CLIENT || wifi_setting==WIFI_AP_STA){
                writed = regs_set_buffer(regs_global.vars.wifi_router_name,wifi_router_name,WIFI_ROUTER_NAME_LEN)==1?1:writed ;
                writed = regs_set_buffer(regs_global.vars.wifi_router_password,wifi_router_pass,WIFI_ROUTER_PASSWORD_LEN)==1?1:writed ;
            }
            if (writed){
                ui32 prev_value = 0;
                main_printf(TAG,"wifi settings was changed prepare to reset");
                task_notify_send(common_duty_task_handle,PREPARE_TO_RESET,&prev_value);
            }
            {
                u8 len_temp = 0;
                buf_temp[len_temp++] = SLIP_END;
                buf_temp[len_temp++] = '!';
                buf_temp[len_temp++] = 'C';
                if ((crc_sended&0xff)==SLIP_END) {
                    buf_temp[len_temp++] = SLIP_ESC;
                    buf_temp[len_temp++] = SLIP_ESC_END;
                }else if ((crc_sended&0xff)==SLIP_ESC){
                    buf_temp[len_temp++] = SLIP_ESC;
                    buf_temp[len_temp++] = SLIP_ESC_ESC;
                }else{
                    buf_temp[len_temp++] = (crc_sended&0xff);
                }
                if (((crc_sended>>8)&0xff)==SLIP_END) {
                    buf_temp[len_temp++] = SLIP_ESC;
                    buf_temp[len_temp++] = SLIP_ESC_END;
                }else if (((crc_sended>>8)&0xff)==SLIP_ESC){
                    buf_temp[len_temp++] = SLIP_ESC;
                    buf_temp[len_temp++] = SLIP_ESC_ESC;
                }else{
                    buf_temp[len_temp++] = ((crc_sended>>8)&0xff);
                }
                buf_temp[len_temp++] = SLIP_END;
                slip_modem_transmit_raw(buf_temp, len_temp);
            }
            res = 1;
        }
    }
    return res;
}
void slip_flow_control_task(void *args){
    flow_control_msg_t msg;
    (void)(args);
    int res = 0;
    uint32_t timeout = 0;
    for (uint8_t i=0; i < SLIP_FLOW_CONTROL_QUEUE_LENGTH; i++){
        flow_control_slip_memory[i].length = 0;
    }
    ESP_LOGI(TAG, "slip_flow_control_task start");
    while (1) {
        if (queue_receive(slip_flow_control_queue, &msg, SLIP_FLOW_CONTROL_QUEUE_TIMEOUT_MS) == pdTRUE) {
            timeout = 0;
            if (*msg.length) {
                res = uart_write_bytes(wifi_slip_config.uart_dev, msg.packet, *msg.length);
                if (res != *msg.length) {
                    ESP_LOGE(TAG, "slip send packet failed: %d %d", res,*msg.length);
                }
            }
            *msg.length = 0;
        }
    }
    vTaskDelete(NULL);
}
#define TIME_BETWEEN_CHECK_PARAMETERS_MS 15000
void slip_handle_uart_rx_task(void *arg){
    slip_handle_config_t *slip_handle = (slip_handle_config_t *) arg;
    ESP_LOGD(TAG, "Start SLIP modem RX task (slip_modem %p)", (void*)slip_handle);
    ESP_LOGD(TAG, "Uart: %d, buffer: %p (%d bytes)", slip_handle->uart_dev, (void*)slip_handle->rx_buffer, SLIP_HANDLE_RX_BUFFER_LEN);
    slipif_recv_state_t recv_state = SLIP_RECV_NORMAL;
    main_printf(TAG,"slip ip %d %d %d %d", regs_global.vars.slip_ip[0], regs_global.vars.slip_ip[1], regs_global.vars.slip_ip[2], regs_global.vars.slip_ip[3]);
    IP4_ADDR(&local_ipaddr, regs_global.vars.slip_ip[0], regs_global.vars.slip_ip[1], regs_global.vars.slip_ip[2], regs_global.vars.slip_ip[3]);
    IP4_ADDR(&local_netmask, regs_global.vars.slip_netmask[0], regs_global.vars.slip_netmask[1], regs_global.vars.slip_netmask[2], regs_global.vars.slip_netmask[3]);
    IP4_ADDR(&local_gw, regs_global.vars.slip_gate[0], regs_global.vars.slip_gate[1], regs_global.vars.slip_gate[2], regs_global.vars.slip_gate[3]);
    netif_add(&slipif, &local_ipaddr, &local_netmask, &local_gw, NULL, &slip_netif_init, &ip_input);

    slip_handle->recv_buffer_len = 0;
    uint8_t temp_buff[SLIP_HANDLE_RX_BUFFER_LEN];
    main_printf(TAG, "slip_handle_uart_rx_task start");
    uint32_t tick=0;
    u32 tick_count_ms = pdTICKS_TO_MS(task_get_tick_count());
    slip_handle_request_parameters();
    while (slip_handle->running == true){
        // Read data from the UART
        int len = uart_read_bytes(slip_handle->uart_dev, temp_buff,SLIP_HANDLE_RX_BUFFER_LEN, pdMS_TO_TICKS((5)));
        if (len > 0) {
            // Ensure null termination
            temp_buff[len] = '\0';
            // Filter if provided

            if (rx_filter(temp_buff, (uint32_t)len)){
                slip_handle->recv_buffer_len = 0;
                continue;
            }else{
                if ((slip_handle->recv_buffer_len + (uint32_t)len) < SLIP_HANDLE_RX_BUFFER_LEN){
                    for (uint32_t i =0 ; i < (uint32_t)len; i++){
                        switch (recv_state) {
                          case SLIP_RECV_NORMAL:
                            switch (temp_buff[i]){
                              case SLIP_END:
                                if (slip_handle->recv_buffer_len > 0){
                                    struct pbuf *p;
                                    p = slip_handle_if_input(slip_handle->rx_buffer,(uint16_t)slip_handle->recv_buffer_len);
                                    if(p != NULL){
                                        if (slipif.input(p, &slipif) != ERR_OK ){
                                            pbuf_free(p);
                                            ESP_LOGE(TAG, "slipif.input failed");
                                        }
                                    }else{
                                        ESP_LOGE(TAG, "slip_handle_if_input failed");
                                    }
                                }
                                slip_handle->recv_buffer_len = 0;
                                break;
                              case SLIP_ESC:
                                recv_state = SLIP_RECV_ESCAPE;
                                break;
                              default:
                                slip_handle->rx_buffer[slip_handle->recv_buffer_len] = temp_buff[i];
                                slip_handle->recv_buffer_len++;
                                break;
                            } /* end switch (c) */
                            break;
                          case SLIP_RECV_ESCAPE:
                            /* un-escape END or ESC bytes, leave other bytes
                               (although that would be a protocol error) */
                            switch (temp_buff[i]) {
                              case SLIP_ESC_END:
                                slip_handle->rx_buffer[slip_handle->recv_buffer_len] = SLIP_END;
                                slip_handle->recv_buffer_len++;
                                break;
                              case SLIP_ESC_ESC:
                                slip_handle->rx_buffer[slip_handle->recv_buffer_len] = SLIP_ESC;
                                slip_handle->recv_buffer_len++;
                                break;
                              default:
                                slip_handle->recv_buffer_len = 0;
                                break;
                            }
                            recv_state = SLIP_RECV_NORMAL;
                            break;
                        } /* end switch (priv->state) */
                    }
                }else{
                    slip_handle->recv_buffer_len = 0;
                }
            }
            // Pass received bytes in to slip interface
        }
        if ((pdTICKS_TO_MS(task_get_tick_count()) - tick_count_ms) >TIME_BETWEEN_CHECK_PARAMETERS_MS){
            tick_count_ms = pdTICKS_TO_MS(task_get_tick_count());
            main_printf(TAG,"slip send request to connect %u",(uint32_t)tick_count_ms);
            slip_handle_request_parameters();
        }
        tick++;
    }
}
flow_control_memory_t * slip_get_buffer(){
    flow_control_memory_t * flow_control_memory = NULL;
    semaphore_take(flow_control_slip_mutex,1000);
    for (uint16_t i =0;i<SLIP_FLOW_CONTROL_QUEUE_LENGTH;i++){
        if (flow_control_slip_memory[i].length == 0){
            flow_control_memory = &flow_control_slip_memory[i];
        }
    }
    xSemaphoreGive(flow_control_slip_mutex);
    return flow_control_memory;
}
int slip_fill_buffer(flow_control_memory_t * flow_control_memory,uint8_t * buffer,uint16_t len){
    if (flow_control_memory!=NULL && len < WIFI_SLIP_MAX_IP_PACKET){
        semaphore_take(flow_control_slip_mutex,1000);
        memcpy(flow_control_memory->packet,buffer,len);
        memcpy(&flow_control_memory->length,&len,sizeof(len));
        xSemaphoreGive(flow_control_slip_mutex);
    }else{
        return -1;
    }
    return 0;
}

err_t slip_netif_init(struct netif *netif){
    err_t result = ERR_OK;
    netif->name[0] = 's';
    netif->name[1] = 'l';
    netif->output = slip_modem_transmit;
    //	netif->linkoutput = slipif_output;
    netif->mtu = TCP_MSS;
    netif->flags = NETIF_FLAG_UP | NETIF_FLAG_LINK_UP;
    netif->state = 0;
    /* initialize the snmp variables and counters inside the struct netif
   * ifSpeed: no assumption can be made without knowing more about the
   * serial line!
   */
    NETIF_INIT_SNMP(netif, snmp_ifType_slip, 0);
    return result;
}
/**
 * Handle the incoming SLIP stream character by character
 *
 * Poll the serial layer by calling sio_read() or sio_tryread().
 *
 * @param netif the lwip network interface structure for this slipif
 * @param block if 1, block until data is received; if 0, return when all data
 *        from the buffer is received (multiple calls to this function will
 *        return a complete packet, NULL is returned before - used for polling)
 * @return The IP packet when SLIP_END is received
 * analog ethernet_input(struct pbuf *p, struct netif *netif)
 */
struct pbuf* slip_handle_if_input(uint8_t* buff,uint16_t length){
    struct pbuf *p = NULL;
    struct pbuf *q;
    /* allocate a new pbuf */
    LWIP_DEBUGF(SLIP_DEBUG, ("slipif_input: alloc\n"));
    if (length > 0){
        p = pbuf_alloc(PBUF_TRANSPORT,length, PBUF_POOL);
    }
    if (p != NULL) {
        uint32_t bufferoffset = 0;
        for(q = p; q != NULL; q = q->next){
            memcpy( (uint8_t*)q->payload, (uint8_t*)((uint8_t*)buff + bufferoffset), q->len);
            bufferoffset += q->len;
        }
    }else{
        ESP_LOGE(TAG,"Failed %s:%d\n",__FILE__,__LINE__);
    }
    return p;
}

#endif // SLIP_HANDLE_C

