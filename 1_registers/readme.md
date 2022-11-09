### TODO:

Toggle the Qt Py's Neopixel LED when the BOOT button is pressed, using only direct register reads to access the boot button status. You may use the SDK to initialize the board and the pins, and the WS2812 example code to toggle the Neopixel. 

## Define PIN and POWER_PIN

In neopixel.c, we initialize PIN and POWER_PIN 

```
#define PIN         12
#define POWER_PIN   11
```
## Initialize the variables

In flashlight.c, we initialize the variables which related to the boot register

```
volatile uint32_t * boot_pin_address;
uint32_t full_gpio_register_value;
uint32_t pin_21_selection_mask;
uint32_t select_pin_state;
uint32_t shifted_pin_21_state;
```

## Define the boot register address

From SIO_BASE_u, the address of boot register is defined as 0xd0000004. Thus, in flashlight.c, we can define the boot address to describe the state of BOOT button. 

```
boot_pin_address = (volatile uint32_t *) 0xd0000004;
full_gpio_register_value = (uint32_t) *boot_pin_address;
pin_21_selection_mask = 1u << 21;
selected_pin_state = full_gpio_register_value & pin_21_selection_mask;
shifted_pin_21_state = selected_pin_state >> 21;
```

## flashlight
![](https://github.com/SEN316/ese519-lab2B-part1-10/blob/main/1_registers/part1.gif)
