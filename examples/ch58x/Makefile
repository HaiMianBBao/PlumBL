RM = cmake -E remove_directory
BL_TYPE?=uf2
#cmake definition config
cmake_definition+= -DTOOLCHAIN_PREFIX=riscv-none-embed-
cmake_definition+= -DCHIP=ch58x
cmake_definition+= -DBOARD=ch58x_plum
cmake_definition+= -DCONFIG_DEBUG=y
cmake_definition+= -DBL_TYPE=${BL_TYPE}
build:Makefile
	cmake -S . -B build -G "Unix Makefiles" $(cmake_definition)
	cd build && make -j4

clean:
	$(RM) build

menuconfig:
	python ../../tools/kconfig/menuconfig.py

.PHONY:build clean menuconfig
