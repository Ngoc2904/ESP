#ifndef APP_GPIO_H_
#define APP_GPIO_H_
#include <stdint.h>
void HAL_GPIO_OUTPUT_CONFIG(uint8_t gpio_nums);
void HAL_GPIO_TOGGLEPIN(gpio_num_t gpio_nums);
#endif