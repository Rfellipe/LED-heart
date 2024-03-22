#cmake_minimum_required(VERSION 3.20.0)
#project(lil_proj)
#find_package(Zephyr REQUIRED HINTS $ENV{ZEPHYR_BASE})
#
#set(IDF_PATH "$ENV{IDF_PATH}")
#include($ENV{IDF_PATH}/tools/cmake/project.cmake)
#add_custom_target(little_proj)
#
#target_sources(app PRIVATE src/main.c)

cmake_minimum_required(VERSION 3.20.0)

set(BOARD "esp32_devkitc_wroom/esp32/procpu")
find_package(Zephyr REQUIRED HINTS $ENV{ZEPHYR_BASE})
project(hello_world)
FILE(GLOB app_sources src/*.c)
target_sources(app PRIVATE ${app_sources})
