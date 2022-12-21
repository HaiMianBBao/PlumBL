#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include "lgk_boot_core.h"

#include "stm32l0xx_hal.h"

#ifndef SECTOR_COUNT
#define SECTOR_COUNT (BOARD_FLASH_SIZE / SECTOR_SIZE)
#endif

void lgk_boot_deley_ms(uint32_t ms)
{
    HAL_Delay(ms);
}

void lgk_boot_jump_app(uint32_t app_add)
{
    (void)app_add;
    typedef void (*pFunction)(void);
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
    __HAL_RCC_GPIOA_CLK_DISABLE();
    __HAL_RCC_GPIOB_CLK_DISABLE();
    __HAL_RCC_GPIOC_CLK_DISABLE();
#ifdef __HAL_RCC_GPIOD_CLK_DISABLE
    __HAL_RCC_GPIOD_CLK_DISABLE();
#endif

    HAL_RCC_DeInit();

    SysTick->CTRL = 0;
    SysTick->LOAD = 0;
    SysTick->VAL = 0;

    /*!< Disable all Interrupts */
    NVIC->ICER[0] = 0xFFFFFFFF;
    NVIC->ICER[1] = 0xFFFFFFFF;
    NVIC->ICER[2] = 0xFFFFFFFF;
    NVIC->ICER[3] = 0xFFFFFFFF;

    NVIC_SystemReset();
}

bool lgk_boot_app_is_vaild(uint32_t check_code_add)
{
    (void)check_code_add;
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
    if (((len & (128 - 1)) != 0) || ((start_add & (128 - 1)) != 0)) {
        lgk_boot_log("Error erase length need align 128 \r\n");
        return;
    }
    uint32_t PageError;
    FLASH_EraseInitTypeDef EraseInitStruct = {};
    EraseInitStruct.TypeErase = FLASH_TYPEERASE_PAGES;
    EraseInitStruct.PageAddress = start_add;
    EraseInitStruct.NbPages = (len / FLASH_PAGE_SIZE);

    HAL_FLASH_Unlock();
    if (HAL_FLASHEx_Erase(&EraseInitStruct, &PageError) != HAL_OK) {
        lgk_boot_log("Error erase error \r\n");
        while (1) {
        }
    }
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
    /* Check if the system was resumed from Standby mode */
    if (__HAL_PWR_GET_FLAG(PWR_FLAG_SB) != RESET) {
        /* Direct jump to app */
        lgk_boot_jump_app(BOARD_FLASH_APP_START);
    }
    /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
    HAL_Init();
    /* Configure the system clock */
    extern void SystemClock_Config(void);
    SystemClock_Config();

    /* Initialize all configured peripherals */
    __HAL_RCC_GPIOA_CLK_ENABLE();
#if (LGK_BOOT_DEBUG == 1)
#endif
    lgk_boot_log("lgk_boot_sys_init \r\n");
}

bool lgk_boot_hard_is_enter(void)
{
#if HARD_ENTER_BOOT
#if (HARD_ENTER_BL_WAY == HARD_ENTER_BL_WAY_GPIO)
    /*!< Normal GPIO */
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
