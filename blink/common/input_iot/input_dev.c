#include "input_dev.h"
#include "driver/gpio.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/timers.h"
 static TimerHandle_t xTimers;
 timerout_handle_t timerout_handler=NULL;
input_int_handle_t input_int_handle = NULL;
static uint64_t _start,_stop,_pressTick;
static void IRAM_ATTR gpio_isr_handler(void* arg)
{
    uint32_t gpio_num = (uint32_t) arg;
    uint32_t rtc= xTaskGetTickCountFromISR();

    if(gpio_get_level(gpio_num)==0){
        _start=rtc;
       
    }else{
       _stop=rtc;
       _pressTick=_stop - _start;
       input_int_handle(gpio_num,_pressTick);
    }
}
static void vTimerCallback( TimerHandle_t xTimer )
 {
 
    /* Optionally do something if the pxTimer parameter is NULL. */
    configASSERT( xTimer );

    /* The number of times this timer has expired is saved as the
    timer's ID.  Obtain the count. */
    int ulCount = ( uint32_t ) pvTimerGetTimerID( xTimer );
    if(ulCount==0){
        timerout_callback(0);
    }

 }
void input_create(int pin, input_int_type_t type)
{
    gpio_config_t io_conf;
    //interrupt of rising edge
    io_conf.intr_type = type;
    //bit mask of the pins, use GPIO4/5 here
    io_conf.pin_bit_mask = (1ULL<<pin);
    //set as input mode    
    io_conf.mode = GPIO_MODE_INPUT;
    //enable pull-up mode
    io_conf.pull_up_en = 1;
    io_conf.pull_down_en = 0;

    gpio_config(&io_conf);    

    //install gpio isr service
    gpio_install_isr_service(0);
    //hook isr handler for specific gpio pin
    gpio_isr_handler_add(pin, gpio_isr_handler, (void*) pin);    
    xTimers = xTimerCreate ( "Timerout",pdMS_TO_TICKS(500),pdFALSE,( void * ) 0, vTimerCallback   );
}

void input_set_callback(void *cb)
{
    if(cb){
        input_int_handle = cb;
    }
}
void timerout_callback(void *p){
     timerout_handler=p;
}