#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include "lgk_boot_core.h"
#include "usbd_core.h"

volatile bool is_flashing_flag = 0;
volatile bool flashing_start_flag = 0;

static uint8_t lgk_boot_process(void)
{
    static uint32_t time_cout_0 = 0;
    static uint32_t time_cout_1 = 0;
    if (flashing_start_flag == 1) {
        /*!< Start formatting or downloading firmware */
        if (is_flashing_flag == 0) {
            time_cout_1 = 0;
            time_cout_0++;
            lgk_boot_deley_ms(1);
            if (time_cout_0 > COMPLETE_DELAY) {
                lgk_boot_log("time_cout_0 \r\n");
                time_cout_0 = 0;
                flashing_start_flag = 0;
                lgk_boot_log("Format completed or firmware burning completed \r\n");
                return BOOT_SYS_COMPLETED;
            }
        } else if (is_flashing_flag == 1) {
            time_cout_0 = 0;
            time_cout_1++;
            lgk_boot_deley_ms(1);
            if (time_cout_1 > COMPLETE_DELAY) {
                lgk_boot_log("time_cout_1 \r\n");
                time_cout_1 = 0;
                flashing_start_flag = 0;
                lgk_boot_log("Format completed or firmware burning completed \r\n");
                return BOOT_SYS_COMPLETED;
            }
        }

        /*!< The format is not completed or the download is completed */
        return BOOT_SYS_DOWNLOADING;
    } else {
        return BOOT_SYS_IDLE;
    }
}

void lgk_boot_main(void)
{
    if (lgk_boot_flag == JUMP_APP_FLAG) {
        /*!< Jump to app */
        lgk_boot_flag = 0xdeadbeef;
        lgk_boot_jump_app(APP_START_ADDRESS);
        while (1) {
            // lgk_boot_log("lgk bootloader jump to app failed \r\n");
        }
    } else if (lgk_boot_flag == JUMP_BOOT_FLAG) {
        /*!< process the data from host */
        lgk_boot_flag = 0xdeadbeef;
        lgk_boot_sys_init();
        lgk_boot_intf_init();
        lgk_boot_log("boot loader mode \r\n");

        uint16_t timeout_count = 0;
        /*!< Wait for USB enumeration to complete */
        while (usb_device_is_configured() == false) {
            lgk_boot_deley_ms(1);
            timeout_count++;
            if (timeout_count > WAIT_TIME_OUT) {
                timeout_count = 0;
                lgk_boot_log("usb is not config and time out reset \r\n");
                lgk_boot_flag = 0xdeadbeef;
                lgk_boot_deley_ms(10);
                lgk_boot_sys_reset();
            }
        }

        while (1) {
            uint8_t process_state = lgk_boot_process();
            if (process_state == BOOT_SYS_COMPLETED) {
                /*!< Format or download completed so we can check app */
                if (lgk_boot_app_is_vaild(APP_CHECK_CODE_ADD)) {
                    lgk_boot_flag = JUMP_APP_FLAG;
                    lgk_boot_sys_reset();
                    while (1) {
                    }
                }
            } else if (process_state == BOOT_SYS_DOWNLOADING) {
                /*!< Formatting or downloading firmware without doing anything */
            } else if (process_state == BOOT_SYS_IDLE) {
                /*!< Firmware upgrade system is idle so we wait for WAIT_TIME_OUT/1000 s */
                lgk_boot_deley_ms(1);
                timeout_count++;
                if (timeout_count > WAIT_TIME_OUT) {
                    timeout_count = 0;
                    lgk_boot_log("lgk boot process time out and reset \r\n");
                    lgk_boot_flag = 0;
                    lgk_boot_deley_ms(10);
                    lgk_boot_sys_reset();
                }
            } else {
                /*!< not support */
            }
        }
    } else {
        /*!< The first power on will be here by default */
        lgk_boot_sys_init();
        lgk_boot_log("lgk_boot_flag address %p\r\n", &lgk_boot_flag);
#if (HARD_ENTER_BOOT == 1)
        if (lgk_boot_hard_is_enter()) {
            lgk_boot_flag = JUMP_BOOT_FLAG;
            lgk_boot_log("reset jump to boot \r\n");
            lgk_boot_sys_reset();
        }
#endif
        /*!< Check app */
        if (lgk_boot_app_is_vaild(APP_CHECK_CODE_ADD)) {
            /*!< jump app */
            lgk_boot_flag = JUMP_APP_FLAG;
            lgk_boot_log("reset jump to app \r\n");
            lgk_boot_sys_reset();
            while (1) {
            }
        } else {
            /*!< force app update */
            lgk_boot_sys_init();
            lgk_boot_intf_init();
            lgk_boot_log("force update mode \r\n");
            uint16_t timeout_count = 0;
            /*!< Wait for USB enumeration to complete */
            while (usb_device_is_configured() == false) {
                lgk_boot_deley_ms(1);
                timeout_count++;
                if (timeout_count > WAIT_TIME_OUT) {
                    timeout_count = 0;
                    lgk_boot_log("usb is not config and time out reset \r\n");
                    lgk_boot_deley_ms(10);
                    lgk_boot_sys_reset();
                }
            }

            /*!< Forced upgrade will not exit bootloader mode without receiving firmware data */
            while (1) {
                uint8_t process_state = lgk_boot_process();

                if (process_state == BOOT_SYS_COMPLETED) {
                    if (lgk_boot_app_is_vaild(APP_CHECK_CODE_ADD)) {
                        lgk_boot_flag = JUMP_APP_FLAG;
                        lgk_boot_sys_reset();
                        while (1) {
                        }
                    }
                }
            }
        }
    }
}
