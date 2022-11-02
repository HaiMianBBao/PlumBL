#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include "lgk_boot_core.h"
#include "CH58x_common.h"
/* Port for st dfu ------------------------------------------------------------------*/

void USB_IRQHandler(void);

void lgk_boot_port_intf_init(void)
{
    /*!< usb dfu flash */
    extern void dfu_flash_init(void);
    dfu_flash_init();

    PFIC_EnableIRQ(USB_IRQn);
    PFIC_EnableFastINT0(USB_IRQn, (uint32_t)(void *)USB_IRQHandler);
}

uint8_t *dfu_read_flash(uint8_t *src, uint8_t *dest, uint32_t Len)
{
    /* Return a valid address to avoid HardFault */
    lgk_boot_flash_read((uint32_t)src, dest, Len);
    return (uint8_t *)(dest);
}

uint16_t dfu_write_flash(uint8_t *src, uint8_t *dest, uint32_t Len)
{
    lgk_boot_flash_write((uint32_t)dest, src, Len);
    return 0;
}

uint16_t dfu_erase_flash(uint32_t Add)
{
    lgk_boot_flash_erase(Add, MIN_FLASH_ERASE_SIZE);
    return (0);
}

void dfu_leave(void)
{
    lgk_boot_flag = 0xdeadbeef;
    lgk_boot_sys_reset();
}
