#ifndef __BSP_LED_H
#define __BSP_LED_H

#include "stm32f1xx_ll_rcc.h"
#include "stm32f1xx_ll_gpio.h"
#include "stm32f1xx_ll_bus.h"

#define LED_GPIO_PORT       GPIOB
#define LED_GPIO_PIN        LL_GPIO_PIN_12

void led_init(void);
void led_toggle(void);

#endif