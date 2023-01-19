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
extern uint32_t _sstack[];
#define lgk_boot_flag _sstack[0]

/* Port for uf2  ------------------------------------------------------------------*/
/*!< Flash Start Address of Application */
#ifndef FLASH_BASE_ADDR
#define FLASH_BASE_ADDR 0x08000000UL
#endif

#ifndef SECTOR_SIZE
#define SECTOR_SIZE 2048
#endif

#ifndef RAM_SIZE
#define RAM_SIZE 64 * 1024
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
#define BOARD_FLASH_SIZE (256 * 1024)
#endif

/**
 * MCU Boot
 */
#define BOOT_START_ADDRESS  FLASH_BASE_ADDR
#define APP_CODE_START_ADDR (BOOT_START_ADDRESS + 0x0000f000)       /*!< 60k */
#define APP_CODE_END_ADDR   (BOOT_START_ADDRESS + BOARD_FLASH_SIZE) /*!< 256k */
#define HEAD_SIZE           0x1000                                  /*!< 4K */

#ifndef BOARD_FLASH_APP_START
#define BOARD_FLASH_APP_START (APP_CODE_START_ADDR + HEAD_SIZE) /*!< 0x0000f000 + 0x1000 = 64k */
#endif

#define BOARD_STACK_APP_END (BOARD_STACK_APP_START + (RAM_SIZE))
#define APP_START_ADDRESS   BOARD_FLASH_APP_START
#define APP_FLASH_SIZE      (BOARD_FLASH_SIZE - APP_START_ADDRESS)