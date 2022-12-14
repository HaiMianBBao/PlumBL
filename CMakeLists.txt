cmake_minimum_required(VERSION 3.15)

sdk_append_inc(${CMAKE_CURRENT_LIST_DIR}/CherryUSB/class/hid)
sdk_append_inc(${CMAKE_CURRENT_LIST_DIR}/CherryUSB/class/cdc)
sdk_append_inc(${CMAKE_CURRENT_LIST_DIR}/CherryUSB/class/msc)
sdk_append_inc(${CMAKE_CURRENT_LIST_DIR}/CherryUSB/class/dfu)
sdk_append_inc(${CMAKE_CURRENT_LIST_DIR}/CherryUSB/common)
sdk_append_inc(${CMAKE_CURRENT_LIST_DIR}/CherryUSB/core)
sdk_append_inc(${CMAKE_CURRENT_LIST_DIR}/CherryUSB/port/${USBIP})
sdk_append_inc(${CMAKE_CURRENT_LIST_DIR}/src/core)
sdk_append_inc(${CMAKE_CURRENT_LIST_DIR}/src/port/${CHIP_FAMILY}/${BL_TYPE})
sdk_append_inc(${CMAKE_CURRENT_LIST_DIR}/src/port/${CHIP_FAMILY})

# USB source
sdk_append_source(${CMAKE_CURRENT_LIST_DIR}/CherryUSB/core/usbd_core.c)
sdk_append_source(${CMAKE_CURRENT_LIST_DIR}/CherryUSB/port/${USBIP}/${USB_DCD})
sdk_append_source(${CMAKE_CURRENT_LIST_DIR}/CherryUSB/class/msc/usbd_msc.c)
sdk_append_source(${CMAKE_CURRENT_LIST_DIR}/CherryUSB/class/hid/usbd_hid.c)
if(${BL_TYPE} STREQUAL "dfu")
sdk_append_source(${CMAKE_CURRENT_LIST_DIR}/CherryUSB/class/dfu/usbd_dfu.c)
endif()

# Bootloader source
sdk_append_source(${CMAKE_CURRENT_LIST_DIR}/src/core/*.c)
sdk_append_source(${CMAKE_CURRENT_LIST_DIR}/src/port/common/${BL_TYPE}/*.c)
sdk_append_source(${CMAKE_CURRENT_LIST_DIR}/src/port/${CHIP_FAMILY}/*.c)

add_subdirectory(drv)

