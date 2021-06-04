/**
 * @file    flash.c
 * @author  Ferenc Nemeth
 * @date    21 Dec 2018
 * @brief   This module handles the memory related functions.
 *
 *          Copyright (c) 2018 Ferenc Nemeth - https://github.com/ferenc-nemeth
 */

#include "flash.h"


/**
 * @brief   This function erases the memory.
 * @param   address: First address to be erased (the last is the end of the flash).
 * @return  status: Report about the success of the erasing.
 */
FlashStatus FlashErase(uint32_t address) {
    HAL_FLASH_Unlock();

    FlashStatus status = FLASH_ERROR;
    FLASH_EraseInitTypeDef erase_init;
    uint32_t error = 0u;

    erase_init.TypeErase = FLASH_TYPEERASE_PAGES;
    erase_init.PageAddress = address;
    erase_init.NbPages = (FLASH_APP_END_ADDRESS - address) / FLASH_PAGE_SIZE;
    /* Do the actual erasing. */
    if (HAL_OK == HAL_FLASHEx_Erase(&erase_init, &error)) {
        status = FLASH_OK;
    }

    HAL_FLASH_Lock();

    return status;
}

/**
 * @brief   This function flashes the memory.
 * @param   address: First address to be written to.
 * @param   *data:   Array of the data that we want to write.
 * @param   *length: Size of the array.
 * @return  status: Report about the success of the writing.
 */
FlashStatus FlashWrite(uint32_t address, uint32_t *data, uint32_t length) {
    FlashStatus status = FLASH_OK;

    HAL_FLASH_Unlock();

    /* Loop through the array. */
    for (uint32_t i = 0u; (i < length) && (FLASH_OK == status); i++)
    {
        /* If we reached the end of the memory, then report an error and don't do anything else.*/
        if (FLASH_APP_END_ADDRESS <= address)
        {
            status |= FLASH_ERROR_BINARY_SIZE;
        }
        else
        {
            /* The actual flashing. If there is an error, then report it. */
            if (HAL_OK != HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, address, data[i]))
            {
                status |= FLASH_ERROR_WRITE;
            }
            /* Read back the content of the memory. If it is wrong, then report an error. */
            if (((data[i])) != (*(volatile uint32_t*)address))
            {
                status |= FLASH_ERROR_READBACK;
            }

            /* Shift the address by a word. */
            address += 4u;
        }
    }

    HAL_FLASH_Lock();

    return status;
}

/**
 * @brief   Actually jumps to the user application.
 * @param   void
 * @return  void
 */
void FlashJumpToApp(void) {
    /* Function pointer to the address of the user application. */    
    typedef void (*pFn)(void);

    pFn jumpToApp = (pFn)(*(__IO uint32_t*)(FLASH_APP_START_ADDRESS+4));
    HAL_DeInit();
    
    /* Change the main stack pointer. */
    __set_MSP(*(__IO uint32_t*)FLASH_APP_START_ADDRESS);
    jumpToApp();
}

