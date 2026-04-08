/**
  ******************************************************************************
  * @file    fatfs_wrapper.c
  * @author  MCD Application Team
  * @brief   Helper wrapper for safe FatFs operations with USB coordination
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
#include "../../../Common/Inc/fatfs_wrapper.h"
#include "../../../Common/Inc/usb_manager.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/

/* Exported functions --------------------------------------------------------*/

/**
  * @brief  Begin a FatFs operation (mounts if needed, blocks if USB connected)
  * @retval 0 if OK, -1 if error
  */
int8_t FatFs_BeginOperation(void)
{
  return USB_Manager_RequestFatFsAccess();
}

/**
  * @brief  End a FatFs operation (releases access)
  * @retval None
  */
void FatFs_EndOperation(void)
{
  USB_Manager_ReleaseFatFsAccess();
}

/**
  * @brief  Read a configuration file safely
  * @param  filename: Name of file to read
  * @param  buffer: Buffer to store data
  * @param  buffer_size: Size of buffer
  * @param  bytes_read: Pointer to store actual bytes read
  * @retval 0 if OK, -1 if error
  */
int8_t FatFs_ReadConfigFile(const char *filename, uint8_t *buffer,
                             uint32_t buffer_size, uint32_t *bytes_read)
{
  FRESULT res;
  FIL file;
  int8_t ret = -1;

  if (FatFs_BeginOperation() != 0)
  {
    return -1;
  }

  /* Open file for reading */
  res = f_open(&file, filename, FA_READ);
  if (res == FR_OK)
  {
    /* Read file */
    res = f_read(&file, buffer, buffer_size, (UINT *)bytes_read);
    if (res == FR_OK)
    {
      ret = 0;
    }

    /* Close file */
    f_close(&file);
  }

  FatFs_EndOperation();

  return ret;
}

/**
  * @brief  Write a configuration file safely
  * @param  filename: Name of file to write
  * @param  data: Data to write
  * @param  data_size: Size of data
  * @retval 0 if OK, -1 if error
  */
int8_t FatFs_WriteConfigFile(const char *filename, const uint8_t *data,
                              uint32_t data_size)
{
  FRESULT res;
  FIL file;
  UINT bytes_written;
  int8_t ret = -1;

  if (FatFs_BeginOperation() != 0)
  {
    return -1;
  }

  /* Open file for writing (create if doesn't exist) */
  res = f_open(&file, filename, FA_CREATE_ALWAYS | FA_WRITE);
  if (res == FR_OK)
  {
    /* Write data */
    res = f_write(&file, data, data_size, &bytes_written);
    if ((res == FR_OK) && (bytes_written == data_size))
    {
      ret = 0;
    }

    /* Close file */
    f_close(&file);
  }

  FatFs_EndOperation();

  return ret;
}
