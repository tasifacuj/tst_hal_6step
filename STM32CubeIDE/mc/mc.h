#pragma once

#include <stdint.h>
#include "main.h"

typedef struct{
    GPIO_TypeDef *H1Port;
    uint32_t  H1Pin;

    GPIO_TypeDef *H2Port;
    uint32_t  H2Pin;

    GPIO_TypeDef *H3Port;
    uint32_t H3Pin;
    
    uint8_t HallState;
    
    uint16_t CntPh;
}HALL_Handle_t;

uint16_t read_hall_pos();
void BLDC_set_pwm( uint16_t pwm );
void BLDC_MotorStop(void);


//#define stimpl
#define BOTTOM_TOP_COMPL
void BLDC_MotorCommutation(uint16_t hallpos);


