#ifndef __LED_H
#define __LED_H

#include "bsp_led.h"

void led_init(void){
    LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_GPIOB);
    
    LL_GPIO_InitTypeDef gpio_struct;
    gpio_struct.Mode = LL_GPIO_MODE_OUTPUT_50MHz;
    gpio_struct.Pin = LED_GPIO_PIN;

    LL_GPIO_Init(LED_GPIO_PORT, &gpio_struct);
    LL_GPIO_SetOutputPin(LED_GPIO_PORT, LED_GPIO_PIN);
}

void led_toggle(void){
    if (LL_GPIO_IsOutputPinSet(LED_GPIO_PORT, LED_GPIO_PIN)) {
        LL_GPIO_ResetOutputPin(LED_GPIO_PORT, LED_GPIO_PIN);
    }else {
        LL_GPIO_SetOutputPin(LED_GPIO_PORT, LED_GPIO_PIN);
    }
}
#endif