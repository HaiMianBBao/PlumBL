SDK_DEMO_PATH ?= .
SDK_BASE = $(SDK_DEMO_PATH)/../../

export SDK_BASE

RM = cmake -E remove_directory
BL_TYPE?=uf2
DEBUG?=n
CHIP?=stm32l433

#cmake definition config
cmake_definition+= -DTOOLCHAIN_PREFIX=arm-none-eabi-
cmake_definition+= -DCHIP_FAMILY=stm32l4xx
cmake_definition+= -DCHIP=${CHIP}
cmake_definition+= -DUSBIP=fsdev
cmake_definition+= -DBOARD=stm32l433rct6_plum
cmake_definition+= -DDEBUG=${DEBUG}
cmake_definition+= -DBL_TYPE=${BL_TYPE}
# cmake_definition+= -DCONFIG_DEBUG=y

build:Makefile
	cmake -S . -B build -G "Unix Makefiles" $(cmake_definition)
	cd build && make -j4

clean:
	$(RM) build

menuconfig:
	python ../../tools/kconfig/menuconfig.py

.PHONY:build clean menuconfig
