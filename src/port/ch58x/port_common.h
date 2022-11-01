#pragma once

#define JUMP_APP_FLAG  0x1122c582 /*!< Jump to app flag */
#define JUMP_BOOT_FLAG 0x3344c582 /*!< Jump to bootloader flag */

/**
 * You need to implement your own way to check whether you can jump to the app
 */
#define APP_CHECK_CODE_ADD 0x00       /*!< Check lgk firmware flag's address */
#define APP_CHECK_CODE     0xbeefbeef /*!< Check lgk firmware flag */

/**
 * 
 */
// #define LGK_BOOT_DEBUG  0
#define HARD_ENTER_BOOT 1
#if HARD_ENTER_BOOT
#define HARD_ENTER_BL_WAY HARD_ENTER_BL_WAY_DB_RST
#endif

/**
 * Find the pointer at the top of the stack
 */
extern uint32_t _eusrstack[];
#define lgk_boot_flag _eusrstack[0]

/* Port for uf2  ------------------------------------------------------------------*/
/*!< Flash Start Address of Application */
#ifndef BOARD_FLASH_APP_START
#define BOARD_FLASH_APP_START 0x00010000
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

#define APP_START_ADDRESS BOARD_FLASH_APP_START
#define jumpApp           ((void (*)(void))((uint32_t *)APP_START_ADDRESS))
#define APP_FLASH_SIZE    (BOARD_FLASH_SIZE - APP_START_ADDRESS)