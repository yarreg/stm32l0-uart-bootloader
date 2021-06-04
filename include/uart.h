#ifndef UART_H
#define UART_H

#include <stdbool.h>
#include "stm32l0xx_hal.h"

bool UART_Init();
bool UART_Receive(uint8_t *data, uint16_t length, uint32_t timeout);
bool UART_TransmitStr(char *data);
bool UART_TransmitChar(char data);


#endif
