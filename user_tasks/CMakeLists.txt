idf_build_get_property(project_dir PROJECT_DIR)

# ---- User task options are defined in the root CMakeLists.txt ----
# This file reads those options to control which sources are compiled.

# Build source and define lists from selected options
set(USER_TASK_SRCS   "regs_description_user.c" "user_tasks_init.c")
set(USER_TASK_DEFS   "")

if(USER_TASK_SCD41)
    list(APPEND USER_TASK_SRCS "scd41.c")
    list(APPEND USER_TASK_DEFS "SCD41_ENABLE=1")
endif()
if(USER_TASK_PWM)
    list(APPEND USER_TASK_SRCS "pwm_test.c")
    list(APPEND USER_TASK_DEFS "PWM_CONTROL_ENABLE=1")
endif()
if(USER_TASK_DI)
    list(APPEND USER_TASK_SRCS "di_handle.c")
    list(APPEND USER_TASK_DEFS "DI_HANDLING_ENABLE=1")
endif()
if(USER_TASK_FEEDER)
    list(APPEND USER_TASK_SRCS "feeder.c")
    list(APPEND USER_TASK_DEFS "FEEDER=1")
endif()
if(USER_TASK_MORSE)
    list(APPEND USER_TASK_SRCS "morse.c")
    list(APPEND USER_TASK_DEFS "MORSE=1")
endif()
if(USER_TASK_POLISHER)
    list(APPEND USER_TASK_SRCS "polisher.c")
    list(APPEND USER_TASK_DEFS "POLISHER=1")
endif()
if(USER_TASK_STEP_MOTOR)
    list(APPEND USER_TASK_SRCS "step_motor.c")
    list(APPEND USER_TASK_DEFS "STEP_MOTOR=1")
endif()
if(USER_TASK_TEST_INT)
    list(APPEND USER_TASK_SRCS "test_int.c")
    list(APPEND USER_TASK_DEFS "TEST_INT=1")
endif()
if(USER_TASK_TOUCH)
    list(APPEND USER_TASK_SRCS "touch_handle.c")
    list(APPEND USER_TASK_DEFS "TOUCH_HANDLE_ENABLE=1")
endif()

idf_component_register(
    SRCS ${USER_TASK_SRCS}
    INCLUDE_DIRS "."
    REQUIRES main
)

target_compile_definitions(__idf_user_tasks PRIVATE ${USER_TASK_DEFS})
