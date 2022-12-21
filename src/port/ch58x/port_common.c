#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include "lgk_boot_core.h"

#include "CH58x_common.h"

void lgk_boot_deley_ms(uint32_t ms)
{
    mDelaymS(ms);
}

void lgk_boot_jump_app(uint32_t app_add)
{
    (void)app_add;
    jumpApp();
}

void lgk_boot_sys_reset(void)
{
    lgk_boot_log("lgk_boot_sys_reset \r\n");
    lgk_boot_deley_ms(100);
    SYS_ResetExecute();
}

__attribute__((weak)) bool lgk_boot_app_is_vaild(uint32_t check_code_add)
{
    (void)check_code_add;
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
    extern void lgk_boot_port_intf_init(void);
    lgk_boot_port_intf_init();
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

void usb_dc_low_level_init(void)
{
    extern void USB_IRQHandler(void);
    PFIC_EnableIRQ(USB_IRQn);
    PFIC_EnableFastINT0(USB_IRQn, (uint32_t)(void *)USB_IRQHandler);
}
