#ifndef STATUS_LED_H
#define STATUS_LED_H

typedef enum {
  STATUS_LED_BOOT = 1,
  STATUS_LED_LWIP_READY = 2,
  STATUS_LED_ETH_LINK_UP = 3,
  STATUS_LED_WEB_READY = 4,
  STATUS_LED_SENSOR_READY = 5,
  STATUS_LED_ETH_RX = 6,
  STATUS_LED_ETH_TX = 7,
  STATUS_LED_ETH_ERROR = 8
} status_led_code_t;

void status_led_init(void);
void status_led_set(status_led_code_t code);
void status_led_start(void);

#endif
