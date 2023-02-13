cmake_minimum_required(VERSION 3.15)

find_package(plumbl_sdk REQUIRED HINTS $ENV{PLUMBL_SDK_BASE})

sdk_append_inc(Inc)
sdk_append_inc(.)
sdk_append_source(Src/stm32f1xx_hal_msp.c)
sdk_append_source(Src/stm32f1xx_it.c)
sdk_append_source(Src/system_stm32f1xx.c)

if(${DEBUG})
message(STATUS "${Red} Warring !! Please check the size you assigned to the bootloader flash. It may exceed this size after the log is enabled ${ColourReset}")  
sdk_add_compile_definitions(-DLGK_BOOT_DEBUG=1)
endif()

if(${USE_MCU_BOOT})
sdk_append_source(test_key.c)
sdk_add_compile_definitions(-DUSE_MCU_BOOT)
endif()

if(${BL_TYPE} STREQUAL "dfu")
sdk_add_compile_definitions(-DUSB_REQUEST_BUFFER_SIZE=4096)
sdk_add_compile_definitions(-DUSBD_DFU_APP_DEFAULT_ADD=0x08005000)
sdk_add_compile_definitions(-DUSBD_DFU_XFER_SIZE=4096)
endif()

if(${BL_TYPE} STREQUAL "uf2")
sdk_add_compile_definitions(-DUF2_BOARD_ID="Plum Bootloader for STM32F103")
sdk_add_compile_definitions(-DUF2_VOLUME_LABEL="STM32F1Plum")
sdk_add_compile_definitions(-DBOARD_UF2_FAMILY_ID=0xabcdf103)
endif()

sdk_add_compile_definitions(-DHSE_VALUE=8000000U)
sdk_add_compile_definitions(-DCOMPLETE_DELAY=50)

sdk_add_compile_definitions(-DPLUMBL_MCU_STM32F103)

sdk_set_main_file(Src/main.c)

if(${USE_MCU_BOOT})
project(${CHIP}_${BL_TYPE}_mcuboot)
else()
project(${CHIP}_${BL_TYPE})
endif()

