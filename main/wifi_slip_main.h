#ifndef WIFI_SLIP_MAIN_H
#define WIFI_SLIP_MAIN_H
#include <stdbool.h>
#include <stdint.h>

#define WIFI_SLIP_MAX_IP_PACKET (536+128)
#define SLIP_MAX_RAW_PACKET WIFI_SLIP_MAX_IP_PACKET*2

/**
  *use for slip to wifi queue
  * */
typedef struct {
    uint8_t * packet;
    uint16_t * length;
} flow_control_msg_t;
/**
  *use for slip to wifi queue
  * */
typedef struct {
    uint8_t packet[WIFI_SLIP_MAX_IP_PACKET];
    uint16_t length;
} flow_control_memory_t;

#define WIFI_FLOW_CONTROL_QUEUE_TIMEOUT_MS (100)
#define WIFI_FLOW_CONTROL_QUEUE_LENGTH (20)
#define WIFI_FLOW_CONTROL_WIFI_SEND_TIMEOUT_MS (100)

#define SLIP_FLOW_CONTROL_QUEUE_TIMEOUT_MS (100)
#define SLIP_FLOW_CONTROL_QUEUE_LENGTH (20)
#define SLIP_FLOW_CONTROL_WIFI_SEND_TIMEOUT_MS (100)
extern xQueueHandle wifi_flow_control_queue;

flow_control_memory_t * wifi_get_buffer();
int wifi_fill_buffer(flow_control_memory_t * flow_control_memory,uint8_t * buffer,uint16_t len);
#endif //WIFI_SLIP_MAIN_H
