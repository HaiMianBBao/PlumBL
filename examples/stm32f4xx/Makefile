SDK_DEMO_PATH ?= .
SDK_BASE = $(SDK_DEMO_PATH)/../../

export SDK_BASE

RM = cmake -E remove_directory
BL_TYPE?=uf2
DEBUG?=n
CHIP?=stm32f401
RTT_VIEWER?=y

#cmake definition config
cmake_definition+= -DTOOLCHAIN_PREFIX=arm-none-eabi-
cmake_definition+= -DCHIP_FAMILY=stm32f4xx
cmake_definition+= -DCHIP=${CHIP}
cmake_definition+= -DUSBIP=dwc2
cmake_definition+= -DBOARD=stm32f401ccu6_plum
cmake_definition+= -DDEBUG=${DEBUG}
cmake_definition+= -DBL_TYPE=${BL_TYPE}
cmake_definition+= -DRTT_VIEWER=${RTT_VIEWER}
# cmake_definition+= -DCONFIG_DEBUG=y

build:Makefile
	cmake -S . -B build -G "Unix Makefiles" $(cmake_definition)
	cd build && make -j4

clean:
	$(RM) build

menuconfig:
	python ../../tools/kconfig/menuconfig.py

.PHONY:build clean menuconfig
