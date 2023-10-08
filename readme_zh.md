# PLUM_BOOTLOADER
这是一个轻量级的单片机引导程序。目前，你可以使用u2f、dfu-util.exe工具来完成固件升级。同时，它也是可移植的，可以很容易地移植到其他芯片上。你只需要实现port文件夹下的接口函数。目前，我已经完成了ch58x系列、ch32v30x、stm32f401、stm32f103、stm32l433的移植。

Ch58x 你需要自己实现是否要跳转到app的函数
```
bool lgk_boot_app_is_vaild(uint32_t check_code_add)
{
    /*!< Your code >*/
    return true;
}
```

# 克隆
## step 1
```
git clone https://github.com/HaiMianBBao/PlumBL.git
or
git clone git@github.com:HaiMianBBao/PlumBL.git
```
## step 2
```
cd PlumBL
git submodule update --init --recursive
```

# 构建
## uf2
```
cd examples/xxx
make or make BL_TYPE=uf2
```

## dfu
```
cd examples/xxx
make BL_TYPE=dfu
```
## mcu boot
```
cd examples/xxx
make BL_TYPE=uf2 USE_MCU_BOOT=yes
or
make BL_TYPE=dfu USE_MCU_BOOT=yes
```

# 制作APP文件
```
ch58x:
#define BOARD_FLASH_APP_START 0x00010000UL

ch32v30x:
#define BOARD_FLASH_APP_START 0x08010000UL

stm32f103:
#define BOARD_FLASH_APP_START 0x08005000UL

stm32f401:
#define BOARD_FLASH_APP_START 0x08010000UL

stm32l433:
#define BOARD_FLASH_APP_START 0x08010000UL
```
你需要将app固件的起始地址设置为BOARD_FLASH_APP_START。您可以在自己的项目文件中修改链接文件以实现此目的。
当你有了xxx.hex或者xxx.bin文件，你只需要去uf2官方拿到uf2conv.py和uf2families.json文件，并且我已经将这俩个文件放在utils文件夹下面
## uf2
```
ch58x:
#define BOARD_UF2_FAMILY_ID  0xabcdc582 // BOARD_UF2_FAMILY_ID is defined in uf2.h

ch32v30x:
#define BOARD_UF2_FAMILY_ID  0xabcdc320 // BOARD_UF2_FAMILY_ID is defined in uf2.h

stm32f103:
#define BOARD_UF2_FAMILY_ID  0xabcdf103 // BOARD_UF2_FAMILY_ID is defined in uf2.h

stm32f401:
#define BOARD_UF2_FAMILY_ID  0xabcdf401 // BOARD_UF2_FAMILY_ID is defined in uf2.h

stm32l433:
#define BOARD_UF2_FAMILY_ID  0xabcd1433 // BOARD_UF2_FAMILY_ID is defined in uf2.h

接下来你就可以运行下面的命令来生成uf2文件
hex:
python uf2conv.py xxx.hex -o xxx.uf2 -c -f BOARD_UF2_FAMILY_ID

1、不使用mcuboot
bin:
python uf2conv.py xxx.bin -o xxx.uf2 -c -f BOARD_UF2_FAMILY_ID -b BOARD_FLASH_APP_START

2、使用mcuboot
python uf2conv.py xxx.bin -o xxx.uf2 -c -f BOARD_UF2_FAMILY_ID -b BOARD_FLASH_APP_START - HEAD_SIZE
HEAD_SIZE的默认值是0x1000
```
有了.uf2文件，你就可以将.uf2文件拖入u盘来实现固件升级了。

## dfu
DfuSeDemo是ST的官方dfu烧录软件，但它需要将bin文件转换为dfu文件才能使用。当你安装DfuSeDemo时，bin转dfu的软件将会自动安装。你可以选择这种方法，但我不推荐。
在windows上使用dfu的另一种方法是通过dfu-util.exe。其他平台需要自己探索。这里我们以ch58x为例。步骤如下：

### Step1
在utils文件夹下运行zadig-2.7.exe，确保设备处于dfu模式。
单击选项并列出所有设备，如下图所示。你可以看到设备PlumBootloader，然后选择它，

![](img/image0.png)

然后单击“替换驱动程序”，将驱动程序替换为winusb。

![](img/image1.png)

### Step2
现在，你可以在utils文件夹下打开终端并输入./CH58x_dfu_util.sh
如果成功，你将看到:

```
./CH58x_dfu_util.sh
dfu-util 0.11

Copyright 2005-2009 Weston Schmidt, Harald Welte and OpenMoko Inc.
Copyright 2010-2021 Tormod Volden and Stefan Schmidt
This program is Free Software and has ABSOLUTELY NO WARRANTY
Please report bugs to http://sourceforge.net/p/dfu-util/tickets/

Found DFU: [0483:df11] ver=2200, devnum=36, cfg=1, intf=0, path="1-2", alt=0, name="@Internal Flash   /0x00000000/16*004Ka,096*04Kg", serial="2022123456"
dfu-util 0.11

Copyright 2005-2009 Weston Schmidt, Harald Welte and OpenMoko Inc.
Copyright 2010-2021 Tormod Volden and Stefan Schmidt
This program is Free Software and has ABSOLUTELY NO WARRANTY
Please report bugs to http://sourceforge.net/p/dfu-util/tickets/

Warning: Invalid DFU suffix signature
A valid DFU suffix will be required in a future dfu-util release
Opening DFU capable USB device...
Device ID 0483:df11
Device DFU version 011a
Claiming USB DFU Interface...
Setting Alternate Interface #0 ...
Determining device status...
DFU state(2) = dfuIDLE, status(0) = No error condition is present
DFU mode device DFU version 011a
Device returned transfer size 4096
DfuSe interface name: "Internal Flash   "
Downloading element to address = 0x00010000, size = 27360
Erase           [=========================] 100%        27360 bytes
Erase    done.
Download        [=========================] 100%        27360 bytes
Download done.
File downloaded successfully
Submitting leave request...
Error during download get_status
```
The bin file of this test will make your settings into a daplink.

# The app interacts with the boot
Your app should have the following code
```
ch58x:
void boot_jump(void)
{
    uint32_t *boot_magic = (uint32_t *)0x20007ffc;
    *boot_magic = 0xc220b134;
    mcu_reset();
}

ch32v30x:
void boot_jump(void)
{
    uint32_t *boot_magic = (uint32_t *)0x20007ffc;
    *boot_magic = 0xc220b134;
    mcu_reset();
}

stm32f103:
void boot_jump(void)
{
    uint32_t *boot_magic = (uint32_t *)0x20004c00;
    *boot_magic = 0xc220b134;
    mcu_reset();
}

stm32f401:
void boot_jump(void)
{
    uint32_t *boot_magic = (uint32_t *)0x2000fc00;
    *boot_magic = 0xc220b134;
    mcu_reset();
}

stm32l433:
void boot_jump(void)
{
    uint32_t *boot_magic = (uint32_t *)0x2000fc00;
    *boot_magic = 0xc220b134;
    mcu_reset();
}
```