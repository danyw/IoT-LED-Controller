#include "flash_storage.h"


// helper function. calculates the page number for the address
static uint32_t GetPage(uint32_t Addr)
{
    return (Addr - FLASH_BASE) / FLASH_PAGE_SIZE;
}



// Write data to flash
HAL_StatusTypeDef Flash_Write(uint32_t address, uint8_t *data, uint32_t length) {
    HAL_StatusTypeDef status;
    uint32_t end_address = address + length;
    uint32_t FirstPage = 0, NbOfPages = 0;
      FLASH_EraseInitTypeDef EraseInitStruct = {0};
    // Check if the address is within the valid range
    if (address < FLASH_USER_START_ADDR || end_address > FLASH_USER_END_ADDR) {
        return HAL_ERROR;
    }

    // Unlock the Flash to enable the flash control register access
    HAL_FLASH_Unlock();

    FirstPage = GetPage(SETTINGS_FLASH_ADDRESS);
	NbOfPages = (sizeof(SystemSettings_t) + FLASH_PAGE_SIZE - 1) / FLASH_PAGE_SIZE;

	// Fill EraseInit structure
	EraseInitStruct.TypeErase = FLASH_TYPEERASE_PAGES;
	EraseInitStruct.Page = FirstPage;
	EraseInitStruct.NbPages = NbOfPages;


	// Erase the required pages
	uint32_t PageError = 0;
	if (HAL_FLASHEx_Erase(&EraseInitStruct, &PageError) != HAL_OK) {
		HAL_FLASH_Lock();
		return HAL_ERROR;
	}

    // Writing without an erase might fail if the flash is not empty
    for (uint32_t i = 0; i < length; i += 8, address += 8, data += 8) {
        // Write in double word (64 bits)
        status = HAL_FLASH_Program(FLASH_TYPEPROGRAM_DOUBLEWORD, address, *((uint64_t*)data));
        if (status != HAL_OK) {
            break;
        }
    }

    // Lock the Flash to disable the flash control register access
    HAL_FLASH_Lock();

    return status;
}

// Read data from flash
HAL_StatusTypeDef Flash_Read(uint32_t address, uint8_t *data, uint32_t length) {
    uint32_t end_address = address + length;

    // Check if the address is within the valid range
    if (address < FLASH_USER_START_ADDR || end_address > FLASH_USER_END_ADDR) {
        return HAL_ERROR;
    }

    // Simple memory copy since flash is memory mapped
    memcpy(data, (uint8_t*)address, length);

    return HAL_OK;
}

//HAL_StatusTypeDef WriteSettingsToFlash(SystemSettings_t* settings) {
//    HAL_StatusTypeDef status;
//    uint32_t FirstPage = 0, NbOfPages = 0;
//    FLASH_EraseInitTypeDef EraseInitStruct = {0};
//
//    // Unlock flash
//    status = HAL_FLASH_Unlock();
//    if (status != HAL_OK) {
//        return status;
//    }
//
//    // Calculate page and number of pages
//	FirstPage = GetPage(SETTINGS_FLASH_ADDRESS);
//	NbOfPages = (sizeof(SystemSettings_t) + FLASH_PAGE_SIZE - 1) / FLASH_PAGE_SIZE;
//
//	// Erase the required pages
//	EraseInitStruct.TypeErase = FLASH_TYPEERASE_PAGES;
//	EraseInitStruct.Page = FirstPage;
//	EraseInitStruct.NbPages = NbOfPages;
//	uint32_t PageError = 0;
//	if (HAL_FLASHEx_Erase(&EraseInitStruct, &PageError) != HAL_OK) {
//		HAL_FLASH_Lock();
//		return HAL_ERROR;
//	}
//
//	// Write settings to flash, double word by double word
//	uint64_t* flash_pointer = (uint64_t*)SETTINGS_FLASH_ADDRESS;
//	uint64_t* data_pointer = (uint64_t*)settings;
//	uint32_t data_length = sizeof(SystemSettings_t);
//
//	for (uint32_t i = 0; i < data_length / 8; i++) {
//		status = HAL_FLASH_Program(FLASH_TYPEPROGRAM_DOUBLEWORD, (uint32_t)flash_pointer, *data_pointer);
//		if (status != HAL_OK) {
//			HAL_FLASH_Lock();
//			return status;
//		}
//	flash_pointer++;
//	data_pointer++;
// }
//
//	// Lock the flash
//	HAL_FLASH_Lock();
//
//    return HAL_OK;
//}


void ReadSettingsFromFlash(SystemSettings_t* settings) {
    // Read settings from flash
    uint64_t* flash_pointer = (uint64_t*)SETTINGS_FLASH_ADDRESS;
    uint64_t* data_pointer = (uint64_t*)settings;
    uint32_t data_length = sizeof(SystemSettings_t);

    // Ensure the data length is a multiple of 8 bytes (64 bits)
    if (data_length % 8 != 0) {
        // Handle the error, perhaps by padding the structure
        return;
    }

    for (uint32_t i = 0; i < data_length / 8; i++) {
        *data_pointer = *flash_pointer;
        flash_pointer++;
        data_pointer++;
    }
}

// Read settings from flash memory and load into SystemSettings
void LoadSettingsFromFlash() {
	  if (Flash_Read(FLASH_USER_START_ADDR, (uint8_t*)&SystemSettings, sizeof(SystemSettings_t)) != HAL_OK) {
	    // Handle error
	  }
}

void WriteSettingsToFlash(SystemSettings_t* settings) {
    if (Flash_Write(FLASH_USER_START_ADDR, (uint8_t*)settings, sizeof(SystemSettings_t)) != HAL_OK) {
      // Handle error
    }
  }

void generateSampleData(SystemSettings_t* settings) {
	const uint8_t STRING_LENGTH;
    // Sample values for CurrentSenseFactor and TemperatureOffset
	settings->PWM_CHANNELS = 4;
	settings->Pwm_on[0] = true;
	settings->Pwm_on[1] = true;
	settings->Pwm_on[2] = true;
	settings->Pwm_on[3] = true;
	settings->Pwm_on[4] = false;
	settings->Pwm_on[5] = false;


    settings->CurrentSenseFactor = 1.23f;
    settings->TemperatureOffset = -5.5f;

    // Generate PWM brightness values
    for (int channel = 0; channel < PWM_CHANNELS_MAX; ++channel) {
        for (int hour = 0; hour < 24; ++hour) {
            // Calculate peak time for each channel
            int peakHour = 6 * channel + 12;
            if (peakHour >= 24) peakHour -= 24;

            // Calculate brightness
            int brightness;
            if (hour <= peakHour) {
                brightness = (int)((1000.0 / peakHour) * hour);
            } else {
                brightness = (int)(1000 - (1000.0 / (24 - peakHour)) * (hour - peakHour));
            }

            settings->pwmx_brightness[channel][hour] = brightness;
        }
    }
}
