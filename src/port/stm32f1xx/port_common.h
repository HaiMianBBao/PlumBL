#pragma once

#define HARD_ENTER_BOOT 1
#if HARD_ENTER_BOOT
#define HARD_ENTER_BL_WAY HARD_ENTER_BL_WAY_DB_RST
#endif

/*!< ---------------------------------------------------------------------------- */

/**
 * Find the pointer at the bottom of the stack
 */
extern uint32_t _sstack[];
#define lgk_boot_flag _sstack[0]

/*!< Flash Start Address of Application */
#ifndef FLASH_BASE_ADDR
#define FLASH_BASE_ADDR 0x08000000UL
#endif

#ifndef SECTOR_SIZE
#define SECTOR_SIZE 1024
#endif

#ifndef BOARD_FLASH_APP_START
#define BOARD_FLASH_APP_START 0x08005000UL
#endif

#ifndef RAM_SIZE
#define RAM_SIZE 20 * 1024
#endif

#ifndef MIN_FLASH_ERASE_SIZE
#define MIN_FLASH_ERASE_SIZE SECTOR_SIZE
#endif

#ifndef BOARD_STACK_APP_START
#define BOARD_STACK_APP_START (0x20000000U)
#endif

#ifndef DBRST_TAP_REG
#define DBRST_TAP_REG lgk_boot_flag
#endif

#ifndef BOARD_FLASH_SIZE
#define BOARD_FLASH_SIZE (64 * 1024)
#endif

#define BOARD_STACK_APP_END (BOARD_STACK_APP_START + (RAM_SIZE))
#define APP_START_ADDRESS   BOARD_FLASH_APP_START
#define APP_FLASH_SIZE      (BOARD_FLASH_SIZE - APP_START_ADDRESS)
