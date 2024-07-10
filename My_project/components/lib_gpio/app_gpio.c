#include "driver/gpio.h"
#include "stdint.h"
#include "app_gpio.h"
void HAL_GPIO_OUTPUT_CONFIG(uint8_t gpio_nums){
    gpio_config_t gpio_cfig={
        .intr_type=GPIO_INTR_DISABLE,
        .mode=GPIO_MODE_INPUT_OUTPUT,
        .pull_down_en=GPIO_PULLDOWN_ENABLE,
        .pin_bit_mask=1ULL<<gpio_nums
    };
   gpio_config(&gpio_cfig);
}
void HAL_GPIO_INPUT_CONFIG(uint8_t gpio_nums){
    gpio_config_t gpio_cfig={
        .intr_type=GPIO_INTR_DISABLE,
        .mode=GPIO_MODE_INPUT_OUTPUT,
        .pull_up_en=GPIO_PULLUP_ENABLE,
        .pin_bit_mask=1ULL<<gpio_nums
    };
    gpio_config(&gpio_cfig);
}
void HAL_GPIO_SETPIN_STATE(gpio_num_t gpio_nums, uint8_t level){
    gpio_set_level(gpio_nums,level);
}
void HAL_GPIO_TOGGLEPIN(gpio_num_t gpio_nums){
  uint8_t toggle = 1 - gpio_get_level(gpio_nums);
  gpio_set_level(gpio_nums,toggle);
  printf("led_toggle\n");
}
uint8_t HAL_GPIO_GETPIN_STATE(gpio_num_t gpio_nums){
    return  gpio_get_level(gpio_nums);
}
