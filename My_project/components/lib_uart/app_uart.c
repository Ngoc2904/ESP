#include <stdio.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "driver/uart.h"
#include "esp_log.h"
#include"app_uart.h"
#define PATTERN_CHR_NUM 3  
#define BUF_SIZE 1024
#define RD_BUF_SIZE BUF_SIZE
static const char *TAG = "uart_library";
#define UART_NUM_0 0
#define UART_NUM_1 1
#define UART_NUM_2 2
static QueueHandle_t uart_queues[3];
typedef struct{
    uart_port_t uart_num;
    QueueHandle_t uart_queue;
}uart_event_task_params_t;
static uart_handle_t uart_handle =NULL;
static void uart_event_task(void *pvParameters) {
    uart_event_task_params_t *params=(uart_event_task_params_t*)pvParameters;
    uart_port_t uart_num=params->uart_num;
    QueueHandle_t uart_queue =params->uart_queue;
    uart_event_t event;
    size_t buffered_size;
    uint8_t* dtmp = (uint8_t*) malloc(RD_BUF_SIZE);

    for (;;) {
        if (xQueueReceive(uart_queue, (void *)&event, (TickType_t)portMAX_DELAY)) {
            bzero(dtmp, RD_BUF_SIZE);
            ESP_LOGI(TAG, "uart[%d] event:", uart_num);
            switch (event.type) {
                case UART_DATA:
                    ESP_LOGI(TAG, "[UART DATA]: %d", event.size);
                    uart_read_bytes(uart_num, dtmp, event.size, portMAX_DELAY);
                    ESP_LOGI(TAG, "[DATA EVT]: %s", dtmp);
                    //uart_write_bytes(uart_num, (const char*) dtmp, event.size);
                    uart_handle(dtmp,event.size);
                    break;
                case UART_FIFO_OVF:
                    ESP_LOGI(TAG, "hw fifo overflow");
                    uart_flush_input(uart_num);
                    xQueueReset(uart_queue);
                    break;
                case UART_BUFFER_FULL:
                    ESP_LOGI(TAG, "ring buffer full");
                    uart_flush_input(uart_num);
                    xQueueReset(uart_queue);
                    break;
                case UART_BREAK:
                    ESP_LOGI(TAG, "uart rx break");
                    break;
                case UART_PARITY_ERR:
                    ESP_LOGI(TAG, "uart parity error");
                    break;
                case UART_FRAME_ERR:
                    ESP_LOGI(TAG, "uart frame error");
                    break;
                case UART_PATTERN_DET:
                    uart_get_buffered_data_len(uart_num, &buffered_size);
                    int pos = uart_pattern_pop_pos(uart_num);
                    ESP_LOGI(TAG, "[UART PATTERN DETECTED] pos: %d, buffered size: %d", pos, buffered_size);
                    if (pos == -1) {
                        uart_flush_input(uart_num);
                    } else {
                        uart_read_bytes(uart_num, dtmp, pos, 100 / portTICK_PERIOD_MS);
                        uint8_t pat[PATTERN_CHR_NUM + 1];
                        memset(pat, 0, sizeof(pat));
                        uart_read_bytes(uart_num, pat, PATTERN_CHR_NUM, 100 / portTICK_PERIOD_MS);
                        ESP_LOGI(TAG, "read data: %s", dtmp);
                        ESP_LOGI(TAG, "read pat : %s", pat);
                    }
                    break;
                default:
                    ESP_LOGI(TAG, "uart event type: %d", event.type);
                    break;
            }
        }
    }
    free(dtmp);
    dtmp = NULL;
    vTaskDelete(NULL);
}
void UART_init(uart_port_t uart_num, int tx_pin, int rx_pin,int baud_rate) {
    uart_config_t uart_config = {
        .baud_rate = baud_rate,
        .data_bits = UART_DATA_8_BITS,
        .parity = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
        .source_clk = UART_SCLK_DEFAULT,
    };
    uart_driver_install(uart_num, BUF_SIZE * 2, BUF_SIZE * 2, 20, &uart_queues[uart_num], 0);
    uart_param_config(uart_num, &uart_config);
    uart_set_pin(uart_num, tx_pin, rx_pin, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE);
    uart_enable_pattern_det_baud_intr(uart_num, '+', PATTERN_CHR_NUM, 9, 0, 0);
    uart_pattern_queue_reset(uart_num, 20);
    uart_event_task_params_t *params=(uart_event_task_params_t*)malloc(sizeof(uart_event_task_params_t));
    params->uart_num=uart_num;
    params->uart_queue=uart_queues[uart_num];
    xTaskCreate(uart_event_task, "uart_event_task", 2048, (void *)params, 12, NULL);
}
void UART_tx_data(uart_port_t uart_num,const char *data, uint16_t len) {
    const int UART_tx_data = uart_write_bytes(uart_num, data, len);
    ESP_LOGI("UART_TX_DATA", "Wrote %d bytes to UART%d", UART_tx_data, uart_num);
}
void UART_callback( void*ptr){
    uart_handle=ptr;
}
