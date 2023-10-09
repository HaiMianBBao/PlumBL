#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include "lgk_boot_core.h"
#include "uf2.h"

/*!< Port for uf2 */
void lgk_boot_port_intf_init(void)
{
    /*!< uf2_init */
    uf2_init();

    /*!< usb msc */
    extern void msc_flash_init(void);
    msc_flash_init();
}

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
#ifdef USE_MCU_BOOT
    uint32_t limit_address = APP_CODE_START_ADDR;
#else
    uint32_t limit_address = BOARD_FLASH_APP_START;
#endif
    if (addr < limit_address) {
        lgk_boot_log("Error addr 0x%08lX will break the bootloader, length = %d\r\n", addr, len);
        return;
    }

#ifdef PLUMBL_MCU_STM32L433
    lgk_boot_flash_write(addr, (void *)data, len);
#else
    // skip the write if contents matches
    if (memcmp(data, (void *)addr, len) != 0) {
        /*!< Need write new array */
        lgk_boot_log("Write 0x%08lX  length = %d\r\n", addr, len);

#if (defined(PLUMBL_MCU_CH582) || defined(PLUMBL_MCU_STM32F103) || defined(PLUMBL_MCU_STM32L073) || defined(PLUMBL_MCU_CH32V30x))
        if ((addr & (MIN_FLASH_ERASE_SIZE - 1)) == 0) {
            lgk_boot_flash_erase(addr, MIN_FLASH_ERASE_SIZE);
        }
#endif

        lgk_boot_flash_write(addr, (void *)data, len);
    }
#endif
}

// Flush/Sync flash contents
void board_flash_flush(void)
{
}

// Erase application
void board_flash_erase_app(void)
{
}
