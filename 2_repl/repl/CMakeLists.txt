cmake_minimum_required(VERSION 3.13)
# Pull in (SDK before the project)
include(pico_sdk_import.cmake)

# Set project name and C/C++ standards
project(repl C CXX ASM)

# Set the version of C/C++
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

# Initialize the SDK
pico_sdk_init()

# Tell CMake where to find executable source file
add_executable(${PROJECT_NAME}
   repl.c
)

pico_generate_pio_header(${PROJECT_NAME} ${CMAKE_CURRENT_LIST_DIR}/ws2812.pio) #OUTPUT_DIR ${CMAKE_CURRENT_LIST_DIR}/generated)

target_sources(${PROJECT_NAME} PRIVATE repl.c)
target_link_libraries(${PROJECT_NAME}
    PRIVATE pico_stdlib 
    PRIVATE hardware_pio
)

pico_enable_stdio_usb(${PROJECT_NAME} 1) # PICO SDK (USB)
pico_enable_stdio_uart(${PROJECT_NAME} 0) # PICO SDK (USB)

pico_add_extra_outputs(${PROJECT_NAME}) # PICO SDK (UF2)