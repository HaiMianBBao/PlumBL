# PLUM_BOOTLOADER
This is a lightweight microcontroller bootloader. At present, you can use u2f, st dfu tools, to complete the firmware upgrade. At the same time, it is also portable and can be easily transplanted to other chips. It only needs to implement the interface under the port folder. At present, I have finished the transplantation of ch58x series.

If you want to realize the interaction between app and bootloader, you need to implement a shared segment boot segment between app and bootloader in RAM, and set the length to four bytes.

You need to implement whether to jump to app by yourself, just implement the following function.

This PlumUSB is written when learning CherryUSB. You can replace it with CherryUSB.
```c
bool lgk_boot_app_is_vaild(uint32_t check_code_add)
{
    /*!< Your code >*/
    return true;
}
```

```c
// bootloader
__attribute__((section(".boot"))) uint32_t  lgk_boot_flag;

// bootloader link file
FLASH (rx) : ORIGIN = 0x00000000, LENGTH = 20K
BOOT (xrw) : ORIGIN = 0x20007ffc, LENGTH = 4
RAM (xrw) : ORIGIN = 0x20000000, LENGTH = 32K - 4


.boot :
{
    . = ALIGN(4);
    *(.noinit)
    *(.boot)
    *(.boot.*)
    . = ALIGN(4);
} >BOOT AT> FLASH
```

# Build
```
source env.sh
cd examples/ch58x
make
```

# Make UF2 file
You need to set the starting address of your firmware to 0x10000. You can modify the link file in your own project file to achieve this purpose.
When you have the xxx. hex file, you only need to go to the official side of UF2 to get the uf2conv.py and uf2Families.json files.
```
#define BOARD_UF2_FAMILY_ID  0xbeefc582 // BOARD_UF2_FAMILY_ID is defined in uf2.h

Then you can run:
python uf2conv.py xxx.hex -o xxx.uf2 -c -f 0xbeefc582
```