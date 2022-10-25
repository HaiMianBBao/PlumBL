/** @mainpage  ch579 usb协议栈 port接口示例
 * <table>
 * <tr><th>Project  <td>
 * <tr><th>Author   <td>LiGuo 1570139720@qq.com
 * </table>
 * @section
 * 你可以仿照此示例将协议栈移植到你想用的soc上
 *
 * 根据不同单片机实现下面的接口即可
 * @section
 * -# 功能
 *
 * @section
 *
 * 版本|作者|时间|描述
 * ----|----|----|----
 * 1.0|LiGuo|2021.11.20|创建项目
 *
 * @section
 * -#
 *
 * @section   版权声明
 * <h2><center>&copy; Copyright 2021 LiGuo.
 * All rights reserved.</center></h2>
 *********************************************************************************
 */

/**
 * @brief   CH579 端点地址描述
 * ep0:     in: 0x80  out:0x00
 * ep1:     in: 0x81  out:0x01
 * ep2:     in: 0x82  out:0x02
 * ep3:     in: 0x83  out:0x03
 * ep4:     in: 0x84  out:0x04
 */

/* 文件引用 ------------------------------------------------------------------*/
#include "lg_usb_dc.h"
#include "lg_usbd_core.h"
#include "ch579_regs.h"
#include <stdlib.h>
#define CH579_USB (uint32_t *)0x40008000

/**
 * @brief   相关寄存器宏
 */

/*!< ep dir in */
#define EP_DIR_IN 1

/*!< ep dir out */
#define EP_DIR_OUT 0

/*!< 8-bit value of endpoint control register */
#define EPn_CTRL(epid) \
  *(volatile uint8_t *)(0x40008022 + epid * 4)

/*!< The length register value of the endpoint send buffer */
#define EPn_TX_LEN(epid) \
  *(volatile uint8_t *)(0x40008020 + epid * 4)

/*!< get ep id by epadd */
#define GET_EP_ID(ep_add) (uint8_t)(ep_add & 0x7f)

/*!< get ep dir by epadd */
#define GET_EP_DIR(ep_add) (uint8_t)(ep_add & 0x80)

/*!< get interrupt endpoint id */
#define GET_INT_EP_ID \
  (volatile uint8_t)(R8_USB_INT_ST & MASK_UIS_ENDP)

/*!< get usb interrupt state reg */
#define GET_USB_INT_STATE \
  (volatile uint8_t)(R8_USB_INT_ST & MASK_UIS_TOKEN)

/*!< read setup packet to use in ep0 in */
#define GET_SETUP_PACKET(data_add) \
  *(struct usb_setup_packet *)data_add

/*!< set device address // call in set_add state stage */
#define SET_DEVICE_ADDRESS(add) \
  R8_USB_DEV_AD = (R8_USB_DEV_AD & RB_UDA_GP_BIT) | add;

/*!< set epid ep tx valid */
#define EPn_SET_TX_VALID(epid) \
  EPn_CTRL(epid) = EPn_CTRL(epid) & ~MASK_UEP_T_RES | UEP_T_RES_ACK;

/*!< set epid ep rx valid */
#define EPn_SET_RX_VALID(epid) \
  EPn_CTRL(epid) = EPn_CTRL(epid) & ~MASK_UEP_R_RES | UEP_R_RES_ACK;

/*!< set epid ep tx nak */
#define EPn_SET_TX_NAK(epid) \
  EPn_CTRL(epid) = EPn_CTRL(epid) & ~MASK_UEP_T_RES | UEP_T_RES_NAK;

/*!< set epid ep rx nak */
#define EPn_SET_RX_NAK(epid) \
  EPn_CTRL(epid) = EPn_CTRL(epid) & ~MASK_UEP_R_RES | UEP_R_RES_NAK;

/*!< set epid ep tx stall */
#define EPn_SET_TX_STALL(epid) \
  EPn_CTRL(epid) = EPn_CTRL(epid) & ~MASK_UEP_T_RES | UEP_T_RES_STALL

/*!< set epid ep rx stall */
#define EPn_SET_RX_STALL(epid) \
  EPn_CTRL(epid) = EPn_CTRL(epid) & ~MASK_UEP_R_RES | UEP_R_RES_STALL

/*!< set epid ep tx len */
#define EPn_SET_TX_LEN(epid, len) \
  EPn_TX_LEN(epid) = len

/*!< set epid ep rx len */
#define EPn_SET_RX_LEN(epid, len)

/*!< get epid ep rx len */
#define EPn_GET_RX_LEN(epid) \
  R8_USB_RX_LEN

/*!< set fifo address */
#define EPn_SET_FIFO_ADD(epid, address) \
  (*((PUINT16V)(0x40008010 + epid * 4))) = address

/*!< ep nums */
#define EP_NUMS 5
/*!< ep mps */
#define EP_MPS 64
/*!< set ep4 in mps 64 */
#define EP4_IN_MPS EP_MPS
/*!< set ep4 out mps 64 */
#define EP4_OUT_MPS EP_MPS

/*!< User defined assignment endpoint RAM */
__attribute__((aligned(4))) uint8_t ep0_data_buff[64 + 64 + 64]; /*!< ep0(64)+ep4_out(64)+ep4_in(64) */
__attribute__((aligned(4))) uint8_t ep1_data_buff[64 + 64];      /*!< ep1_out(64)+ep1_in(64) */
__attribute__((aligned(4))) uint8_t ep2_data_buff[64 + 64];      /*!< ep2_out(64)+ep2_in(64) */
__attribute__((aligned(4))) uint8_t ep3_data_buff[64 + 64];      /*!< ep3_out(64)+ep3_in(64) */

uint8_t *EP0_RAM_Addr;
uint8_t *EP1_RAM_Addr;
uint8_t *EP2_RAM_Addr;
uint8_t *EP3_RAM_Addr;

#define pEP0_DataBuf (EP0_RAM_Addr)
#define pEP1_OUT_DataBuf (EP1_RAM_Addr)
#define pEP1_IN_DataBuf (EP1_RAM_Addr + 64)
#define pEP2_OUT_DataBuf (EP2_RAM_Addr)
#define pEP2_IN_DataBuf (EP2_RAM_Addr + 64)
#define pEP3_OUT_DataBuf (EP3_RAM_Addr)
#define pEP3_IN_DataBuf (EP3_RAM_Addr + 64)
#define pEP4_OUT_DataBuf (EP0_RAM_Addr + 64)
#define pEP4_IN_DataBuf (EP0_RAM_Addr + 128)

/**
 * @brief   Endpoint information structure
 */
typedef struct _usbd_ep_info
{
  uint8_t mps;          /*!< 端点最大包长 */
  uint8_t eptype;       /*!< 端点类型 */
  uint8_t *ep_ram_addr; /*!< Endpoint buffer address */
} usbd_ep_info;

/*!< ch579 usb stack 核心相关的 */
static struct _ch579_core_prvi
{
  uint8_t address; /*!< 地址 */
  usbd_ep_info ep_in[EP_NUMS];
  usbd_ep_info ep_out[EP_NUMS];
  struct usb_setup_packet setup;
} usb_dc_cfg;

/**
 * @brief            设置地址
 * @pre              无
 * @param[in]        address ：8位有效地址
 * @retval           >=0 成功 否则失败
 */
int usbd_set_address(const uint8_t address)
{
  if (address == 0)
  {
    R8_USB_DEV_AD = (R8_USB_DEV_AD & RB_UDA_GP_BIT) | address;
  }
  usb_dc_cfg.address = address;
  return 0;
}

/**
 * @brief            打开端点
 * @pre              无
 * @param[in]        ep_cfg : 端点配置结构体指针
 * @retval           >=0 成功 否则失败
 */
int usbd_ep_open(const struct usbd_endpoint_cfg *ep_cfg)
{
  /*!< 端点0是默认端点  只支持控制传输 */
  /*!< 端点号 */
  uint8_t epid = GET_EP_ID(ep_cfg->ep_addr);
  /*!< ep dir */
  bool dir = GET_EP_DIR(ep_cfg->ep_addr);
  /*!< 端点最大包长 */
  uint8_t mps = ep_cfg->ep_mps;

  if (dir == EP_DIR_IN)
  {
    /*!< in */
    usb_dc_cfg.ep_in[epid].mps = mps;
  }
  else if (dir == EP_DIR_OUT)
  {
    /*!< out */
    usb_dc_cfg.ep_out[epid].mps = mps;
  }
  return 0;
}

/**
 * @brief            关闭端点
 * @pre              无
 * @param[in]        ep ： 端点地址
 * @retval           >=0 成功 否则失败
 */
int usbd_ep_close(const uint8_t ep)
{
  return 0;
}

/**
 * @brief            写发送缓冲区
 * @pre              无
 * @param[in]        ep ： 端点地址
 * @param[in]        data ： 待写入数据缓冲区首地址
 * @param[in]        data_len ： 写入总长
 * @param[in]        ret_bytes ： 实际写入的长度
 * @retval           >=0 成功 否则失败
 */
int usbd_ep_write(const uint8_t ep, const uint8_t *data, uint32_t data_len, uint32_t *ret_bytes)
{
  /*!< EP ID */
  uint8_t epid = GET_EP_ID(ep);
  /*!< 端点最大包长 */
  uint8_t ep_mps = usb_dc_cfg.ep_in[epid].mps;
  /*!< 分析实际写入到缓冲区的数据个数 */
  uint32_t real_wt_nums = 0;

  /*!< Analyze bytes actually written */
  if (data == NULL && data_len > 0)
  {
    return -1;
  }

  if (data_len == 0)
  {
    /*!< write 0 len data */
    EPn_SET_TX_LEN(epid, 0);
    /*!< enable tx */
    EPn_SET_TX_VALID(epid);
    /*!< return */
    return 0;
  }

  if (data_len > ep_mps)
  {
    /*!< If the data length is greater than the maximum packet length of the endpoint,
         the actual written data length is limited to the maximum packet length of the endpoint */
    real_wt_nums = ep_mps;
  }
  else
  {
    real_wt_nums = data_len;
  }
  /*!< write buff */
  memcpy(usb_dc_cfg.ep_in[epid].ep_ram_addr, data, real_wt_nums);
  /*!< write real_wt_nums len data */
  EPn_SET_TX_LEN(epid, real_wt_nums);
  /*!< enable tx */
  EPn_SET_TX_VALID(epid);

  if (ret_bytes != NULL)
  {
    *ret_bytes = real_wt_nums;
  }
  return 0;
}

/**
 * @brief            读接收缓冲区
 * @pre              无
 * @param[in]        ep ： 端点地址
 * @param[in]        data ： 读出数据存放的缓冲区首地址
 * @param[in]        max_data_len ： 最大读出长度
 * @param[in]        read_bytes ： 实际读出的长度
 * @retval           >=0 成功 否则失败
 */
int usbd_ep_read(const uint8_t ep, uint8_t *data, uint32_t max_data_len, uint32_t *read_bytes)
{
  /*!< ep id */
  uint8_t epid = GET_EP_ID(ep);
  /*!< real read byte nums */
  uint32_t real_rd_nums = 0;
  /*!< ep mps */
  uint8_t ep_mps = usb_dc_cfg.ep_out[epid].mps;
  if (data == NULL && max_data_len > 0)
  {
    return -1;
  }

  if (max_data_len == 0)
  {
    /*!< Enable reception */
    /*!< Non endpoint 0 can directly enable reception here,
         and endpoint 0 can enable reception after the end of the interrupt */
    if (epid != 0)
      EPn_SET_RX_VALID(epid);
    return 0;
  }

  if (max_data_len > ep_mps)
  {
    /*!< If the maximum length of the expected readout is greater than the maximum packet length of the endpoint,
         the expected maximum readout length is limited to the maximum packet length */
    max_data_len = ep_mps;
  }

  real_rd_nums = EPn_GET_RX_LEN(epid);
  real_rd_nums = MIN(real_rd_nums, max_data_len);

  /*!< read buff */
  memcpy(data, usb_dc_cfg.ep_out[epid].ep_ram_addr, real_rd_nums);

  if (read_bytes != NULL)
  {
    *read_bytes = real_rd_nums;
  }
  return 0;
}

/**
 * @brief            端点设置暂停
 * @pre              无
 * @param[in]        ep ： 端点地址
 * @retval           >=0 成功 否则失败
 */
int usbd_ep_set_stall(const uint8_t ep)
{
  /*!< ep id */
  uint8_t epid = GET_EP_ID(ep);
  EPn_SET_RX_STALL(epid);
  EPn_SET_TX_STALL(epid);
  return 0;
}

/**
 * @brief            端点清除暂停
 * @pre              无
 * @param[in]        ep ： 端点地址
 * @retval           >=0 成功 否则失败
 */
int usbd_ep_clear_stall(const uint8_t ep)
{
  int ret;
  switch (ep)
  {
  case 0x82:
    R8_UEP2_CTRL = (R8_UEP2_CTRL & ~(RB_UEP_T_TOG | MASK_UEP_T_RES)) | UEP_T_RES_NAK;
    ret = 0;
    break;
  case 0x02:
    R8_UEP2_CTRL = (R8_UEP2_CTRL & ~(RB_UEP_R_TOG | MASK_UEP_R_RES)) | UEP_R_RES_ACK;
    ret = 0;
    break;
  case 0x81:
    R8_UEP1_CTRL = (R8_UEP1_CTRL & ~(RB_UEP_T_TOG | MASK_UEP_T_RES)) | UEP_T_RES_NAK;
    ret = 0;
    break;
  case 0x01:
    R8_UEP1_CTRL = (R8_UEP1_CTRL & ~(RB_UEP_R_TOG | MASK_UEP_R_RES)) | UEP_R_RES_ACK;
    ret = 0;
    break;
  default:
    /*!< Unsupported endpoint */
    ret = -1;
    break;
  }
  return ret;
}

/**
 * @brief            检查端点状态
 * @pre              无
 * @param[in]        ep ： 端点地址
 * @param[in]        stalled ： 传出的端点状态
 * @retval           >=0 成功 否则失败
 */

int usbd_ep_get_stall(const uint8_t ep, uint8_t *stalled)
{
  return 0;
}

/**
 * @brief            usb 初始化
 * @pre              无
 * @param[in]        无
 * @retval           >=0 成功 否则失败
 */
int usb_dc_init(void)
{
  EP0_RAM_Addr = ep0_data_buff;
  EP1_RAM_Addr = ep1_data_buff;
  EP2_RAM_Addr = ep2_data_buff;
  EP3_RAM_Addr = ep3_data_buff;

  usb_dc_cfg.ep_in[0].ep_ram_addr = pEP0_DataBuf;
  usb_dc_cfg.ep_out[0].ep_ram_addr = pEP0_DataBuf;

  usb_dc_cfg.ep_in[1].ep_ram_addr = pEP1_IN_DataBuf;
  usb_dc_cfg.ep_out[1].ep_ram_addr = pEP1_OUT_DataBuf;

  usb_dc_cfg.ep_in[2].ep_ram_addr = pEP2_IN_DataBuf;
  usb_dc_cfg.ep_out[2].ep_ram_addr = pEP2_OUT_DataBuf;

  usb_dc_cfg.ep_in[3].ep_ram_addr = pEP3_IN_DataBuf;
  usb_dc_cfg.ep_out[3].ep_ram_addr = pEP3_OUT_DataBuf;

  usb_dc_cfg.ep_in[4].ep_ram_addr = pEP4_IN_DataBuf;
  usb_dc_cfg.ep_out[4].ep_ram_addr = pEP4_OUT_DataBuf;

  R8_USB_CTRL = 0x00; // 先设定模式,取消 RB_UC_CLR_ALL

  R8_UEP4_1_MOD = RB_UEP4_RX_EN | RB_UEP4_TX_EN | RB_UEP1_RX_EN | RB_UEP1_TX_EN; // 端点4 OUT+IN,端点1 OUT+IN
  R8_UEP2_3_MOD = RB_UEP2_RX_EN | RB_UEP2_TX_EN | RB_UEP3_RX_EN | RB_UEP3_TX_EN; // 端点2 OUT+IN,端点3 OUT+IN

  R16_UEP0_DMA = (uint16_t)(uint32_t)EP0_RAM_Addr;
  R16_UEP1_DMA = (uint16_t)(uint32_t)EP1_RAM_Addr;
  R16_UEP2_DMA = (uint16_t)(uint32_t)EP2_RAM_Addr;
  R16_UEP3_DMA = (uint16_t)(uint32_t)EP3_RAM_Addr;

  R8_UEP0_CTRL = UEP_R_RES_ACK | UEP_T_RES_NAK;
  R8_UEP1_CTRL = UEP_R_RES_ACK | UEP_T_RES_NAK | RB_UEP_AUTO_TOG;
  R8_UEP2_CTRL = UEP_R_RES_ACK | UEP_T_RES_NAK | RB_UEP_AUTO_TOG;
  R8_UEP3_CTRL = UEP_R_RES_ACK | UEP_T_RES_NAK | RB_UEP_AUTO_TOG;
  R8_UEP4_CTRL = UEP_R_RES_ACK | UEP_T_RES_NAK;

  R8_USB_DEV_AD = 0x00;
  R8_USB_CTRL = RB_UC_DEV_PU_EN | RB_UC_INT_BUSY | RB_UC_DMA_EN; // 启动USB设备及DMA，在中断期间中断标志未清除前自动返回NAK
  R8_USB_INT_FG = 0xFF;                                          // 清中断标志
  R16_PIN_ANALOG_IE |= RB_PIN_USB_IE;                            // 防止USB端口浮空
  R8_UDEV_CTRL = RB_UD_PD_DIS | RB_UD_PORT_EN;                   // 允许USB端口
  R8_USB_INT_EN = RB_UIE_SUSPEND | RB_UIE_BUS_RST | RB_UIE_TRANSFER;

  return 0;
}

/**
 * @brief            usb 中断处理函数
 * @pre              无
 * @param[in]        无
 * @retval           无
 */
void usb_isr_handler(void)
{
  uint8_t intflag;
  intflag = R8_USB_INT_FG;
  if (intflag & RB_UIF_TRANSFER)
  {
    uint32_t len;
    uint8_t reg = R8_USB_INT_ST & (MASK_UIS_TOKEN | MASK_UIS_ENDP);
    switch (reg & MASK_UIS_TOKEN) /*!< Analyze operation token and endpoint number */
    {
    case UIS_TOKEN_SETUP:
      R8_UEP0_CTRL = RB_UEP_R_TOG | RB_UEP_T_TOG | UEP_R_RES_ACK | UEP_T_RES_NAK;
      /*!< Get setup packet */
      usb_dc_cfg.setup = GET_SETUP_PACKET(usb_dc_cfg.ep_out[0].ep_ram_addr);
      if (usb_dc_cfg.setup.bmRequestType_b.Dir == USB_REQUEST_HOST_TO_DEVICE)
      {
        R8_UEP0_T_LEN = 0;
        EPn_SET_TX_VALID(0);
      }

      len = R8_USB_RX_LEN;
      if (len != 8)
      {
        /*!< Error */
        return;
      }
      else
      {
        /*!< call setup handler */
        usbd_notify_callback(USB_EVENT_SETUP_NOTIFY, NULL);
      }
      break;
    case UIS_TOKEN_IN:
      if (!(reg & 0x0F))
      {
        /*!< EP0 */
        switch (usb_dc_cfg.setup.bmRequestType_b.Dir)
        {
        case USB_REQUEST_DEVICE_TO_HOST:
          /*!< Get */
          R8_UEP0_CTRL ^= RB_UEP_T_TOG;
          /*!< Set ep0 tx nak */
          EPn_SET_TX_NAK(0);
          /*!<  */
          usbd_notify_callback(USB_EVENT_EP0_IN_NOTIFY, NULL);
          break;
        case USB_REQUEST_HOST_TO_DEVICE:
          /*!< set */
          switch (usb_dc_cfg.setup.bRequest)
          {
          case USB_SET_ADDRESS:
            /*!< Fill in the equipment address */
            R8_USB_DEV_AD = (R8_USB_DEV_AD & RB_UDA_GP_BIT) | usb_dc_cfg.address;
            /*!< No data returned T-NACK */
            R8_UEP0_CTRL |= UEP_R_RES_ACK | UEP_T_RES_NAK;
            break;
          default:
            /*!< PRINT("state over \n"); */
            /*!< Normal out state phase */
            R8_UEP0_CTRL |= UEP_R_RES_ACK | UEP_T_RES_NAK;
            break;
          }
          break;
        }
      }
      else
      {
        /*!< Others EP */
        EPn_SET_TX_NAK(reg & 0x0F);
        if ((reg & 0x07) == 0x04)
        {
          R8_UEP4_CTRL ^= RB_UEP_R_TOG;
        }
        usbd_notify_callback(USB_EVENT_EP_IN_NOTIFY, (uint32_t *)(reg & 0x07));
      }
      break;
    case UIS_TOKEN_OUT:
      if (!(reg & 0x0F))
      {
        /*!< EP0 */
        usbd_notify_callback(USB_EVENT_EP0_OUT_NOTIFY, (uint32_t *)&len);
        EPn_SET_RX_VALID(0);
      }
      else
      {
        /*!< Others EP */
        if ((reg & 0x0F) == 0x04)
        {
          R8_UEP4_CTRL ^= RB_UEP_R_TOG;
        }
        usbd_notify_callback(USB_EVENT_EP_OUT_NOTIFY, (uint32_t *)(reg & 0x07));
      }
      break;
    default:
      break;
    }
    R8_USB_INT_FG = RB_UIF_TRANSFER;
  }
  else if (intflag & RB_UIF_BUS_RST)
  {
    /*!< printf("reset \n"); */
    /*!< Configure endpoint 0 */
    struct usbd_endpoint_cfg ep0 = {
        .ep_addr = 0x80,
        .ep_mps = 64,
        .ep_transfer_type = USBD_EP_TYPE_CTRL};
    usbd_ep_open(&ep0);

    ep0.ep_addr = 0x00;
    usbd_ep_open(&ep0);

    R8_USB_DEV_AD = 0;
    R8_UEP0_CTRL = UEP_R_RES_ACK | UEP_T_RES_NAK;
    R8_UEP1_CTRL = UEP_R_RES_ACK | UEP_T_RES_NAK | RB_UEP_AUTO_TOG;
    R8_UEP2_CTRL = UEP_R_RES_ACK | UEP_T_RES_NAK | RB_UEP_AUTO_TOG;
    R8_UEP3_CTRL = UEP_R_RES_ACK | UEP_T_RES_NAK | RB_UEP_AUTO_TOG;
    R8_UEP4_CTRL = UEP_R_RES_ACK | UEP_T_RES_NAK;

    R8_USB_INT_FG = RB_UIF_BUS_RST;

    /*!< Call the reset callback in the protocol stack to register the endpoint callback function */
    usbd_notify_callback(USB_EVENT_RESET, NULL);
  }
  else if (intflag & RB_UIF_SUSPEND)
  {
    if (R8_USB_MIS_ST & RB_UMS_SUSPEND)
    {
      /*!< Suspend */
      usbd_notify_callback(USB_EVENT_SUSPEND, NULL);
    }
    else
    {
      /*!< Wake up */
      usbd_notify_callback(USB_EVENT_RESUME, NULL);
    }
    R8_USB_INT_FG = RB_UIF_SUSPEND;
  }
  else
  {
    R8_USB_INT_FG = intflag;
  }
}
