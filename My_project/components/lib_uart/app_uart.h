#ifndef APP_UART_H_
#define APP_UART_H_
#include "driver/uart.h"
#include <stdio.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "driver/uart.h"
#include "esp_log.h"
#include "stdint.h"
#include "driver/gpio.h"
void UART_init(uart_port_t uart_num, int tx_pin, int rx_pin,int baud_rate);
void UART_tx_data( uart_port_t uart_num,const char *data, uint16_t len);
typedef void(*uart_handle_t)(uint8_t*data,int len);
void UART_callback(void*ptr);
#endif