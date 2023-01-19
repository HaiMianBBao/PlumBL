if(NOT DEFINED ENV{PLUMBL_SDK_BASE})
    message( "please set PLUMBL_SDK_BASE in your system environment")
endif()

set(PLUMBL_SDK_BASE $ENV{PLUMBL_SDK_BASE})
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_C_STANDARD 99)

set(build_dir ${CMAKE_CURRENT_BINARY_DIR}/build_out)
set(PROJECT_SOURCE_DIR ${PLUMBL_SDK_BASE})
set(PROJECT_BINARY_DIR ${build_dir})
set(EXECUTABLE_OUTPUT_PATH ${build_dir})
set(LIBRARY_OUTPUT_PATH ${PROJECT_BINARY_DIR}/lib)

set_property(GLOBAL APPEND PROPERTY SOURCE_LIST)
set_property(GLOBAL APPEND PROPERTY INCLUDE_LIST)

add_library(sdk_intf_lib INTERFACE)

include(${PLUMBL_SDK_BASE}/cmake/showlog.cmake)
include(${PLUMBL_SDK_BASE}/cmake/toolchain.cmake)
include(${PLUMBL_SDK_BASE}/cmake/extension.cmake)

enable_language(C CXX ASM)

add_subdirectory(${PLUMBL_SDK_BASE} ${build_dir})