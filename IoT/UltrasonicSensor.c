#include <stdio.h>
#include "soc/gpio_struct.h"
#include "driver/gpio.h"
#include "soc/io_mux_reg.h"
#include "soc/soc.h"
#include "soc/dport_reg.h"
#include "esp_timer.h"
#include "rom/ets_sys.h"  
#include "freertos/FreeRTOS.h"  
#include "freertos/task.h"      

#define TRIG_PIN 5
#define ECHO_PIN 18
#define BUZZER_PIN 19
#define THRESHOLD_CM 10

void gpio_config_registers(uint32_t gpio_num, uint32_t mode) {
    if (mode == GPIO_MODE_OUTPUT) {
        GPIO.enable_w1ts = (1 << gpio_num);
    } else {
        GPIO.enable_w1tc = (1 << gpio_num);
    }
}

void gpio_set_level_registers(uint32_t gpio_num, uint32_t level) {
    if (level) {
        GPIO.out_w1ts = (1 << gpio_num);
    } else {
        GPIO.out_w1tc = (1 << gpio_num);
    }
}

uint32_t gpio_get_level_registers(uint32_t gpio_num) {
    return (GPIO.in >> gpio_num) & 0x1;
}

float get_distance() {
    gpio_set_level_registers(TRIG_PIN, 0);
    esp_rom_delay_us(2);
    gpio_set_level_registers(TRIG_PIN, 1);
    esp_rom_delay_us(10);
    gpio_set_level_registers(TRIG_PIN, 0);

    int64_t start_time = esp_timer_get_time();
    while (gpio_get_level_registers(ECHO_PIN) == 0) {
        start_time = esp_timer_get_time();
    }

    int64_t end_time = esp_timer_get_time();
    while (gpio_get_level_registers(ECHO_PIN) == 1) {
        end_time = esp_timer_get_time();
    }

    int64_t duration = end_time - start_time;
    float distance = (duration * 0.0343) / 2;
    return distance;
}

void activate_buzzer() {
    gpio_set_level_registers(BUZZER_PIN, 1);
    esp_rom_delay_us(500000);
    gpio_set_level_registers(BUZZER_PIN, 0);
}

void app_main() {
    gpio_config_registers(TRIG_PIN, GPIO_MODE_OUTPUT);
    gpio_config_registers(ECHO_PIN, GPIO_MODE_INPUT);
    gpio_config_registers(BUZZER_PIN, GPIO_MODE_OUTPUT);

    while (1) {
        float distance = get_distance();
        printf("Distance: %.2f cm\n", distance);

        if (distance > 0 && distance < THRESHOLD_CM) {
            printf("Object detected! Activating buzzer...\n");
            activate_buzzer();
        }

        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
}