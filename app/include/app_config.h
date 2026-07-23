#ifndef APP_CONFIG_H
#define APP_CONFIG_H

#include "stm32f7xx_hal.h"

#ifndef APP_ADXL_I2C_HANDLE
extern I2C_HandleTypeDef hi2c1;
#define APP_ADXL_I2C_HANDLE hi2c1
#endif

#ifndef APP_QSPI_HANDLE
extern QSPI_HandleTypeDef hqspi;
#define APP_QSPI_HANDLE hqspi
#endif

#endif
