cmake_minimum_required(VERSION 3.15)

find_package(plumbl_sdk REQUIRED HINTS $ENV{PLUMBL_SDK_BASE})

sdk_append_inc(.)
sdk_append_inc(boards)
sdk_append_inc(boards/${BOARD})
sdk_append_inc(cmsis/include)
sdk_append_inc(usb)
sdk_append_inc(usb/uf2)

sdk_append_source(boards/boards.c)
sdk_append_source(dfu_ble_svc.c)
sdk_append_source(dfu_init.c)
sdk_append_source(flash_nrf5x.c)

# UART or USB Serial
if($(MCU_SUB_VARIANT) STREQUAL "nrf52")
else()
# pinconfig is required for 840 for CF2
sdk_append_source(boards/${BOARD}/pinconfig.c)
# USB Application ( MSC + UF2 )
sdk_append_source(usb/uf2/ghostfat.c)
sdk_append_source(usb/msc_uf2.c)
sdk_append_source(usb/usb_desc.c)
sdk_append_source(usb/usb.c)
endif()

sdk_add_compile_definitions(-DPLUMBL_MCU_NRF5X)

sdk_set_main_file(main.c)
project(${CHIP}_${BL_TYPE})

get_target_property(C_CPP_PROPERTIES_DEFINES sdk_intf_lib INTERFACE_COMPILE_DEFINITIONS)
get_target_property(C_CPP_PROPERTIES_OPITONS sdk_intf_lib INTERFACE_COMPILE_OPTIONS)

set(SD_HEX  drv/nrf5x_drivers/softdevice/${SD_FILENAME}/${SD_FILENAME}_softdevice.hex)
set(MBR_HEX drv/nrf5x_drivers/softdevice/mbr/hex/mbr_nrf52_2.4.1_mbr.hex)

add_custom_command(TARGET ${CHIP}_${BL_TYPE}.elf POST_BUILD
COMMAND python ../../../utils/hexmerge.py --overlap=replace -o ${CHIP}_${BL_TYPE}_mbr_nosd.hex build_out/${CHIP}_${BL_TYPE}.hex ../../../${MBR_HEX}
COMMAND python ../../../utils/hexmerge.py -o ${CHIP}_${BL_TYPE}_mbr_sd.hex build_out/${CHIP}_${BL_TYPE}.hex ../../../${SD_HEX})