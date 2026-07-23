#ifndef QSPI_FLASH_H
#define QSPI_FLASH_H

#include <stdbool.h>
#include "stm32f7xx_hal.h"

bool app_qspi_init(QSPI_HandleTypeDef *hqspi);
const char *app_qspi_web_index(void);
bool app_qspi_web_index_ready(void);

#endif
