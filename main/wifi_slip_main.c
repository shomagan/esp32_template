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
#include "esp_sleep.h"
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
#include <driver/spi_master.h>
#include "udp_broadcast.h"
#include "touch_handle.h"
#include "di_handle.h"
#include "sr04.h"
/* The examples use WiFi configuration that you can set via project configuration menu.
   If you'd rather not, just change the below entries to strings with
   the config you want - ie #define EXAMPLE_WIFI_SSID "mywifissid"
*/
#define ESP_WIFI_CHANNEL   1u
#define MAX_STA_CONN       4u
#define CONFIG_RTS_GPIO (UART_PIN_NO_CHANGE)
#define CONFIG_CTS_GPIO (UART_PIN_NO_CHANGE)
#define CONFIG_UART_PORT_NUM      (2u)
#define CONFIG_UART_BAUD_RATE     (115200u)
#define CONFIG_IPV4 (1u)
#define BUF_SIZE (512u)
#define WAKE_UP_CONTROL_START_IS_NEEDED 0
#define WAKE_UP_CONTROL_END_IS_NEEDED 1
static const char *TAG = "wifi_slip_main";
static bool s_sta_is_connected = false;
slip_handle_config_t wifi_slip_config;
u8 sta_connected = 0;
u8g2_esp32_hal_t u8g2_esp32_hal = U8G2_ESP32_HAL_DEFAULT;
u8g2_t u8g2; // a structure which will contain all the data for one display
/*ssd1306*/
static int init_display(void);
int main_init_tasks(void);
static void wifi_event_handler(void* arg, esp_event_base_t event_base,
                                    int32_t event_id, void* event_data);
static void wifi_common_init(void);
static int common_init_gpio(void);
static void wifi_init_softap(void);
static bool wifi_init_sta(void);
static void wifi_init_soft_ap_sta(void);
static int wifi_init(void);
static int wake_up_control(void);
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
    vTaskDelay(1000 / portTICK_PERIOD_MS);
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
    wifi_slip_config.uart_baud = 400000;
    wifi_slip_config.recv_buffer_len = 0;
    regs_init();
    mirror_storage_init();
    dinamic_address = os_pool_create(&pool_dinamic_addr_def);
    preinit_global_vars();
    ESP_ERROR_CHECK(esp_slip_init(&wifi_slip_config));
    common_init_gpio();
    if(WAKE_UP_CONTROL_END_IS_NEEDED == wake_up_control()){
        prepare_to_sleep();
        esp_deep_sleep_start();
    }
    init_display();
    main_init_tasks();/*init all necessary tasks */
    wifi_init();
    httpd_init_sofi();
    modbus_tcp_init();
    udp_broadcast_init();
}

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
            break;
        default:
            break;
        }
    }else if(event_base==IP_EVENT){
        if (event_id==IP_EVENT_STA_GOT_IP){
            ip_event_got_ip_t *event = (ip_event_got_ip_t *) event_data;
            regs_global.vars.sta_connect = 1;
            memcpy(&regs_global.vars.sta_ip, &event->ip_info.ip.addr, sizeof(uint32_t));
            ESP_LOGI(TAG, "IP_EVENT_STA_GOT_IP");
        }
    }
}
/*
* @brief wake_up_control - control wake up
* @return non zero value if wake up is not needed
*/
static int wake_up_control(void){
    int result = WAKE_UP_CONTROL_START_IS_NEEDED;
    u32 deep_sleep_pin = 0;
    deep_sleep_pin = gpio_get_level(EXT_WAKEUP_PIN);
    if (0u==deep_sleep_pin){/*dont wake up if pressed less than 2.5 sec*/
        result = WAKE_UP_CONTROL_END_IS_NEEDED;
    }    
    return result;
}
/*
* @brief wifi_init - init wifi
* @return
*/
static int wifi_init(void){
    int result = 0;
    wifi_common_init();
    main_printf(TAG, "wifi setting %d",regs_global.vars.wifi_setting);
    main_printf(TAG, "ap_name:%s ap_password:%s sta_name:%s sta_password:%s channel:%d",
             regs_global.vars.wifi_name, regs_global.vars.wifi_password,
             regs_global.vars.wifi_router_name, regs_global.vars.wifi_router_password,ESP_WIFI_CHANNEL);
#if MAIN_CONFIG_WIFI_AP
    regs_global.vars.wifi_setting = WIFI_ACCESS_POINT;
#elif MAIN_CONFIG_WIFI_NODE
    regs_global.vars.wifi_setting = WIFI_CLIENT;
#endif
    if (regs_global.vars.wifi_setting == WIFI_ACCESS_POINT ||
        regs_global.vars.wifi_setting == WIFI_ESP32_CHANGED_ONLY_ACCESS_POINT){
        main_printf(TAG, "ESP_WIFI_MODE_AP");
        wifi_init_softap();
    }else if(regs_global.vars.wifi_setting == WIFI_CLIENT ||
             regs_global.vars.wifi_setting == WIFI_ESP32_CHANGED_ONLY_CLIENT){
        main_printf(TAG, "ESP_WIFI_MODE_STA");
        wifi_init_sta();
    }else if(regs_global.vars.wifi_setting == WIFI_AP_STA||
             regs_global.vars.wifi_setting == WIFI_ESP32_CHANGED_ONLY_AP_STA){
        main_printf(TAG, "ESP_WIFI_MODE_AP_STA");
        wifi_init_soft_ap_sta();
    }
    return result;
}
/**
 * @brief wifi_common_init - common wifi init
 */
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
    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK( esp_wifi_init(&cfg) );
    ESP_ERROR_CHECK( esp_event_handler_register(WIFI_EVENT, ESP_EVENT_ANY_ID, &wifi_event_handler, NULL));
    ESP_ERROR_CHECK( esp_event_handler_register(IP_EVENT, IP_EVENT_STA_GOT_IP, &wifi_event_handler, NULL));
    ESP_ERROR_CHECK( esp_wifi_set_storage(WIFI_STORAGE_RAM));
    ESP_ERROR_CHECK( esp_wifi_set_mode(WIFI_MODE_NULL));
}
/**
 * @brief wifi_init_softap - init softap
 */
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
static bool wifi_init_sta(void){
    wifi_config_t sta_config;
    memset(&sta_config, 0,sizeof(wifi_config_t));
    strcpy((char *)sta_config.sta.ssid, (char*)regs_global.vars.wifi_router_name);
    strcpy((char *)sta_config.sta.password, (char*)regs_global.vars.wifi_router_password);
    main_printf(TAG, "ap_name:%s ap_password:%s ",
             sta_config.sta.ssid, sta_config.sta.password);
    ESP_ERROR_CHECK( esp_wifi_set_mode(WIFI_MODE_STA) );
    ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_STA, &sta_config));
    ESP_ERROR_CHECK(esp_wifi_start());
    ESP_ERROR_CHECK( esp_wifi_connect() );
    return 0;
}

static int init_display(){
    int res=0;
    gpio_config_t io_conf = {0};
    //disable interrupt
    io_conf.intr_type = GPIO_INTR_DISABLE;
    //set as output mode
    io_conf.mode = GPIO_MODE_OUTPUT;
    //bit mask of the pins that you want to set,e.g.GPIO18/19
    io_conf.pin_bit_mask = 1<<PIN_RESET;
    //disable pull-down mode
    io_conf.pull_down_en = 0;
    //disable pull-up mode
    io_conf.pull_up_en = 0;
    //configure GPIO with the given settings
    gpio_config(&io_conf);
    gpio_set_level(PIN_RESET, 0);
    gpio_set_level(PIN_RESET, 1);
    u8g2_esp32_hal.sda   = PIN_SDA;
    u8g2_esp32_hal.scl  = PIN_SCL;
    u8g2_esp32_hal_init(u8g2_esp32_hal);
    u8g2_Setup_ssd1306_i2c_128x32_univision_f(
        &u8g2,
        U8G2_R0,
        //u8x8_byte_sw_i2c,
        u8g2_esp32_i2c_byte_cb,
        u8g2_esp32_gpio_and_delay_cb);  // init u8g2 structure
    u8x8_SetI2CAddress(&u8g2.u8x8,0x78);
    regs_global.vars.i2c_display_address = 0x78>>1;
    u8g2_InitDisplay(&u8g2); // send init sequence to the display, display is in sleep mode after this,
    main_printf(TAG, "init display passed");
    u8g2_SetPowerSave(&u8g2, 0); // wake up display
    main_printf(TAG, "set power save passed");
    u8g2_ClearBuffer(&u8g2);
    char address[64] = {0};
    sprintf(address,"mdb: %u, ip: %u.%u.%u.%u",regs_global.vars.mdb_addr,regs_global.vars.sta_ip[0],regs_global.vars.sta_ip[1],regs_global.vars.sta_ip[2],regs_global.vars.sta_ip[3]);
    u8g2_SetFont(&u8g2, u8g2_font_5x8_tf);
    u8g2_DrawStr(&u8g2, 0,7, address);
    u8g2_SendBuffer(&u8g2);
    return res;
}
/**
 * @brief main_init_tasks - all task must starting here
 * @return
 */
int main_init_tasks(){
    int res=0;
    res = task_create(slip_flow_control_task, "wifi2slip_flow_ctl", 2048, NULL, (tskIDLE_PRIORITY + 2), &wifi_slip_config.slip_flow_control_handle);
    if (res != pdTRUE) {
        ESP_LOGE(TAG, "create wifi to slip flow control task failed");
    }
    res = task_create(common_duty_task, "common_duty_task", 3048, NULL, (tskIDLE_PRIORITY + 2), &common_duty_task_handle);
    if (res != pdTRUE) {
        ESP_LOGE(TAG, "create slip to wifi flow control task failed");
    }
    res = task_create(slip_handle_uart_rx_task, "slip_modem_uart_rx_task", SLIP_RX_TASK_STACK_SIZE, &wifi_slip_config, SLIP_RX_TASK_PRIORITY, &wifi_slip_config.uart_rx_task);
    if (res != pdTRUE) {
        ESP_LOGE(TAG, "create slip_modem_uart_rx_task failed");
    }
#if PWM_TEST_ENABLE
    res = task_create(pwm_control_task, "pwm_control_task", 2048, &wifi_slip_config, (tskIDLE_PRIORITY + 2), &pwm_task_handle);
    if (res != pdTRUE) {
        ESP_LOGE(TAG, "create pwm_control_task failed");
    }
#endif
#if TOUCH_HANDLE_ENABLE
    res = task_create(touch_task, "touch_task", 4096, NULL, (tskIDLE_PRIORITY + 2), &touch_task_handle);
    if (res != pdTRUE) {
        ESP_LOGE(TAG, "create touch_handle_task failed");
    }
#endif
#if MODBUS_MASTER_ENABLE
    res = task_create(modbus_tcp_client_common_task, "modbus_tcp_client_common_task", 4096, NULL, (tskIDLE_PRIORITY + 2), &modbus_master_id);
    if(res != pdTRUE){
        main_printf(TAG,"modbus tcp task inited success\n");
    }
#endif
#if DI_HANDLING_ENABLE
    res = task_create(di_handle_task, "di_handle_task", 2048, NULL, (tskIDLE_PRIORITY + 2), &di_handle_id);
    if(res != pdTRUE){
        main_printf(TAG,"di_handle_task inited success\n");
    }
#endif
#if SR04_HANDLING_ENABLE
    res = task_create(sr04_task, "sr04_task", 2464, NULL, (tskIDLE_PRIORITY + 3), &sr04_handle_id);
    if(res != pdTRUE){
        main_printf(TAG,"sr04_task inited success\n");
    }

#endif
    return res;
}
/**
 * @brief common_init_gpio - init the common(system, like LED) use pins
 * @return
 */
static int common_init_gpio(void){
    gpio_config_t io_conf = {0};
    //disable interrupt
    io_conf.intr_type = GPIO_INTR_DISABLE;
    //set as output mode
    io_conf.mode = GPIO_MODE_OUTPUT;
    //bit mask of the pins that you want to set,e.g.GPIO18/19
    io_conf.pin_bit_mask = 1<<CONFIG_LED_BLINK_GPIO;
    //disable pull-down mode
    io_conf.pull_down_en = 0;
    //disable pull-up mode
    io_conf.pull_up_en = 0;
    //configure GPIO with the given settings
    gpio_config(&io_conf);
#if ENABLE_DEEP_SLEEP
    /*set up pin to enter deep sleep mode - 25*/
    io_conf.intr_type = GPIO_INTR_DISABLE;
    io_conf.mode = GPIO_MODE_INPUT;
    io_conf.pin_bit_mask = (1ULL<<EXT_WAKEUP_PIN);
    io_conf.pull_down_en = 1;
    io_conf.pull_up_en = 0;
    gpio_config(&io_conf);
    gpio_get_level(EXT_WAKEUP_PIN);
#endif /*ENABLE_DEEP_SLEEP*/
    return 0;
}
#endif //WIFI_SLIP_MAIN_C
