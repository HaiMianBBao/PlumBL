/*
 * @Description: usb协议栈相关的工具
 * @Author: LiGuo
 * @Email: 1570139720@qq.com
 */
#pragma once

#ifdef __cplusplus
extern "C"
{
#endif

#include <stdbool.h>
#include <string.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include "lg_usb_slist.h"

#if defined(__CC_ARM)
#ifndef __USED
#define __USED __attribute__((used))
#endif
#ifndef __WEAK
#define __WEAK __attribute__((weak))
#endif
#ifndef __PACKED
#define __PACKED __attribute__((packed))
#endif
#ifndef __PACKED_STRUCT
#define __PACKED_STRUCT __packed struct
#endif
#ifndef __PACKED_UNION
#define __PACKED_UNION __packed union
#endif
#ifndef __ALIGNED
#define __ALIGNED(x) __attribute__((aligned(x)))
#endif
#elif defined(__GNUC__)
#ifndef __USED
#define __USED __attribute__((used))
#endif
#ifndef __WEAK
#define __WEAK __attribute__((weak))
#endif
#ifndef __PACKED
#define __PACKED __attribute__((packed, aligned(1)))
#endif
#ifndef __PACKED_STRUCT
#define __PACKED_STRUCT struct __attribute__((packed, aligned(1)))
#endif
#ifndef __PACKED_UNION
#define __PACKED_UNION union __attribute__((packed, aligned(1)))
#endif
#ifndef __ALIGNED
#define __ALIGNED(x) __attribute__((aligned(x)))
#endif
#elif defined(__ICCARM__)
#ifndef __USED
#if __ICCARM_V8
#define __USED __attribute__((used))
#else
#define __USED _Pragma("__root")
#endif
#endif

#ifndef __WEAK
#if __ICCARM_V8
#define __WEAK __attribute__((weak))
#else
#define __WEAK _Pragma("__weak")
#endif
#endif

#ifndef __PACKED
#if __ICCARM_V8
#define __PACKED __attribute__((packed, aligned(1)))
#else
/* Needs IAR language extensions */
#define __PACKED __packed
#endif
#endif

#ifndef __PACKED_STRUCT
#if __ICCARM_V8
#define __PACKED_STRUCT struct __attribute__((packed, aligned(1)))
#else
/* Needs IAR language extensions */
#define __PACKED_STRUCT __packed struct
#endif
#endif

#ifndef __PACKED_UNION
#if __ICCARM_V8
#define __PACKED_UNION union __attribute__((packed, aligned(1)))
#else
/* Needs IAR language extensions */
#define __PACKED_UNION __packed union
#endif
#endif

#ifndef __ALIGNED
#if __ICCARM_V8
#define __ALIGNED(x) __attribute__((aligned(x)))
#elif (__VER__ >= 7080000)
/* Needs IAR language extensions */
#define __ALIGNED(x) __attribute__((aligned(x)))
#else
#warning No compiler specific solution for __ALIGNED.__ALIGNED is ignored.
#define __ALIGNED(x)
#endif
#endif

#endif

#ifndef __ALIGN_BEGIN
#define __ALIGN_BEGIN
#endif
#ifndef __ALIGN_END
#define __ALIGN_END __attribute__((aligned(4)))
#endif

#ifndef ARG_UNUSED
#define ARG_UNUSED(x) (void)(x)
#endif

#define GET_BE16(field) \
    (((uint16_t)(field)[0] << 8) | ((uint16_t)(field)[1]))

#define GET_BE32(field) \
    (((uint32_t)(field)[0] << 24) | ((uint32_t)(field)[1] << 16) | ((uint32_t)(field)[2] << 8) | ((uint32_t)(field)[3] << 0))


/**
 * @brief   取低字节
 */
#ifndef LO_BYTE
#define LO_BYTE(x) ((uint8_t)(x & 0x00FF))
#endif

/**
 * @brief   取高字节
 */
#ifndef HI_BYTE
#define HI_BYTE(x) ((uint8_t)((x & 0xFF00) >> 8))
#endif

/**
 * @brief   取a b 中的较小值
 */
#ifndef MIN
#define MIN(a, b) (((a) < (b)) ? (a) : (b))
#endif

//#define USB_DESC_SECTION __attribute__((section("usb_desc"))) __used __aligned(1)

#define REQTYPE_GET_DIR(x) (((x) >> 7) & 0x01)
#define REQTYPE_GET_TYPE(x) (((x) >> 5) & 0x03U)
#define REQTYPE_GET_RECIP(x) ((x)&0x1F)

#define WBVAL(x) (x & 0xFF), ((x >> 8) & 0xFF)										   /*!< 16位 每8位分离一下 */
#define DBVAL(x) (x & 0xFF), ((x >> 8) & 0xFF), ((x >> 16) & 0xFF), ((x >> 24) & 0xFF) /*!< 32位 每8位分离一下 */

/*!< 打印调试相关 */
#ifdef USB_DEBUG
//#include "printf.h"
#define USB_LOG printf
#define USBD_LOG_DBG USB_LOG

#define USB_LOG_SETUP(setup)                                   \
	printf("*************SETUP*************** \r\n");          \
	printf("bmRequsetType:  0x%x \r\n", setup->bmRequestType); \
	printf("bRequest:  0x%x \r\n", setup->bRequest);           \
	printf("wValue:   0x%x \r\n", setup->wValue);              \
	printf("wIndex:   0x%x \r\n", setup->wIndex);              \
	printf("wLength:  0x%x \r\n", setup->wLength);             \
	printf("********************************* \r\n");
#else
#define USB_LOG(...) (void)0
#define USBD_LOG_DBG USB_LOG

#endif

#ifdef __cplusplus
}
#endif
