#ifndef LIGHT_SENSOR_H
#define LIGHT_SENSOR_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include "stm32u5xx_hal.h"

HAL_StatusTypeDef LightSensor_Init(void);
HAL_StatusTypeDef LightSensor_Read(uint32_t *value);

#ifdef __cplusplus
}
#endif

#endif /* LIGHT_SENSOR_H */
