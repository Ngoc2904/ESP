
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
#include "app_mqtt.h"

#include "json_parser.h"
#include "json_generator.h"

#include "app_ota.h"
#include "app_config.h"
#include "dht11.h"
#include "ledc_io.h"
#include "app_adc.h"
typedef struct {
    char buf[256];
    size_t offset;
} json_gen_test_result_t;



static const char *TAG = "main";
static json_gen_test_result_t result;
static json_gen_test_result_t result_bool;
static struct dht11_reading dht11_current_data,dht11_last_data;


bool fan_off ;
bool fan_on;
bool mode;
bool status;
bool ota;
int slider;
static void flush_str(char *buf, void *priv)
{
    json_gen_test_result_t *result = (json_gen_test_result_t *)priv;
    if (result) {
        if (strlen(buf) > sizeof(result->buf) - result->offset) {
            printf("Result Buffer too small\r\n");
            return;
        }
        memcpy(result->buf + result->offset, buf, strlen(buf));
        result->offset += strlen(buf);
    }
}

int json_parse_user(char *json, int len)
{
    jparse_ctx_t jctx;
    int ret = json_parse_start(&jctx, json, len);
	if (ret != OS_SUCCESS) {
		printf("Parser 1 failed\n");
		return -1;
	}   
    if (json_obj_get_bool(&jctx, "status", &status) != OS_SUCCESS) 
    {
		printf("Parser 2 failed\n");
		return -1;
    }
    if (json_obj_get_bool(&jctx, "mode", &mode) != OS_SUCCESS)
    {
		printf("Parser 3 failed\n");
		return -1;        
    }
    if (json_obj_get_int(&jctx, "slider",&slider) != OS_SUCCESS)
    {
		printf("Parser 3 failed\n");
		return -1;        
    }
    json_parse_end(&jctx);
    return 0;
}

int json_gen(json_gen_test_result_t *result, char *key1, int value1,char *key2, int value2,char *key3, float value3,char*key4,bool value4,char*key5,bool value5,char*key6,bool value6,char*key7,bool value7)
{
	char buf[20];
    memset(result, 0, sizeof(json_gen_test_result_t));
	json_gen_str_t jstr;    
    json_gen_str_start(&jstr, buf, sizeof(buf), flush_str, result);
	json_gen_start_object(&jstr);    
	json_gen_obj_set_int(&jstr, key1, value1);  
    json_gen_obj_set_int(&jstr, key2, value2);  
    json_gen_obj_set_float(&jstr, key3, value3);

    json_gen_obj_set_bool(&jstr, key4, value4);
    json_gen_obj_set_bool(&jstr, key5, value5); 
    json_gen_obj_set_bool(&jstr, key6, value6); 
    json_gen_obj_set_bool(&jstr, key7, value7); 
	json_gen_end_object(&jstr);
	json_gen_str_end(&jstr);
    return 0;
}
 int duty;
void mqtt_data_callback(char *dt, int len)
{   dt[len] = '\0';
   duty=atoi(dt);
    pwm_set_duty(duty-1);
    
    printf("DATA=%s\r\n", dt);    
    //ota
    if(strstr(dt, "ota")){
        ota_task_init_start();
        ota=true;
    }
    // status
    if(strstr(dt, "fanon")){
        pwm_set_duty(100); 
        fan_on=true;
        fan_off=false;
        mode=false;
    } 
    else if(strstr(dt, "fanoff")){
         pwm_set_duty(0);
         fan_off=true;
         fan_on=false;
         mode=false;
    }else if(strstr(dt,"mode_on")){
          mode=true;
          fan_off=false;
          fan_on=false;
       if(dht11_current_data.temperature>24){
        pwm_set_duty(100); 
       } 
    }else{
        mode=false;
    }
}

void app_main(void)
{
    //Initialize NVS
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
      ESP_ERROR_CHECK(nvs_flash_erase());
      ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);
    ESP_ERROR_CHECK(esp_netif_init());
    ESP_ERROR_CHECK(esp_event_loop_create_default());
    pwm_init(2);
    pwm_init(14);
    DHT11_init(4);
    mqtt_app_init();
    mqtt_set_callback(mqtt_data_callback);
    app_config1();
    mqtt_app_start();
    while(1){
    dht11_current_data=DHT11_read();
    if(dht11_current_data.status==0){
        dht11_last_data=DHT11_read();
    } 
    if(dht11_current_data.temperature!=-1){
         json_gen(&result,"Temperature",dht11_current_data.temperature,"Humidity",dht11_current_data.humidity,"Voltage",app_adc(),"fan_on",fan_on,"fan_off",fan_off,"mode",mode,"OTA",ota);
         mqtt_pub("topic/iot1.1",&result.buf,strlen(result.buf));
         vTaskDelay(5000/portTICK_PERIOD_MS);
         continue;
     }
    }
}
