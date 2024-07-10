set(app_src app_gpio.c)
set(pri_req)
idf_component_register(SRCS ${app_src}
                    INCLUDE_DIRS "."
                    REQUIRES driver
                    REQUIRES ${pri_req})

