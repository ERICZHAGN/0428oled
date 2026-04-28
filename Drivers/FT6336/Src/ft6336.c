#include "ft6336.h"

#define FT6336_REG_TD_STATUS   0x02U
#define FT6336_REG_TOUCH1_XH   0x03U
#define FT6336_REG_CHIP_ID     0xA3U

static HAL_StatusTypeDef FT6336_ReadRegisters(FT6336_Handle *touch, uint8_t reg, uint8_t *buffer, uint16_t size)
{
  if ((touch == NULL) || (touch->hi2c == NULL) || (buffer == NULL))
  {
    return HAL_ERROR;
  }

  return HAL_I2C_Mem_Read(touch->hi2c,
                          (uint16_t)(FT6336_I2C_ADDRESS << 1),
                          reg,
                          I2C_MEMADD_SIZE_8BIT,
                          buffer,
                          size,
                          HAL_MAX_DELAY);
}

HAL_StatusTypeDef FT6336_Init(FT6336_Handle *touch, I2C_HandleTypeDef *hi2c)
{
  if ((touch == NULL) || (hi2c == NULL))
  {
    return HAL_ERROR;
  }

  touch->hi2c = hi2c;
  touch->chip_id = 0U;

  HAL_GPIO_WritePin(TP_RST_GPIO_Port, TP_RST_Pin, GPIO_PIN_RESET);
  HAL_Delay(10);
  HAL_GPIO_WritePin(TP_RST_GPIO_Port, TP_RST_Pin, GPIO_PIN_SET);
  HAL_Delay(100);

  return FT6336_ReadRegisters(touch, FT6336_REG_CHIP_ID, &touch->chip_id, 1U);
}

HAL_StatusTypeDef FT6336_ReadTouch(FT6336_Handle *touch, FT6336_TouchState *state)
{
  uint8_t status;
  uint8_t data[4];

  if ((touch == NULL) || (state == NULL))
  {
    return HAL_ERROR;
  }

  state->touched = 0U;
  state->x = 0U;
  state->y = 0U;

  if (FT6336_ReadRegisters(touch, FT6336_REG_TD_STATUS, &status, 1U) != HAL_OK)
  {
    return HAL_ERROR;
  }

  if ((status & 0x0FU) == 0U)
  {
    return HAL_OK;
  }

  if (FT6336_ReadRegisters(touch, FT6336_REG_TOUCH1_XH, data, sizeof(data)) != HAL_OK)
  {
    return HAL_ERROR;
  }

  state->touched = 1U;
  state->x = (uint16_t)(((uint16_t)(data[0] & 0x0FU) << 8) | data[1]);
  state->y = (uint16_t)(((uint16_t)(data[2] & 0x0FU) << 8) | data[3]);

  return HAL_OK;
}
