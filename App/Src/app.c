#include "app.h"
#include <stdio.h>
#include "main.h"
#include "light_sensor.h"
#include "oled.h"

extern I2C_HandleTypeDef hi2c1;

static OLED_Handle g_oled;
static uint32_t g_last_update_tick;

static void App_UpdateLightDisplay(void)
{
  uint32_t light_value = 0U;
  char line[22];

  if (OLED_ShowString6x8(&g_oled, 0, 0, "Light Sensor        ") != OLED_OK)
  {
    Error_Handler();
  }

  if (LightSensor_Read(&light_value) == HAL_OK)
  {
    (void)snprintf(line, sizeof(line), "PC0 ADC:%4lu   ", (unsigned long)light_value);
    if (OLED_ShowString6x8(&g_oled, 2, 0, line) != OLED_OK)
    {
      Error_Handler();
    }
  }
  else
  {
    if (OLED_ShowString6x8(&g_oled, 2, 0, "PA3 ADC: ERR       ") != OLED_OK)
    {
      Error_Handler();
    }
  }
}

void App_Init(void)
{
  if (OLED_Attach(&g_oled, &hi2c1, OLED_DEFAULT_ADDRESS) != OLED_OK)
  {
    Error_Handler();
  }

  if (OLED_Init(&g_oled) != OLED_OK)
  {
    Error_Handler();
  }

  g_last_update_tick = HAL_GetTick() - 200U;
  App_UpdateLightDisplay();
}

void App_Loop(void)
{
  if ((HAL_GetTick() - g_last_update_tick) < 200U)
  {
    return;
  }

  g_last_update_tick = HAL_GetTick();
  App_UpdateLightDisplay();
}
