cmake_minimum_required(VERSION 3.15)

file(GLOB_RECURSE PlumUSB_SOURCES
    "${CMAKE_CURRENT_LIST_DIR}/PlumUSB/core/lg_usbd_core.c"
    "${CMAKE_CURRENT_LIST_DIR}/PlumUSB/port/${USBIP}/*.c"
    "${CMAKE_CURRENT_LIST_DIR}/PlumUSB/class/msc/lg_usbd_msc.c"
    "${CMAKE_CURRENT_LIST_DIR}/PlumUSB/class/hid/*.c"
)

file(GLOB PlumBOOT_SOURCES
    "${CMAKE_CURRENT_LIST_DIR}/src/core/*c"
    "${CMAKE_CURRENT_LIST_DIR}/src/port/${CHIP_FAMILY}/${BL_TYPE}/*.c"
    "${CMAKE_CURRENT_LIST_DIR}/src/port/${CHIP_FAMILY}/*.c"
)

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

sdk_append_source(${PlumUSB_SOURCES})
sdk_append_source(${PlumBOOT_SOURCES})

if(${BL_TYPE} STREQUAL "st_dfu")
sdk_append_source(${CMAKE_CURRENT_LIST_DIR}/PlumUSB/class/dfu/lg_usbd_dfu.c)
endif()

add_subdirectory(drv)

