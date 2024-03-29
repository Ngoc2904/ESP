/* WiFi station Example

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "esp_system.h"
#include "esp_wifi.h"
#include "esp_event.h"
#include "esp_log.h"
#include "nvs_flash.h"
#include "lwip/err.h"
#include "lwip/sys.h"
#include "app_http_server.h"
#include "output_iot.h"
#include "dht11.h"
#include "ledc_io.h"
#include "app_config.h"
#include "app_http_request.h"
static struct dht11_reading dht11_current_data,dht11_last_data;
  
void sw_set_callback(char *dta,uint16_t llen){
     if( *dta =='1'){
     pwm_set_duty(100);
     }else if(*dta =='0'){
         pwm_set_duty(0);
     }
}
void dht11_set_callback(void)
{
   char resp[100];
   sprintf(resp,"{\"temperature\": \"%.02d\", \"humidity\": \"%.02d\"}",dht11_last_data.temperature,dht11_last_data.humidity);
   dht11_respone(resp,strlen(resp));
}
void slider_set_callback(char *dta,int llen){
     
      char number_string[10];
      memcpy(number_string,dta,llen+1);
      int duty =atoi(number_string);
      printf("%d\n",duty);
      pwm_set_duty(duty);
}
void button_satus(){

}
void app_main(void)
{
    //Initialize NVS
    ESP_ERROR_CHECK(nvs_flash_init());
    ESP_ERROR_CHECK(esp_netif_init());
    ESP_ERROR_CHECK(esp_event_loop_create_default());
    DHT11_init(GPIO_NUM_4);
    output_io_create(2);
    sw_callback(sw_set_callback);
    dht11_callback(dht11_set_callback);
    slider_callback(slider_set_callback);
      pwm_init(2);
      app_config();
      start_webserver();
      app_http_request();
while(1){
    dht11_current_data=DHT11_read();
    if(dht11_current_data.status==0){
        dht11_last_data=DHT11_read();
    }
    vTaskDelay(500/portTICK_PERIOD_MS);
    
}
}
