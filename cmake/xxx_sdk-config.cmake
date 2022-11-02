if(NOT DEFINED ENV{SDK_BASE})
    message( "please set SDK_BASE in your system environment")
endif()

set(SDK_BASE $ENV{SDK_BASE})

set(build_dir ${CMAKE_CURRENT_BINARY_DIR}/build_out)
set(PROJECT_SOURCE_DIR ${SDK_BASE})
set(PROJECT_BINARY_DIR ${build_dir})
set(EXECUTABLE_OUTPUT_PATH ${build_dir})
set(LIBRARY_OUTPUT_PATH ${PROJECT_BINARY_DIR}/lib)

set_property(GLOBAL APPEND PROPERTY SOURCE_LIST)
set_property(GLOBAL APPEND PROPERTY INCLUDE_LIST)
set_property(GLOBAL APPEND PROPERTY LINKLIB_LIST)

#添加一个接口库 所有跟SDK有关的都将被链接到这个库
add_library(sdk_intf_lib INTERFACE)

# # #add app库
# add_library(app_lib STATIC ${SDK_BASE}/empty_file.c)
# # add_library(app_lib STATIC)
# # add_library(app_lib INTERFACE)
# #链接
# target_link_libraries(app_lib sdk_intf_lib)

include(${SDK_BASE}/cmake/toolchain.cmake)
include(${SDK_BASE}/cmake/extension.cmake)
# include(${SDK_BASE}/cmake/compiler_flags.cmake)

# sdk_add_compile_options_ifdef(CONFIG_DEBUG -g3)
# sdk_add_compile_options(-Os)

enable_language(C CXX ASM)

add_subdirectory(${SDK_BASE} ${build_dir})