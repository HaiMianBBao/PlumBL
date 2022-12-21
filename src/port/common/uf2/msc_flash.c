#include "usbd_core.h"
#include "usbd_msc.h"
#include "lgk_boot_core.h"

#define MSC_IN_EP  0x81
#define MSC_OUT_EP 0x02

#define USBD_VID 0x239A
#define USBD_PID 0x005D

#define USBD_MAX_POWER     100
#define USBD_LANGID_STRING 1033

#define USB_CONFIG_SIZE (9 + MSC_DESCRIPTOR_LEN)

const uint8_t msc_flash_descriptor[] = {
    USB_DEVICE_DESCRIPTOR_INIT(USB_2_0, 0x00, 0x00, 0x00, USBD_VID, USBD_PID, 0x0200, 0x01),
    USB_CONFIG_DESCRIPTOR_INIT(USB_CONFIG_SIZE, 0x01, 0x01, USB_CONFIG_BUS_POWERED, USBD_MAX_POWER),
    MSC_DESCRIPTOR_INIT(0x00, MSC_OUT_EP, MSC_IN_EP, 0x02),
    ///////////////////////////////////////
    /// string0 descriptor
    ///////////////////////////////////////
    USB_LANGID_INIT(USBD_LANGID_STRING),
    ///////////////////////////////////////
    /// string1 descriptor
    ///////////////////////////////////////
    0x14,                       /* bLength */
    USB_DESCRIPTOR_TYPE_STRING, /* bDescriptorType */
    'L', 0x00,                  /* wcChar0 */
    'G', 0x00,                  /* wcChar1 */
    ' ', 0x00,                  /* wcChar2 */
    'S', 0x00,                  /* wcChar3 */
    't', 0x00,                  /* wcChar4 */
    'u', 0x00,                  /* wcChar5 */
    'd', 0x00,                  /* wcChar6 */
    'i', 0x00,                  /* wcChar7 */
    'o', 0x00,                  /* wcChar8 */
    ///////////////////////////////////////
    /// string2 descriptor
    ///////////////////////////////////////
    0x1e,                       /* bLength */
    USB_DESCRIPTOR_TYPE_STRING, /* bDescriptorType */
    'P', 0x00,                  /* wcChar0 */
    'l', 0x00,                  /* wcChar1 */
    'u', 0x00,                  /* wcChar2 */
    'm', 0x00,                  /* wcChar3 */
    'B', 0x00,                  /* wcChar4 */
    'o', 0x00,                  /* wcChar5 */
    'o', 0x00,                  /* wcChar6 */
    't', 0x00,                  /* wcChar7 */
    'l', 0x00,                  /* wcChar8 */
    'o', 0x00,                  /* wcChar9 */
    'a', 0x00,                  /* wcChar10 */
    'd', 0x00,                  /* wcChar11 */
    'e', 0x00,                  /* wcChar12 */
    'r', 0x00,                  /* wcChar13 */
    ///////////////////////////////////////
    /// string3 descriptor
    ///////////////////////////////////////
    0x16,                       /* bLength */
    USB_DESCRIPTOR_TYPE_STRING, /* bDescriptorType */
    '2', 0x00,                  /* wcChar0 */
    '0', 0x00,                  /* wcChar1 */
    '2', 0x00,                  /* wcChar2 */
    '2', 0x00,                  /* wcChar3 */
    '1', 0x00,                  /* wcChar4 */
    '2', 0x00,                  /* wcChar5 */
    '3', 0x00,                  /* wcChar6 */
    '4', 0x00,                  /* wcChar7 */
    '5', 0x00,                  /* wcChar8 */
    '6', 0x00,                  /* wcChar9 */
#ifdef CONFIG_USB_HS
    ///////////////////////////////////////
    /// device qualifier descriptor
    ///////////////////////////////////////
    0x0a,
    USB_DESCRIPTOR_TYPE_DEVICE_QUALIFIER,
    0x00,
    0x02,
    0x00,
    0x00,
    0x00,
    0x40,
    0x01,
    0x00,
#endif
    0x00
};

void usbd_configure_done_callback(void)
{
    /* do nothing */
}

#define BLOCK_SIZE  512
#define BLOCK_COUNT 0x10109

#include "uf2.h"
static WriteState _wr_state = { 0 };

void usbd_msc_get_cap(uint8_t lun, uint32_t *block_num, uint16_t *block_size)
{
    *block_num = BLOCK_COUNT; // Pretend having so many buffer,not has actually.
    *block_size = BLOCK_SIZE;
}

int usbd_msc_sector_read(uint32_t sector, uint8_t *buffer, uint32_t length)
{
    if (sector < BLOCK_COUNT) {
        uf2_read_block(sector, buffer);
    }
    return 0;
}

int usbd_msc_sector_write(uint32_t sector, uint8_t *buffer, uint32_t length)
{
    static uint8_t state;
    (void)state;
    if (sector < BLOCK_COUNT) {
        /**
         * In the uf2_write_block function, if the uF2 file with this board ID is recognized,
         * the app area will be erased
         */
        state = uf2_write_block(0, buffer, &_wr_state);
        lgk_boot_log("%d \r\n", state);
    }

    if (_wr_state.numBlocks) {
        /*!< Burning firmware */
        extern volatile bool is_flashing_flag;
        extern volatile bool flashing_start_flag;
        flashing_start_flag = 1;
        is_flashing_flag = !is_flashing_flag;
        if (_wr_state.numWritten >= _wr_state.numBlocks) {
            /*!< Do nothing */
        }
    }
    return 0;
}

struct usbd_interface intf0;

/* function ------------------------------------------------------------------*/
/**
 * @brief            msc ram init
 * @pre              none
 * @param[in]        none
 * @retval           none
 */
void msc_flash_init(void)
{
    usbd_desc_register(msc_flash_descriptor);

    usbd_add_interface(usbd_msc_init_intf(&intf0, MSC_OUT_EP, MSC_IN_EP));
    usbd_initialize();
}
