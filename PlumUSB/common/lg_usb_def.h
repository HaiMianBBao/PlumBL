/**
  ******************************************************************************
  * @file          lg_usb_def.h
  * @brief         usb dcd defines 
  * @author        Li Guo
  *                1570139720@qq.com
  * @version       1.0
  * @date          2022.01.25
  ******************************************************************************
  * @attention
  * 
  * <h2><center>&copy; Copyright 2021 Li Guo.
  * All rights reserved.</center></h2>
  * 
  * @htmlonly 
  * <span style='font-weight: bold'>History</span> 
  * @endhtmlonly
  * 版本|作者|时间|描述
  * ----|----|----|----
  * 1.0|Li Guo|2022.01.25|创建文件
  ******************************************************************************
  */
#pragma once

#ifdef __cplusplus
extern "C"{
#endif

/* include -------------------------------------------------------------------*/
#include "stdint.h"



/* marco ---------------------------------------------------------------------*/
#define USB_1_1   0x0110
#define USB_2_0   0x0200

#define USB_CONFIG_POWERED_MASK  0x40
#define USB_CONFIG_BUS_POWERED   0x80
//#define USB_CONFIG_SELF_POWERED  0xC0
#define USB_CONFIG_REMOTE_WAKEUP 0x20



#define USB_EP_DIR_MASK 0x80U
#define USB_EP_DIR_IN   0x80U
#define USB_EP_DIR_OUT  0x00U


/*!< Get endpoint index (number) from endpoint address */
#define USB_EP_GET_IDX(ep) ((ep) & ~USB_EP_DIR_MASK)
/*!< Get direction from endpoint address */
#define USB_EP_GET_DIR(ep) ((ep)&USB_EP_DIR_MASK)
/*!< Get endpoint address from endpoint index and direction */
#define USB_EP_GET_ADDR(idx, dir) ((idx) | ((dir)&USB_EP_DIR_MASK))
/*!< True if the endpoint is an IN endpoint */
#define USB_EP_DIR_IS_IN(ep) (USB_EP_GET_DIR(ep) == USB_EP_DIR_IN)
/*!< True if the endpoint is an OUT endpoint */
#define USB_EP_DIR_IS_OUT(ep) (USB_EP_GET_DIR(ep) == USB_EP_DIR_OUT)


/*!< USB 描述符类型 */
#define USB_DESCRIPTOR_TYPE_DEVICE                0x01u   /*!< 设备描述符 */
#define USB_DESCRIPTOR_TYPE_CONFIGURATION         0x02u   /*!< 配置描述符 */
#define USB_DESCRIPTOR_TYPE_STRING                0x03u   /*!< 字符串描述符 */
#define USB_DESCRIPTOR_TYPE_INTERFACE             0x04u   /*!< 接口描述符 */
#define USB_DESCRIPTOR_TYPE_ENDPOINT              0x05u   /*!< 端点描述符 */
#define USB_DESCRIPTOR_TYPE_DEVICE_QUALIFIER      0x06u   /*!< 设备限定描述符 */
#define USB_DESCRIPTOR_TYPE_OTHER_SPEED_CONFIG    0x07u   /*!< 其他速度配置描述符 */
#define USB_DESCRIPTOR_TYPE_INTERFACE_POWER       0x08u   /*!< 接口供电描述符 */

#define USB_DESCRIPTOR_TYPE_OTG                   0x09u   
#define USB_DESCRIPTOR_TYPE_DEBUG                 0x0Au   
#define USB_DESCRIPTOR_TYPE_INTERFACE_ASSOCIATION 0x0Bu   /*!< 接口关联描述符 */
#define USB_DESCRIPTOR_TYPE_BINARY_OBJECT_STORE   0x0Fu
#define USB_DESCRIPTOR_TYPE_DEVICE_CAPABILITY     0x10u

/*!< 描述符长度 */
#define USB_DESCRIPTOR_LENGTH_DEVICE              0x12u   /*!< 设备描述符长度 */
#define USB_DESCRIPTOR_LENGTH_CONFIGURATION       0x09u   /*!< 配置描述符长度 */
#define USB_DESCRIPTOR_LENGTH_INTERFACE           0x09u   /*!< 接口描述符长度 */
#define USB_DESCRIPTOR_LENGTH_ENDPOINT            0x07u   /*!< 端点描述符长度 */
#define USB_DESCRIPTOR_LENGTH_INTERFACE_ASSOC     0x08u   /*!< 接口关联描述符长度 */
#define USB_DESCRIPTOR_LENGTH_STRING              0x04u   /*!< 字符串描述符长度 */

/*!< 端点最大包长 */
#define USB_ENDPOINT_PACKET_MAX_LENGTH            0x40u   /*!< 端点最大包长 */

/*!< usb 字符串描述符标号 */
#define USB_STRING_LANGID_INDEX                   0x00    /*!< 没用到 */
#define USB_STRING_MFC_INDEX                      0x01    /*!< 制造商字符串描述符索引 */
#define USB_STRING_PRODUCT_INDEX                  0x02    /*!< 产品字符串描述符索引 */
#define USB_STRING_SERIAL_INDEX                   0x03    /*!< 产品序列号索引 */
#define USB_STRING_CONFIG_INDEX                   0x04    /*!< 没用到 */
#define USB_STRING_INTERFACE_INDEX                0x05    /*!< 没用到 */
#define USB_STRING_OS_INDEX                       0x06    /*!< 没用到 */
#define USB_STRING_MAX                            USB_STRING_OS_INDEX                         

/*!< bmRequestType.Type 请求类型 */
#define USB_REQUEST_STANDARD            0u                 /*!< 标准请求 */
#define USB_REQUEST_CLASS               1u                 /*!< 类设备请求 */
#define USB_REQUEST_VENDOR              2u                 /*!< 制造商请求 */
#define USB_REQUEST_RESERVED            3u                 /*!< 保留 */

/*!< bmRequestType.Dir 请求方向 */
#define USB_REQUEST_HOST_TO_DEVICE      0u                 /*!< 主机到设备 */
#define USB_REQUEST_DEVICE_TO_HOST      1u                 /*!< 设备到主机 */

/*!< bmRequestType Recipient 标准请求的接收者 */
#define USB_REQUEST_TO_DEVICE           0u                 /*!< 请求接收者为设备 */
#define USB_REQUEST_TO_INTERRFACE       1u                 /*!< 请求接收者为接口 */
#define USB_REQUEST_TO_ENDPOINT         2u                 /*!< 请求接收者为端点 */
#define USB_REQUEST_TO_OTHER            3u                 /*!< 请求接收者为其它 */

/*!< bRequest 标准请求代号 */
#define USB_REQUEST_GET_STATUS          0x00u              /*!<  */
#define USB_REQUEST_CLEAR_FEATURE       0x01u              /*!<  */

#define USB_REQUEST_SET_FEATURE         0x03u              /*!<  */

#define USB_REQUEST_SET_ADDRESS         0x05u              /*!<  */
#define USB_REQUEST_GET_DESCRIPTOR      0x06u              /*!<  */
#define USB_REQUEST_SET_DESCRIPTOR      0x07u              /*!<  */
#define USB_REQUEST_GET_CONFIGURATION   0x08u              /*!<  */
#define USB_REQUEST_SET_CONFIGURATION   0x09u              /*!<  */
#define USB_REQUEST_GET_INTERFACE       0x0Au              /*!<  */
#define USB_REQUEST_SET_INTERFACE       0x0Bu              /*!<  */
#define USB_REQUEST_SYNCH_FRAME         0x0Cu              /*!<  */

#define USB_REQUEST_SET_ENCRYPTION      0x0Du              /*!<  */
#define USB_REQUEST_GET_ENCRYPTION      0x0Eu              /*!<!  */
#define USB_REQUEST_RPIPE_ABORT         0x0Eu              /*!<!  */
#define USB_REQUEST_SET_HANDSHAKE       0x0Fu              /*!<!  */
#define USB_REQUEST_RPIPE_RESET         0x0Fu              /*!<!  */
#define USB_REQUEST_GET_HANDSHAKE       0x10u              /*!<  */
#define USB_REQUEST_SET_CONNECTION      0x11u              /*!<  */
#define USB_REQUEST_SET_SECURITY_DATA   0x12u              /*!<  */
#define USB_REQUEST_GET_SECURITY_DATA   0x13u              /*!<  */
#define USB_REQUEST_SET_WUSB_DATA       0x14u              /*!<  */
#define USB_REQUEST_LOOPBACK_DATA_WRITE 0x15u              /*!<  */
#define USB_REQUEST_LOOPBACK_DATA_READ  0x16u              /*!<  */
#define USB_REQUEST_SET_INTERFACE_DS    0x17u              /*!<  */

/*!< USB2.0标准功能选择器 */
#define USB_FEATURE_ENDPOINT_HALT        0x00u        /*!< 端点宕机 */
#define USB_FEATURE_SELF_POWERED         0x00u
#define USB_FEATURE_DEVICE_REMOTE_WAKEUP 0x01u        /*!< 远程唤醒 */
#define USB_FEATURE_TEST_MODE            0x02u        /*!<  */
#define USB_FEATURE_BATTERY              0x02u
#define USB_FEATURE_BHNPENABLE           0x03u
#define USB_FEATURE_WUSBDEVICE           0x03u
#define USB_FEATURE_AHNPSUPPORT          0x04u
#define USB_FEATURE_AALTHNPSUPPORT       0x05u
#define USB_FEATURE_DEBUGMODE            0x06u

/* USB Device Classes */
#define USB_DEVICE_CLASS_RESERVED      0x00
#define USB_DEVICE_CLASS_AUDIO         0x01
#define USB_DEVICE_CLASS_CDC           0x02
#define USB_DEVICE_CLASS_HID           0x03
#define USB_DEVICE_CLASS_MONITOR       0x04
#define USB_DEVICE_CLASS_PHYSICAL      0x05
#define USB_DEVICE_CLASS_IMAGE         0x06
#define USB_DEVICE_CLASS_PRINTER       0x07
#define USB_DEVICE_CLASS_MASS_STORAGE  0x08
#define USB_DEVICE_CLASS_HUB           0x09
#define USB_DEVICE_CLASS_CDC_DATA      0x0a
#define USB_DEVICE_CLASS_SMART_CARD    0x0b
#define USB_DEVICE_CLASS_SECURITY      0x0d
#define USB_DEVICE_CLASS_VIDEO         0x0e
#define USB_DEVICE_CLASS_HEALTHCARE    0x0f
#define USB_DEVICE_CLASS_DIAG_DEVICE   0xdc
#define USB_DEVICE_CLASS_WIRELESS      0xe0
#define USB_DEVICE_CLASS_MISC          0xef
#define USB_DEVICE_CLASS_APP_SPECIFIC  0xfe
#define USB_DEVICE_CLASS_VEND_SPECIFIC 0xff


    /**
     * @brief   USB setup包格式 用一个结构体打包
     */
    struct usb_setup_packet
    {
        __PACKED_UNION
        {
            uint8_t bmRequestType; /*!< 请求的特征 */
            struct
            {
                uint8_t Recipient : 5; /*!< 请求的对象：设备、接口、端点、其他  */
                uint8_t Type : 2;      /*!< 请求类型：标准请求、类请求、vendor请求 */
                uint8_t Dir : 1;       /*!< 请求方向 */
            } __PACKED bmRequestType_b;
        };
        uint8_t bRequest; /*!< 具体请求 */
        __PACKED_UNION
        {
            uint16_t wValue;
            struct
            {
                uint8_t wValueL;
                uint8_t wValueH;
            } __PACKED;
        };
        __PACKED_UNION
        {
            uint16_t wIndex;
            struct
            {
                uint8_t wIndexL;
                uint8_t wIndexH;
            } __PACKED;
        };
        uint16_t wLength; /*!< USB setup想要获取或者发送数据的长度 */
    } __PACKED;

    /**
     * @brief   标准设备描述符
     */
    struct usb_device_descriptor
    {
        uint8_t bLength;            /*!< 描述符长度   bytes = 18 */
        uint8_t bDescriptorType;    /*!< 描述符类型   type = 1 */
        uint16_t bcdUSB;            /*!< 于此设备描述符表兼容的USB版本号   0x0200 */
        uint8_t bDeviceClass;       /*!< 设备类码 */
        uint8_t bDeviceSubClass;    /*!< 设备子类 */
        uint8_t bDeviceProtocol;    /*!< 设备协议码 */
        uint8_t bMaxPacketSize0;    /*!< 端点0最大包大小 只能是 8 16 32 64*/
        uint16_t idVendor;          /*!< 厂商标志 */
        uint16_t idProduct;         /*!< 产品标志 */
        uint16_t bcdDevice;         /*!< 设备发行号 */
        uint8_t iManufacturer;      /*!< 描述厂商信息的字符串值 */
        uint8_t iProduct;           /*!< 描述产品信息的字符串值 */
        uint8_t iSeriaNumber;       /*!< 产品序列号 */
        uint8_t bNumConfigurations; /*!< 可能的配置数量 */
    } __PACKED;

    /**
     * @brief   标准配置描述符
     */
    struct usb_config_descriptor
    {
        uint8_t bLength;             /*!< 描述符长度   bytes = 9 */
        uint8_t bDescriptorType;     /*!< 描述符类型   type = 2 or 7 */
        uint16_t wTotalLength;       /*!< 此配置信息的总厂（包括配置、接口、端点、设备类及厂商定义的描述符） */
        uint8_t bNumInterfaces;      /*!< 此配置所支持的接口个数 */
        uint8_t bConfigurationValue; /*!< 在SetConfiguration 请求中用作参数来选定此配置 */
        uint8_t iConfiguration;      /*!< 配置描述符索引 */
        uint8_t bmAttributes;        /*!< 配置特性 */
        uint8_t bMaxPower;           /*!< 从总线上获取的最大电流 单位为2ma */
    } __PACKED;

    /**
     * @brief   标准接口描述符
     */
    struct usb_interface_descriptor
    {
        uint8_t bLength;            /*!< 描述符长度   bytes = 9 */
        uint8_t bDescriptorType;    /*!< 描述符类型   type = 4 */
        uint8_t bInterfaceNumber;   // TODO:/*!< 接口号 当前配置支持的接口数组索引 */
        uint8_t bAlternateSetting;  // TODO:/*!< 可选设置的索引值 */
        uint8_t bNumEndpoints;      /*!< 此接口的端点数量 */
        uint8_t bInterfaceClass;    /*!< 接口所属类值 如果这个值为0xFF 则此接口的类是由厂商说明 */
        uint8_t bInterfaceSubClass; /*!< 子类码 是根据bInterfaceClass来的 如果bInterfaceClass为0 那么这个值必须为0 另外这个东西还是要看情况而定 */
        uint8_t bInterfaceProtocol; /*!< 协议码：bInterfaceClass和bInterfaceSubClass而定 */
        uint8_t iInterface;         /*!< 接口字符串描述符索引 */
    } __PACKED;

    /**
     * @brief   标准端点描述符
     */
    struct usb_endpoint_descriptor
    {
        uint8_t bLength;         /*!< 描述度长度  bytes = 7 */
        uint8_t bDescriptorType; /*!< 描述符类型 */
        __PACKED_UNION
        {
            uint8_t bEndpointAddress; /*!< 端点地址 */
            struct
            {
                uint8_t eIndex : 4;  /*!< 端点号 */
                uint8_t reserve : 3; /*!< 保留 */
                uint8_t dir : 1;     /*!< 端点传输方向 */
            } __PACKED bEndpointAddress_b;
        };
        __PACKED_UNION
        {
            uint8_t bmAttributes; /*!< 端点特性 */
            struct
            {
                uint8_t transType : 2; /*!< 端点传输类型 */
                uint8_t reserve : 6;   /*!< 其他位保留 */
            } __PACKED bmAttributes_b;
        };
        uint16_t wMaxPacketSize; /*!< 当前配置下此端点接收或发送的最大包的大小 */
        uint8_t bInterval;       /*!< 轮询间隔 */
    } __PACKED;

    /**
     * @brief   字符串描述符
     */
    struct usb_string_descriptor
    {
        uint8_t bLength;         /*!< 描述符长度   bytes = n+2 */
        uint8_t bDescriptorType; /*!< 描述符类型   type = 3 */
        uint16_t bString;        /*!< 字符串  */
        /*!<
        4\uint16_t bString1;
        6\uint16_t bString2;
        ...
        n\uint16_t bStringn;
         */
    } __PACKED;

    /**
     * @brief   接口关联描述符
     */
    struct usb_interface_association_descriptor
    {
        uint8_t bLength;           /*!< 描述符长度   bytes = 8 */
        uint8_t bDescriptorType;   /*!< 描述符类型   type = 0x0b */
        uint8_t bFirstInterface;   /*!< 标号与功能相关的第一个接口 */
        uint8_t bInterfaceCount;   /*!< 与功能相关的邻近接口的数量 */
        uint8_t bFunctionClass;    /*!< 类别代码 */
        uint8_t bFunctionSubClass; /*!< 子类别代码 */
        uint8_t bFunctionProtocol; /*!< 协议代码 */
        uint8_t iFunction;         /*!< 功能字符串索引 */
    } __PACKED;

    /**
     * @brief   USB描述符头
     */
    struct usb_desc_header
    {
        uint8_t bLength;         /*!< 描述符长度 */
        uint8_t bDescriptorType; /**< 描述符类型 */
    };

/* 宏定义 ------------------------------------------------------------------*/
/**
 * @brief   初始化设备描述符  用宏来完成
 */
#define USB_DEVICE_DESCRIPTOR_INIT(bcdUSB, bDeviceClass, bDeviceSubClass, bDeviceProtocol, idVendor, idProduct, bcdDevice, bNumConfigurations) \
    USB_DESCRIPTOR_LENGTH_DEVICE,       /*!< 描述符长度 */                                                                                \
        USB_DESCRIPTOR_TYPE_DEVICE,     /*!< 描述符类型 */                                                                                \
        WBVAL(bcdUSB),                  /*!< bcdUSB */                                                                                         \
        bDeviceClass,                   /*!< 设备类码 */                                                                                   \
        bDeviceSubClass,                /*!< 设备子类 */                                                                                   \
        bDeviceProtocol,                /*!< 设备协议码 */                                                                                \
        USB_ENDPOINT_PACKET_MAX_LENGTH, /*!< 端点0最大包大小 只能是 8 16 32 64*/                                                     \
        WBVAL(idVendor),                /*!< 厂商标志 */                                                                                   \
        WBVAL(idProduct),               /*!< 产品标志 */                                                                                   \
        WBVAL(bcdDevice),               /*!< 设备发行号 */                                                                                \
        USB_STRING_MFC_INDEX,           /*!< 描述厂商信息的字符串值 */                                                              \
        USB_STRING_PRODUCT_INDEX,       /*!< 描述产品信息的字符串值 */                                                              \
        USB_STRING_SERIAL_INDEX,        /*!< 产品序列号 */                                                                                \
        bNumConfigurations              /*!< 可能的配置数量 */

/**
 * @brief   初始化配置描述符  用宏来完成
 */
#define USB_CONFIG_DESCRIPTOR_INIT(wTotalLength, bNumInterfaces, bConfigurationValue, bmAttributes, bMaxPower)                                         \
    USB_DESCRIPTOR_LENGTH_CONFIGURATION,   /*!< 描述符长度 */                                                                                     \
        USB_DESCRIPTOR_TYPE_CONFIGURATION, /*!< 描述符类型 */                                                                                     \
        WBVAL(wTotalLength),               /*!< 此配置信息的总厂（包括配置、接口、端点、设备类及厂商定义的描述符） */ \
        bNumInterfaces,                    /*!< 此配置所支持的接口个数 */                                                                   \
        bConfigurationValue,               /*!< 在SetConfiguration 请求中用作参数来选定此配置 */                                         \
        0x00,                              /*!< 配置描述符索引 */                                                                               \
        bmAttributes,                      /*!< 配置特性 */                                                                                        \
        bMaxPower                          /*!< 从总线上获取的最大电流 单位为2ma */

/**
 * @brief   初始化接口描述符  用宏来完成
 */
#define USB_INTERFACE_DESCRIPTOR_INIT(bInterfaceNumber, bAlternateSetting, bNumEndpoints, bInterfaceClass, bInterfaceSubClass, bInterfaceProtocol, iInterface) \
    USB_DESCRIPTOR_LENGTH_INTERFACE,   /*!< 描述符长度 */                                                                                                 \
        USB_DESCRIPTOR_TYPE_INTERFACE, /*!< 描述符类型 */                                                                                                 \
        bInterfaceNumber,              /*!< 接口号 当前配置支持的接口数组索引 */                                                               \
        bAlternateSetting,             /*!< 可选设置的索引值 */                                                                                        \
        bNumEndpoints,                 /*!< 此接口的端点数量 */                                                                                        \
        bInterfaceClass,               /*!< 接口所属类值 如果这个值为0xFF 则此接口的类是由厂商说明 */                                  \
        bInterfaceSubClass,            /*!< 子类码 */                                                                                                       \
        bInterfaceProtocol,            /*!< 协议码：bInterfaceClass和bInterfaceSubClass而定 */                                                          \
        iInterface                     /*!< 接口字符串描述符索引 */

/**
 * @brief   初始化端点描述符  用宏来完成
 */
#define USB_ENDPOINT_DESCRIPTOR_INIT(bEndpointAddress, bmAttributes, wMaxPacketSize, bInterval)            \
    USB_DESCRIPTOR_LENGTH_ENDPOINT,   /*!< 描述符长度 */                                              \
        USB_DESCRIPTOR_TYPE_ENDPOINT, /*!< 描述符类型 */                                              \
        bEndpointAddress,             /*!< 端点地址 */                                                 \
        bmAttributes,                 /*!< 端点特性 */                                                 \
        WBVAL(wMaxPacketSize),        /*!< 当前配置下此端点接收或发送的最大包的大小 */ \
        bInterval                     /*!< 轮询间隔 */

/**
 * @brief   初始化字符串描述符  用宏来完成
 */
#define USB_LANGID_INIT(id)                                 \
    USB_DESCRIPTOR_LENGTH_STRING,   /*!< 描述符长度 */ \
        USB_DESCRIPTOR_TYPE_STRING, /*!< 描述符类型 */ \
        WBVAL(id)                   /*!< 字符串值 */

#ifdef __cplusplus
}
#endif


/************************ (C) COPYRIGHT 2021 Li Guo *****END OF FILE*************/