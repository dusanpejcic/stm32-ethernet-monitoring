#include "app_main.h"

#include "app_config.h"
#include "cmsis_os2.h"
#include "qspi_flash.h"
#include "sensor_state.h"
#include "sensor_task.h"
#include "web_server.h"

void app_main(void)
{
  (void)app_qspi_init(&APP_QSPI_HANDLE);
  sensor_state_init();
  (void)sensor_task_start(&APP_ADXL_I2C_HANDLE);
  (void)web_server_start();

  for (;;) {
    osDelay(1000);
  }
}
