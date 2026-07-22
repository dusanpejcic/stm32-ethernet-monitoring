# STM32 Ethernet Monitoring System

Embedded monitoring system based on the STM32F7508-DK development board. The project uses an ADXL345 accelerometer connected over I2C and provides sensor data through Ethernet using the lwIP TCP/IP stack and an embedded HTTP server.

## Overview

The system reads acceleration data from the ADXL345 sensor and makes the collected information available over a local Ethernet network.

The project demonstrates:

- STM32 peripheral configuration
- I2C sensor communication
- Ethernet communication
- lwIP integration
- Embedded HTTP server implementation
- Real-time sensor data acquisition

## Hardware

- STM32F7508-DK development board
- ADXL345 accelerometer
- Ethernet connection

## Communication Interfaces

- I2C
- Ethernet
- RMII

## Software and Tools

- STM32CubeMX
- STM32CubeIDE
- STM32 HAL
- lwIP
- Embedded C

## Current Functionality

- ADXL345 detection over I2C
- Accelerometer data acquisition
- Static IP configuration
- Ethernet communication using lwIP
- Embedded HTTP server
- Sensor data presentation through a web page

## Repository Structure

The repository structure will be documented after the complete project source files are added.

## Author

**Dušan Pejčić**

Electrical Engineering Student  
Interested in Embedded Systems, Firmware Development and Industrial Electronics
