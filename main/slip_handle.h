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
#include "os_type.h"

#define SLIP_END     0xC0 /* 0300: start and end of every packet */
#define SLIP_ESC     0xDB /* 0333: escape start (one byte escaped data follows) */
#define SLIP_ESC_END 0xDC /* 0334: following escape: original byte is 0xC0 (END) */
#define SLIP_ESC_ESC 0xDD /* 0335: following escape: original byte is 0xDB (ESC) */
#define SLIP_RX_TASK_PRIORITY   10
#define SLIP_RX_TASK_STACK_SIZE (4 * 1024)


#define SLIP_HANDLE_RX_BUFFER_LEN SLIP_MAX_RAW_PACKET
// Filter callbacks for handling application specific slip messages
typedef enum slipif_recv_state{
  SLIP_RECV_NORMAL,
  SLIP_RECV_ESCAPE
}slipif_recv_state_t;

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
    task_handle_t uart_rx_task;
    task_handle_t slip_flow_control_handle;
    task_handle_t wifi_flow_control_handle;
    queue_handle_t uart_queue;
    uint8_t rx_buffer[SLIP_HANDLE_RX_BUFFER_LEN];
} slip_handle_config_t;
extern struct netif slipif;

extern slip_handle_config_t wifi_slip_config;
extern queue_handle_t slip_flow_control_queue;
bool slip_rx_filter(uint8_t *data, uint32_t len);
esp_err_t slip_modem_transmit_raw(void *buffer, size_t len);
flow_control_memory_t * slip_get_buffer();
int slip_fill_buffer(flow_control_memory_t * flow_control_memory,uint8_t * buffer,uint16_t len);
esp_err_t esp_slip_init(slip_handle_config_t *slip_modem);
/**
 * @brief slip_handle_uart_rx_task handle incoming slip packet
 * @param arg
 */
void slip_handle_uart_rx_task(void *arg);
/**
 * @brief slip_flow_control_task - handle queue for send packet to the slip tunel
 * use queue because the slip have lower speed then wifi
 * @param args
 */
void slip_flow_control_task(void *args);
esp_err_t slip_modem_deinit(slip_handle_config_t *slip_handle);
#endif //SLIP_HANDLE_H
