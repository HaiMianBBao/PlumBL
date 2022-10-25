/** @mainpage  ch582 usb协议栈 port接口示例
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
 * @brief   CH582 端点地址描述
 * ep0:     in: 0x80  out:0x00
 * ep1:     in: 0x81  out:0x01
 * ep2:     in: 0x82  out:0x02
 * ep3:     in: 0x83  out:0x03
 * ep4:     in: 0x84  out:0x04
 * ep5:     in: 0x85  out:0x05
 * ep6:     in: 0x86  out:0x06
 * ep7:     in: 0x87  out:0x07
 */

/* 文件引用 ------------------------------------------------------------------*/
#include "lg_usb_dc.h"
#include "lg_usbd_core.h"
#include "./ch582_regs.h"
#include <stdlib.h>

/**
 * @brief   Related register macro
 */

/*!< ep dir in */
#define EP_DIR_IN 1
/*!< ep dir out */
#define EP_DIR_OUT 0

#define USB0_BASE 0x40008000u
#define USB1_BASE 0x40008400u

#define CH58x_USBFS_DEV ((USB_FS_TypeDef *)USB0_BASE)

/*!< 8-bit value of endpoint control register */
#define EPn_CTRL(epid) \
  *(volatile uint8_t *)(&(CH58x_USBFS_DEV->UEP0_CTRL) + epid * 4 + (epid / 5) * 48)

/*!< The length register value of the endpoint send buffer */
#define EPn_TX_LEN(epid) \
  *(volatile uint8_t *)(&(CH58x_USBFS_DEV->UEP0_T_LEN) + epid * 4 + (epid / 5) * 48)

/*!< Get ep id by epadd */
#define GET_EP_ID(ep_add) (uint8_t)(ep_add & 0x7f)
/*!< Get ep dir by epadd */
#define GET_EP_DIR(ep_add) (uint8_t)(ep_add & 0x80)
/*!< Get interrupt endpoint id */
#define GET_INT_EP_ID \
  (volatile uint8_t)(R8_USB_INT_ST & MASK_UIS_ENDP)
/*!< Get usb interrupt state reg */
#define GET_USB_INT_STATE \
  (volatile uint8_t)(R8_USB_INT_ST & MASK_UIS_TOKEN)
/*!< Read setup packet to use in ep0 in */
#define GET_SETUP_PACKET(data_add) \
  *(struct usb_setup_packet *)data_add
/*!< Set device address // call in set_add state stage */
#define SET_DEVICE_ADDRESS(add) \
  R8_USB_DEV_AD = (R8_USB_DEV_AD & RB_UDA_GP_BIT) | add;
/*!< Set epid ep tx valid */
#define EPn_SET_TX_VALID(epid) \
  EPn_CTRL(epid) = (EPn_CTRL(epid) & ~MASK_UEP_T_RES) | UEP_T_RES_ACK;

/*!< Set epid ep rx valid */
#define EPn_SET_RX_VALID(epid) \
  EPn_CTRL(epid) = (EPn_CTRL(epid) & ~MASK_UEP_R_RES) | UEP_R_RES_ACK;
/*!< Set epid ep tx nak */
#define EPn_SET_TX_NAK(epid) \
  EPn_CTRL(epid) = (EPn_CTRL(epid) & ~MASK_UEP_T_RES) | UEP_T_RES_NAK;
/*!< Set epid ep rx nak */
#define EPn_SET_RX_NAK(epid) \
  EPn_CTRL(epid) = (EPn_CTRL(epid) & ~MASK_UEP_R_RES) | UEP_R_RES_NAK;
/*!< Set epid ep tx stall */
#define EPn_SET_TX_STALL(epid) \
  EPn_CTRL(epid) = (EPn_CTRL(epid) & ~MASK_UEP_T_RES) | UEP_T_RES_STALL
/*!< Set epid ep rx stall */
#define EPn_SET_RX_STALL(epid) \
  EPn_CTRL(epid) = (EPn_CTRL(epid) & ~MASK_UEP_R_RES) | UEP_R_RES_STALL
/*!< Set epid ep tx len */
#define EPn_SET_TX_LEN(epid, len) \
  EPn_TX_LEN(epid) = len
/*!< Set epid ep rx len */
#define EPn_SET_RX_LEN(epid, len)
/*!< Get epid ep rx len */
#define EPn_GET_RX_LEN(epid) \
  R8_USB_RX_LEN

/*!< ep nums */
#define EP_NUMS 8
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
__attribute__((aligned(4))) uint8_t ep5_data_buff[64 + 64];      /*!< ep5_out(64)+ep5_in(64) */
__attribute__((aligned(4))) uint8_t ep6_data_buff[64 + 64];      /*!< ep6_out(64)+ep6_in(64) */
__attribute__((aligned(4))) uint8_t ep7_data_buff[64 + 64];      /*!< ep7_out(64)+ep7_in(64) */

uint8_t *EP0_RAM_Addr;
uint8_t *EP1_RAM_Addr;
uint8_t *EP2_RAM_Addr;
uint8_t *EP3_RAM_Addr;
uint8_t *EP5_RAM_Addr;
uint8_t *EP6_RAM_Addr;
uint8_t *EP7_RAM_Addr;

#define pEP0_DataBuf (EP0_RAM_Addr)
#define pEP1_OUT_DataBuf (EP1_RAM_Addr)
#define pEP1_IN_DataBuf (EP1_RAM_Addr + 64)
#define pEP2_OUT_DataBuf (EP2_RAM_Addr)
#define pEP2_IN_DataBuf (EP2_RAM_Addr + 64)
#define pEP3_OUT_DataBuf (EP3_RAM_Addr)
#define pEP3_IN_DataBuf (EP3_RAM_Addr + 64)
#define pEP4_OUT_DataBuf (EP0_RAM_Addr + 64)
#define pEP4_IN_DataBuf (EP0_RAM_Addr + 128)
/*!<  */
#define pEP5_OUT_DataBuf (EP5_RAM_Addr)
#define pEP5_IN_DataBuf (EP5_RAM_Addr + 64)
#define pEP6_OUT_DataBuf (EP6_RAM_Addr)
#define pEP6_IN_DataBuf (EP6_RAM_Addr + 64)
#define pEP7_OUT_DataBuf (EP7_RAM_Addr)
#define pEP7_IN_DataBuf (EP7_RAM_Addr + 64)

/**
 * @brief   Endpoint information structure
 */
typedef struct _usbd_ep_info
{
  uint8_t mps;          /*!< Maximum packet length of endpoint */
  uint8_t eptype;       /*!< Endpoint Type */
  uint8_t *ep_ram_addr; /*!< Endpoint buffer address */
} usbd_ep_info;

/*!< ch582 usb */
static struct _ch582_core_prvi
{
  uint8_t address; /*!< Address */
  usbd_ep_info ep_in[EP_NUMS];
  usbd_ep_info ep_out[EP_NUMS];
  struct usb_setup_packet setup;
} usb_dc_cfg;

/**
 * @brief            Set address
 * @pre              None
 * @param[in]        address ：8-bit valid address
 * @retval           >=0 success otherwise failure
 */
int usbd_set_address(const uint8_t address)
{
  if (address == 0)
  {
    CH58x_USBFS_DEV->USB_DEV_AD = (CH58x_USBFS_DEV->USB_DEV_AD & 0x80) | address;
  }
  usb_dc_cfg.address = address;
  return 0;
}

/**
 * @brief            Open endpoint
 * @pre              None
 * @param[in]        ep_cfg : Endpoint configuration structure pointer
 * @retval           >=0 success otherwise failure
 */
int usbd_ep_open(const struct usbd_endpoint_cfg *ep_cfg)
{
  /*!< ep id */
  uint8_t epid = GET_EP_ID(ep_cfg->ep_addr);
  /*!< ep dir */
  bool dir = GET_EP_DIR(ep_cfg->ep_addr);
  /*!< ep max packet length */
  uint8_t mps = ep_cfg->ep_mps;
  /*!< update ep max packet length */
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
 * @brief            Close endpoint
 * @pre              None
 * @param[in]        ep ： Endpoint address
 * @retval           >=0 success otherwise failure
 */
int usbd_ep_close(const uint8_t ep)
{
  return 0;
}

/**
 * @brief            Write send buffer
 * @pre              None
 * @param[in]        ep ： Endpoint address
 * @param[in]        data ： First address of data buffer to be written
 * @param[in]        data_len ： Write total length
 * @param[in]        ret_bytes ： Length actually written
 * @retval           >=0 success otherwise failure
 */
int usbd_ep_write(const uint8_t ep, const uint8_t *data, uint32_t data_len, uint32_t *ret_bytes)
{
  /*!< ep id */
  uint8_t epid = GET_EP_ID(ep);
  /*!< real write byte nums */
  uint32_t real_wt_nums = 0;
  /*!< ep mps */
  uint8_t ep_mps = usb_dc_cfg.ep_in[epid].mps;
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
 * @brief            Read receive buffer
 * @pre              None
 * @param[in]        ep ： Endpoint address
 * @param[in]        data ： Read the first address of the buffer where the data is stored
 * @param[in]        max_data_len ： Maximum readout length
 * @param[in]        read_bytes ： Actual read length
 * @retval           >=0 success otherwise failure
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

  /*!< Special treatment for ch582 //start */
  if (epid)
  {
    real_rd_nums = EPn_GET_RX_LEN(epid);
    real_rd_nums = MIN(real_rd_nums, max_data_len);
  }
  else
  {
    /*!< ep0 */
    /*!< For ch582, when reading the length of USB received data during idle period, this data is uncertain */
    real_rd_nums = max_data_len;
  }
  /*!< Special treatment for ch582 //end */

  /*!< read buff */
  memcpy(data, usb_dc_cfg.ep_out[epid].ep_ram_addr, real_rd_nums);
  if (read_bytes != NULL)
  {
    *read_bytes = real_rd_nums;
  }

  return 0;
}

/**
 * @brief            Endpoint setting stall
 * @pre              None
 * @param[in]        ep ： Endpoint address
 * @retval           >=0 success otherwise failure
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
 * @brief            Endpoint clear stall
 * @pre              None
 * @param[in]        ep ： Endpoint address
 * @retval           >=0 success otherwise failure
 */
int usbd_ep_clear_stall(const uint8_t ep)
{
  int ret;
  switch (ep)
  {
  case 0x82:
    CH58x_USBFS_DEV->UEP2_CTRL = (CH58x_USBFS_DEV->UEP2_CTRL & ~(RB_UEP_T_TOG | MASK_UEP_T_RES)) | UEP_T_RES_NAK;
    ret = 0;
    break;
  case 0x02:
    CH58x_USBFS_DEV->UEP2_CTRL = (CH58x_USBFS_DEV->UEP2_CTRL & ~(RB_UEP_R_TOG | MASK_UEP_R_RES)) | UEP_R_RES_ACK;
    ret = 0;
    break;
  case 0x81:
    CH58x_USBFS_DEV->UEP1_CTRL = (CH58x_USBFS_DEV->UEP1_CTRL & ~(RB_UEP_T_TOG | MASK_UEP_T_RES)) | UEP_T_RES_NAK;
    ret = 0;
    break;
  case 0x01:
    CH58x_USBFS_DEV->UEP1_CTRL = (CH58x_USBFS_DEV->UEP1_CTRL & ~(RB_UEP_R_TOG | MASK_UEP_R_RES)) | UEP_R_RES_ACK;
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
 * @brief            Check endpoint status
 * @pre              None
 * @param[in]        ep ： Endpoint address
 * @param[out]       stalled ： Outgoing endpoint status
 * @retval           >=0 success otherwise failure
 */
int usbd_ep_get_stall(const uint8_t ep, uint8_t *stalled)
{
  return 0;
}

/**
 * @brief            USB initialization
 * @pre              None
 * @param[in]        None
 * @retval           >=0 success otherwise failure
 */
int usb_dc_init(void)
{
  EP0_RAM_Addr = ep0_data_buff;
  EP1_RAM_Addr = ep1_data_buff;
  EP2_RAM_Addr = ep2_data_buff;
  EP3_RAM_Addr = ep3_data_buff;

  EP5_RAM_Addr = ep5_data_buff;
  EP6_RAM_Addr = ep6_data_buff;
  EP7_RAM_Addr = ep7_data_buff;

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

  usb_dc_cfg.ep_in[5].ep_ram_addr = pEP5_IN_DataBuf;
  usb_dc_cfg.ep_out[5].ep_ram_addr = pEP5_OUT_DataBuf;

  usb_dc_cfg.ep_in[6].ep_ram_addr = pEP6_IN_DataBuf;
  usb_dc_cfg.ep_out[6].ep_ram_addr = pEP6_OUT_DataBuf;

  usb_dc_cfg.ep_in[7].ep_ram_addr = pEP7_IN_DataBuf;
  usb_dc_cfg.ep_out[7].ep_ram_addr = pEP7_OUT_DataBuf;

  /*!< Set the mode first and cancel RB_UC_CLR_ALL */
  CH58x_USBFS_DEV->USB_CTRL = 0x00;
  CH58x_USBFS_DEV->UEP4_1_MOD = RB_UEP4_RX_EN | RB_UEP4_TX_EN | RB_UEP1_RX_EN | RB_UEP1_TX_EN; /*!< EP4 OUT+IN   EP1 OUT+IN */
  CH58x_USBFS_DEV->UEP2_3_MOD = RB_UEP2_RX_EN | RB_UEP2_TX_EN | RB_UEP3_RX_EN | RB_UEP3_TX_EN; /*!< EP2 OUT+IN   EP3 OUT+IN */
  CH58x_USBFS_DEV->UEP567_MOD = RB_UEP5_RX_EN | RB_UEP5_TX_EN | RB_UEP6_RX_EN | RB_UEP6_TX_EN | RB_UEP7_RX_EN | RB_UEP7_TX_EN; /*!< EP5 EP6 EP7   OUT+IN */

  CH58x_USBFS_DEV->UEP0_DMA = (uint16_t)(uint32_t)EP0_RAM_Addr;
  CH58x_USBFS_DEV->UEP1_DMA = (uint16_t)(uint32_t)EP1_RAM_Addr;
  CH58x_USBFS_DEV->UEP2_DMA = (uint16_t)(uint32_t)EP2_RAM_Addr;
  CH58x_USBFS_DEV->UEP3_DMA = (uint16_t)(uint32_t)EP3_RAM_Addr;

  CH58x_USBFS_DEV->UEP5_DMA = (uint16_t)(uint32_t)EP5_RAM_Addr;
  CH58x_USBFS_DEV->UEP6_DMA = (uint16_t)(uint32_t)EP6_RAM_Addr;
  CH58x_USBFS_DEV->UEP7_DMA = (uint16_t)(uint32_t)EP7_RAM_Addr;

  CH58x_USBFS_DEV->UEP0_CTRL = UEP_R_RES_ACK | UEP_T_RES_NAK;
  CH58x_USBFS_DEV->UEP1_CTRL = UEP_R_RES_ACK | UEP_T_RES_NAK | RB_UEP_AUTO_TOG;
  CH58x_USBFS_DEV->UEP2_CTRL = UEP_R_RES_ACK | UEP_T_RES_NAK | RB_UEP_AUTO_TOG;
  CH58x_USBFS_DEV->UEP3_CTRL = UEP_R_RES_ACK | UEP_T_RES_NAK | RB_UEP_AUTO_TOG;
  CH58x_USBFS_DEV->UEP4_CTRL = UEP_R_RES_ACK | UEP_T_RES_NAK;

  CH58x_USBFS_DEV->UEP5_CTRL = UEP_R_RES_ACK | UEP_T_RES_NAK | RB_UEP_AUTO_TOG;
  CH58x_USBFS_DEV->UEP6_CTRL = UEP_R_RES_ACK | UEP_T_RES_NAK | RB_UEP_AUTO_TOG;
  CH58x_USBFS_DEV->UEP7_CTRL = UEP_R_RES_ACK | UEP_T_RES_NAK | RB_UEP_AUTO_TOG;

  CH58x_USBFS_DEV->USB_DEV_AD = 0x00;

  /*!< Start the USB device and DMA, and automatically return to NAK before the interrupt flag is cleared during the interrupt */
  CH58x_USBFS_DEV->USB_CTRL = RB_UC_DEV_PU_EN | RB_UC_INT_BUSY | RB_UC_DMA_EN;
  if ((uint32_t)&(CH58x_USBFS_DEV->USB_CTRL) == (uint32_t)USB0_BASE)
  {
    /*!< USB0 */
    R16_PIN_ANALOG_IE |= RB_PIN_USB_IE | RB_PIN_USB_DP_PU;
  }
  else if ((uint32_t)&(CH58x_USBFS_DEV->USB_CTRL) == (uint32_t)USB1_BASE)
  {
    /*!< USB1 */
    R16_PIN_ANALOG_IE |= RB_PIN_USB2_IE | RB_PIN_USB2_DP_PU;
  }

  CH58x_USBFS_DEV->USB_INT_FG = 0xff; /*!< Clear interrupt flag */
  CH58x_USBFS_DEV->UDEV_CTRL = RB_UD_PD_DIS | RB_UD_PORT_EN; /*!< Allow USB port */
  CH58x_USBFS_DEV->USB_INT_EN = RB_UIE_SUSPEND | RB_UIE_BUS_RST | RB_UIE_TRANSFER;
  return 0;
}

/**
 * @brief            USB interrupt processing function
 * @pre              None
 * @param[in]        None
 * @retval           None
 */
void usb_isr_handler(void)
{
  volatile uint8_t intflag = 0;
  intflag = CH58x_USBFS_DEV->USB_INT_FG;

  if (intflag & RB_UIF_TRANSFER)
  {
    if ((CH58x_USBFS_DEV->USB_INT_ST & MASK_UIS_TOKEN) != MASK_UIS_TOKEN) /*!< Non idle */
    {
      switch (CH58x_USBFS_DEV->USB_INT_ST & (MASK_UIS_TOKEN | MASK_UIS_ENDP))
      {
      case UIS_TOKEN_IN:
        /**
         * IN  The host takes away the data that has been stored in FIFO
         */
        switch (usb_dc_cfg.setup.bmRequestType_b.Dir)
        {
        case USB_REQUEST_DEVICE_TO_HOST:
          /*!< Get */
          CH58x_USBFS_DEV->UEP0_CTRL ^= RB_UEP_T_TOG;
          /**
           * Here is to take away the last data, and the IN interrupt will be triggered only after it is successfully taken away.
           * Therefore, the status of the in endpoint is set to NAK here. If there is data transmission,
           * the endpoint status will be set to ack again in the in handler of EP0.
           */
          EPn_SET_TX_NAK(0);
          usbd_notify_callback(USB_EVENT_EP0_IN_NOTIFY, NULL);
          break;
        case USB_REQUEST_HOST_TO_DEVICE:
          /*!< Set */
          switch (usb_dc_cfg.setup.bRequest)
          {
          case USB_SET_ADDRESS:
            /*!< Fill in the equipment address */
            CH58x_USBFS_DEV->USB_DEV_AD = (CH58x_USBFS_DEV->USB_DEV_AD & RB_UDA_GP_BIT) | usb_dc_cfg.address;
            /*!< No data returned T-NACK */
            /**
             * When the host sends the in token,
             * the data in FIFO has been taken away.
             * Ch58x USB IP needs to manually set the status of the in endpoint to NAK
             */
            CH58x_USBFS_DEV->UEP0_CTRL |= UEP_R_RES_ACK | UEP_T_RES_NAK;
            break;
          default:
            /*!< PRINT("state over \n"); */
            /*!< Normal out state phase */
            /**
             * The host has taken away 0-length packets.
             * Here, you only need to set the status of the endpoint to NAK
             */
            CH58x_USBFS_DEV->UEP0_CTRL |= UEP_R_RES_ACK | UEP_T_RES_NAK;
            break;
          }
          break;
        }
        break;
      case UIS_TOKEN_OUT | 0:
        /*!< ep0 out */
        CH58x_USBFS_DEV->UEP0_CTRL ^= RB_UEP_R_TOG;
        usbd_notify_callback(USB_EVENT_EP0_OUT_NOTIFY, NULL);
        EPn_SET_RX_VALID(0);
        break;
      case UIS_TOKEN_OUT | 1:
        usbd_notify_callback(USB_EVENT_EP_OUT_NOTIFY, (uint32_t *)(1 & 0x7f));
        break;
      case UIS_TOKEN_IN | 1:
        EPn_SET_TX_NAK(1);
        usbd_notify_callback(USB_EVENT_EP_IN_NOTIFY, (uint32_t *)(1 | 0x80));
        break;
      case UIS_TOKEN_OUT | 2:
        if (R8_USB_INT_ST & RB_UIS_TOG_OK)
        {
          /*!< Out of sync packets will be discarded */
          usbd_notify_callback(USB_EVENT_EP_OUT_NOTIFY, (uint32_t *)(2 & 0x7f));
        }
        break;
      case UIS_TOKEN_IN | 2:
        EPn_SET_TX_NAK(2);
        usbd_notify_callback(USB_EVENT_EP_IN_NOTIFY, (uint32_t *)(2 | 0x80));
        break;
      case UIS_TOKEN_OUT | 3:
        if (R8_USB_INT_ST & RB_UIS_TOG_OK)
        {
          /*!< Out of sync packets will be discarded */
          usbd_notify_callback(USB_EVENT_EP_OUT_NOTIFY, (uint32_t *)(3 & 0x7f));
        }
        break;
      case UIS_TOKEN_IN | 3:
        EPn_SET_TX_NAK(3);
        usbd_notify_callback(USB_EVENT_EP_IN_NOTIFY, (uint32_t *)(3 | 0x80));
        break;
      case UIS_TOKEN_OUT | 4:
        if (CH58x_USBFS_DEV->USB_INT_ST & RB_UIS_TOG_OK)
        {
          CH58x_USBFS_DEV->UEP4_CTRL ^= RB_UEP_R_TOG;
          usbd_notify_callback(USB_EVENT_EP_OUT_NOTIFY, (uint32_t *)(4 & 0x7f));
        }
        break;
      case UIS_TOKEN_IN | 4:
        CH58x_USBFS_DEV->UEP4_CTRL ^= RB_UEP_T_TOG;
        EPn_SET_TX_NAK(4);
        usbd_notify_callback(USB_EVENT_EP_IN_NOTIFY, (uint32_t *)(4 | 0x80));
        break;
      case UIS_TOKEN_OUT | 5:
        if (R8_USB_INT_ST & RB_UIS_TOG_OK)
        {
          usbd_notify_callback(USB_EVENT_EP_OUT_NOTIFY, (uint32_t *)(5 & 0x7f));
        }
        break;
      case UIS_TOKEN_IN | 5:
        if (R8_USB_INT_ST & RB_UIS_TOG_OK)
        {
          EPn_SET_TX_NAK(5);
          usbd_notify_callback(USB_EVENT_EP_IN_NOTIFY, (uint32_t *)(5 | 0x80));
        }
        break;
      case UIS_TOKEN_OUT | 6:
        if (R8_USB_INT_ST & RB_UIS_TOG_OK)
        {
          usbd_notify_callback(USB_EVENT_EP_OUT_NOTIFY, (uint32_t *)(6 & 0x7f));
        }
        break;
      case UIS_TOKEN_IN | 6:
        if (R8_USB_INT_ST & RB_UIS_TOG_OK)
        {
          EPn_SET_TX_NAK(6);
          usbd_notify_callback(USB_EVENT_EP_IN_NOTIFY, (uint32_t *)(6 | 0x80));
        }
        break;
      case UIS_TOKEN_OUT | 7:
        if (R8_USB_INT_ST & RB_UIS_TOG_OK)
        {
          usbd_notify_callback(USB_EVENT_EP_OUT_NOTIFY, (uint32_t *)(7 & 0x7f));
        }
        break;
      case UIS_TOKEN_IN | 7:
        if (R8_USB_INT_ST & RB_UIS_TOG_OK)
        {
          EPn_SET_TX_NAK(7);
          usbd_notify_callback(USB_EVENT_EP_IN_NOTIFY, (uint32_t *)(7 | 0x80));
        }
        break;
      default:
        break;
      }
      CH58x_USBFS_DEV->USB_INT_FG = RB_UIF_TRANSFER;
    }

    if (CH58x_USBFS_DEV->USB_INT_ST & RB_UIS_SETUP_ACT)
    {
      /*!< Setup */
      /**
       * Setup the device must respond with ACK, and the data phase is DATA1
       */
      CH58x_USBFS_DEV->UEP0_CTRL = RB_UEP_R_TOG | RB_UEP_T_TOG | UEP_R_RES_ACK | UEP_T_RES_NAK;
      /*!< get setup packet */
      usb_dc_cfg.setup = GET_SETUP_PACKET(usb_dc_cfg.ep_out[0].ep_ram_addr);
      if (usb_dc_cfg.setup.bmRequestType_b.Dir == USB_REQUEST_HOST_TO_DEVICE)
      {
        /**
         * Ep0 The next in must be the status stage.
         * The device must reply to the host data 0 length packet.
         * Here, set the transmission length to 0 and the transmission status to ACK,
         * and wait for the host to send the in token to retrieve
         */
        CH58x_USBFS_DEV->UEP0_T_LEN = 0;
        EPn_SET_TX_VALID(0);
      }
      usbd_notify_callback(USB_EVENT_SETUP_NOTIFY, NULL);
      /*!< enable ep0 rx */
      EPn_SET_RX_VALID(0);
      CH58x_USBFS_DEV->USB_INT_FG = RB_UIF_TRANSFER;
    }
  }
  else if (intflag & RB_UIF_BUS_RST)
  {
    /*!< Reset */
    /*!< Configure endpoint 0 */
    struct usbd_endpoint_cfg ep0 = {
        .ep_addr = 0x80,
        .ep_mps = 64,
        .ep_transfer_type = USBD_EP_TYPE_CTRL};
    usbd_ep_open(&ep0);

    ep0.ep_addr = 0x00;
    usbd_ep_open(&ep0);

    CH58x_USBFS_DEV->USB_DEV_AD = 0;
    CH58x_USBFS_DEV->UEP0_CTRL = UEP_R_RES_ACK | UEP_T_RES_NAK;
    CH58x_USBFS_DEV->UEP1_CTRL = UEP_R_RES_ACK | UEP_T_RES_NAK | RB_UEP_AUTO_TOG;
    CH58x_USBFS_DEV->UEP2_CTRL = UEP_R_RES_ACK | UEP_T_RES_NAK | RB_UEP_AUTO_TOG;
    CH58x_USBFS_DEV->UEP3_CTRL = UEP_R_RES_ACK | UEP_T_RES_NAK | RB_UEP_AUTO_TOG;

    CH58x_USBFS_DEV->UEP5_CTRL = UEP_R_RES_ACK | UEP_T_RES_NAK | RB_UEP_AUTO_TOG;
    CH58x_USBFS_DEV->UEP6_CTRL = UEP_R_RES_ACK | UEP_T_RES_NAK | RB_UEP_AUTO_TOG;
    CH58x_USBFS_DEV->UEP7_CTRL = UEP_R_RES_ACK | UEP_T_RES_NAK | RB_UEP_AUTO_TOG;

    CH58x_USBFS_DEV->USB_INT_FG = RB_UIF_BUS_RST;

    /*!< Call the reset callback in the protocol stack to register the endpoint callback function */
    usbd_notify_callback(USB_EVENT_RESET, NULL);
  }
  else if (intflag & RB_UIF_SUSPEND)
  {
    if (CH58x_USBFS_DEV->USB_MIS_ST & RB_UMS_SUSPEND)
    {
      /*!< Suspend */
      usbd_notify_callback(USB_EVENT_SUSPEND, NULL);
    }
    else
    {
      /*!< Wake up */
      usbd_notify_callback(USB_EVENT_RESUME, NULL);
    }

    CH58x_USBFS_DEV->USB_INT_FG = RB_UIF_SUSPEND;
  }
  else
  {
    CH58x_USBFS_DEV->USB_INT_FG = intflag;
  }
}
