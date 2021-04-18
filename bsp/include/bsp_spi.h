#ifndef __BSP_SPI_H
#define __BSP_SPI_H

#include "main.h"

#define SPIx_INSTANCE               SPI1
#define SPIx_CLK_ENABLE()           LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_SPI1)
#define SPIx_IRQn                   SPI1_IRQn
#define SPIx_IRQHandler             SPI1_IRQHandler

/* Enable the peripheral clock of GPIOA */
#define SPIx_GPIO_CLK_ENABLE()      LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_GPIOA)

#define SPIx_NSS_PIN                LL_GPIO_PIN_4
#define SPIx_NSS_GPIO_PORT          GPIOA
#define SPIx_SCK_PIN                LL_GPIO_PIN_5
#define SPIx_SCK_GPIO_PORT          GPIOA
#define SPIx_MISO_PIN               LL_GPIO_PIN_6
#define SPIx_MISO_GPIO_PORT         GPIOA
#define SPIx_MOSI_PIN               LL_GPIO_PIN_7
#define SPIx_MOSI_GPIO_PORT         GPIOA

void SPI1_IRQHandler(void);
void spi_init(void);
BaseType_t spi_send_bytes(const char* data, uint32_t len);

#endif