/**
 * @file usbd_dfu.h
 * @brief
 *
 * Copyright (c) 2022 sakumisu
 *
 * Licensed to the Apache Software Foundation (ASF) under one or more
 * contributor license agreements.  See the NOTICE file distributed with
 * this work for additional information regarding copyright ownership.  The
 * ASF licenses this file to you under the Apache License, Version 2.0 (the
 * "License"); you may not use this file except in compliance with the
 * License.  You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.  See the
 * License for the specific language governing permissions and limitations
 * under the License.
 *
 */
#ifndef _USBD_DFU_H_
#define _USBD_DFU_H_

#include "lg_usb_dfu.h"

#ifdef __cplusplus
extern "C" {
#endif

void usbd_dfu_add_interface(usbd_class_t *devclass, usbd_interface_t *intf);

/* Interface functions that need to be implemented by the user */
uint8_t *dfu_read_flash(uint8_t *src, uint8_t *dest, uint32_t len);
uint16_t dfu_write_flash(uint8_t *src, uint8_t *dest, uint32_t len);
uint16_t dfu_erase_flash(uint32_t add);
void dfu_leave(void);

#ifdef __cplusplus
}
#endif

#endif /* _USBD_DFU_H_ */