#include <stdio.h>
#include "driver/uart.h"
#include "driver/gpio.h"
#include "esp_log.h"

#define RXD2 16  // GPIO pin for RX
#define TXD2 17  // GPIO pin for TX

#define UART_PORT UART_NUM_2  // Use UART2
#define BUF_SIZE (1024)       // Buffer size for UART data

static const char *TAG = "GPS_Example";

void app_main(void)
{
    // Configure UART parameters
    uart_config_t uart_config = {
        .baud_rate = 9600,
        .data_bits = UART_DATA_8_BITS,
        .parity = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
        .source_clk = UART_SCLK_APB,
    };

    // Install UART driver
    ESP_ERROR_CHECK(uart_driver_install(UART_PORT, BUF_SIZE * 2, 0, 0, NULL, 0));
    ESP_ERROR_CHECK(uart_param_config(UART_PORT, &uart_config));
    ESP_ERROR_CHECK(uart_set_pin(UART_PORT, TXD2, RXD2, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE));

    // Allocate a buffer for incoming data
    uint8_t *data = (uint8_t *) malloc(BUF_SIZE);

    while (1) {
        // Read data from the UART
        int len = uart_read_bytes(UART_PORT, data, BUF_SIZE, 20 / portTICK_PERIOD_MS);  // Fixed here
        if (len > 0) {
            // Print the received data to the console
            ESP_LOGI(TAG, "Received data: %.*s", len, data);
        }
    }

    // Free the allocated memory
    free(data);
}