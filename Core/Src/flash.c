#include "flash.h"
#include "main.h"
#include <string.h>
//#include "stm32f4xx_hal.h"
#include <stdint.h>



uint8_t Flash_Write_Configs(Flash_Write_Block_t *block_data) {
    if (HAL_FLASH_Unlock() != HAL_OK) {
        return 0; 
    }

    FLASH_EraseInitTypeDef EraseInitStruct;
    uint32_t SectorError;

    EraseInitStruct.TypeErase = FLASH_TYPEERASE_SECTORS;
    EraseInitStruct.Sector = SECTOR; 
    EraseInitStruct.NbSectors = 1;
    EraseInitStruct.VoltageRange = FLASH_VOLTAGE_RANGE_3;

    if (HAL_FLASHEx_Erase(&EraseInitStruct, &SectorError) != HAL_OK) {
        HAL_FLASH_Lock();
        return 0; 
    }

    uint32_t *data_ptr = (uint32_t*) block_data;
    for (uint32_t i = 0; i < sizeof(Flash_Write_Block_t) / 4; i++) {
        if (HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, FLASH_CONFIGS_START + (i * 4), data_ptr[i]) != HAL_OK) {
            HAL_FLASH_Lock();
            return 0; 
        }
    }

    HAL_FLASH_Lock();
    return 1; 
}

void Flash_Read_Configs(Flash_Write_Block_t *block_data)
{
    uint8_t *src = (uint8_t*)FLASH_CONFIGS_START;
    uint8_t *dest = (uint8_t*)block_data;

    for (uint32_t i = 0; i < sizeof(Flash_Write_Block_t); i++) {
        dest[i] = src[i];
    }
}

uint8_t Write_Presets(Preset_t *presets)
{
    memcpy(device_config_block.current_presets, presets, sizeof(device_config_block.current_presets));
    return Flash_Write_Configs(&device_config_block);
}

uint8_t Write_Configs(Config_t *configs)
{
    memcpy(&device_config_block.current_config, configs, sizeof(device_config_block.current_config));
    return Flash_Write_Configs(&device_config_block);
}