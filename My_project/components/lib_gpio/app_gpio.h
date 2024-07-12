#ifndef APP_GPIO_H_
#define APP_GPIO_H_
#include <stdint.h>
void GPIO_OUTPUT_CONFIG(uint8_t gpio_nums);
void GPIO_TOGGLEPIN(gpio_num_t gpio_nums);
void GPIO_INPUT_CONFIG(uint8_t gpio_nums);
void GPIO_SETPIN_STATE(gpio_num_t gpio_nums, uint8_t level);
uint8_t GPIO_GETPIN_STATE(gpio_num_t gpio_nums);
#endif