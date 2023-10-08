const uint8_t dfu_flash_descriptor[] = {
    USB_DEVICE_DESCRIPTOR_INIT(USB_1_1, 0x00, 0x00, 0x00, USBD_VID, USBD_PID, 0x2200, 0x01),
    USB_CONFIG_DESCRIPTOR_INIT(USB_CONFIG_SIZE, 0x01, 0x01, USB_CONFIG_BUS_POWERED, USBD_MAX_POWER),
    DFU_DESCRIPTOR_INIT(),
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
    ///////////////////////////////////////
    /// string4 descriptor
    ///////////////////////////////////////
    0x60,                       /* bLength */
    USB_DESCRIPTOR_TYPE_STRING, /* bDescriptorType */
    '@', 0x00,                  /* wcChar0 */
    'I', 0x00,                  /* wcChar1 */
    'n', 0x00,                  /* wcChar2 */
    't', 0x00,                  /* wcChar3 */
    'e', 0x00,                  /* wcChar4 */
    'r', 0x00,                  /* wcChar5 */
    'n', 0x00,                  /* wcChar6 */
    'a', 0x00,                  /* wcChar7 */
    'l', 0x00,                  /* wcChar8 */
    ' ', 0x00,                  /* wcChar9 */
    'F', 0x00,                  /* wcChar10 */
    'l', 0x00,                  /* wcChar11*/
    'a', 0x00,                  /* wcChar12 */
    's', 0x00,                  /* wcChar13 */
    'h', 0x00,                  /* wcChar14 */
    ' ', 0x00,                  /* wcChar15 */
    ' ', 0x00,                  /* wcChar16 */
    ' ', 0x00,                  /* wcChar17 */
    '/', 0x00,                  /* wcChar18 */
    '0', 0x00,                  /* wcChar19 */
    'x', 0x00,                  /* wcChar20 */
    '0', 0x00,                  /* wcChar21*/
    '0', 0x00,                  /* wcChar22 */
    '0', 0x00,                  /* wcChar23 */
    '0', 0x00,                  /* wcChar24 */
    '0', 0x00,                  /* wcChar25 */
    '0', 0x00,                  /* wcChar26 */
    '0', 0x00,                  /* wcChar27 */
    '0', 0x00,                  /* wcChar28 */
    '/', 0x00,                  /* wcChar29 */
    '1', 0x00,                  /* wcChar30 */
    '6', 0x00,                  /* wcChar31*/
    '*', 0x00,                  /* wcChar32 */
    '0', 0x00,                  /* wcChar33 */
    '0', 0x00,                  /* wcChar34 */
    '4', 0x00,                  /* wcChar35 */
    'K', 0x00,                  /* wcChar36 */
    'a', 0x00,                  /* wcChar37 */
    ',', 0x00,                  /* wcChar38 */
    '0', 0x00,                  /* wcChar39 */
    '9', 0x00,                  /* wcChar40 */
    '6', 0x00,                  /* wcChar41*/
    '*', 0x00,                  /* wcChar42 */
    '0', 0x00,                  /* wcChar43 */
    '4', 0x00,                  /* wcChar44 */
    'K', 0x00,                  /* wcChar45 */
    'g', 0x00,                  /* wcChar46 */

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
    0x00};
