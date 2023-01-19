#pragma once

#ifndef HARD_ENTER_BOOT
#define HARD_ENTER_BOOT 1
#endif

#if HARD_ENTER_BOOT
#ifndef HARD_ENTER_BL_WAY
#define HARD_ENTER_BL_WAY HARD_ENTER_BL_WAY_DB_RST
#endif
#endif

/**
 * Find the pointer at the top of the stack
 */
extern uint32_t _eusrstack[];
#define lgk_boot_flag _eusrstack[0]

#ifndef FLASH_BASE_ADDR
#define FLASH_BASE_ADDR 0
#endif

#ifndef MIN_FLASH_ERASE_SIZE
#define MIN_FLASH_ERASE_SIZE 4096
#endif

#ifndef DBRST_TAP_REG
#define DBRST_TAP_REG lgk_boot_flag
#endif

#ifndef BOARD_FLASH_SIZE
#define BOARD_FLASH_SIZE (448 * 1024)
#endif

/**
 * MCU Boot
 */
#define BOOT_START_ADDRESS  FLASH_BASE_ADDR
#define APP_CODE_START_ADDR (BOOT_START_ADDRESS + 0x0000f000) /*!< 60k */
#define APP_CODE_END_ADDR   (BOOT_START_ADDRESS + 0x00070000) /*!< 448k */
#define HEAD_SIZE           0x1000                            /*!< 4K */

/*!< Flash Start Address of Application */
#ifndef BOARD_FLASH_APP_START
#define BOARD_FLASH_APP_START (APP_CODE_START_ADDR + HEAD_SIZE)
#endif

#define APP_START_ADDRESS BOARD_FLASH_APP_START
#define jumpApp           ((void (*)(void))((uint32_t *)APP_START_ADDRESS))
#define APP_FLASH_SIZE    (BOARD_FLASH_SIZE - APP_START_ADDRESS)
