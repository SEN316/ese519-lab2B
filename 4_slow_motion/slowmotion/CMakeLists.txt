cmake_minimum_required(VERSION 3.13) # CMAKE (heading)
include(pico_sdk_import.cmake) # CMAKE -> PICO SDK (include)

project(slowmotion C CXX ASM) # CMAKE ("project")
pico_sdk_init() # PICO SDK (initialization)

add_executable(slowmotion) # CMAKE (target)

pico_generate_pio_header(slowmotion ${CMAKE_CURRENT_LIST_DIR}/ws2812.pio OUTPUT_DIR ${CMAKE_CURRENT_LIST_DIR}/generated) # PICO SDK (PIO)

target_sources(slowmotion PRIVATE slowmotion.c neopixel.c) # CMAKE -> C (compiler)
target_link_libraries(slowmotion PRIVATE pico_stdlib hardware_pio) # CMAKE -> C (linker)

pico_enable_stdio_usb(slowmotion 1) # PICO SDK (USB)
pico_enable_stdio_uart(slowmotion 0) # PICO SDK (USB)

pico_add_extra_outputs(slowmotion) # PICO SDK (UF2)