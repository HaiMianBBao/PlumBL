/*
 * @Description: usb协议栈核心的部分
 * @Author: LiGuo
 * @Email: 1570139720@qq.com
 */
#pragma once
#ifdef __cplusplus
extern "C"
{
#endif
/* 文件引用 ------------------------------------------------------------------*/
#include "stdint.h"
#include "stdbool.h"
#include "usb_config.h"
#include "lg_usb_util.h"
#include "lg_usb_def.h"
#include "lg_usb_dc.h"
#include "lg_usb_slist.h"
/* 枚举 ------------------------------------------------------------------*/
/**
 * @brief   枚举USB事件类型
 */
enum usb_event_type
{
    USB_EVENT_ERROR,              /*!< USB错误 */
    USB_EVENT_RESET,              /*!< USB复位 */
    USB_EVENT_SOF,                /*!< USB开始接收帧接收 */
    USB_EVENT_CONNECTED,          /*!< USB连接建立完成 硬件枚举已经完成 */
    USB_EVENT_CONFIGURED,         /*!< USB配置完成 */
    USB_EVENT_SUSPEND,            /*!< 主机暂停USB连接 */
    USB_EVENT_DISCONNECTED,       /*!< USB断开  */
    USB_EVENT_RESUME,             /*!< 主机恢复USB连接 */

    USB_EVENT_SET_INTERFACE,      /*!< 设置接口状态 */
    USB_EVENT_SET_REMOTE_WAKEUP,  /*!< 注释 */
    USB_EVENT_CLEAR_REMOTE_WAKEUP,/*!< 注释 */
    USB_EVENT_SET_HALT,           /*!< 注释 */
    USB_EVENT_CLEAR_HALT,         /*!< 注释 */
    USB_EVENT_SETUP_NOTIFY,       /*!< setup包接收到时产生的回调 */
    USB_EVENT_EP0_IN_NOTIFY,      /*!< 端点0  设备发送给主机数据时候产生的事件 */
    USB_EVENT_EP0_OUT_NOTIFY,     /*!< 端点0  主机发送给设备数据时候产生的的事件 */
    USB_EVENT_EP_IN_NOTIFY,       /*!< 端点   in产生的事件 */
    USB_EVENT_EP_OUT_NOTIFY,      /*!< 端点   out产生的事件 */
    USB_EVENT_UNKNOWN             /*!< 初始化USB连接状态 */
};


/**
 * @brief   枚举USB状态
 */
typedef enum
{
    USB_OK = 0U,
    USB_BUSY,
    USB_EMEM,
    USB_FAIL       
}usbd_status_typedef;

/**
 * @brief   枚举设备状态
 */
typedef enum
{
    UNCONNECTED = 0, /*!< 设备刚上电  还没开始复位时候的状态 */
    ATTACHED,        /*!< 设备复位后转为此状态 */
    POWERED,         /*!< 暂时没用到 */
    SUSPENDED,       /*!< 设备被挂起后转为此状态 */
    ADDRESSED,       /*!< 主机发送设置地址命令的时候 设备转为此状态 */
    CONFIGURED       /*!< 主机在发送set configuration  设备并且成功完成配置后转为此状态 */
}usbd_device_state;




/* 类型定义 ------------------------------------------------------------------*/
/*!< usb_request_handler 是一个函数指针 函数模板 可以用 usb_request_handler来声明一个数组 数组里面的元素就是函数指针*/
typedef int(*usbd_request_handler)(struct usb_setup_packet *setup,uint8_t **data,uint32_t *len);
/*!< 端点回调函数模板 */
typedef void(*usbd_endpoint_callback)(uint8_t ep);
/*!< usb事物处理函数模板 指示当前usb处于什么状态 */
typedef void(*usbd_notify_handler)(uint8_t event,void *arg);


/**
 * @brief   端点的数据结构
 */
typedef struct usbd_endpoint
{
    usb_slist_t list;              /*!< 链接所有的端点 */
    uint8_t addr;                  /*!< 端点地址 */
    usbd_endpoint_callback ep_cb;  /*!< 端点回调函数 */
}usbd_endpoint_t;

/**
 * @brief   接口的数据结构
 */
typedef struct usbd_interface
{
    usb_slist_t list;                      /*!< 链接所有的接口 */
    usbd_request_handler class_handler;    /*!< 类设备请求 */
    usbd_request_handler vendor_handler;   /*!< 制造商请求 */
    usbd_request_handler custom_handler;   /*!< 自带的一些请求  比如HID */
    usbd_notify_handler  notify_handler;   /*!< 可能有的状态的发送 */

    uint8_t intf_num;                      /*!< 当前接口所处偏移 */
    usb_slist_t ep_list;                   /*!< 一个接口可能有很多端点 这是链接所有的端点 */
}usbd_interface_t;

/**
 * @brief   usb class类的数据结构
 */
typedef struct usbd_class
{
    usb_slist_t list;        /*!< 链接所有的class */
    const char *name;        /*!< 类的名称 */
    usb_slist_t intf_list;   /*!< 链接所有的接口 */
}usbd_class_t;


/* 函数声明 ------------------------------------------------------------------*/
/**
 * @brief   这些函数有些是用户在初始化的时候需要用到的  有些是在类里面用到
 */
void usbd_class_register(usbd_class_t *class);
void usbd_class_add_interface(usbd_class_t *class,usbd_interface_t *interface);
void usbd_interface_add_endpoint(usbd_interface_t *interface,usbd_endpoint_t *ep);
void usbd_notify_callback(uint8_t event,void *arg);
void usbd_desc_register(const uint8_t *desc);
usbd_device_state get_device_state(void);
bool is_device_configured(void);

#ifdef __cplusplus
}
#endif
