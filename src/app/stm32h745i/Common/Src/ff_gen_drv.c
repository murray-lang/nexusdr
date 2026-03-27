/**
  ******************************************************************************
  * @file    ff_gen_drv.c
  * @brief   FatFs generic low level driver implementation
  ******************************************************************************
  * @attention
  *
  * This file implements the FatFs generic driver interface.
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "ff_gen_drv.h"
#include <stddef.h>  /* For NULL */

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
Disk_drvTypeDef disk = {{0}, {0}, {0}, 0};

/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/**
  * @brief  Links a compatible diskio driver/lun id and increments the number of active
  *         linked drivers.
  * @note   The number of linked drivers (volumes) is up to 10 due to FatFs limits.
  * @param  drv: pointer to the disk IO Driver structure
  * @param  path: pointer to the logical drive path
  * @param  lun : only used for USB Key Disk to add multi-lun management
            else the parameter must be equal to 0
  * @retval Returns 0 in case of success, otherwise 1.
  */
uint8_t FATFS_LinkDriverEx(const Diskio_drvTypeDef *drv, char *path, BYTE lun)
{
    uint8_t ret = 1;
    uint8_t DiskNum = 0;

    if (disk.nbr < FF_VOLUMES) {
        disk.is_initialized[disk.nbr] = 0;
        disk.drv[disk.nbr] = drv;
        disk.lun[disk.nbr] = lun;
        DiskNum = disk.nbr++;
        path[0] = (char)('0' + DiskNum);
        path[1] = ':';
        path[2] = '/';
        path[3] = 0;
        ret = 0;
    }

    return ret;
}

/**
  * @brief  Links a compatible diskio driver and increments the number of active
  *         linked drivers.
  * @note   The number of linked drivers (volumes) is up to 10 due to FatFs limits
  * @param  drv: pointer to the disk IO Driver structure
  * @param  path: pointer to the logical drive path
  * @retval Returns 0 in case of success, otherwise 1.
  */
uint8_t FATFS_LinkDriver(const Diskio_drvTypeDef *drv, char *path)
{
    return FATFS_LinkDriverEx(drv, path, 0);
}

/**
  * @brief  Unlinks a diskio driver and decrements the number of active linked
  *         drivers.
  * @param  path: pointer to the logical drive path
  * @param  lun : not used
  * @retval Returns 0 in case of success, otherwise 1.
  */
uint8_t FATFS_UnLinkDriverEx(char *path, BYTE lun)
{
    uint8_t DiskNum = 0;
    uint8_t ret = 1;

    if (disk.nbr >= 1) {
        DiskNum = path[0] - '0';
        if (disk.drv[DiskNum] != 0) {
            disk.drv[DiskNum] = 0;
            disk.lun[DiskNum] = 0;
            disk.nbr--;
            ret = 0;
        }
    }

    return ret;
}

/**
  * @brief  Unlinks a diskio driver and decrements the number of active linked
  *         drivers.
  * @param  path: pointer to the logical drive path
  * @retval Returns 0 in case of success, otherwise 1.
  */
uint8_t FATFS_UnLinkDriver(char *path)
{
    return FATFS_UnLinkDriverEx(path, 0);
}

/**
  * @brief  Gets number of linked drivers to the FatFs module.
  * @retval Number of attached drivers.
  */
uint8_t FATFS_GetAttachedDriversNbr(void)
{
    return disk.nbr;
}

/* FatFs diskio functions implementation -------------------------------------*/

/**
  * @brief  Initializes a Drive
  * @param  pdrv: Physical drive number (0..)
  * @retval DSTATUS: Operation status
  */
DSTATUS disk_initialize(BYTE pdrv)
{
    DSTATUS stat = STA_NOINIT;

    if (pdrv < disk.nbr) {
        if (disk.drv[pdrv]->disk_initialize != NULL) {
            stat = disk.drv[pdrv]->disk_initialize(disk.lun[pdrv]);

            if (stat == 0) {
                disk.is_initialized[pdrv] = 1;
            }
        }
    }
    return stat;
}

/**
  * @brief  Gets Disk Status
  * @param  pdrv: Physical drive number (0..)
  * @retval DSTATUS: Operation status
  */
DSTATUS disk_status(BYTE pdrv)
{
    DSTATUS stat = STA_NOINIT;

    if (pdrv < disk.nbr) {
        if (disk.drv[pdrv]->disk_status != NULL) {
            stat = disk.drv[pdrv]->disk_status(disk.lun[pdrv]);
        }
    }
    return stat;
}

/**
  * @brief  Reads Sector(s)
  * @param  pdrv: Physical drive number (0..)
  * @param  buff: Data buffer to store read data
  * @param  sector: Sector address (LBA)
  * @param  count: Number of sectors to read (1..128)
  * @retval DRESULT: Operation result
  */
DRESULT disk_read(BYTE pdrv, BYTE *buff, DWORD sector, UINT count)
{
    DRESULT res = RES_ERROR;

    if (pdrv < disk.nbr) {
        if (disk.drv[pdrv]->disk_read != NULL) {
            res = disk.drv[pdrv]->disk_read(disk.lun[pdrv], buff, sector, count);
        }
    }
    return res;
}

/**
  * @brief  Writes Sector(s)
  * @param  pdrv: Physical drive number (0..)
  * @param  buff: Data to be written
  * @param  sector: Sector address (LBA)
  * @param  count: Number of sectors to write (1..128)
  * @retval DRESULT: Operation result
  */
#if FF_FS_READONLY == 0
DRESULT disk_write(BYTE pdrv, const BYTE *buff, DWORD sector, UINT count)
{
    DRESULT res = RES_ERROR;

    if (pdrv < disk.nbr) {
        if (disk.drv[pdrv]->disk_write != NULL) {
            res = disk.drv[pdrv]->disk_write(disk.lun[pdrv], buff, sector, count);
        }
    }
    return res;
}
#endif /* FF_FS_READONLY == 0 */

/**
  * @brief  I/O control operation
  * @param  pdrv: Physical drive number (0..)
  * @param  cmd: Control code
  * @param  buff: Buffer to send/receive control data
  * @retval DRESULT: Operation result
  */
DRESULT disk_ioctl(BYTE pdrv, BYTE cmd, void *buff)
{
    DRESULT res = RES_ERROR;

    if (pdrv < disk.nbr) {
        if (disk.drv[pdrv]->disk_ioctl != NULL) {
            res = disk.drv[pdrv]->disk_ioctl(disk.lun[pdrv], cmd, buff);
        }
    }
    return res;
}
