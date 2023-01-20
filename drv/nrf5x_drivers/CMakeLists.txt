set(NRFX_PATH  ${CMAKE_CURRENT_LIST_DIR}/nrfx)
set(SDK11_PATH ${CMAKE_CURRENT_LIST_DIR}/sdk11/components)
set(SDK_PATH   ${CMAKE_CURRENT_LIST_DIR}/sdk/components)
set(TUSB_PATH  ${CMAKE_CURRENT_LIST_DIR}/tinyusb/src)
set(SD_PATH    ${CMAKE_CURRENT_LIST_DIR}/softdevice/${SD_FILENAME})

# nrfx
sdk_append_source(${NRFX_PATH}/drivers/src/nrfx_power.c)
sdk_append_source(${NRFX_PATH}/drivers/src/nrfx_nvmc.c)
sdk_append_source(${NRFX_PATH}/mdk/system_${MCU_SUB_VARIANT}.c)

# SDK 11 files: serial + OTA DFU
sdk_append_source(${SDK11_PATH}/libraries/bootloader_dfu/bootloader.c)
sdk_append_source(${SDK11_PATH}/libraries/bootloader_dfu/bootloader_settings.c)
sdk_append_source(${SDK11_PATH}/libraries/bootloader_dfu/bootloader_util.c)
sdk_append_source(${SDK11_PATH}/libraries/bootloader_dfu/dfu_transport_serial.c)
sdk_append_source(${SDK11_PATH}/libraries/bootloader_dfu/dfu_transport_ble.c)
sdk_append_source(${SDK11_PATH}/libraries/bootloader_dfu/dfu_single_bank.c)
sdk_append_source(${SDK11_PATH}/ble/ble_services/ble_dfu/ble_dfu.c)
sdk_append_source(${SDK11_PATH}/ble/ble_services/ble_dis/ble_dis.c)
sdk_append_source(${SDK11_PATH}/drivers_nrf/pstorage/pstorage_raw.c)

# Latest SDK files: peripheral drivers
sdk_append_source(${SDK_PATH}/libraries/timer/app_timer.c)
sdk_append_source(${SDK_PATH}/libraries/scheduler/app_scheduler.c)
sdk_append_source(${SDK_PATH}/libraries/util/app_error.c)
sdk_append_source(${SDK_PATH}/libraries/util/app_util_platform.c)
sdk_append_source(${SDK_PATH}/libraries/crc16/crc16.c)
sdk_append_source(${SDK_PATH}/libraries/hci/hci_mem_pool.c)
sdk_append_source(${SDK_PATH}/libraries/hci/hci_slip.c)
sdk_append_source(${SDK_PATH}/libraries/hci/hci_transport.c)
sdk_append_source(${SDK_PATH}/libraries/util/nrf_assert.c)

# UART or USB Serial
if(${MCU_SUB_VARIANT} STREQUAL "nrf52")
sdk_append_inc(${SDK11_PATH}/libraries/util)
sdk_append_inc(${SDK_PATH}/drivers_nrf/common)
sdk_append_inc(${SDK_PATH}/drivers_nrf/uart)
sdk_append_source(${SDK_PATH}/libraries/uart/app_uart.c)
sdk_append_source(${SDK_PATH}/drivers_nrf/uart/nrf_drv_uart.c)
sdk_append_source(${SDK_PATH}/drivers_nrf/common/nrf_drv_common.c)
else()
# TinyUSB stack
sdk_append_source(${TUSB_PATH}/portable/nordic/nrf5x/dcd_nrf5x.c)
sdk_append_source(${TUSB_PATH}/common/tusb_fifo.c)
sdk_append_source(${TUSB_PATH}/device/usbd.c)
sdk_append_source(${TUSB_PATH}/device/usbd_control.c)
sdk_append_source(${TUSB_PATH}/class/cdc/cdc_device.c)
sdk_append_source(${TUSB_PATH}/class/msc/msc_device.c)
sdk_append_source(${TUSB_PATH}/tusb.c)
endif()

sdk_append_source(${NRFX_PATH}/mdk/gcc_startup_${MCU_SUB_VARIANT}.S)
sdk_append_inc(${TUSB_PATH})

# nrfx
sdk_append_inc(${NRFX_PATH})
sdk_append_inc(${NRFX_PATH}/mdk)
sdk_append_inc(${NRFX_PATH}/hal)
sdk_append_inc(${NRFX_PATH}/drivers/include)
sdk_append_inc(${NRFX_PATH}/drivers/src)

# sdk11 for cdc/ble dfu
sdk_append_inc(${SDK11_PATH}/libraries/bootloader_dfu/hci_transport)
sdk_append_inc(${SDK11_PATH}/libraries/bootloader_dfu)
sdk_append_inc(${SDK11_PATH}/drivers_nrf/pstorage)
sdk_append_inc(${SDK11_PATH}/ble/common)
sdk_append_inc(${SDK11_PATH}/ble/ble_services/ble_dfu)
sdk_append_inc(${SDK11_PATH}/ble/ble_services/ble_dis)

# later sdk with updated drivers
sdk_append_inc(${SDK_PATH}/libraries/timer)
sdk_append_inc(${SDK_PATH}/libraries/scheduler)
sdk_append_inc(${SDK_PATH}/libraries/crc16)
sdk_append_inc(${SDK_PATH}/libraries/util)
sdk_append_inc(${SDK_PATH}/libraries/hci/config)
sdk_append_inc(${SDK_PATH}/libraries/uart)
sdk_append_inc(${SDK_PATH}/libraries/hci)
sdk_append_inc(${SDK_PATH}/drivers_nrf/delay)

# SoftDevice
sdk_append_inc(${SD_PATH}/${SD_FILENAME}_API/include)
sdk_append_inc(${SD_PATH}/${SD_FILENAME}_API/include/nrf52)

sdk_add_compile_options(-mthumb
                        -mabi=aapcs 
                        -mcpu=cortex-m4 
                        -mfloat-abi=hard 
                        -mfpu=fpv4-sp-d16 
                        -ggdb 
                        -Os 
                        -ffunction-sections 
                        -fdata-sections 
                        -fno-builtin 
                        -fshort-enums 
                        -fstack-usage 
                        -fno-strict-aliasing 
                        -Wall 
                        -Wextra 
                        -Werror 
                        -Wfatal-errors 
                        -Werror-implicit-function-declaration 
                        -Wfloat-equal 
                        -Wundef 
                        -Wshadow 
                        -Wwrite-strings 
                        -Wsign-compare 
                        -Wmissing-format-attribute 
                        -Wno-endif-labels 
                        -Wunreachable-code
                        -Wno-unused-parameter
                        -Wno-expansion-to-defined
                        --param=min-pagesize=0)

sdk_add_compile_definitions(-D__HEAP_SIZE=0)
sdk_add_compile_definitions(-DCONFIG_GPIO_AS_PINRESET)
sdk_add_compile_definitions(-DSOFTDEVICE_PRESENT)
sdk_add_compile_definitions(-DBLEDIS_FW_VERSION=${BLEDIS_FW_VERSION})
sdk_add_compile_definitions(-DMK_BOOTLOADER_VERSION=${MK_BOOTLOADER_VERSION})
sdk_add_compile_definitions(-DUF2_VERSION=${UF2_VERSION})

if(${MCU_SUB_VARIANT} STREQUAL "nrf52")
sdk_add_compile_definitions(-DNRF52)
sdk_add_compile_definitions(-DNRF52832_XXAA)
sdk_add_compile_definitions(-DS132)
elseif(${MCU_SUB_VARIANT} STREQUAL "nrf52833") 
sdk_add_compile_definitions(-DNRF52833_XXAA)
sdk_add_compile_definitions(-DS140)
elseif(${MCU_SUB_VARIANT} STREQUAL "nrf52840")
sdk_add_compile_definitions(-DNRF52840_XXAA)
sdk_add_compile_definitions(-DS140)
endif()

# Debug option use RTT for printf
if(${DEBUG})
    sdk_add_compile_definitions(-DCFG_DEBUG)
    sdk_add_compile_definitions(-DSEGGER_RTT_MODE_DEFAULT=SEGGER_RTT_MODE_BLOCK_IF_FIFO_FULL)
    sdk_append_inc(SEGGER_RTT/RTT)
    sdk_append_source(SEGGER_RTT/RTT/SEGGER_RTT.c)
    sdk_add_compile_definitions(-DDFU_APP_DATA_RESERVED=0)
    # expand bootloader address to 28KB of reserved app
    if(${MCU_SUB_VARIANT} STREQUAL "nrf52840")
        sdk_add_compile_definitions(-DBOOTLOADER_REGION_START=0xED000)
    else()
        sdk_add_compile_definitions(-DBOOTLOADER_REGION_START=0x6D000)
    endif()
else()
sdk_add_compile_definitions(-DDFU_APP_DATA_RESERVED=${DFU_APP_DATA_RESERVED})
endif()

sdk_add_link_options(-mthumb
                    -mabi=aapcs 
                    -mcpu=cortex-m4 
                    -mfloat-abi=hard 
                    -mfpu=fpv4-sp-d16 
                    -ggdb 
                    -Os 
                    -ffunction-sections 
                    -fdata-sections 
                    -fno-builtin 
                    -fshort-enums 
                    -fstack-usage 
                    -fno-strict-aliasing 
                    -Wall 
                    -Wextra 
                    -Werror 
                    -Wfatal-errors 
                    -Werror-implicit-function-declaration 
                    -Wfloat-equal 
                    -Wundef 
                    -Wshadow 
                    -Wwrite-strings 
                    -Wsign-compare 
                    -Wmissing-format-attribute 
                    -Wno-endif-labels 
                    -Wunreachable-code
                    -Wno-unused-parameter
                    -Wno-expansion-to-defined
                    --param=min-pagesize=0
                    -Wl,-cref -Wl,-gc-sections
                    -lm -lc
                    --specs=nosys.specs 
                    )

if(${DEBUG})
    sdk_add_link_options(--specs=nano.specs)
    set(LD_FILE ../../board/linker/nrf5x/${MCU_SUB_VARIANT}_debug.ld)
else()
    set(LD_FILE ../../board/linker/nrf5x/${MCU_SUB_VARIANT}.ld)
endif()

sdk_set_linker_script(${LD_FILE})