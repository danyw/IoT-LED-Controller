#include "led_control.h"
#include "tim.h"

typedef struct {
    TIM_HandleTypeDef* htim;
    uint32_t channel;
} PWM_Channel;

PWM_Channel PWM_Channels[] = {
    {&htim1, TIM_CHANNEL_1},
    {&htim1, TIM_CHANNEL_2},
    {&htim1, TIM_CHANNEL_3},
    {&htim2, TIM_CHANNEL_1},
    {&htim2, TIM_CHANNEL_2},
    {&htim2, TIM_CHANNEL_3}
};

static void update_pwm_duty_cycle(uint8_t pwm_index, uint32_t dutyCycle) {
    if (pwm_index >= sizeof(PWM_Channels) / sizeof(PWM_Channels[0])) {
        // Handle invalid index
        return;
    }

    PWM_Channel channel = PWM_Channels[pwm_index - 1];
   // uint32_t period = __HAL_TIM_GET_AUTORELOAD(channel.htim);
   // uint32_t pulse = (dutyCycle * (period + 1)) / 100;
    __HAL_TIM_SET_COMPARE(channel.htim, channel.channel, dutyCycle);
}



void set_brightness_single(uint8_t pwm_index, uint32_t brightness) {
	update_pwm_duty_cycle(pwm_index, brightness);
}



void set_brightness_all(uint32_t brightness) {
    for (uint8_t i = 0; i < SystemSettings.PWM_CHANNELS; i++) {
        update_pwm_duty_cycle(i + 1, brightness);
    }
}

void driver_module_power(uint8_t state) {
    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_13, state ? GPIO_PIN_SET : GPIO_PIN_RESET);
}

void led_enable(uint8_t state) {
    HAL_GPIO_WritePin(GPIOC, GPIO_PIN_10, state ? GPIO_PIN_SET : GPIO_PIN_RESET);
}

void effect_rainbow(void) {
    for (uint8_t i = 0; i < 1000; i++) {
        for (uint8_t channel = 0; channel < SystemSettings.PWM_CHANNELS; channel++) {
            uint32_t brightness = ((i + channel) * 100) % 1000; // Example calculation
            set_brightness_single(channel + 1, brightness);
        }
        HAL_Delay(20); //  slow down the effect
    }
}

void effect_fade_in(uint8_t channel, uint8_t final_brightness, uint16_t duration) {
    uint32_t stepDelay = duration / final_brightness;
    for (uint32_t brightness = 0; brightness <= final_brightness; brightness++) {
        set_brightness_single(channel, brightness);
        HAL_Delay(stepDelay);
    }
}



void effect_fade_out(uint8_t channel, uint16_t duration) {
    uint32_t currentBrightness = SystemSettings.pwmx_brightness[channel - 1][0];
    uint32_t stepDelay = duration / currentBrightness;
    for (int32_t brightness = currentBrightness; brightness >= 0; brightness--) {
        set_brightness_single(channel, brightness);
        HAL_Delay(stepDelay);
    }
}







