#include "app.h"

#include <stdio.h>
#include <string.h>

#include "main.h"
#include "ft6336.h"
#include "ili9341.h"
#include "light_sensor.h"

extern I2C_HandleTypeDef hi2c1;
extern SPI_HandleTypeDef hspi1;

typedef struct
{
  uint16_t x;
  uint16_t y;
  uint16_t width;
  uint16_t height;
} App_Region;

static ILI9341_Handle g_lcd;
static FT6336_Handle g_touch;
static uint8_t g_touch_ready;
static uint8_t g_touch_active;
static uint16_t g_servo_angle = 90U;
static uint32_t g_last_sensor_tick;
static uint32_t g_button_highlight_until;

static const App_Region g_regions[6] =
{
  {10U, 10U, 105U, 93U},
  {125U, 10U, 105U, 93U},
  {10U, 113U, 105U, 93U},
  {125U, 113U, 105U, 93U},
  {10U, 216U, 105U, 93U},
  {125U, 216U, 105U, 93U}
};

static uint8_t App_PointInRegion(uint16_t x, uint16_t y, const App_Region *region)
{
  return (uint8_t)((x >= region->x) &&
                   (x < (region->x + region->width)) &&
                   (y >= region->y) &&
                   (y < (region->y + region->height)));
}

static void App_ClearRegionContent(const App_Region *region)
{
  if (ILI9341_FillRect(&g_lcd,
                       (uint16_t)(region->x + 4U),
                       (uint16_t)(region->y + 26U),
                       (uint16_t)(region->width - 8U),
                       (uint16_t)(region->height - 30U),
                       ILI9341_COLOR_DARKBLUE) != HAL_OK)
  {
    Error_Handler();
  }
}

static void App_DrawRegionFrame(const App_Region *region, const char *title, uint16_t border_color)
{
  if (ILI9341_FillRect(&g_lcd, region->x, region->y, region->width, region->height, ILI9341_COLOR_DARKBLUE) != HAL_OK)
  {
    Error_Handler();
  }

  if (ILI9341_DrawRect(&g_lcd, region->x, region->y, region->width, region->height, border_color) != HAL_OK)
  {
    Error_Handler();
  }

  if (ILI9341_DrawRect(&g_lcd,
                       (uint16_t)(region->x + 2U),
                       (uint16_t)(region->y + 2U),
                       (uint16_t)(region->width - 4U),
                       (uint16_t)(region->height - 4U),
                       ILI9341_COLOR_LIGHTGRAY) != HAL_OK)
  {
    Error_Handler();
  }

  if (ILI9341_DrawStringCentered(&g_lcd,
                                 region->x,
                                 (uint16_t)(region->y + 8U),
                                 region->width,
                                 title,
                                 ILI9341_COLOR_YELLOW,
                                 ILI9341_COLOR_DARKBLUE,
                                 2U) != HAL_OK)
  {
    Error_Handler();
  }
}

static void App_DrawPlaceholder(const App_Region *region, const char *line1, const char *line2)
{
  App_ClearRegionContent(region);

  if (ILI9341_DrawStringCentered(&g_lcd,
                                 region->x,
                                 (uint16_t)(region->y + 40U),
                                 region->width,
                                 line1,
                                 ILI9341_COLOR_WHITE,
                                 ILI9341_COLOR_DARKBLUE,
                                 2U) != HAL_OK)
  {
    Error_Handler();
  }

  if ((line2 != NULL) && (strlen(line2) > 0U))
  {
    if (ILI9341_DrawStringCentered(&g_lcd,
                                   region->x,
                                   (uint16_t)(region->y + 62U),
                                   region->width,
                                   line2,
                                   ILI9341_COLOR_LIGHTGRAY,
                                   ILI9341_COLOR_DARKBLUE,
                                   1U) != HAL_OK)
    {
      Error_Handler();
    }
  }
}

static void App_DrawButton(const App_Region *region, const char *label, uint8_t highlighted)
{
  uint16_t fill_color = highlighted ? ILI9341_COLOR_ORANGE : ILI9341_COLOR_BLUE;
  uint16_t text_color = highlighted ? ILI9341_COLOR_BLACK : ILI9341_COLOR_WHITE;

  if (ILI9341_FillRect(&g_lcd,
                       (uint16_t)(region->x + 4U),
                       (uint16_t)(region->y + 28U),
                       (uint16_t)(region->width - 8U),
                       (uint16_t)(region->height - 32U),
                       fill_color) != HAL_OK)
  {
    Error_Handler();
  }

  if (ILI9341_DrawRect(&g_lcd,
                       (uint16_t)(region->x + 4U),
                       (uint16_t)(region->y + 28U),
                       (uint16_t)(region->width - 8U),
                       (uint16_t)(region->height - 32U),
                       ILI9341_COLOR_WHITE) != HAL_OK)
  {
    Error_Handler();
  }

  if (ILI9341_DrawStringCentered(&g_lcd,
                                 (uint16_t)(region->x + 4U),
                                 (uint16_t)(region->y + 55U),
                                 (uint16_t)(region->width - 8U),
                                 label,
                                 text_color,
                                 fill_color,
                                 1U) != HAL_OK)
  {
    Error_Handler();
  }
}

static void App_DrawLightValue(void)
{
  uint32_t light_value = 0U;
  char line[16];

  App_ClearRegionContent(&g_regions[0]);

  if (LightSensor_Read(&light_value) == HAL_OK)
  {
    (void)snprintf(line, sizeof(line), "ADC %4lu", (unsigned long)light_value);
    if (ILI9341_DrawStringCentered(&g_lcd,
                                   g_regions[0].x,
                                   (uint16_t)(g_regions[0].y + 40U),
                                   g_regions[0].width,
                                   line,
                                   ILI9341_COLOR_CYAN,
                                   ILI9341_COLOR_DARKBLUE,
                                   2U) != HAL_OK)
    {
      Error_Handler();
    }

    if (ILI9341_DrawStringCentered(&g_lcd,
                                   g_regions[0].x,
                                   (uint16_t)(g_regions[0].y + 66U),
                                   g_regions[0].width,
                                   "PA3 IN8",
                                   ILI9341_COLOR_WHITE,
                                   ILI9341_COLOR_DARKBLUE,
                                   1U) != HAL_OK)
    {
      Error_Handler();
    }
  }
  else
  {
    App_DrawPlaceholder(&g_regions[0], "READ ERR", "CHECK ADC");
  }
}

static void App_DrawServoValue(void)
{
  char line[16];

  App_ClearRegionContent(&g_regions[2]);

  (void)snprintf(line, sizeof(line), "%3u DEG", g_servo_angle);
  if (ILI9341_DrawStringCentered(&g_lcd,
                                 g_regions[2].x,
                                 (uint16_t)(g_regions[2].y + 40U),
                                 g_regions[2].width,
                                 line,
                                 ILI9341_COLOR_GREEN,
                                 ILI9341_COLOR_DARKBLUE,
                                 2U) != HAL_OK)
  {
    Error_Handler();
  }

  if (ILI9341_DrawStringCentered(&g_lcd,
                                 g_regions[2].x,
                                 (uint16_t)(g_regions[2].y + 66U),
                                 g_regions[2].width,
                                 "UI ONLY",
                                 ILI9341_COLOR_LIGHTGRAY,
                                 ILI9341_COLOR_DARKBLUE,
                                 1U) != HAL_OK)
  {
    Error_Handler();
  }
}

static void App_DrawStaticUi(void)
{
  if (ILI9341_FillScreen(&g_lcd, ILI9341_COLOR_BLACK) != HAL_OK)
  {
    Error_Handler();
  }

  App_DrawRegionFrame(&g_regions[0], "LIGHT", ILI9341_COLOR_CYAN);
  App_DrawRegionFrame(&g_regions[1], "TEMP", ILI9341_COLOR_MAGENTA);
  App_DrawRegionFrame(&g_regions[2], "SERVO", ILI9341_COLOR_GREEN);
  App_DrawRegionFrame(&g_regions[3], "BTN 1", ILI9341_COLOR_YELLOW);
  App_DrawRegionFrame(&g_regions[4], "BTN 2", ILI9341_COLOR_YELLOW);
  App_DrawRegionFrame(&g_regions[5], "BTN 3", ILI9341_COLOR_YELLOW);

  App_DrawLightValue();
  App_DrawPlaceholder(&g_regions[1], "TBD", "TEMP HUMI");
  App_DrawServoValue();
  App_DrawButton(&g_regions[3], "ANGLE UP", 0U);
  App_DrawButton(&g_regions[4], "RESERVE", 0U);
  App_DrawButton(&g_regions[5], "RESERVE", 0U);
}

static void App_HandleTouch(void)
{
  FT6336_TouchState state;

  if (g_touch_ready == 0U)
  {
    return;
  }

  if (FT6336_ReadTouch(&g_touch, &state) != HAL_OK)
  {
    return;
  }

  if ((state.touched != 0U) && (g_touch_active == 0U) && App_PointInRegion(state.x, state.y, &g_regions[3]) != 0U)
  {
    g_servo_angle = (uint16_t)((g_servo_angle >= 180U) ? 0U : (g_servo_angle + 15U));
    g_button_highlight_until = HAL_GetTick() + 150U;
    App_DrawServoValue();
    App_DrawButton(&g_regions[3], "ANGLE UP", 1U);
  }

  g_touch_active = state.touched;
}

void App_Init(void)
{
  if (ILI9341_Init(&g_lcd, &hspi1) != HAL_OK)
  {
    Error_Handler();
  }

  g_touch_ready = (uint8_t)(FT6336_Init(&g_touch, &hi2c1) == HAL_OK);
  g_touch_active = 0U;
  g_button_highlight_until = 0U;
  g_last_sensor_tick = HAL_GetTick() - 300U;

  App_DrawStaticUi();
}

void App_Loop(void)
{
  App_HandleTouch();

  if ((g_button_highlight_until != 0U) && ((int32_t)(HAL_GetTick() - g_button_highlight_until) >= 0))
  {
    g_button_highlight_until = 0U;
    App_DrawButton(&g_regions[3], "ANGLE UP", 0U);
  }

  if ((HAL_GetTick() - g_last_sensor_tick) >= 300U)
  {
    g_last_sensor_tick = HAL_GetTick();
    App_DrawLightValue();
  }
}
