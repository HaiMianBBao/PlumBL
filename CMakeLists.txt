cmake_minimum_required(VERSION 3.15)

include_directories(${CMAKE_CURRENT_LIST_DIR}
    ${CMAKE_CURRENT_LIST_DIR}/PlumUSB/class/hid
    ${CMAKE_CURRENT_LIST_DIR}/PlumUSB/class/cdc
    ${CMAKE_CURRENT_LIST_DIR}/PlumUSB/class/msc
    ${CMAKE_CURRENT_LIST_DIR}/PlumUSB/class/dfu
    ${CMAKE_CURRENT_LIST_DIR}/PlumUSB/common
    ${CMAKE_CURRENT_LIST_DIR}/PlumUSB/core
    ${CMAKE_CURRENT_LIST_DIR}/PlumUSB/port/${CHIP}
)

include_directories(${CMAKE_CURRENT_LIST_DIR}
    ${CMAKE_CURRENT_LIST_DIR}/src/core
    ${CMAKE_CURRENT_LIST_DIR}/src/port/${CHIP}/${BL_TYPE}
)


file(GLOB_RECURSE PlumUSB_SOURCES
    "${CMAKE_CURRENT_LIST_DIR}/PlumUSB/core/lg_usbd_core.c"
    "${CMAKE_CURRENT_LIST_DIR}/PlumUSB/port/${CHIP}/lg_usb_dc_port.c"
    "${CMAKE_CURRENT_LIST_DIR}/PlumUSB/class/msc/lg_usbd_msc.c"
    "${CMAKE_CURRENT_LIST_DIR}/PlumUSB/class/dfu/*.c"
    "${CMAKE_CURRENT_LIST_DIR}/PlumUSB/class/hid/*.c"
)

file(GLOB_RECURSE PlumBOOT_SOURCES
"${CMAKE_CURRENT_LIST_DIR}/src/core/*c"
"${CMAKE_CURRENT_LIST_DIR}/src/port/${CHIP}/${BL_TYPE}/*.c"
)


sdk_generate_library()
file(GLOB SOURCES
    ${PlumUSB_SOURCES}
    ${PlumBOOT_SOURCES}
)
sdk_library_add_sources(${SOURCES})

add_subdirectory(drv)

