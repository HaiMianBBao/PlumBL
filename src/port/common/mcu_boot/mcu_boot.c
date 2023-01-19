#include <string.h>
#include <stdlib.h>
#include <assert.h>
#include "stdbool.h"
#include "lgk_boot_core.h"
#include "port_common.h"
#include "flash_map_backend/flash_map_backend.h"
#include "sysflash/sysflash.h"
#include "mcuboot_config/mcuboot_logging.h"
#include <bootutil/sign_key.h>

#if defined(MCUBOOT_SIGN_RSA)
#define HAVE_KEYS
extern const unsigned char rsa_pub_key[];
extern const unsigned int rsa_pub_key_len;
#elif defined(MCUBOOT_SIGN_EC256)
#define HAVE_KEYS
extern const unsigned char ecdsa_pub_key[];
extern unsigned int ecdsa_pub_key_len;
#endif

/*
 * NOTE: *_pub_key and *_pub_key_len are autogenerated based on the provided
 *       key file. If no key file was configured, the array and length must be
 *       provided and added to the build manually.
 */
#if defined(HAVE_KEYS)
const struct bootutil_key bootutil_keys[] = {
    {
#if defined(MCUBOOT_SIGN_RSA)
        .key = rsa_pub_key,
        .len = &rsa_pub_key_len,
#elif defined(MCUBOOT_SIGN_EC256)
        .key = ecdsa_pub_key,
        .len = &ecdsa_pub_key_len,
#endif
    },
};
const int bootutil_key_cnt = 1;
#endif /* HAVE_KEYS */

static const struct flash_area iap = {
    .fa_id = FLASH_AREA_BOOTLOADER,
    .fa_device_id = FLASH_DEVICE_INTERNAL_FLASH,
    .fa_off = BOOT_START_ADDRESS,
    // a bias is applied deliberately to accommodate the header used by mcuboot
    .fa_size = APP_CODE_START_ADDR - BOOT_START_ADDRESS,
};

static const struct flash_area app = {
    .fa_id = FLASH_AREA_IMAGE_PRIMARY(0),
    .fa_device_id = FLASH_DEVICE_INTERNAL_FLASH,
    .fa_off = APP_CODE_START_ADDR,
    .fa_size = APP_CODE_END_ADDR - APP_CODE_START_ADDR,
};

static const struct flash_area *s_flash_areas[] = {
    &iap,
    &app,
};

static const struct flash_area *prv_lookup_flash_area(uint8_t id)
{
    for (uint32_t i = 0; i < sizeof(s_flash_areas) / sizeof(s_flash_areas[0]); i++) {
        const struct flash_area *area = s_flash_areas[i];

        if (id == area->fa_id) {
            return area;
        }
    }
    return NULL;
}

int flash_area_open(uint8_t id, const struct flash_area **area_outp)
{
    const struct flash_area *area = prv_lookup_flash_area(id);

    *area_outp = area;
    MCUBOOT_LOG_DBG("%s: ID=%d, %s", __func__, (int)id, area != NULL ? "OK" : "fail");
    return area != NULL ? 0 : -1;
}

void flash_area_close(const struct flash_area *area)
{
    // no cleanup to do for this flash part
}

int flash_area_read(const struct flash_area *fa, uint32_t off, void *dst, uint32_t len)
{
    if (fa->fa_device_id != FLASH_DEVICE_INTERNAL_FLASH) {
        return -1;
    }

    const uint32_t end_offset = off + len;

    if (end_offset > fa->fa_size) {
        MCUBOOT_LOG_ERR("%s: Out of Bounds (0x%lx vs 0x%lx)", __func__, end_offset, fa->fa_size);
        return -1;
    }

    void *addr = (void *)(fa->fa_off + off);

    MCUBOOT_LOG_DBG("%s: Addr: 0x%08x Length: %d", __func__, (int)addr, (int)len);
    memcpy(dst, addr, len);
    return 0;
}

int flash_area_write(const struct flash_area *fa, uint32_t off, const void *src, uint32_t len)
{
    if (fa->fa_device_id != FLASH_DEVICE_INTERNAL_FLASH) {
        return -1;
    }

    const uint32_t end_offset = off + len;

    if (end_offset > fa->fa_size) {
        MCUBOOT_LOG_ERR("%s: Out of Bounds (0x%lx vs 0x%lx)", __func__, end_offset, fa->fa_size);
        return -1;
    }

    const uint32_t addr = fa->fa_off + off;

    MCUBOOT_LOG_DBG("%s: Addr: 0x%08x Length: %d", __func__, (int)addr, (int)len);
    lgk_boot_flash_write((uint32_t)addr, (void *)src, len);
    return 0;
}

int flash_area_erase(const struct flash_area *fa, uint32_t off, uint32_t len)
{
    if (fa->fa_device_id != FLASH_DEVICE_INTERNAL_FLASH) {
        return -1;
    }

    const uint32_t start_addr = fa->fa_off + off;

    MCUBOOT_LOG_DBG("%s: Addr: 0x%08x Length: %d", __func__, (int)start_addr, (int)len);

    lgk_boot_flash_erase(start_addr, len);

    return 0;
}

uint32_t flash_area_align(const struct flash_area *area)
{
    // the smallest unit a flash write can occur along.
    // Note: Image trailers will be scaled by this size
    return 4;
}

uint8_t flash_area_erased_val(const struct flash_area *area)
{
    // the value a byte reads when erased on storage.
    //! the actual value is 0xa9bdf9f3
    return 0xff;
}

// int flash_area_get_sectors(int fa_id, uint32_t *count, struct flash_sector *sectors)
// {
//     const struct flash_area *fa = prv_lookup_flash_area(fa_id);

//     if (fa->fa_device_id != FLASH_DEVICE_INTERNAL_FLASH) {
//         return -1;
//     }

//     const uint32_t sector_size = FLASH_MIN_WR_SIZE;
//     uint32_t total_count = 0;

//     for (uint32_t off = 0; off < fa->fa_size; off += sector_size) {
//         // Note: Offset here is relative to flash area, not device
//         sectors[total_count].fs_off = off;
//         sectors[total_count].fs_size = sector_size;
//         total_count++;
//     }

//     *count = total_count;
//     return 0;
// }

int flash_area_id_from_multi_image_slot(int image_index, int slot)
{
    MCUBOOT_LOG_DBG("%s: image_index=%d, slot=%d", __func__, image_index, slot);

    switch (slot) {
        case 0:
            return FLASH_AREA_IMAGE_PRIMARY(image_index);
        case 1:
            return FLASH_AREA_IMAGE_SECONDARY(image_index);
        default:
            break;
    }

    MCUBOOT_LOG_ERR("Unexpected Request: image_index=%d, slot=%d", image_index, slot);
    return -1; /* flash_area_open will fail on that */
}

int flash_area_id_from_image_slot(int slot)
{
    return flash_area_id_from_multi_image_slot(0, slot);
}

int flash_area_to_sectors(int idx, int *cnt, struct flash_area *fa)
{
    MCUBOOT_LOG_DBG("%s: idx=%d, slot=%d", __func__, fa->fa_id);

    int rc = 0;

    if (fa->fa_device_id == FLASH_DEVICE_INTERNAL_FLASH) {
        (void)idx;
        (void)cnt;
        rc = 0;
    }

    return rc;
}

void mcuboot_assert_handler(const char *file, int line, const char *func)
{
    lgk_boot_log("assertion failed: file \"%s\", line %d, func: %s\n", file, line, func);
}