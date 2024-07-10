#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "esp_log.h"
#include "sdkconfig.h"
#include "app_gpio.h"
static const char *TAG = "main";


void app_main(void)
{
    HAL_GPIO_OUTPUT_CONFIG(2);
    
    while (1) {
        HAL_GPIO_TOGGLEPIN(2);
        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
}
