#pragma once

#define JUMP_APP_FLAG  0x1122c582 /*!< Jump to app flag */
#define JUMP_BOOT_FLAG 0x3344c582 /*!< Jump to bootloader flag */

/**
 * You need to implement your own way to check whether you can jump to the app
 */
#define APP_CHECK_CODE_ADD 0x00       /*!< Check lgk firmware flag's address */
#define APP_CHECK_CODE     0xbeefbeef /*!< Check lgk firmware flag */

/* Port for uf2  ------------------------------------------------------------------*/
/*!< Flash Start Address of Application */
#define BOARD_FLASH_APP_START 0x00010000
#define MIN_FLASH_ERASE_SIZE  4096

#define APP_START_ADDRESS BOARD_FLASH_APP_START

#include "uf2.h"