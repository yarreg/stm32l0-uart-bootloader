#include "clock.h"
#include "gpio.h"
#include "xmodem.h"


#define BOOTLOADER_TIMEOUT 3000
#define BOOTLOADER_MAGIC "bl1\n"
#define BOOTLOADER_BOOT_PIN_THRESHOLD 1000


void SysTick_Handler() { 
    HAL_IncTick();
}

void StartBootlaoder() {
    UART_TransmitStr("bootloader\r\n");
    HAL_GPIO_TogglePin(RED_LED_PORT, RED_LED_PIN);

    /* Ask for new data and start the Xmodem protocol. */
    xmodem_receive();

    UART_TransmitStr("\n\rerror\r\n");
}

bool WaitForBootloaderSequence() {
    char ch;
    int8_t i = 0;
    bool found = false;
    char bootloaderMagic[] = BOOTLOADER_MAGIC;
    char buffer[20] = {0};
    uint32_t endTime = HAL_GetTick() + BOOTLOADER_TIMEOUT;
    uint32_t bootPinPressTime = 0;

    while (endTime > HAL_GetTick() && i < sizeof(buffer)) {
        HAL_GPIO_TogglePin(RED_LED_PORT, RED_LED_PIN);

        if (HAL_GPIO_ReadPin(BOOT_PORT, BOOT_PIN) == GPIO_PIN_RESET) {
            if (bootPinPressTime == 0) {
                bootPinPressTime = HAL_GetTick();
            }

            if (HAL_GetTick() - bootPinPressTime >= BOOTLOADER_BOOT_PIN_THRESHOLD) {
                found = true;
                break;
            }
        } else {
            bootPinPressTime = 0;
        }

        if (UART_Receive((uint8_t*)&ch, 1, 100)) {
            buffer[i] = ch;

            int8_t bootloaderMagicIndex = sizeof(bootloaderMagic)-2;
            for (int8_t j = i; j >= 0 && bootloaderMagicIndex >= 0; j--) {
                if (bootloaderMagic[bootloaderMagicIndex] != buffer[j]) {
                    break;
                }

                bootloaderMagicIndex--;
            }

            if (bootloaderMagicIndex == -1) {
                found = true;
                break;
            }

            i++;
        }
    }

    HAL_GPIO_WritePin(RED_LED_PORT, RED_LED_PIN, GPIO_PIN_RESET);
    return found;
}

int main(void) {
    HAL_Init();

    SystemClock_Config();

    GPIO_Init();
    UART_Init();       

    if (!WaitForBootloaderSequence()) {
        FlashJumpToApp();        
    } else {
        StartBootlaoder();
    }

    while (1) {}  
}

void _Error_Handler(char *file, int line) {
    while(1) {}
}
