idf_component_register(SRCS "app_main.c" "app_http_server.c"  "app_mqtt.c"  "app_ota.c""app_config""dht11.c""ledc_io.c""app_adc.c"
                    INCLUDE_DIRS ".")
set(req json_parser json_generator)