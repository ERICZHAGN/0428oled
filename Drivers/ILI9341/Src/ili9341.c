#include "ili9341.h"

#include <string.h>

#define ILI9341_CMD_SWRESET  0x01U
#define ILI9341_CMD_SLPOUT   0x11U
#define ILI9341_CMD_DISPON   0x29U
#define ILI9341_CMD_CASET    0x2AU
#define ILI9341_CMD_PASET    0x2BU
#define ILI9341_CMD_RAMWR    0x2CU
#define ILI9341_CMD_MADCTL   0x36U
#define ILI9341_CMD_PIXFMT   0x3AU

static const uint8_t ili9341_font6x8[][6] =
{
  {0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
  {0x7E, 0x11, 0x11, 0x11, 0x7E, 0x00},
  {0x7F, 0x49, 0x49, 0x49, 0x36, 0x00},
  {0x3E, 0x41, 0x41, 0x41, 0x22, 0x00},
  {0x7F, 0x41, 0x41, 0x22, 0x1C, 0x00},
  {0x7F, 0x49, 0x49, 0x49, 0x41, 0x00},
  {0x7F, 0x09, 0x09, 0x09, 0x01, 0x00},
  {0x3E, 0x41, 0x49, 0x49, 0x7A, 0x00},
  {0x7F, 0x08, 0x08, 0x08, 0x7F, 0x00},
  {0x00, 0x41, 0x7F, 0x41, 0x00, 0x00},
  {0x20, 0x40, 0x41, 0x3F, 0x01, 0x00},
  {0x7F, 0x08, 0x14, 0x22, 0x41, 0x00},
  {0x7F, 0x40, 0x40, 0x40, 0x40, 0x00},
  {0x7F, 0x02, 0x0C, 0x02, 0x7F, 0x00},
  {0x7F, 0x04, 0x08, 0x10, 0x7F, 0x00},
  {0x3E, 0x41, 0x41, 0x41, 0x3E, 0x00},
  {0x7F, 0x09, 0x09, 0x09, 0x06, 0x00},
  {0x3E, 0x41, 0x51, 0x21, 0x5E, 0x00},
  {0x7F, 0x09, 0x19, 0x29, 0x46, 0x00},
  {0x46, 0x49, 0x49, 0x49, 0x31, 0x00},
  {0x01, 0x01, 0x7F, 0x01, 0x01, 0x00},
  {0x3F, 0x40, 0x40, 0x40, 0x3F, 0x00},
  {0x1F, 0x20, 0x40, 0x20, 0x1F, 0x00},
  {0x7F, 0x20, 0x18, 0x20, 0x7F, 0x00},
  {0x63, 0x14, 0x08, 0x14, 0x63, 0x00},
  {0x03, 0x04, 0x78, 0x04, 0x03, 0x00},
  {0x61, 0x51, 0x49, 0x45, 0x43, 0x00},
  {0x3E, 0x51, 0x49, 0x45, 0x3E, 0x00},
  {0x00, 0x42, 0x7F, 0x40, 0x00, 0x00},
  {0x42, 0x61, 0x51, 0x49, 0x46, 0x00},
  {0x21, 0x41, 0x45, 0x4B, 0x31, 0x00},
  {0x18, 0x14, 0x12, 0x7F, 0x10, 0x00},
  {0x27, 0x45, 0x45, 0x45, 0x39, 0x00},
  {0x3C, 0x4A, 0x49, 0x49, 0x30, 0x00},
  {0x01, 0x71, 0x09, 0x05, 0x03, 0x00},
  {0x36, 0x49, 0x49, 0x49, 0x36, 0x00},
  {0x06, 0x49, 0x49, 0x29, 0x1E, 0x00},
  {0x7E, 0x11, 0x11, 0x11, 0x7E, 0x00},
  {0x7F, 0x48, 0x48, 0x48, 0x30, 0x00},
  {0x38, 0x44, 0x44, 0x44, 0x20, 0x00},
  {0x30, 0x48, 0x48, 0x48, 0x7F, 0x00},
  {0x38, 0x54, 0x54, 0x54, 0x18, 0x00},
  {0x08, 0x7E, 0x09, 0x01, 0x02, 0x00},
  {0x18, 0xA4, 0xA4, 0xA4, 0x7C, 0x00},
  {0x7F, 0x08, 0x08, 0x08, 0x70, 0x00},
  {0x00, 0x48, 0x7A, 0x40, 0x00, 0x00},
  {0x40, 0x80, 0x88, 0x7A, 0x00, 0x00},
  {0x7F, 0x10, 0x28, 0x44, 0x00, 0x00},
  {0x00, 0x41, 0x7F, 0x40, 0x00, 0x00},
  {0x7C, 0x04, 0x18, 0x04, 0x78, 0x00},
  {0x7C, 0x08, 0x04, 0x04, 0x78, 0x00},
  {0x38, 0x44, 0x44, 0x44, 0x38, 0x00},
  {0xFC, 0x24, 0x24, 0x24, 0x18, 0x00},
  {0x18, 0x24, 0x24, 0x24, 0xFC, 0x00},
  {0x7C, 0x08, 0x04, 0x04, 0x08, 0x00},
  {0x48, 0x54, 0x54, 0x54, 0x20, 0x00},
  {0x04, 0x3F, 0x44, 0x40, 0x20, 0x00},
  {0x3C, 0x40, 0x40, 0x20, 0x7C, 0x00},
  {0x1C, 0x20, 0x40, 0x20, 0x1C, 0x00},
  {0x3C, 0x40, 0x30, 0x40, 0x3C, 0x00},
  {0x44, 0x28, 0x10, 0x28, 0x44, 0x00},
  {0x1C, 0xA0, 0xA0, 0xA0, 0x7C, 0x00},
  {0x44, 0x64, 0x54, 0x4C, 0x44, 0x00}
};

static const uint8_t *ILI9341_GetFont6x8(char ch)
{
  if (ch == ' ')
  {
    return ili9341_font6x8[0];
  }

  if ((ch >= 'A') && (ch <= 'Z'))
  {
    return ili9341_font6x8[1 + (ch - 'A')];
  }

  if ((ch >= '0') && (ch <= '9'))
  {
    return ili9341_font6x8[27 + (ch - '0')];
  }

  if ((ch >= 'a') && (ch <= 'z'))
  {
    return ili9341_font6x8[37 + (ch - 'a')];
  }

  return ili9341_font6x8[0];
}

static HAL_StatusTypeDef ILI9341_WriteCommand(ILI9341_Handle *lcd, uint8_t cmd)
{
  HAL_GPIO_WritePin(LCD_DC_GPIO_Port, LCD_DC_Pin, GPIO_PIN_RESET);
  return HAL_SPI_Transmit(lcd->hspi, &cmd, 1U, HAL_MAX_DELAY);
}

static HAL_StatusTypeDef ILI9341_WriteData(ILI9341_Handle *lcd, const uint8_t *data, uint16_t size)
{
  HAL_GPIO_WritePin(LCD_DC_GPIO_Port, LCD_DC_Pin, GPIO_PIN_SET);
  return HAL_SPI_Transmit(lcd->hspi, (uint8_t *)data, size, HAL_MAX_DELAY);
}

static HAL_StatusTypeDef ILI9341_SetAddressWindow(ILI9341_Handle *lcd, uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1)
{
  uint8_t data[4];

  data[0] = (uint8_t)(x0 >> 8);
  data[1] = (uint8_t)(x0 & 0xFFU);
  data[2] = (uint8_t)(x1 >> 8);
  data[3] = (uint8_t)(x1 & 0xFFU);
  if (ILI9341_WriteCommand(lcd, ILI9341_CMD_CASET) != HAL_OK)
  {
    return HAL_ERROR;
  }
  if (ILI9341_WriteData(lcd, data, sizeof(data)) != HAL_OK)
  {
    return HAL_ERROR;
  }

  data[0] = (uint8_t)(y0 >> 8);
  data[1] = (uint8_t)(y0 & 0xFFU);
  data[2] = (uint8_t)(y1 >> 8);
  data[3] = (uint8_t)(y1 & 0xFFU);
  if (ILI9341_WriteCommand(lcd, ILI9341_CMD_PASET) != HAL_OK)
  {
    return HAL_ERROR;
  }
  if (ILI9341_WriteData(lcd, data, sizeof(data)) != HAL_OK)
  {
    return HAL_ERROR;
  }

  return ILI9341_WriteCommand(lcd, ILI9341_CMD_RAMWR);
}

static HAL_StatusTypeDef ILI9341_WriteColorBurst(ILI9341_Handle *lcd, uint16_t color, uint32_t count)
{
  uint8_t buffer[128];
  uint32_t i;
  uint32_t chunk_pixels;
  uint16_t chunk_size;

  for (i = 0; i < (sizeof(buffer) / 2U); i++)
  {
    buffer[(i * 2U)] = (uint8_t)(color >> 8);
    buffer[(i * 2U) + 1U] = (uint8_t)(color & 0xFFU);
  }

  while (count > 0U)
  {
    chunk_pixels = count;
    if (chunk_pixels > (sizeof(buffer) / 2U))
    {
      chunk_pixels = sizeof(buffer) / 2U;
    }

    chunk_size = (uint16_t)(chunk_pixels * 2U);
    if (ILI9341_WriteData(lcd, buffer, chunk_size) != HAL_OK)
    {
      return HAL_ERROR;
    }

    count -= chunk_pixels;
  }

  return HAL_OK;
}

HAL_StatusTypeDef ILI9341_Init(ILI9341_Handle *lcd, SPI_HandleTypeDef *hspi)
{
  static const uint8_t init_cf[] = {0x00U, 0xC1U, 0x30U};
  static const uint8_t init_ed[] = {0x64U, 0x03U, 0x12U, 0x81U};
  static const uint8_t init_e8[] = {0x85U, 0x10U, 0x7AU};
  static const uint8_t init_cb[] = {0x39U, 0x2CU, 0x00U, 0x34U, 0x02U};
  static const uint8_t init_ea[] = {0x00U, 0x00U};
  static const uint8_t init_c5[] = {0x30U, 0x30U};
  static const uint8_t init_b1[] = {0x00U, 0x10U};
  static const uint8_t init_b6[] = {0x0AU, 0xA2U};
  static const uint8_t init_e0[] = {0x0FU, 0x2AU, 0x28U, 0x08U, 0x0EU, 0x08U, 0x54U, 0xA9U, 0x43U, 0x0AU, 0x0FU, 0x00U, 0x00U, 0x00U, 0x00U};
  static const uint8_t init_e1[] = {0x00U, 0x15U, 0x17U, 0x07U, 0x11U, 0x06U, 0x2BU, 0x56U, 0x3CU, 0x05U, 0x10U, 0x0FU, 0x3FU, 0x3FU, 0x0FU};

  if ((lcd == NULL) || (hspi == NULL))
  {
    return HAL_ERROR;
  }

  lcd->hspi = hspi;
  lcd->width = ILI9341_WIDTH;
  lcd->height = ILI9341_HEIGHT;

  HAL_GPIO_WritePin(LCD_RST_GPIO_Port, LCD_RST_Pin, GPIO_PIN_SET);
  HAL_Delay(1);
  HAL_GPIO_WritePin(LCD_RST_GPIO_Port, LCD_RST_Pin, GPIO_PIN_RESET);
  HAL_Delay(80);
  HAL_GPIO_WritePin(LCD_RST_GPIO_Port, LCD_RST_Pin, GPIO_PIN_SET);
  HAL_Delay(10);

  if (ILI9341_WriteCommand(lcd, 0xCFU) != HAL_OK || ILI9341_WriteData(lcd, init_cf, sizeof(init_cf)) != HAL_OK) return HAL_ERROR;
  if (ILI9341_WriteCommand(lcd, 0xEDU) != HAL_OK || ILI9341_WriteData(lcd, init_ed, sizeof(init_ed)) != HAL_OK) return HAL_ERROR;
  if (ILI9341_WriteCommand(lcd, 0xE8U) != HAL_OK || ILI9341_WriteData(lcd, init_e8, sizeof(init_e8)) != HAL_OK) return HAL_ERROR;
  if (ILI9341_WriteCommand(lcd, 0xCBU) != HAL_OK || ILI9341_WriteData(lcd, init_cb, sizeof(init_cb)) != HAL_OK) return HAL_ERROR;
  if (ILI9341_WriteCommand(lcd, 0xF7U) != HAL_OK) return HAL_ERROR;
  {
    const uint8_t value = 0x20U;
    if (ILI9341_WriteData(lcd, &value, 1U) != HAL_OK) return HAL_ERROR;
  }
  if (ILI9341_WriteCommand(lcd, 0xEAU) != HAL_OK || ILI9341_WriteData(lcd, init_ea, sizeof(init_ea)) != HAL_OK) return HAL_ERROR;
  if (ILI9341_WriteCommand(lcd, 0xC0U) != HAL_OK) return HAL_ERROR;
  {
    const uint8_t value = 0x1BU;
    if (ILI9341_WriteData(lcd, &value, 1U) != HAL_OK) return HAL_ERROR;
  }
  if (ILI9341_WriteCommand(lcd, 0xC1U) != HAL_OK) return HAL_ERROR;
  {
    const uint8_t value = 0x01U;
    if (ILI9341_WriteData(lcd, &value, 1U) != HAL_OK) return HAL_ERROR;
  }
  if (ILI9341_WriteCommand(lcd, 0xC5U) != HAL_OK || ILI9341_WriteData(lcd, init_c5, sizeof(init_c5)) != HAL_OK) return HAL_ERROR;
  if (ILI9341_WriteCommand(lcd, 0xC7U) != HAL_OK) return HAL_ERROR;
  {
    const uint8_t value = 0xB7U;
    if (ILI9341_WriteData(lcd, &value, 1U) != HAL_OK) return HAL_ERROR;
  }
  if (ILI9341_WriteCommand(lcd, ILI9341_CMD_MADCTL) != HAL_OK) return HAL_ERROR;
  {
    const uint8_t value = 0x08U;
    if (ILI9341_WriteData(lcd, &value, 1U) != HAL_OK) return HAL_ERROR;
  }
  if (ILI9341_WriteCommand(lcd, ILI9341_CMD_PIXFMT) != HAL_OK) return HAL_ERROR;
  {
    const uint8_t value = 0x55U;
    if (ILI9341_WriteData(lcd, &value, 1U) != HAL_OK) return HAL_ERROR;
  }
  if (ILI9341_WriteCommand(lcd, 0xB1U) != HAL_OK || ILI9341_WriteData(lcd, init_b1, sizeof(init_b1)) != HAL_OK) return HAL_ERROR;
  if (ILI9341_WriteCommand(lcd, 0xB6U) != HAL_OK || ILI9341_WriteData(lcd, init_b6, sizeof(init_b6)) != HAL_OK) return HAL_ERROR;
  if (ILI9341_WriteCommand(lcd, 0xF2U) != HAL_OK) return HAL_ERROR;
  {
    const uint8_t value = 0x00U;
    if (ILI9341_WriteData(lcd, &value, 1U) != HAL_OK) return HAL_ERROR;
  }
  if (ILI9341_WriteCommand(lcd, 0x26U) != HAL_OK) return HAL_ERROR;
  {
    const uint8_t value = 0x01U;
    if (ILI9341_WriteData(lcd, &value, 1U) != HAL_OK) return HAL_ERROR;
  }
  if (ILI9341_WriteCommand(lcd, 0xE0U) != HAL_OK || ILI9341_WriteData(lcd, init_e0, sizeof(init_e0)) != HAL_OK) return HAL_ERROR;
  if (ILI9341_WriteCommand(lcd, 0xE1U) != HAL_OK || ILI9341_WriteData(lcd, init_e1, sizeof(init_e1)) != HAL_OK) return HAL_ERROR;
  if (ILI9341_WriteCommand(lcd, 0x35U) != HAL_OK) return HAL_ERROR;
  {
    const uint8_t value = 0x00U;
    if (ILI9341_WriteData(lcd, &value, 1U) != HAL_OK) return HAL_ERROR;
  }
  if (ILI9341_WriteCommand(lcd, ILI9341_CMD_SLPOUT) != HAL_OK) return HAL_ERROR;
  if (ILI9341_WriteCommand(lcd, 0x21U) != HAL_OK) return HAL_ERROR;
  HAL_Delay(120);
  if (ILI9341_WriteCommand(lcd, ILI9341_CMD_DISPON) != HAL_OK) return HAL_ERROR;

  return ILI9341_FillScreen(lcd, ILI9341_COLOR_BLACK);
}

HAL_StatusTypeDef ILI9341_FillScreen(ILI9341_Handle *lcd, uint16_t color)
{
  return ILI9341_FillRect(lcd, 0U, 0U, lcd->width, lcd->height, color);
}

HAL_StatusTypeDef ILI9341_FillRect(ILI9341_Handle *lcd, uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint16_t color)
{
  if ((lcd == NULL) || (width == 0U) || (height == 0U))
  {
    return HAL_ERROR;
  }

  if ((x >= lcd->width) || (y >= lcd->height))
  {
    return HAL_ERROR;
  }

  if ((x + width) > lcd->width)
  {
    width = lcd->width - x;
  }

  if ((y + height) > lcd->height)
  {
    height = lcd->height - y;
  }

  if (ILI9341_SetAddressWindow(lcd, x, y, (uint16_t)(x + width - 1U), (uint16_t)(y + height - 1U)) != HAL_OK)
  {
    return HAL_ERROR;
  }

  return ILI9341_WriteColorBurst(lcd, color, (uint32_t)width * (uint32_t)height);
}

HAL_StatusTypeDef ILI9341_DrawRect(ILI9341_Handle *lcd, uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint16_t color)
{
  if ((width < 2U) || (height < 2U))
  {
    return HAL_ERROR;
  }

  if (ILI9341_FillRect(lcd, x, y, width, 1U, color) != HAL_OK) return HAL_ERROR;
  if (ILI9341_FillRect(lcd, x, (uint16_t)(y + height - 1U), width, 1U, color) != HAL_OK) return HAL_ERROR;
  if (ILI9341_FillRect(lcd, x, y, 1U, height, color) != HAL_OK) return HAL_ERROR;
  return ILI9341_FillRect(lcd, (uint16_t)(x + width - 1U), y, 1U, height, color);
}

HAL_StatusTypeDef ILI9341_DrawChar6x8(ILI9341_Handle *lcd, uint16_t x, uint16_t y, char ch, uint16_t fg, uint16_t bg, uint8_t scale)
{
  const uint8_t *font;
  uint8_t col;
  uint8_t row;
  uint16_t color;

  if ((lcd == NULL) || (scale == 0U))
  {
    return HAL_ERROR;
  }

  font = ILI9341_GetFont6x8(ch);

  for (col = 0U; col < 6U; col++)
  {
    for (row = 0U; row < 8U; row++)
    {
      color = ((font[col] & (1U << row)) != 0U) ? fg : bg;
      if (ILI9341_FillRect(lcd,
                           (uint16_t)(x + (uint16_t)col * scale),
                           (uint16_t)(y + (uint16_t)row * scale),
                           scale,
                           scale,
                           color) != HAL_OK)
      {
        return HAL_ERROR;
      }
    }
  }

  return HAL_OK;
}

HAL_StatusTypeDef ILI9341_DrawString6x8(ILI9341_Handle *lcd, uint16_t x, uint16_t y, const char *str, uint16_t fg, uint16_t bg, uint8_t scale)
{
  if ((lcd == NULL) || (str == NULL) || (scale == 0U))
  {
    return HAL_ERROR;
  }

  while (*str != '\0')
  {
    if (ILI9341_DrawChar6x8(lcd, x, y, *str, fg, bg, scale) != HAL_OK)
    {
      return HAL_ERROR;
    }

    x = (uint16_t)(x + (uint16_t)(6U * scale));
    str++;
  }

  return HAL_OK;
}

HAL_StatusTypeDef ILI9341_DrawStringCentered(ILI9341_Handle *lcd, uint16_t x, uint16_t y, uint16_t width, const char *str, uint16_t fg, uint16_t bg, uint8_t scale)
{
  uint16_t text_width;
  uint16_t start_x;

  if ((lcd == NULL) || (str == NULL) || (scale == 0U))
  {
    return HAL_ERROR;
  }

  text_width = (uint16_t)(strlen(str) * 6U * scale);
  start_x = x;
  if (width > text_width)
  {
    start_x = (uint16_t)(x + ((width - text_width) / 2U));
  }

  return ILI9341_DrawString6x8(lcd, start_x, y, str, fg, bg, scale);
}
