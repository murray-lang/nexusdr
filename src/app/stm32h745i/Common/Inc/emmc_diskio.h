/**
  ******************************************************************************
  * @file    emmc_diskio.h
  * @brief   FatFs low level disk I/O driver header for eMMC storage
  ******************************************************************************
  */

#ifndef EMMC_DISKIO_H
#define EMMC_DISKIO_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "ff_gen_drv.h"

/* Exported constants --------------------------------------------------------*/
extern const Diskio_drvTypeDef EMMC_Driver;

#ifdef __cplusplus
}
#endif

#endif /* EMMC_DISKIO_H */
