#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/timers.h"
#include "freertos/event_groups.h"
#include "sdkconfig.h"
#include "output_dev.h"
#include "input_dev.h"
#include "freertos/timers.h"
#include "freertos/event_groups.h"
 static EventGroupHandle_t xCreatedEventGroup;
 static EventGroupHandle_t xCreatedEventGroup_button;
#define BIT_BUTTON	( 1 << 0 )
#define BIT_UART	( 1 << 1 )

#define button_short (1<<0)
#define button_nomal (1<<1)
#define button_long (1<<2)

TimerHandle_t xTimers[ 2 ];
void vTask1( void * pvParameters )
{
    for( ;; )
    {
        EventBits_t uxBits;
        uxBits = xEventGroupWaitBits(
            xCreatedEventGroup,   /* The event group being tested. */
             BIT_BUTTON | BIT_UART, /* The bits within the event group to wait for. */
            pdTRUE,        /* BIT_0 & BIT_4 should be cleared before returning. */
            pdFALSE,       /* Don't wait for both bits, either bit will do. */
            portMAX_DELAY );/* Wait a maximum of 100ms for either bit to be set. */
        if(uxBits&BIT_BUTTON){
            printf("buttonpress\n");
            output_toggle(2);
        }
        if(uxBits&BIT_UART){
            printf("UART DATA\n");
        }    
    }
}
void vTask2( void * pvParameters )
{
    for( ;; )
    {
       EventBits_t uxBits;
        uxBits = xEventGroupWaitBits(
            xCreatedEventGroup_button,   /* The event group being tested. */
             button_short | button_nomal | button_long, /* The bits within the event group to wait for. */
            pdTRUE,        /* BIT_0 & BIT_4 should be cleared before returning. */
            pdFALSE,       /* Don't wait for both bits, either bit will do. */
            portMAX_DELAY );/* Wait a maximum of 100ms for either bit to be set. */
            if(uxBits&button_short){
                  printf("press short\n");
            }else if(uxBits&button_nomal){
                  printf("press nomal\n");
            }else{
                  printf("press long\n");
            }
    }
}
void vTimerCallback( TimerHandle_t xTimer )
 {
 
    /* Optionally do something if the pxTimer parameter is NULL. */
    configASSERT( xTimer );

    /* The number of times this timer has expired is saved as the
    timer's ID.  Obtain the count. */
    int ulCount = ( uint32_t ) pvTimerGetTimerID( xTimer );
    if(ulCount==0){
        output_toggle(2);
    }else if(ulCount==1){
        printf("hello\n");
        
    }

 }

void button_callback(int pin,uint64_t tick){
    if(pin==0){
            int press_ms=tick/portTICK_PERIOD_MS;
             BaseType_t xHigherPriorityTaskWoken;
              xHigherPriorityTaskWoken = pdFALSE;
            if(press_ms<1000){   
              xEventGroupSetBitsFromISR(xCreatedEventGroup_button,button_short ,&xHigherPriorityTaskWoken );
            }else if(press_ms<=2000){
              xEventGroupSetBitsFromISR(xCreatedEventGroup_button,button_nomal ,&xHigherPriorityTaskWoken );
            }else if(press_ms>3000){
              xEventGroupSetBitsFromISR(xCreatedEventGroup_button,button_long ,&xHigherPriorityTaskWoken );
            }
    }
}
void button_timerout_callback(int pin){
   if(pin==0){
    printf("timer out\n");
   }

}
void app_main(void)
{
    //xTimers[ 0 ] = xTimerCreate ( "Timerblink",pdMS_TO_TICKS(500),pdTRUE,( void * ) 0, vTimerCallback   );
      //xTimers[ 1 ] = xTimerCreate ( "Timerprintf",pdMS_TO_TICKS(500),pdTRUE,( void * ) 1, vTimerCallback   );   
     // xTimerStart(xTimers[0],0);
      //xTimerStart(xTimers[1],0);   
      xCreatedEventGroup = xEventGroupCreate();
      xCreatedEventGroup_button=xEventGroupCreate();
  
    output_create(2);     
    input_create(0,ANY_EDGE); 
    input_set_callback(button_callback);                       
    //xTaskCreate(vTask1,"vTask1",3*1024,NULL,4,NULL );     
   xTaskCreate(vTask2,"vTask2",1024,NULL,6,NULL );  
   timerout_callback(button_timerout_callback);

}
