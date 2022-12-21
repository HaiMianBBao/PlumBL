#include "usbd_core.h"
#include "usbd_dfu.h"
#include "lgk_boot_core.h"

#define USBD_VID 0x0903
#define USBD_PID 0x0220
#define USBD_MAX_POWER 100
#define USBD_LANGID_STRING 1033

#define FLASH_DESC_STR "@Internal Flash   /0x00000000/16*004Ka,096*04Kg"

#define USB_CONFIG_SIZE (9 + 9 + 9)

#include "dfu_flash_desc.h"

void usbd_configure_done_callback(void)
{
    /* no out ep, do nothing */
}

struct usbd_interface intf0;
/* function ------------------------------------------------------------------*/
/**
 * @brief            msc ram init
 * @pre              none
 * @param[in]        none
 * @retval           none
 */
void dfu_flash_init(void)
{
    usbd_desc_register(dfu_flash_descriptor);
    usbd_add_interface(usbd_dfu_init_intf(&intf0));
    usbd_initialize();
}


