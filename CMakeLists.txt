# The following five lines of boilerplate have to be in your project's
# CMakeLists in this exact order for cmake to work correctly
cmake_minimum_required(VERSION 3.5)

# ---- User task options ----
# Enable/disable tasks here to control both compilation and code generation.
option(USER_TASK_SCD41      "Enable SCD41 sensor task"       OFF)
option(USER_TASK_PWM        "Enable PWM control task"        ON)
option(USER_TASK_DI         "Enable DI handling task"        OFF)
option(USER_TASK_FEEDER     "Enable Feeder task"             OFF)
option(USER_TASK_MORSE      "Enable Morse-code task"         OFF)
option(USER_TASK_POLISHER   "Enable Polisher task"           OFF)
option(USER_TASK_STEP_MOTOR "Enable step-motor task"         OFF)
option(USER_TASK_TEST_INT   "Enable test-interrupt task"     OFF)
option(USER_TASK_TOUCH      "Enable touch handling task"     OFF)

# Include user_tasks as a project component
set(EXTRA_COMPONENT_DIRS user_tasks)

include($ENV{IDF_PATH}/tools/cmake/project.cmake)
project(wifi_slip)
