#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include "lgk_boot_core.h"

/*!< mcu */
#include "CH58x_common.h"

#define DFU_MEDIA_ERASE   0x00U
#define DFU_MEDIA_PROGRAM 0x01U

#ifndef DBRST_TAP_REG
#define DBRST_TAP_REG _eusrstack[0]
#endif

#define jumpApp ((void (*)(void))((uint32_t *)APP_START_ADDRESS))

void USB_IRQHandler(void);

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
    if (app_check == APP_CHECK_CODE)
    {
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

    /*!< usb dfu */
    extern void dfu_flash_init(void);
    dfu_flash_init();

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
#if HARD_ENTER_BOOT
#if (HARD_ENTER_BL_WAY == HARD_ENTER_BL_WAY_GPIO)
    /*!< Normal GPIO */
    static uint8_t init_flag = 0;
    if (init_flag == 0) {
        init_flag = 1;
        GPIOB_SetBits(GPIO_Pin_4);
        GPIOB_ModeCfg(GPIO_Pin_4, GPIO_ModeIN_PU);
    }

    if (GPIOB_ReadPortPin(GPIO_Pin_4) == 0) {
        return true;
    }
#elif (HARD_ENTER_BL_WAY == HARD_ENTER_BL_WAY_DB_RST)
    /*!< Double reset */
    /*!< App want to reboot quickly */
    lgk_boot_log("DBRST_TAP_REG address %p vaule is %08lx \r\n", _eusrstack, DBRST_TAP_REG);
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

/* Port for st dfu ------------------------------------------------------------------*/
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
    lgk_boot_flag = JUMP_APP_FLAG;
    lgk_boot_sys_reset();
}

/* isr function ------------------------------------------------------------------*/
__attribute__((interrupt("WCH-Interrupt-fast")))
__attribute__((section(".highcode"))) void
USB_IRQHandler(void)
{
    extern void usb_isr_handler(void);
    usb_isr_handler();
}
