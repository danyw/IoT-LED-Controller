#include "sensors.h"

float CurrentSenseFactor = 1.0f;
float VoltageDividerRatio = 0.0697674f;
float TemperatureScale = 10.0f;
float TemperatureOffset = 0.0f;
float VrefInt = 3.3f;


static ADC_HandleTypeDef* AdcHandle;

void Sensors_Init(ADC_HandleTypeDef* hadc) {
    AdcHandle = hadc;
}

void Sensors_Measure(void) {
    HAL_ADC_Start_DMA(AdcHandle, (uint32_t*)rawValues, 3);
}

float Sensors_GetVoltage(uint16_t raw) {
    // Calculate the actual voltage from the scaled value
    return (float)raw * VoltageDividerRatio * (VrefInt / ADC_RESOLUTION);
}

float Sensors_GetCurrent(uint16_t raw) {
    // Calculate the actual current using the MAX4080T factor
    return (float)raw * CurrentSenseFactor * (VrefInt / ADC_RESOLUTION);
}

float Sensors_GetTemperature(uint16_t raw) {
    // Calculate the actual temperature from the voltage
    return ((float)raw * (VrefInt / ADC_RESOLUTION) / TemperatureScale) + TemperatureOffset;
}

float Sensors_CalculatePower(float voltage, float current) {
    // Calculate power
    return voltage * current;
}
