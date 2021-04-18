#ifndef __BSP_USART_H
#define __BSP_USART_H

#include "main.h"

#define USARTx_INSTANCE               USART1
#define USARTx_CLK_ENABLE()           LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_USART1)
#define USARTx_IRQn                   USART1_IRQn
#define USARTx_IRQHandler             USART1_IRQHandler

#define USARTx_GPIO_CLK_ENABLE()      LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_GPIOA)   /* Enable the peripheral clock of GPIOA */
#define USARTx_TX_PIN                 LL_GPIO_PIN_9
#define USARTx_TX_GPIO_PORT           GPIOA
#define USARTx_RX_PIN                 LL_GPIO_PIN_10
#define USARTx_RX_GPIO_PORT           GPIOA
#define APB_Div 1

void usart_init(void);
void usart_task_create(void);
BaseType_t serial_send_byte(uint8_t data, TickType_t timeout);
void serial_send_bytes(uint8_t *data, uint32_t len);

#endif