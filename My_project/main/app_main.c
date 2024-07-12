#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "esp_log.h"
#include "sdkconfig.h"
#include "app_gpio.h"
#include "app_uart.h"
#include <stdint.h>
#include <string.h>
static const char *TAG = "main";
void UART_set_callback(uint8_t *data,int len){
   if(strstr((char*)data,"on")!=0){
   GPIO_TOGGLEPIN(2);
   }else if(strstr((char*)data,"off")!=0){
   GPIO_TOGGLEPIN(2);
   }
}
void app_main(void)
{
    GPIO_OUTPUT_CONFIG(2);
    UART_init(UART_NUM_0, GPIO_NUM_1, GPIO_NUM_3,115200);
    UART_callback(UART_set_callback);
    while (1) {
        UART_tx_data(UART_NUM_0,"data\n",4);
        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
}
