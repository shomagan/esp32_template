idf_component_register(SRCS "web_stream_main.c"
                       INCLUDE_DIRS "."
                       REQUIRES esp32-camera esp_netif esp_wifi esp_http_server esp_mm esp_driver_cam esp_driver_i2c nvs_flash 
                       PRIV_REQUIRES esp_driver_ledc
                       )
