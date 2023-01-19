sdk_append_source($ENV{PLUMBL_SDK_BASE}/board/startup/startup_stm32l073xx.s)
sdk_set_linker_script($ENV{PLUMBL_SDK_BASE}/board/linker/STM32L073VBTx_FLASH.ld)
sdk_add_compile_definitions(-DSTM32L073xx)

sdk_add_compile_options(-mcpu=cortex-m0plus
                        -mthumb 
                        -Og
                        -Wall
                        -fdata-sections 
                        -ffunction-sections
                        --specs=nano.specs  
                        --specs=nosys.specs)

sdk_add_link_options(-mcpu=cortex-m0plus
                     -mthumb 
                     --specs=nano.specs 
                     -lc -lm
                     -Wl,--cref -Wl,--gc-sections 
                     --specs=nosys.specs)

if(${GCC_VERSION_MAJOR} GREATER 11)
sdk_add_link_options(-Wl,--no-warn-rwx-segments)
endif()