#include "flash.h"
#include "main.h"
#include <string.h>
#include <stdint.h>

/**
 * @brief Writes configuration data to flash memory.
 * 
 * This function unlocks the flash memory, erases the specified sector, and writes the configuration data to the flash memory.
 * 
 * @param block_data Pointer to the data block to be written to flash memory.
 * @return Returns 1 if the operation is successful, otherwise returns 0.
 */
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

/**
 * @brief Reads configuration data from flash memory.
 * 
 * This function reads the configuration data from the flash memory and stores it in the provided data block.
 * 
 * @param block_data Pointer to the data block where the data will be read.
 */
void Flash_Read_Configs(Flash_Write_Block_t *block_data)
{
    uint8_t *src = (uint8_t*)FLASH_CONFIGS_START;
    uint8_t *dest = (uint8_t*)block_data;

    for (uint32_t i = 0; i < sizeof(Flash_Write_Block_t); i++) {
        dest[i] = src[i];
    }
}

/**
 * @brief Writes preset data to flash memory.
 * 
 * This function copies the provided preset data to the device configuration block and writes it to flash memory.
 * 
 * @param presets Pointer to the preset data to be written to flash memory.
 * @return Returns 1 if the operation is successful, otherwise returns 0.
 */
uint8_t Write_Presets(Preset_t *presets)
{
    memcpy(device_config_block.current_presets, presets, sizeof(device_config_block.current_presets));
    return Flash_Write_Configs(&device_config_block);
}

/**
 * @brief Writes configuration data to flash memory.
 * 
 * This function copies the provided configuration data to the device configuration block and writes it to flash memory.
 * 
 * @param configs Pointer to the configuration data to be written to flash memory.
 * @return Returns 1 if the operation is successful, otherwise returns 0.
 */
uint8_t Write_Configs(Config_t *configs)
{
    memcpy(&device_config_block.current_config, configs, sizeof(device_config_block.current_config));
    return Flash_Write_Configs(&device_config_block);
}

/**
 * @brief Checks if this is the first initialization.
 * 
 * This function checks the flash memory to determine if this is the first initialization by reading the first byte of the configuration data.
 * 
 * @return Returns 1 if this is the first initialization, otherwise returns 0.
 */
uint8_t Is_First_Initialization()
{
    uint8_t flash_value = *(volatile uint8_t*)FLASH_CONFIGS_START;

    if (flash_value == 0xFF) {
        return 1; 
    } else {
        return 0; 
    }
}