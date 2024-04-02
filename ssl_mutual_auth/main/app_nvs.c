#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "nvs_flash.h"
#include "nvs.h"
#include "app_nvs.h"
static  nvs_handle_t my_handle;
#define USER_NAMESPACE "storage"
void app_nvs_set_value(char *key,int value) {
    int err;
     err = nvs_open(USER_NAMESPACE, NVS_READWRITE, &my_handle);
     if (err != ESP_OK) {
        printf("Error (%s) opening NVS handle!\n", esp_err_to_name(err));
     }
        err = nvs_set_i32(my_handle, key, value);
        printf((err != ESP_OK) ? "Failed!\n" : "Done\n");
         printf("Committing updates in NVS ... ");
        err = nvs_commit(my_handle);
        printf((err != ESP_OK) ? "Failed!\n" : "Done\n");
        nvs_close(my_handle);

}
void app_nvs_get_value( char *key , int *value){
    int err;
     err = nvs_open(USER_NAMESPACE, NVS_READWRITE, &my_handle);
     if (err != ESP_OK) {
        printf("Error (%s) opening NVS handle!\n", esp_err_to_name(err));
     }
         err = nvs_get_i32(my_handle, key,value);
        switch (err) {
            case ESP_OK:
                printf("Done\n");
              
                break;
            case ESP_ERR_NVS_NOT_FOUND:
                printf("The value is not initialized yet!\n");
                break;
            default :
                printf("Error (%s) reading!\n", esp_err_to_name(err));
        }

}


void app_nvs_set_string(char *key,char * str) {
    int err;
     err = nvs_open(USER_NAMESPACE, NVS_READWRITE, &my_handle);
     if (err != ESP_OK) {
        printf("Error (%s) opening NVS handle!\n", esp_err_to_name(err));
     }
        err = nvs_set_str(my_handle, key, str);
        printf((err != ESP_OK) ? "Failed!\n" : "Done\n");
         printf("Committing updates in NVS ... ");
        err = nvs_commit(my_handle);
        printf((err != ESP_OK) ? "Failed!\n" : "Done\n");
        nvs_close(my_handle);

}


void app_nvs_get_string( char *key , char *out){
    int err;
    size_t length=0;
     err = nvs_open(USER_NAMESPACE, NVS_READWRITE, &my_handle);
     if (err != ESP_OK) {
        printf("Error (%s) opening NVS handle!\n", esp_err_to_name(err));
     }
         err = nvs_get_str(my_handle, key,out,&length);
         err=0;
          err = nvs_get_str(my_handle, key,out,&length);
        switch (err) {
            case ESP_OK:
                printf("Done\n");
                printf("%s=%s,length=%d",key,out,length);
                break;
            case ESP_ERR_NVS_NOT_FOUND:
                printf("The value is not initialized yet!\n");
                break;
            default :
                printf("Error (%s) reading!\n", esp_err_to_name(err));
        }

}