#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include "lgk_boot_core.h"

#include "stm32f1xx_hal.h"

void lgk_boot_deley_ms(uint32_t ms)
{
    HAL_Delay(ms);
}

typedef void (*pFunction)(void);
void lgk_boot_jump_app(uint32_t app_add)
{
    pFunction JumpToApplication;
    uint32_t JumpAddress;
    /* Jump to user application */
    /*!< Jump to app reset_handler */
    JumpAddress = *(__IO uint32_t *)(BOARD_FLASH_APP_START + 4);
    JumpToApplication = (pFunction)JumpAddress;

    /* switch exception handlers to the application */
    SCB->VTOR = (uint32_t)BOARD_FLASH_APP_START;
    /* Initialize user application's Stack Pointer */
    __set_MSP(*(__IO uint32_t *)BOARD_FLASH_APP_START);
    JumpToApplication();
}

void lgk_boot_sys_reset(void)
{
    lgk_boot_log("lgk_boot_sys_reset \r\n");
    USB_DisableGlobalInt(USB);
    /*!< -- */
    USB_StopDevice(USB);
    __HAL_RCC_USB_CLK_DISABLE();
    HAL_NVIC_DisableIRQ(USB_LP_CAN1_RX0_IRQn);
#if 0
    /*!< Force pull down the D+ */
    GPIO_InitTypeDef GPIO_InitStruct = { 0 };
    __HAL_RCC_GPIOA_CLK_ENABLE();

    /*Configure GPIO pin Output Level */
    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_12, GPIO_PIN_RESET);

    /*Configure GPIO pin : PA12 */
    GPIO_InitStruct.Pin = GPIO_PIN_12;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
    lgk_boot_deley_ms(50);
#endif
    extern UART_HandleTypeDef huart1;
    HAL_UART_DeInit(&huart1);
    __HAL_RCC_USART1_CLK_DISABLE();
    HAL_GPIO_DeInit(GPIOA, GPIO_PIN_9 | GPIO_PIN_10);
    HAL_GPIO_DeInit(GPIOA, GPIO_PIN_12 | GPIO_PIN_11);

    __HAL_RCC_GPIOA_CLK_DISABLE();
    __HAL_RCC_GPIOB_CLK_DISABLE();
    __HAL_RCC_GPIOC_CLK_DISABLE();
    __HAL_RCC_GPIOD_CLK_DISABLE();

    HAL_RCC_DeInit();
    HAL_DeInit();

    NVIC_SystemReset();
}

bool lgk_boot_app_is_vaild(uint32_t check_code_add)
{
    volatile uint32_t const *app_vector = (volatile uint32_t const *)BOARD_FLASH_APP_START;

    /*!< 1st word is stack pointer (should be in SRAM region) */
    if (app_vector[0] < BOARD_STACK_APP_START || app_vector[0] > BOARD_STACK_APP_END) {
        return false;
    }

    /*!< 2nd word is App entry point (reset) */
    if (app_vector[1] < BOARD_FLASH_APP_START || app_vector[1] > BOARD_FLASH_APP_START + BOARD_FLASH_SIZE) {
        return false;
    }
    return true;
}

void lgk_boot_flash_erase(uint32_t start_add, uint32_t len)
{
    HAL_FLASH_Unlock();
    uint32_t PageError;
    /* Variable contains Flash operation status */
    HAL_StatusTypeDef status;
    FLASH_EraseInitTypeDef eraseinitstruct;

    eraseinitstruct.TypeErase = FLASH_TYPEERASE_PAGES;
    eraseinitstruct.PageAddress = start_add;
    eraseinitstruct.NbPages = 1U;
    status = HAL_FLASHEx_Erase(&eraseinitstruct, &PageError);
    (void)status;
    HAL_FLASH_Lock();
}

void lgk_boot_flash_write(uint32_t start_add, void *buffer, uint32_t size)
{
    HAL_FLASH_Unlock();
    uint32_t i = 0;

    for (i = 0; i < size; i += 4) {
        /* Device voltage range supposed to be [2.7V to 3.6V], the operation will
         * be done by byte */
        if (HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, (uint32_t)(start_add + i),
                              *(uint32_t *)(buffer + i)) == HAL_OK) {
            /* Check the written value */
            if (*(uint32_t *)(buffer + i) != *(uint32_t *)(start_add + i)) {
                /* Flash content doesn't match SRAM content */
                lgk_boot_log("Error flash content doesn't match SRAM content\r\n");
            }
        } else {
            /* Error occurred while writing data in Flash memory */
            lgk_boot_log("Error occurred while writing data in Flash memory\r\n");
        }
    }
    HAL_FLASH_Lock();
}

void lgk_boot_flash_read(uint32_t start_add, void *buffer, uint32_t size)
{
    memcpy(buffer, (void *)start_add, size);
}

void lgk_boot_intf_init(void)
{
    lgk_boot_log("lgk_boot_intf_init \r\n");
    extern void lgk_boot_port_intf_init(void);
    lgk_boot_port_intf_init();
}

void lgk_boot_sys_init(void)
{
    /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
    HAL_Init();
    /* Configure the system clock */
    extern void SystemClock_Config(void);
    SystemClock_Config();

    /* Initialize all configured peripherals */
    __HAL_RCC_GPIOD_CLK_ENABLE();
    __HAL_RCC_GPIOA_CLK_ENABLE();
    extern void MX_USART1_UART_Init();
    MX_USART1_UART_Init();

    USB_DisableGlobalInt(USB);
    /*!< -- */
    USB_StopDevice(USB);
    __HAL_RCC_USB_CLK_DISABLE();
    HAL_NVIC_DisableIRQ(USB_LP_CAN1_RX0_IRQn);

    /*!< Force pull down the D+ */
    GPIO_InitTypeDef GPIO_InitStruct = { 0 };
    __HAL_RCC_GPIOA_CLK_ENABLE();

    /*Configure GPIO pin Output Level */
    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_12, GPIO_PIN_RESET);

    /*Configure GPIO pin : PA12 */
    GPIO_InitStruct.Pin = GPIO_PIN_12;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
    lgk_boot_deley_ms(10);

#if (LGK_BOOT_DEBUG == 1)
/**
 * Use rtt to debug
 */
#endif
    lgk_boot_log("lgk_boot_sys_init \r\n");
}

bool lgk_boot_hard_is_enter(void)
{
#if HARD_ENTER_BOOT
#if (HARD_ENTER_BL_WAY == HARD_ENTER_BL_WAY_GPIO)
    /*!< Normal GPIO */
#pragma message "You need to implement the way of hardware entering the bootloader"
    return false;
#elif (HARD_ENTER_BL_WAY == HARD_ENTER_BL_WAY_DB_RST)
    /*!< Double reset */
    /*!< App want to reboot quickly */
    lgk_boot_log("DBRST_TAP_REG address %p vaule is %08lx \r\n", &DBRST_TAP_REG, DBRST_TAP_REG);
    if (DBRST_TAP_REG == DBRST_TAP_MAGIC_QUICK_BOOT) {
        DBRST_TAP_REG = 0;
        return false;
    }

    if (DBRST_TAP_REG == DBRST_TAP_MAGIC) {
        /*!< Double tap occurred */
        DBRST_TAP_REG = 0;
        lgk_boot_log("Double Tap Reset\r\n");
        return true;
    }

    /*!< Register our first reset for double reset detection */
    DBRST_TAP_REG = DBRST_TAP_MAGIC;

    /*!< delay a fraction of second if Reset pin is tap during this delay --> we will enter dfu */
    lgk_boot_deley_ms(DBRST_TAP_DELAY);
    DBRST_TAP_REG = 0xdeadbeef;
#else

#endif
#endif
    return false;
}

