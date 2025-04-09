#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "esp_system.h"
#include "esp_wifi.h"
#include "esp_event.h"
#include "esp_log.h"
#include "nvs_flash.h"
#include "driver/uart.h"
#include "driver/gpio.h"
#include "driver/timer.h"
#include "esp_timer.h"
#include "lwip/err.h"
#include "lwip/sys.h"
#include "mqtt_client.h"
#include "esp_rom_sys.h"

#define TAG "GPS_PROXIMITY_TRACKER"

// Ultrasonic sensor pins and configuration
#define TRIG_PIN              GPIO_NUM_25
#define ECHO_PIN              GPIO_NUM_26
#define BUZZER_PIN            GPIO_NUM_23
#define THRESHOLD_CM          15          // Threshold distance in cm

// UART Configuration for NEO-6M GPS
#define GPS_UART_NUM          UART_NUM_2
#define GPS_TX_PIN            2
#define GPS_RX_PIN            4
#define GPS_UART_BUF_SIZE     1024
#define GPS_BAUD_RATE         9600

// Wi-Fi Configuration
#define WIFI_SSID             "Akshi"
#define WIFI_PASS             "Anushanth04"
#define WIFI_CONNECTED_BIT    BIT0
#define WIFI_FAIL_BIT         BIT1
#define WIFI_MAXIMUM_RETRY    5

// MQTT Configuration
#define MQTT_BROKER           "test.mosquitto.org"
#define MQTT_PORT             1883
#define MQTT_TOPIC            "gpslocation"
#define MQTT_PROXIMITY_TOPIC  "proximityalert"
#define GPS_UPDATE_INTERVAL   10000      // 10 seconds

// Wi-Fi event group
static EventGroupHandle_t s_wifi_event_group;
static int s_retry_num = 0;

// MQTT client handle
static esp_mqtt_client_handle_t mqtt_client = NULL;
static bool mqtt_connected = false;

// GPS data structure
typedef struct {
    float latitude;
    float longitude;
    float altitude;
    int satellites;
    float hdop;
    int hour;
    int minute;
    int second;
    bool is_valid;
} gps_data_t;

// GPS data
static gps_data_t gps_data = {
    .is_valid = false
};

// Proximity alert state
static bool proximity_alert_active = false;

// Function prototypes
static void wifi_event_handler(void* arg, esp_event_base_t event_base, int32_t event_id, void* event_data);
static void mqtt_event_handler(void *handler_args, esp_event_base_t base, int32_t event_id, void *event_data);
static void init_wifi(void);
static void init_mqtt(void);
static void gps_task(void *pvParameters);
static void mqtt_publish_task(void *pvParameters);
static void ultrasonic_task(void *pvParameters);
static void ultrasonic_init(void);
static float get_distance(void);
static void buzzer_init(void);
static void activate_buzzer(void);
static bool parse_gps_data(const char* nmea_sentence);
static bool parse_gpgga(const char* sentence);
static bool parse_gprmc(const char* sentence);
static float parse_coordinate(const char* coord_str, char direction);

// Wi-Fi event handler
static void wifi_event_handler(void* arg, esp_event_base_t event_base, int32_t event_id, void* event_data)
{
    if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_START) {
        esp_wifi_connect();
    } else if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_DISCONNECTED) {
        if (s_retry_num < WIFI_MAXIMUM_RETRY) {
            esp_wifi_connect();
            s_retry_num++;
            ESP_LOGI(TAG, "Retrying to connect to the AP");
        } else {
            xEventGroupSetBits(s_wifi_event_group, WIFI_FAIL_BIT);
        }
        ESP_LOGI(TAG,"Connect to the AP fail");
    } else if (event_base == IP_EVENT && event_id == IP_EVENT_STA_GOT_IP) {
        ip_event_got_ip_t* event = (ip_event_got_ip_t*) event_data;
        ESP_LOGI(TAG, "Got IP:" IPSTR, IP2STR(&event->ip_info.ip));
        s_retry_num = 0;
        xEventGroupSetBits(s_wifi_event_group, WIFI_CONNECTED_BIT);
    }
}

// MQTT event handler
static void mqtt_event_handler(void *handler_args, esp_event_base_t base, int32_t event_id, void *event_data)
{
    esp_mqtt_event_handle_t event = event_data;
    
    switch ((esp_mqtt_event_id_t)event_id) {
        case MQTT_EVENT_CONNECTED:
            ESP_LOGI(TAG, "MQTT connected");
            mqtt_connected = true;
            break;
        
        case MQTT_EVENT_DISCONNECTED:
            ESP_LOGI(TAG, "MQTT disconnected");
            mqtt_connected = false;
            break;
        
        case MQTT_EVENT_PUBLISHED:
            ESP_LOGI(TAG, "MQTT message published successfully, msg_id=%d", event->msg_id);
            break;
        
        case MQTT_EVENT_ERROR:
            ESP_LOGI(TAG, "MQTT error");
            break;
        
        default:
            break;
    }
}

// Initialize Wi-Fi
static void init_wifi(void)
{
    s_wifi_event_group = xEventGroupCreate();

    ESP_ERROR_CHECK(esp_netif_init());
    ESP_ERROR_CHECK(esp_event_loop_create_default());
    esp_netif_create_default_wifi_sta();

    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));

    ESP_ERROR_CHECK(esp_event_handler_register(WIFI_EVENT, ESP_EVENT_ANY_ID, &wifi_event_handler, NULL));
    ESP_ERROR_CHECK(esp_event_handler_register(IP_EVENT, IP_EVENT_STA_GOT_IP, &wifi_event_handler, NULL));

    wifi_config_t wifi_config = {
        .sta = {
            .ssid = WIFI_SSID,
            .password = WIFI_PASS,
            .threshold.authmode = WIFI_AUTH_WPA2_PSK,
        },
    };
    
    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
    ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_STA, &wifi_config));
    ESP_ERROR_CHECK(esp_wifi_start());

    ESP_LOGI(TAG, "wifi_init_sta finished.");

    // Wait for connection
    EventBits_t bits = xEventGroupWaitBits(s_wifi_event_group,
            WIFI_CONNECTED_BIT | WIFI_FAIL_BIT,
            pdFALSE,
            pdFALSE,
            portMAX_DELAY);

    if (bits & WIFI_CONNECTED_BIT) {
        ESP_LOGI(TAG, "Connected to AP SSID:%s", WIFI_SSID);
    } else if (bits & WIFI_FAIL_BIT) {
        ESP_LOGI(TAG, "Failed to connect to SSID:%s", WIFI_SSID);
    } else {
        ESP_LOGE(TAG, "Unexpected event");
    }
}

// Initialize MQTT
static void init_mqtt(void)
{
    esp_mqtt_client_config_t mqtt_cfg = {
        .broker.address.uri = "mqtt://" MQTT_BROKER,
        .broker.address.port = MQTT_PORT,
    };

    mqtt_client = esp_mqtt_client_init(&mqtt_cfg);
    esp_mqtt_client_register_event(mqtt_client, ESP_EVENT_ANY_ID, mqtt_event_handler, NULL);
    esp_mqtt_client_start(mqtt_client);
}

// Function to initialize the ultrasonic sensor
static void ultrasonic_init(void) 
{
    gpio_config_t io_conf = {};
    
    // Configure TRIG pin as output
    io_conf.intr_type = GPIO_INTR_DISABLE;
    io_conf.mode = GPIO_MODE_OUTPUT;
    io_conf.pin_bit_mask = (1ULL << TRIG_PIN);
    io_conf.pull_down_en = 0;
    io_conf.pull_up_en = 0;
    gpio_config(&io_conf);
    
    // Configure ECHO pin as input
    io_conf.intr_type = GPIO_INTR_DISABLE;
    io_conf.mode = GPIO_MODE_INPUT;
    io_conf.pin_bit_mask = (1ULL << ECHO_PIN);
    io_conf.pull_down_en = 0;
    io_conf.pull_up_en = 0;
    gpio_config(&io_conf);
    
    ESP_LOGI(TAG, "Ultrasonic sensor initialized");
}

// Function to get the distance from the ultrasonic sensor
static float get_distance(void) 
{
    // Send a pulse to the TRIG pin to start measurement
    gpio_set_level(TRIG_PIN, 0); // Make sure TRIG is LOW
    vTaskDelay(2 / portTICK_PERIOD_MS);
    gpio_set_level(TRIG_PIN, 1); // Send the pulse
	esp_rom_delay_us(10);  // 10 microseconds pulse
    gpio_set_level(TRIG_PIN, 0); // Stop sending the pulse
    
    // Read the ECHO pin
    int64_t start_time = esp_timer_get_time();  // Capture start time
    
    // Set timeout for echo signal (25ms - corresponds to ~4m)
    int64_t timeout = start_time + 25000; 
    
    // Wait for ECHO to go HIGH with timeout
    while (gpio_get_level(ECHO_PIN) == 0) {
        start_time = esp_timer_get_time();
        if (start_time > timeout) {
            return -1; // Return error value if timeout
        }
    }
    
    int64_t end_time = esp_timer_get_time();    // Capture end time
    
    // Wait for ECHO to go LOW with timeout
    while (gpio_get_level(ECHO_PIN) == 1) {
        end_time = esp_timer_get_time();
        if (end_time > timeout + 25000) { // Additional 25ms timeout
            return -1;  // Return error value if timeout
        }
    }
    
    // Calculate the duration of the pulse in microseconds
    int64_t duration = end_time - start_time;
    
    // Calculate the distance in centimeters (speed of sound = 34300 cm/s)
    float distance = (duration * 0.0343) / 2;
    
    return distance; // Return the distance in cm
}

// Function to initialize the buzzer
static void buzzer_init(void) 
{
    gpio_config_t io_conf = {};
    
    // Configure BUZZER pin as output
    io_conf.intr_type = GPIO_INTR_DISABLE;
    io_conf.mode = GPIO_MODE_OUTPUT;
    io_conf.pin_bit_mask = (1ULL << BUZZER_PIN);
    io_conf.pull_down_en = 0;
    io_conf.pull_up_en = 0;
    gpio_config(&io_conf);
    
    // Initialize buzzer to OFF state
    gpio_set_level(BUZZER_PIN, 0);
    
    ESP_LOGI(TAG, "Buzzer initialized");
}

// Function to activate the buzzer
static void activate_buzzer(void) 
{
    gpio_set_level(BUZZER_PIN, 1); // Turn on the buzzer
    vTaskDelay(100 / portTICK_PERIOD_MS); // Keep it on for 100ms
    gpio_set_level(BUZZER_PIN, 0); // Turn off the buzzer
    vTaskDelay(100 / portTICK_PERIOD_MS); // Off for 100ms
}

// Parse GPS data
static bool parse_gps_data(const char* nmea_sentence)
{
    if (strncmp(nmea_sentence, "$GPGGA", 6) == 0) {
        return parse_gpgga(nmea_sentence);
    } else if (strncmp(nmea_sentence, "$GPRMC", 6) == 0) {
        return parse_gprmc(nmea_sentence);
    }
    return false;
}

// Parse coordinate from NMEA format to decimal degrees
static float parse_coordinate(const char* coord_str, char direction)
{
    if (strlen(coord_str) == 0) {
        return 0.0;
    }
    
    float coord = atof(coord_str);
    int degrees = (int)(coord / 100);
    float minutes = coord - (degrees * 100);
    float decimal_degrees = degrees + (minutes / 60.0);
    
    if (direction == 'S' || direction == 'W') {
        decimal_degrees = -decimal_degrees;
    }
    
    return decimal_degrees;
}

// Parse GPGGA sentence (Global Positioning System Fix Data)
static bool parse_gpgga(const char* sentence)
{
    char* token;
    char* save_ptr;
    char sentence_copy[128];
    
    strncpy(sentence_copy, sentence, sizeof(sentence_copy) - 1);
    sentence_copy[sizeof(sentence_copy) - 1] = '\0';
    
    // First token is the sentence type
    token = strtok_r(sentence_copy, ",", &save_ptr);
    if (!token) return false;
    
    // Time
    token = strtok_r(NULL, ",", &save_ptr);
    if (!token) return false;
    if (strlen(token) >= 6) {
        gps_data.hour = (token[0] - '0') * 10 + (token[1] - '0');
        gps_data.minute = (token[2] - '0') * 10 + (token[3] - '0');
        gps_data.second = (token[4] - '0') * 10 + (token[5] - '0');
    }
    
    // Latitude
    token = strtok_r(NULL, ",", &save_ptr);
    if (!token) return false;
    char* lat_direction = strtok_r(NULL, ",", &save_ptr);
    if (!lat_direction) return false;
    
    if (strlen(token) > 0 && strlen(lat_direction) > 0) {
        gps_data.latitude = parse_coordinate(token, lat_direction[0]);
    }
    
    // Longitude
    token = strtok_r(NULL, ",", &save_ptr);
    if (!token) return false;
    char* lng_direction = strtok_r(NULL, ",", &save_ptr);
    if (!lng_direction) return false;
    
    if (strlen(token) > 0 && strlen(lng_direction) > 0) {
        gps_data.longitude = parse_coordinate(token, lng_direction[0]);
    }
    
    // Fix quality
    token = strtok_r(NULL, ",", &save_ptr);
    if (!token) return false;
    int fix_quality = atoi(token);
    gps_data.is_valid = (fix_quality > 0);
    
    // Number of satellites
    token = strtok_r(NULL, ",", &save_ptr);
    if (!token) return false;
    gps_data.satellites = atoi(token);
    
    // HDOP
    token = strtok_r(NULL, ",", &save_ptr);
    if (!token) return false;
    gps_data.hdop = atof(token);
    
    // Altitude
    token = strtok_r(NULL, ",", &save_ptr);
    if (!token) return false;
    gps_data.altitude = atof(token);
    
    return gps_data.is_valid;
}

// Parse GPRMC sentence (Recommended Minimum Specific GPS/Transit Data)
static bool parse_gprmc(const char* sentence)
{
    char* token;
    char* save_ptr;
    char sentence_copy[128];
    
    strncpy(sentence_copy, sentence, sizeof(sentence_copy) - 1);
    sentence_copy[sizeof(sentence_copy) - 1] = '\0';
    
    // First token is the sentence type
    token = strtok_r(sentence_copy, ",", &save_ptr);
    if (!token) return false;
    
    // Time
    token = strtok_r(NULL, ",", &save_ptr);
    if (!token) return false;
    if (strlen(token) >= 6) {
        gps_data.hour = (token[0] - '0') * 10 + (token[1] - '0');
        gps_data.minute = (token[2] - '0') * 10 + (token[3] - '0');
        gps_data.second = (token[4] - '0') * 10 + (token[5] - '0');
    }
    
    // Status
    token = strtok_r(NULL, ",", &save_ptr);
    if (!token) return false;
    gps_data.is_valid = (token[0] == 'A');
    
    // Latitude
    token = strtok_r(NULL, ",", &save_ptr);
    if (!token) return false;
    char* lat_direction = strtok_r(NULL, ",", &save_ptr);
    if (!lat_direction) return false;
    
    if (strlen(token) > 0 && strlen(lat_direction) > 0) {
        gps_data.latitude = parse_coordinate(token, lat_direction[0]);
    }
    
    // Longitude
    token = strtok_r(NULL, ",", &save_ptr);
    if (!token) return false;
    char* lng_direction = strtok_r(NULL, ",", &save_ptr);
    if (!lng_direction) return false;
    
    if (strlen(token) > 0 && strlen(lng_direction) > 0) {
        gps_data.longitude = parse_coordinate(token, lng_direction[0]);
    }
    
    return gps_data.is_valid;
}

// Ultrasonic task - continuously monitors distance and activates buzzer
static void ultrasonic_task(void *pvParameters)
{
    ultrasonic_init();
    buzzer_init();
    
    char payload[150];
    bool previous_alert_state = false;
    
    while (1) {
        float distance = get_distance();
        
        // Check if distance is valid (not -1)
        if (distance > 0) {
            ESP_LOGI(TAG, "Distance: %.2f cm", distance);
            
            // If the distance is below the threshold, activate the buzzer
            if (distance < THRESHOLD_CM) {
                ESP_LOGI(TAG, "Object detected within threshold! Activating buzzer...");
                activate_buzzer();
                proximity_alert_active = true;
                
                // Publish proximity alert with GPS data if available
                if (mqtt_connected && !previous_alert_state) {
                    snprintf(payload, sizeof(payload),
                            "{\"alert\":true,\"distance\":%.2f,\"lat\":%.6f,\"lng\":%.6f,\"time\":\"%02d:%02d:%02d\"}",
                            distance,
                            gps_data.latitude,
                            gps_data.longitude,
                            gps_data.hour,
                            gps_data.minute,
                            gps_data.second);
                    
                    int msg_id = esp_mqtt_client_publish(mqtt_client, MQTT_PROXIMITY_TOPIC, payload, 0, 1, 0);
                    if (msg_id != -1) {
                        ESP_LOGI(TAG, "Published proximity alert: %s", payload);
                    } else {
                        ESP_LOGE(TAG, "Publish failed for proximity alert");
                    }
                }
            } else {
                proximity_alert_active = false;
                
                // If alert status changed from active to inactive, publish the change
                if (previous_alert_state && mqtt_connected) {
                    snprintf(payload, sizeof(payload),
                            "{\"alert\":false,\"distance\":%.2f,\"lat\":%.6f,\"lng\":%.6f,\"time\":\"%02d:%02d:%02d\"}",
                            distance,
                            gps_data.latitude,
                            gps_data.longitude,
                            gps_data.hour,
                            gps_data.minute,
                            gps_data.second);
                    
                    int msg_id = esp_mqtt_client_publish(mqtt_client, MQTT_PROXIMITY_TOPIC, payload, 0, 1, 0);
                    if (msg_id != -1) {
                        ESP_LOGI(TAG, "Published proximity clear: %s", payload);
                    }
                }
            }
            
            previous_alert_state = proximity_alert_active;
        } else {
            ESP_LOGW(TAG, "Invalid distance reading");
        }
        
        vTaskDelay(100 / portTICK_PERIOD_MS); // Check every 100ms
    }
}

// GPS task
static void gps_task(void *pvParameters)
{
    // Configure UART for GPS
    uart_config_t uart_config = {
        .baud_rate = GPS_BAUD_RATE,
        .data_bits = UART_DATA_8_BITS,
        .parity = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
        .rx_flow_ctrl_thresh = 0,
        .source_clk = UART_SCLK_APB,
    };

    ESP_ERROR_CHECK(uart_param_config(GPS_UART_NUM, &uart_config));
    ESP_ERROR_CHECK(uart_set_pin(GPS_UART_NUM, GPS_TX_PIN, GPS_RX_PIN, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE));
    ESP_ERROR_CHECK(uart_driver_install(GPS_UART_NUM, GPS_UART_BUF_SIZE, 0, 0, NULL, 0));

    uint8_t* data = malloc(GPS_UART_BUF_SIZE);
    char line_buffer[256];
    int line_pos = 0;

    while (1) {
        int len = uart_read_bytes(GPS_UART_NUM, data, GPS_UART_BUF_SIZE, 100 / portTICK_PERIOD_MS);
        
        for (int i = 0; i < len; i++) {
            char c = data[i];
            
            if (c == '\n') {
                line_buffer[line_pos] = '\0';
                
                // Process the NMEA sentence
                if (line_pos > 0 && line_buffer[0] == '$') {
                    if (parse_gps_data(line_buffer)) {
                        ESP_LOGI(TAG, "Valid GPS position: Lat=%.6f, Lng=%.6f, Alt=%.1f, Sats=%d",
                                gps_data.latitude, gps_data.longitude, gps_data.altitude, gps_data.satellites);
                    }
                }
                
                line_pos = 0;
            } else if (c != '\r' && line_pos < sizeof(line_buffer) - 1) {
                line_buffer[line_pos++] = c;
            }
        }
        
        vTaskDelay(10 / portTICK_PERIOD_MS);
    }
    
    free(data);
}

// MQTT publish task
static void mqtt_publish_task(void *pvParameters)
{
    TickType_t last_update = 0;
    char payload[150];
    
    while (1) {
        TickType_t now = xTaskGetTickCount();
        
        if ((now - last_update) * portTICK_PERIOD_MS >= GPS_UPDATE_INTERVAL) {
            last_update = now;
            
            if (gps_data.is_valid && gps_data.satellites >= 3) {
                // Add proximity alert status to the GPS data
                snprintf(payload, sizeof(payload),
                        "{\"lat\":%.6f,\"lng\":%.6f,\"alt\":%.1f,\"sat\":%d,\"hdop\":%.1f,\"time\":\"%02d:%02d:%02d\",\"proximity_alert\":%s}",
                        gps_data.latitude,
                        gps_data.longitude,
                        gps_data.altitude,
                        gps_data.satellites,
                        gps_data.hdop,
                        gps_data.hour,
                        gps_data.minute,
                        gps_data.second,
                        proximity_alert_active ? "true" : "false");
                
                if (mqtt_connected) {
                    int msg_id = esp_mqtt_client_publish(mqtt_client, MQTT_TOPIC, payload, 0, 1, 0);
                    if (msg_id != -1) {
                        ESP_LOGI(TAG, "Published GPS data: %s", payload);
                    } else {
                        ESP_LOGE(TAG, "Publish failed for GPS data");
                    }
                }
            } else {
                // Send a message when no valid GPS data is available
                if (mqtt_connected) {
                    snprintf(payload, sizeof(payload),
                            "{\"status\":\"no_valid_gps\",\"proximity_alert\":%s}",
                            proximity_alert_active ? "true" : "false");
                    
                    int msg_id = esp_mqtt_client_publish(mqtt_client, MQTT_TOPIC, payload, 0, 1, 0);
                    if (msg_id != -1) {
                        ESP_LOGI(TAG, "Published status: No valid GPS data available");
                    } else {
                        ESP_LOGE(TAG, "Publish failed for status message");
                    }
                }
            }
        }
        
        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
}

void app_main(void)
{
    // Initialize NVS
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);

    ESP_LOGI(TAG, "Starting GPS Proximity Tracker...");

    // Initialize Wi-Fi
    init_wifi();

    // Initialize MQTT
    init_mqtt();

    // Create tasks
    xTaskCreate(gps_task, "gps_task", 4096, NULL, 4, NULL);
    xTaskCreate(mqtt_publish_task, "mqtt_publish_task", 4096, NULL, 3, NULL);
    xTaskCreate(ultrasonic_task, "ultrasonic_task", 4096, NULL, 5, NULL);
}