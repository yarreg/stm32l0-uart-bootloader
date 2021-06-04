#include "uart.h"


static UART_HandleTypeDef huart;


bool UART_Init() {
    __HAL_RCC_USART1_CLK_ENABLE();

    huart.Instance                    = USART1;
    huart.Init.BaudRate               = 115200;
    huart.Init.WordLength             = UART_WORDLENGTH_8B;
    huart.Init.StopBits               = UART_STOPBITS_1;
    huart.Init.Parity                 = UART_PARITY_NONE;
    huart.Init.Mode                   = UART_MODE_TX_RX;
    huart.Init.HwFlowCtl              = UART_HWCONTROL_NONE;
    huart.Init.OverSampling           = UART_OVERSAMPLING_16;
    huart.Init.OneBitSampling         = UART_ONE_BIT_SAMPLE_DISABLE;
    huart.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;

    return (HAL_UART_Init(&huart) == HAL_OK);     
}

bool UART_Receive(uint8_t *data, uint16_t length, uint32_t timeout) {
    return (HAL_OK == HAL_UART_Receive(&huart, data, length, timeout));
}

bool UART_TransmitStr(char *data) {
    uint16_t length = 0;

    /* Calculate the length. */
    while (data[length]) {
        length++;
    }

    return (HAL_OK == HAL_UART_Transmit(&huart, (uint8_t*)data, length, HAL_MAX_DELAY));
}

bool UART_TransmitChar(char ch) {
    /* Make available the UART module. */
    if (HAL_UART_STATE_TIMEOUT == HAL_UART_GetState(&huart)) {
        HAL_UART_Abort(&huart);
    }

    return (HAL_OK == HAL_UART_Transmit(&huart, (uint8_t*)&ch, 1u, HAL_MAX_DELAY));
}
