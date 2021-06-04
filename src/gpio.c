#include "gpio.h"


void GPIO_Init() {
    __HAL_RCC_GPIOA_CLK_ENABLE();
    __HAL_RCC_GPIOB_CLK_ENABLE();

    /**USART1 GPIO Configuration
    PA9  ------> USART1_TX
    PA10 ------> USART1_RX 
    */
    GPIO_CFG(UART_PORT, {.Pin = UART_TX_PIN, .Mode = GPIO_MODE_AF_PP, .Speed = GPIO_SPEED_FREQ_HIGH, .Alternate = GPIO_AF4_USART1});
    GPIO_CFG(UART_PORT, {.Pin = UART_RX_PIN, .Mode = GPIO_MODE_AF_PP, .Speed = GPIO_SPEED_FREQ_HIGH, .Alternate = GPIO_AF4_USART1});

    /**LEDS GPIO
    PB2 ------> LED1 
    PB7 ------> LED2 
    */
    GPIO_CFG(GREEN_LED_PORT, {.Pin = GREEN_LED_PIN, .Mode = GPIO_MODE_OUTPUT_PP, .Speed = GPIO_SPEED_FREQ_HIGH});
    GPIO_CFG(RED_LED_PORT,   {.Pin = RED_LED_PIN,   .Mode = GPIO_MODE_OUTPUT_PP, .Speed = GPIO_SPEED_FREQ_HIGH});

    /** BOOT PIN
    PB12 ------> BOOT PIN
    */
    GPIO_CFG(BOOT_PORT, {.Pin = BOOT_PIN, .Mode = GPIO_MODE_INPUT, .Pull = GPIO_PULLUP, .Speed = GPIO_SPEED_FREQ_HIGH});
}