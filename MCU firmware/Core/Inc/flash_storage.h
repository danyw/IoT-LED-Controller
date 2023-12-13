#ifndef FLASH_STORAGE_H
#define FLASH_STORAGE_H

#include "stdint.h"
#include "stm32wbxx_hal.h" // Include the appropriate header for STM32WB55
#include "main.h"

#define SETTINGS_FLASH_ADDRESS   ((uint32_t)0x08032000) // Start address of user flash area
#define FLASH_USER_START_ADDR   ((uint32_t)0x08032000) // Start address of user flash area
#define FLASH_USER_END_ADDR     ((uint32_t)0x080FFFFF) // End address of user flash area

HAL_StatusTypeDef Flash_Write(uint32_t address, uint8_t *data, uint32_t length);
//HAL_StatusTypeDef WriteSettingsToFlash(SystemSettings_t* settings);
HAL_StatusTypeDef Flash_Read(uint32_t address, uint8_t *data, uint32_t length);

void ReadSettingsFromFlash(SystemSettings_t* settings);
void LoadSettingsFromFlash();
void WriteSettingsToFlash(SystemSettings_t* settings);
void generateSampleData(SystemSettings_t* settings);

#endif // FLASH_STORAGE_H
