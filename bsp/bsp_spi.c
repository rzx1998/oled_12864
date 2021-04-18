#include "bsp_spi.h"
#include "stm32f1xx_ll_spi.h"

#define QUEUE_LENGTH 100

static QueueHandle_t spi_tx_queue = {0};
static QueueHandle_t spi_rx_queue = {0};

static void spi_gpio_init(void){
    SPIx_GPIO_CLK_ENABLE();

    LL_GPIO_InitTypeDef gpio_init;

    //config NSS ---- PA4
    gpio_init.Pin = SPIx_NSS_PIN;
    gpio_init.Mode = LL_GPIO_MODE_ALTERNATE;
    gpio_init.Speed = LL_GPIO_SPEED_FREQ_HIGH;
    gpio_init.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
    LL_GPIO_Init(SPIx_NSS_GPIO_PORT, &gpio_init);

    //config SCK ---- PA5
    gpio_init.Pin = SPIx_SCK_PIN;
    gpio_init.Mode = LL_GPIO_MODE_ALTERNATE;
    LL_GPIO_Init(SPIx_SCK_GPIO_PORT, &gpio_init);

    //config MISO ---- PA6
    gpio_init.Pin = SPIx_MISO_PIN;
    gpio_init.Mode = LL_GPIO_MODE_INPUT;
    LL_GPIO_Init(SPIx_MISO_GPIO_PORT, &gpio_init);
    
    //config MOSI ---- PA7
    gpio_init.Pin = SPIx_MOSI_PIN;
    gpio_init.Mode = LL_GPIO_MODE_ALTERNATE;
    LL_GPIO_Init(SPIx_MOSI_GPIO_PORT, &gpio_init);
}

void spi_init(void){
    //init gpio
    spi_gpio_init();
    //enable clock
    SPIx_CLK_ENABLE();

    LL_SPI_InitTypeDef spi_init;
    //baudrate = 4M
    spi_init.BaudRate = LL_SPI_BAUDRATEPRESCALER_DIV256;
    spi_init.BitOrder = LL_SPI_MSB_FIRST;
    spi_init.ClockPhase = LL_SPI_PHASE_1EDGE;
    spi_init.ClockPolarity = LL_SPI_POLARITY_LOW;
    spi_init.CRCCalculation = LL_SPI_CRCCALCULATION_DISABLE;
    spi_init.DataWidth = LL_SPI_DATAWIDTH_8BIT;
    spi_init.Mode = LL_SPI_MODE_MASTER;
    spi_init.NSS = LL_SPI_NSS_HARD_OUTPUT;
    spi_init.TransferDirection = LL_SPI_FULL_DUPLEX;
    LL_SPI_Init(SPIx_INSTANCE, &spi_init);

    //config the interrupt priority
    NVIC_SetPriority(SPIx_IRQn, 15);
    NVIC_EnableIRQ(SPIx_IRQn);
    LL_SPI_EnableIT_TXE(SPIx_INSTANCE);

    //initialize the tx/rx queue
    spi_tx_queue = xQueueCreate(QUEUE_LENGTH, sizeof(int8_t));
    spi_rx_queue = xQueueCreate(QUEUE_LENGTH, sizeof(int8_t));
}

BaseType_t spi_send_bytes(const char* data, uint32_t len){
    uint32_t i = 0;

    for(;i < len; i++){
        if(xQueueSend(spi_tx_queue, data + i, pdMS_TO_TICKS(100)) != pdTRUE){
            LL_SPI_Enable(SPIx_INSTANCE);
            vTaskDelay(pdMS_TO_TICKS(100));
        }
    }
    
    LL_SPI_Enable(SPIx_INSTANCE);
}

void SPI1_IRQHandler(void){
    uint8_t buf;

    if(xQueueReceiveFromISR(spi_tx_queue, &buf, pdFALSE) != pdTRUE){
        LL_SPI_Disable(SPIx_INSTANCE);
        //LL_SPI_DisableIT_TXE(SPIx_INSTANCE);
    }else{
        LL_SPI_TransmitData8(SPIx_INSTANCE, buf);
    }
}