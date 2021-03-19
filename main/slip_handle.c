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
#define SLIP_RX_TASK_PRIORITY   10
#define SLIP_RX_TASK_STACK_SIZE (4 * 1024)
struct netif slipif;

static const char *TAG = "slip_handle";
static SemaphoreHandle_t flow_control_slip_mutex = NULL;
static flow_control_memory_t flow_control_slip_memory[SLIP_FLOW_CONTROL_QUEUE_LENGTH];
//static void slip_flow_control_task(void *args);
static err_t slip_netif_init(struct netif *netif);
static struct pbuf* slip_handle_if_input(uint8_t* buff,uint16_t length);
static err_t slip_modem_transmit(struct netif *netif,struct pbuf *p,const ip4_addr_t *ipaddr);
xQueueHandle slip_flow_control_queue = NULL;

typedef enum slipif_recv_state{
  SLIP_RECV_NORMAL,
  SLIP_RECV_ESCAPE
}slipif_recv_state_t;

static void slip_handle_uart_rx_task(void *arg);
//static esp_err_t pkt_slip2wifi(uint8_t *buffer, uint16_t len);
static bool rx_filter(uint8_t *data, uint32_t len);
// Internal handler called on driver start
/*static esp_err_t pkt_slip2wifi(uint8_t *buffer, uint16_t len){
    esp_err_t ret = ESP_OK;
    if (len < WIFI_SLIP_MAX_IP_PACKET){
        flow_control_memory_t * buffer_number = wifi_get_buffer();
        if (buffer_number!=NULL){
            wifi_fill_buffer(buffer_number, buffer, len);
            flow_control_msg_t msg = {
                .packet = buffer_number->packet,
                .length = &buffer_number->length
            };
            if(xQueueSend(wifi_flow_control_queue, &msg, pdMS_TO_TICKS(WIFI_FLOW_CONTROL_QUEUE_TIMEOUT_MS)) != pdTRUE){
                ESP_LOGE(TAG, "send flow control message failed or timeout");
                ret = ESP_FAIL;
                buffer_number->length = 0;
            }
        }
    }
    return ret;
}*/
static ip4_addr_t local_ipaddr;
static ip4_addr_t local_netmask;
static ip4_addr_t local_gw;
extern err_t tcpip_input(struct pbuf *p, struct netif *inp);
esp_err_t esp_slip_start(slip_handle_config_t *slip_modem){
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
    // Start slip RX task
    slip_modem->running = true;
    vSemaphoreCreateBinary(flow_control_slip_mutex)
    slip_flow_control_queue = xQueueCreate(SLIP_FLOW_CONTROL_QUEUE_LENGTH, sizeof(flow_control_msg_t));
    if (!slip_flow_control_queue) {
        ESP_LOGE(TAG, "create wifi to slip flow control queue failed");
        return ESP_FAIL;
    }
    IP4_ADDR(&local_ipaddr, 172, 16, 1, 2);
    IP4_ADDR(&local_netmask, 255, 255 , 255, 0);
    IP4_ADDR(&local_gw, 172, 16, 1, 232);
    netif_add(&slipif, &local_ipaddr, &local_netmask, &local_gw, NULL, &slip_netif_init, &ip_input);

    /*BaseType_t ret = xTaskCreate(slip_flow_control_task, "wifi2slip_flow_ctl", 2048, NULL, (tskIDLE_PRIORITY + 2), &slip_modem->slip_flow_control_handle);
    if (ret != pdTRUE) {
        ESP_LOGE(TAG, "create wifi to slip flow control task failed");
        return ESP_FAIL;
    }*/
    xTaskCreate(slip_handle_uart_rx_task, "slip_modem_uart_rx_task", SLIP_RX_TASK_STACK_SIZE, slip_modem, SLIP_RX_TASK_PRIORITY, &slip_modem->uart_rx_task);
    return ESP_OK;
}
esp_err_t slip_modem_destroy(slip_handle_config_t *slip_handle){
    // Stop uart rx task

    vTaskDelete(slip_handle->slip_flow_control_handle);
    vTaskDelete(slip_handle->uart_rx_task);
    vSemaphoreDelete(flow_control_slip_mutex);
    // Delete driver
    uart_driver_delete(slip_handle->uart_dev);
    // Free slip interface
    free(slip_handle);
    return ESP_OK;
}

esp_err_t slip_modem_transmit_raw(void *slip_driver, void *buffer, size_t len){
    ESP_LOGD(TAG, "%s", __func__);
    ESP_LOG_BUFFER_HEXDUMP(TAG, buffer, (uint16_t)len, ESP_LOG_DEBUG);
    slip_handle_config_t *slip_handle = (slip_handle_config_t *) slip_driver;
    int32_t res = uart_write_bytes(slip_handle->uart_dev, (char *)buffer, len);
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
    ESP_LOGD(TAG, "%s", __func__);
    ESP_LOGI(TAG, "slip transmit packet len - %u",p->len);
    /*for(uint16_t i=0;i<p->len;i++){
        ESP_LOGI(TAG, "%u",((u8_t *)p->payload)[i]);
    }*/
    uint32_t out_len = 0;
    /* Send pbuf out on the serial I/O device. */
    /* Start with packet delimiter. */
    uint8_t c = SLIP_END;
    res = uart_write_bytes(wifi_slip_config.uart_dev, (char *)&c, 1);
    if (res < 0) {
        // Handle errors
        ESP_LOGE(TAG, "%s: uart_write_bytes error %i", __func__, res);
    }
    for (q = p;q!=NULL;q=q->next){
        for (uint16_t i = 0; i < q->len; i++) {
            c = ((u8_t *)q->payload)[i];
            switch (c) {
            case SLIP_END:
                /* need to escape this byte (0xC0 -> 0xDB, 0xDC) */
                c = SLIP_ESC;
                res = uart_write_bytes(wifi_slip_config.uart_dev, (char *)&c, 1);
                c = SLIP_ESC_END;
                res = uart_write_bytes(wifi_slip_config.uart_dev, (char *)&c, 1);
                break;
              case SLIP_ESC:
                c = SLIP_ESC;
                res = uart_write_bytes(wifi_slip_config.uart_dev, (char *)&c, 1);
                c = SLIP_ESC_ESC;
                res = uart_write_bytes(wifi_slip_config.uart_dev, (char *)&c, 1);
                break;
              default:
                /* normal byte - no need for escaping */
                res = uart_write_bytes(wifi_slip_config.uart_dev, (char *)&c, 1);
                break;
            }
            if (res < 0) {
                // Handle errors
                ESP_LOGE(TAG, "%s: uart_write_bytes error %i", __func__, res);
            }
        }
        out_len += q->len;
    }
    /* End with packet delimiter. */
    c = SLIP_END;
    res = uart_write_bytes(wifi_slip_config.uart_dev, (char *)&c, 1);
    if (res < 0) {
        // Handle errors
        ESP_LOGE(TAG, "%s: uart_write_bytes error %i", __func__, res);
    }
    return (err_t)res;
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
    if (data[1] == '?') {
        switch (data[2]) {
        case 'P':
            ESP_LOGI(TAG, "Prefix request");
            return true;
        default:
            ESP_LOGI(TAG, "Unhandled request '%c'", data[2]);
            break;
        }
        return true;
    } else if (data[1] == '!') {
        switch (data[2]) {
        default:
            ESP_LOGI(TAG, "Unhandled command '%c'", data[2]);
            break;
        }
    }
    return false;
}/*
static void slip_flow_control_task(void *args){
    flow_control_msg_t msg;
    (void)(args);
    int res = 0;
    uint32_t timeout = 0;
    for (uint8_t i=0; i < SLIP_FLOW_CONTROL_QUEUE_LENGTH; i++){
        flow_control_slip_memory[i].length = 0;
    }
    ESP_LOGI(TAG, "slip_flow_control_task start");
    while (1) {
        if (xQueueReceive(slip_flow_control_queue, &msg, pdMS_TO_TICKS(SLIP_FLOW_CONTROL_QUEUE_TIMEOUT_MS)) == pdTRUE) {
            timeout = 0;
            if (*msg.length) {
                do {
                    vTaskDelay(pdMS_TO_TICKS(timeout));
                    timeout += 2;
                    slip_modem_transmit((void*)&wifi_slip_config, msg.packet, *msg.length);
                } while (res && timeout < WIFI_FLOW_CONTROL_WIFI_SEND_TIMEOUT_MS);
                if (res != ESP_OK) {
                    ESP_LOGE(TAG, "WiFi send packet failed: %d", res);
                }
            }
            *msg.length = 0;
        }
    }
    vTaskDelete(NULL);
}*/

static void slip_handle_uart_rx_task(void *arg){
    slip_handle_config_t *slip_handle = (slip_handle_config_t *) arg;
    ESP_LOGD(TAG, "Start SLIP modem RX task (slip_modem %p)", (void*)slip_handle);
    ESP_LOGD(TAG, "Uart: %d, buffer: %p (%d bytes)", slip_handle->uart_dev, (void*)slip_handle->rx_buffer, SLIP_HANDLE_RX_BUFFER_LEN);
    slipif_recv_state_t recv_state = SLIP_RECV_NORMAL;
    slip_handle->recv_buffer_len = 0;
    uint8_t temp_buff[SLIP_HANDLE_RX_BUFFER_LEN];
    ESP_LOGI(TAG, "slip_handle_uart_rx_task start");
    uint32_t tick=0;
    while (slip_handle->running == true){
        // Read data from the UART
        int len = uart_read_bytes(slip_handle->uart_dev, temp_buff,SLIP_HANDLE_RX_BUFFER_LEN, pdMS_TO_TICKS((10)));

        if (len > 0) {
            // Log slip RX data
            ESP_LOGD(TAG, "rx %d bytes", len);
            // Ensure null termination
            temp_buff[len] = '\0';
            // Filter if provided
            if (slip_handle->recv_buffer_len == 0){
                if (rx_filter(temp_buff, (uint32_t)len)) {
                    continue;
                }
            }
            if ((slip_handle->recv_buffer_len + (uint32_t)len) < SLIP_HANDLE_RX_BUFFER_LEN){
                for (uint32_t i =0 ; i < (uint32_t)len; i++){
                    switch (recv_state) {
                      case SLIP_RECV_NORMAL:
                        switch (temp_buff[i]){
                          case SLIP_END:
                            if (slip_handle->recv_buffer_len > 0){
                                struct pbuf *p;
                                ESP_LOGI(TAG, "slip recv packet len %u",slip_handle->recv_buffer_len);
                                /*for(uint16_t j=0;j<slip_handle->recv_buffer_len;j++){
                                    ESP_LOGI(TAG, "%u",temp_buff[j]);
                                }*/
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
            // Pass received bytes in to slip interface
        }
        // Yeild to allow other tasks to progress
        tick++;
        vTaskDelay( pdMS_TO_TICKS(10));
    }
}
flow_control_memory_t * slip_get_buffer(){
    flow_control_memory_t * flow_control_memory = NULL;
    xSemaphoreTake(flow_control_slip_mutex,1000);
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
        xSemaphoreTake(flow_control_slip_mutex,1000);
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
    netif->flags = NETIF_FLAG_UP | NETIF_FLAG_LINK_UP | NETIF_FLAG_BROADCAST | NETIF_FLAG_ETHARP;
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
/**
  * @brief calculation crc16 for buffer
  * @param buff - array with data
  * @param len - lenght data buffer without two crc byte
  * @return crc16
  * @ingroup crc
  */
uint16_t calc_crc16(uint8_t* buff, uint16_t len){
    uint16_t i,k;
    uint16_t crc;
    crc=k=0;
    while(k<len){
        crc=crc^(uint16_t)((uint16_t)buff[k++]<<8);
        i=8;
        do{
            if(crc & 0x8000){ crc=(uint16_t)(crc<<1)^0x1021;}
            else{ crc=(uint16_t)(crc<<1);}
        } while (--i);
    }
    return crc;
}
/**
  * @brief control check crc16
  * @param buff - array with las two byte for summ
  * @param len - lenght packet with two crc byte
  * @return 1 - OK, \n
  *         0 - ERROR
  * @ingroup crc
  */
uint16_t check_crc16(uint8_t* buff, uint16_t len){
    uint16_t crc;
    uint16_t i,k;
    crc=k=0;
    while(k<(len-2)){
        crc=crc^(uint16_t)(((uint16_t)buff[k++])<<8);
        i=8;
        do{
            if(crc & 0x8000){ crc=(uint16_t)(crc<<1)^0x1021;}
            else{ crc=(uint16_t)(crc<<1);}
        } while(--i);
    }
    if(crc==(buff[len-2]+((uint16_t)(buff[len-1]<<8)))){ return 1;}
    else{ return 0;}
}

#endif // SLIP_HANDLE_C

