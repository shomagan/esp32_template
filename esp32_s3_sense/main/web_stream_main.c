/*
  XIAO ESP32-S3 Sense - MJPEG HTTP stream example for ESP-IDF v6.0
  Pins filled for Seeed XIAO ESP32-S3 Sense (from your mapping).

  Before build:
  - Replace WIFI_SSID / WIFI_PASS with your network credentials (or load from
  sdkconfig).
  - If your camera sensor is NOT OV2640, change CAMERA_MODEL accordingly.
  - In menuconfig: set target to esp32s3, enable PSRAM (Component config -> ESP
  System -> Support for external RAM).
  - Build & flash: idf.py set-target esp32s3 build flash monitor
*/

#include "driver/gpio.h"
#include "esp_log.h"
#include "esp_system.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "nvs_flash.h"
#include <string.h>

#include "esp_event.h"
#include "esp_http_server.h"
#include "esp_netif.h"
#include "esp_wifi.h"

#include "esp_camera.h"

static const char *TAG = "xiao_stream";

/* ====== USER CONFIG - EDIT THESE ====== */
#define WIFI_SSID "lastik_shoma"
#define WIFI_PASS "Cactus_campus16"

/* Camera pin defines filled from your list */
#define PIN_PWDN -1     // power down (not used)
#define PIN_RESET -1    // reset (not used)
#define PIN_XCLK 10     // Camera-related clock pin (GPIO10)
#define PIN_SIOD 40     // SCCB SDA (I2C data) (GPIO40)
#define PIN_SIOC 39     // SCCB SCL (I2C clock) (GPIO39)
#define PIN_Y9 48       // Camera data Y9 (GPIO48)
#define PIN_Y8 11       // Camera data Y8 (GPIO11)
#define PIN_Y7 12       // Camera data Y7 (GPIO12)
#define PIN_Y6 14       // Camera data Y6 (GPIO14)
#define PIN_Y5 16       // Camera data Y5 (GPIO16)
#define PIN_Y4 18       // Camera data Y4 (GPIO18)
#define PIN_Y3 17       // Camera data Y3 (GPIO17)
#define PIN_Y2 15       // Camera data Y2 (GPIO15)
#define PIN_VSYNC 38    // VSYNC (GPIO38)
#define PIN_HREF 47     // HREF (GPIO47)
#define PIN_PCLK 13     // PCLK (GPIO13)
#define PIN_USER_LED 21 // User LED (GPIO21)

/* Choose frame size / quality */
#define FRAME_SIZE FRAMESIZE_VGA // adjust if you need smaller/larger
#define JPEG_QUALITY 10          // 0-63 lower number = higher quality
#define FB_COUNT 2

/* Camera sensor model - set to the sensor your board uses (assumed OV2640 here)
 */
#ifndef CAMERA_MODEL
#define CAMERA_MODEL CAMERA_MODEL_OV2640
#endif
/* ====== END USER CONFIG ====== */
#define task_get_id xTaskGetCurrentTaskHandle
#define task_handle_t TaskHandle_t
task_handle_t wireless_control_handle_id = NULL;

/* MJPEG stream MIME parts */
static const char *_STREAM_CONTENT_TYPE =
    "multipart/x-mixed-replace;boundary=frame";
static const char *_STREAM_BOUNDARY = "\r\n--frame\r\n";
static const char *_STREAM_PART =
    "Content-Type: image/jpeg\r\nContent-Length: %u\r\n\r\n";

static inline BaseType_t task_create(TaskFunction_t function, const char *name,
                                     uint32_t stack_size, void *param,
                                     UBaseType_t prio, TaskHandle_t *handler) {
  return xTaskCreate(function, name, stack_size, param, prio, handler);
}
static inline void task_delete(TaskHandle_t handler) { vTaskDelete(handler); }
static inline BaseType_t task_notify_wait(uint32_t flags, uint32_t *signal,
                                          uint32_t ms) {
  return xTaskNotifyWait(0, flags, signal, ms / portTICK_PERIOD_MS);
}

enum wireless_control_task_event {
  WIRELESS_TASK_STOP_CHILD_PROCCES =
      (1 << 0),                        /*!< sended before kill external thread*/
  WIRELESS_TASK_STOP_WIFI = (1 << 1),  /*!< disable WIFI - stop it*/
  WIRELESS_TASK_START_WIFI = (1 << 2), /*!< enable WIFI -start it if neccessery,
                                          register should be set first*/
  WIRELESS_TASK_RESET_WIFI_FOR_120_SEC =
      (1 << 3), /*!< disable WIFI FOR 120 SEC*/
};
uint8_t connection_state = 0;
static void wifi_event_handler(void *arg, esp_event_base_t event_base,
                               int32_t event_id, void *event_data) {
  (void)(arg);
  (void)(event_data);
  if (event_base == WIFI_EVENT) {
    switch (event_id) {
    case WIFI_EVENT_AP_STACONNECTED:
      ESP_LOGI(TAG, "Wi-Fi AP got a station connected");
      break;
    case WIFI_EVENT_AP_STADISCONNECTED:
      ESP_LOGI(TAG, "Wi-Fi AP got a station disconnected");
      break;
    case WIFI_EVENT_STA_DISCONNECTED:
      ESP_LOGI(TAG, "Wi-Fi sta got a station disconnected");
      gpio_set_level(PIN_USER_LED, 1);
      connection_state = 0;
      break;
    case WIFI_EVENT_STA_CONNECTED:
      ESP_LOGI(TAG, "Wi-Fi sta got a station connected");
      gpio_set_level(PIN_USER_LED, 0);
      connection_state = 1;
      break;

    default:
      break;
    }
  } else if (event_base == IP_EVENT) {
    if (event_id == IP_EVENT_STA_GOT_IP) {
      ip_event_got_ip_t *event = (ip_event_got_ip_t *)event_data;
      ESP_LOGI(TAG, "IP_EVENT_STA_GOT_IP IP: " IPSTR,
               IP2STR(&event->ip_info.ip));
    }
  }
}
/* HTTP stream handler */
static esp_err_t stream_handler(httpd_req_t *req) {
  esp_err_t res = ESP_OK;
  httpd_resp_set_type(req, _STREAM_CONTENT_TYPE);
  httpd_resp_set_hdr(req, "Access-Control-Allow-Origin", "*");
  while (true) {
    camera_fb_t *fb = esp_camera_fb_get();
    if (!fb) {
      ESP_LOGE(TAG, "Camera capture failed");
      httpd_resp_send_500(req);
      return ESP_FAIL;
    }
    if (fb->format != PIXFORMAT_JPEG) {
      ESP_LOGE(TAG, "Non-JPEG format captured");
      esp_camera_fb_return(fb);
      return ESP_FAIL;
    }
    if (httpd_resp_send_chunk(req, _STREAM_BOUNDARY,
                              strlen(_STREAM_BOUNDARY)) != ESP_OK) {
      esp_camera_fb_return(fb);
      break;
    }
    char part_buf[64];
    int hlen =
        snprintf(part_buf, sizeof(part_buf), _STREAM_PART, (unsigned)fb->len);
    if (httpd_resp_send_chunk(req, part_buf, hlen) != ESP_OK) {
      esp_camera_fb_return(fb);
      break;
    }
    if (httpd_resp_send_chunk(req, (const char *)fb->buf, fb->len) != ESP_OK) {
      esp_camera_fb_return(fb);
      break;
    }
    esp_camera_fb_return(fb);
    vTaskDelay(pdMS_TO_TICKS(10));
  }
  httpd_resp_send_chunk(req, NULL, 0);
  return res;
}

/* Simple index page showing the stream */
static esp_err_t index_handler(httpd_req_t *req) {
  const char *html = "<html><head><title>ESP32-S3 Camera</title></head>"
                     "<body><h1> Nowhere </h1>"
                     "<img src=\"/stream\" />"
                     "</body></html>";
  httpd_resp_set_type(req, "text/html");
  httpd_resp_send(req, html, strlen(html));
  return ESP_OK;
}

static httpd_uri_t stream_uri = {.uri = "/stream",
                                 .method = HTTP_GET,
                                 .handler = stream_handler,
                                 .user_ctx = NULL};

static httpd_uri_t index_uri = {
    .uri = "/", .method = HTTP_GET, .handler = index_handler, .user_ctx = NULL};

static httpd_handle_t start_webserver(void) {
  httpd_config_t config = HTTPD_DEFAULT_CONFIG();
  config.stack_size = 8192;
  httpd_handle_t server = NULL;
  if (httpd_start(&server, &config) == ESP_OK) {
    httpd_register_uri_handler(server, &index_uri);
    httpd_register_uri_handler(server, &stream_uri);
  }
  return server;
}

/* Initialize Wi-Fi in station mode */
static void wifi_init_sta(void) {
  esp_netif_init();
  esp_event_loop_create_default();
  esp_netif_create_default_wifi_sta();

  wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
  esp_wifi_init(&cfg);
  ESP_ERROR_CHECK(esp_event_handler_register(WIFI_EVENT, ESP_EVENT_ANY_ID,
                                             &wifi_event_handler, NULL));
  ESP_ERROR_CHECK(esp_event_handler_register(IP_EVENT, IP_EVENT_STA_GOT_IP,
                                             &wifi_event_handler, NULL));

  wifi_config_t wifi_config = {0};
  strncpy((char *)wifi_config.sta.ssid, WIFI_SSID,
          sizeof(wifi_config.sta.ssid) - 1);
  strncpy((char *)wifi_config.sta.password, WIFI_PASS,
          sizeof(wifi_config.sta.password) - 1);

  esp_wifi_set_mode(WIFI_MODE_STA);
  esp_wifi_set_config(WIFI_IF_STA, &wifi_config);
  esp_wifi_start();

  esp_wifi_connect();
  ESP_LOGI(TAG, "Connecting to Wi-Fi SSID:'%s' ...", WIFI_SSID);
}

/* Initialize camera with config. Make sure pins are correct for your board. */
static esp_err_t camera_init_board(void) {
  camera_config_t config = {.pin_pwdn = PIN_PWDN,
                            .pin_reset = PIN_RESET,
                            .pin_xclk = PIN_XCLK,
                            .pin_sccb_sda = PIN_SIOD,
                            .pin_sccb_scl = PIN_SIOC,
                            .pin_d7 = PIN_Y9,
                            .pin_d6 = PIN_Y8,
                            .pin_d5 = PIN_Y7,
                            .pin_d4 = PIN_Y6,
                            .pin_d3 = PIN_Y5,
                            .pin_d2 = PIN_Y4,
                            .pin_d1 = PIN_Y3,
                            .pin_d0 = PIN_Y2,
                            .pin_vsync = PIN_VSYNC,
                            .pin_href = PIN_HREF,
                            .pin_pclk = PIN_PCLK,

                            .xclk_freq_hz = 20000000,
                            .ledc_timer = LEDC_TIMER_0,
                            .ledc_channel = LEDC_CHANNEL_0,

                            .pixel_format = PIXFORMAT_JPEG,
                            .frame_size = FRAME_SIZE,
                            .jpeg_quality = JPEG_QUALITY,
                            .fb_count = FB_COUNT,
                            .grab_mode = CAMERA_GRAB_LATEST};

  esp_err_t err = esp_camera_init(&config);
  if (err != ESP_OK) {
    ESP_LOGE(TAG, "Camera init failed with error 0x%x", err);
    return err;
  }

  sensor_t *s = esp_camera_sensor_get();
  if (!s) {
    ESP_LOGE(TAG, "Failed to get sensor");
    return ESP_FAIL;
  }

  s->set_vflip(s, 0);
  s->set_hmirror(s, 0);

  ESP_LOGI(TAG, "Camera initialized");
  return ESP_OK;
}
void user_led_init(void) {
  gpio_config_t io_conf = {.pin_bit_mask = (1ULL << PIN_USER_LED),
                           .mode = GPIO_MODE_OUTPUT,
                           .pull_up_en = GPIO_PULLUP_DISABLE,
                           .pull_down_en = GPIO_PULLDOWN_DISABLE,
                           .intr_type = GPIO_INTR_DISABLE};
  gpio_config(&io_conf);
  gpio_set_level(PIN_USER_LED, 1); // Turn off LED initially
}
#define wireless_control_TASK_PERIOD (5000u)

static int wireless_control_deinit() {
  int result = 0;
  return result;
}

void wireless_control_task(void *arg) {
  (void)arg;
  uint32_t signal_value;
  uint64_t task_counter = 0u;
  wifi_ap_record_t ap_info;

  while (1) {
    signal_value = 0;

    if (task_notify_wait(WIRELESS_TASK_STOP_CHILD_PROCCES | WIRELESS_TASK_STOP_WIFI,
                         &signal_value,
                         wireless_control_TASK_PERIOD) == pdTRUE) {
      /*by signal*/
      if (signal_value & WIRELESS_TASK_STOP_CHILD_PROCCES) {
        wireless_control_deinit();
        task_delete(task_get_id());
      }
    }
    if(connection_state == 0){
      if (ESP_OK != esp_wifi_connect()) {
        ESP_LOGI(TAG, "connection to sta failed %u", task_counter);
      }
    }
    esp_wifi_sta_get_ap_info(&ap_info);
    task_counter++;
  }
}

void app_main(void) {
  nvs_flash_init();
  ESP_LOGI(TAG, "Starting XIAO S3 camera stream example");
  user_led_init();
  wifi_init_sta();
  if (camera_init_board() != ESP_OK) {
    ESP_LOGE(TAG, "Camera init failed, aborting");
    return;
  }
  start_webserver();
  ESP_LOGI(TAG, "Server started. Open http://<board-ip>/ in a browser");
  int res =
      task_create(wireless_control_task, "wireless_control_task", 3048, NULL,
                  (tskIDLE_PRIORITY + 2), &wireless_control_handle_id);
  if (res != pdTRUE) {
    ESP_LOGE(TAG, "create wireless_control task failed");
  }
}