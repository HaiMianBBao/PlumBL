sdk_add_compile_options(-march=rv32imafc -mabi=ilp32f -mcmodel=medany -msmall-data-limit=8 -mno-save-restore)
sdk_add_compile_options(-fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -fno-common -fsingle-precision-constant -g -Os)

sdk_set_linker_script(${CMAKE_CURRENT_LIST_DIR}/linker/ch32v30x.ld)
sdk_add_link_options(-march=rv32imafc -mabi=ilp32f -nostartfiles -Xlinker --gc-sections  -Wl,--print-memory-usage --specs=nano.specs --specs=nosys.specs)