sdk_append_source($ENV{PLUMBL_SDK_BASE}/board/startup/startup_stm32l433xx.s)
sdk_set_linker_script($ENV{PLUMBL_SDK_BASE}/board/linker/STM32L433RCTx_FLASH.ld)
sdk_add_compile_definitions(-DSTM32L433xx)

sdk_add_compile_options(-mcpu=cortex-m4 
                        -mthumb 
                        -mfpu=fpv4-sp-d16 
                        -mfloat-abi=hard 
                        -Og
                        -Wall
                        -fdata-sections 
                        -ffunction-sections
                        --specs=nano.specs  
                        --specs=nosys.specs)

sdk_add_link_options(-mcpu=cortex-m4 
                     -mthumb 
                     -mfpu=fpv4-sp-d16 
                     -mfloat-abi=hard 
                     --specs=nano.specs 
                     -lc -lm
                     -Wl,--cref -Wl,--gc-sections 
                     --specs=nosys.specs)

if(${GCC_VERSION_MAJOR} GREATER 11)
sdk_add_link_options(-Wl,--no-warn-rwx-segments)
endif()