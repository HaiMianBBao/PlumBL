#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include "lgk_boot_core.h"
#include "uf2.h"

#include "CH58x_common.h"

void USB_IRQHandler(void);

/*!< Port for uf2 */
void lgk_boot_port_intf_init(void)
{
    /*!< uf2_init */
    uf2_init();

    /*!< usb msc */
    extern void msc_flash_init(void);
    msc_flash_init();

    PFIC_EnableIRQ(USB_IRQn);
    PFIC_EnableFastINT0(USB_IRQn, (uint32_t)(void *)USB_IRQHandler);
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
    if ((addr & (MIN_FLASH_ERASE_SIZE - 1)) == 0) {
        lgk_boot_flash_erase(addr, MIN_FLASH_ERASE_SIZE);
    }

    // skip the write if contents matches
    if (memcmp(data, (void *)addr, len) != 0) {
        /*!< Need write new array */
        lgk_boot_log("Write 0x%08lX\r\n", addr);
        lgk_boot_flash_write(addr, (void *)data, len);
    }
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
