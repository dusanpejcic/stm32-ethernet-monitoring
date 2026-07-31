# STM32 Ethernet Monitoring System

Embedded monitoring system based on the STM32F750 Discovery board featuring Ethernet connectivity, FreeRTOS multitasking, an HTTP web server, ADXL345 sensor acquisition, and external QuadSPI memory for static web resources.

---

## Features

- FreeRTOS (CMSIS-RTOS v2)
- lwIP TCP/IP stack
- Embedded HTTP server
- ADXL345 accelerometer over I2C
- Static IP Ethernet communication
- External QuadSPI Flash memory mapping
- Memory-mapped HTML resources
- Thread-safe sensor data sharing
- Modular application architecture

---

## Hardware

- STM32F7508-DK Discovery Kit
- ADXL345 accelerometer
- External QuadSPI NOR Flash
- LAN8742 Ethernet PHY

---

## Software Stack

- STM32CubeMX
- STM32 HAL
- FreeRTOS
- lwIP
- GCC ARM Toolchain

---

## Project Architecture

app/
Core/
LWIP/

---

## Software Architecture

Application
│
├── Sensor Task
│
├── HTTP Server
│
├── Sensor State
│
├── ADXL345 Driver
│
└── Status LED Service

---

## FreeRTOS Tasks

| Task | Description |
|------|-------------|
| Default Task | Initializes network stack and application |
| Sensor Task | Periodically reads ADXL345 sensor |
| Ethernet Link Thread | Monitors Ethernet link state |

---

## Memory Layout

Internal Flash

- Application firmware

External QuadSPI Flash

- Embedded HTML page
- Static web resources

---

## Networking

Protocol: HTTP

Static IP:

172.16.1.222

---

## Technologies

- Embedded C
- STM32 HAL
- FreeRTOS
- lwIP
- Ethernet
- HTTP
- I2C
- QuadSPI
- ARM Cortex-M7

---

## Building

Open the project in STM32CubeIDE or regenerate it using the included CubeMX (.ioc) configuration.

---

## License

This project is intended for educational and portfolio purposes.
