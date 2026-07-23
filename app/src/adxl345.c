#include "adxl345.h"

#define ADXL345_REG_DEVID       0x00u
#define ADXL345_REG_POWER_CTL   0x2Du
#define ADXL345_REG_DATA_FORMAT 0x31u
#define ADXL345_REG_DATAX0      0x32u

#define ADXL345_DEVID           0xE5u
#define ADXL345_MEASURE         0x08u
#define ADXL345_FULL_RES_2G     0x08u
#define ADXL345_I2C_TIMEOUT_MS  50u

static bool write_reg(adxl345_t *dev, uint8_t reg, uint8_t value)
{
  return HAL_I2C_Mem_Write(dev->i2c, dev->address, reg, I2C_MEMADD_SIZE_8BIT,
                           &value, 1, ADXL345_I2C_TIMEOUT_MS) == HAL_OK;
}

static bool read_reg(adxl345_t *dev, uint8_t reg, uint8_t *value)
{
  return HAL_I2C_Mem_Read(dev->i2c, dev->address, reg, I2C_MEMADD_SIZE_8BIT,
                          value, 1, ADXL345_I2C_TIMEOUT_MS) == HAL_OK;
}

bool adxl345_init(adxl345_t *dev, I2C_HandleTypeDef *i2c)
{
  if (dev == NULL || i2c == NULL) {
    return false;
  }

  dev->i2c = i2c;
  dev->address = ADXL345_I2C_ADDR_7BIT << 1;

  uint8_t devid = 0;
  if (!read_reg(dev, ADXL345_REG_DEVID, &devid) || devid != ADXL345_DEVID) {
    return false;
  }

  if (!write_reg(dev, ADXL345_REG_DATA_FORMAT, ADXL345_FULL_RES_2G)) {
    return false;
  }

  return write_reg(dev, ADXL345_REG_POWER_CTL, ADXL345_MEASURE);
}

bool adxl345_read(adxl345_t *dev, adxl345_sample_t *sample)
{
  if (dev == NULL || sample == NULL) {
    return false;
  }

  uint8_t data[6] = {0};
  if (HAL_I2C_Mem_Read(dev->i2c, dev->address, ADXL345_REG_DATAX0,
                       I2C_MEMADD_SIZE_8BIT, data, sizeof(data),
                       ADXL345_I2C_TIMEOUT_MS) != HAL_OK) {
    return false;
  }

  sample->raw_x = (int16_t)((uint16_t)data[1] << 8 | data[0]);
  sample->raw_y = (int16_t)((uint16_t)data[3] << 8 | data[2]);
  sample->raw_z = (int16_t)((uint16_t)data[5] << 8 | data[4]);

  sample->x_g = (float)sample->raw_x * 0.0039f;
  sample->y_g = (float)sample->raw_y * 0.0039f;
  sample->z_g = (float)sample->raw_z * 0.0039f;

  return true;
}
