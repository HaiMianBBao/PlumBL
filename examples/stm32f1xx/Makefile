SDK_DEMO_PATH ?= .
SDK_BASE = $(SDK_DEMO_PATH)/../..
export SDK_BASE

ifeq ($(OS),Windows_NT)
CMAKE = $(SDK_BASE)/tools/cmake/bin/cmake.exe
else
CMAKE = $(SDK_BASE)/tools/cmake/bin/cmake
endif

RM = $(CMAKE) -E remove_directory

BL_TYPE?=uf2
DEBUG?=n
CHIP?=stm32f103
RTT_VIEWER?=y

#cmake definition config
cmake_definition+= -DTOOLCHAIN_PREFIX=arm-none-eabi-
cmake_definition+= -DCHIP_FAMILY=stm32f1xx
cmake_definition+= -DCHIP=${CHIP}
cmake_definition+= -DUSBIP=fsdev
cmake_definition+= -DBOARD=stm32f103c8t6_plum
cmake_definition+= -DDEBUG=${DEBUG}
cmake_definition+= -DBL_TYPE=${BL_TYPE}
cmake_definition+= -DRTT_VIEWER=${RTT_VIEWER}

# cmake_definition+= -DCONFIG_DEBUG=y

build:Makefile
	$(CMAKE) -S . -B build -G "Unix Makefiles" $(cmake_definition)
	cd build && make -j4

clean:
	$(RM) build

# menuconfig:
# 	python ../../tools/kconfig/menuconfig.py

# .PHONY:build clean menuconfig
