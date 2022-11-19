#ifndef NEOPIXEL_H
#define NEOPIXEL_H

#include <stdlib.h>
#include "pico/stdlib.h"

void neopixel_init();
void neopixel_set_rgb(uint32_t rgb);

#endif