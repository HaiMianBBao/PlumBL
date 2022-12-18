#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include "lgk_boot_core.h"
#include "uf2.h"

#include "stm32l4xx_hal.h"

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
    //TODO: Skip the same data in the future
#if 0
    if (memcmp(data, (void *)addr, len) != 0) {
        /*!< Need write new array */
        lgk_boot_log("Write 0x%08lX\r\n", addr);
        lgk_boot_flash_write(addr, (void *)data, len);
    }
#else
    lgk_boot_flash_write(addr, (void *)data, len);
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