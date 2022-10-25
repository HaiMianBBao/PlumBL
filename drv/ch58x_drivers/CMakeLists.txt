file(GLOB_RECURSE CH582_SOURCE
"${CMAKE_CURRENT_SOURCE_DIR}/StdPeriphDriver/*.c"
"${CMAKE_CURRENT_SOURCE_DIR}/RVMSIS/*.c"
"${CMAKE_CURRENT_SOURCE_DIR}/Startup/startup_CH583.S"
)

list(REMOVE_ITEM CH582_SOURCE
    "${CMAKE_CURRENT_LIST_DIR}/StdPeriphDriver/CH58x_usbdev.c"
    "${CMAKE_CURRENT_LIST_DIR}/StdPeriphDriver/CH58x_usbhostBase.c"
    "${CMAKE_CURRENT_LIST_DIR}/StdPeriphDriver/CH58x_usbhostClass.c"
    "${CMAKE_CURRENT_LIST_DIR}/StdPeriphDriver/CH58x_usb2dev.c"
    "${CMAKE_CURRENT_LIST_DIR}/StdPeriphDriver/CH58x_usb2hostBase.c"
    "${CMAKE_CURRENT_LIST_DIR}/StdPeriphDriver/CH58x_usb2hostClass.c"
    "${CMAKE_CURRENT_LIST_DIR}/StdPeriphDriver/CH58x_timer0.c"
    "${CMAKE_CURRENT_LIST_DIR}/StdPeriphDriver/CH58x_timer1.c"
    "${CMAKE_CURRENT_LIST_DIR}/StdPeriphDriver/CH58x_timer2.c"
    "${CMAKE_CURRENT_LIST_DIR}/StdPeriphDriver/CH58x_timer3.c"
    "${CMAKE_CURRENT_LIST_DIR}/StdPeriphDriver/CH58x_adc.c"
    "${CMAKE_CURRENT_LIST_DIR}/StdPeriphDriver/CH58x_i2c.c"
    "${CMAKE_CURRENT_LIST_DIR}/StdPeriphDriver/CH58x_pwm.c"
    "${CMAKE_CURRENT_LIST_DIR}/StdPeriphDriver/CH58x_spi0.c"
    "${CMAKE_CURRENT_LIST_DIR}/StdPeriphDriver/CH58x_spi1.c"
)

sdk_generate_library()
sdk_add_link_libraries(${CMAKE_CURRENT_SOURCE_DIR}/StdPeriphDriver/libISP583.a)


#添加源文件参与编译
sdk_library_add_sources(${CH582_SOURCE})

sdk_add_include_directories(
    StdPeriphDriver/inc
    RVMSIS
)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_C_STANDARD 99)

sdk_add_compile_options(-march=rv32imac -mabi=ilp32 -Wall -mcmodel=medany -msmall-data-limit=8 -mno-save-restore -std=gnu99)
sdk_add_compile_options(-fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -g)

sdk_set_linker_script(${CMAKE_CURRENT_SOURCE_DIR}/Ld/plum_boot.ld)

sdk_add_link_options(-nostartfiles
                -Xlinker --gc-sections  
                -Wl,--print-memory-usage 
                # -Wl,-Map,${PROJECT_NAME}.map 
                --specs=nano.specs 
                --specs=nosys.specs)
