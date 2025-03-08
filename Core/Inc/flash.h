#ifndef _FLASH_H
#define _FLASH_H

#include "device.h"
#include "config.h"
#include "stm32f4xx_hal.h"
#include <stdint.h>

#define FLASH_SECTOR_5_ADDR     0x08020000
#define FLASH_CONFIGS_START     FLASH_SECTOR_5_ADDR

#define SECTOR                  FLASH_SECTOR_5


typedef struct {
    Preset_t current_presets[NUM_PRESETS];  /* Current presets on device */
    Config_t current_config;                /* Current config of device */
} Flash_Write_Block_t;


uint8_t Flash_Write_Configs(Flash_Write_Block_t *block_data);
void Flash_Read_Configs(Flash_Write_Block_t *block_data);
uint8_t Write_Presets(Preset_t *presets);
uint8_t Write_Configs(Config_t *configs);

#endif