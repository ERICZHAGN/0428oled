#ifndef FT6336_H
#define FT6336_H

#ifdef __cplusplus
extern "C" {
#endif

#include "main.h"

#define FT6336_I2C_ADDRESS  0x38U

typedef struct
{
  I2C_HandleTypeDef *hi2c;
  uint8_t chip_id;
} FT6336_Handle;

typedef struct
{
  uint8_t touched;
  uint16_t x;
  uint16_t y;
} FT6336_TouchState;

HAL_StatusTypeDef FT6336_Init(FT6336_Handle *touch, I2C_HandleTypeDef *hi2c);
HAL_StatusTypeDef FT6336_ReadTouch(FT6336_Handle *touch, FT6336_TouchState *state);

#ifdef __cplusplus
}
#endif

#endif /* FT6336_H */
