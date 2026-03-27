/**
  ******************************************************************************
  * @file    fatfs_storage.h
  * @brief   FatFs storage wrapper for easy file system operations
  ******************************************************************************
  * @attention
  *
  * This file provides a user-friendly wrapper around FatFs for eMMC storage
  * operations on the STM32H745I-DISCO board.
  *
  ******************************************************************************
  */

#ifndef FATFS_STORAGE_H
#define FATFS_STORAGE_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "ff.h"
#include <stdint.h>

/* Exported types ------------------------------------------------------------*/
typedef enum {
    FS_OK = 0,
    FS_ERROR,
    FS_DISK_ERR,
    FS_NOT_READY,
    FS_NO_FILE,
    FS_NO_PATH,
    FS_INVALID_NAME,
    FS_DENIED,
    FS_EXIST,
    FS_INVALID_OBJECT,
    FS_WRITE_PROTECTED,
    FS_INVALID_DRIVE,
    FS_NOT_ENABLED,
    FS_NO_FILESYSTEM,
    FS_TIMEOUT,
    FS_NOT_ENOUGH_CORE,
    FS_TOO_MANY_OPEN_FILES
} FS_StatusTypeDef;

/* Exported functions --------------------------------------------------------*/

/**
  * @brief  Initialize the FatFs file system on eMMC
  * @retval FS_StatusTypeDef
  */
FS_StatusTypeDef FS_Init(void);

/**
  * @brief  Deinitialize the FatFs file system
  * @retval FS_StatusTypeDef
  */
FS_StatusTypeDef FS_DeInit(void);

/**
  * @brief  Mount the file system
  * @retval FS_StatusTypeDef
  */
FS_StatusTypeDef FS_Mount(void);

/**
  * @brief  Unmount the file system
  * @retval FS_StatusTypeDef
  */
FS_StatusTypeDef FS_Unmount(void);

/**
  * @brief  Format the eMMC storage
  * @retval FS_StatusTypeDef
  */
FS_StatusTypeDef FS_Format(void);

/**
  * @brief  Get free space on the volume
  * @param  freeBytes: Pointer to store free bytes
  * @param  totalBytes: Pointer to store total bytes
  * @retval FS_StatusTypeDef
  */
FS_StatusTypeDef FS_GetFreeSpace(uint64_t *freeBytes, uint64_t *totalBytes);

/**
  * @brief  Open a file
  * @param  fil: Pointer to file object
  * @param  path: File path
  * @param  mode: Access mode (FA_READ, FA_WRITE, FA_OPEN_EXISTING, etc.)
  * @retval FS_StatusTypeDef
  */
FS_StatusTypeDef FS_FileOpen(FIL *fil, const char *path, BYTE mode);

/**
  * @brief  Close a file
  * @param  fil: Pointer to file object
  * @retval FS_StatusTypeDef
  */
FS_StatusTypeDef FS_FileClose(FIL *fil);

/**
  * @brief  Read from a file
  * @param  fil: Pointer to file object
  * @param  buff: Buffer to store read data
  * @param  btr: Number of bytes to read
  * @param  br: Pointer to store number of bytes read
  * @retval FS_StatusTypeDef
  */
FS_StatusTypeDef FS_FileRead(FIL *fil, void *buff, UINT btr, UINT *br);

/**
  * @brief  Write to a file
  * @param  fil: Pointer to file object
  * @param  buff: Data to write
  * @param  btw: Number of bytes to write
  * @param  bw: Pointer to store number of bytes written
  * @retval FS_StatusTypeDef
  */
FS_StatusTypeDef FS_FileWrite(FIL *fil, const void *buff, UINT btw, UINT *bw);

/**
  * @brief  Sync file data to storage
  * @param  fil: Pointer to file object
  * @retval FS_StatusTypeDef
  */
FS_StatusTypeDef FS_FileSync(FIL *fil);

/**
  * @brief  Create a directory
  * @param  path: Directory path
  * @retval FS_StatusTypeDef
  */
FS_StatusTypeDef FS_MakeDir(const char *path);

/**
  * @brief  Delete a file or directory
  * @param  path: Path to delete
  * @retval FS_StatusTypeDef
  */
FS_StatusTypeDef FS_Delete(const char *path);

/**
  * @brief  Rename/move a file or directory
  * @param  oldPath: Current path
  * @param  newPath: New path
  * @retval FS_StatusTypeDef
  */
FS_StatusTypeDef FS_Rename(const char *oldPath, const char *newPath);

/**
  * @brief  Open a directory
  * @param  dir: Pointer to directory object
  * @param  path: Directory path
  * @retval FS_StatusTypeDef
  */
FS_StatusTypeDef FS_DirOpen(DIR *dir, const char *path);

/**
  * @brief  Read a directory entry
  * @param  dir: Pointer to directory object
  * @param  fno: Pointer to file info structure
  * @retval FS_StatusTypeDef
  */
FS_StatusTypeDef FS_DirRead(DIR *dir, FILINFO *fno);

/**
  * @brief  Close a directory
  * @param  dir: Pointer to directory object
  * @retval FS_StatusTypeDef
  */
FS_StatusTypeDef FS_DirClose(DIR *dir);

/**
  * @brief  Get file/directory status
  * @param  path: Path to file/directory
  * @param  fno: Pointer to file info structure
  * @retval FS_StatusTypeDef
  */
FS_StatusTypeDef FS_Stat(const char *path, FILINFO *fno);

/**
  * @brief  Get volume label
  * @param  label: Buffer to store label (minimum 12 bytes)
  * @retval FS_StatusTypeDef
  */
FS_StatusTypeDef FS_GetLabel(char *label);

/**
  * @brief  Set volume label
  * @param  label: New volume label
  * @retval FS_StatusTypeDef
  */
FS_StatusTypeDef FS_SetLabel(const char *label);

/* Helper function to convert FRESULT to FS_StatusTypeDef */
FS_StatusTypeDef FS_ConvertResult(FRESULT res);

#ifdef __cplusplus
}
#endif

#endif /* FATFS_STORAGE_H */
