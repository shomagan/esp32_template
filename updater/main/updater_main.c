#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_wifi.h"
#include "esp_netif.h"
#include "esp_event.h"
#include "esp_log.h"
#include "esp_ota_ops.h"
#include "esp_partition.h"
#include "esp_http_server.h"
#include "nvs_flash.h"

/* Fixed on purpose: a recovery app must stay reachable without depending on
 * config kept in sync with the main OS project. */
#define UPDATER_WIFI_SSID "esp32_updater"
#define UPDATER_WIFI_PASS "update_me_now"
#define UPDATER_WIFI_CHANNEL 1
#define UPDATER_MAX_STA_CONN 2

static const char *TAG = "updater";

static const char index_html[] =
"<!DOCTYPE html><html><head><title>ESP32 Firmware Updater</title></head><body>"
"<h3>ESP32 Firmware Updater</h3>"
"<p>Select a factory app image (.bin) and upload it, then boot into it.</p>"
"<input type=\"file\" id=\"file\">"
"<button onclick=\"upload()\">Upload</button>"
"<button onclick=\"boot()\">Boot into new firmware</button>"
"<p id=\"status\"></p>"
"<script>"
"function upload(){"
"  var f=document.getElementById('file').files[0];"
"  if(!f){document.getElementById('status').innerText='pick a file first';return;}"
"  var xhr=new XMLHttpRequest();"
"  xhr.open('POST','/upload',true);"
"  xhr.onload=function(){document.getElementById('status').innerText=xhr.responseText;};"
"  document.getElementById('status').innerText='uploading...';"
"  xhr.send(f);"
"}"
"function boot(){"
"  var xhr=new XMLHttpRequest();"
"  xhr.open('POST','/boot',true);"
"  xhr.onload=function(){document.getElementById('status').innerText=xhr.responseText;};"
"  xhr.send();"
"}"
"</script></body></html>";

static const esp_partition_t *s_target_partition = NULL;
static bool s_ota_ok = false;

static esp_err_t index_get_handler(httpd_req_t *req) {
    httpd_resp_set_type(req, "text/html");
    return httpd_resp_send(req, index_html, HTTPD_RESP_USE_STRLEN);
}

static esp_err_t upload_post_handler(httpd_req_t *req) {
    s_ota_ok = false;
    /* esp_ota_begin() only accepts ota_x partitions as a write target - a
     * "factory" partition can be a boot target (esp_ota_set_boot_partition)
     * but never an OTA write target, so uploads land in ota_1, not factory. */
    s_target_partition = esp_partition_find_first(ESP_PARTITION_TYPE_APP, ESP_PARTITION_SUBTYPE_APP_OTA_1, "ota_1");
    if (s_target_partition == NULL) {
        httpd_resp_send_err(req, HTTPD_500_INTERNAL_SERVER_ERROR, "ota_1 partition not found");
        return ESP_FAIL;
    }
    if (req->content_len <= 0 || req->content_len > s_target_partition->size) {
        httpd_resp_send_err(req, HTTPD_400_BAD_REQUEST, "file size does not fit ota_1 partition");
        return ESP_FAIL;
    }

    esp_ota_handle_t ota_handle = 0;
    esp_err_t err = esp_ota_begin(s_target_partition, OTA_SIZE_UNKNOWN, &ota_handle);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "esp_ota_begin failed: %s", esp_err_to_name(err));
        httpd_resp_send_err(req, HTTPD_500_INTERNAL_SERVER_ERROR, "ota begin failed");
        return ESP_FAIL;
    }

    char buf[1024];
    int remaining = req->content_len;
    while (remaining > 0) {
        int to_read = remaining < (int)sizeof(buf) ? remaining : (int)sizeof(buf);
        int received = httpd_req_recv(req, buf, to_read);
        if (received == HTTPD_SOCK_ERR_TIMEOUT) {
            continue;
        }
        if (received <= 0) {
            esp_ota_abort(ota_handle);
            httpd_resp_send_err(req, HTTPD_500_INTERNAL_SERVER_ERROR, "upload receive failed");
            return ESP_FAIL;
        }
        err = esp_ota_write(ota_handle, buf, received);
        if (err != ESP_OK) {
            ESP_LOGE(TAG, "esp_ota_write failed: %s", esp_err_to_name(err));
            esp_ota_abort(ota_handle);
            httpd_resp_send_err(req, HTTPD_500_INTERNAL_SERVER_ERROR, "ota write failed");
            return ESP_FAIL;
        }
        remaining -= received;
    }

    err = esp_ota_end(ota_handle);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "esp_ota_end failed: %s", esp_err_to_name(err));
        httpd_resp_sendstr(req, "upload finished but image validation failed - not booting it");
        return ESP_OK;
    }

    s_ota_ok = true;
    httpd_resp_sendstr(req, "upload OK, image validated. Press \"Boot into new firmware\" to switch.");
    return ESP_OK;
}

static esp_err_t boot_post_handler(httpd_req_t *req) {
    if (!s_ota_ok || s_target_partition == NULL) {
        httpd_resp_send_err(req, HTTPD_400_BAD_REQUEST, "no validated firmware uploaded yet");
        return ESP_FAIL;
    }
    esp_err_t err = esp_ota_set_boot_partition(s_target_partition);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "esp_ota_set_boot_partition failed: %s", esp_err_to_name(err));
        httpd_resp_send_err(req, HTTPD_500_INTERNAL_SERVER_ERROR, "set boot partition failed");
        return ESP_FAIL;
    }
    httpd_resp_sendstr(req, "booting new firmware...");
    vTaskDelay(pdMS_TO_TICKS(500));
    esp_restart();
    return ESP_OK;
}

static void start_httpd(void) {
    httpd_handle_t server = NULL;
    httpd_config_t config = HTTPD_DEFAULT_CONFIG();
    config.stack_size = 8192;
    config.uri_match_fn = httpd_uri_match_wildcard;
    ESP_ERROR_CHECK(httpd_start(&server, &config));

    static const httpd_uri_t index_uri = {.uri = "/", .method = HTTP_GET, .handler = index_get_handler};
    static const httpd_uri_t upload_uri = {.uri = "/upload", .method = HTTP_POST, .handler = upload_post_handler};
    static const httpd_uri_t boot_uri = {.uri = "/boot", .method = HTTP_POST, .handler = boot_post_handler};
    ESP_ERROR_CHECK(httpd_register_uri_handler(server, &index_uri));
    ESP_ERROR_CHECK(httpd_register_uri_handler(server, &upload_uri));
    ESP_ERROR_CHECK(httpd_register_uri_handler(server, &boot_uri));
}

static void wifi_init_softap(void) {
    ESP_ERROR_CHECK(esp_netif_init());
    ESP_ERROR_CHECK(esp_event_loop_create_default());
    esp_netif_create_default_wifi_ap();

    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));

    wifi_config_t wifi_config = {
        .ap = {
            .ssid = UPDATER_WIFI_SSID,
            .ssid_len = strlen(UPDATER_WIFI_SSID),
            .channel = UPDATER_WIFI_CHANNEL,
            .password = UPDATER_WIFI_PASS,
            .max_connection = UPDATER_MAX_STA_CONN,
            .authmode = WIFI_AUTH_WPA2_PSK,
        },
    };
    if (strlen(UPDATER_WIFI_PASS) == 0) {
        wifi_config.ap.authmode = WIFI_AUTH_OPEN;
    }

    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_AP));
    ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_AP, &wifi_config));
    ESP_ERROR_CHECK(esp_wifi_start());
    ESP_LOGI(TAG, "softAP started, SSID:%s password:%s channel:%d", UPDATER_WIFI_SSID, UPDATER_WIFI_PASS, UPDATER_WIFI_CHANNEL);
}

void app_main(void) {
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);

    wifi_init_softap();
    start_httpd();
    ESP_LOGI(TAG, "updater ready");
}
