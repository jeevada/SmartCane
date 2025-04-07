#include <TinyGPS++.h>
#include <WiFi.h>
#include <PubSubClient.h>

// GPS Hardware Serial Configuration
#define GPS_RX 4
#define GPS_TX 2
HardwareSerial neogps(2);  // UART2 for GPS

TinyGPSPlus gps;

// WiFi Configuration
const char* ssid = "Pravinj7";        // Wi-Fi SSID
const char* password = ""; // Wi-Fi password

// MQTT Configuration
const char* mqttServer = "test.mosquitto.org";  // Can be any MQTT broker accessible over the internet
const int mqttPort = 1883;
const char* mqttTopic = "gpslcation";

WiFiClient wifiClient;
PubSubClient mqttClient(wifiClient);

// Timing Variables
unsigned long lastReconnectAttempt = 0;
const unsigned long reconnectInterval = 10000; // 10 seconds interval to reconnect
unsigned long lastGPSUpdate = 0;
const unsigned long gpsInterval = 10000; // GPS update interval

void setup() {
  Serial.begin(115200);
  while (!Serial); // Wait for serial to initialize

  // Initialize serial port for GPS
  neogps.begin(9600, SERIAL_8N1, GPS_RX, GPS_TX);
  
  Serial.println("\nStarting GPS Tracker...");
  
  // Connect to Wi-Fi
  connectToWiFi();
  
  // Initialize MQTT
  mqttClient.setServer(mqttServer, mqttPort);
  mqttClient.setCallback(mqttCallback);
}

void loop() {
  // Handle Wi-Fi connection
  if (WiFi.status() != WL_CONNECTED) {
    connectToWiFi();
  }

  // Handle MQTT connection
  if (!mqttClient.connected()) {
    unsigned long now = millis();
    if (now - lastReconnectAttempt > reconnectInterval) {
      lastReconnectAttempt = now;
      reconnectMQTT();
    }
  } else {
    mqttClient.loop();
  }

  processGPSData();
}

void connectToWiFi() {
  if (WiFi.status() != WL_CONNECTED) {
    Serial.print("Connecting to Wi-Fi...");
    WiFi.begin(ssid, password);

    while (WiFi.status() != WL_CONNECTED) {
      delay(1000);
      Serial.print(".");
    }

    Serial.println("Connected to Wi-Fi");
  }
}

void reconnectMQTT() {
  if (!mqttClient.connected()) {
    Serial.print("Connecting to MQTT...");

    // Create a unique client ID
    String clientId = "ESP32_GPS_Tracker-" + String(random(0xffff), HEX);

    if (mqttClient.connect(clientId.c_str())) {
      Serial.println("Connected to MQTT!");
      mqttClient.subscribe(mqttTopic);
    } else {
      Serial.print("Failed (");
      Serial.print(mqttClient.state());
      Serial.println(")");
    }
  }
}

void processGPSData() {
  if (millis() - lastGPSUpdate >= gpsInterval) {
    lastGPSUpdate = millis();

    bool newData = false;
    for (unsigned long start = millis(); millis() - start < 2000;) {
      while (neogps.available()) {
        if (gps.encode(neogps.read())) {
          newData = true;
        }
      }
    }

    if (newData && gps.location.isValid() && gps.satellites.value() >= 3) {
      char payload[150];
      snprintf(payload, sizeof(payload),
               "{\"lat\":%.6f,\"lng\":%.6f,\"alt\":%.1f,\"sat\":%d,\"hdop\":%.1f,\"time\":\"%02d:%02d:%02d\"}",
               gps.location.lat(),
               gps.location.lng(),
               gps.altitude.meters(),
               gps.satellites.value(),
               gps.hdop.hdop(),
               gps.time.hour(),
               gps.time.minute(),
               gps.time.second());

      if (mqttClient.connected()) {
        if (mqttClient.publish(mqttTopic, payload)) {
          Serial.print("Published: ");
          Serial.println(payload);
        } else {
          Serial.println("Publish failed");
        }
      }
    } else {
      // Send a message when no valid GPS data is available
      char noDataMessage[] = "No valid GPS data available";
      if (mqttClient.connected()) {
        if (mqttClient.publish(mqttTopic, noDataMessage)) {
          Serial.println("Published: No valid GPS data available");
        } else {
          Serial.println("Publish failed for no data message");
        }
      }
    }
  }
}

void mqttCallback(char* topic, byte* payload, unsigned int length) {
  Serial.print("MQTT Message [");
  Serial.print(topic);
  Serial.print("]: ");
  for (unsigned int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }
  Serial.println();
}

