#include "sensor_state.h"

#include <string.h>
#include "cmsis_os2.h"

#define FILTER_SHIFT 3
#define FILTER_SCALE (1 << FILTER_SHIFT)

static sensor_state_t state;
static osMutexId_t state_mutex;
static int32_t filtered_x;
static int32_t filtered_y;
static int32_t filtered_z;

void sensor_state_init(void)
{
  memset(&state, 0, sizeof(state));

  const osMutexAttr_t attrs = {
    .name = "sensorStateMutex"
  };
  state_mutex = osMutexNew(&attrs);
}

void sensor_state_update(const adxl345_sample_t *sample)
{
  adxl345_sample_t smoothed;

  if (sample == NULL) {
    return;
  }

  if (state_mutex != NULL) {
    (void)osMutexAcquire(state_mutex, osWaitForever);
  }

  if (!state.valid) {
    filtered_x = (int32_t)sample->raw_x * FILTER_SCALE;
    filtered_y = (int32_t)sample->raw_y * FILTER_SCALE;
    filtered_z = (int32_t)sample->raw_z * FILTER_SCALE;
  } else {
    filtered_x += ((int32_t)sample->raw_x * FILTER_SCALE - filtered_x) >> FILTER_SHIFT;
    filtered_y += ((int32_t)sample->raw_y * FILTER_SCALE - filtered_y) >> FILTER_SHIFT;
    filtered_z += ((int32_t)sample->raw_z * FILTER_SCALE - filtered_z) >> FILTER_SHIFT;
  }

  smoothed.raw_x = (int16_t)(filtered_x / FILTER_SCALE);
  smoothed.raw_y = (int16_t)(filtered_y / FILTER_SCALE);
  smoothed.raw_z = (int16_t)(filtered_z / FILTER_SCALE);
  smoothed.x_g = (float)smoothed.raw_x * 0.0039f;
  smoothed.y_g = (float)smoothed.raw_y * 0.0039f;
  smoothed.z_g = (float)smoothed.raw_z * 0.0039f;

  state.sample = smoothed;
  state.sequence++;
  state.valid = true;

  if (state_mutex != NULL) {
    (void)osMutexRelease(state_mutex);
  }
}

sensor_state_t sensor_state_get(void)
{
  sensor_state_t snapshot;

  if (state_mutex != NULL) {
    (void)osMutexAcquire(state_mutex, osWaitForever);
  }

  snapshot = state;

  if (state_mutex != NULL) {
    (void)osMutexRelease(state_mutex);
  }

  return snapshot;
}
