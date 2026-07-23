#include "web_server.h"

#include <stdint.h>
#include <string.h>
#include "lwip/api.h"
#include "qspi_flash.h"
#include "sensor_state.h"
#include "status_led.h"
#include "web_index.h"

#define WEB_TASK_STACK_BYTES 4096u
#define WEB_TASK_PRIORITY    osPriorityNormal
#define HTTP_PORT            80u

static void send_response(struct netconn *conn, const char *type,
                          const char *body)
{
  static const char h1[] = "HTTP/1.1 200 OK\r\nContent-Type: ";
  static const char h2[] = "\r\nConnection: close\r\nCache-Control: no-store\r\n\r\n";

  (void)netconn_write(conn, h1, strlen(h1), NETCONN_COPY);
  (void)netconn_write(conn, type, strlen(type), NETCONN_COPY);
  (void)netconn_write(conn, h2, strlen(h2), NETCONN_COPY);
  (void)netconn_write(conn, body, strlen(body), NETCONN_COPY);
}

static void send_not_found(struct netconn *conn)
{
  static const char body[] = "not found\n";
  static const char header[] =
    "HTTP/1.1 404 Not Found\r\n"
    "Content-Type: text/plain\r\n"
    "Content-Length: 10\r\n"
    "Connection: close\r\n"
    "\r\n";

  (void)netconn_write(conn, header, strlen(header), NETCONN_COPY);
  (void)netconn_write(conn, body, strlen(body), NETCONN_COPY);
}

static void append_char(char **cursor, char *end, char value)
{
  if (*cursor < end) {
    **cursor = value;
    (*cursor)++;
  }
}

static void append_str(char **cursor, char *end, const char *value)
{
  while (*value != '\0') {
    append_char(cursor, end, *value);
    value++;
  }
}

static void append_uint(char **cursor, char *end, uint32_t value)
{
  char tmp[10];
  size_t count = 0;

  do {
    tmp[count++] = (char)('0' + (value % 10u));
    value /= 10u;
  } while (value != 0u && count < sizeof(tmp));

  while (count > 0u) {
    append_char(cursor, end, tmp[--count]);
  }
}

static void append_int(char **cursor, char *end, int32_t value)
{
  if (value < 0) {
    append_char(cursor, end, '-');
    append_uint(cursor, end, (uint32_t)(-value));
  } else {
    append_uint(cursor, end, (uint32_t)value);
  }
}

static void append_milli_g(char **cursor, char *end, int16_t raw)
{
  int32_t milli = ((int32_t)raw * 39) / 10;
  uint32_t frac;

  if (milli < 0) {
    append_char(cursor, end, '-');
    milli = -milli;
  }

  append_uint(cursor, end, (uint32_t)milli / 1000u);
  append_char(cursor, end, '.');
  frac = (uint32_t)milli % 1000u;
  append_char(cursor, end, (char)('0' + (frac / 100u)));
  append_char(cursor, end, (char)('0' + ((frac / 10u) % 10u)));
  append_char(cursor, end, (char)('0' + (frac % 10u)));
}

static void send_sensor_json(struct netconn *conn)
{
  sensor_state_t s = sensor_state_get();
  char body[192];
  char *cursor = body;
  char *end = body + sizeof(body) - 1u;

  append_str(&cursor, end, "{\"valid\":");
  append_str(&cursor, end, s.valid ? "true" : "false");
  append_str(&cursor, end, ",\"seq\":");
  append_uint(&cursor, end, s.sequence);
  append_str(&cursor, end, ",\"x\":");
  append_milli_g(&cursor, end, s.sample.raw_x);
  append_str(&cursor, end, ",\"y\":");
  append_milli_g(&cursor, end, s.sample.raw_y);
  append_str(&cursor, end, ",\"z\":");
  append_milli_g(&cursor, end, s.sample.raw_z);
  append_str(&cursor, end, ",\"rawX\":");
  append_int(&cursor, end, s.sample.raw_x);
  append_str(&cursor, end, ",\"rawY\":");
  append_int(&cursor, end, s.sample.raw_y);
  append_str(&cursor, end, ",\"rawZ\":");
  append_int(&cursor, end, s.sample.raw_z);
  append_str(&cursor, end, "}\n");
  *cursor = '\0';

  send_response(conn, "application/json", body);
}

static void handle_request(struct netconn *conn, const char *request)
{
  if (strncmp(request, "GET /sensor.json ", 17) == 0) {
    send_sensor_json(conn);
  } else if (strncmp(request, "GET / ", 6) == 0 ||
             strncmp(request, "GET /index.html ", 16) == 0) {
    const char *index = app_qspi_web_index_ready() ?
      app_qspi_web_index() : web_index_html;
    send_response(conn, "text/html; charset=utf-8", index);
  } else {
    send_not_found(conn);
  }
}

static void web_task(void *argument)
{
  (void)argument;

  struct netconn *listener = netconn_new(NETCONN_TCP);
  if (listener == NULL) {
    osThreadExit();
  }

  if (netconn_bind(listener, IP_ADDR_ANY, HTTP_PORT) != ERR_OK ||
      netconn_listen(listener) != ERR_OK) {
    netconn_delete(listener);
    osThreadExit();
  }
  status_led_set(STATUS_LED_WEB_READY);

  for (;;) {
    struct netconn *client = NULL;
    if (netconn_accept(listener, &client) != ERR_OK || client == NULL) {
      continue;
    }

    struct netbuf *rx = NULL;
    if (netconn_recv(client, &rx) == ERR_OK && rx != NULL) {
      void *data = NULL;
      u16_t len = 0;
      netbuf_data(rx, &data, &len);
      if (data != NULL && len > 0) {
        char request[96];
        size_t copy_len = len < sizeof(request) - 1u ? len : sizeof(request) - 1u;
        memcpy(request, data, copy_len);
        request[copy_len] = '\0';
        handle_request(client, request);
      }
      netbuf_delete(rx);
    }

    netconn_close(client);
    netconn_delete(client);
  }
}

osThreadId_t web_server_start(void)
{
  const osThreadAttr_t attrs = {
    .name = "webTask",
    .priority = WEB_TASK_PRIORITY,
    .stack_size = WEB_TASK_STACK_BYTES
  };

  return osThreadNew(web_task, NULL, &attrs);
}
