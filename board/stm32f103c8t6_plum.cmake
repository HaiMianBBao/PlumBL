set(CMAKE_CXX_STANDARD 11)
set(CMAKE_C_STANDARD 99)

sdk_set_linker_script($ENV{PLUMBL_SDK_BASE}/board/linker/STM32F103C8Tx_FLASH.ld)
sdk_add_compile_definitions(-DSTM32F103xB)
sdk_append_source($ENV{PLUMBL_SDK_BASE}/board/startup/startup_stm32f103xb.s)

sdk_add_compile_options(-mcpu=cortex-m3
                        -mthumb 
                        -Og
                        -Wall
                        -fdata-sections 
                        -ffunction-sections
                        -g
                        --specs=nano.specs 
                        --specs=nosys.specs
                        )

sdk_add_link_options(-mcpu=cortex-m3 
                     -mthumb 
                     --specs=nano.specs 
                     -lc -lm
                     -Wl,--cref -Wl,--gc-sections 
                     --specs=nosys.specs
                    )
