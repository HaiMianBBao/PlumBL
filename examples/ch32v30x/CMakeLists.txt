cmake_minimum_required(VERSION 3.15)

find_package(plumbl_sdk REQUIRED HINTS $ENV{PLUMBL_SDK_BASE})
sdk_append_inc(.)

if(DEBUG)
sdk_add_compile_definitions(-DDEBUG=1)
sdk_add_compile_definitions(-DLGK_BOOT_DEBUG=1)
endif()

if(USE_MCU_BOOT)
sdk_append_source(test_key.c)
sdk_add_compile_definitions(-DUSE_MCU_BOOT)
endif()

if(${BL_TYPE} STREQUAL "dfu")
sdk_add_compile_definitions(-DUSB_REQUEST_BUFFER_SIZE=4096)
sdk_add_compile_definitions(-DUSBD_DFU_APP_DEFAULT_ADD=0x10000)
sdk_add_compile_definitions(-DUSBD_DFU_XFER_SIZE=4096)
endif()

if(${BL_TYPE} STREQUAL "uf2")
sdk_add_compile_definitions(-DUF2_BOARD_ID="Plum Bootloader")
sdk_add_compile_definitions(-DUF2_VOLUME_LABEL="EasyUF2")
sdk_add_compile_definitions(-DBOARD_UF2_FAMILY_ID=0xabcdc320)
endif()

sdk_add_compile_definitions(-DPLUMBL_MCU_CH32V30x)

sdk_set_main_file(main.c)

if(USE_MCU_BOOT)
project(${CHIP}_${BL_TYPE}_mcuboot)
else()
project(${CHIP}_${BL_TYPE})
endif()
