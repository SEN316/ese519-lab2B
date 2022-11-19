cmake_minimum_required(VERSION 3.13) # CMAKE (heading)
include(pico_sdk_import.cmake) # CMAKE -> PICO SDK (include)

project(lab_on_a_chip C CXX ASM) # CMAKE ("project")
pico_sdk_init() # PICO SDK (initialization)

add_executable(lab_on_a_chip) # CMAKE (target)

pico_generate_pio_header(lab_on_a_chip ${CMAKE_CURRENT_LIST_DIR}/i2c.pio OUTPUT_DIR ${CMAKE_CURRENT_LIST_DIR}/generated) # PICO SDK (PIO)
pico_generate_pio_header(lab_on_a_chip ${CMAKE_CURRENT_LIST_DIR}/ws2812.pio OUTPUT_DIR ${CMAKE_CURRENT_LIST_DIR}/generated) 

target_sources(lab_on_a_chip PRIVATE pio_i2c.c pio_i2c.h apds.h apds.c lab_on_a_chip.c neopixel.c neopixel.h piosequencer.c piosequencer.h) # CMAKE -> C (compiler)
target_link_libraries(lab_on_a_chip PRIVATE pico_multicore pico_stdlib hardware_pio hardware_dma) # CMAKE -> C (linker)

pico_enable_stdio_usb(lab_on_a_chip 1) # PICO SDK (USB)
pico_enable_stdio_uart(lab_on_a_chip 0) # PICO SDK (USB)

pico_add_extra_outputs(lab_on_a_chip) # PICO SDK (UF2)