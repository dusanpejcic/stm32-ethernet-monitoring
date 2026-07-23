#ifndef SENSOR_TASK_H
#define SENSOR_TASK_H

#include "cmsis_os2.h"
#include "stm32f7xx_hal.h"

osThreadId_t sensor_task_start(I2C_HandleTypeDef *i2c);

#endif

