set(CMAKE_CXX_STANDARD 11)
set(CMAKE_C_STANDARD 99)

sdk_add_compile_options(-march=rv32imac -mabi=ilp32 -Wall -mcmodel=medany -msmall-data-limit=8 -mno-save-restore -std=gnu99)
sdk_add_compile_options(-fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -g -Os)

sdk_set_linker_script(${CMAKE_CURRENT_SOURCE_DIR}/Ld/plum_boot.ld)

sdk_add_link_options(-nostartfiles
                -Xlinker --gc-sections  
                -Wl,--print-memory-usage 
                --specs=nano.specs 
                --specs=nosys.specs)