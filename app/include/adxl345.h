#ifndef ADXL345_H
#define ADXL345_H

#include <stdbool.h>
#include <stdint.h>
#include "stm32f7xx_hal.h"

#ifndef ADXL345_I2C_ADDR_7BIT
#define ADXL345_I2C_ADDR_7BIT 0x53u
#endif

typedef struct {
  int16_t raw_x;
  int16_t raw_y;
  int16_t raw_z;
  float x_g;
  float y_g;
  float z_g;
} adxl345_sample_t;

typedef struct {
  I2C_HandleTypeDef *i2c;
  uint16_t address;
} adxl345_t;

bool adxl345_init(adxl345_t *dev, I2C_HandleTypeDef *i2c);
bool adxl345_read(adxl345_t *dev, adxl345_sample_t *sample);

#endif

