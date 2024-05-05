# The following five lines of boilerplate have to be in your project's
# CMakeLists in this exact order for cmake to work correctly
cmake_minimum_required(VERSION 3.5)

include($ENV{IDF_PATH}/tools/cmake/project.cmake)
project(wifi_slip)
#add_custom_command(OUTPUT main/regs.h
#    COMMAND python ${PROJECT_SOURCE_DIR}/generator/regs_generator.py
#    WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}/generator/
#    COMMENT "Running regs script generator..."
#)
#add_custom_target(generator ALL DEPENDS main/regs.h)


