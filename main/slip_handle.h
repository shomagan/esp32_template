#ifndef SLIP_HANDLE_H
#define SLIP_HANDLE_H
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

#pragma once
#include <stdbool.h>
#include <stdint.h>
#include "esp_netif.h"
#include "driver/uart.h"
#include "wifi_slip_main.h"

#define SLIP_END     0xC0 /* 0300: start and end of every packet */
#define SLIP_ESC     0xDB /* 0333: escape start (one byte escaped data follows) */
#define SLIP_ESC_END 0xDC /* 0334: following escape: original byte is 0xC0 (END) */
#define SLIP_ESC_ESC 0xDD /* 0335: following escape: original byte is 0xDB (ESC) */

/** Maximum packet size that is received by this netif */
typedef enum {
    WIFI_ACCESS_POINT = (1<<0),
    WIFI_CLIENT       = (1<<1)
}wifi_setting_t;/*todo!!!rename to meso setting */

#define SLIP_HANDLE_RX_BUFFER_LEN SLIP_MAX_RAW_PACKET
// Filter callbacks for handling application specific slip messages
typedef int slip_rx_filter_cb_t(void *ctx, uint8_t *data, uint32_t len);
/** @brief Configuration structure for SLIP modem interface
 *
 */
typedef struct {
    uint8_t running;
    uint8_t status;
    uint8_t reserv0;
    uint8_t reserv1;
    uart_port_t uart_dev; /* UART device for reading and writing SLIP information, this must be initialised externally */
    int uart_tx_pin;      /* UART TX pin number */
    int uart_rx_pin;      /* UART TX pin number */
    int uart_baud;   /* UART baud rate */
    uint32_t recv_buffer_len;             /* Length packet received*/
    TaskHandle_t uart_rx_task;
    TaskHandle_t slip_flow_control_handle;
    TaskHandle_t wifi_flow_control_handle;
    QueueHandle_t uart_queue;
    uint8_t rx_buffer[SLIP_HANDLE_RX_BUFFER_LEN];
} slip_handle_config_t;
extern struct netif slipif;

extern slip_handle_config_t wifi_slip_config;
extern xQueueHandle slip_flow_control_queue;
bool slip_rx_filter(uint8_t *data, uint32_t len);
esp_err_t slip_modem_transmit_raw(void *slip_driver, void *buffer, size_t len);
flow_control_memory_t * slip_get_buffer();
int slip_fill_buffer(flow_control_memory_t * flow_control_memory,uint8_t * buffer,uint16_t len);
esp_err_t esp_slip_start(slip_handle_config_t *slip_modem);
/**
  * @brief calculation crc16 for buffer
  * @param buff - array with data
  * @param len - lenght data buffer without two crc byte
  * @return crc16
  * @ingroup crc
  */
uint16_t calc_crc16(uint8_t* buff, uint16_t len);
/**
  * @brief control check crc16
  * @param buff - array with las two byte for summ
  * @param len - lenght packet with two crc byte
  * @return 1 - OK, \n
  *         0 - ERROR
  * @ingroup crc
  */
uint16_t check_crc16(uint8_t* buff, uint16_t len);


#endif //SLIP_HANDLE_H
