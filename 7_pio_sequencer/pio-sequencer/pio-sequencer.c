#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include "pico/stdlib.h"
#include "hardware/pio.h"
#include "hardware/dma.h"
#include "hardware/structs/bus_ctrl.h"
#include "hardware/clocks.h"
#include "ws2812.pio.h"
#include "hardware/structs/pwm.h"
#include "registers.h"
#include "neopixel.h"
const uint CAPTURE_PIN_BASE = 22;
const uint CAPTURE_PIN_COUNT = 2;
const uint CAPTURE_N_SAMPLES = 96;
const uint BOOT_PIN = 21;

#define WS2812_PIN 12
#define WS2812_POWER_PIN 11
#define IS_RGBW true

uint32_t button_is_pressed;

static inline uint bits_packed_per_word(uint pin_count) {
    const uint SHIFT_REG_WIDTH = 32;
    return SHIFT_REG_WIDTH - (SHIFT_REG_WIDTH % pin_count);
}

void logic_analyser_init(PIO pio, uint sm, uint pin_base, uint pin_count, float div) {
    uint16_t capture_prog_instr = pio_encode_in(pio_pins, pin_count);
    struct pio_program capture_prog = {
            .instructions = &capture_prog_instr,
            .length = 1,
            .origin = -1
    };
    uint offset = pio_add_program(pio, &capture_prog);

    pio_sm_config c = pio_get_default_sm_config();
    sm_config_set_in_pins(&c, pin_base);
    sm_config_set_wrap(&c, offset, offset);
    sm_config_set_clkdiv(&c, div);
    // Note that we may push at a < 32 bit threshold if pin_count does not
    // divide 32. We are using shift-to-right, so the sample data ends up
    // left-justified in the FIFO in this case, with some zeroes at the LSBs.
    sm_config_set_in_shift(&c, true, true, bits_packed_per_word(pin_count));
    sm_config_set_fifo_join(&c, PIO_FIFO_JOIN_RX);
    pio_sm_init(pio, sm, offset, &c);
}

void logic_analyser_arm(PIO pio, uint sm, uint dma_chan, uint32_t *capture_buf, size_t capture_size_words,
                        uint trigger_pin, bool trigger_level) {
    pio_sm_set_enabled(pio, sm, false);
    pio_sm_clear_fifos(pio, sm);
    pio_sm_restart(pio, sm);

    dma_channel_config c = dma_channel_get_default_config(dma_chan);
    channel_config_set_read_increment(&c, false);
    channel_config_set_write_increment(&c, true);
    channel_config_set_dreq(&c, pio_get_dreq(pio, sm, false));

    dma_channel_configure(dma_chan, &c,
        capture_buf,        // Destination pointer
        &pio->rxf[sm],      // Source pointer
        capture_size_words, // Number of transfers
        true                // Start immediately
    );

    //pio_sm_exec(pio, sm, pio_encode_wait_gpio(trigger_level, trigger_pin));
    pio_sm_set_enabled(pio, sm, true);
}

void print_capture_buf(const uint32_t *buf, uint pin_base, uint pin_count, uint32_t n_samples) {
    uint record_size_bits = bits_packed_per_word(pin_count);
    for (int pin = 0; pin < pin_count; ++pin) {
        //printf("%02d: ", pin + pin_base);
        for (int sample = 0; sample < n_samples; ++sample) {
            uint bit_index = pin + sample * pin_count;
            uint word_index = bit_index / record_size_bits;
            if (buf[word_index] != 0){
                button_is_pressed = 0x0;
            }
            else{
                button_is_pressed = 0x1;
            }

        }
    }
}

int main() {
    stdio_init_all();
    PIO pio = pio0;
    uint sm = 0;
    uint dma_chan = 0;
    uint32_t last_serial_byte;
    uint32_t light_color;
    uint offset = pio_add_program(pio, &ws2812_program);
    ws2812_program_init(pio, sm, offset, WS2812_PIN, 800000, IS_RGBW);

    gpio_init(WS2812_POWER_PIN);
    gpio_set_dir(WS2812_POWER_PIN, GPIO_OUT);
    gpio_put(WS2812_POWER_PIN, 1);

    
    uint total_sample_bits = CAPTURE_N_SAMPLES * CAPTURE_PIN_COUNT;
    total_sample_bits += bits_packed_per_word(CAPTURE_PIN_COUNT) - 1;
    uint buf_size_words = total_sample_bits / bits_packed_per_word(CAPTURE_PIN_COUNT);
    uint32_t *capture_buf = malloc(buf_size_words * sizeof(uint32_t));
    hard_assert(capture_buf);
    bus_ctrl_hw->priority = BUSCTRL_BUS_PRIORITY_DMA_W_BITS | BUSCTRL_BUS_PRIORITY_DMA_R_BITS;
    
    PIO pio_1 = pio1;
    logic_analyser_init(pio_1, sm, CAPTURE_PIN_BASE, CAPTURE_PIN_COUNT, 256.f);
    while (true) {
        last_serial_byte = getchar_timeout_us(0); // don't block main loop
        if (last_serial_byte == 'r'){
            while(true){
                last_serial_byte = getchar_timeout_us(0);
                if (last_serial_byte == 's'){
                    printf("finished");
                    break;
                }
                logic_analyser_arm(pio_1, sm, dma_chan, capture_buf, buf_size_words, CAPTURE_PIN_BASE, false);
                print_capture_buf(capture_buf, CAPTURE_PIN_BASE, CAPTURE_PIN_COUNT, CAPTURE_N_SAMPLES);
                if (button_is_pressed){
                    printf("1\n");    
                    neopixel_set_rgb(0x00ff0000);
                } 
                else {
                    printf("0\n");
                    neopixel_set_rgb(0x00000000);
                }
                sleep_ms(50); // don't DDOS the serial console
            }
        
        }
        if (last_serial_byte == 'w'){
            printf("OK\n");
            while(true){
            last_serial_byte = getchar_timeout_us(0);
            if (last_serial_byte == '1'){
                neopixel_set_rgb(0x00ff0000);
                sleep_ms(50);
            }
            else{
                neopixel_set_rgb(0x00000000);
                sleep_ms(50);
            }
            //sleep_ms(10);
            }
        }
    }
    
    return 0;
}