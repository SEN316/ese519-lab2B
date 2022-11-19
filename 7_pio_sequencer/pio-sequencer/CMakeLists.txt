cmake_minimum_required(VERSION 3.13) # CMAKE (heading)
include(pico_sdk_import.cmake) # CMAKE -> PICO SDK (include)

project(pio-sequencer C CXX ASM) # CMAKE ("project")
pico_sdk_init() # PICO SDK (initialization)

add_executable(pio-sequencer) # CMAKE (target)

pico_generate_pio_header(pio-sequencer ${CMAKE_CURRENT_LIST_DIR}/ws2812.pio OUTPUT_DIR ${CMAKE_CURRENT_LIST_DIR}/generated) # PICO SDK (PIO)

target_sources(pio-sequencer PRIVATE neopixel.c pio-sequencer.c) # CMAKE -> C (compiler)
target_link_libraries(pio-sequencer PRIVATE pico_stdlib hardware_pio hardware_dma) # CMAKE -> C (linker)

pico_enable_stdio_usb(pio-sequencer 1) # PICO SDK (USB)
pico_enable_stdio_uart(pio-sequencer 0) # PICO SDK (USB)

pico_add_extra_outputs(pio-sequencer) # PICO SDK (UF2)