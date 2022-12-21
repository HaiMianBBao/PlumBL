SDK_DEMO_PATH ?= .
PLUMBL_SDK_BASE = $(SDK_DEMO_PATH)/../..
export PLUMBL_SDK_BASE

BL_TYPE?=uf2
DEBUG?=n
CHIP?=stm32f401
CHIP_FAMILY?=stm32f4xx
BOARD?=stm32f401ccu6_plum
USBIP?=dwc2
USB_DCD?=usb_dc_dwc2.c
RTT_VIEWER?=y
TOOLCHAIN_PREFIX ?= arm-none-eabi-

include $(PLUMBL_SDK_BASE)/project.build
