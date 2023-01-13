#include <mcuboot_config/mcuboot_config.h>

// A user-defined identifier for different storage mediums (i.e internal flash, external NOR flash, eMMC, etc)
#define FLASH_DEVICE_INTERNAL_FLASH 1

// An arbitrarily high slot ID we will use to indicate that there is not slot
#define FLASH_SLOT_DOES_NOT_EXIST 255

// NB: MCUboot expects this define to exist but it's only used if MCUBOOT_SWAP_USING_SCRATCH=1 is set
#define FLASH_AREA_IMAGE_SCRATCH FLASH_SLOT_DOES_NOT_EXIST

// The slot we will use to track the bootloader allocation
#define FLASH_AREA_BOOTLOADER 1

// we only have enough flash for one slot
#define FLASH_AREA_IMAGE_PRIMARY(x)   2
#define FLASH_AREA_IMAGE_SECONDARY(x) 2