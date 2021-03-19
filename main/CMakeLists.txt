idf_component_register(
    SRCS "wifi_slip_main.c"
    SRCS "slip_handle.c"
    INCLUDE_DIRS "."
    REQUIRES esp_netif nvs_flash
)
