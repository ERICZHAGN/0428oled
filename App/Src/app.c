#include "app.h"
#include "main.h"
#include "oled.h"

extern I2C_HandleTypeDef hi2c1;

static OLED_Handle g_oled;

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

  if (OLED_ShowString6x8(&g_oled, 0, 0, "HelloWorld") != OLED_OK)
  {
    Error_Handler();
  }
}

void App_Loop(void)
{
}
