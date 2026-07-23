#include "status_led.h"

#include "cmsis_os2.h"
#include "stm32f7xx_hal.h"

#define STATUS_LED_GPIO_PORT GPIOI
#define STATUS_LED_PIN       GPIO_PIN_1

static volatile status_led_code_t status_code = STATUS_LED_BOOT;

void status_led_init(void)
{
  GPIO_InitTypeDef gpio = {0};

  __HAL_RCC_GPIOI_CLK_ENABLE();

  gpio.Pin = STATUS_LED_PIN;
  gpio.Mode = GPIO_MODE_OUTPUT_PP;
  gpio.Pull = GPIO_NOPULL;
  gpio.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(STATUS_LED_GPIO_PORT, &gpio);
  HAL_GPIO_WritePin(STATUS_LED_GPIO_PORT, STATUS_LED_PIN, GPIO_PIN_RESET);
}

void status_led_set(status_led_code_t code)
{
  if (code > status_code) {
    status_code = code;
  }
}

static void status_led_task(void *argument)
{
  (void)argument;

  for (;;) {
    status_led_code_t code = status_code;

    for (unsigned int i = 0; i < (unsigned int)code; i++) {
      HAL_GPIO_WritePin(STATUS_LED_GPIO_PORT, STATUS_LED_PIN, GPIO_PIN_SET);
      osDelay(120);
      HAL_GPIO_WritePin(STATUS_LED_GPIO_PORT, STATUS_LED_PIN, GPIO_PIN_RESET);
      osDelay(180);
    }

    osDelay(900);
  }
}

void status_led_start(void)
{
  const osThreadAttr_t attrs = {
    .name = "statusLed",
    .priority = osPriorityLow,
    .stack_size = 512
  };

  (void)osThreadNew(status_led_task, NULL, &attrs);
}
