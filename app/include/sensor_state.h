#ifndef SENSOR_STATE_H
#define SENSOR_STATE_H

#include <stdbool.h>
#include <stdint.h>
#include "adxl345.h"

typedef struct {
  adxl345_sample_t sample;
  uint32_t sequence;
  bool valid;
} sensor_state_t;

void sensor_state_init(void);
void sensor_state_update(const adxl345_sample_t *sample);
sensor_state_t sensor_state_get(void);

#endif

