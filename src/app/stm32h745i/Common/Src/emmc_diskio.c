/**
  ******************************************************************************
  * @file    emmc_diskio.c
  * @brief   FatFs low level disk I/O driver for eMMC storage
  ******************************************************************************
  * @attention
  *
  * This file implements the FatFs disk I/O interface for eMMC storage
  * on the STM32H745I-DISCO board.
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "ff_gen_drv.h"
#include "emmc_storage.h"

/* Private defines -----------------------------------------------------------*/
#define EMMC_DRIVE_NUMBER  0

/* Private variables ---------------------------------------------------------*/
static volatile DSTATUS emmc_disk_status = STA_NOINIT;

/* Private function prototypes -----------------------------------------------*/
DSTATUS EMMC_initialize(BYTE pdrv);
DSTATUS EMMC_status(BYTE pdrv);
DRESULT EMMC_read(BYTE pdrv, BYTE *buff, DWORD sector, UINT count);
#if FF_FS_READONLY == 0
DRESULT EMMC_write(BYTE pdrv, const BYTE *buff, DWORD sector, UINT count);
#endif
DRESULT EMMC_ioctl(BYTE pdrv, BYTE cmd, void *buff);

/* Disk I/O driver structure */
const Diskio_drvTypeDef EMMC_Driver =
{
    EMMC_initialize,
    EMMC_status,
    EMMC_read,
#if FF_FS_READONLY == 0
    EMMC_write,
#endif
    EMMC_ioctl,
};

/* Private functions ---------------------------------------------------------*/

/**
  * @brief  Initializes a Drive
  * @param  pdrv: Physical drive number (0..)
  * @retval DSTATUS: Operation status
  */
DSTATUS EMMC_initialize(BYTE pdrv)
{
    EMMC_StatusTypeDef status;

    if (pdrv != EMMC_DRIVE_NUMBER) {
        return STA_NOINIT;
    }

    /* Check if already initialized */
    if (emmc_disk_status == 0) {
        return emmc_disk_status;
    }

    /* Initialize eMMC */
    status = EMMC_Init();

    if (status == EMMC_OK) {
        emmc_disk_status = 0;
    } else {
        emmc_disk_status = STA_NOINIT;
    }

    return emmc_disk_status;
}

/**
  * @brief  Gets Disk Status
  * @param  pdrv: Physical drive number (0..)
  * @retval DSTATUS: Operation status
  */
DSTATUS EMMC_status(BYTE pdrv)
{
    EMMC_StatusTypeDef status;

    if (pdrv != EMMC_DRIVE_NUMBER) {
        return STA_NOINIT;
    }

    /* Check if initialized */
    if (!EMMC_IsInitialized()) {
        return STA_NOINIT;
    }

    /* Check card state */
    status = EMMC_GetCardState();

    if (status == EMMC_OK) {
        emmc_disk_status = 0;
    } else if (status == EMMC_BUSY) {
        emmc_disk_status = 0; /* Card is busy but present */
    } else {
        emmc_disk_status = STA_NOINIT;
    }

    return emmc_disk_status;
}

/**
  * @brief  Reads Sector(s)
  * @param  pdrv: Physical drive number (0..)
  * @param  buff: Data buffer to store read data
  * @param  sector: Sector address (LBA)
  * @param  count: Number of sectors to read (1..128)
  * @retval DRESULT: Operation result
  */
DRESULT EMMC_read(BYTE pdrv, BYTE *buff, DWORD sector, UINT count)
{
    EMMC_StatusTypeDef status;
    DRESULT res = RES_ERROR;

    if (pdrv != EMMC_DRIVE_NUMBER) {
        return RES_PARERR;
    }

    if (!EMMC_IsInitialized()) {
        return RES_NOTRDY;
    }

    /* Read block(s) */
    status = EMMC_ReadBlocks(buff, sector, count);

    if (status == EMMC_OK) {
        res = RES_OK;
    } else if (status == EMMC_BUSY) {
        res = RES_NOTRDY;
    } else {
        res = RES_ERROR;
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
DRESULT EMMC_write(BYTE pdrv, const BYTE *buff, DWORD sector, UINT count)
{
    EMMC_StatusTypeDef status;
    DRESULT res = RES_ERROR;

    if (pdrv != EMMC_DRIVE_NUMBER) {
        return RES_PARERR;
    }

    if (!EMMC_IsInitialized()) {
        return RES_NOTRDY;
    }

    /* Write block(s) */
    status = EMMC_WriteBlocks(buff, sector, count);

    if (status == EMMC_OK) {
        res = RES_OK;
    } else if (status == EMMC_BUSY) {
        res = RES_NOTRDY;
    } else {
        res = RES_ERROR;
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
DRESULT EMMC_ioctl(BYTE pdrv, BYTE cmd, void *buff)
{
    DRESULT res = RES_ERROR;
    EMMC_CardInfoTypeDef card_info;


    if (pdrv != EMMC_DRIVE_NUMBER) {
        return RES_PARERR;
    }

    if (!EMMC_IsInitialized()) {
        return RES_NOTRDY;
    }

    switch (cmd) {
        /* Make sure that no pending write process */
        case CTRL_SYNC:
            res = RES_OK;
            break;

        /* Get number of sectors on the disk (DWORD) */
        case GET_SECTOR_COUNT:
            if (EMMC_GetCardInfo(&card_info) == EMMC_OK) {

                /* WORKAROUND FOR HAL BUG: BSP_MMC_GetCardInfo() returns swapped values
                 *
                 * According to HAL_MMC_CardInfoTypeDef structure definition:
                 *   - BlockNbr should contain the number of blocks (capacity in blocks)
                 *   - BlockSize should contain the size of each block in bytes (typically 512)
                 *
                 * However, BSP_MMC_GetCardInfo() on STM32H745I-DISCO returns:
                 *   - BlockNbr = 512 (this is actually the block SIZE)
                 *   - BlockSize = 7364944 (this is actually the block COUNT)
                 *
                 * This appears to be a bug in the BSP or HAL MMC driver where the fields
                 * are populated in reverse order. The LogBlockNbr and LogBlockSize fields
                 * are also incorrect (512 and 0 respectively).
                 *
                 * Until this is fixed in the HAL, we work around it by using:
                 *   - BlockSize for sector count (even though it should be BlockNbr)
                 *   - BlockNbr for sector size (even though it should be BlockSize)
                 */
                *(DWORD *)buff = card_info.BlockSize;  /* Use BlockSize for count (should be BlockNbr) */
                res = RES_OK;
            } else {
            }
            break;

        /* Get R/W sector size (WORD) */
        case GET_SECTOR_SIZE:
            if (EMMC_GetCardInfo(&card_info) == EMMC_OK) {
                /* WORKAROUND FOR HAL BUG: See comment in GET_SECTOR_COUNT above.
                 * Use BlockNbr for sector size (should be BlockSize) */
                *(WORD *)buff = (WORD)card_info.BlockNbr;  /* Use BlockNbr for size (should be BlockSize) */
                res = RES_OK;
            } else {
            }
            break;

        /* Get erase block size in unit of sector (DWORD) */
        case GET_BLOCK_SIZE:
            *(DWORD *)buff = EMMC_BLOCK_SIZE;
            res = RES_OK;
            break;

        default:
            res = RES_PARERR;
            break;
    }
    return res;
}
