#ifndef LED_CONTROL_H
#define LED_CONTROL_H

#include "main.h"


// Function Prototypes
void set_brightness_single(uint8_t channel, uint32_t brightness);
void set_brightness_multiple(const uint8_t brightness[PWM_CHANNELS]);
void set_brightness_all(uint8_t brightness);
void driver_module_power(uint8_t state);
void led_enable(uint8_t state);
void effect_rainbow(void);
void effect_fade_in(uint8_t channel, uint8_t final_brightness, uint16_t duration);
void effect_fade_out(uint8_t channel, uint16_t duration);

#endif // LED_CONTROL_H