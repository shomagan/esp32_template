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
#include "esp_log.h"
#include "nvs_flash.h"
#include "esp_private/wifi.h"
#include "driver/uart.h"
#include "driver/gpio.h"
#include "sdkconfig.h"
#include "lwip/err.h"
#include "lwip/sys.h"
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
#include "modbus_tcp/modbus_tcp.h"
#include <driver/spi_master.h>
#include "udp_broadcast.h"
#include "sr04.h"
#include "sleep_control.h"
#include "wireless_control.h"
#include "battery_state.h"
#include "u8g2_esp32_hal.h"
#if TELEGRAM
#include "telegram.h"
#endif
extern void user_tasks_start(void *wakeup_cause);
/*end include*/

/* The examples use WiFi configuration that you can set via project configuration menu.
   If you'd rather not, just change the below entries to strings with
   the config you want - ie #define EXAMPLE_WIFI_SSID "mywifissid"
*/
#define CONFIG_RTS_GPIO (UART_PIN_NO_CHANGE)
#define CONFIG_CTS_GPIO (UART_PIN_NO_CHANGE)
#define CONFIG_UART_PORT_NUM      (2u)
#define CONFIG_UART_BAUD_RATE     (115200u)
#define CONFIG_IPV4 (1u)
#define BUF_SIZE (512u)

static const char *TAG = "wifi_slip_main";
slip_handle_config_t wifi_slip_config;

/*ssd1306*/

int main_init_tasks(esp_sleep_wakeup_cause_t * esp_sleep_wakeup_cause);
static int common_init_gpio(void);
static esp_sleep_wakeup_cause_t esp_sleep_wakeup_causes;
/**
 * @brief app_main
 */
void app_main(void){
    //Initialize NVS
    esp_err_t ret = mirror_storage_init();
    ESP_ERROR_CHECK(ret);
    ESP_ERROR_CHECK(esp_netif_init());
    ESP_ERROR_CHECK(esp_event_loop_create_default());
    wifi_slip_config.running = 0;
    wifi_slip_config.status = 0;
    wifi_slip_config.reserv0 = 0;
    wifi_slip_config.reserv1 = 1;
#if CONFIG_IDF_TARGET_ESP32
    wifi_slip_config.uart_dev = 2;
#else
    wifi_slip_config.uart_dev = 0;
#endif
    wifi_slip_config.uart_tx_pin = 17;
    wifi_slip_config.uart_rx_pin = 16;
#if CONFIG_IDF_TARGET_ESP32
    wifi_slip_config.uart_baud = 400000;
#else
    wifi_slip_config.uart_baud = 115200;
#endif
    wifi_slip_config.recv_buffer_len = 0;
    regs_init();
    dinamic_address = os_pool_create(&pool_dinamic_addr_def);
#if SLIP_ENABLE
    ESP_ERROR_CHECK(esp_slip_init(&wifi_slip_config));
#endif
    common_init_gpio();
    esp_sleep_wakeup_causes = esp_sleep_get_wakeup_causes();
#if DISPLAY
    init_display();
#endif
#if (LWIP_HTTPD_USE_SOCKS == 0)
    httpd_init_sofi();
#endif
    modbus_tcp_init();
    udp_broadcast_init();
    main_init_tasks(&esp_sleep_wakeup_causes);/*init all necessary tasks */
}

/**
 * @brief main_init_tasks - all task must starting here
 * @return
 */
int main_init_tasks(esp_sleep_wakeup_cause_t * esp_sleep_wakeup_cause){
    int res=0;
    res = task_create(wireless_control_task, "wireless_control_task", 3048, NULL, (tskIDLE_PRIORITY + 2), &wireless_control_handle_id);
    if (res != pdTRUE) {
        ESP_LOGE(TAG, "create wireless_control task failed");
    }
    res = task_create(common_duty_task, "common_duty_task", 3048, NULL, (tskIDLE_PRIORITY + 2), &common_duty_task_handle);
    if (res != pdTRUE) {
        ESP_LOGE(TAG, "create slip to wifi flow control task failed");
    }
#if SLIP_ENABLE
    res = task_create(slip_flow_control_task, "wifi2slip_flow_ctl", 2048, NULL, (tskIDLE_PRIORITY + 2), &wifi_slip_config.slip_flow_control_handle);
    if (res != pdTRUE) {
        ESP_LOGE(TAG, "create wifi to slip flow control task failed");
    }
    res = task_create(slip_handle_uart_rx_task, "slip_modem_uart_rx_task", SLIP_RX_TASK_STACK_SIZE, &wifi_slip_config, SLIP_RX_TASK_PRIORITY, &wifi_slip_config.uart_rx_task);
    if (res != pdTRUE) {
        ESP_LOGE(TAG, "create slip_modem_uart_rx_task failed");
    }
#endif /*SLIP_ENABLE*/

#if MODBUS_MASTER_ENABLE
    res = task_create(modbus_tcp_client_common_task, "modbus_tcp_client_common_task", 4096, NULL, (tskIDLE_PRIORITY + 2), &modbus_master_id);
    if(res != pdTRUE){
        main_printf(TAG,"modbus tcp task inited success\n");
    }
#endif
    /* Start all user tasks */
    user_tasks_start((void *)esp_sleep_wakeup_cause);
#if LWIP_HTTPD_USE_SOCKS
    res = task_create(http_sock_task, "http_sock_task", 2464, NULL, (tskIDLE_PRIORITY + 2), &http_sock_handle_id);
    if (res != pdTRUE) {
        main_printf(TAG, "create httpd_task failed");
    }
#endif /*LWIP_HTTPD_USE_SOCKS*/


#if TELEGRAM
    res = task_create(telegram_task, "telegram_task", 12000, (void *)esp_sleep_wakeup_cause, (tskIDLE_PRIORITY + 2), &telegram_handle_id);
    if(res != pdTRUE){
        main_printf(TAG,"telegram_task inited success\n");
    }

#if EPAPER
    res = task_create(epaper_task, "epaper_task", 2464, (void *)esp_sleep_wakeup_cause, (tskIDLE_PRIORITY + 2), &epaper_handle_id);
    if(res != pdTRUE){
        main_printf(TAG,"epaper_task inited success\n");
    }
#endif
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
#if ENABLE_DEEP_SLEEP && CONFIG_IDF_TARGET_ESP32
    /*set up pin to enter deep sleep mode - 25*/
    io_conf.intr_type = GPIO_INTR_DISABLE;
    io_conf.mode = GPIO_MODE_INPUT;
    io_conf.pin_bit_mask = (1ULL<<EXT_WAKEUP_PIN);
    io_conf.pull_down_en = 1;
    io_conf.pull_up_en = 0;
    gpio_config(&io_conf);
    gpio_get_level(EXT_WAKEUP_PIN);
#else
    const gpio_config_t config = {
        .pin_bit_mask = BIT(EXT_WAKEUP_PIN),
        .mode = GPIO_MODE_INPUT,
        .pull_up_en = 0,
        .pull_down_en = 1,
        .intr_type = GPIO_INTR_DISABLE
    };
    ESP_ERROR_CHECK(gpio_config(&config));
#endif /*ENABLE_DEEP_SLEEP*/
    return 0;
}
#endif //WIFI_SLIP_MAIN_C
