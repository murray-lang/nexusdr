/**
  ******************************************************************************
  * @file    usb_manager.c
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

/* Includes ------------------------------------------------------------------*/
#include "../../../Common/Inc/usb_manager.h"

#include <misc_utils.h>

#include "main.h"
#include "ff.h"
#include <stdio.h>

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
#define USB_MANAGER_HSEM_ID (3U)  /* Dedicated semaphore for USB/FatFs coordination */
#define UART_HSEM_ID (2U)  /* UART semaphore for printf */

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
static volatile USB_StateTypeDef usb_state = USB_STATE_DISCONNECTED;
static volatile uint8_t fatfs_mounted = 0;
static FATFS usb_manager_fs;  /* Local FatFs object for USB manager */
extern char MMCPath[4];

/* Private function prototypes -----------------------------------------------*/
static void UnmountFatFs(void);

/* Private functions ---------------------------------------------------------*/

/**
  * @brief  Initialize USB manager
  * @retval None
  */
void USB_Manager_Init(void)
{
  usb_state = USB_STATE_DISCONNECTED;
  fatfs_mounted = 0;
}

/**
  * @brief  Check if USB is connected
  * @retval true if connected, false otherwise
  */
bool USB_Manager_IsConnected(void)
{
  return (usb_state == USB_STATE_CONNECTED);
}

/**
  * @brief  Notify USB manager of connection
  * @retval None
  */
void USB_Manager_OnConnect(void)
{
  LOCK_HSEM(UART_HSEM_ID);
  printf("[USB]:\tUSB Connected - Disabling FatFs access\r\n");
  UNLOCK_HSEM(UART_HSEM_ID);

  /* Unmount FatFs if it's mounted */
  UnmountFatFs();

  /* Update state */
  usb_state = USB_STATE_CONNECTED;

  /* Take the semaphore to block FatFs access */
  HAL_HSEM_FastTake(USB_MANAGER_HSEM_ID);
}

/**
  * @brief  Notify USB manager of disconnection
  * @retval None
  */
void USB_Manager_OnDisconnect(void)
{
  LOCK_HSEM(UART_HSEM_ID);
  printf("[USB]:\tUSB Disconnected - Re-enabling FatFs access\r\n");
  UNLOCK_HSEM(UART_HSEM_ID);

  /* Update state */
  usb_state = USB_STATE_DISCONNECTED;

  /* Release the semaphore to allow FatFs access */
  HAL_HSEM_Release(USB_MANAGER_HSEM_ID, 0);
}

/**
  * @brief  Request permission to mount FatFs (blocks if USB connected)
  * @retval 0 if OK, -1 if error
  */
int8_t USB_Manager_RequestFatFsAccess(void)
{
  /* Try to acquire semaphore (will block if USB is connected) */
  LOCK_HSEM(USB_MANAGER_HSEM_ID);

  /* If we got here, USB is not connected - we can mount */
  if (fatfs_mounted == 0)
  {
    FRESULT res = f_mount(&usb_manager_fs, (TCHAR const*)MMCPath, 1);
    if (res == FR_OK)
    {
      fatfs_mounted = 1;
      return 0;
    }
    else
    {
      UNLOCK_HSEM(USB_MANAGER_HSEM_ID);
      return -1;
    }
  }
  return 0;  /* Already mounted */
}

/**
  * @brief  Release FatFs access
  * @retval None
  */
void USB_Manager_ReleaseFatFsAccess(void)
{
  /* Release the semaphore */
  UNLOCK_HSEM(USB_MANAGER_HSEM_ID);
}

/**
  * @brief  Check if FatFs can be safely accessed
  * @retval true if safe, false if USB is using the device
  */
bool USB_Manager_CanAccessFatFs(void)
{
  return (usb_state == USB_STATE_DISCONNECTED);
}

/**
  * @brief  Unmount FatFs filesystem
  * @retval None
  */
static void UnmountFatFs(void)
{
  if (fatfs_mounted)
  {
    /* Unmount the filesystem */
    f_mount(NULL, (TCHAR const*)MMCPath, 0);
    fatfs_mounted = 0;

    LOCK_HSEM(UART_HSEM_ID);
    printf("[USB]:\tFatFs unmounted\r\n");
    UNLOCK_HSEM(UART_HSEM_ID);
  }
}
