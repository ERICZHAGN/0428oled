#include "light_sensor.h"

#include "main.h"
#include "stm32u5xx_hal_adc.h"
#include "stm32u5xx_hal_adc_ex.h"

#define LIGHT_SENSOR_ADC_TIMEOUT_MS  500U

extern ADC_HandleTypeDef hadc1;

static uint8_t g_light_sensor_initialized = 0U;

HAL_StatusTypeDef LightSensor_Init(void)
{
  if (g_light_sensor_initialized != 0U)
  {
    return HAL_OK;
  }

  if (HAL_ADCEx_Calibration_Start(&hadc1, ADC_CALIB_OFFSET, ADC_SINGLE_ENDED) != HAL_OK)
  {
    return HAL_ERROR;
  }

  g_light_sensor_initialized = 1U;
  return HAL_OK;
}

HAL_StatusTypeDef LightSensor_Read(uint32_t *value)
{
  if (value == NULL)
  {
    return HAL_ERROR;
  }

  if (LightSensor_Init() != HAL_OK)
  {
    return HAL_ERROR;
  }

  if (HAL_ADC_Start(&hadc1) != HAL_OK)
  {
    return HAL_ERROR;
  }

  if (HAL_ADC_PollForConversion(&hadc1, LIGHT_SENSOR_ADC_TIMEOUT_MS) != HAL_OK)
  {
    (void)HAL_ADC_Stop(&hadc1);
    return HAL_ERROR;
  }

  *value = HAL_ADC_GetValue(&hadc1);

  if (HAL_ADC_Stop(&hadc1) != HAL_OK)
  {
    return HAL_ERROR;
  }

  return HAL_OK;
}
