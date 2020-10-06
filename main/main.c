#include <string.h>
#include <ultrasonic.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "esp_log.h"

#define MAX_DISTANCE_CM 500
#define TRIGGER GPIO_NUM_12
#define ECHO    GPIO_NUM_13

static const char *TAG = "ULTRASONIC";

void task_ultrasonic(void *pvParamters) {
    ultrasonic_sensor_t sensor = {
        .trigger_pin = TRIGGER,
        .echo_pin = ECHO
    };

    ultrasonic_init(&sensor);

    while (true) {
        uint32_t distance;
        esp_err_t res = ultrasonic_measure_cm(&sensor, MAX_DISTANCE_CM, &distance);
        if (res != ESP_OK) {

            switch (res) {
                case ESP_ERR_ULTRASONIC_PING:
                    ESP_LOGE(TAG, "ERROR - Cannot ping (device is in invalid state)");
                    break;
                case ESP_ERR_ULTRASONIC_PING_TIMEOUT:
                    ESP_LOGE(TAG, "ERROR - Ping timeout (no device found)");
                    break;
                case ESP_ERR_ULTRASONIC_ECHO_TIMEOUT:
                    ESP_LOGE(TAG, "ERROR - Echo timeout (i.e. distance too big)");
                    break;
                default:
                    ESP_LOGE(TAG, "ERROR - %d", res);
            }
        } else {
            ESP_LOGE(TAG, "Distance: %d cm", distance);
        }

        vTaskDelay(500 / portTICK_PERIOD_MS);
    }
}

void app_main(void) {
    gpio_pad_select_gpio(TRIGGER);
    gpio_set_direction(TRIGGER, GPIO_MODE_INPUT);
    gpio_pad_select_gpio(ECHO);
    gpio_set_direction(ECHO, GPIO_MODE_INPUT);

    xTaskCreate(task_ultrasonic, "task_ultrasonic", configMINIMAL_STACK_SIZE * 3, NULL, 5, NULL);
}
