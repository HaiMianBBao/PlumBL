#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include "lgk_boot_core.h"

#include "stm32l4xx_hal.h"

#ifndef SECTOR_COUNT
#define SECTOR_COUNT (BOARD_FLASH_SIZE / SECTOR_SIZE)
#endif

static uint8_t erased_sectors[SECTOR_COUNT] = { 0 };

static bool is_blank(uint32_t addr, uint32_t size)
{
    for (uint32_t i = 0; i < size; i += sizeof(uint32_t)) {
        if (*(uint32_t *)(addr + i) != 0xffffffff) {
            return false;
        }
    }
    return true;
}

static bool flash_erase(uint32_t addr)
{
    // starting address from 0x08000000
    uint32_t sector_addr = FLASH_BASE_ADDR;
    bool erased = false;

    uint32_t sector = 0;
    uint32_t size = 0;

    for (uint32_t i = 0; i < SECTOR_COUNT; i++) {
        if (sector_addr < FLASH_BASE_ADDR + BOARD_FLASH_SIZE) {
            size = SECTOR_SIZE;
            if (sector_addr + size > addr) {
                sector = i;
                erased = erased_sectors[i];
                erased_sectors[i] = 1; // don't erase anymore - we will continue writing here!
                break;
            }
            sector_addr += size;
        }
    }

    if (!erased && !is_blank(sector_addr, size)) {
        lgk_boot_log("Erase: %08lX size = %lu KB ... \r\n", sector_addr, size / 1024);
        FLASH_EraseInitTypeDef EraseInitStruct = {};
        EraseInitStruct.TypeErase = TYPEERASE_PAGES;
        EraseInitStruct.Banks = FLASH_BANK_1;
        EraseInitStruct.Page = sector;
        EraseInitStruct.NbPages = 1;

        // erase the sector
        uint32_t SectorError = 0;
        HAL_FLASHEx_Erase(&EraseInitStruct, &SectorError);
        FLASH_WaitForLastOperation(HAL_MAX_DELAY);
        if (is_blank(sector_addr, size)) {
            lgk_boot_log("is_blank\r\n");
        }
    }

    return true;
}

static void flash_write(uint32_t dst, const uint8_t *src, int len)
{
    flash_erase(dst);

    lgk_boot_log("Write flash at address %08lX\r\n", dst);
    for (int i = 0; i < len; i += 8) {
        uint64_t data = *((uint64_t *)((void *)(src + i)));

        if (HAL_FLASH_Program(FLASH_TYPEPROGRAM_DOUBLEWORD, dst + i, data) != HAL_OK) {
            lgk_boot_log("Failed to write flash at address %08lX", dst + i);
            break;
        }

        if (FLASH_WaitForLastOperation(HAL_MAX_DELAY) != HAL_OK) {
            lgk_boot_log("Waiting on last operation failed");
            return;
        }
    }

    if (memcmp((void *)dst, src, len) != 0) {
        lgk_boot_log("failed to write");
    }
}

void lgk_boot_deley_ms(uint32_t ms)
{
    HAL_Delay(ms);
}

void lgk_boot_jump_app(uint32_t app_add)
{
    (void)app_add;
    volatile uint32_t const *app_vector = (volatile uint32_t const *)BOARD_FLASH_APP_START;
    uint32_t sp = app_vector[0];
    uint32_t app_entry = app_vector[1];

    /*!< switch exception handlers to the application  */
    SCB->VTOR = (uint32_t)BOARD_FLASH_APP_START;

    /*!< Set stack pointer */
    __set_MSP(sp);
    __set_PSP(sp);

    /*!< Jump to Application Entry */
    asm("bx %0" ::"r"(app_entry));
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
    uint32_t sp = app_vector[0];
    uint32_t app_entry = app_vector[1];
    lgk_boot_log("sp: %08lx \r\n", sp);
    lgk_boot_log("app_entry: %08lx \r\n", app_entry);

    /*!< 1st word is stack pointer (must be in SRAM region) */
    if ((sp & 0xff000003) != 0x20000000) {
        return false;
    }

    /*!< 2nd word is App entry point (reset) */
    if (app_entry < BOARD_FLASH_APP_START || app_entry > BOARD_FLASH_APP_START + BOARD_FLASH_SIZE) {
        return false;
    }

    return true;
}

void lgk_boot_flash_erase(uint32_t start_add, uint32_t len)
{
    (void)len;
    flash_erase(start_add);
}

void lgk_boot_flash_write(uint32_t start_add, void *buffer, uint32_t size)
{
    HAL_FLASH_Unlock();
    flash_write(start_add, buffer, size);
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
    USB->BCDR |= (uint16_t)USB_BCDR_DPPU;
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
