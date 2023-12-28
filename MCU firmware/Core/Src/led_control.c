#include "led_control.h"
#include "tim.h"
#include "rtc.h"




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

void start_led_control(SystemSettings_t* settings) {
    driver_module_power(1);
    HAL_Delay(150);
    led_enable(1);
    HAL_Delay(20);
    RTC_TimeTypeDef sTime;
    HAL_RTC_GetTime(&hrtc, &sTime, FORMAT_BIN);
    uint8_t time = sTime.Hours;
    HAL_TIM_PWM_Start(&htim1,TIM_CHANNEL_1 );
//    for (uint8_t i = 0; i < PWM_CHANNELS_MAX ; i++) {
//    	if (settings->Pwm_on[i] == true) {
//			update_pwm_duty_cycle(i + 1, settings->pwmx_brightness[i][time]);
//			HAL_TIM_PWM_Start( PWM_Channels[i].htim, PWM_Channels[i].channel);
//		}
//    }
    HAL_GPIO_TogglePin(GPIOB, GPIO_PIN_1);

}

void stop_led_control(void) {
    for (uint8_t i = 0; i < PWM_CHANNELS_MAX ; i++) {
    	HAL_TIM_PWM_Stop( PWM_Channels[i].htim, PWM_Channels[i].channel);
    }
    led_enable(0);
    HAL_Delay(20);
    driver_module_power(0);
}

void update_led_control(void) {
    RTC_TimeTypeDef sTime;
    HAL_RTC_GetTime(&hrtc, &sTime, FORMAT_BIN);
    uint8_t currentHour = sTime.Hours;
    uint8_t minutes = sTime.Minutes;

    // Calculate the next hour, wrapping around at 24
    uint8_t nextHour = (currentHour + 1) % 24;

    for (uint8_t i = 0; i < PWM_CHANNELS_MAX; i++) {
        if (SystemSettings.Pwm_on[i] == true) {
            // Get the brightness for the current and next hours
            uint32_t currentBrightness = SystemSettings.pwmx_brightness[i][currentHour];
            uint32_t nextBrightness = SystemSettings.pwmx_brightness[i][nextHour];

            // Calculate the difference and the proportional brightness
            int32_t brightnessDifference = nextBrightness - currentBrightness;
            uint32_t proportionalBrightness = currentBrightness + (brightnessDifference * minutes / 60);

            // Update the PWM duty cycle
            set_brightness_single(i + 1, proportionalBrightness);
        }
    }
}

void turn_on_single(uint8_t pwm_index) {
	SystemSettings.Pwm_on[pwm_index] = true;
	RTC_TimeTypeDef sTime;
	HAL_RTC_GetTime(&hrtc, &sTime, FORMAT_BIN);
	uint8_t currentHour = sTime.Hours;
	uint8_t minutes = sTime.Minutes;
	uint8_t nextHour = (currentHour + 1) % 24;
	uint32_t currentBrightness = SystemSettings.pwmx_brightness[pwm_index][currentHour];
	uint32_t nextBrightness = SystemSettings.pwmx_brightness[pwm_index][nextHour];
	int32_t brightnessDifference = nextBrightness - currentBrightness;
	uint32_t proportionalBrightness = currentBrightness + (brightnessDifference * minutes / 60);
	set_brightness_single(pwm_index + 1, proportionalBrightness);
}

