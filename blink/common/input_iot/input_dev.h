#ifndef __INPUT_DEV_H
#define __INPUT_DEV_H
#include <stdio.h>
typedef enum {
    LO_TO_HI = 1,
    HI_TO_LO = 2,
    ANY_EDGE = 3 
}   input_int_type_t;
typedef void(*timerout_handle_t)(int);
typedef void (*input_int_handle_t) (int ,uint64_t );
void input_create(int pin, input_int_type_t type);
void input_set_callback(void *cb);
void timerout_callback(void *p);
#endif