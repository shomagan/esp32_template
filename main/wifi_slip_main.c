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
#include "pwm_test.h"
#include "modbus_tcp/modbus_tcp.h"
#include <driver/spi_master.h>
#include "udp_broadcast.h"
#include "touch_handle.h"
#include "di_handle.h"
#include "sr04.h"
#include "step_motor.h"
#include "sleep_control.h"
#include "wireless_control.h"
#include "feeder.h"
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
u8g2_esp32_hal_t u8g2_esp32_hal = U8G2_ESP32_HAL_DEFAULT;
u8g2_t u8g2; // a structure which will contain all the data for one display
/*ssd1306*/
#if SS1306_MODULE
static int init_display(void);
#endif
int main_init_tasks(esp_sleep_wakeup_cause_t * esp_sleep_wakeup_cause);
static int common_init_gpio(void);
static esp_sleep_wakeup_cause_t esp_sleep_wakeup_cause;
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
    mirror_storage_init();
    dinamic_address = os_pool_create(&pool_dinamic_addr_def);
    preinit_global_vars();
#if SLIP_ENABLE    
    ESP_ERROR_CHECK(esp_slip_init(&wifi_slip_config));
#endif
    common_init_gpio();
    esp_sleep_wakeup_cause = esp_sleep_get_wakeup_cause(); 
#if SS1306_MODULE
    init_display();
#endif
    httpd_init_sofi();
    modbus_tcp_init();
    udp_broadcast_init();
    main_init_tasks(&esp_sleep_wakeup_cause);/*init all necessary tasks */
}


#if SS1306_MODULE
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
#endif
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
#if PWM_TEST_ENABLE
    res = task_create(pwm_control_task, "pwm_control_task", 2048, NULL, (tskIDLE_PRIORITY + 2), &pwm_task_handle);
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
#if SR04_MODULE
    res = task_create(sr04_task, "sr04_task", 2464, NULL, (tskIDLE_PRIORITY + 3), &sr04_handle_id);
    if(res != pdTRUE){
        main_printf(TAG,"sr04_task inited success\n");
    }
#endif
#if STEP_MOTOR
    res = task_create(step_motor_task, "step_motor_task", 2464, NULL, (tskIDLE_PRIORITY + 2), &step_motor_handle_id);
    if(res != pdTRUE){
        main_printf(TAG,"step_motor_task inited success\n");
    }
#endif
#if ENABLE_DEEP_SLEEP
    res = task_create(sleep_control_task, "sleep_control_task", 2464, (void *)esp_sleep_wakeup_cause, (tskIDLE_PRIORITY + 2), &sleep_control_handle_id);
    if(res != pdTRUE){
        main_printf(TAG,"sleep_control_task inited success\n");
    }
#endif
#if FEEDER
    res = task_create(feeder_task, "feeder_task", 2464, (void *)esp_sleep_wakeup_cause, (tskIDLE_PRIORITY + 2), &feeder_handle_id);
    if(res != pdTRUE){
        main_printf(TAG,"feeder_task inited success\n");
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
