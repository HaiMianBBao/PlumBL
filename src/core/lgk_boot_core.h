#pragma once

#include <stdio.h>

#define DBRST_TAP_MAGIC            0xf01669ef /*!< Enter Bootloader magic */
#define DBRST_TAP_MAGIC_QUICK_BOOT 0xf02669ef /*!< Skip double tap delay detection */
#define DBRST_TAP_MAGIC_ERASE_APP  0xf5e80ab4 /*!< Erase entire application */

#define HARD_ENTER_BL_WAY_GPIO   0
#define HARD_ENTER_BL_WAY_DB_RST 1

#ifndef JUMP_APP_FLAG
#define JUMP_APP_FLAG 0xc903a520 /*!< Jump to app flag */
#endif

#ifndef JUMP_BOOT_FLAG
#define JUMP_BOOT_FLAG 0xc220b134 /*!< Jump to bootloader flag */
#endif

/**
 * You need to implement your own way to check whether you can jump to the app
 */
#ifndef APP_CHECK_CODE_ADD
#define APP_CHECK_CODE_ADD 0x00 /*!< Check lgk firmware flag's address */
#endif

#ifndef APP_CHECK_CODE
#define APP_CHECK_CODE 0xbeefbeef /*!< Check lgk firmware flag */
#endif

#include "port_common.h"

#ifndef LGK_BOOT_DEBUG
#define LGK_BOOT_DEBUG 0
#endif

#ifndef HARD_ENTER_BOOT
#define HARD_ENTER_BOOT 0
#endif

#ifndef DBRST_TAP_DELAY
#define DBRST_TAP_DELAY 100
#endif

#ifndef COMPLETE_DELAY
#define COMPLETE_DELAY 1000
#endif

#define WAIT_TIME_OUT 60000

/*!< lgk bootloader api */
/**
 * @brief            Whether the hardware enters the bootloader
 * @pre              None
 * @param[in]        None
 * @retval           bool true: Enter bootloader // false: Do not enter bootloader
 */
bool lgk_boot_hard_is_enter(void);

/**
 * @brief            Wait ms
 * @pre              None
 * @param[in]        ms Waiting time
 * @retval           None
 */
void lgk_boot_deley_ms(uint32_t ms);

/**
 * @brief            Boot loader jump to app
 * @pre              App is vaild
 * @param[in]        app_add App start address
 * @retval           None
 */
void lgk_boot_jump_app(uint32_t app_add);

/**
 * @brief            System software reset
 * @pre              None
 * @param[in]        None
 * @retval           None
 */
void lgk_boot_sys_reset(void);

/**
 * @brief            Check app is vaild
 * @pre              None
 * @param[in]        check_code_add Check code address
 * @retval           bool true: app is vaild // false is not vaild
 */
bool lgk_boot_app_is_vaild(uint32_t check_code_add);

/**
 * @brief            Code flash erase
 * @pre              None
 * @param[in]        start_add Erase start address
 * @param[in]        size      Erase size
 * @retval           None
 */
void lgk_boot_flash_erase(uint32_t start_add, uint32_t size);

/**
 * @brief            Code flash write
 * @pre              None
 * @param[in]        start_add Write start address
 * @param[in]        buffer    Data address to be written
 * @param[in]        size      Length of written data
 * @retval           None
 */
void lgk_boot_flash_write(uint32_t start_add, void *buffer, uint32_t size);

/**
 * @brief            Code flash read
 * @pre              None
 * @param[in]        start_add Read start address
 * @param[in]        buffer    Store read data
 * @param[in]        size      Length of data read
 * @retval           None
 */
void lgk_boot_flash_read(uint32_t start_add, void *buffer, uint32_t size);

/**
 * @brief            Initialize the interface for firmware upgrade
 * @pre              None
 * @param[in]        None
 * @retval           None
 */
void lgk_boot_intf_init(void);

/**
 * @brief            Basic system initialization
 * @pre              None
 * @param[in]        None
 * @retval           None
 */
void lgk_boot_sys_init(void);

#if (LGK_BOOT_DEBUG == 1)
#define lgk_boot_log printf
#else
#define lgk_boot_log(...) (void)0
#endif