/**
  ******************************************************************************
  * @file          lg_usb_dc.h
  * @brief         usb dcd framework api
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

#include "stdint.h"
#include "stdbool.h"

/*!< Maximum packet length for endpoint 0 */
#define USB_CTRL_EP_MPS 64
/*!< Out address of endpoint 0 */
#define USB_CTRL_OUT_EP0 0
/*!< In address of endpoint 0 */
#define USB_CTRL_IN_EP0 0x80

/*!< Endpoint transport type */
#define USBD_EP_TYPE_CTRL 0
#define USBD_EP_TYPE_ISOC 1
#define USBD_EP_TYPE_BULK 2
#define USBD_EP_TYPE_INTR 3
#define USBD_EP_TYPE_MASK 3
#define USBD_EP_TYPE_DBLBUF 4


/**
 * @brief   Enumerate endpoint transport types
 */
enum usb_dc_ep_transfer_type
{
    /*!< Control transmission */
    USB_DC_EP_CONTROL = 0,
    /*!< Synchronous transmission */
    USB_DC_EP_ISOCHRONOUS,
    /*!< Bulk transmission  */
    USB_DC_EP_BULK,
    /*!< Interrupt transmission */
    USB_DC_EP_INTERRUPT
};
/**
 * @brief   Store the information of the configured endpoint
 */
struct usbd_endpoint_cfg
{
    /*!< Endpoint address */
    uint8_t ep_addr;
    /*!< Maximum packet length of endpoint */
    uint8_t ep_mps;
    /*!< Transport type of endpoint */
    enum usb_dc_ep_transfer_type  ep_transfer_type;
};
/**
  * @brief            Set address
  * @pre              None
  * @param[in]        address
  * @retval           >=0 Succeeded, otherwise failed
  */
int usbd_set_address(const uint8_t address);

/**
  * @brief            Open endpoint
  * @pre              None
  * @param[in]        ep_cfg Configuration structure of endpoint
  * @retval           >=0 Succeeded, otherwise failed
  */
int usbd_ep_open(const struct usbd_endpoint_cfg *ep_cfg);

/**
  * @brief            Close endpoint
  * @pre              None
  * @param[in]        ep Endpoint address
  * @retval           >=0 Succeeded, otherwise failed
  */
int usbd_ep_close(const uint8_t ep);

/**
 * @brief Writes data to the specified endpoint
 *
 * This function is called to write data to the specified endpoint. The
 * supplied usbd_endpoint_callback function will be called when data is transmitted
 * out.
 *
 * @param[in]  ep        The address of the endpoint to be written to
 * @param[in]  data      Pointer to data to be sent
 * @param[in]  data_len  Length of data requested to be written
 * @param[out] ret_bytes The length of bytes actually written 
 * because the maximum packet length of possible endpoints is less than data_ len
 *
 * @return >=0 Succeeded, otherwise failed
 */
int usbd_ep_write(const uint8_t ep, const uint8_t *data, uint32_t data_len, uint32_t *ret_bytes);

/**
 * @brief Reads data from the specified endpoint
 *
 * This is similar to usb_dc_ep_read, the difference being that, it doesn't
 * clear the endpoint NAKs so that the consumer is not bogged down by further
 * upcalls till he is done with the processing of the data. The caller should
 * reactivate ep by invoking usb_dc_ep_read_continue() do so.
 *
 * @param[in]  ep           Endpoint address to be read
 * @param[in]  data         Pointer to the buffer where the read data is to be stored
 * @param[in]  max_data_len Maximum read data length
 * @param[out] read_bytes   The length of bytes actually read, 
 * because the maximum packet length of the endpoint may be less than max_ data_ len
 *
 * @return >=0 Succeeded, otherwise failed
 */
int usbd_ep_read(const uint8_t ep, uint8_t *data, uint32_t max_data_len, uint32_t *read_bytes);

/**
  * @brief            Sets a pause condition for the selected endpoint
  * @pre              None
  * @param[in]        ep Endpoint address to be set
  * @retval           >=0 Succeeded, otherwise failed
  */
int usbd_ep_set_stall(const uint8_t ep);

/**
  * @brief            Clears the pause condition for the selected endpoint
  * @pre              None
  * @param[in]        ep Endpoint address to be clear
  * @retval           >=0 Succeeded, otherwise failed
  */
int usbd_ep_clear_stall(const uint8_t ep);

/**
  * @brief            Check whether the endpoint is in a tentative state
  * @pre              None
  * @param[in]        ep Endpoint address to be checked
  * @param[out]       stalled This parameter can get the status of the endpoint (whether it is suspended or not)
  * @retval           >=0 Succeeded, otherwise failed
  */
int usbd_ep_get_stall(const uint8_t ep,uint8_t *stalled);

#ifdef __cplusplus
}
#endif



/************************ (C) COPYRIGHT 2021 Li Guo *****END OF FILE*************/