cmake_minimum_required(VERSION 3.15)

sdk_append_inc(${CMAKE_CURRENT_LIST_DIR}/PlumUSB/class/hid)
sdk_append_inc(${CMAKE_CURRENT_LIST_DIR}/PlumUSB/class/cdc)
sdk_append_inc(${CMAKE_CURRENT_LIST_DIR}/PlumUSB/class/msc)
sdk_append_inc(${CMAKE_CURRENT_LIST_DIR}/PlumUSB/class/dfu)
sdk_append_inc(${CMAKE_CURRENT_LIST_DIR}/PlumUSB/common)
sdk_append_inc(${CMAKE_CURRENT_LIST_DIR}/PlumUSB/core)
sdk_append_inc(${CMAKE_CURRENT_LIST_DIR}/PlumUSB/port/${USBIP})
sdk_append_inc(${CMAKE_CURRENT_LIST_DIR}/src/core)
sdk_append_inc(${CMAKE_CURRENT_LIST_DIR}/src/port/${CHIP_FAMILY}/${BL_TYPE})
sdk_append_inc(${CMAKE_CURRENT_LIST_DIR}/src/port/${CHIP_FAMILY})

# USB source
sdk_append_source(${CMAKE_CURRENT_LIST_DIR}/PlumUSB/core/lg_usbd_core.c)
sdk_append_source(${CMAKE_CURRENT_LIST_DIR}/PlumUSB/port/${USBIP}/*.c)
sdk_append_source(${CMAKE_CURRENT_LIST_DIR}/PlumUSB/class/msc/lg_usbd_msc.c)
sdk_append_source(${CMAKE_CURRENT_LIST_DIR}/PlumUSB/class/hid/*.c)
if(${BL_TYPE} STREQUAL "dfu")
sdk_append_source(${CMAKE_CURRENT_LIST_DIR}/PlumUSB/class/dfu/lg_usbd_dfu.c)
endif()

# Bootloader source
sdk_append_source(${CMAKE_CURRENT_LIST_DIR}/src/core/*.c)
sdk_append_source(${CMAKE_CURRENT_LIST_DIR}/src/port/${CHIP_FAMILY}/${BL_TYPE}/*.c)
sdk_append_source(${CMAKE_CURRENT_LIST_DIR}/src/port/${CHIP_FAMILY}/*.c)

add_subdirectory(drv)

