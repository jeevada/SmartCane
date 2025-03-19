#include <stdio.h>
#include <string.h>
#include "driver/uart.h"
#include "driver/gpio.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

// Define UART settings for SIM800L
#define UART_PORT UART_NUM_1
#define UART_TX_PIN GPIO_NUM_17
#define UART_RX_PIN GPIO_NUM_16
#define BUF_SIZE 1024

// Define UART settings for GPS
#define GPS_UART_PORT UART_NUM_2
#define GPS_TX_PIN GPIO_NUM_4
#define GPS_RX_PIN GPIO_NUM_2

// Phone number to send SMS
static const char *PHONE = "+94756004127"; // Replace with your phone number

// Tag for logging
static const char *TAG = "GPS_SMS";

// Function to initialize UART
void uart_init() {
    // Configure UART for SIM800L
    uart_config_t uart_config = {
        .baud_rate = 115200,
        .data_bits = UART_DATA_8_BITS,
        .parity = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
        .source_clk = UART_SCLK_APB,
    };
    ESP_ERROR_CHECK(uart_param_config(UART_PORT, &uart_config));
    ESP_ERROR_CHECK(uart_set_pin(UART_PORT, UART_TX_PIN, UART_RX_PIN, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE));
    ESP_ERROR_CHECK(uart_driver_install(UART_PORT, BUF_SIZE * 2, 0, 0, NULL, 0));

    // Configure UART for GPS
    uart_config.baud_rate = 9600; // GPS module typically uses 9600 baud
    ESP_ERROR_CHECK(uart_param_config(GPS_UART_PORT, &uart_config));
    ESP_ERROR_CHECK(uart_set_pin(GPS_UART_PORT, GPS_TX_PIN, GPS_RX_PIN, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE));
    ESP_ERROR_CHECK(uart_driver_install(GPS_UART_PORT, BUF_SIZE * 2, 0, 0, NULL, 0));
}

// Function to send AT command and wait for response
void send_at_command(const char *command, const char *expected_response, uint32_t timeout_ms) {
    char response[BUF_SIZE] = {0};
    int length = 0;

    // Send AT command
    ESP_LOGI(TAG, "Sending: %s", command);
    uart_write_bytes(UART_PORT, command, strlen(command));
    uart_write_bytes(UART_PORT, "\r", 1); // Add carriage return

    // Wait for response
    uint32_t start_time = esp_log_timestamp();
    while (esp_log_timestamp() - start_time < timeout_ms) {
        length += uart_read_bytes(UART_PORT, (uint8_t *)response + length, BUF_SIZE - length - 1, 20 / portTICK_PERIOD_MS);
        if (length > 0) {
            response[length] = '\0'; // Null-terminate the response
            if (strstr(response, expected_response)) {
                ESP_LOGI(TAG, "Response: %s", response);
                return;
            }
        }
    }

    ESP_LOGE(TAG, "Timeout waiting for response: %s", response);
}

// Function to send SMS
void send_sms(const char *message) {
    // Set SMS text mode
    send_at_command("AT+CMGF=1", "OK", 1000);

    // Set recipient number
    char cmd[64];
    snprintf(cmd, sizeof(cmd), "AT+CMGS=\"%s\"", PHONE);
    send_at_command(cmd, ">", 1000);

    // Send message
    uart_write_bytes(UART_PORT, message, strlen(message));
    vTaskDelay(pdMS_TO_TICKS(100)); // Delay for 100ms
    uart_write_bytes(UART_PORT, "\x1A", 1); // Send Ctrl+Z
    vTaskDelay(pdMS_TO_TICKS(1000)); // Delay for 1 second
    ESP_LOGI(TAG, "SMS Sent: %s", message);
}

// Function to parse NMEA sentence and extract latitude and longitude
void parse_gps_data(char *nmea_sentence, float *latitude, float *longitude) {
    if (strstr(nmea_sentence, "$GPGGA")) {
        char *token = strtok(nmea_sentence, ",");
        int field = 0;
        while (token != NULL) {
            field++;
            if (field == 3) { // Latitude
                *latitude = atof(token) / 100.0;
            } else if (field == 4) { // Latitude direction (N/S)
                if (token[0] == 'S') *latitude *= -1;
            } else if (field == 5) { // Longitude
                *longitude = atof(token) / 100.0;
            } else if (field == 6) { // Longitude direction (E/W)
                if (token[0] == 'W') *longitude *= -1;
                break;
            }
            token = strtok(NULL, ",");
        }
    }
}

void app_main() {
    // Initialize UART
    uart_init();

    // Wait for SIM800L to initialize
    vTaskDelay(pdMS_TO_TICKS(5000));

    // Configure SIM800L for SMS text mode
    send_at_command("AT+CMGF=1", "OK", 1000);
    send_at_command("AT+CNMI=2,1,0,0,0", "OK", 1000);
    send_at_command("AT+CMGD=1,4", "OK", 1000);

    char gps_data[BUF_SIZE] = {0};
    float latitude = 0.0, longitude = 0.0;

    while (1) {
        // Read GPS data
        int len = uart_read_bytes(GPS_UART_PORT, (uint8_t *)gps_data, BUF_SIZE - 1, 1000 / portTICK_PERIOD_MS);
        if (len > 0) {
            gps_data[len] = '\0'; // Null-terminate the data
            parse_gps_data(gps_data, &latitude, &longitude);

            if (latitude != 0.0 && longitude != 0.0) {
                // Format latitude and longitude
                char sms_message[64];
                snprintf(sms_message, sizeof(sms_message), "Latitude: %.6f\nLongitude: %.6f", latitude, longitude);

                // Send SMS
                send_sms(sms_message);
                ESP_LOGI(TAG, "Latitude: %.6f, Longitude: %.6f", latitude, longitude);
            }
        }

        // Wait for 5 seconds
        vTaskDelay(pdMS_TO_TICKS(5000));
    }
}