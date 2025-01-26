# Blind Vision: A Smart Cane for Visually Impaired

## Project Overview
**Blind Vision** is a smart cane designed to assist visually impaired individuals by providing obstacle detection, real-time location tracking, and caregiver communication. This innovative solution combines embedded systems and IoT technologies to enhance user mobility and safety.

---

## Problem Statement
Visually impaired individuals face challenges in navigating unfamiliar environments safely and independently. Traditional canes provide basic assistance but lack features like real-time location updates or caregiver communication. This project addresses these limitations with a caregiver-centric design and modern technologies.

---

## Features
1. **Obstacle Detection**  
   - Utilizes an ultrasonic sensor (HC-SR04) to detect obstacles and alerts users via a buzzer.

2. **Live Environment Monitoring**  
   - Captures surroundings with an ESP32-CAM module for additional caregiver insights.

3. **Real-time Location Tracking**  
   - Employs a GPS module (GY-NEO6MV2) to monitor and share the user's location.

4. **Caregiver Communication**  
   - Sends real-time location updates to caregivers via SMS using a GSM module (SIM800L).

5. **Energy-Efficient Design**  
   - Powered by rechargeable lithium-ion batteries with efficient power management.

---

## Novelty
- Focuses on **caregiver communication** over user feedback for enhanced safety.
- Integrates cost-effective and scalable components for IoT applications.
- Ensures reliable GPS and GSM functionality for remote monitoring.
- Prioritizes portability, low power consumption, and a simplified user interface.

---

## System Architecture
### Core Modules
1. **ESP32-CAM**: Central processing unit with built-in camera, Wi-Fi, and Bluetooth.
2. **Ultrasonic Sensor (HC-SR04)**: For obstacle detection.
3. **GPS Module (GY-NEO6MV2)**: For real-time tracking.
4. **GSM Module (SIM800L)**: For communication with caregivers.
5. **Buzzer**: Audio alerts for immediate feedback.

### Power Management
- Includes a buck converter (LM2596) and voltage regulator (AMS1117-3.3) for efficient power supply.
- Capacitors ensure stable voltage and prevent disruptions.

---

## Circuit Design & Protocols
### Design Principles
- Compact and modular PCB layout.
- Proper grounding to reduce noise and interference.
- Component protection with Schottky diodes and TVS diodes.

### Protocols
- **Power Management**: Stable and efficient power supply.
- **Signal Integrity**: Minimized interference for reliable communication.
- **Safety**: Overcurrent protection and insulation for durability.

---

## Budget
| Component                     | Quantity | Unit Cost (LKR) | Total Cost (LKR) |
|-------------------------------|----------|-----------------|------------------|
| ESP32-CAM                     | 1        | 2160.00         | 2160.00          |
| GSM Module (SIM800L)          | 1        | 1050.00         | 1050.00          |
| GPS Module (GY-NEO6MV2)       | 1        | 900.00          | 900.00           |
| Ultrasonic Sensor (HC-SR04)   | 1        | 230.00          | 230.00           |
| Rechargeable Batteries        | 4        | 190.00          | 760.00           |
| Additional Components         | -        | -               | 2175.00          |
| **Total**                     | -        | -               | **7275.00**      |

---

## Team Members
- **Arthika A.** (2021/E/011)
- **Anushanth S.** (2021/E/147)
- **Akshala T.H.** (2021/E/161)
- **Jeevakaran S.** (2021/E/151)

---

## Repository Information
This repository includes:
1. System design documentation.
2. Circuit schematics.
3. Codebase for ESP32-CAM, GPS, and GSM modules.
4. Testing protocols and results.

Feel free to contribute by creating issues or submitting pull requests.

---

## License
This project is open-source and licensed under the [MIT License](LICENSE).

---

## Acknowledgments
We thank our **EC6020 - Embedded Systems** course instructors for their guidance and support throughout this project.
