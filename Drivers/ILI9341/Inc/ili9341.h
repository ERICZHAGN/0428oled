#ifndef ILI9341_H
#define ILI9341_H

#ifdef __cplusplus
extern "C" {
#endif

#include "main.h"

#define ILI9341_WIDTH   240U
#define ILI9341_HEIGHT  320U

#define ILI9341_COLOR_WHITE       0xFFFFU
#define ILI9341_COLOR_BLACK       0x0000U
#define ILI9341_COLOR_BLUE        0x001FU
#define ILI9341_COLOR_RED         0xF800U
#define ILI9341_COLOR_GREEN       0x07E0U
#define ILI9341_COLOR_CYAN        0x07FFU
#define ILI9341_COLOR_YELLOW      0xFFE0U
#define ILI9341_COLOR_MAGENTA     0xF81FU
#define ILI9341_COLOR_GRAY        0x8410U
#define ILI9341_COLOR_DARKBLUE    0x01CFU
#define ILI9341_COLOR_LIGHTGRAY   0xC618U
#define ILI9341_COLOR_ORANGE      0xFD20U

typedef struct
{
  SPI_HandleTypeDef *hspi;
  uint16_t width;
  uint16_t height;
} ILI9341_Handle;

HAL_StatusTypeDef ILI9341_Init(ILI9341_Handle *lcd, SPI_HandleTypeDef *hspi);
HAL_StatusTypeDef ILI9341_FillScreen(ILI9341_Handle *lcd, uint16_t color);
HAL_StatusTypeDef ILI9341_FillRect(ILI9341_Handle *lcd, uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint16_t color);
HAL_StatusTypeDef ILI9341_DrawRect(ILI9341_Handle *lcd, uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint16_t color);
HAL_StatusTypeDef ILI9341_DrawChar6x8(ILI9341_Handle *lcd, uint16_t x, uint16_t y, char ch, uint16_t fg, uint16_t bg, uint8_t scale);
HAL_StatusTypeDef ILI9341_DrawString6x8(ILI9341_Handle *lcd, uint16_t x, uint16_t y, const char *str, uint16_t fg, uint16_t bg, uint8_t scale);
HAL_StatusTypeDef ILI9341_DrawStringCentered(ILI9341_Handle *lcd, uint16_t x, uint16_t y, uint16_t width, const char *str, uint16_t fg, uint16_t bg, uint8_t scale);

#ifdef __cplusplus
}
#endif

#endif /* ILI9341_H */
