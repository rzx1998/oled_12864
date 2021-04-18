#include "bsp_usart.h"

#define USART_BAUDRATE  115200
#define TX_QUEUE_LEN    (100)
#define RX_QUEUE_LEN    (100)

static QueueHandle_t txQueue = {0};
static QueueHandle_t rxQueue = {0};

void USART1_IRQHandler(void);

void usart_init(void){
    //enable gpio and usart periph clock
    USARTx_GPIO_CLK_ENABLE();
    USARTx_CLK_ENABLE();
    //config gpio
    LL_GPIO_InitTypeDef gpio_init;
    //tx
    gpio_init.Pin = USARTx_TX_PIN;
    gpio_init.Mode = LL_GPIO_MODE_ALTERNATE;
    gpio_init.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
    gpio_init.Pull = LL_GPIO_PULL_UP;
    gpio_init.Speed = LL_GPIO_SPEED_FREQ_HIGH;
    LL_GPIO_Init(USARTx_TX_GPIO_PORT, &gpio_init);
    //rx
    gpio_init.Pin = USARTx_RX_PIN;
    gpio_init.Mode = LL_GPIO_MODE_FLOATING;
    gpio_init.Pull = LL_GPIO_PULL_UP;
    gpio_init.Speed = LL_GPIO_SPEED_FREQ_HIGH;
    LL_GPIO_Init(USARTx_RX_GPIO_PORT, &gpio_init);

    /* (2) NVIC Configuration for USART interrupts */
    /*  - Set priority for USARTx_IRQn */
    /*  - Enable USARTx_IRQn */
    NVIC_SetPriority(USARTx_IRQn, 15);
    NVIC_EnableIRQ(USARTx_IRQn);

    /* Configure USART functional parameters ********************************/
    /* Disable USART prior modifying configuration registers */
    /* Note: Commented as corresponding to Reset value */
    // LL_USART_Disable(USARTx_INSTANCE);

    /* TX/RX direction */
    LL_USART_SetTransferDirection(USARTx_INSTANCE, LL_USART_DIRECTION_TX_RX);

    /* 8 data bit, 1 start bit, 1 stop bit, no parity */
    LL_USART_ConfigCharacter(USARTx_INSTANCE, LL_USART_DATAWIDTH_8B, LL_USART_PARITY_NONE, LL_USART_STOPBITS_1);

    /* No Hardware Flow control */
    /* Reset value is LL_USART_HWCONTROL_NONE */
    // LL_USART_SetHWFlowCtrl(USARTx_INSTANCE, LL_USART_HWCONTROL_NONE);

    /* Set Baudrate to 115200 using APB frequency set to 72000000/APB_Div Hz */
    /* Frequency available for USART peripheral can also be calculated through LL RCC macro */
    /* Ex :
        Periphclk = LL_RCC_GetUSARTClockFreq(Instance); or LL_RCC_GetUARTClockFreq(Instance); depending on USART/UART instance
        In this example, Peripheral Clock is expected to be equal to 72000000/APB_Div Hz => equal to SystemCoreClock/APB_Div
    */
    LL_USART_SetBaudRate(USARTx_INSTANCE, SystemCoreClock/APB_Div, 115200); 

    /* (4) Enable USART *********************************************************/
    LL_USART_Enable(USARTx_INSTANCE);

    txQueue = xQueueCreate(TX_QUEUE_LEN, sizeof(uint8_t *));
    rxQueue = xQueueCreate(RX_QUEUE_LEN, sizeof(uint8_t *));
}

BaseType_t serial_send_byte(uint8_t data, TickType_t timeout){
    if(xQueueSend(txQueue, (void *) &data, timeout) == pdPASS){
        return pdPASS;
    }else{
        return pdFALSE;
    }
}

void serial_send_bytes(uint8_t *data, uint32_t len){
    uint32_t i;
    for(i = 0; i < len; i++){
        if(xQueueSend(txQueue, data + i, 1000) == pdFALSE){
            LL_USART_EnableIT_TXE(USARTx_INSTANCE);
            vTaskDelay(pdMS_TO_TICKS(100));
        }
    }

    LL_USART_EnableIT_TXE(USARTx_INSTANCE);
}

static void tx_handle(void *pvParameters){
    uint8_t buf;
    for(;;){
        if(xQueueReceive(txQueue, &buf, pdMS_TO_TICKS(100)) == pdTRUE){
            LL_USART_TransmitData8(USARTx_INSTANCE, buf);
            //led_toggle();
        }
    }
}

void usart_task_create(void){
    xTaskCreate(tx_handle, "tx", configMINIMAL_STACK_SIZE, NULL, 14, NULL);
}

void USART1_IRQHandler(void){
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;
    uint8_t chr;

    if(LL_USART_IsEnabledIT_TXE(USARTx_INSTANCE) == pdTRUE){
        if(xQueueReceiveFromISR(txQueue, &chr, &xHigherPriorityTaskWoken) == pdTRUE){
            LL_USART_TransmitData8(USARTx_INSTANCE, chr);
        }else{
            LL_USART_DisableIT_TXE(USARTx_INSTANCE);
        }
    }
}
