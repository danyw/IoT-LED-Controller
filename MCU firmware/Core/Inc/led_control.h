#ifndef LED_CONTROL_H
#define LED_CONTROL_H

#include "main.h"


// Function Prototypes
void set_brightness_single(uint8_t channel, uint32_t brightness);
void set_brightness_all(uint32_t brightness);
void driver_module_power(uint8_t state);
void led_enable(uint8_t state);
void effect_rainbow(void);
void effect_fade_in(uint8_t channel, uint8_t final_brightness, uint16_t duration);
void effect_fade_out(uint8_t channel, uint16_t duration);
void start_led_control(SystemSettings_t* settings);
void update_led_control(void);
void stop_led_control(void);

#endif // LED_CONTROL_H
