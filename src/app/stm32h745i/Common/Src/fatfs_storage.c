/**
  ******************************************************************************
  * @file    fatfs_storage.c
  * @brief   FatFs storage wrapper implementation
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "fatfs_storage.h"
#include "emmc_diskio.h"
#include "ff_gen_drv.h"
#include <string.h>

/* Private defines -----------------------------------------------------------*/
#define EMMC_DRIVE_PATH     "0:/"

/* Private variables ---------------------------------------------------------*/
static FATFS fs_object __attribute__((section(".RAM_D1"))) __attribute__((aligned(32)));
static BYTE format_work_buffer[4096] __attribute__((section(".RAM_D1"))) __attribute__((aligned(32)));
static char emmc_path[4];
static uint8_t fs_initialized = 0;
static uint8_t fs_mounted = 0;

/* Diagnostic variables (extern accessible) */
FRESULT last_fresult = FR_OK;
uint32_t format_attempt_num = 0;

/* Private function prototypes -----------------------------------------------*/
/* Exported functions --------------------------------------------------------*/

/**
  * @brief  Initialize the FatFs file system on eMMC
  * @retval FS_StatusTypeDef
  */
FS_StatusTypeDef FS_Init(void)
{
    if (fs_initialized) {
        return FS_OK;
    }

    /* Link the eMMC disk I/O driver */
    if (FATFS_LinkDriver(&EMMC_Driver, emmc_path) != 0) {
        return FS_ERROR;
    }

    fs_initialized = 1;
    return FS_OK;
}

/**
  * @brief  Deinitialize the FatFs file system
  * @retval FS_StatusTypeDef
  */
FS_StatusTypeDef FS_DeInit(void)
{
    if (!fs_initialized) {
        return FS_OK;
    }

    /* Unmount if mounted */
    if (fs_mounted) {
        FS_Unmount();
    }

    /* Unlink the driver */
    FATFS_UnLinkDriver(emmc_path);

    fs_initialized = 0;
    return FS_OK;
}

/**
  * @brief  Mount the file system
  * @retval FS_StatusTypeDef
  */
FS_StatusTypeDef FS_Mount(void)
{
    FRESULT res;

    if (!fs_initialized) {
        return FS_NOT_READY;
    }

    if (fs_mounted) {
        return FS_OK;
    }

    /* Mount the file system (0 = delayed mount, 1 = immediate mount) */
    res = f_mount(&fs_object, emmc_path, 1);

    if (res == FR_OK) {
        fs_mounted = 1;
        return FS_OK;
    }

    return FS_ConvertResult(res);
}

/**
  * @brief  Unmount the file system
  * @retval FS_StatusTypeDef
  */
FS_StatusTypeDef FS_Unmount(void)
{
    FRESULT res;

    if (!fs_mounted) {
        return FS_OK;
    }

    /* Unmount the file system */
    res = f_mount(NULL, emmc_path, 0);

    if (res == FR_OK) {
        fs_mounted = 0;
        return FS_OK;
    }

    return FS_ConvertResult(res);
}

/**
  * @brief  Format the eMMC storage
  * @retval FS_StatusTypeDef
  */
FS_StatusTypeDef FS_Format(void)
{
    FRESULT res;

    if (!fs_initialized) {
        return FS_NOT_READY;
    }

    /* Format the drive
     * Try multiple approaches to see which one works
     */

    // First try: FM_FAT32 with 4KB clusters
    format_attempt_num = 1;
    res = f_mkfs(emmc_path, FM_FAT32, 4096, format_work_buffer, sizeof(format_work_buffer));
    last_fresult = res;

    if (res != FR_OK) {
        // Second try: FM_FAT32 with auto cluster size
        format_attempt_num = 2;
        res = f_mkfs(emmc_path, FM_FAT32, 0, format_work_buffer, sizeof(format_work_buffer));
        last_fresult = res;
    }

    if (res != FR_OK) {
        // Third try: FM_ANY (let FatFs decide)
        format_attempt_num = 3;
        res = f_mkfs(emmc_path, FM_ANY, 0, format_work_buffer, sizeof(format_work_buffer));
        last_fresult = res;
    }

    if (res != FR_OK) {
        // Fourth try: FM_FAT (FAT16/12 instead of FAT32)
        format_attempt_num = 4;
        res = f_mkfs(emmc_path, FM_FAT, 0, format_work_buffer, sizeof(format_work_buffer));
        last_fresult = res;
    }

    if (res != FR_OK) {
        // All attempts failed - last_fresult and format_attempt_num contain details
        return FS_ConvertResult(res);
    }

    // Success - clear error tracking
    format_attempt_num = 0;
    last_fresult = FR_OK;

    /* Unmount if currently mounted */
    if (fs_mounted) {
        FS_Unmount();
    }

    return FS_OK;
}

/**
  * @brief  Get free space on the volume
  * @param  freeBytes: Pointer to store free bytes
  * @param  totalBytes: Pointer to store total bytes
  * @retval FS_StatusTypeDef
  */
FS_StatusTypeDef FS_GetFreeSpace(uint64_t *freeBytes, uint64_t *totalBytes)
{
    FRESULT res;
    DWORD freeClusters;
    FATFS *fs;

    if (!fs_mounted) {
        return FS_NOT_READY;
    }

    if (freeBytes == NULL && totalBytes == NULL) {
        return FS_ERROR;
    }

    /* Get volume information and free clusters */
    res = f_getfree(emmc_path, &freeClusters, &fs);

    if (res != FR_OK) {
        return FS_ConvertResult(res);
    }

    if (freeBytes != NULL) {
        *freeBytes = (uint64_t)freeClusters * fs->csize * FF_MAX_SS;
    }

    if (totalBytes != NULL) {
        *totalBytes = (uint64_t)(fs->n_fatent - 2) * fs->csize * FF_MAX_SS;
    }

    return FS_OK;
}

/**
  * @brief  Open a file
  */
FS_StatusTypeDef FS_FileOpen(FIL *fil, const char *path, BYTE mode)
{
    FRESULT res;

    if (!fs_mounted || fil == NULL || path == NULL) {
        return FS_NOT_READY;
    }

    res = f_open(fil, path, mode);
    return FS_ConvertResult(res);
}

/**
  * @brief  Close a file
  */
FS_StatusTypeDef FS_FileClose(FIL *fil)
{
    FRESULT res;

    if (fil == NULL) {
        return FS_ERROR;
    }

    res = f_close(fil);
    return FS_ConvertResult(res);
}

/**
  * @brief  Read from a file
  */
FS_StatusTypeDef FS_FileRead(FIL *fil, void *buff, UINT btr, UINT *br)
{
    FRESULT res;

    if (fil == NULL || buff == NULL) {
        return FS_ERROR;
    }

    res = f_read(fil, buff, btr, br);
    return FS_ConvertResult(res);
}

/**
  * @brief  Write to a file
  */
FS_StatusTypeDef FS_FileWrite(FIL *fil, const void *buff, UINT btw, UINT *bw)
{
    FRESULT res;

    if (fil == NULL || buff == NULL) {
        return FS_ERROR;
    }

    res = f_write(fil, buff, btw, bw);
    return FS_ConvertResult(res);
}

/**
  * @brief  Sync file data to storage
  */
FS_StatusTypeDef FS_FileSync(FIL *fil)
{
    FRESULT res;

    if (fil == NULL) {
        return FS_ERROR;
    }

    res = f_sync(fil);
    return FS_ConvertResult(res);
}

/**
  * @brief  Create a directory
  */
FS_StatusTypeDef FS_MakeDir(const char *path)
{
    FRESULT res;

    if (!fs_mounted || path == NULL) {
        return FS_NOT_READY;
    }

    res = f_mkdir(path);
    return FS_ConvertResult(res);
}

/**
  * @brief  Delete a file or directory
  */
FS_StatusTypeDef FS_Delete(const char *path)
{
    FRESULT res;

    if (!fs_mounted || path == NULL) {
        return FS_NOT_READY;
    }

    res = f_unlink(path);
    return FS_ConvertResult(res);
}

/**
  * @brief  Rename/move a file or directory
  */
FS_StatusTypeDef FS_Rename(const char *oldPath, const char *newPath)
{
    FRESULT res;

    if (!fs_mounted || oldPath == NULL || newPath == NULL) {
        return FS_NOT_READY;
    }

    res = f_rename(oldPath, newPath);
    return FS_ConvertResult(res);
}

/**
  * @brief  Open a directory
  */
FS_StatusTypeDef FS_DirOpen(DIR *dir, const char *path)
{
    FRESULT res;

    if (!fs_mounted || dir == NULL || path == NULL) {
        return FS_NOT_READY;
    }

    res = f_opendir(dir, path);
    return FS_ConvertResult(res);
}

/**
  * @brief  Read a directory entry
  */
FS_StatusTypeDef FS_DirRead(DIR *dir, FILINFO *fno)
{
    FRESULT res;

    if (dir == NULL || fno == NULL) {
        return FS_ERROR;
    }

    res = f_readdir(dir, fno);
    return FS_ConvertResult(res);
}

/**
  * @brief  Close a directory
  */
FS_StatusTypeDef FS_DirClose(DIR *dir)
{
    FRESULT res;

    if (dir == NULL) {
        return FS_ERROR;
    }

    res = f_closedir(dir);
    return FS_ConvertResult(res);
}

/**
  * @brief  Get file/directory status
  */
FS_StatusTypeDef FS_Stat(const char *path, FILINFO *fno)
{
    FRESULT res;

    if (!fs_mounted || path == NULL || fno == NULL) {
        return FS_NOT_READY;
    }

    res = f_stat(path, fno);
    return FS_ConvertResult(res);
}

/**
  * @brief  Get volume label
  */
FS_StatusTypeDef FS_GetLabel(char *label)
{
    FRESULT res;

    if (!fs_mounted || label == NULL) {
        return FS_NOT_READY;
    }

    res = f_getlabel(emmc_path, label, NULL);
    return FS_ConvertResult(res);
}

/**
  * @brief  Set volume label
  */
FS_StatusTypeDef FS_SetLabel(const char *label)
{
    FRESULT res;

    if (!fs_mounted || label == NULL) {
        return FS_NOT_READY;
    }

    res = f_setlabel(label);
    return FS_ConvertResult(res);
}

/**
  * @brief  Convert FRESULT to FS_StatusTypeDef
  */
FS_StatusTypeDef FS_ConvertResult(FRESULT res)
{
    switch (res) {
        case FR_OK:                  return FS_OK;
        case FR_DISK_ERR:            return FS_DISK_ERR;
        case FR_INT_ERR:             return FS_ERROR;
        case FR_NOT_READY:           return FS_NOT_READY;
        case FR_NO_FILE:             return FS_NO_FILE;
        case FR_NO_PATH:             return FS_NO_PATH;
        case FR_INVALID_NAME:        return FS_INVALID_NAME;
        case FR_DENIED:              return FS_DENIED;
        case FR_EXIST:               return FS_EXIST;
        case FR_INVALID_OBJECT:      return FS_INVALID_OBJECT;
        case FR_WRITE_PROTECTED:     return FS_WRITE_PROTECTED;
        case FR_INVALID_DRIVE:       return FS_INVALID_DRIVE;
        case FR_NOT_ENABLED:         return FS_NOT_ENABLED;
        case FR_NO_FILESYSTEM:       return FS_NO_FILESYSTEM;
        case FR_TIMEOUT:             return FS_TIMEOUT;
        case FR_NOT_ENOUGH_CORE:     return FS_NOT_ENOUGH_CORE;
        case FR_TOO_MANY_OPEN_FILES: return FS_TOO_MANY_OPEN_FILES;
        default:                     return FS_ERROR;
    }
}
