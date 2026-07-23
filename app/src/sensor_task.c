#include "sensor_task.h"

#include <stddef.h>
#include "adxl345.h"
#include "sensor_state.h"
#include "status_led.h"

#define SENSOR_TASK_STACK_BYTES 2048u
#define SENSOR_TASK_PRIORITY    osPriorityBelowNormal
#define SENSOR_PERIOD_MS        50u

typedef struct {
  I2C_HandleTypeDef *i2c;
} sensor_task_context_t;

static sensor_task_context_t context;

static void sensor_task(void *argument)
{
  sensor_task_context_t *ctx = (sensor_task_context_t *)argument;
  adxl345_t adxl;
  adxl345_sample_t sample;

  while (!adxl345_init(&adxl, ctx->i2c)) {
    osDelay(500);
  }
  status_led_set(STATUS_LED_SENSOR_READY);

  for (;;) {
    if (adxl345_read(&adxl, &sample)) {
      sensor_state_update(&sample);
    }
    osDelay(SENSOR_PERIOD_MS);
  }
}

osThreadId_t sensor_task_start(I2C_HandleTypeDef *i2c)
{
  context.i2c = i2c;

  const osThreadAttr_t attrs = {
    .name = "sensorTask",
    .priority = SENSOR_TASK_PRIORITY,
    .stack_size = SENSOR_TASK_STACK_BYTES
  };

  return osThreadNew(sensor_task, &context, &attrs);
}
