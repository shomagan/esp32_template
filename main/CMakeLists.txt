idf_build_get_property(project_dir PROJECT_DIR)

idf_component_register(
    SRCS "wifi_slip_main.c"
    SRCS "slip_handle.c"
    SRCS "modbus.c"
    SRCS "regs.c"
    SRCS "regs_description.c"
    SRCS "link_functions.c"
    SRCS "http/fs.c"
    SRCS "http/fsdata.c"
    SRCS "http/http_client.c"
    SRCS "http/httpd.c"
    SRCS "http/httpd_ssi_cgi.c"
    SRCS "http/json.c"
    SRCS "common.c"
    SRCS "mirror_storage.c"
    SRCS "crc.c"
    SRCS "memory_handle.c"
    SRCS "modbus_tcp/modbus_tcp.c"
    SRCS "modbus_tcp_client/modbus_tcp_client.c"
    SRCS "udp_broadcast/udp_broadcast.c"
    SRCS "u8g2_esp32_hal.c"
    SRCS "radar/sr04.c"
    SRCS "interrupt.c"
    SRCS "sleep_control.c"
    SRCS "wireless_control.c"
    SRCS "battery_state.c"
    SRCS "telegram.c"
    INCLUDE_DIRS "."
    INCLUDE_DIRS "radar/."
    INCLUDE_DIRS "http/."
    INCLUDE_DIRS "udp_broadcast/."
    INCLUDE_DIRS "modbus_tcp_client/."
    EMBED_TXTFILES ${project_dir}/certs/telegram.pem
    REQUIRES esp_netif nvs_flash u8g2 bt esp-tls esp_driver_gptimer esp_driver_uart esp_driver_gpio esp_driver_spi esp_driver_rmt driver esp_driver_mcpwm esp_wifi efuse esp_event esp_adc esp_system esp_pm lwip app_update esp_partition bootloader_support
)

# Run generator to regenerate regs_description files when regs.h or regs.c change.
# --no-user-tasks: user tasks generator is run separately below.
add_custom_command(
    OUTPUT ${project_dir}/main/regs_description.c ${project_dir}/main/regs_description.h
    COMMAND python regs_generator.py --no-user-tasks
    WORKING_DIRECTORY ${project_dir}/generator/
    DEPENDS ${project_dir}/main/regs.h ${project_dir}/main/regs.c
            ${project_dir}/generator/regs_generator.py
            ${project_dir}/generator/regs_handl.py
    COMMENT "Generating regs_description from regs.h..."
)
add_custom_target(regs_generator_target
    DEPENDS ${project_dir}/main/regs_description.c ${project_dir}/main/regs_description.h
)
add_dependencies(__idf_main regs_generator_target)

# ---- User task regs generator ----
# Generation is unconditional: every task header with a #generator_use_description
# struct gets its regs_description_t table (and table_version) injected into its own
# .c file, regardless of whether that task is enabled below. Enabling/disabling a task
# only controls whether user_tasks/CMakeLists.txt compiles that .c file - the generator
# itself never checks the USER_TASK_* options.
set(USER_TASK_HDR_PATHS
    ${project_dir}/user_tasks/scd41.h
    ${project_dir}/user_tasks/pwm_test.h
    ${project_dir}/user_tasks/di_handle.h
    ${project_dir}/user_tasks/feeder.h
    ${project_dir}/user_tasks/morse.h
    ${project_dir}/user_tasks/polisher.h
    ${project_dir}/user_tasks/test_int.h
    ${project_dir}/user_tasks/touch_handle.h
)
set(USER_TASK_C_PATHS
    ${project_dir}/user_tasks/scd41.c
    ${project_dir}/user_tasks/pwm_test.c
    ${project_dir}/user_tasks/di_handle.c
    ${project_dir}/user_tasks/feeder.c
    ${project_dir}/user_tasks/morse.c
    ${project_dir}/user_tasks/polisher.c
    ${project_dir}/user_tasks/test_int.c
    ${project_dir}/user_tasks/touch_handle.c
)

add_custom_command(
    OUTPUT ${USER_TASK_C_PATHS}
    COMMAND python regs_generator.py --user-tasks-only
    WORKING_DIRECTORY ${project_dir}/generator/
    DEPENDS ${USER_TASK_HDR_PATHS}
            ${project_dir}/generator/regs_generator.py
            ${project_dir}/generator/regs_handl.py
    COMMENT "Injecting regs_description tables into all user task sources..."
)
add_custom_target(user_regs_generator_target
    DEPENDS ${USER_TASK_C_PATHS}
)
add_dependencies(__idf_main user_regs_generator_target)
