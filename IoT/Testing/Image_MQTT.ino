#include "esp_camera.h"
#include <WiFi.h>
#include <PubSubClient.h>
#include "Base64.h"

// ===========================
// WiFi Credentials
// ===========================
const char *ssid = "Jeeva";
const char *password = "jeevajeeva";

// ===========================
// MQTT Broker Information
// ===========================
const char *mqtt_server = "test.mosquitto.org"; // Change to your broker
const int mqtt_port = 1883;
const char *mqtt_topic = "esp32cam/image";

// MQTT Client
WiFiClient espClient;
PubSubClient client(espClient);

// ===========================
// Camera Pins (AI Thinker Model)
// ===========================
#define PWDN_GPIO_NUM  32
#define RESET_GPIO_NUM -1
#define XCLK_GPIO_NUM  0
#define SIOD_GPIO_NUM  26
#define SIOC_GPIO_NUM  27

#define Y9_GPIO_NUM    35
#define Y8_GPIO_NUM    34
#define Y7_GPIO_NUM    39
#define Y6_GPIO_NUM    36
#define Y5_GPIO_NUM    21
#define Y4_GPIO_NUM    19
#define Y3_GPIO_NUM    18
#define Y2_GPIO_NUM    5
#define VSYNC_GPIO_NUM 25
#define HREF_GPIO_NUM  23
#define PCLK_GPIO_NUM  22

#define LED_GPIO_NUM   4 // Built-in Flash LED

// ===========================
// Function to Initialize Camera
// ===========================
void setupCamera() {
  camera_config_t config;
  config.ledc_channel = LEDC_CHANNEL_0;
  config.ledc_timer = LEDC_TIMER_0;
  config.pin_d0 = Y2_GPIO_NUM;
  config.pin_d1 = Y3_GPIO_NUM;
  config.pin_d2 = Y4_GPIO_NUM;
  config.pin_d3 = Y5_GPIO_NUM;
  config.pin_d4 = Y6_GPIO_NUM;
  config.pin_d5 = Y7_GPIO_NUM;
  config.pin_d6 = Y8_GPIO_NUM;
  config.pin_d7 = Y9_GPIO_NUM;
  config.pin_xclk = XCLK_GPIO_NUM;
  config.pin_pclk = PCLK_GPIO_NUM;
  config.pin_vsync = VSYNC_GPIO_NUM;
  config.pin_href = HREF_GPIO_NUM;
  config.pin_sccb_sda = SIOD_GPIO_NUM;
  config.pin_sccb_scl = SIOC_GPIO_NUM;
  config.pin_pwdn = PWDN_GPIO_NUM;
  config.pin_reset = RESET_GPIO_NUM;
  config.xclk_freq_hz = 20000000;
  config.pixel_format = PIXFORMAT_JPEG;
  config.frame_size = FRAMESIZE_QVGA;
  config.jpeg_quality = 10;
  config.fb_count = 2;

  if (psramFound()) {
    config.frame_size = FRAMESIZE_SVGA;
    config.jpeg_quality = 10;
    config.fb_count = 2;
  } else {
    config.frame_size = FRAMESIZE_QVGA;
    config.fb_count = 1;
  }

  // Initialize the camera
  esp_err_t err = esp_camera_init(&config);
  if (err != ESP_OK) {
    Serial.printf("Camera init failed with error 0x%x", err);
    return;
  }
}

// ===========================
// Function to Capture and Send Image
// ===========================
void captureAndSendPhoto() {
  Serial.println("Capturing photo...");
  
  camera_fb_t *fb = esp_camera_fb_get();
  if (!fb) {
    Serial.println("Failed to capture image");
    return;
  }

  Serial.printf("Captured Image: %d bytes\n", fb->len);

  // Convert image to Base64
  String encodedImage = base64::encode(fb->buf, fb->len);

  // Publish to MQTT
  Serial.println("Publishing image to MQTT...");
  if (client.publish(mqtt_topic, encodedImage.c_str())) {
    Serial.println("Image sent successfully!");
  } else {
    Serial.println("Failed to send image!");
  }

  esp_camera_fb_return(fb);
}

// ===========================
// Function to Connect to WiFi
// ===========================
void connectWiFi() {
  Serial.print("Connecting to WiFi...");
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWiFi connected!");
}

// ===========================
// Function to Connect to MQTT Broker
// ===========================
void connectMQTT() {
  client.setServer(mqtt_server, mqtt_port);
  Serial.print("Connecting to MQTT...");
  while (!client.connected()) {
    if (client.connect("ESP32-CAM")) {
      Serial.println("connected!");
    } else {
      Serial.print(".");
      delay(1000);
    }
  }
}

// ===========================
// Setup Function
// ===========================
void setup() {
  Serial.begin(115200);
  connectWiFi();
  connectMQTT();
  setupCamera();
}

// ===========================
// Loop Function
// ===========================
void loop() {
  if (!client.connected()) {
    connectMQTT();
  }
  client.loop();
  
  captureAndSendPhoto(); // Take and send a photo
  delay(10000); // Capture every 10 seconds
}
