/**
  ******************************************************************************
  * @file    usb_manager.h
  * @author  MCD Application Team
  * @brief   USB connection manager with FatFs coordination
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2019 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __USB_MANAGER_H
#define __USB_MANAGER_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include <stdint.h>
#include <stdbool.h>

/* Exported types ------------------------------------------------------------*/
typedef enum {
  USB_STATE_DISCONNECTED = 0,
  USB_STATE_CONNECTED = 1
} USB_StateTypeDef;

/* Exported constants --------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/
/* Exported functions ------------------------------------------------------- */

/**
  * @brief  Initialize USB manager
  * @retval None
  */
void USB_Manager_Init(void);

/**
  * @brief  Check if USB is connected (called by USB callbacks)
  * @retval true if connected, false otherwise
  */
bool USB_Manager_IsConnected(void);

/**
  * @brief  Notify USB manager of connection (called by USB callbacks)
  * @retval None
  */
void USB_Manager_OnConnect(void);

/**
  * @brief  Notify USB manager of disconnection (called by USB callbacks)
  * @retval None
  */
void USB_Manager_OnDisconnect(void);

/**
  * @brief  Request permission to mount FatFs (blocks if USB connected)
  * @retval 0 if OK, -1 if error
  */
int8_t USB_Manager_RequestFatFsAccess(void);

/**
  * @brief  Release FatFs access
  * @retval None
  */
void USB_Manager_ReleaseFatFsAccess(void);

/**
  * @brief  Check if FatFs can be safely accessed
  * @retval true if safe, false if USB is using the device
  */
bool USB_Manager_CanAccessFatFs(void);

#ifdef __cplusplus
}
#endif

#endif /* __USB_MANAGER_H */
