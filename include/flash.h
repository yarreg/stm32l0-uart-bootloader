/**
 * @file    flash.h
 * @author  Ferenc Nemeth
 * @date    21 Dec 2018
 * @brief   This module handles the memory related functions.
 *
 *          Copyright (c) 2018 Ferenc Nemeth - https://github.com/ferenc-nemeth
 */

#ifndef FLASH_H_
#define FLASH_H_

#include "stm32l0xx_hal.h"

/* Start and end addresses of the user application. */
#define FLASH_APP_START_ADDRESS ((uint32_t)0x08004000u)
#define FLASH_APP_END_ADDRESS   FLASH_BANK2_END

/* Status report for the functions. */
typedef enum {
  FLASH_OK                 = 0x00u, /**< The action was successful. */
  FLASH_ERROR_BINARY_SIZE  = 0x01u, /**< The binary is too big. */
  FLASH_ERROR_WRITE        = 0x02u, /**< Writing failed. */
  FLASH_ERROR_READBACK     = 0x04u, /**< Writing was successful, but the content of the memory is wrong. */
  FLASH_ERROR              = 0xFFu  /**< Generic error. */
} FlashStatus;

FlashStatus FlashErase(uint32_t address);
FlashStatus FlashWrite(uint32_t address, uint32_t *data, uint32_t length);
void FlashJumpToApp(void);

#endif /* FLASH_H_ */
