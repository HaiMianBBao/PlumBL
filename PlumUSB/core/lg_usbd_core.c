/**
  ******************************************************************************
  * @file          lg_usbd_core.c
  * @brief         usbd core api
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
/* include -------------------------------------------------------------------*/
#include "lg_usbd_core.h"

/**
 * @brief   USB setup 请求框架
 */

/*!< Type类型有三种  接受者也有三种  3*3  9个函数  实际不需要9个函数*/
/*!< Type
    0==Standard
    1==Class
    2==Vendor
 */

/*!< Recipient
    0==Device
    1==Interface
    2==Endpoint
    3==Other
*/
#ifndef USB_REQUEST_BUFFER_SIZE
#define USB_REQUEST_BUFFER_SIZE 256      /*!< usb请求的最大包长限制 */
#endif
#define USB_EP_OUT_NUM 8                 /*!< OUI端点数目8个 */
#define USB_EP_IN_NUM 8                  /*!< IN端点数目8个 */
#define USBD_EP_CALLBACK_SEARCH_METHOD 1 /*!< 用数组的方法去存储端点回调函数 */
/* 变量定义 ------------------------------------------------------------------*/
/*!< 这里就是定义了一个结构体  要区分typedef 声明在结构体最后出现的名称 那是给结构体变量起了一个别名 */
static struct usbd_core_cfg_priv
{
    struct usb_setup_packet setup;             /*!< Setup 包 */
    uint8_t *ep0_data_buf;                     /*!< 端点0数据缓冲区的指针 */
    uint32_t ep0_data_buf_residue;             /*!< ep0 buffer中的剩余字节 */
    uint32_t ep0_data_buf_len;                 /*!< 总的实际发送长度 */
    bool zlp_flag;                             /*!< 0长度数据包 发送数据包要以短包或者0包结束 主机才知道设备硬件发送完成//这个0长度数据包针对的是发送的数据个数是64的倍数才会用到 */
    const uint8_t *descriptors;                /*!< 指向注册过的描述符 是一个设备的所有描述符  设备 配置 接口 类描述 端点 字符串 */
    uint8_t req_data[USB_REQUEST_BUFFER_SIZE]; /*!< Buffer used for storing standard, class and vendor request data */

#if USBD_EP_CALLBACK_SEARCH_METHOD == 1
    usbd_endpoint_callback in_ep_cb[USB_EP_IN_NUM];   /*!< in 端点回调函数 */
    usbd_endpoint_callback out_ep_cb[USB_EP_OUT_NUM]; /*!< out 端点回调函数 */
#endif
    bool enabled;           /*!< 用来检测usb是否启用的变量 */
    bool configured;        /*!< 存放枚举是否完成 configured==true枚举完成 */
    uint8_t configuration;  /*!< 存放SET_CONFIGURATION 时候wValue的值 知道是当前是配置哪个配置 */
    uint16_t remote_wakeup; /*!< Remote wakeup feature status */
    uint8_t reserved;
    uint8_t device_state;
} usbd_core_cfg;

static usb_slist_t usbd_class_head = USB_SLIST_OBJECT_INIT(usbd_class_head); /*!< 给class的链表分配一个头节点 这个头结点的初始值为0 */
uint8_t *mos_desc; /*!< mos描述符 */
uint8_t *bos_desc; /*!< bos描述符 */

/* Function ------------------------------------------------------------------*/
/**
 * @brief            Check whether the current device is in the configuration state
 * @pre              None
 * @param[in]        None
 * @retval           True is configured, otherwise it is not
 */
bool is_device_configured(void)
{
    return (usbd_core_cfg.configuration != 0);
}

/**
 * @brief            Gets the status of the current device
 * @pre              None
 * @param[in]        None
 * @retval           usbd_device_state  USB status value
 */
usbd_device_state get_device_state(void)
{
    return usbd_core_cfg.device_state;
}
/**
  * @brief            Set device status (internal use of protocol stack)
  * @pre              None
  * @param[in]        state ：Status value to be set
  * @retval           None
  */
static void set_device_state(usbd_device_state state)
{
    usbd_core_cfg.device_state = state;
}
/**
 * @brief            Check whether the given digital interface is valid
 * @pre              None
 * @param[in]        interface Interface number
 * @retval           True is valid, otherwise it is invalid
 */
static bool is_interface_valid(uint8_t interface)
{
    /*!< 在描述符中搜寻配置描述符 */
    // void *p = usbd_core_cfg.descriptors;     /*!< 定义一个指针指向总的描述符 */
    const uint8_t *p = usbd_core_cfg.descriptors; /*!< 定义一个指针指向总的描述符 */
    struct usb_config_descriptor *cfg_descr;      /*!< 定义一个配置描述符的指针用来访问配置描述符里面的东西 */
    while (p[0])
    {
        if (p[1] == 0x02)
        {
            /*!< 找到配置描述符 */
            /*!< 看接口号是不是小于配置描述符中的接口数量 */
            /*!< 先将p强制转换为配置描述符的指针类型 赋值给cfg_descr*/
            cfg_descr = (struct usb_config_descriptor *)p;
            if (interface < cfg_descr->bNumInterfaces)
            {
                return true;
            }
        }
        p += p[0]; /*!< p偏移当前描述符的长度 */
    }
    return false;
}
/**
 * @brief            Check whether the endpoint is valid
 * @pre              None
 * @param[in]        ep endpoint address
 * @retval           True is valid, otherwise it is invalid
 */
static bool is_ep_valid(uint8_t ep)
{
    /*!< 只要检查是否是端点0即可 */
    if ((ep & 0x7f) == 0)
    {
        /*!< 是端点0 则直接返回错误 */
        return false;
    }
    return true;
}
/**
 * @brief            Register endpoint callback function
 * @pre              None
 * @param[in]        None
 * @retval           None
 */
static void usbd_ep_callback_register(void)
{
    /*!< 列出所有的接口 */
    /*!< 靠setup中的wIndex的低字节来判断是哪一个接口 */
    usb_slist_t *i_ptr; /*!< 用来搜索整个Class链表 */
    usb_slist_t *j_ptr; /*!< 用来搜索每个类下面的接口 */
    usb_slist_t *k_ptr; /*!< 用来搜索每个接口下面的端点 */
    usb_slist_for_each(i_ptr, &usbd_class_head)
    {
        usbd_class_t *class_ptr = usb_slist_entry(i_ptr, usbd_class_t, list);
        usb_slist_for_each(j_ptr, &(class_ptr->intf_list))
        {
            usbd_interface_t *intf_ptr = usb_slist_entry(j_ptr, usbd_interface_t, list);
            usb_slist_for_each(k_ptr, &(intf_ptr->ep_list))
            {
                usbd_endpoint_t *ep_ptr = usb_slist_entry(k_ptr, usbd_endpoint_t, list);
                if (ep_ptr->ep_cb)
                {
                    if (ep_ptr->addr & 0x80) /*!< 判断端点的方向 */
                    {
                        /*!< IN */
                        usbd_core_cfg.in_ep_cb[ep_ptr->addr & 0x7f] = ep_ptr->ep_cb;
                    }
                    else
                    {
                        /*!< OUT */
                        usbd_core_cfg.out_ep_cb[ep_ptr->addr & 0x7f] = ep_ptr->ep_cb;
                    }
                }
            }
        }
    }
}
/**
 * @brief            获取描述符
 * @pre              无
 * @param[in]        type_index  高8位是描述符的类型  低8位是代表描述符的索引
 * @param[in]        data        二级指针 用来存放描述符的数据
 * @param[in]        len         存放描述符的长度
 * @retval           true        获取成功  否则失败
 */
static bool usbd_get_descriptor(uint16_t type_index, uint8_t **data, uint32_t *len)
{
    bool found = false;
    uint8_t current_index = 0;
    uint8_t *p;                              /*!< 存放所有的描述符的 */
    uint8_t type = (type_index >> 8) & 0xff; /*!< 分离出wValue的高8位是代表的描述符的类型 */
    uint8_t index = (type_index)&0xff;       /*!< 分离出wValue的低8位是代表描述符的索引 */
    if (type == 3 && index == 0xee)
    {
        USB_LOG("Get MS OS 2.0 descriptor string \r\n");
        /*!< 获取微软MOS描述符 */
        if (mos_desc)
        {
            *data = mos_desc;
            *len = 100; /*!< TIP 假设放的100 */
            return true;
        }
        else
        {
            return false;
        }
    }
    else if (type == 15) /*!< bos描述符 */
    {
        USB_LOG("Get BOS descriptor string \r\n");
        /*!< 获取微软bos描述符 */
        if (bos_desc)
        {
            *data = bos_desc;
            *len = 100; /*!< TIP 假设放的100 */
            return true;
        }
        else
        {
            return false;
        }
    }

    /*!< 获取描述符是不可能获取到接口和端点描述符的 */
    /*!< 设备限定描述符在高速设备中需要使用 */
    if (type == 4 || type == 5 || type >= 7)
    {
        return false;
    }
    p = (uint8_t *)usbd_core_cfg.descriptors;
    /*!< 开始编写搜索方式 */
    /*!< 描述符的第一个字节是自身的长度
         第二个字节是描述符的类型
         所以可以根据此来写循环查找主机
         要获取的描述符 */
    while (p[0])
    {
        // if (p[1] == type)
        // {
        //     if (current_index == index)
        //     {
        //         found = true;
        //         break;
        //     }
        //     current_index++;
        // }
        if (p[1] == type)
        {
            if (current_index == index)
            {
                /*!< 找到对应的描述符了 */
                found = true;
                break;
            }
            current_index++;
        }
        /*!< 不是主机这次想要获取的 */
        /*!< 将指针调到下一个描述符的起始处 */
        p += p[0];
        //current_index++; // TODO:/*!< 这个index不是很明白 */ /*!< 解决：查资料知道 当设备中国实现了多个相同类型的描述符时 描述符索引用于选择特定的描述符 */
        /*!< 仅仅适用于（配置configuration和字符串string描述符）*/
    }
    /*!< 分析找到的描述符 */
    if (found)
    {
        *data = p; /*!< 需要注意的是  这个data是个二级指针 */
        /*!< 还要做个判断就是  看描述符是否是配置描述符  因为配置描述符
             的总长包括配置、接口、端点、和设备类及厂商的定义的描述符 */
        if (type == 2)
        {
            /*!< 配置描述符 */
            /*!< 获取配置描述符的时候 在配置描述符中的wTotailength是占两个字节的 这俩字节存放是配置 接口 端点 设备类及厂商定义的描述符的总长度 */
            *len = p[2] | (p[3] << 8); // TODO:/*!< 对描述符长度进行特殊处理 */ /*!< TIP 这里不是很明白  不明白这个长度为什么要这样计算 */
            /*!< 解决：因为wTotailength占用两个字节 p指针指向的数据是uint_8类型的 p[3]作为高位，p[2]作为低位 合成一个16位的长度 */
        }
        else
        {
            *len = p[0]; /*!< 不是配置描述符  描述符的长度就是描述符的第一个字节 */
        }
    }
    else
    {
        /*!< 没有找到描述符 */
        USB_LOG("Err:descriptor <type:%x,index:%x> not found! \r\n", type, index);
    }
    return found;
}
/**
 * @brief            设置端点(不能是端点0  端点0在 usb设备reset的时候进行配置)
 * @pre              无
 * @param[in]        ep_desc 端点描述符数据
 * @retval           true 配置成功 否则失败
 */
static bool usbd_set_endpoint(struct usb_endpoint_descriptor *ep_desc)
{
    if (!is_ep_valid(ep_desc->bEndpointAddress_b.eIndex))
    {
        return false;
    }
    struct usbd_endpoint_cfg ep_cfg;
    ep_cfg.ep_addr = ep_desc->bEndpointAddress;                  /*!< 从端点描述符里面拿出端点地址 */
    ep_cfg.ep_mps = ep_desc->wMaxPacketSize;                     /*!< 从端点描述符里面拿出端点最大包长 */
    ep_cfg.ep_transfer_type = ep_desc->bmAttributes_b.transType; /*!< 从端点描述符里面拿出端点传输类型 可以使用遮罩来只取最低的两位 也可以使用位域*/

    USB_LOG("Open endpoint:  addr:%x  type:%d  maxpacket:%d \r\n", ep_cfg.ep_addr, ep_cfg.ep_transfer_type, ep_cfg.ep_mps);

    usbd_ep_open(&ep_cfg);           /*!< 打开端点  这个open函数里面还可以配置dma fifo等等*/
    usbd_core_cfg.configured = true; /*!< 枚举完成 */
    return true;
}
/**
 * @brief            关闭端点
 * @pre              无
 * @param[in]        ep_desc 端点描述符数据
 * @retval           true 关闭成功 否则失败
 */
static bool usbd_reset_endpoint(struct usb_endpoint_descriptor *ep_desc)
{
    USB_LOG("Close endpoint:  addr:%x  type:%d  maxpacket:%d \r\n", ep_desc->bEndpointAddress, ep_desc->bmAttributes_b.transType, ep_desc->wMaxPacketSize);

    usbd_ep_close(ep_desc->bEndpointAddress);
    return true;
}
/**
 * @brief            配置接口
 * @pre              无
 * @param[in]        intf_num     接口号
 * @param[in]        alt_setting  一个接口可以有多个描述符 此参数就是来接口的对应描述符
 * @retval           true 配置成功 否则失败
 */
static bool usbd_set_interface(uint16_t intf_num, uint16_t alt_setting)
{
    bool found = true;
    uint8_t *p = (uint8_t *)usbd_core_cfg.descriptors; /*!< 记录了配置描述符的指针 */
    uint8_t *intf_desc = NULL;                         /*!< 记录接口描述符的首地址 */
    uint32_t current_intf_num = 0;                     /*!< 记录当前的接口号 */
    uint32_t current_alt_setting = 0;                  /*!< 记录当前的alt_setting */

    USB_LOG("Set interface:  intf_num:%d  alt_setting:%d \r\n", intf_num, alt_setting);

    while (p[0])
    {

        switch (p[1])
        {
        case 0x04:                      /*!< 搜索接口描述符 */
            current_intf_num = p[2];    /*!< 记录接口号 */
            current_alt_setting = p[3]; /*!< 记录alternate setting */
            if (current_intf_num == intf_num && current_alt_setting == alt_setting)
            {
                intf_desc = p; /*!< 记录接口描述符的首地址 */
            }
            USB_LOG("Current interface:  intf_num:%d  alt_setting:%d \r\n", current_intf_num, current_alt_setting);
            break;
        case 0x05: /*!< 搜索端点描述符 */
            if (current_intf_num == intf_num)
            {
                if (current_alt_setting == alt_setting)
                {
                    usbd_set_endpoint((struct usb_endpoint_descriptor *)p);
                }
                else
                {
                    usbd_reset_endpoint((struct usb_endpoint_descriptor *)p);
                }
            }
            else
            {
                found = false;
            }
        default:
            break;
        }
        p += p[0];
    }
    /*!< call usbd_notify_callback 来反应当前状态 */
    usbd_notify_callback(USB_EVENT_SET_INTERFACE, (void *)intf_desc);
    return found;
}
/**
 * @brief             SET_CONFIGURATION  主要就是对端点的一些属性进行配置
 * @pre               无
 * @param[in]         index：当前配置的索引
 * @param[in]         alt_setting：需要设置的alt_setting
 * @retval            true 成功 否则失败
 */
static bool usbd_set_configuration(uint16_t index, uint16_t alt_setting)
{
    uint8_t *p = (uint8_t *)usbd_core_cfg.descriptors; /*!< 记录了配置描述符的指针 */
    uint32_t current_index = 0;                        /*!< 记录当描述符的index */
    uint32_t current_alt_setting = 0;                  /*!< 记录当前的alt_setting */
    struct usb_endpoint_descriptor *ep_desc;           /*!< 记录端点描述符的指针 */
    if (index == 0)
    {
        /*!< 没有配置设备 */
        USB_LOG("Device not configured ! -- invalid configuration \r\n");
    }
    bool found = false;
    /*!< 开始找描述符
         根据配置描述符找到接口描述符
         根据接口描述符找到端点描述符 */
    while (p[0])
    {
        /*!< p[1]里面存放的是描述符的类型 */
        switch (p[1])
        {
        case 0x02:
            /*!< 配置描述符 */
            current_index = p[5]; /*!< 记录当前的index */
            if (current_index == index)
            {
                found = true;
            }
            break;
        case 0x04:
            /*!< 接口描述符 */
            /*!< 接口描述符里面有个altsetting p[3]  默认情况下是0 只有在使用其他的类  比如像video类和audio类的时候 这个altsetting会变 */
            current_alt_setting = p[3]; /*!< 记录当前的altsetting */
            break;
        case 0x05:
            /*!< 端点描述符 */
            if (current_index != index || current_alt_setting != alt_setting)
            {
                break;
            }
            /*!< 如果上述if中的语句不成立  说明已经找到端点描述符了 */
            ep_desc = (struct usb_endpoint_descriptor *)p; /*!< 将当前指向的指针赋值给存放端点描述符首地址的指针ep_desc 需要用到一次强制转换 */
            found = usbd_set_endpoint(ep_desc);            /*!< 根据端点描述符来配置端点 */
            break;
        default:
            break;
        }
        p += p[0]; /*!< 跳到下一个描述符 */
    }
    return found;
}
/**
 * @brief            Standard 标准设备请求
 * @pre              无
 * @param[in]        setup
 * @param[in]        data
 * @param[in]        len
 * @retval           true 成功 否则失败
 */
static bool usbd_standard_device_request_handler(struct usb_setup_packet *setup, uint8_t **data, uint32_t *len)
{
    // uint16_t value = setup->wValue;
    bool ret = true;
    switch (setup->bRequest)
    {
    case USB_REQUEST_GET_STATUS:
        USB_LOG("REQ_GET_STATUS \r\n");
        /*!< GET STATUS */
        /* bit 0: self-powered */
        /* bit 1: remote wakeup */
        /*!< 要求我们获取两个字节返回给主机 主要内容就是设备的自供电 远程唤醒 */
        *data = (uint8_t *)&usbd_core_cfg.remote_wakeup;
        *len = 2;
    case USB_REQUEST_CLEAR_FEATURE:
        /*!< CLEAR_FEATURE */
        USB_LOG("REQ_CLEAR_FEATURE \r\n");
        ret = false;

        if (setup->wValue == 1)
        {
            /*!< 对应USB设备的远程唤醒功能 */
            usbd_core_cfg.remote_wakeup = 0;
            usbd_notify_callback(USB_EVENT_CLEAR_REMOTE_WAKEUP, NULL);
            ret = true;
        }
        // if (setup->wValue == 2)
        // {
        //     /*!< 用于对USB的测试模式  其不能通过USB的ClearFeature来消除或禁止 */
        // }
        break;
    case USB_REQUEST_SET_FEATURE:
        /*!< SET FEATURE */
        USB_LOG("REQ_SET_FEATURE \r\n");
        ret = false;

        if (setup->wValue == 1)
        {
            usbd_core_cfg.remote_wakeup = 1;
            usbd_notify_callback(USB_EVENT_SET_REMOTE_WAKEUP, NULL);
            ret = true;
        }
        if (setup->wValue == USB_FEATURE_TEST_MODE)
        {
            /* put TEST_MODE code here */
        }
        break;
    case USB_REQUEST_SET_ADDRESS:
        /*!< 设置设备地址  SET_ADDRESS */
        USB_LOG("REQ_SET_ADDRESS addr: 0x%x \r\n", setup->wValue);
        usbd_set_address(setup->wValue & 0xff);
        usbd_core_cfg.device_state = ADDRESSED; /*!< 主机发设置地址后  设备状态转为地址状态 */
        break;
    case USB_REQUEST_GET_DESCRIPTOR:
        /*!< 获取描述符 */
        USB_LOG("REQ_GET_DESCRIPTOR \r\n");
        if ((setup->wValueH) == USB_DESCRIPTOR_TYPE_DEVICE_QUALIFIER)
        {
            /*!< 全速设备没有设备限定描述符 */
            usbd_ep_set_stall(USB_CTRL_IN_EP0);
            return false;
        }
        ret = usbd_get_descriptor(setup->wValue, data, len);
        break;
    case USB_REQUEST_SET_CONFIGURATION:
        /*!< SET_CONFIGURATION */
        USB_LOG("REQ_SET_CONFIGURATION \r\n");
        if (!usbd_set_configuration(setup->wValue, 0)) /*!< TIP usbd_set_configuration这个函数等待实现  */
        {
            /*!< 报错 */
            USB_LOG("Err:USB Set Configuration failed \r\n");
            ret = false;
        }
        else
        {
            // TODO: /*!< 这里我的理解就是  枚举了一个配置 并把此配置的标号给到configuration 让协议栈可以在其它地方获得正在配置的的配置标号 */
            /*!< 配置成功  并且更新当前配置 */
            usbd_core_cfg.configuration = setup->wValue;
            usbd_notify_callback(USB_EVENT_CONFIGURED, NULL); /*!< call  usbd_notify_callback 来反映当前协议栈的状态*/
        }
        break;
    case USB_REQUEST_GET_CONFIGURATION:
        /*!< GET_CONFIGURATION */
        USB_LOG("REQ_GET_CONFIGURATION \r\n");
        *data = (uint8_t *)&usbd_core_cfg.configuration;
        *len = 1;
        break;
    case USB_REQUEST_GET_INTERFACE:
        /*!< GET_INTERRFACE */
        USB_LOG("REQ_GET_INTERRFACE \r\n");
        break;
    case USB_REQUEST_SET_INTERFACE:
        /*!< SET_INTERRFACE */
        USB_LOG("REQ_SET_INTERRFACE \r\n");
        break;
    default:
        USB_LOG("Err:Illegal device request 0x%02x \r\n", setup->bRequest);
        ret = false;
        break;
    }
    return ret;
}
/**
 * @brief            标准接口请求
 * @pre              无
 * @param[in]        setup
 * @param[in]        data
 * @param[in]        len
 * @retval           true 成功 否则失败
 */
static bool usbd_standard_interface_request_handler(struct usb_setup_packet *setup, uint8_t **data, uint32_t *len)
{
    bool ret = true;
    if (!is_device_configured() || !is_interface_valid(setup->wIndex))
    {
        if (!is_device_configured())
        {
            USB_LOG("Err:The device is not configured \r\n");
        }
        else
        {
            USB_LOG("Err:Invalid interface number \r\n");
        }
        return false;
    }
    switch (setup->bRequest)
    {
    case USB_REQUEST_GET_STATUS:
        /*!< GET_STATUS */
        /*!< 要求我们获取两个字节返回给主机 主要内容就是设备的自供电 远程唤醒 */
        // *data = NULL;//TODO: 这里不是很明白
        // *len = 2;
        *data = (uint8_t *)&usbd_core_cfg.remote_wakeup;
        *len = 2;
    case USB_REQUEST_CLEAR_FEATURE:
        /*!< CLEAR_FEATURE */
        if (setup->wValue == 1)
        {
            /*!< 没啥用 */
        }
        break;
    case USB_REQUEST_SET_FEATURE:
        /*!< SET_FEATURE */
        if (setup->wValue == 1)
        {
            /*!< 没啥用 */
        }
        break;
    case USB_REQUEST_GET_INTERFACE:
        /*!< GET_INTERFACE */
        /*!< 这个请求在不支持备用接口的类里面调用的时候  总是会返回0
             而具有备用接口的类的这个请求会在自定义请求里面处理 */
        *data = (uint8_t *)&usbd_core_cfg.reserved; // TODO: 这里不是很明白
        *len = 1;
        break;
    case USB_REQUEST_SET_INTERFACE:
        /*!< SET_INTERFACE 配置接口 */
        USB_LOG("REQ_SET_INTERFACE \r\n");
        ret = usbd_set_interface(setup->wIndex, setup->wValue);
        break;
    default:
        USB_LOG("Illegal interface request \r\n");
        return false;
        break;
    }
    return ret;
}
/**
 * @brief            标准端点请求
 * @pre              无
 * @param[in]        setup
 * @param[in]        data
 * @param[in]        len
 * @retval           true 成功 否则失败
 */
static bool usbd_standard_endpoint_request_handler(struct usb_setup_packet *setup, uint8_t **data, uint32_t *len)
{
    bool ret = true;
    uint8_t ep = setup->wIndex;
    if (usbd_core_cfg.configuration == 0)
    {
        /*!< 没有枚举完成 */
        return false;
    }
    switch (setup->bRequest)
    {
    case USB_REQUEST_GET_STATUS:
        /*!< GET STATUS 获取端点状态 */
        if (((ep & 0x7f) == 0) || is_device_configured())
        {
            usbd_ep_get_stall(ep, (uint8_t *)&usbd_core_cfg.remote_wakeup);
            *data = (uint8_t *)&usbd_core_cfg.remote_wakeup;
            *len = 2;
            break;
        } // TODO: 不明白这里的获取端点状态 下面注释部分是之前写的
        // *data = NULL;
        // *len = 2;
        return false;
    case USB_REQUEST_CLEAR_FEATURE:
        /*!< CLEAR_FEATURE */
        /*!< 当设备尚未配置时，此请求对控制端点有效。
             对于其他端点，必须配置设备。首先检查处理过的ep是否为控制端点。
             如果没有，则设备必须处于Configured状态才能接受请求。
         */
        if (setup->wValue == USB_FEATURE_ENDPOINT_HALT)
        {
            if (((ep & 0x7f) == 0) || is_device_configured())
            {
                USB_LOG("ep:%x clear halt \r\n", ep);
                usbd_ep_clear_stall(ep);
                usbd_notify_callback(USB_EVENT_CLEAR_HALT, NULL);
                break;
            }
        }
        return false;
    case USB_REQUEST_SET_FEATURE:
        /*!< SET FEATURE */
        /*!< 当设备尚未配置时，此请求对控制端点有效。
             对于其他端点，必须配置设备。首先检查处理过的ep是否为控制端点。
             如果没有，则设备必须处于Configured状态才能接受请求。
         */
        if (setup->wValue == USB_FEATURE_ENDPOINT_HALT)
        {
            if (((ep & 0x7f) == 0) || is_device_configured())
            {
                USB_LOG("ep:%x set halt \r\n", ep);
                usbd_ep_set_stall(ep);
                usbd_notify_callback(USB_EVENT_SET_HALT, NULL);
                break;
            }
        }
        return false;
    case USB_REQUEST_SYNCH_FRAME:
        if (is_device_configured())
        {
            /*!< 请求未实现 */
            // TODO: 不知道这里是什么意思
            USB_LOG("ep request 0x%02x not implemented \r\n", setup->bRequest);
        }
        return false;
    default:
        USB_LOG("Err:Illegal endpoint request \r\n");
        break;
    }
    return ret;
}
/**
 * @brief            总的标准请求 用来管理标准设备 标准接口等
 * @pre              无
 * @param[in]        setup
 * @param[in]        data
 * @param[in]        len
 * @retval           true 成功 否则失败
 */
static int usbd_standard_request_handler(struct usb_setup_packet *setup, uint8_t **data, uint32_t *len)
{
    int ret = true;
    switch (setup->bmRequestType_b.Recipient)
    {
    case USB_REQUEST_TO_DEVICE:
        /*!< 接收方是Device */
        if (usbd_standard_device_request_handler(setup, data, len) == false)
        {
            ret = -1;
        }
        break;
    case USB_REQUEST_TO_INTERRFACE:
        /*!< Interface */
        if (usbd_standard_interface_request_handler(setup, data, len) == false)
        {
            ret = -1;
        }
        break;
    case USB_REQUEST_TO_ENDPOINT:
        /*!< Endpoint */
        if (usbd_standard_endpoint_request_handler(setup, data, len) == false)
        {
            ret = -1;
        }
        break;
    default:
        ret = -1;
        break;
    }
    return ret;
}
/**
 * @brief            总的Class请求
 * @pre              无
 * @param[in]        setup
 * @param[in]        data
 * @param[in]        len
 * @retval           true 成功 否则失败
 */
static int usbd_class_request_handler(struct usb_setup_packet *setup, uint8_t **data, uint32_t *len)
{
    if (setup->bmRequestType_b.Recipient != USB_REQUEST_TO_INTERRFACE) /*!< 类设备请求的接收者只能是接口 */
    {
        return -1;
    }
    /*!< 列出所有的接口 */
    /*!< 靠setup中的wIndex的低字节来判断是哪一个接口 */
    usb_slist_t *i_ptr; /*!< 用来搜索整个Class链表 */
    usb_slist_t *j_ptr; /*!< 用来搜索每个类下面的接口 */
#if (0)
    for (i_ptr = usbd_class_head.next; i_ptr != NULL; i_ptr = i_ptr->next)
    {
        usbd_class_t *class_ptr = (usbd_class_t *)i_ptr; /*!< 将遍历的这个指针i_ptr强转成usbd_class类型的  用来访问里面的数据 */
        for (j_ptr = class_ptr->intf_list.next; j_ptr != NULL; j_ptr = j_ptr->next)
        {
            usbd_interface_t *intf_ptr = (usbd_interface_t *)j_ptr; /*!< 将遍历的这个指针j_ptr强转成usbd_interface_t类型的  用来访问里面的数据 */
            if (intf_ptr->class_handler && (intf_ptr->intf_num == (setup->wIndex & 0xff)))
            {
                return intf_ptr->class_handler(setup, data, len); /*!< 调用接口的类请求函数 */
            }
        }
    }
#endif
    /*!< 用i_ptr node 去遍历整个class链表 */
    usb_slist_for_each(i_ptr, &usbd_class_head)
    {
        /*!< 根据i_ptr这个节点找到这个节点所在的结构体（usbd_class_t） */
        usbd_class_t *class_ptr = usb_slist_entry(i_ptr, usbd_class_t, list);
        /*!< 用j_ptr 去遍历结构体class_ptr 中的intf_list下面挂载的接口 */
        usb_slist_for_each(j_ptr, &(class_ptr->intf_list))
        {
            usbd_interface_t *intf_ptr = usb_slist_entry(j_ptr, usbd_interface_t, list);
            if (intf_ptr->class_handler && (intf_ptr->intf_num == (setup->wIndex & 0xff)))
            {
                return intf_ptr->class_handler(setup, data, len); /*!< 调用接口的类请求函数 */
            }
        }
    }
    return -1;
}
/**
 * @brief            总的制造商请求
 * @pre              无
 * @param[in]        setup
 * @param[in]        data
 * @param[in]        len
 * @retval           true 成功 否则失败
 */
static int usbd_vendor_request_handler(struct usb_setup_packet *setup, uint8_t **data, uint32_t *len)
{
    if (setup->bmRequestType_b.Recipient != 0) /*!< 一般来说  制造商请求的的接收者是类 */
    {
        return -1;
    }
    // TODO: 这里的一些微软描述符没弄  暂时用不到
    /*!< 微软的 */
    if (mos_desc)
    {
        if (setup->bRequest == 0x17)
        {
            switch (setup->wIndex)
            {
            case 0x04: /*!< 最通用的 */
                /*!< 获取微软描述符的一些信息 */
                break;
            default:
                break;
            }
        }
    }

    usb_slist_t *i_ptr; /*!< 用来搜索整个Class链表 */
    usb_slist_t *j_ptr; /*!< 用来搜索每个类下面的接口 */
#if (0)
    for (i_ptr = usbd_class_head.next; i_ptr != NULL; i_ptr = i_ptr->next)
    {
        usbd_class_t *class_ptr = (usbd_class_t *)i_ptr; /*!< 将遍历的这个指针i_ptr强转成usbd_class类型的  用来访问里面的数据 */
        for (j_ptr = class_ptr->intf_list.next; j_ptr != NULL; j_ptr = j_ptr->next)
        {
            usbd_interface_t *intf_ptr = (usbd_interface_t *)j_ptr; /*!< 将遍历的这个指针j_ptr强转成usbd_interface_t类型的  用来访问里面的数据 */
            if (intf_ptr->vendor_handler && ((intf_ptr->intf_num == (setup->wIndex & 0xff)) || ((intf_ptr->intf_num == (setup->wValue & 0xff)))))
            /*!< 这个制造商的请求可能不是根据wIndex的值来确定接口号 也可能是wVaule */
            {
                return intf_ptr->vendor_handler(setup, data, len); /*!< 调用接口的类请求函数 */
            }
        }
    }
#endif
    usb_slist_for_each(i_ptr, &usbd_class_head)
    {
        usbd_class_t *class_ptr = usb_slist_entry(i_ptr, usbd_class_t, list);
        usb_slist_for_each(j_ptr, &(class_ptr->intf_list))
        {
            usbd_interface_t *intf_ptr = usb_slist_entry(j_ptr, usbd_interface_t, list);
            if (intf_ptr->vendor_handler && ((intf_ptr->intf_num == (setup->wIndex & 0xff)) || ((intf_ptr->intf_num == (setup->wValue & 0xff)))))
            /*!< 这个制造商的请求可能不是根据wIndex的值来确定接口号 也可能是wVaule */
            {
                return intf_ptr->vendor_handler(setup, data, len); /*!< 调用接口的类请求函数 */
            }
        }
    }

    return -1;
}
/**
 * @brief            不常见的请求  自定义请求
 * @pre              无
 * @param[in]        setup
 * @param[in]        data
 * @param[in]        len
 * @retval           true 成功 否则失败
 */
static int usbd_custom_request_handler(struct usb_setup_packet *setup, uint8_t **data, uint32_t *len)
{
    if (setup->bmRequestType_b.Recipient != USB_REQUEST_TO_INTERRFACE) /*!< Custom设备请求的接收者只能是接口 */
    {
        return -1;
    }
    /*!< 列出所有的接口 */
    /*!< 靠setup中的wIndex的低字节来判断是哪一个接口 */
    usb_slist_t *i_ptr; /*!< 用来搜索整个Class链表 */
    usb_slist_t *j_ptr; /*!< 用来搜索每个类下面的接口 */
#if (0)
    for (i_ptr = usbd_class_head.next; i_ptr != NULL; i_ptr = i_ptr->next)
    {
        usbd_class_t *class_ptr = (usbd_class_t *)i_ptr; /*!< 将遍历的这个指针i_ptr强转成usbd_class类型的  用来访问里面的数据 */
        for (j_ptr = class_ptr->intf_list.next; j_ptr != NULL; j_ptr = j_ptr->next)
        {
            usbd_interface_t *intf_ptr = (usbd_interface_t *)j_ptr; /*!< 将遍历的这个指针j_ptr强转成usbd_interface_t类型的  用来访问里面的数据 */
            if (intf_ptr->custom_handler && (intf_ptr->intf_num == (setup->wIndex & 0xff)))
            {
                return intf_ptr->custom_handler(setup, data, len); /*!< 调用接口的类请求函数 */
            }
        }
    }
#endif

    usb_slist_for_each(i_ptr, &usbd_class_head)
    {
        usbd_class_t *class_ptr = usb_slist_entry(i_ptr, usbd_class_t, list);
        usb_slist_for_each(j_ptr, &(class_ptr->intf_list))
        {
            usbd_interface_t *intf_ptr = (usbd_interface_t *)j_ptr; /*!< 将遍历的这个指针j_ptr强转成usbd_interface_t类型的  用来访问里面的数据 */
            if (intf_ptr->custom_handler && (intf_ptr->intf_num == (setup->wIndex & 0xff)))
            {
                return intf_ptr->custom_handler(setup, data, len); /*!< 调用接口的类请求函数 */
            }
        }
    }

    return -1;
}
/**
 * @brief            总的setup请求来管理其他请求
 * @pre              无
 * @param[in]        setup
 * @param[in]        data
 * @param[in]        len
 * @retval           true 成功 否则失败
 */
static int usbd_setup_request_handler(struct usb_setup_packet *setup, uint8_t **data, uint32_t *len)
{
    int ret = 0;
    /*!< 对custom请求  需要提前判断 */
    if (!usbd_custom_request_handler(setup, data, len))
    {
        /*!< 请求成功直接返回 如果失败则继续往下 */
        return 0;
    }
    /*!< 请求类型 */
    switch (setup->bmRequestType_b.Type)
    {
    case USB_REQUEST_STANDARD:
        /*!< 标准请求：Standard */
        if (usbd_standard_request_handler(setup, data, len) < 0)
        {
            USB_LOG("Err:Handler Error %d \r\n", setup->bmRequestType_b.Type);
            ret = -1;
        }
        break;
    case USB_REQUEST_CLASS:
        /*!< Class请求 */
        if (usbd_class_request_handler(setup, data, len) < 0)
        {
            USB_LOG("Err:Handler Error %d \r\n", setup->bmRequestType_b.Type);
            ret = -1;
        }
        break;
    case USB_REQUEST_VENDOR:
        /*!< Vendor请求 */
        if (usbd_vendor_request_handler(setup, data, len) < 0)
        {
            USB_LOG("Err:Handler Error %d \r\n", setup->bmRequestType_b.Type);
            ret = -1;
        }
    default:
        ret = -1;
        break;
    }
    return ret;
}
/**
 * @brief            注册描述符
 * @pre              无
 * @param[in]        desc  总的描述符的首地址
 * @retval           返回
 */
void usbd_desc_register(const uint8_t *desc)
{
    usbd_core_cfg.descriptors = desc;
}
/**
 * @brief            注册一个类（就是在已有的类链表的尾部加上一个类）
 * @pre              无
 * @param[in]        class 需要被注册的类
 * @retval           无
 */
void usbd_class_register(usbd_class_t *class)
{
    usb_slist_add_tail(&usbd_class_head, &(class->list)); /*!< 将class->list 插入到usbd_class_head后面 */
    usb_slist_init(&(class->intf_list));                  /*!< 初始化class类下的接口链表的头节点 */
}
/**
 * @brief            给类添加一个接口
 * @pre              无
 * @param[in]        class      需要被添加接口的类
 * @param[in]        interface  待添加进类的接口
 * @retval           无
 */
void usbd_class_add_interface(usbd_class_t *class, usbd_interface_t *interface)
{
    static uint8_t intf_offset = 0;                              /*!< 这个函数体内部的局部变量  只会在第一次调用此函数的时候被初始化  且不会被销毁 */
    interface->intf_num = intf_offset;                           /*!< 将偏移量给到接口的intf_num */
    usb_slist_add_tail(&(class->intf_list), &(interface->list)); /*!< 将interface->list 插入到class->intf_list的后面 */
    usb_slist_init(&(interface->ep_list));                       /*!< 初始化interface下的端点链表的头节点 */
    intf_offset++;                                               /*!< 每调用一次 intf_offset自增 */
}
/**
 * @brief            给接口添加一个端点
 * @pre              无
 * @param[in]        interface  需要被添加端点的接口
 * @param[in]        ep         待添加进接口的端点
 * @retval           无
 */
void usbd_interface_add_endpoint(usbd_interface_t *interface, usbd_endpoint_t *ep)
{
    usb_slist_add_tail(&(interface->ep_list), &(ep->list)); /*!< 将ep->list 插入到interface->ep_list的后面 */
}
/**
 * @brief            发送数据给主机
 * @pre              无
 * @param[in]        len   数据长度
 * @retval           无
 */
static void usbd_send_to_host(uint16_t len)
{
    /*!< 分两种 */
    /*!< 第一种 0长包 */
    if (usbd_core_cfg.zlp_flag == false)
    {
        uint32_t chunk = usbd_core_cfg.ep0_data_buf_residue;
        /*!< 这里我的理解就是：
             假如需要发送18字节 那么ep0_data_buf_residue就等于18
             对于端点0  最大是可以一下发送64字节的  所以18字节一下就发送完了 并且传入的chunk会被修改成实际发送的字节数目
             所以 只需要发送一次。
             但是如果要发送70字节 传入的chunk就会被修改成实际发送的数目（64字节）
             所以需要两次才能完成发送
             这个时候  ep0_data_buf_residue的作用就体现出来了  用来记录剩余需要发送的字节数目
             发完一次  主机接收到数据以后就会触发端点0的in中断
             如果待发送数据长度大于64字节 直接在in中断里面再次启动usbd_send_to_host即可*/
        usbd_ep_write(USB_CTRL_IN_EP0, usbd_core_cfg.ep0_data_buf, usbd_core_cfg.ep0_data_buf_residue, &chunk);
        // TODO:                                                                           /*!< TIP 这里不太明白 可能这个chunk传进去会根据实际所发送字节数目变换 */
        usbd_core_cfg.ep0_data_buf_residue -= chunk; /*!< 减掉已经发送完成的字节数目 */
        usbd_core_cfg.ep0_data_buf += chunk;         /*!< 数据缓冲区指针偏移chunk个 */
        // TODO:                                                                                  /*!< 判断是否完成发送  且 ！！这里又不明白 不明白为什么要用ep0_data_buf_len来计算 */
        if ((!usbd_core_cfg.ep0_data_buf_residue) && !(usbd_core_cfg.ep0_data_buf_len % 64) && usbd_core_cfg.ep0_data_buf_len >= 64) /*!< 判断是否完成发送  且 ！！这里又不明白*/
        {
            /*!< 未完成发送  且发送总长是64的整数倍 这个时候需要发送一个0长度数据包 */
            usbd_core_cfg.zlp_flag = true; /*!< 满足这个条件 zlp_flag插上 表示下一次要发送一个0长度包 */
        }
    }
    else
    {
        usbd_core_cfg.zlp_flag = false;                /*!< 问题  zlp什么时候为true */
        usbd_ep_write(USB_CTRL_IN_EP0, NULL, 0, NULL); /*!< 发送一个0长包 0长包只会在发送64整数倍字节数目的时候才会发送 */
    }
}
/**
 * @brief            setup中断  其中会调用setup请求
 * @pre              无
 * @param[in]        无
 * @retval           无
 */
static void usbd_ep0_setup_handler(void *arg)
{
    struct usb_setup_packet *setup;
    if (arg == NULL)
    {
        //struct usb_setup_packet *setup = &(usbd_core_cfg.setup);
        setup = &(usbd_core_cfg.setup);                                                                  /*!< 创建一个局部变量  将setup中断主机发送过来的数据存放在里面 */
        if (usbd_ep_read(USB_CTRL_OUT_EP0, (uint8_t *)setup, sizeof(struct usb_setup_packet), NULL) < 0) /*!< 开始读取数值  8字节 */
        {
            /*!< 发生错误 */
            USB_LOG("Err:Read Setup Packte failed \r\n");
            usbd_ep_set_stall(USB_CTRL_IN_EP0); /*!< 发送错误包STALL包 */
            return;
        }
    }
    else
    {
        setup = arg;
    }
#ifdef USB_DEBUG
    //USB_LOG_SETUP(setup);
#endif
    if (setup->wLength > USB_REQUEST_BUFFER_SIZE) /*!< 如果setup想要发送或者获取的数据大于255 */
    {
        if (setup->bmRequestType_b.Dir != USB_REQUEST_DEVICE_TO_HOST) /*!< 方向不是设备到主机 */
        {
            usbd_ep_set_stall(USB_CTRL_IN_EP0); /*!< 发送错误包STALL包  ！！不让你发 */
            return;
        }
    }

    /*!< 到这就是正确的数据///如果上面直接返回了  说明是错误的数据 */
    /*!< 存储数据 */
    /*!< TODO fix a bug */
    usbd_core_cfg.ep0_data_buf = usbd_core_cfg.req_data;
    usbd_core_cfg.ep0_data_buf_len = setup->wLength;                                  /*!< 主机想要发送 或者 想要获取的数据长度 */
    usbd_core_cfg.ep0_data_buf_residue = setup->wLength;                              /*!< 实际剩余的长度  这是一个设备给主机发送数据包的一个辅助变量 */
    usbd_core_cfg.zlp_flag = false;                                                   /*!< 用来判断0长包 */
    if (setup->wLength && (setup->bmRequestType_b.Dir == USB_REQUEST_HOST_TO_DEVICE)) /*!< setup的数据包长度大于0（要注意的是有两种，一种是主机想要获得，另一种是主机想要发送） 且方向是主机到设备（主机想要发送）//是SET */
    {
        /*!< 基本上是class请求 */
        /*!< 主机发送给设备数据 是out 不在这里分析 这种情况拿到out中断里面去分析 */
        USB_LOG("The host is going to send data to the device \r\n");
        /*!< in端点和out端点是不是共享的  如果不是共享的  开启下一次的端点 */
        //usbd_ep_read(USB_CTRL_OUT_EP0, NULL, 0, NULL);
        // TODO: /*!< 这里不是很明白怎么回事 */
        return; /*!< 这里直接退出setup中断  去等out中断了 */
    }
    // TODO: /*!< 到这  默认主机是获取数据 因为只有一种情况是主机发送数据 wLength==0 Dir==1 就是发送0个数据 这种情况有待考虑 */
    /*!< 到这里 说明是主机想要获得数据 获取的数据长度也都知道了 可以直接调用setup_request了 */
    if (usbd_setup_request_handler(setup, &usbd_core_cfg.ep0_data_buf, &usbd_core_cfg.ep0_data_buf_len) < 0)
    {
        /*!< usbd_setup_request_handler 失败 */
        USB_LOG("Err:usbd_setup_request_handler failed \r\n");
        usbd_ep_set_stall(USB_CTRL_IN_EP0);
        return;
    }
    usbd_core_cfg.ep0_data_buf_residue = MIN(usbd_core_cfg.ep0_data_buf_len, setup->wLength); /*!< 实际发送的数据长度  (setup->wLength是主机要的)因为主机可能想要64字节 但是我只有10个 所以要取小的 */
    /*!< 到这里   想要发送的数据和想要发送数据的长度都已经定下来了    数据长度就是usbd_core_cfg.ep0_data_buf_residue  想要发送的数据就是 usbd_core_cfg.ep0_data_buf*/
    usbd_send_to_host(setup->wLength);
}
/**
 * @brief            端点0的IN中断
 * @pre              无
 * @param[in]        无
 * @retval           无
 */
static void usbd_ep0_in_handler(void)
{
    if (usbd_core_cfg.ep0_data_buf_residue != 0 || usbd_core_cfg.zlp_flag == true)
    {
        usbd_send_to_host(usbd_core_cfg.setup.wLength);
    }
}
/**
 * @brief            端点0的OUT中断
 * @pre              无
 * @param[in]        无
 * @retval           无
 */
static void usbd_ep0_out_handler(void *arg)
{
    if (arg != NULL)
    {
        /*!< 更新端点0剩余字节字节数 */
        usbd_core_cfg.ep0_data_buf_residue = *(uint32_t *)arg;
    }
    uint32_t chunk;
    /*!< 如果主机发来的是0长度数据包 */
    if (usbd_core_cfg.ep0_data_buf_residue == 0)
    {
        /*!< 吸收0长度数据包 并且开启下一次out端点 */
        if (usbd_ep_read(USB_CTRL_OUT_EP0, NULL, 0, NULL) < 0)
        {
            // TODO: /*!< 等待完成 */
            usbd_ep_set_stall(USB_CTRL_IN_EP0);
        }
        return;
    }
    // usbd_core_cfg.ep0_data_buf = usbd_core_cfg.req_data;                                                        /*!< 注意在没有usbd_core_cfg.ep0_data_buf = usbd_core_cfg.req_data这句之前  ep0_data_buf只是一个指向一个数据地址的指针*/
    // if (usbd_ep_read(USB_CTRL_OUT_EP0, usbd_core_cfg.ep0_data_buf, usbd_core_cfg.ep0_data_buf_len, &chunk) < 0) /*!< 读取主机发送的数据 */
    if (usbd_ep_read(USB_CTRL_OUT_EP0, usbd_core_cfg.ep0_data_buf, usbd_core_cfg.ep0_data_buf_residue, &chunk) < 0) /*!< 读取主机发送的数据 */
    {
        /*!< 读取失败 */
        usbd_ep_set_stall(USB_CTRL_IN_EP0);
        return;
    }
    
    usbd_core_cfg.ep0_data_buf += chunk;
    usbd_core_cfg.ep0_data_buf_residue -= chunk; /*!< 这个值会在端点0的SETUP中断里面先被填充了 */
    struct usb_setup_packet *setup = &(usbd_core_cfg.setup);
    if (usbd_core_cfg.ep0_data_buf_residue == 0)
    {
        /*!< 接收完成 */
        /*!< 这是out中断 是主机发送给设备数据  所以在这里调用这个usbd_setup_request_handler有别于setup中断里面调用的 */
        /*!< 这里调用  其实会给class请求  是将数据传给设备 */
        usbd_core_cfg.ep0_data_buf = usbd_core_cfg.req_data; /*!< 修正指针的位置 */
        if (usbd_setup_request_handler(setup, &usbd_core_cfg.ep0_data_buf, &usbd_core_cfg.ep0_data_buf_len) < 0)
        {
            return;
        }
        /*!< 返回状态给主机 */
        usbd_send_to_host(setup->wLength);
    }
    else
    {
        // TODO: /*!< 发生错误  自己解决 */
    }
}
/**
 * @brief            其他端点的IN中断
 * @pre              无
 * @param[in]        ep
 * @retval           无
 */
static void usbd_ep_in_handler(uint8_t ep)
{
    usbd_core_cfg.in_ep_cb[ep & 0x7f](ep);
}
/**
 * @brief            其他端点的OUT中断
 * @pre              无
 * @param[in]        ep
 * @retval           无
 */
static void usbd_ep_out_handler(uint8_t ep)
{
    usbd_core_cfg.out_ep_cb[ep & 0x7f](ep);
}
/**
 * @brief            简述
 * @pre              前置条件
 * @param[in]        参数
 * @retval           返回
 */
static void usbd_class_event_notify_handler(uint8_t event, void *args)
{
    /*!< 先搜索注册了多少个类 */
    /*!< 列出所有的接口 */
    /*!< 靠setup中的wIndex的低字节来判断是哪一个接口 */
    usb_slist_t *i_ptr; /*!< 用来搜索整个Class链表 */
    usb_slist_t *j_ptr; /*!< 用来搜索每个类下面的接口 */
    usb_slist_for_each(i_ptr, &usbd_class_head)
    {
        usbd_class_t *class_ptr = usb_slist_entry(i_ptr, usbd_class_t, list); /*!< 这个API的作用就是返回节点中的list的地址  指向类型为usbd_class_t */
        /*!< 接下来用j_ptr这个节点指针来访问所有 类下面链接的接口 */
        usb_slist_for_each(j_ptr, &(class_ptr->intf_list))
        {
            /*!< 但是现在我只能遍历接口节点  无法访问到里面的数据 */
            /*!< 下面用一个接口数据结过的指针来拿到接口节点里面的数据 */
            usbd_interface_t *intf_ptr = usb_slist_entry(j_ptr, usbd_interface_t, list);
            if (intf_ptr->notify_handler)
            {
                intf_ptr->notify_handler(event, args);
            }
        }
    }
}
/**
 * @brief            总中断  硬件中断会Call这个函数  根据不同的EVENT执行不同的操作
 * @pre              无
 * @param[in]        event  事件类型
 * @param[in]        arg
 * @retval           返回
 */
void usbd_notify_callback(uint8_t event, void *arg)
{
    switch (event)
    {
    case USB_EVENT_RESET:
        usbd_set_address(0);         /*!< TIP 这里比较重要  要将设备地址设置为0 */
        usbd_ep_callback_register(); /*!< 注册普通端点回调函数 */
        set_device_state(ATTACHED);  /*!< 复位完后  设备为上电状态 */
    case USB_EVENT_CLEAR_HALT:
        usbd_class_event_notify_handler(event, arg);
        break;
    case USB_EVENT_SOF:
        /* code */
        break;
    case USB_EVENT_CONNECTED:
        /* code */
        break;
    case USB_EVENT_CONFIGURED:
        /* code */
        set_device_state(CONFIGURED); /*!< 设置配置成功后 设备状态转为配置状态 */
        break;
    case USB_EVENT_SUSPEND:
        /* code */
        set_device_state(SUSPENDED);
        break;
    case USB_EVENT_DISCONNECTED:
        /* code */
        break;
    case USB_EVENT_RESUME:
        /* code */
        set_device_state(CONFIGURED);/*!< 设备在电脑休眠时候会进入suspend状态 电脑唤醒时会call USB_EVENT_RESUME 在此将设备转为配置状态 */
        break;
    case USB_EVENT_SET_INTERFACE:
        /* code */
        break;
    case USB_EVENT_SET_REMOTE_WAKEUP:
        /* code */
        break;
    case USB_EVENT_CLEAR_REMOTE_WAKEUP:
        /* code */
        break;
    case USB_EVENT_SET_HALT:
        /* code */
        break;
    /*!< USB硬件相关的中断 */
    case USB_EVENT_SETUP_NOTIFY:
        usbd_ep0_setup_handler(arg); /*!< 端点0  setup中断 */
        break;
    case USB_EVENT_EP0_IN_NOTIFY:
        usbd_ep0_in_handler(); /*!< 端点0  in中断 */
        break;
    case USB_EVENT_EP0_OUT_NOTIFY:
        usbd_ep0_out_handler((uint32_t *)arg); /*!< 端点0  out中断 */
        break;
    case USB_EVENT_EP_IN_NOTIFY:
        usbd_ep_in_handler((uint32_t)arg); /*!< 普通端点 in中断 */
        break;
    case USB_EVENT_EP_OUT_NOTIFY:
        usbd_ep_out_handler((uint32_t)arg); /*!< 普通端点 out中断 */
        break;
    case USB_EVENT_UNKNOWN:
        /* code */
        break;
    case USB_EVENT_ERROR:
        /* code */
        break;
    default:
        break;
    }
}


/************************ (C) COPYRIGHT 2021 Li Guo *****END OF FILE*************/