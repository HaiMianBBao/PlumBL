#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include "lgk_boot_core.h"

/*!< mcu */
#include "CH58x_common.h"

#define jumpApp ((void (*)(void))((uint32_t *)APP_START_ADDRESS))

#define BOARD_FLASH_SIZE (448 * 1024)

void USB_IRQHandler(void);

#define APP_FLASH_SIZE (BOARD_FLASH_SIZE - APP_START_ADDRESS)

void lgk_boot_deley_ms(uint32_t ms)
{
    mDelaymS(ms);
}

void lgk_boot_jump_app(uint32_t app_add)
{
    jumpApp();
}

void lgk_boot_sys_reset(void)
{
    lgk_boot_log("lgk_boot_sys_reset \r\n");
    lgk_boot_deley_ms(100);
    SYS_ResetExecute();
}

bool lgk_boot_app_is_vaild(uint32_t check_code_add)
{
#if 0
    uint32_t app_check;
    lgk_boot_flash_read(check_code_add, (uint32_t *)&app_check, 1);
    lgk_boot_log("app_check_0 %08x \r\n", app_check);
    lgk_boot_deley_ms(10);
    if (app_check == APP_CHECK_CODE) {
        return true;
    }
#endif
    return true;
}

void lgk_boot_flash_erase(uint32_t start_add, uint32_t size)
{
    FLASH_ROM_ERASE(start_add, size);
}

void lgk_boot_flash_write(uint32_t start_add, void *buffer, uint32_t size)
{
    FLASH_ROM_WRITE(start_add, (uint8_t *)buffer, size);
}

void lgk_boot_flash_read(uint32_t start_add, void *buffer, uint32_t size)
{
    FLASH_ROM_READ(start_add, buffer, size);
}

void lgk_boot_intf_init(void)
{
    lgk_boot_log("lgk_boot_intf_init \r\n");

    /*!< usb msc */
    extern void msc_flash_init(void);
    msc_flash_init();

    /*!< uf2_init */
    uf2_init();
    PFIC_EnableIRQ(USB_IRQn);
    PFIC_EnableFastINT0(USB_IRQn, (uint32_t)(void *)USB_IRQHandler);
}

void lgk_boot_sys_init(void)
{
    SetSysClock(CLK_SOURCE_PLL_60MHz);

#if (LGK_BOOT_DEBUG == 1)
    /**
     * Use UART1 to debug GPIOA8--RX // GPIOA9--TX
     */
    GPIOA_SetBits(GPIO_Pin_9);
    GPIOA_ModeCfg(GPIO_Pin_8, GPIO_ModeIN_PU);
    GPIOA_ModeCfg(GPIO_Pin_9, GPIO_ModeOut_PP_5mA);
    UART1_DefInit();
#endif
    lgk_boot_log("lgk_boot_sys_init \r\n");
}

bool lgk_boot_hard_is_enter(void)
{
    static uint8_t init_flag = 0;
    if (init_flag == 0) {
        init_flag = 1;
        GPIOB_SetBits(GPIO_Pin_4);
        GPIOB_ModeCfg(GPIO_Pin_4, GPIO_ModeIN_PU);
    }

    if (GPIOB_ReadPortPin(GPIO_Pin_4) == 0) {
        return true;
    }
    return false;
}

/*!< Port for uf2 */

// Initialize flash for DFU
void board_flash_init(void)
{
}

// Get size of flash
uint32_t board_flash_size(void)
{
    return BOARD_FLASH_SIZE;
}

// Read from flash
void board_flash_read(uint32_t addr, void *buffer, uint32_t len)
{
    lgk_boot_flash_read(addr, buffer, len);
}

// Write to flash
void board_flash_write(uint32_t addr, void const *data, uint32_t len)
{
    if ((addr & (MIN_FLASH_ERASE_SIZE - 1)) == 0) {
        lgk_boot_flash_erase(addr, MIN_FLASH_ERASE_SIZE);
    }

    // skip the write if contents matches
    if (memcmp(data, (void *)addr, len) != 0) {
        /*!< Need write new array */
        lgk_boot_log("Write 0x%08lX\r\n", addr);
        lgk_boot_flash_write(addr, (void *)data, len);
    }
    // lgk_boot_flash_write(addr, (void *)data, len);
}

// Flush/Sync flash contents
void board_flash_flush(void)
{
}

// Erase application
void board_flash_erase_app(void)
{
    uint32_t total_page = APP_FLASH_SIZE / MIN_FLASH_ERASE_SIZE;
    for (uint8_t i = 0; i < total_page; i++) {
        lgk_boot_flash_erase(APP_START_ADDRESS + i * MIN_FLASH_ERASE_SIZE, MIN_FLASH_ERASE_SIZE);
    }
}

/* isr function ------------------------------------------------------------------*/
__attribute__((interrupt("WCH-Interrupt-fast")))
__attribute__((section(".highcode"))) void
USB_IRQHandler(void)
{
    extern void usb_isr_handler(void);
    usb_isr_handler();
}
