/** @mainpage  nrf52840 usb协议栈 port接口示例
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
/* 文件引用 ------------------------------------------------------------------*/
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

#include "lg_usb_dc.h"
#include "lg_usbd_core.h"
#include "./nrf52840_regs.h"


/**
 * @brief   Related register macro
 */

/*!< ep dir in */
#define EP_DIR_IN 1
/*!< ep dir out */
#define EP_DIR_OUT 0
/*!< get ep id by epadd */
#define GET_EP_ID(ep_add) (uint8_t)(ep_add & 0x7f)

/*!< get ep dir by epadd */
#define GET_EP_DIR(ep_add) (uint8_t)(ep_add & 0x80)

/*!< ep nums */
#define EP_NUMS 9
/*!< ep mps */
#define EP_MPS 64

/*!< get interrupt endpoint id */
#define GET_INT_EP_ID

/*!< get usb interrupt state reg */
#define GET_USB_INT_STATE

/*!< read setup packet to use in ep0 in */
//#define GET_SETUP_PACKET()

/*!< set device address // call in set_add state stage */
#define SET_DEVICE_ADDRESS(add)

/*!< set epid ep tx valid */
#define EPn_SET_TX_VALID(epid)

/*!< set epid ep rx valid */
#define EPn_SET_RX_VALID(epid) \
  NRF_USBD->SIZE.EPOUT[epid] = EP_MPS;
/*!< set epid ep tx nak */
#define EPn_SET_TX_NAK(epid)

/*!< set epid ep rx nak */
#define EPn_SET_RX_NAK(epid)

/*!< set epid ep tx stall */
#define EPn_SET_TX_STALL(epid)

/*!< set epid ep rx stall */
#define EPn_SET_RX_STALL(epid)

/*!< clear epid ep rx stall */
#define EPn_CLEAR_RX_STALL(epid)

/*!< clear epid ep tx stall */
#define EPn_CLEAR_TX_STALL(epid)

/*!< set epid ep tx len */
#define EPn_SET_TX_LEN(epid, len)

/*!< set epid ep rx len */
#define EPn_SET_RX_LEN(epid, len)

/*!< get epid ep rx len */
#define EPn_GET_RX_LEN(epid)

/*!< set fifo address */
#define EPn_SET_FIFO_ADD(epid, address)

/*!< nrf5x special */
#define EP_ISO_NUM 8

/*!< Peripheral address base */
#define NRF_USBD_BASE 0x40027000UL
#define NRF_USBD ((NRF_USBD_Type *)NRF_USBD_BASE)
/**
 * @brief   Endpoint information structure
 */
typedef struct _usbd_ep_info
{
  uint8_t mps;          /*!< Maximum packet length of endpoint */
  uint8_t eptype;       /*!< Endpoint Type */
  uint8_t *ep_ram_addr; /*!< Endpoint buffer address */
  /*!< ----------- */

  /*!< Other endpoint parameters that may be used */
  uint8_t *buffer;
  uint16_t total_len;
  volatile uint16_t actual_len;

  // nRF will auto accept OUT packet after DMA is done
  // indicate packet is already ACK
  volatile bool data_received;

  // Set to true when data was transferred from RAM to ISO IN output buffer.
  // New data can be put in ISO IN output buffer after SOF.
  bool iso_in_transfer_ready;
} usbd_ep_info;

/*!< nrf52840 usb */
static struct _nrf52840_core_prvi
{
  uint8_t address;               /*!< address */
  usbd_ep_info ep_in[EP_NUMS];   /*!< ep in */
  usbd_ep_info ep_out[EP_NUMS];  /*!< ep out */
  struct usb_setup_packet setup; /*!< Setup package that may be used in interrupt processing (outside the protocol stack) */

  volatile bool dma_running;
} usb_dc_cfg;

/*!< Ep buffer */
uint8_t ep0_tx_buff[EP_MPS];
uint8_t ep0_rx_buff[EP_MPS];
uint8_t *ep0_tx_buf_p = ep0_tx_buff;
uint8_t *ep0_rx_buf_p = ep0_rx_buff;

uint8_t ep1_tx_buff[EP_MPS];
uint8_t ep1_rx_buff[EP_MPS];
uint8_t *ep1_tx_buf_p = ep1_tx_buff;
uint8_t *ep1_rx_buf_p = ep1_rx_buff;

uint8_t ep2_tx_buff[EP_MPS];
uint8_t ep2_rx_buff[EP_MPS];
uint8_t *ep2_tx_buf_p = ep2_tx_buff;
uint8_t *ep2_rx_buf_p = ep2_rx_buff;

uint8_t ep3_tx_buff[EP_MPS];
uint8_t ep3_rx_buff[EP_MPS];
uint8_t *ep3_tx_buf_p = ep3_tx_buff;
uint8_t *ep3_rx_buf_p = ep3_rx_buff;

uint8_t ep4_tx_buff[EP_MPS];
uint8_t ep4_rx_buff[EP_MPS];
uint8_t *ep4_tx_buf_p = ep4_tx_buff;
uint8_t *ep4_rx_buf_p = ep4_rx_buff;

uint8_t ep5_tx_buff[EP_MPS];
uint8_t ep5_rx_buff[EP_MPS];
uint8_t *ep5_tx_buf_p = ep5_tx_buff;
uint8_t *ep5_rx_buf_p = ep5_rx_buff;

uint8_t ep6_tx_buff[EP_MPS];
uint8_t ep6_rx_buff[EP_MPS];
uint8_t *ep6_tx_buf_p = ep6_tx_buff;
uint8_t *ep6_rx_buf_p = ep6_rx_buff;

uint8_t ep7_tx_buff[EP_MPS];
uint8_t ep7_rx_buff[EP_MPS];
uint8_t *ep7_tx_buf_p = ep7_tx_buff;
uint8_t *ep7_rx_buf_p = ep7_rx_buff;


static inline void GET_SETUP_PACKET(struct usb_setup_packet *setup)
{
  setup->bmRequestType = (uint8_t)(NRF_USBD->BMREQUESTTYPE);
  setup->bRequest = (uint8_t)(NRF_USBD->BREQUEST);
  setup->wIndex = (uint16_t)(NRF_USBD->WINDEXL | ((NRF_USBD->WINDEXH) << 8));
  setup->wLength = (uint16_t)(NRF_USBD->WLENGTHL | ((NRF_USBD->WLENGTHH) << 8));
  setup->wValue = (uint16_t)(NRF_USBD->WVALUEL | ((NRF_USBD->WVALUEH) << 8));
}

/**
  * @brief            Set tx easydma
  * @pre              None
  * @param[in]        ep      End point address  
  * @param[in]        ptr     Data ram ptr
  * @param[in]        maxcnt  Max length
  * @retval           None
  */
static void nrf_usbd_ep_easydma_set_tx(uint8_t ep, uint32_t ptr, uint32_t maxcnt)
{
  uint8_t epid = GET_EP_ID(ep);
  if (epid == EP_ISO_NUM)
  {
    NRF_USBD->ISOIN.PTR = ptr;
    NRF_USBD->ISOIN.MAXCNT = maxcnt;
    return;
  }
  NRF_USBD->EPIN[epid].PTR = ptr;
  NRF_USBD->EPIN[epid].MAXCNT = maxcnt;
}

/**
  * @brief            Set rx easydma
  * @pre              None
  * @param[in]        ep      End point address  
  * @param[in]        ptr     Data ram ptr
  * @param[in]        maxcnt  Max length
  * @retval           None
  */
static void nrf_usbd_ep_easydma_set_rx(uint8_t ep, uint32_t ptr, uint32_t maxcnt)
{
  uint8_t epid = GET_EP_ID(ep);
  if (epid == EP_ISO_NUM)
  {
    NRF_USBD->ISOOUT.PTR = ptr;
    NRF_USBD->ISOOUT.MAXCNT = maxcnt;
    return;
  }
  NRF_USBD->EPOUT[epid].PTR = ptr;
  NRF_USBD->EPOUT[epid].MAXCNT = maxcnt;
}

/**
 * @brief            Check if we are in ISR
 * @pre              None
 * @param[in]        None
 * @retval           true : in  false : no in
 */
static inline bool is_in_isr(void)
{
  return (SCB->ICSR & SCB_ICSR_VECTACTIVE_Msk) ? true : false;
}

static inline uint32_t NVIC_GetEnableIRQ(IRQn_Type IRQn)
{
  if ((int32_t)(IRQn) >= 0)
  {
    return ((uint32_t)(((NVIC->ISER[(((uint32_t)(int32_t)IRQn) >> 5UL)] & (1UL << (((uint32_t)(int32_t)IRQn) & 0x1FUL))) != 0UL) ? 1UL : 0UL));
  }
  else
  {
    return (0U);
  }
}

static void start_dma(volatile uint32_t *reg_startep)
{
  _dcd.dma_running = true;

  (*reg_startep) = 1;
  __ISB();
  __DSB();

  // TASKS_EP0STATUS, TASKS_EP0RCVOUT seem to need EasyDMA to be available
  // However these don't trigger any DMA transfer and got ENDED event subsequently
  // Therefore dma_pending is corrected right away
  if ((reg_startep == &NRF_USBD->TASKS_EP0STATUS) || (reg_startep == &NRF_USBD->TASKS_EP0RCVOUT))
  {
    usb_dc_cfg.dma_running = false;
  }
}

// only 1 EasyDMA can be active at any time
// TODO use Cortex M4 LDREX and STREX command (atomic) to have better mutex access to EasyDMA
// since current implementation does not 100% guarded against race condition
static void edpt_dma_start(volatile uint32_t *reg_startep)
{
  // Called in critical section i.e within USB ISR, or USB/Global interrupt disabled
  if (is_in_isr() || __get_PRIMASK() || !NVIC_GetEnableIRQ(USBD_IRQn))
  {
    if (usb_dc_cfg.dma_running)
    {
      /*!< 当前在 */
      // use usbd task to defer later
      // TODO need to do something
      // usbd_defer_func((osal_task_func_t)edpt_dma_start, (void *)(uintptr_t)reg_startep, true);
    }
    else
    {
      start_dma(reg_startep);
    }
  }
  else
  {
    // Called in non-critical thread-mode, should be 99% of the time.
    // Should be safe to blocking wait until previous DMA transfer complete
    uint8_t const rhport = 0;
    bool started = false;
    // osal_mutex_lock(dcd_mutex, OSAL_TIMEOUT_WAIT_FOREVER);
    //  TODO
    while (!started)
    {
      // LDREX/STREX may be needed in form of std atomic (required C11) or
      // use osal mutex to guard against multiple core MCUs such as nRF53
      NVIC_DisableIRQ(USBD_IRQn);

      if (!usb_dc_cfg.dma_running)
      {
        start_dma(reg_startep);
        started = true;
      }

      NVIC_EnableIRQ(USBD_IRQn);
    }
    // TODO
    // osal_mutex_unlock(dcd_mutex);
  }
}

// Start DMA to move data from Endpoint -> RAM
static void xact_out_dma(uint8_t epnum)
{
  usbd_ep_info *ep = &(usb_dc_cfg.ep_out[epnum]);

  uint32_t xact_len;

  if (epnum == EP_ISO_NUM)
  {
    xact_len = NRF_USBD->SIZE.ISOOUT;
    // If ZERO bit is set, ignore ISOOUT length
    if (xact_len & USBD_SIZE_ISOOUT_ZERO_Msk)
    {
      xact_len = 0;
    }
    else
    {
      // Trigger DMA move data from Endpoint -> SRAM
      NRF_USBD->ISOOUT.PTR = (uint32_t)ep->buffer;
      NRF_USBD->ISOOUT.MAXCNT = xact_len;

      edpt_dma_start(&NRF_USBD->TASKS_STARTISOOUT);
    }
  }
  else
  {
    // limit xact len to remaining length
    if (NRF_USBD->SIZE.EPOUT[epnum] < (ep->total_len - ep->actual_len))
    {
      xact_len = NRF_USBD->SIZE.EPOUT[epnum];
    }
    else
    {
      xact_len = ep->total_len - ep->actual_len;
    }
    // Trigger DMA move data from Endpoint -> SRAM
    NRF_USBD->EPOUT[epnum].PTR = (uint32_t)ep->buffer;
    NRF_USBD->EPOUT[epnum].MAXCNT = xact_len;

    edpt_dma_start(&NRF_USBD->TASKS_STARTEPOUT[epnum]);
  }
}


static void xact_in_dma(uint8_t epnum)
{
  usbd_ep_info *xfer = &(usb_dc_cfg.ep_in[epnum]);

  // Each transaction is up to Max Packet Size
  uint16_t const xact_len = tu_min16(xfer->total_len - xfer->actual_len, xfer->mps);

  NRF_USBD->EPIN[epnum].PTR    = (uint32_t) xfer->buffer;
  NRF_USBD->EPIN[epnum].MAXCNT = xact_len;

  edpt_dma_start(&NRF_USBD->TASKS_STARTEPIN[epnum]);
}

/**
 * @brief            Set address
 * @pre              None
 * @param[in]        address 8-bit valid address
 * @retval           >=0 success otherwise failure
 */
int usbd_set_address(const uint8_t address)
{
  if (address == 0)
  {
    /*!< init 0 address */
    SET_DEVICE_ADDRESS(address);
  }
  else
  {
    /*!< For non-0 addresses, write the address to the register in the state phase of setting the address */
  }

  NRF_USBD->EVENTCAUSE |= NRF_USBD->EVENTCAUSE;
  NRF_USBD->EVENTS_USBEVENT = 0;

  NRF_USBD->INTENSET = USBD_INTEN_USBEVENT_Msk;
  //nothing to do, handled by hardware; but don't STALL
  usb_dc_cfg.address = address;
  return 1;
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
  if (dir == EP_DIR_IN)
  {
    /*!< in */
    usb_dc_cfg.ep_in[epid].mps = mps;
    usb_dc_cfg.ep_in[epid].eptype = ep_cfg->ep_transfer_type;
    /*!< open ep */
    if (ep_cfg->ep_transfer_type != USB_DC_EP_ISOCHRONOUS)
    {
      /*!< Enable endpoint interrupt */
      NRF_USBD->INTENSET = (1 << (USBD_INTEN_ENDEPIN0_Pos + epid));
      /*!< Enable the in endpoint host to respond when sending in token */
      NRF_USBD->EPINEN |= (1 << (epid));
      __ISB();
      __DSB();
    }
    else
    {
      NRF_USBD->EVENTS_ENDISOIN = 0;

      // SPLIT ISO buffer when ISO OUT endpoint is already opened.
      if (usb_dc_cfg.ep_out[EP_ISO_NUM].mps)
        NRF_USBD->ISOSPLIT = USBD_ISOSPLIT_SPLIT_HalfIN;

      // Clear SOF event in case interrupt was not enabled yet.
      if ((NRF_USBD->INTEN & USBD_INTEN_SOF_Msk) == 0)
        NRF_USBD->EVENTS_SOF = 0;

      // Enable SOF and ISOIN interrupts, and ISOIN endpoint.
      NRF_USBD->INTENSET = USBD_INTENSET_ENDISOIN_Msk | USBD_INTENSET_SOF_Msk;
      NRF_USBD->EPINEN |= USBD_EPINEN_ISOIN_Msk;
    }    
  }
  else if (dir == EP_DIR_OUT)
  {
    /*!< out */
    usb_dc_cfg.ep_out[epid].mps = mps;
    usb_dc_cfg.ep_out[epid].eptype = ep_cfg->ep_transfer_type;
    /*!< open ep */
    if (ep_cfg->ep_transfer_type != USB_DC_EP_ISOCHRONOUS)
    {
      NRF_USBD->INTENSET = (1 << (USBD_INTEN_ENDEPOUT0_Pos + epid));
      NRF_USBD->EPOUTEN |= (1 << (epid));
      __ISB();
      __DSB();
      /*!< Write any value to SIZE register will allow nRF to ACK/accept data */
      NRF_USBD->SIZE.EPOUT[epid] = 0;  
    }
    else
    {
      // SPLIT ISO buffer when ISO IN endpoint is already opened.
      if (usb_dc_cfg.ep_in[EP_ISO_NUM].mps)
        NRF_USBD->ISOSPLIT = USBD_ISOSPLIT_SPLIT_HalfIN;

      // Clear old events
      NRF_USBD->EVENTS_ENDISOOUT = 0;

      // Clear SOF event in case interrupt was not enabled yet.
      if ((NRF_USBD->INTEN & USBD_INTEN_SOF_Msk) == 0)
        NRF_USBD->EVENTS_SOF = 0;

      // Enable SOF and ISOOUT interrupts, and ISOOUT endpoint.
      NRF_USBD->INTENSET = USBD_INTENSET_ENDISOOUT_Msk | USBD_INTENSET_SOF_Msk;
      NRF_USBD->EPOUTEN |= USBD_EPOUTEN_ISOOUT_Msk;
    }
  }

  /*!< clear stall and reset DataToggle */
  NRF_USBD->EPSTALL = (USBD_EPSTALL_STALL_UnStall << USBD_EPSTALL_STALL_Pos) | (ep_cfg->ep_addr);
  NRF_USBD->DTOGGLE = (USBD_DTOGGLE_VALUE_Data0 << USBD_DTOGGLE_VALUE_Pos) | (ep_cfg->ep_addr);

  __ISB();
  __DSB();

  return 1;
}

/**
 * @brief            Close endpoint
 * @pre              None
 * @param[in]        ep ： Endpoint address
 * @retval           >=0 success otherwise failure
 */
int usbd_ep_close(const uint8_t ep)
{
  /*!< ep id */
  uint8_t epid = GET_EP_ID(ep_cfg->ep_addr);
  /*!< ep dir */
  bool dir = GET_EP_DIR(ep_cfg->ep_addr);
  /*!< ep max packet length */
  uint8_t mps = ep_cfg->ep_mps;
  /*!< update ep max packet length */
  if (epid != EP_ISO_NUM)
  {
    // CBI
    if (dir == EP_DIR_OUT)
    {
      NRF_USBD->INTENCLR = (1 << (USBD_INTEN_ENDEPOUT0_Pos + epid));
      NRF_USBD->EPOUTEN &= ~(1 << (epid));
    }
    else
    {
      NRF_USBD->INTENCLR = (1 << (USBD_INTEN_ENDEPIN0_Pos + epid));
      NRF_USBD->EPINEN &= ~(1 << (epid));
    }
  }
  else
  {
    // ISO
    if (dir == EP_DIR_OUT)
    {
      usb_dc_cfg.ep_out[EP_ISO_NUM].mps = 0;
      NRF_USBD->INTENCLR = USBD_INTENCLR_ENDISOOUT_Msk;
      NRF_USBD->EPOUTEN &= ~USBD_EPOUTEN_ISOOUT_Msk;
      NRF_USBD->EVENTS_ENDISOOUT = 0;
    }
    else
    {
      usb_dc_cfg.ep_in[EP_ISO_NUM].mps = 0;
      NRF_USBD->INTENCLR = USBD_INTENCLR_ENDISOIN_Msk;
      NRF_USBD->EPINEN &= ~USBD_EPINEN_ISOIN_Msk;
    }
    // One of the ISO endpoints closed, no need to split buffers any more.
    NRF_USBD->ISOSPLIT = USBD_ISOSPLIT_SPLIT_OneDir;
    // When both ISO endpoint are close there is no need for SOF any more.
    if (usb_dc_cfg.ep_in[EP_ISO_NUM].mps + usb_dc_cfg.ep_out[EP_ISO_NUM].mps == 0)
    {
      NRF_USBD->INTENCLR = USBD_INTENCLR_SOF_Msk;
    }
  }
  __ISB();
  __DSB();

  return 1;
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
    memset(usb_dc_cfg.ep_in[epid].ep_ram_addr,0,ep_mps);
    nrf_usbd_ep_easydma_set_tx(epid, (uint32_t)usb_dc_cfg.ep_in[epid].ep_ram_addr, 0);
    NRF_USBD->TASKS_STARTEPIN[epid] = 1;
    
    EPn_SET_TX_LEN(epid, 0);
    /*!< enable tx */
    EPn_SET_TX_VALID(epid);
    /*!< return */
    return 0;
  }

  if (data_len > ep_mps)
  {
    /*!< The data length is greater than the maximum packet length of the endpoint */
    real_wt_nums = ep_mps;
  }
  else
  {
    real_wt_nums = data_len;
  }

  /*!< write buff start */
  memcpy(usb_dc_cfg.ep_in[epid].ep_ram_addr,data,real_wt_nums);
  /*!< write buff over */

  /*!< write real_wt_nums len data */
  nrf_usbd_ep_easydma_set_tx(epid, (uint32_t)usb_dc_cfg.ep_in[epid].ep_ram_addr, real_wt_nums);

  NRF_USBD->TASKS_STARTEPIN[epid] = 1;

  EPn_SET_TX_LEN(epid, real_wt_nums);
  /*!< enable tx */
  EPn_SET_TX_VALID(epid);

  if (ret_bytes != NULL)
  {
    *ret_bytes = real_wt_nums;
  }

  return 1;
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
    /*!< 使能接收 */
    if (epid != 0)
      EPn_SET_RX_VALID(epid);
    return 0;
  }

  if (max_data_len > ep_mps)
    max_data_len = ep_mps;

  if (epid)
  {
    real_rd_nums = EPn_GET_RX_LEN(epid);
    real_rd_nums = MIN(real_rd_nums, max_data_len);
  }
  else
  {
    /*!< ep0 */
    real_rd_nums = max_data_len;
  }

  /*!< read buff start */
  memcpy(data, usb_dc_cfg.ep_out[epid].ep_ram_addr, real_rd_nums);
  /*!< read buff over */
  nrf_usbd_ep_easydma_set_tx(epid, (uint32_t)usb_dc_cfg.ep_out[epid].ep_ram_addr, real_rd_nums);
  NRF_USBD->TASKS_STARTEPOUT[epid] = 1;

  if (read_bytes != NULL)
  {
    *read_bytes = real_rd_nums;
  }

  return 1;
}

/**
 * @brief            Endpoint setting pause
 * @pre              None
 * @param[in]        ep ： Endpoint address
 * @retval           >=0 success otherwise failure
 */
int usbd_ep_set_stall(const uint8_t ep)
{
  /*!< ep id */
  uint8_t epid = GET_EP_ID(ep);
  bool dir = GET_EP_DIR(ep_cfg->ep_addr);

  usbd_ep_info *ep;
  if (dir == EP_DIR_IN)
  {
    ep = &(usb_dc_cfg.ep_in[epid]);
  }
  else
  {
    ep = &(usb_dc_cfg.ep_out[epid]);
  }

  if (epid == 0)
  {
    NRF_USBD->TASKS_EP0STALL = 1;
  }
  else if (epid != EP_ISO_NUM)
  {
    NRF_USBD->EPSTALL = (USBD_EPSTALL_STALL_Stall << USBD_EPSTALL_STALL_Pos) | (ep_cfg->ep_addr);

    // Note: nRF can auto ACK packet OUT before get stalled.
    // There maybe data in endpoint fifo already, we need to pull it out
    if ((dir == EP_DIR_OUT) && ep->data_received)
    {
      ep->data_received = false;
      xact_out_dma(epid);
    }
  }
  __ISB();
  __DSB();
  return 0;
}

/**
 * @brief            Endpoint clear pause
 * @pre              None
 * @param[in]        ep ： Endpoint address
 * @retval           >=0 success otherwise failure
 */
int usbd_ep_clear_stall(const uint8_t ep)
{
  /*!< ep id */
  uint8_t epid = GET_EP_ID(ep);
  uint8_t dir = GET_EP_DIR(ep);

  if (epid != 0 && epid != EP_ISO_NUM)
  {
    // reset data toggle to DATA0
    // First write this register with VALUE=Nop to select the endpoint, then either read it to get the status from
    // VALUE, or write it again with VALUE=Data0 or Data1
    NRF_USBD->DTOGGLE = ep;
    NRF_USBD->DTOGGLE = (USBD_DTOGGLE_VALUE_Data0 << USBD_DTOGGLE_VALUE_Pos) | ep;

    // clear stall
    NRF_USBD->EPSTALL = (USBD_EPSTALL_STALL_UnStall << USBD_EPSTALL_STALL_Pos) | ep;

    // Write any value to SIZE register will allow nRF to ACK/accept data
    if (dir == EP_DIR_OUT)
      NRF_USBD->SIZE.EPOUT[epid] = 0;

    __ISB();
    __DSB();
  }

  return 0;
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
  /*!< dc init */
  usb_dc_cfg.ep_in[0].ep_ram_addr = ep0_tx_buf_p;
  usb_dc_cfg.ep_in[1].ep_ram_addr = ep1_tx_buf_p;
  usb_dc_cfg.ep_in[2].ep_ram_addr = ep2_tx_buf_p;
  usb_dc_cfg.ep_in[3].ep_ram_addr = ep3_tx_buf_p;
  usb_dc_cfg.ep_in[4].ep_ram_addr = ep4_tx_buf_p;
  usb_dc_cfg.ep_in[5].ep_ram_addr = ep5_tx_buf_p;
  usb_dc_cfg.ep_in[6].ep_ram_addr = ep6_tx_buf_p;
  usb_dc_cfg.ep_in[7].ep_ram_addr = ep7_tx_buf_p;

  usb_dc_cfg.ep_out[0].ep_ram_addr = ep0_rx_buf_p;
  usb_dc_cfg.ep_out[1].ep_ram_addr = ep1_rx_buf_p;
  usb_dc_cfg.ep_out[2].ep_ram_addr = ep2_rx_buf_p;
  usb_dc_cfg.ep_out[3].ep_ram_addr = ep3_rx_buf_p;
  usb_dc_cfg.ep_out[4].ep_ram_addr = ep4_rx_buf_p;
  usb_dc_cfg.ep_out[5].ep_ram_addr = ep5_rx_buf_p;
  usb_dc_cfg.ep_out[6].ep_ram_addr = ep6_rx_buf_p;
  usb_dc_cfg.ep_out[7].ep_ram_addr = ep7_rx_buf_p;

  NVIC_EnableIRQ(USBD_IRQn);
  NRF_USBD->ENABLE = 0x01;
  return 0;
}

/**
  * @brief            usb bus reset
  * @pre              None
  * @param[in]        None
  * @retval           None
  */
void bus_reset(void)
{
  // 6.35.6 USB controller automatically disabled all endpoints (except control)
  NRF_USBD->EPOUTEN = 1UL;
  NRF_USBD->EPINEN = 1UL;

  for (int i = 0; i < 8; i++)
  {
    NRF_USBD->TASKS_STARTEPIN[i] = 0;
    NRF_USBD->TASKS_STARTEPOUT[i] = 0;
  }

  NRF_USBD->TASKS_STARTISOIN = 0;
  NRF_USBD->TASKS_STARTISOOUT = 0;

  // Clear USB Event Interrupt
  NRF_USBD->EVENTS_USBEVENT = 0;
  NRF_USBD->EVENTCAUSE |= NRF_USBD->EVENTCAUSE;

  // Reset interrupt
  NRF_USBD->INTENCLR = NRF_USBD->INTEN;
  NRF_USBD->INTENSET = USBD_INTEN_USBRESET_Msk | USBD_INTEN_USBEVENT_Msk | USBD_INTEN_EPDATA_Msk |
                       USBD_INTEN_EP0SETUP_Msk | USBD_INTEN_EP0DATADONE_Msk | USBD_INTEN_ENDEPIN0_Msk | USBD_INTEN_ENDEPOUT0_Msk;

  memset(&usb_dc_cfg, 0, sizeof(usb_dc_cfg));

  /*!< config ep0 */
  struct usbd_endpoint_cfg ep0 = {
      ep_addr = 0x80,
      ep_mps = 64,
      ep_transfer_type = USB_DC_EP_CONTROL,
  };
  usbd_ep_open(&ep0);
  ep0.ep_addr = 0x00;
  usbd_ep_open(&ep0);
}

/**
 * @brief            USB interrupt processing function
 * @pre              None
 * @param[in]        None
 * @retval           None
 */
void usb_isr_handler(void)
{
  uint32_t const inten = NRF_USBD->INTEN;
  uint32_t int_status = 0;

  volatile uint32_t *regevt = &NRF_USBD->EVENTS_USBRESET;

  for (uint8_t i = 0; i < USBD_INTEN_EPDATA_Pos + 1; i++)
  {
    if ((inten & (1 << i)) && regevt[i])
    {
      int_status |= (1 << (i));

      // event clear
      regevt[i] = 0;
      __ISB();
      __DSB();
    }
  }

  if (int_status & USBD_INTEN_USBRESET_Msk)
  {
    bus_reset();
    // dcd_event_bus_reset(0, TUSB_SPEED_FULL, true);
    usbd_notify_callback(USB_EVENT_RESET, NULL);
  }

  // ISOIN: Data was moved to endpoint buffer, client will be notified in SOF
  if (int_status & USBD_INTEN_ENDISOIN_Msk)
  {
    usbd_ep_info *xfer = &(usb_dc_cfg.ep_in[EP_ISO_NUM]);

    xfer->actual_len = NRF_USBD->ISOIN.AMOUNT;
    // Data transferred from RAM to endpoint output buffer.
    // Next transfer can be scheduled after SOF.
    xfer->iso_in_transfer_ready = true;
  }
#if 0
  if (int_status & USBD_INTEN_SOF_Msk)
  {
    bool iso_enabled = false;

    // ISOOUT: Transfer data gathered in previous frame from buffer to RAM
    if (NRF_USBD->EPOUTEN & USBD_EPOUTEN_ISOOUT_Msk)
    {
      iso_enabled = true;
      xact_out_dma(EP_ISO_NUM);
    }

    // ISOIN: Notify client that data was transferred
    if (NRF_USBD->EPINEN & USBD_EPINEN_ISOIN_Msk)
    {
      iso_enabled = true;
      usbd_ep_info *xfer = &(usb_dc_cfg.ep_in[TUSB_DIR_IN]);
      if (xfer->iso_in_transfer_ready)
      {
        xfer->iso_in_transfer_ready = false;
        // TODO
        // dcd_event_xfer_complete(0, EP_ISO_NUM | TUSB_DIR_IN_MASK, xfer->actual_len, XFER_RESULT_SUCCESS, true);
      }
    }

    if (!iso_enabled)
    {
      // ISO endpoint is not used, SOF is only enabled one-time for remote wakeup
      // so we disable it now
      NRF_USBD->INTENCLR = USBD_INTENSET_SOF_Msk;
    }

    // TODO
    // dcd_event_bus_signal(0, DCD_EVENT_SOF, true);
  }
#endif
  if (int_status & USBD_INTEN_USBEVENT_Msk)
  {
    enum
    {
      EVT_CAUSE_MASK = USBD_EVENTCAUSE_SUSPEND_Msk | USBD_EVENTCAUSE_RESUME_Msk | USBD_EVENTCAUSE_USBWUALLOWED_Msk
    };
    uint32_t const evt_cause = NRF_USBD->EVENTCAUSE & EVT_CAUSE_MASK;
    NRF_USBD->EVENTCAUSE = evt_cause; // clear interrupt

    if (evt_cause & USBD_EVENTCAUSE_SUSPEND_Msk)
    {
      // Put controller into low power mode
      // Leave HFXO disable to application, since it may be used by other peripherals
      NRF_USBD->LOWPOWER = 1;
      usbd_notify_callback(USB_EVENT_SUSPEND, NULL);
    }

    if (evt_cause & USBD_EVENTCAUSE_USBWUALLOWED_Msk)
    {
      // USB is out of low power mode, and wakeup is allowed
      // Initiate RESUME signal
      NRF_USBD->DPDMVALUE = USBD_DPDMVALUE_STATE_Resume;
      NRF_USBD->TASKS_DPDMDRIVE = 1;

      // There is no Resume interrupt for remote wakeup, enable SOF for to report bus ready state
      // Clear SOF event in case interrupt was not enabled yet.
      if ((NRF_USBD->INTEN & USBD_INTEN_SOF_Msk) == 0)
        NRF_USBD->EVENTS_SOF = 0;
      NRF_USBD->INTENSET = USBD_INTENSET_SOF_Msk;
    }

    if (evt_cause & USBD_EVENTCAUSE_RESUME_Msk)
    {
      usbd_notify_callback(USB_EVENT_RESUME, NULL);
    }
  }

  /*!< Setup tokens are specific to the Control endpoint. */
  if (int_status & USBD_INTEN_EP0SETUP_Msk)
  {
    uint8_t setup[8] =
        {
            NRF_USBD->BMREQUESTTYPE, NRF_USBD->BREQUEST, NRF_USBD->WVALUEL, NRF_USBD->WVALUEH,
            NRF_USBD->WINDEXL, NRF_USBD->WINDEXH, NRF_USBD->WLENGTHL, NRF_USBD->WLENGTHH};
    usb_dc_cfg.setup = (struct usb_setup_packet)setup;
    // nrf5x hw auto handle set address, there is no need to inform usb stack

    if (!(USB_REQUEST_TO_DEVICE == usb_dc_cfg.setup.bmRequestType_b.Recipient &&
          USB_REQUEST_STANDARD == usb_dc_cfg.setup.bmRequestType_b.Type &&
          USB_REQUEST_SET_ADDRESS == usb_dc_cfg.setup.bRequest))
    {
      usbd_notify_callback(USB_EVENT_SETUP_NOTIFY, (void *)&(usb_dc_cfg.setup));
    }
  }
#if 0
  if (int_status & EDPT_END_ALL_MASK)
  {
    // DMA complete move data from SRAM <-> Endpoint
    // Must before endpoint transfer handling
    // edpt_dma_end();
  }

  //--------------------------------------------------------------------+
  /* Control/Bulk/Interrupt (CBI) Transfer
   *
   * Data flow is:
   *           (bus)              (dma)
   *    Host <-------> Endpoint <-------> RAM
   *
   * For CBI OUT:
   *  - Host -> Endpoint
   *      EPDATA (or EP0DATADONE) interrupted, check EPDATASTATUS.EPOUT[i]
   *      to start DMA. For Bulk/Interrupt, this step can occur automatically (without sw),
   *      which means data may or may not be ready (out_received flag).
   *  - Endpoint -> RAM
   *      ENDEPOUT[i] interrupted, transaction complete, sw prepare next transaction
   *
   * For CBI IN:
   *  - RAM -> Endpoint
   *      ENDEPIN[i] interrupted indicate DMA is complete. HW will start
   *      to move data to host
   *  - Endpoint -> Host
   *      EPDATA (or EP0DATADONE) interrupted, check EPDATASTATUS.EPIN[i].
   *      Transaction is complete, sw prepare next transaction
   *
   * Note: in both Control In and Out of Data stage from Host <-> Endpoint
   * EP0DATADONE will be set as interrupt source
   */
  //--------------------------------------------------------------------+

  /* CBI OUT: Endpoint -> SRAM (aka transaction complete)
   * Note: Since nRF controller auto ACK next packet without SW awareness
   * We must handle this stage before Host -> Endpoint just in case 2 event happens at once
   *
   * ISO OUT: Transaction must fit in single packet, it can be shorter then total
   * len if Host decides to sent fewer bytes, it this case transaction is also
   * complete and next transfer is not initiated here like for CBI.
   */
  for (uint8_t epnum = 0; epnum < EP_CBI_COUNT + 1; epnum++)
  {
    if (int_status & (1 << (USBD_INTEN_ENDEPOUT0_Pos + epnum)))
    {
      usbd_ep_info *xfer = &(usb_dc_cfg.ep_out[epnum]);
      uint8_t const xact_len = NRF_USBD->EPOUT[epnum].AMOUNT;

      xfer->buffer += xact_len;
      xfer->actual_len += xact_len;

      // Transfer complete if transaction len < Max Packet Size or total len is transferred
      if ((epnum != EP_ISO_NUM) && (xact_len == xfer->mps) && (xfer->actual_len < xfer->total_len))
      {
        if (epnum == 0)
        {
          // Accept next Control Out packet. TASKS_EP0RCVOUT also require EasyDMA
          edpt_dma_start(&NRF_USBD->TASKS_EP0RCVOUT);
        }
        else
        {
          // nRF auto accept next Bulk/Interrupt OUT packet
          // nothing to do
        }
      }
      else
      {
        xfer->total_len = xfer->actual_len;

        // CBI OUT complete
        // TODO
        // dcd_event_xfer_complete(0, epnum, xfer->actual_len, XFER_RESULT_SUCCESS, true);
      }
    }

    // Ended event for CBI IN : nothing to do
  }

  // Endpoint <-> Host ( In & OUT )
  if (int_status & (USBD_INTEN_EPDATA_Msk | USBD_INTEN_EP0DATADONE_Msk))
  {
    uint32_t data_status = NRF_USBD->EPDATASTATUS;
    NRF_USBD->EPDATASTATUS = data_status;
    __ISB();
    __DSB();

    // EP0DATADONE is set with either Control Out on IN Data
    // Since EPDATASTATUS cannot be used to determine whether it is control OUT or IN.
    // We will use BMREQUESTTYPE in setup packet to determine the direction
    bool const is_control_in = (int_status & USBD_INTEN_EP0DATADONE_Msk) && (NRF_USBD->BMREQUESTTYPE & TUSB_DIR_IN_MASK);
    bool const is_control_out = (int_status & USBD_INTEN_EP0DATADONE_Msk) && !(NRF_USBD->BMREQUESTTYPE & TUSB_DIR_IN_MASK);

    // CBI In: Endpoint -> Host (transaction complete)
    for (uint8_t epnum = 0; epnum < EP_CBI_COUNT; epnum++)
    {
      if (data_status & (1 << epnum) || (epnum == 0 && is_control_in))
      {
        usbd_ep_info *xfer = (usb_dc_cfg.ep_in[epnum]);
        uint8_t const xact_len = NRF_USBD->EPIN[epnum].AMOUNT;

        xfer->buffer += xact_len;
        xfer->actual_len += xact_len;

        if (xfer->actual_len < xfer->total_len)
        {
          // Start DMA to copy next data packet
          xact_in_dma(epnum);
        }
        else
        {
          // CBI IN complete
          // TODO
          // dcd_event_xfer_complete(0, epnum | TUSB_DIR_IN_MASK, xfer->actual_len, XFER_RESULT_SUCCESS, true);
        }
      }
    }

    // CBI OUT: Host -> Endpoint
    for (uint8_t epnum = 0; epnum < EP_CBI_COUNT; epnum++)
    {
      if (data_status & (1 << 16 + epnum) || (epnum == 0 && is_control_out))
      {
        usbd_ep_info *xfer = (usb_dc_cfg.ep_out[epnum]);
        if (xfer->actual_len < xfer->total_len)
        {
          xact_out_dma(epnum);
        }
        else
        {
          // Data overflow !!! Nah, nRF will auto accept next Bulk/Interrupt OUT packet
          // Mark this endpoint with data received
          xfer->data_received = true;
        }
      }
    }
  }
  #endif
}
