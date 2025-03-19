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

// Phone number and message
static const char *phoneNumber = "+94756004127"; // Replace with your phone number
static const char *message = "Hello from ESP32 and SIM800L!";

// Tag for logging
static const char *TAG = "SIM800L";

// Function to initialize UART
void uart_init() {
    uart_config_t uart_config = {
        .baud_rate = 115200, // Changed to 115200 baud
        .data_bits = UART_DATA_8_BITS,
        .parity = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
        .source_clk = UART_SCLK_APB,
    };

    // Configure UART parameters
    ESP_ERROR_CHECK(uart_param_config(UART_PORT, &uart_config));

    // Set UART pins
    ESP_ERROR_CHECK(uart_set_pin(UART_PORT, UART_TX_PIN, UART_RX_PIN, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE));

    // Install UART driver
    ESP_ERROR_CHECK(uart_driver_install(UART_PORT, BUF_SIZE * 2, 0, 0, NULL, 0));
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
void send_sms() {
    char command[64];

    // Set text mode
    send_at_command("AT+CMGF=1", "OK", 1000);

    // Set phone number
    snprintf(command, sizeof(command), "AT+CMGS=\"%s\"", phoneNumber);
    send_at_command(command, ">", 1000);

    // Send message
    ESP_LOGI(TAG, "Sending message: %s", message);
    uart_write_bytes(UART_PORT, message, strlen(message));
    vTaskDelay(pdMS_TO_TICKS(1000)); // Wait for the module to process the message

    // Send Ctrl+Z to indicate the end of the message
    uart_write_bytes(UART_PORT, "\x1A", 1);
    vTaskDelay(pdMS_TO_TICKS(5000)); // Wait for the module to send the SMS

    // Wait for final response
    send_at_command("", "OK", 10000); // Increased timeout to 10 seconds
    ESP_LOGI(TAG, "SMS Sent!");
}

void app_main() {
    // Initialize UART
    uart_init();

    // Wait for SIM800L to initialize
    vTaskDelay(pdMS_TO_TICKS(5000));

    // Send AT command to check if the module is responding
    send_at_command("AT", "OK", 1000);

    // Send SMS
    send_sms();
}