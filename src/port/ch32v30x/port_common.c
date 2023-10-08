#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "lgk_boot_core.h"
#include "debug.h"

#define USBHS_DMA_EN      (1 << 0)
#define USBHS_ALL_CLR     (1 << 1)
#define USBHS_FORCE_RST   (1 << 2)
#define USBHS_INT_BUSY_EN (1 << 3)
#define USBHS_DEV_PU_EN   (1 << 4)
#define USBHS_SPEED_MASK  (3 << 5)
#define USBHS_FULL_SPEED  (0 << 5)
#define USBHS_HIGH_SPEED  (1 << 5)
#define USBHS_LOW_SPEED   (2 << 5)
#define USBHS_HOST_MODE   (1 << 7)

void lgk_boot_deley_ms(uint32_t ms)
{
    Delay_Ms(ms);
}

void lgk_boot_jump_app(uint32_t app_add)
{
    (void)app_add;
    NVIC_EnableIRQ(Software_IRQn);
    NVIC_SetPendingIRQ(Software_IRQn);
}

void lgk_boot_sys_reset(void)
{
    lgk_boot_log("lgk_boot_sys_reset \r\n");

    NVIC_DisableIRQ(USBHS_IRQn);
    USBHSD->HOST_CTRL = 0x00;
    USBHSD->CONTROL = 0x00;
    USBHSD->INT_EN = 0x00;
    USBHSD->ENDP_CONFIG = 0xFFffffff;
    USBHSD->CONTROL &= ~USBHS_DEV_PU_EN;
    USBHSD->CONTROL |= USBHS_ALL_CLR | USBHS_FORCE_RST;
    USBHSD->CONTROL = 0x00;
    USBOTG_FS->BASE_CTRL = 0x06;
    USBOTG_FS->INT_EN = 0x00;
    lgk_boot_deley_ms(50);
    lgk_boot_log("jump APP\r\n");
    GPIO_DeInit(GPIOA);
    GPIO_DeInit(GPIOB);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, DISABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, DISABLE);
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_USBHS, DISABLE);
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_OTG_FS, DISABLE);
    lgk_boot_deley_ms(10);
    NVIC_DisableIRQ(OTG_FS_IRQn);

    NVIC_SystemReset();
}

__attribute__((weak)) bool lgk_boot_app_is_vaild(uint32_t check_code_add)
{
    (void)check_code_add;
#ifdef USE_MCU_BOOT
    struct boot_rsp rsp;
    fih_int rc = boot_go(&rsp);
    if (rc == 0) {
        uint32_t image_off = rsp.br_image_off;
        uint32_t header_size = rsp.br_hdr->ih_hdr_size;
        lgk_boot_log("Image found. header_size %d image_off %d\r\n", header_size, image_off);
        if ((image_off + header_size) != BOARD_FLASH_APP_START) {
            return false;
        } else {
            // return true;
            goto common;
        }
    } else {
        lgk_boot_log("Failed searching for a bootable image.\r\n");
        return false;
    }
common:
#endif
    const uint32_t val = *(volatile uint32_t const *)BOARD_FLASH_APP_START;
    return (val != 0xe339e339);
}

void lgk_boot_flash_erase(uint32_t start_add, uint32_t size)
{
    FLASH_Unlock_Fast();
    FLASH_ErasePage_Fast(start_add);
    FLASH_Lock_Fast();
}

void lgk_boot_flash_write(uint32_t start_add, void *buffer, uint32_t size)
{
    uint32_t flash_addr = FLASH_BASE_ADDR + start_add;
    FLASH_Unlock_Fast();
    FLASH_ProgramPage_Fast(start_add, (uint32_t *)buffer);
    FLASH_Lock_Fast();
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
    SystemCoreClockUpdate();
    Delay_Init();

    USART_Printf_Init(115200);
    lgk_boot_log("SystemClk:%d\r\n", SystemCoreClock);

    lgk_boot_log("lgk_boot_sys_init \r\n");
}

bool lgk_boot_hard_is_enter(void)
{
#if HARD_ENTER_BOOT
#if (HARD_ENTER_BL_WAY == HARD_ENTER_BL_WAY_GPIO)
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
    RCC_USBCLK48MConfig(RCC_USBCLK48MCLKSource_USBPHY);
    RCC_USBHSPLLCLKConfig(RCC_HSBHSPLLCLKSource_HSE);
    RCC_USBHSConfig(RCC_USBPLL_Div2);
    RCC_USBHSPLLCKREFCLKConfig(RCC_USBHSPLLCKREFCLK_4M);
    RCC_USBHSPHYPLLALIVEcmd(ENABLE);

    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_USBHS, ENABLE);
    NVIC_EnableIRQ(USBHS_IRQn);

    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_OTG_FS, ENABLE);
    //EXTEN->EXTEN_CTR |= EXTEN_USBD_PU_EN;
    NVIC_EnableIRQ(OTG_FS_IRQn);

    Delay_Us(100);
}
