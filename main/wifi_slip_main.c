#ifndef WIFI_SLIP_MAIN_C
#define WIFI_SLIP_MAIN_C
/*  WiFi wifi_slip Example
   This example code is in the Public Domain (or CC0 licensed, at your option.)
   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/
#include <string.h>
#include "type_def.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "freertos/semphr.h"
#include "esp_system.h"
#include "esp_wifi.h"
#include "esp_event.h"
#include "esp_log.h"
#include "nvs_flash.h"
#include "esp_private/wifi.h"
#include "driver/uart.h"
#include "driver/gpio.h"
#include "sdkconfig.h"
#include "lwip/err.h"
#include "lwip/sys.h"
#include "esp_wifi_types.h"
#include "esp_netif.h"
#include "lwip/sockets.h"
#include "slip_handle.h"
#include "regs.h"

const u8 def_ip[4] = {192,168,88,2};
const u8 def_netmask[4] = {255,255,255,0};
const u8 def_gateaway[4] = {192,168,88,1};
const u8 def_slip_ip[4] = {172,16,1,232};
const u8 def_slip_netmask[4] = {255,255,255,0};
const u8 def_slip_gateaway[4] = {172,16,1,1};
const u8 def_board_ver = BOARD_VERSION;
const u8 def_wifi_name[WIFI_NAME_LEN] = "bric_plc_net";               //!<"must be strong full filled", &save &def
const u8 def_wifi_password[WIFI_PASSWORD_LEN] = "bricsofi";            //!<"must be strong more then 8 byte", &save &def
const u16 def_wifi_setting = WIFI_ACCESS_POINT;               //!<"type of wifi and settings" &save &def

/* The examples use WiFi configuration that you can set via project configuration menu.
   If you'd rather not, just change the below entries to strings with
   the config you want - ie #define EXAMPLE_WIFI_SSID "mywifissid"
*/
#define ESP_WIFI_SSID      "sofi_network"
#define ESP_WIFI_PASS      "bric_sofi"
#define ESP_WIFI_CHANNEL   1
#define MAX_STA_CONN       4
#define CONFIG_TXD_GPIO (17)
#define CONFIG_RXD_GPIO (16)
#define CONFIG_RTS_GPIO (UART_PIN_NO_CHANGE)
#define CONFIG_CTS_GPIO (UART_PIN_NO_CHANGE)
#define CONFIG_UART_PORT_NUM      (2)
#define CONFIG_UART_BAUD_RATE     (115200)
#define CONFIG_IPV4 (1)
#define BUF_SIZE (512)
static SemaphoreHandle_t flow_control_wifi_mutex = NULL;
static const char *TAG = "wifi softAP with slip";
static bool s_sta_is_connected = false;
static flow_control_memory_t flow_control_wifi_memory[WIFI_FLOW_CONTROL_QUEUE_LENGTH];
xQueueHandle wifi_flow_control_queue = NULL;
slip_handle_config_t wifi_slip_config;

// Forward packets from Wi-Fi to slip
/*static esp_err_t pkt_wifi2slip(void *buffer, uint16_t len, void *eb){
    if (len < WIFI_SLIP_MAX_IP_PACKET){
        flow_control_memory_t * buffer_number = slip_get_buffer();
        if (buffer_number!=NULL){
            slip_fill_buffer(buffer_number, buffer, len);
            flow_control_msg_t msg = {
                .packet = buffer_number->packet,
                .length = &buffer_number->length
            };
            if(xQueueSend(slip_flow_control_queue, &msg, pdMS_TO_TICKS(WIFI_FLOW_CONTROL_QUEUE_TIMEOUT_MS)) != pdTRUE){
                ESP_LOGE(TAG, "send flow control message failed or timeout");
                buffer_number->length = 0;
            }
        }
        esp_wifi_internal_free_rx_buffer(eb);
    }
    return ESP_OK;
}*/

/**
 * @brief wifi_event_handler
 * @param arg
 * @param event_base
 * @param event_id
 * @param event_data
 */
static void wifi_event_handler(void* arg, esp_event_base_t event_base,
                                    int32_t event_id, void* event_data){
    (void)(arg);
    (void)(event_base);
    (void)(event_data);
    static uint8_t s_con_cnt = 0;
    switch (event_id) {
    case WIFI_EVENT_AP_STACONNECTED:
        ESP_LOGI(TAG, "Wi-Fi AP got a station connected");
        if (!s_con_cnt) {
            s_sta_is_connected = true;
        }
        s_con_cnt++;
        break;
    case WIFI_EVENT_AP_STADISCONNECTED:
        ESP_LOGI(TAG, "Wi-Fi AP got a station disconnected");
        s_con_cnt--;
        if (!s_con_cnt) {
            s_sta_is_connected = false;
        }
        break;
    default:
        break;
    }
}

/*static void wifi_flow_control_task(void *args){
    (void)(args);
    flow_control_msg_t msg;
    int res = 0;
    uint32_t timeout = 0;
    for (uint8_t i=0; i < WIFI_FLOW_CONTROL_QUEUE_LENGTH; i++){
        flow_control_wifi_memory[i].length = 0;
    }
    ESP_LOGI(TAG, "wifi_flow_control_task start");
    uint32_t ticks = 0;
    while (1) {
        if (xQueueReceive(wifi_flow_control_queue, &msg, pdMS_TO_TICKS(WIFI_FLOW_CONTROL_QUEUE_TIMEOUT_MS)) == pdTRUE) {
            timeout = 0;
            if (s_sta_is_connected && *msg.length) {
                do {
                    vTaskDelay(pdMS_TO_TICKS(10));
                    timeout += 10;
                    res = esp_wifi_internal_tx(WIFI_IF_AP, msg.packet, *msg.length);
                } while (res && timeout < WIFI_FLOW_CONTROL_WIFI_SEND_TIMEOUT_MS);
                if (res != ESP_OK) {
                    ESP_LOGE(TAG, "WiFi send packet failed: %d len - %u timeout - %u", res, *msg.length, timeout);
                }
            }
            *msg.length = 0;

        }
        ticks++;
    }
    vTaskDelete(NULL);
}*/

/**
 * @brief wifi_init_softap
 */
void wifi_init_softap(void){
    esp_netif_create_default_wifi_ap();
    ESP_ERROR_CHECK(esp_event_handler_register(WIFI_EVENT, ESP_EVENT_ANY_ID, wifi_event_handler, NULL));
    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT()
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));
    ESP_ERROR_CHECK(esp_wifi_set_storage(WIFI_STORAGE_RAM));


    wifi_config_t wifi_config = {
        .ap = {
            .ssid = ESP_WIFI_SSID,
            .ssid_len = strlen(ESP_WIFI_SSID),
            .channel = ESP_WIFI_CHANNEL,
            .password = ESP_WIFI_PASS,
            .max_connection = MAX_STA_CONN,
            .authmode = WIFI_AUTH_WPA2_PSK
        },
    };
    if (strlen(ESP_WIFI_PASS) == 0) {
        wifi_config.ap.authmode = WIFI_AUTH_OPEN;
    }
    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_AP));
    ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_AP, &wifi_config));
    ESP_ERROR_CHECK(esp_wifi_start());
    ESP_LOGI(TAG, "wifi_init_softap finished. SSID:%s password:%s channel:%d",
             ESP_WIFI_SSID, ESP_WIFI_PASS, ESP_WIFI_CHANNEL);
}
static esp_err_t wifi_initialize_flow_control(void){
    vSemaphoreCreateBinary(flow_control_wifi_mutex)
    wifi_flow_control_queue = xQueueCreate(WIFI_FLOW_CONTROL_QUEUE_LENGTH, sizeof(flow_control_msg_t));
    if (!wifi_flow_control_queue) {
        ESP_LOGE(TAG, "create slip to wifi flow control queue failed");
        return ESP_FAIL;
    }
    /*BaseType_t ret = xTaskCreate(wifi_flow_control_task, "wifi_flow_ctl", 2048, NULL, (tskIDLE_PRIORITY + 2), wifi_slip_config.wifi_flow_control_handle);
    if (ret != pdTRUE) {
        ESP_LOGE(TAG, "create slip to wifi flow control task failed");
        return ESP_FAIL;
    }*/
    return ESP_OK;
}


/*#if CONFIG_IPV4
static const esp_netif_ip_info_t s_slip_ip4 = {
        .ip = { .addr = ESP_IP4TOADDR( 172, 16, 3, 1) },
};
#endif
*/

flow_control_memory_t * wifi_get_buffer(){
    flow_control_memory_t * flow_control_memory = NULL;
    xSemaphoreTake(flow_control_wifi_mutex,1000);
    for (uint16_t i =0;i<WIFI_FLOW_CONTROL_QUEUE_LENGTH;i++){
        if (flow_control_wifi_memory[i].length == 0){
            flow_control_memory = &flow_control_wifi_memory[i];
        }
    }
    xSemaphoreGive(flow_control_wifi_mutex);
    return flow_control_memory;
}
int wifi_fill_buffer(flow_control_memory_t * flow_control_memory,uint8_t * buffer,uint16_t len){
    if ((flow_control_memory!=NULL) && (len < WIFI_SLIP_MAX_IP_PACKET)){
        xSemaphoreTake(flow_control_wifi_mutex,1000);
        memcpy(flow_control_memory->packet,buffer,len);
        memcpy(&flow_control_memory->length,&len,sizeof(len));
        xSemaphoreGive(flow_control_wifi_mutex);
    }else{
        return -1;
    }
    return 0;
}
#endif //WIFI_SLIP_MAIN_C

/**
 * @brief app_main
 */
void app_main(void){
    //Initialize NVS
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND){
      ESP_ERROR_CHECK(nvs_flash_erase());
      ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);
    ESP_ERROR_CHECK(esp_netif_init());
    ESP_ERROR_CHECK(esp_event_loop_create_default());
    wifi_slip_config.running = 0;
    wifi_slip_config.status = 0;
    wifi_slip_config.reserv0 = 0;
    wifi_slip_config.reserv1 = 1;
    wifi_slip_config.uart_dev = 2;
    wifi_slip_config.uart_tx_pin = 17;
    wifi_slip_config.uart_rx_pin = 16;
    wifi_slip_config.uart_baud = 115200;
    wifi_slip_config.recv_buffer_len = 0;
    ESP_ERROR_CHECK(esp_slip_start(&wifi_slip_config));
    ESP_ERROR_CHECK(wifi_initialize_flow_control());
    ESP_LOGI(TAG, "ESP_WIFI_MODE_AP");
    wifi_init_softap();
}
int read_parameters_from_nvs(){
    // Open
    printf("\n");
    printf("Opening Non-Volatile Storage (NVS) handle... ");
    nvs_handle_t my_handle;
    esp_err_t err = nvs_open("storage", NVS_READWRITE, &my_handle);
    if (err != ESP_OK) {
        printf("Error (%s) opening NVS handle!\n", esp_err_to_name(err));
    } else {
        printf("Done\n");
        // Read
        printf("Reading restart counter from NVS ... ");
        uint32_t restart_counter = 0; // value will default to 0, if not set yet in NVS
        err = nvs_get_u32(my_handle, "restart_counter", &restart_counter);
        switch (err) {
            case ESP_OK:
                printf("Done\n");
                printf("Restart counter = %d\n", restart_counter);
                break;
            case ESP_ERR_NVS_NOT_FOUND:
                printf("The value is not initialized yet!\n");
                break;
            default :
                printf("Error (%s) reading!\n", esp_err_to_name(err));
        }
        // Write
        printf("Updating restart counter in NVS ... ");
        restart_counter++;
        err = nvs_set_i32(my_handle, "restart_counter", restart_counter);
        printf((err != ESP_OK) ? "Failed!\n" : "Done\n");
        // Commit written value.
        // After setting any values, nvs_commit() must be called to ensure changes are written
        // to flash storage. Implementations may write to storage at other times,
        // but this is not guaranteed.
        printf("Committing updates in NVS ... ");
        err = nvs_commit(my_handle);
        printf((err != ESP_OK) ? "Failed!\n" : "Done\n");
        // Close
        nvs_close(my_handle);
    }
    printf("\n");
    // Restart module
    for (int i = 10; i >= 0; i--) {
        printf("Restarting in %d seconds...\n", i);
        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
    printf("Restarting now.\n");
    fflush(stdout);
    esp_restart();
}
