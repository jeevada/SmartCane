# ESP32-CAM Live Video Streaming Web Server

![ESP32-CAM](https://img.shields.io/badge/ESP32-CAM-blueviolet) ![ESP-IDF](https://img.shields.io/badge/ESP--IDF-v5.0+-orange) ![WiFi](https://img.shields.io/badge/WiFi-Connected-brightgreen)

A live video streaming web server implementation for ESP32-CAM modules using ESP-IDF framework. Supports multiple camera board configurations and provides real-time MJPG streaming over WiFi.

## Features
- 📶 WiFi connectivity with automatic reconnection
- 📷 Camera initialization for multiple ESP32-CAM board variants
- 🌐 HTTP server with MJPEG streaming capability
- ⚙️ Configurable video settings (resolution, quality, FPS)
- 🛠️ Supports multiple ESP32-CAM board configurations:
  - Freenove ESP32-WROVER
  - ESP-EYE
  - AiThinker ESP32-CAM
  - TTGO T-Journal

## Hardware Requirements
- ESP32-CAM module (supported variants listed above)
- OV2640 camera module
- FTDI programmer or compatible USB-to-UART converter
- 5V power supply (minimum 2A recommended)

## Setup Instructions

### Prerequisites
- [ESP-IDF v5.0+](https://docs.espressif.com/projects/esp-idf/en/latest/esp32/get-started/)
- Python 3.7+
- Serial drivers for your USB-to-UART converter

### Installation
1. Clone the repository:
```bash
git clone https://github.com/your-username/esp32-cam-streaming.git
cd esp32-cam-streaming


