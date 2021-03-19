cmake_minimum_required(VERSION 3.6)
set(PRJ_NAME makefsdata)
add_definitions(-DSOFI_LINK=1)
include(gccToolchain.cmake)
project(${PRJ_NAME} C CXX ASM)
include_directories(../../../../system)
include_directories(../../../include)
include_directories(../../../../../inc)

add_executable (makefsdata.exe makefsdata.c)
add_custom_command(TARGET makefsdata.exe POST_BUILD
                   COMMAND ${CMAKE_COMMAND} -E copy
                       ${CMAKE_SOURCE_DIR}/cmake_link/makefsdata.exe ${CMAKE_SOURCE_DIR}/../)


