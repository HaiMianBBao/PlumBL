cmake_minimum_required(VERSION 3.15)

find_package(plumbl_sdk REQUIRED HINTS $ENV{PLUMBL_SDK_BASE})
sdk_append_inc(.)

if(DEBUG)
sdk_add_compile_definitions(-DDEBUG=1)
sdk_add_compile_definitions(-DLGK_BOOT_DEBUG=1)
endif()

if(${BL_TYPE} STREQUAL "dfu")
sdk_add_compile_definitions(-DUSB_REQUEST_BUFFER_SIZE=4096)
sdk_add_compile_definitions(-DUSBD_DFU_APP_DEFAULT_ADD=0x10000)
sdk_add_compile_definitions(-DUSBD_DFU_XFER_SIZE=4096)
endif()

sdk_set_main_file(main.c)

project(${CHIP}_${BL_TYPE})