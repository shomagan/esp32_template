#ifndef WIFI_SLIP_MAIN_C
#define WIFI_SLIP_MAIN_C
/*  WiFi wifi_slip Example
   This example code is in the Public Domain (or CC0 licensed, at your option.)
   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/
#include <string.h>
#include "wifi_slip_main.h"
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
#include "pin_map.h"
#include "common.h"
#include "mirror_storage.h"
#include "memory_handle.h"
#include "modbus.h"
#include "httpd.h"
#include "pwm_test.h"
#include "modbus_tcp/modbus_tcp.h"
#include <string.h>
/* The examples use WiFi configuration that you can set via project configuration menu.
   If you'd rather not, just change the below entries to strings with
   the config you want - ie #define EXAMPLE_WIFI_SSID "mywifissid"
*/
#define ESP_WIFI_CHANNEL   1
#define MAX_STA_CONN       4
#define CONFIG_RTS_GPIO (UART_PIN_NO_CHANGE)
#define CONFIG_CTS_GPIO (UART_PIN_NO_CHANGE)
#define CONFIG_UART_PORT_NUM      (2)
#define CONFIG_UART_BAUD_RATE     (115200)
#define CONFIG_IPV4 (1)
#define BUF_SIZE (512)
static const char *TAG = "wifi_slip_main";
static bool s_sta_is_connected = false;
slip_handle_config_t wifi_slip_config;
u8 sta_connected = 0;

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
    (void)(event_data);

    if (event_base==WIFI_EVENT){
        switch (event_id) {
        case WIFI_EVENT_AP_STACONNECTED:
            ESP_LOGI(TAG, "Wi-Fi AP got a station connected");
            if (!regs_global.vars.ap_connections_number) {
                s_sta_is_connected = true;
            }
            regs_global.vars.ap_connections_number++;
            break;
        case WIFI_EVENT_AP_STADISCONNECTED:
            ESP_LOGI(TAG, "Wi-Fi AP got a station disconnected");
            if (regs_global.vars.ap_connections_number){
                regs_global.vars.ap_connections_number--;
            }
            if (!regs_global.vars.ap_connections_number) {
                s_sta_is_connected = false;
            }
            break;
        case WIFI_EVENT_STA_DISCONNECTED:
            ESP_LOGI(TAG, "Wi-Fi sta got a station disconnected");
            regs_global.vars.sta_connect=0;
            esp_wifi_connect();
            break;
        default:
            break;
        }
    }else if(event_base==IP_EVENT){
        if (event_id==IP_EVENT_STA_GOT_IP){
            regs_global.vars.sta_connect=1;
            ESP_LOGI(TAG, "IP_EVENT_STA_GOT_IP");
        }
    }
}
static void wifi_common_init(void){
    static bool initialized = false;
    if (initialized) {
        return;
    }
    esp_netif_t *ap_netif = esp_netif_create_default_wifi_ap();
    if(ap_netif==NULL){
        ESP_LOGE(TAG, "ESP_WIFI_MODE_AP");
    }
    esp_netif_t *sta_netif = esp_netif_create_default_wifi_sta();
    if(sta_netif==NULL){
        ESP_LOGE(TAG, "ESP_WIFI_MODE_AP");
    }
    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT()
    ESP_ERROR_CHECK( esp_wifi_init(&cfg) );
    ESP_ERROR_CHECK( esp_event_handler_register(WIFI_EVENT, ESP_EVENT_ANY_ID, &wifi_event_handler, NULL));
    ESP_ERROR_CHECK( esp_event_handler_register(IP_EVENT, IP_EVENT_STA_GOT_IP, &wifi_event_handler, NULL));
    ESP_ERROR_CHECK( esp_wifi_set_storage(WIFI_STORAGE_RAM));
    ESP_ERROR_CHECK( esp_wifi_set_mode(WIFI_MODE_NULL));
}
static void wifi_init_soft_ap_sta(void){
    wifi_config_t ap_config;
    memset(&ap_config,0,sizeof(wifi_config_t));
    memset(ap_config.ap.ssid,0,sizeof(ap_config.ap.ssid));
    memset(ap_config.ap.password,0,sizeof(ap_config.ap.password));
    memcpy(ap_config.ap.ssid,regs_global.vars.wifi_name,WIFI_NAME_LEN);
    memcpy(ap_config.ap.password,regs_global.vars.wifi_password,WIFI_PASSWORD_LEN);
    ap_config.ap.authmode = WIFI_AUTH_WPA2_PSK;
    ap_config.ap.ssid_len = WIFI_NAME_LEN;
    ap_config.ap.max_connection = MAX_STA_CONN;
    ap_config.ap.channel = ESP_WIFI_CHANNEL;
    wifi_config_t sta_config;
    memset(&sta_config,0,sizeof(wifi_config_t));
    strcpy((char *)sta_config.sta.ssid, (char*)regs_global.vars.wifi_router_name);
    strcpy((char *)sta_config.sta.password, (char*)regs_global.vars.wifi_router_password);
    ESP_ERROR_CHECK( esp_wifi_set_mode(WIFI_MODE_APSTA) );
    ESP_ERROR_CHECK( esp_wifi_set_config(WIFI_IF_AP, &ap_config) );
    ESP_ERROR_CHECK( esp_wifi_set_config(WIFI_IF_STA, &sta_config) );
    ESP_ERROR_CHECK( esp_wifi_start() );
    ESP_LOGI(TAG, "WIFI_MODE_AP started. SSID:%s password:%s channel:%d",
             ap_config.ap.ssid, ap_config.ap.password, ESP_WIFI_CHANNEL);
    ESP_ERROR_CHECK( esp_wifi_connect() );
    return;
}
/**
 * @brief wifi_init_softap
 */
void wifi_init_softap(void){
    static uint8_t temp_ssid[32] = {0};
    memcpy(temp_ssid,regs_global.vars.wifi_name,WIFI_NAME_LEN);
    wifi_config_t wifi_config = {
        .ap = {
            .ssid_len = WIFI_NAME_LEN,
            .channel = ESP_WIFI_CHANNEL,
            .max_connection = MAX_STA_CONN,
            .authmode = WIFI_AUTH_WPA2_PSK
        },
    };
    memset(wifi_config.ap.ssid,0,sizeof(wifi_config.ap.ssid));
    memset(wifi_config.ap.password,0,sizeof(wifi_config.ap.password));
    memcpy(wifi_config.ap.ssid,regs_global.vars.wifi_name,WIFI_NAME_LEN);
    memcpy(wifi_config.ap.password,regs_global.vars.wifi_password,WIFI_PASSWORD_LEN);
    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_AP));
    ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_AP, &wifi_config));
    ESP_ERROR_CHECK(esp_wifi_start());
    ESP_LOGI(TAG, "wifi_init_softap finished. SSID:%s password:%s channel:%d",
             regs_global.vars.wifi_name, regs_global.vars.wifi_password, ESP_WIFI_CHANNEL);
}
static bool wifi_init_sta(){
    wifi_config_t wifi_config;
    memset(&wifi_config, 0,sizeof(wifi_config_t));
    strcpy((char *)wifi_config.sta.ssid, (char*)regs_global.vars.wifi_router_name);
    strcpy((char *)wifi_config.sta.password, (char*)regs_global.vars.wifi_password);
    ESP_ERROR_CHECK( esp_wifi_set_mode(WIFI_MODE_STA) );
    ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_STA, &wifi_config));
    ESP_ERROR_CHECK( esp_wifi_connect() );
    return 0;
}

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
    ESP_ERROR_CHECK(esp_slip_init(&wifi_slip_config));
    regs_init();
    mirror_storage_init();
    dinamic_address = os_pool_create(&pool_dinamic_addr_def);
    common_init();/*init common things*/
    common_init_tasks();/*init all necessary tasks */
    preinit_global_vars();
    pwm_test_init();
    wifi_common_init();
    if (regs_global.vars.wifi_setting == WIFI_ACCESS_POINT){
        ESP_LOGI(TAG, "ESP_WIFI_MODE_AP");
        wifi_init_softap();
    }else if(regs_global.vars.wifi_setting == WIFI_CLIENT){
        ESP_LOGI(TAG, "ESP_WIFI_MODE_STA");
        wifi_init_sta();
    }else if(regs_global.vars.wifi_setting == WIFI_AP_STA){
        ESP_LOGI(TAG, "ESP_WIFI_MODE_AP_STA");
        wifi_init_soft_ap_sta();
    }

    httpd_init_sofi();
    modbus_tcp_init();
}

#endif //WIFI_SLIP_MAIN_C
