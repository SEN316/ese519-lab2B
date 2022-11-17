/**
 * Copyright (c) 2020 Raspberry Pi (Trading) Ltd.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <stdio.h>
#include <stdlib.h>

#include "pico/stdlib.h"
#include "hardware/pio.h"
#include "hardware/clocks.h"
#include "ws2812.pio.h"
#include "registers.h"

#define IS_RGBW true
#define NUM_PIXELS 150

#ifdef PICO_DEFAULT_WS2812_PIN
#define WS2812_PIN PICO_DEFAULT_WS2812_PIN
#else
// default to pin 2 if the board doesn't have a default WS2812 pin defined
#define WS2812_POWER_PIN 11
#define WS2812_PIN 12
#endif

void turn_on_NeoPixel_power(){
    const uint led_pin = WS2812_POWER_PIN;
    gpio_init(led_pin);
    gpio_set_dir(led_pin, GPIO_OUT);
    gpio_put(led_pin,1);
}


static inline void put_pixel(uint32_t pixel_grb) {
    pio_sm_put_blocking(pio0, 0, pixel_grb << 8u);
}

static inline uint32_t urgb_u32(uint8_t r, uint8_t g, uint8_t b) {
    return
            ((uint32_t) (r) << 8) |
            ((uint32_t) (g) << 16) |
            (uint32_t) (b);
}

void set_neopixel_color(uint32_t color){
    uint32_t r = color & (uint32_t)0x00ff0000;
    r = r >> 16;
    uint32_t g = color & (uint32_t)0x0000ff00;
    g = g >> 8;
    uint32_t b = color & (uint32_t)0x000000ff;
    put_pixel(urgb_u32(r,g,b));
}


int main() {
    stdio_init_all();

    PIO pio = pio0;
    int sm = 0;
    uint offset = pio_add_program(pio, &ws2812_program);
    turn_on_NeoPixel_power();
    ws2812_program_init(pio, sm, offset, WS2812_PIN, 800000, IS_RGBW);


    uint32_t input_address = 0x00000000;
    ADDRESS address = 0x00000000;
    VALUE value;
    int mode;

    sleep_ms(10000); //WAIT FOR PUTTY!

    while (true) {
        
        printf("Start！ \n");

        // address selection
        printf("(1)Enter an address you want to read/write: \n");
        scanf("%x", &input_address);  
        printf("Confirm: Address is %x\n",input_address);
        address = (ADDRESS) input_address;

        // mode selection
        printf("(2)Enter 0 for Read Mode/1 for Write Mode): \n");
        scanf("%d", &mode);
        
        // reading mode
        if(mode == 0) {
            printf("Read Mode: The data read is: %x\n", register_read(address));
        }
        // writing mode
        else {
            // value input
            printf("Write Mode: Enter an value you want to write in: \n");
            scanf("%x", &value);  
            printf("Confirm: Value is %x \n",value);
            register_write(address, value);
            printf("The value written in: %x\n", register_read(address));
        }
        sleep_ms(250);

    }

    return 0;
}