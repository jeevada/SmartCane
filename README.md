# SmartCane
## GROUP-08

### TITLE : Blind Vision - A smart cane for visually impaired<br>

GROUP MEMBERS :<br>
  2021/E/011 - ARTHIKA A.<br>
  2021/E/147 - ANUSHANTH S.<br>
  2021/E/161 - AKSHALA T.H.<br>
  2021/E/151 - JEEVAKARAN S.

---

### **Problem**
Visually impaired individuals often face challenges in navigating unfamiliar environments safely and independently. Traditional mobility aids like canes provide basic physical assistance but fail to address modern safety requirements, such as real-time location updates or communication with caregivers.

Existing smart solutions for visually impaired individuals often rely on audio feedback for guidance, which may not always be practical in noisy environments or for users with hearing impairments. There is a clear need for a smart cane that enhances user safety by informing caregivers about the user's location, ensuring rapid assistance during emergencies without adding complexities for the user.


---

### **Solution**
The **Blind vision: a smart cane for visually impaired** project provides an innovative solution by combining embedded systems and smart technologies to enhance mobility and safety for visually impaired individuals. The smart cane includes the following features:

1. Obstacle Detection:
    * An ultrasonic sensor detects obstacles in the user's path and alerts them via a buzzer to avoid collisions.
    * The sensor measures distance and provides timely warnings to the user.
 
2.	Live Environment Monitoring:
    * The ESP32-CAM module captures real-time images or videos of the user's surroundings, which can be transmitted to caregivers if needed.
    * This feature can also assist in identifying objects or places.

3.	Real-time Location Tracking:
    * Using a GPS module to continuously gather and update the user’s location to the care taker.

4.	Caregiver Communication:
    * A GSM module that transmits the user’s location directly to their caregiver via SMS, ensuring the caregiver is promptly informed of the user’s whereabouts.

5.	Simplified Design: 
    * The cane avoids using audio feedback or speakers to inform the user, focusing solely on transmitting location details to the caregiver.
    * The entire system is integrated into a lightweight cane powered by a rechargeable battery, ensuring portability and ease of use.

---
### NOVELTY
The **"BLIND VISION: A SMART CANE FOR VISUALLY IMPAIRED"** introduces several novel features and design choices that set it apart from traditional and existing smart mobility aids:
1.	**Focus on Caregiver Communication Instead of User Feedback**
    * Unlike conventional smart canes that provide audio or vibration feedback to the visually impaired user, this system prioritizes **transmitting real-time location information to caregivers**.
    * By eliminating audio feedback, the cane avoids reliance on sensory cues that might not be effective in noisy or chaotic environments, making it simpler and more reliable for users.
2.	**Compact and Efficient Design with IoT Integration**
    * The use of the **ESP32-CAM**, a compact and powerful microcontroller with built-in Wi-Fi capabilities, ensures that the system is not only portable but also scalable for future IoT applications.
    * While it doesn’t currently use Wi-Fi connectivity, this choice makes it possible to enhance functionality in future iterations (e.g., integration with cloud-based tracking systems).
3.	**Low-Cost, Accessible Technology**
    * Components such as the GPS module (GY-NEO6MV2) and GSM module (SIM800L) are chosen for their affordability and ease of integration. This makes the system cost-effective, especially important for accessibility in underserved or economically constrained communities.
4.	**Highly Reliable GPS and GSM Integration**
    * The combination of GPS tracking and GSM-based communication ensures that location data is transmitted effectively, even in low-connectivity or remote areas, **offering real-time caregiver updates** wherever cellular coverage is available.
    * This approach allows caregivers to provide timely assistance in emergencies or monitor the user’s movements proactively.
5.	**Energy Efficiency and Portability**
    * The system design includes **a buck converter and voltage regulator** for efficient power management, ensuring long battery life and uninterrupted operation during extended use.
    * The lightweight design of the cane and compact integration of all components make it a highly portable and practical solution for everyday use.
6.	**User-Centered Design Philosophy**
    * By focusing solely on caregiver communication rather than user interaction, the system reduces cognitive load for the visually impaired user, allowing them to focus entirely on their navigation.
    * The absence of audio feedback also makes it inclusive for users who may have both visual and auditory impairments.

The system's **novelty lies in its caregiver-centric approach**. Unlike most smart canes that primarily assist the user, this design aims to create a **safety net through real-time tracking and caregiver communication, enabling external support when needed**. This innovative perspective broadens the scope of smart cane applications, transitioning from user-only solutions to a collaborative safety model.

---

### **HIGH LEVEL ARCHITECTURE**
The high-level architecture of the system is based on the integration of multiple modules for sensing, processing, and communication. The technologies and components were chosen based on their compatibility, functionality, cost-efficiency, and ability to meet the requirements of the project.

1. **Core Processing Unit: ESP32-CAM**
  * **Why Chosen**:
    * **Dual Functionality** : The ESP32-CAM combines processing capabilities with a built-in camera, eliminating the need for an additional microcontroller or camera module.
    * **Low Power Consumption**: Its efficient design makes it ideal for battery-powered projects.
    * **Wi-Fi and Bluetooth Support**: Enables wireless connectivity for future expansions like app-based control.
    * **Compact Size**: Perfect for embedding in a lightweight, portable device like a smart cane.

2. **Obstacle Detection Module: Ultrasonic Sensor (HC-SR04)**
  * **Why Chosen**:
    * **Accuracy**: Provides precise distance measurements up to 4 meters, essential for detecting obstacles in real-time.
    * **Simplicity** : Easy to interface with the ESP32-CAM, requiring minimal setup and coding.
    * **Reliability**: Performs well in various lighting and environmental conditions, ensuring dependable obstacle detection.

3. **Location Tracking Module: GY-NEO6MV2 GPS**
  * **Why Chosen**:
    * **Real-Time Tracking**: Delivers accurate latitude and longitude data, crucial for location sharing in emergencies.
    * **Low Cost**: An affordable yet reliable GPS module for basic navigation needs.
    * **Wide Compatibility**: Easily interfaces with the ESP32 for data transmission.

4. **Communication Module: SIM800L GSM**
  * **Why Chosen**:
    * **Emergency Connectivity**: Allows SMS-based communication for alerting caregivers or emergency services with GPS coordinates.
    * **Versatile Network Support**: Works on most mobile networks globally.
    * **Compact Design**: Fits seamlessly into a portable device.

5. **Alert System: Buzzer**
  * **Why Chosen**
    * **Immediate Feedback**: Provides audio alerts for obstacle proximity, ensuring quick response from the user.
    * **Cost-Effective**: A simple and efficient way to alert users without requiring additional complex systems.
    * **Customizable Alerts**: Can produce different sound patterns based on the distance of obstacles.

6. **Power Supply: Rechargeable Batteries**
  * Why Chosen:
    * **Portability**: Batteries make the device independent of external power sources.
    * **Rechargeable Design**: Reduces operational costs and environmental impact.
    * **Sufficient Voltage**: Provides stable power for all modules, including the ESP32, GSM, GPS, and sensors.

#### System Design Choices
1.	**ESP32-CAM as the Heart of the System**:
    * It was chosen for its versatility, combining computational power with a camera, Wi-Fi, and Bluetooth in a single module.
    * Simplifies the architecture by reducing the need for additional components like external microcontrollers or image-processing modules.
2.	Modular Approach:
    * Each functionality (obstacle detection, GPS, GSM) is handled by a separate module, making the system easier to debug, upgrade, and maintain.
3.	Low-Power, Lightweight Design:
    * Ensures the device is portable and suitable for extended use without frequent recharging.
4.	**Cost Efficiency**:
    * The selected components strike a balance between performance and affordability, making the smart cane accessible to a broader audience.

By using these technologies, the architecture ensures the smart cane is **efficient, reliable, and scalable**, catering to the needs of visually impaired individuals while keeping the system cost-effective and future-ready.

---

### **CIRCUIT DESIGN & PROTOCOLS**
#### Circuit design:

![image](https://github.com/user-attachments/assets/42c18cbd-d3f1-4588-bcb0-af403bbafb77)

  
#### Protocols
We have decided to follow these protocols to ensure the proper functioning, safety, and reliability of our smart cane circuit:
1.	**Power Management Protocols**
    * We will incorporate a **rechargeable lithium-ion battery** to power the entire system, ensuring portability and convenience.
    * A **buck converter** will be used to efficiently step down voltage and reduce power loss, while the **AMS1117-3.3 voltage regulator** will stabilize the supply voltage to 3.3V for the ESP32-CAM, GPS module, and GSM module.
    * We plan to add capacitors near the voltage regulator and buck converter to filter any voltage ripples, ensuring smooth and stable operation of the components.
2.	**Signal Integrity Protocols**
    * We will ensure proper grounding throughout the circuit to minimize noise and interference, particularly between the GSM and GPS modules.
    * Pull-up resistors will be used on communication lines where necessary to ensure stable signal transitions, avoiding floating inputs.
    * Shielded cables or appropriate routing will be considered to reduce cross-talk and electromagnetic interference (EMI) caused by the GSM module during operation.
3.	**Component Protection Protocols**
    * To protect sensitive components like the ESP32-CAM and modules, we will include **Schottky diodes** at critical points to prevent reverse current.
    * We have decided to integrate **resistors** to safeguard against short circuits or overcurrent situations.
    * **A TVS (Transient Voltage Suppressor) diode** may be added to protect against voltage spikes, especially on the GSM module's input.
4. **PCB Design Protocols**
    * We will design a **compact PCB layout** to accommodate all components, ensuring minimal wiring complexity and enhanced portability.
    * The layout will follow proper spacing rules to prevent short circuits and accommodate heat dissipation for power-related components.
    * Separate ground planes will be used for the digital and analog sections to prevent noise from affecting the performance of the GPS module.
5.	**Testing Protocols**
    * We plan to simulate the circuit using software such as Proteus or TinkerCAD before building the physical prototype to identify potential issues.
    * Once the hardware is built, we will conduct tests at different voltage levels to ensure the power supply system is stable and reliable.
    * We will also test communication between the ESP32-CAM, GPS module, and GSM module in different scenarios, such as weak cellular signal areas, to verify robustness.
6.	Safety Protocols
    * Heat sinks or thermal pads will be considered for components like the buck converter and voltage regulator to prevent overheating during extended usage.
    * Proper insulation will be added to exposed wires and solder joints to avoid accidental short circuits.
    * A casing for the PCB and other components will be designed to protect the circuitry from environmental damage like dust and moisture.

By adhering to these protocols, we aim to build a system that is safe, reliable, and optimized for long-term operation while meeting the project's goals.

---

### BUDGET

| **Component**                     | **Quantity** | **Unit Cost (LKR)** | **Estimated Cost (LKR)** |
|-----------------------------------|--------------|----------------------|--------------------------|
| ESP-32 CAM Microcontroller        | 1            | 2160.00             | 2160.00                 |
| GSM Module (SIM800L)              | 1            | 1050.00             | 1050.00                 |
| GPS Module (GY-NEO6MV2)           | 1            | 900.00              | 900.00                  |
| Ultrasonic Sensor (HC-SR04)       | 1            | 230.00              | 230.00                  |
| Rechargeable Lithium-Ion Battery  | 4            | 190.00              | 760.00                  |
| Battery Holder                    | 1            | 180.00              | 180.00                  |
| Battery Charger Module            | 1            | 60.00               | 60.00                   |
| Buzzer                            | 1            | 50.00               | 40.00                   |
| USB-TTL Converter                 | 1            | 420.00              | 420.00                  |
| Voltage Regulator (AMS1117-3.3)   | 1            | 15.00               | 15.00                   |
| Buck Converter (LM2596)           | 1            | 70.00               | 70.00                   |
| Capacitors - 10µF                 | 5            | 3.00                | 15.00                   |
| Capacitors - 22µF                 | 5            | 3.00                | 15.00                   |
| Capacitors - 47µF                 | 5            | 3.00                | 15.00                   |
| Capacitors - 100µF                | 5            | 3.00                | 15.00                   |
| Capacitors - 220µF                | 5            | 4.00                | 20.00                   |
| Decoupling Capacitors – 0.1µF     | 5            | 5.00                | 25.00                   |
| Resistor - 10kΩ                   | 20           | 3.00                | 60.00                   |
| Resistor - 15kΩ                   | 20           | 3.00                | 60.00                   |
| Inductor - 100 µH                 | 3            | 70.00               | 210.00                  |
| Schottky Diodes (IN5822)          | 5            | 15.00               | 75.00                   |
| Schottky Diodes (IN4007)          | 5            | 2.00                | 10.00                   |
| Transient Voltage Supressor       | 1            | 30.00               | 30.00                   |
| Switch                            | 1            | 50.00               | 50.00                   |
| Jumper Wire (M-M)                 | 40           | 4.00                | 160.00                  |
| Jumper Wire (F-M)                 | 40           | 4.00                | 160.00                  |
| Jumper Wire (F-F)                 | 40           | 4.00                | 160.00                  |
| Breadboard                        | 1            | 310.00              | 310.00                  |
| **Total**                         |              |                      | **7275.00**             |

---

### **Timeline**

![image](https://github.com/user-attachments/assets/78f138cd-a6a3-44e9-bf08-d7500523950a)

---
