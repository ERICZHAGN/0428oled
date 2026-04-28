#ifndef OLED_H
#define OLED_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include "stm32u5xx_hal.h"

#define OLED_DEFAULT_ADDRESS  0x3CU
#define OLED_WIDTH            128U
#define OLED_HEIGHT           64U
#define OLED_PAGE_COUNT       (OLED_HEIGHT / 8U)

typedef enum
{
  OLED_OK = 0,
  OLED_ERROR = -1,
  OLED_INVALID_PARAM = -2
} OLED_Result;

typedef struct
{
  I2C_HandleTypeDef *hi2c;
  uint8_t address;
  uint8_t width;
  uint8_t height;
  uint8_t page_count;
} OLED_Handle;

OLED_Result OLED_Attach(OLED_Handle *oled, I2C_HandleTypeDef *hi2c, uint8_t address);
OLED_Result OLED_Init(OLED_Handle *oled);
OLED_Result OLED_WriteCommand(OLED_Handle *oled, uint8_t cmd);
OLED_Result OLED_WriteData(OLED_Handle *oled, const uint8_t *data, uint16_t size);
OLED_Result OLED_SetCursor(OLED_Handle *oled, uint8_t page, uint8_t column);
OLED_Result OLED_Fill(OLED_Handle *oled, uint8_t data);
OLED_Result OLED_Clear(OLED_Handle *oled);
OLED_Result OLED_ShowChar6x8(OLED_Handle *oled, uint8_t page, uint8_t column, char ch);
OLED_Result OLED_ShowString6x8(OLED_Handle *oled, uint8_t page, uint8_t column, const char *str);
OLED_Result OLED_DisplayOn(OLED_Handle *oled);
OLED_Result OLED_DisplayOff(OLED_Handle *oled);

#ifdef __cplusplus
}
#endif

#endif /* OLED_H */
