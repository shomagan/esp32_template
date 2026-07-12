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
    REQUIRES esp_netif nvs_flash u8g2 bt esp-tls esp_driver_gptimer esp_driver_uart esp_driver_gpio esp_driver_spi esp_driver_rmt driver esp_driver_mcpwm esp_wifi efuse esp_event esp_adc esp_system esp_pm lwip
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
# Build --user-task-header arguments from the root CMakeLists.txt options.
set(USER_GENERATOR_ARGS "")
set(USER_TASK_HDR_PATHS "")
if(USER_TASK_SCD41)
    list(APPEND USER_GENERATOR_ARGS "--user-task-header" "scd41.h")
    list(APPEND USER_TASK_HDR_PATHS "${project_dir}/user_tasks/scd41.h")
endif()
if(USER_TASK_PWM)
    list(APPEND USER_GENERATOR_ARGS "--user-task-header" "pwm_test.h")
    list(APPEND USER_TASK_HDR_PATHS "${project_dir}/user_tasks/pwm_test.h")
endif()
if(USER_TASK_DI)
    list(APPEND USER_GENERATOR_ARGS "--user-task-header" "di_handle.h")
    list(APPEND USER_TASK_HDR_PATHS "${project_dir}/user_tasks/di_handle.h")
endif()
if(USER_TASK_FEEDER)
    list(APPEND USER_GENERATOR_ARGS "--user-task-header" "feeder.h")
    list(APPEND USER_TASK_HDR_PATHS "${project_dir}/user_tasks/feeder.h")
endif()
if(USER_TASK_MORSE)
    list(APPEND USER_GENERATOR_ARGS "--user-task-header" "morse.h")
    list(APPEND USER_TASK_HDR_PATHS "${project_dir}/user_tasks/morse.h")
endif()
if(USER_TASK_POLISHER)
    list(APPEND USER_GENERATOR_ARGS "--user-task-header" "polisher.h")
    list(APPEND USER_TASK_HDR_PATHS "${project_dir}/user_tasks/polisher.h")
endif()
if(USER_TASK_STEP_MOTOR)
    list(APPEND USER_GENERATOR_ARGS "--user-task-header" "step_motor.h")
    list(APPEND USER_TASK_HDR_PATHS "${project_dir}/user_tasks/step_motor.h")
endif()
if(USER_TASK_TEST_INT)
    list(APPEND USER_GENERATOR_ARGS "--user-task-header" "test_int.h")
    list(APPEND USER_TASK_HDR_PATHS "${project_dir}/user_tasks/test_int.h")
endif()
if(USER_TASK_TOUCH)
    list(APPEND USER_GENERATOR_ARGS "--user-task-header" "touch_handle.h")
    list(APPEND USER_TASK_HDR_PATHS "${project_dir}/user_tasks/touch_handle.h")
endif()

add_custom_command(
    OUTPUT ${project_dir}/user_tasks/regs_description_user.c
    COMMAND python regs_generator.py --user-tasks-only ${USER_GENERATOR_ARGS}
    WORKING_DIRECTORY ${project_dir}/generator/
    DEPENDS ${USER_TASK_HDR_PATHS}
            ${project_dir}/generator/regs_generator.py
            ${project_dir}/generator/regs_handl.py
    COMMENT "Generating regs_description_user.c for user tasks..."
)
add_custom_target(user_regs_generator_target
    DEPENDS ${project_dir}/user_tasks/regs_description_user.c
)
add_dependencies(__idf_main user_regs_generator_target)
