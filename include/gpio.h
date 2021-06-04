#ifndef GPIO_H
#define GPIO_H

#include "stm32l0xx_hal.h"


#define GPIO_CFG(PORT, ...) HAL_GPIO_Init(PORT, (&(GPIO_InitTypeDef)__VA_ARGS__))


// LEDs
#define RED_LED_PORT    GPIOB
#define RED_LED_PIN     GPIO_PIN_7
#define GREEN_LED_PORT  GPIOB
#define GREEN_LED_PIN   GPIO_PIN_2

// UART
#define UART_PORT       GPIOA
#define UART_TX_PIN     GPIO_PIN_9
#define UART_RX_PIN     GPIO_PIN_10

// Boot
#define BOOT_PORT       GPIOB
#define BOOT_PIN        GPIO_PIN_12


void GPIO_Init();

#endif
