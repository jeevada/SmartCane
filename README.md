# SmartCane
## GROUP-08

### TITLE : Blind Vision - A smart cane for visually impaired<br>
GROUP MEMBERS :<br>
  2021/E/011 - ARTHIKA A.<br>
  2021/E/147 - ANUSHANTH S.<br>
  2021/E/161 - AKSHALA T.H.<br>
  2021/E/151 - JEEVAKARAN S.

---

### **Problem Statement**
Visually impaired individuals face numerous challenges in navigating their surroundings independently, which can lead to safety risks, reduced mobility, and limited social engagement. Traditional white canes provide limited assistance by detecting obstacles within a close range and lack advanced capabilities like real-time object detection, navigation, and feedback mechanisms.

---

### **Solution**
"Blind Vision" proposes a smart cane equipped with advanced sensors, IoT capabilities, and machine learning to assist visually impaired individuals. The cane will:
1. Detect obstacles in real-time using ultrasonic sensors and cameras.
2. Provide auditory or haptic feedback to guide users.
3. Integrate with GPS for navigation assistance.
4. Alert users about changes in terrain or potential hazards (e.g., stairs, slopes).
5. Employ wireless communication for emergency alerts to caregivers or emergency services.

---

### **High-Level Architecture**
#### **Technologies**  
1. **Hardware:**
   - **Ultrasonic sensors** for obstacle detection.
   - **Camera module** for object recognition (e.g., Raspberry Pi Camera).
   - **Microcontroller/Processor**: Arduino Nano/ESP32 or Raspberry Pi.
   - **Feedback Mechanisms**: Haptic motor, buzzer, or speaker.
   - **GPS Module** for navigation.
   - **Battery Unit**: Rechargeable Li-ion battery.
   - **Wireless Module**: Bluetooth/Wi-Fi for emergency communication.

2. **Software:**
   - **Machine Learning Models**: Pre-trained object detection models (e.g., YOLO, MobileNet) for obstacle classification.
   - **Navigation Algorithms**: Integration with GPS APIs for route guidance.
   - **Programming Languages**: Python (for ML), C/C++ (for microcontroller firmware).
   - **Mobile App**: Optional app for caregiver connectivity using Flutter or React Native.

3. **Protocols:**
   - **I2C/SPI**: For communication between sensors and microcontroller.
   - **MQTT/HTTP**: For wireless data transmission (alerts or logs).
   - **NMEA Protocol**: For GPS data handling.

---

### **Circuit Design**
#### Components:
- **Ultrasonic Sensor (e.g., HC-SR04):** Connected to Arduino for distance measurement.
- **Camera Module:** Interfaced with Raspberry Pi for object recognition.
- **Vibration Motor:** Controlled by GPIO pins for haptic feedback.
- **Speaker/Buzzer:** For auditory alerts, connected via a transistor circuit.
- **GPS Module:** Communicating over UART.
- **Battery and Power Management:** Voltage regulator to ensure stable power supply.
  
#### Basic Connections:
- Sensors and modules interfaced with GPIO/I2C/UART pins of the microcontroller.
- Central power supply shared via a voltage regulator.
- Wireless module (e.g., ESP32) interfaced for communication.

---

### **Budget**
| **Item**                | **Estimated Cost (USD)** |
|-------------------------|--------------------------|
| Ultrasonic Sensors (x2) | $5                       |
| Camera Module           | $20                      |
| Arduino Nano/ESP32      | $10                      |
| Raspberry Pi (if used)  | $40                      |
| GPS Module              | $15                      |
| Vibration Motor         | $2                       |
| Speaker/Buzzer          | $5                       |
| Rechargeable Battery    | $10                      |
| Misc. Components (Resistors, wires, PCB) | $10 |
| **Total**               | **$117**                |

*Optional*: App development or additional testing may add ~$50.

---

### **Timeline**
| **Phase**            | **Duration** |
|-----------------------|--------------|
| **Research**         | 2 weeks      |
| **Hardware Assembly** | 2 weeks      |
| **Software Development** | 3 weeks |
| **Integration & Testing** | 2 weeks |
| **Final Deployment** | 1 week       |
| **Total**            | **10 weeks** |

---
