/**
  ******************************************************************************
  * @file    fatfs_example.h
  * @brief   Example usage of FatFs on eMMC storage
  ******************************************************************************
  * @attention
  *
  * This file demonstrates how to use the FatFs wrapper for eMMC storage.
  *
  * USAGE EXAMPLE:
  * ==============
  *
  * 1. Initialize the file system:
  * ------------------------------
  *    #include "fatfs_storage.h"
  *
  *    FS_StatusTypeDef status;
  *    status = FS_Init();
  *    if (status != FS_OK) {
  *        // Handle error
  *    }
  *
  *    status = FS_Mount();
  *    if (status == FS_NO_FILESYSTEM) {
  *        // Format if no filesystem found
  *        status = FS_Format();
  *    }
  *
  * 2. Write a file:
  * ----------------
  *    FIL file;
  *    UINT bytes_written;
  *    const char *data = "Hello, eMMC!";
  *
  *    status = FS_FileOpen(&file, "0:/test.txt", FA_CREATE_ALWAYS | FA_WRITE);
  *    if (status == FS_OK) {
  *        status = FS_FileWrite(&file, data, strlen(data), &bytes_written);
  *        FS_FileClose(&file);
  *    }
  *
  * 3. Read a file:
  * ---------------
  *    FIL file;
  *    UINT bytes_read;
  *    char buffer[100];
  *
  *    status = FS_FileOpen(&file, "0:/test.txt", FA_READ);
  *    if (status == FS_OK) {
  *        status = FS_FileRead(&file, buffer, sizeof(buffer), &bytes_read);
  *        buffer[bytes_read] = '\0';  // Null terminate
  *        FS_FileClose(&file);
  *    }
  *
  * 4. List directory:
  * ------------------
  *    DIR dir;
  *    FILINFO fno;
  *
  *    status = FS_DirOpen(&dir, "0:/");
  *    if (status == FS_OK) {
  *        while (1) {
  *            status = FS_DirRead(&dir, &fno);
  *            if (status != FS_OK || fno.fname[0] == 0) break;
  *
  *            if (fno.fattrib & AM_DIR) {
  *                printf("DIR: %s\n", fno.fname);
  *            } else {
  *                printf("FILE: %s (%lu bytes)\n", fno.fname, fno.fsize);
  *            }
  *        }
  *        FS_DirClose(&dir);
  *    }
  *
  * 5. Create directory and write file:
  * ------------------------------------
  *    FS_MakeDir("0:/data");
  *
  *    status = FS_FileOpen(&file, "0:/data/log.txt", FA_CREATE_ALWAYS | FA_WRITE);
  *    if (status == FS_OK) {
  *        FS_FileWrite(&file, "Log entry\n", 10, &bytes_written);
  *        FS_FileClose(&file);
  *    }
  *
  * 6. Get free space:
  * ------------------
  *    uint64_t free_bytes, total_bytes;
  *
  *    status = FS_GetFreeSpace(&free_bytes, &total_bytes);
  *    if (status == FS_OK) {
  *        printf("Free: %llu MB / Total: %llu MB\n",
  *               free_bytes / 1024 / 1024,
  *               total_bytes / 1024 / 1024);
  *    }
  *
  * 7. Cleanup:
  * -----------
  *    FS_Unmount();
  *    FS_DeInit();
  *
  * NOTES:
  * ======
  * - The eMMC must be initialized by calling EMMC_Init() before using FatFs
  * - Typically call FS_Init() and FS_Mount() once at startup
  * - All paths start with "0:/" for the eMMC drive
  * - Always close files after use to ensure data is written
  * - Both CM4 and CM7 cores can access the file system, but ensure proper
  *   synchronization between cores when accessing the same files
  *
  ******************************************************************************
  */

#ifndef FATFS_EXAMPLE_H
#define FATFS_EXAMPLE_H

#ifdef __cplusplus
extern "C" {
#endif

/* This is just a documentation header - no implementation needed */

#ifdef __cplusplus
}
#endif

#endif /* FATFS_EXAMPLE_H */
