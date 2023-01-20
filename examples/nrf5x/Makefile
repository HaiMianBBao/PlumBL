SDK_DEMO_PATH ?= .
PLUMBL_SDK_BASE = $(SDK_DEMO_PATH)/../..
export PLUMBL_SDK_BASE

BL_TYPE?=uf2
DEBUG?=n
CHIP?=nrf52833
CHIP_FAMILY?=nrf5x
BOARD?=feather_nrf52833_express
USBIP?=nrf5x
USB_DCD?=usb_dc_nrf5x.c
RTT_VIEWER?=y
TOOLCHAIN_PREFIX ?= arm-none-eabi-

include $(PLUMBL_SDK_BASE)/examples/nrf5x/boards/${BOARD}/board.mk

GIT_VERSION := 0.7.0-10-gda70d4a-dirty
GIT_SUBMODULE_VERSIONS := lib/nrfx (v2.0.0) lib/tinyusb (0.12.0-145-g9775e7691) lib/uf2 (remotes/origin/configupdate-9-gadbb8c7)
_VER = $(subst ., ,$(word 1, $(subst -, ,$(GIT_VERSION))))
MK_BOOTLOADER_VERSION = '($(word 1,$(_VER)) << 16) + ($(word 2,$(_VER)) << 8) + $(word 3,$(_VER))'

# MCU_SUB_VARIANT can be nrf52 (nrf52832), nrf52833, nrf52840
ifeq ($(MCU_SUB_VARIANT),nrf52)
  SD_NAME = s132
  DFU_DEV_REV = 0xADAF
  DFU_APP_DATA_RESERVED=7*4096
else ifeq ($(MCU_SUB_VARIANT),nrf52833)
  SD_NAME = s140
  DFU_DEV_REV = 52833
  DFU_APP_DATA_RESERVED=7*4096
else ifeq ($(MCU_SUB_VARIANT),nrf52840)
  SD_NAME = s140
  DFU_DEV_REV = 52840
  DFU_APP_DATA_RESERVED=10*4096
else
  $(error Sub Variant $(MCU_SUB_VARIANT) is unknown)
endif

# SD_VERSION can be overwritten by board.mk
ifndef SD_VERSION
SD_VERSION   = 6.1.1
endif

SD_FILENAME  = $(SD_NAME)_nrf52_$(SD_VERSION)
SD_PATH      = ${PLUMBL_SDK_BASE}/drv/nrf5x_drivers/softdevice/$(SD_FILENAME)

cmake_definition += -DMCU_SUB_VARIANT=${MCU_SUB_VARIANT}
cmake_definition += -DSD_PATH=${SD_PATH}
cmake_definition += -DSD_FILENAME=${SD_FILENAME}
cmake_definition += -DDFU_APP_DATA_RESERVED=${DFU_APP_DATA_RESERVED}
# cmake_definition += -DSD_NAME=${SD_NAME}
# cmake_definition += -DSD_VERSION=${SD_VERSION}
cmake_definition += -DMK_BOOTLOADER_VERSION=${MK_BOOTLOADER_VERSION}
cmake_definition += -DUF2_VERSION='"${GIT_VERSION} ${GIT_SUBMODULE_VERSIONS}"'
cmake_definition += -DBLEDIS_FW_VERSION='"$(GIT_VERSION) $(SD_NAME) $(SD_VERSION)"'

ifeq ($(OS),Windows_NT)
CMAKE = $(PLUMBL_SDK_BASE)/tools/cmake/bin/cmake.exe
else
CMAKE = $(PLUMBL_SDK_BASE)/tools/cmake/bin/cmake
endif

RM = $(CMAKE) -E remove_directory

#cmake definition config
cmake_definition+= -DTOOLCHAIN_PREFIX=${TOOLCHAIN_PREFIX}
cmake_definition+= -DCHIP_FAMILY=${CHIP_FAMILY}
cmake_definition+= -DCHIP=${CHIP}
# cmake_definition+= -DUSBIP=${USBIP}
# cmake_definition+= -DUSB_DCD=${USB_DCD}
cmake_definition+= -DBOARD=${BOARD}
cmake_definition+= -DDEBUG=${DEBUG}
cmake_definition+= -DBL_TYPE=${BL_TYPE}
# cmake_definition+= -DRTT_VIEWER=${RTT_VIEWER}
                                                
build:Makefile
	@$(CMAKE) -S . -B build -G "Unix Makefiles" $(cmake_definition)
	cd build && make -j4

ninja:
	@$(CMAKE) -S . -B build -G "Ninja" $(cmake_definition)
	cd build && ninja -j4

clean:
	$(RM) build

menuconfig:
	python $(PLUMBL_SDK_BASE)/tools/kconfig/menuconfig.py

.PHONY:build clean menuconfig



# include $(PLUMBL_SDK_BASE)/project.build