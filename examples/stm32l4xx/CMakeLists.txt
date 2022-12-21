cmake_minimum_required(VERSION 3.15)

find_package(plumbl_sdk REQUIRED HINTS $ENV{PLUMBL_SDK_BASE})

sdk_append_inc(Inc)
sdk_append_inc(.)
sdk_append_source(Src/stm32l4xx_hal_msp.c)
sdk_append_source(Src/stm32l4xx_it.c)
sdk_append_source(Src/system_stm32l4xx.c)

if(DEBUG)
sdk_add_compile_definitions(-DLGK_BOOT_DEBUG=1)
endif()

if(${BL_TYPE} STREQUAL "dfu")
sdk_add_compile_definitions(-DUSB_REQUEST_BUFFER_SIZE=4096)
sdk_add_compile_definitions(-DUSBD_DFU_APP_DEFAULT_ADD=0x10000)
sdk_add_compile_definitions(-DUSBD_DFU_XFER_SIZE=4096)
endif()

if(${BL_TYPE} STREQUAL "uf2")
sdk_add_compile_definitions(-DUF2_BOARD_ID="Plum Bootloader for STM32L433")
sdk_add_compile_definitions(-DUF2_VOLUME_LABEL="STM32L4Plum")
sdk_add_compile_definitions(-DBOARD_UF2_FAMILY_ID=0xabcd1433)
endif()

sdk_add_compile_definitions(-DPLUMBL_MCU_STM32L433)

sdk_set_main_file(Src/main.c)

project(${CHIP}_${BL_TYPE})