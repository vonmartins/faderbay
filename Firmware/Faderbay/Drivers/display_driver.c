/* =========================================================
 * display_driver.c
 * Driver layer — Low-level OLED/display SPI driver
 * ========================================================= */

// ============================= INCLUDES ==============================

#include "display_driver.h"
#include "faderbay_types.h"
#include "nlog.h"
#include "u8g2.h"
#include "spi.h"
#include "gpio.h"
#include "timer.h"
#include "main.h"
#include <stdint.h>

// =========================== PRIVATE DEFINES =========================

// ============================ PRIVATE TYPES ==========================

// =========================== PRIVATE VARIABLES =======================

static const char *TAG = "DISP_DRV";

static u8g2_t s_u8g2;
static SPI_HandleTypeDef * s_hspi;

// ========================= PRIVATE FUNC. DECL. =======================

static uint8_t u8x8_byte_hw_spi(u8x8_t *u8x8, uint8_t msg, uint8_t arg_int, void *arg_ptr);
static uint8_t u8x8_gpio_and_delay(u8x8_t *u8x8, uint8_t msg, uint8_t arg_int, void *arg_ptr);

// =========================== PRIVATE FUNCTIONS =======================

static uint8_t u8x8_byte_hw_spi(u8x8_t *u8x8, uint8_t msg, uint8_t arg_int, void *arg_ptr) {
    switch (msg) {
        case U8X8_MSG_BYTE_SEND:
            // LOGI(TAG, "SPI send %d bytes", arg_int);
            SPI_SendBuffer(s_hspi, (uint8_t*) arg_ptr, arg_int);
            break;
        case U8X8_MSG_BYTE_INIT:
            break;
        case U8X8_MSG_BYTE_SET_DC:
            break;
        case U8X8_MSG_BYTE_START_TRANSFER:
            // LOGI(TAG, "CS LOW");
            GPIO_Set(DISP_CS_GPIO_Port, DISP_CS_Pin, 1);
            break;
        case U8X8_MSG_BYTE_END_TRANSFER:
            // LOGI(TAG, "CS HIGH");
            GPIO_Set(DISP_CS_GPIO_Port, DISP_CS_Pin, 0);
            break;
    }
    return 1;
}

static uint8_t u8x8_gpio_and_delay(u8x8_t *u8x8, uint8_t msg, uint8_t arg_int, void *arg_ptr) {
    switch (msg) {
        case U8X8_MSG_GPIO_AND_DELAY_INIT:
            break;
        case U8X8_MSG_DELAY_MILLI:
            Timer_Delay(arg_int);
            break;
        case U8X8_MSG_GPIO_CS:
            GPIO_Set(DISP_CS_GPIO_Port, DISP_CS_Pin, arg_int);
            break;
        case U8X8_MSG_GPIO_RESET:
            break;
    }
    return 1;
}

// =========================== PUBLIC FUNCTIONS ========================

fb_err_t DisplayDriver_Init(SPI_HandleTypeDef *hspi) {
    if (hspi == NULL) {
        LOGE(TAG, "Init failed: null SPI handle");
        return FB_ERR_INVALID_PARAM;
    }
    s_hspi = hspi;
    Timer_Delay(100);
    u8g2_Setup_st7920_s_144x32_f(&s_u8g2, U8G2_R0, u8x8_byte_hw_spi, u8x8_gpio_and_delay);
    u8g2_InitDisplay(&s_u8g2);
    u8g2_SetPowerSave(&s_u8g2, 0);
    u8g2_ClearBuffer(&s_u8g2);
    u8g2_SetFont(&s_u8g2, u8g2_font_7x13_tf);
    LOGI(TAG, "Init OK");
    return FB_OK;
}

void DisplayDriver_Clear(void) {
    u8g2_ClearBuffer(&s_u8g2);
}

void DisplayDriver_DrawStr(uint8_t x, uint8_t y, const char *str) {
    u8g2_DrawStr(&s_u8g2, x, y, str);
}

void DisplayDriver_DrawFrame(uint8_t x, uint8_t y, uint8_t w, uint8_t h) {
    u8g2_DrawFrame(&s_u8g2, x, y, w, h);
}

void DisplayDriver_Flush(void) {
    // LOGI(TAG, "Flush");
    u8g2_SendBuffer(&s_u8g2);
}


// TEMP

static void st7920_send(uint8_t rs, uint8_t data) {
    uint8_t sync = 0xF8 | (rs ? 0x02 : 0x00);
    uint8_t hi = data & 0xF0;
    uint8_t lo = (data << 4) & 0xF0;
    uint8_t buf[3] = {sync, hi, lo};
    GPIO_Set(DISP_CS_GPIO_Port, DISP_CS_Pin, 1);
    Timer_Delay(1);
    SPI_SendBuffer(s_hspi, buf, 3);
    Timer_Delay(1);
    GPIO_Set(DISP_CS_GPIO_Port, DISP_CS_Pin, 0);
}

void DisplayDriver_TestPattern(void) {
    LOGI(TAG, "TestPattern Start");
    Timer_Delay(100);

    // Init basico ST7920
    st7920_send(0, 0x30);
    Timer_Delay(5);
    st7920_send(0, 0x30);
    Timer_Delay(5);
    st7920_send(0, 0x0C);
    Timer_Delay(5);
    st7920_send(0, 0x01);
    Timer_Delay(5);
    st7920_send(0, 0x06);
    Timer_Delay(5);


    st7920_send(0, 0x80);
    Timer_Delay(5);
    st7920_send(1, 'H');
    Timer_Delay(2);
    st7920_send(1, 'E');
    Timer_Delay(2);
    st7920_send(1, 'L');
    Timer_Delay(2);
    st7920_send(1, 'L');
    Timer_Delay(2);
    st7920_send(1, 'O');
    Timer_Delay(2);
}