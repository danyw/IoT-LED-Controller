#ifndef SENSORS_H
#define SENSORS_H

#include "stm32wbxx_hal.h"
#include "main.h"

#define ADC_RESOLUTION 4096.0f // 12-bit ADC

extern float CurrentSenseFactor; // Replace with actual initial value
extern float VoltageDividerRatio; //
extern float TemperatureScale; //
extern float TemperatureOffset; //
extern float VrefInt; // Internal reference voltage



void Sensors_Init(ADC_HandleTypeDef* hadc);
void Sensors_Measure(void);
float Sensors_GetVoltage(uint16_t raw);
float Sensors_GetCurrent(uint16_t raw);
float Sensors_GetTemperature(uint16_t raw);
float Sensors_CalculatePower(float voltage, float current);

#endif // SENSORS_H
