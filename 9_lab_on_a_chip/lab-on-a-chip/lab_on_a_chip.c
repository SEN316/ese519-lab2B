#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>

#include "pico/stdlib.h"
#include "hardware/pio.h"
#include "hardware/dma.h"
#include "hardware/structs/bus_ctrl.h"
#include "pico/multicore.h"
#include "apds.h"
#include "i2c.pio.h"
#include "neopixel.h"
#include "piosequencer.h"

const uint CAPTURE_PIN_BASE = 12;
const uint CAPTURE_PIN_COUNT = 1;
const uint CAPTURE_N_SAMPLES = 350000;
const uint BOOT_PIN = 21;
const int address = 0x39;
#define PIN_SDA 22
#define PIN_SCL 23

uint32_t rgb_to_neopixel(uint8_t r, uint8_t g, uint8_t b){
    uint32_t result = 0;
    result |= (uint32_t)r << 16u;
    result |= (uint32_t)g << 8u;
    result |= (uint32_t)b;
    return result;
}

//core 1 is set to keep fetching data from APDS9960
void core1_main() {
    uint32_t proximity;
    uint32_t r, g, b, c;
    PIO pio_i2c = pio1;
    uint sm = 0;
    while(true){
        read_proximity(pio_i2c, sm, &proximity, address, true);
        read_rgbc(pio_i2c, sm, &r, &g, &b, &c);
        neopixel_set_rgb(rgb_to_neopixel(r, g, b));
        sleep_ms(5);
    }
}

int main() {
    stdio_init_all();

    // We're going to capture into a u32 buffer, for best DMA efficiency. Need
    // to be careful of rounding in case the number of pins being sampled
    // isn't a power of 2.
    uint total_sample_bits = CAPTURE_N_SAMPLES * CAPTURE_PIN_COUNT;
    total_sample_bits += bits_packed_per_word(CAPTURE_PIN_COUNT) - 1;
    uint buf_size_words = total_sample_bits / bits_packed_per_word(CAPTURE_PIN_COUNT);
    uint32_t * capture_buf = malloc(buf_size_words * sizeof(uint32_t));
    hard_assert(capture_buf);

    // Grant high bus priority to the DMA, so it can shove the processors out
    // of the way. This should only be needed if you are pushing things up to
    // >16bits/clk here, i.e. if you need to saturate the bus completely.
    bus_ctrl_hw->priority = BUSCTRL_BUS_PRIORITY_DMA_W_BITS | BUSCTRL_BUS_PRIORITY_DMA_R_BITS;

    //pio and DMA setting
    PIO pio = pio0;
    PIO pio_i2c = pio1;
    uint sm = 0;
    uint dma_chan = 0;

    //initialization
    logic_analyser_init(pio, sm, CAPTURE_PIN_BASE, CAPTURE_PIN_COUNT, 125000000 / (4 * 400 * 8 * 1000));// double freq of i2c rate. 400kbits/s => 800*8kHz

    gpio_init(BOOT_PIN);
    gpio_set_dir(BOOT_PIN, GPIO_IN);
    sleep_ms(1000);

    // initialize PIO.I2C and APDS9960
    uint offset = pio_add_program(pio_i2c, &i2c_program);
    i2c_program_init(pio_i2c, sm, offset, PIN_SDA, PIN_SCL);
    init_sensor(pio_i2c, sm);// the default i2c rate is set to 400kHz

    neopixel_init();
    
    multicore_launch_core1(core1_main); //keep fetching data from APDS9960 through core1.
    
    while(true){   
        printf("press boot button to arming trigger\n");

        do{} while (gpio_get(BOOT_PIN) == 1);

        logic_analyser_arm(pio, sm, dma_chan, capture_buf, buf_size_words, BOOT_PIN, false);

        printf("Start recording\n");
        
        dma_channel_wait_for_finish_blocking(dma_chan);
        printf("Done!\n");

        print_capture_buf(capture_buf, CAPTURE_PIN_BASE, CAPTURE_PIN_COUNT, CAPTURE_N_SAMPLES);

        sleep_ms(1000);
        }
        
}
