/**
  ******************************************************************************
  * @file    fatfs_wrapper.h
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

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __FATFS_WRAPPER_H
#define __FATFS_WRAPPER_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "ff.h"
#include <stdint.h>

/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/

/**
  * @brief  Macro to safely execute FatFs operations
  * @note   Usage: SAFE_FATFS_OPERATION({ your_fatfs_code_here; });
  * @note   Automatically handles USB detection and mounting/unmounting
  */
#define SAFE_FATFS_OPERATION(code) \
  do { \
    if (FatFs_BeginOperation() == 0) { \
      code \
      FatFs_EndOperation(); \
    } \
  } while(0)

/* Exported functions ------------------------------------------------------- */

/**
  * @brief  Begin a FatFs operation (mounts if needed, blocks if USB connected)
  * @retval 0 if OK, -1 if error
  */
int8_t FatFs_BeginOperation(void);

/**
  * @brief  End a FatFs operation (releases access)
  * @retval None
  */
void FatFs_EndOperation(void);

/**
  * @brief  Read a configuration file safely
  * @param  filename: Name of file to read
  * @param  buffer: Buffer to store data
  * @param  buffer_size: Size of buffer
  * @param  bytes_read: Pointer to store actual bytes read
  * @retval 0 if OK, -1 if error
  */
int8_t FatFs_ReadConfigFile(const char *filename, uint8_t *buffer,
                             uint32_t buffer_size, uint32_t *bytes_read);

/**
  * @brief  Write a configuration file safely
  * @param  filename: Name of file to write
  * @param  data: Data to write
  * @param  data_size: Size of data
  * @retval 0 if OK, -1 if error
  */
int8_t FatFs_WriteConfigFile(const char *filename, const uint8_t *data,
                              uint32_t data_size);

#ifdef __cplusplus
}
#endif

#endif /* __FATFS_WRAPPER_H */
