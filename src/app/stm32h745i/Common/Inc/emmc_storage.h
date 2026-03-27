/**
  ******************************************************************************
  * @file    emmc_storage.h
  * @brief   Common eMMC storage interface for STM32H745I dual-core access
  ******************************************************************************
  * @attention
  *
  * This file provides a common interface for both CM4 and CM7 cores to access
  * the eMMC storage on the STM32H745I-DISCO board.
  *
  ******************************************************************************
  */

#ifndef EMMC_STORAGE_H
#define EMMC_STORAGE_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32h7xx_hal.h"

/* Try to use BSP MMC card info if available, otherwise define our own */
#if defined(BSP_MMC_CardInfo)
    /* Use the BSP definition which maps to HAL_MMC_CardInfoTypeDef */
    #include "stm32h745i_discovery_mmc.h"
    typedef BSP_MMC_CardInfo EMMC_CardInfoTypeDef;
#else
    /* Define our own card info structure */
    typedef struct {
        uint32_t CardType;
        uint32_t CardVersion;
        uint32_t Class;
        uint32_t RelCardAdd;
        uint32_t BlockNbr;
        uint32_t BlockSize;
        uint32_t LogBlockNbr;
        uint32_t LogBlockSize;
        uint32_t CardSpeed;
    } EMMC_CardInfoTypeDef;
#endif

typedef enum {
    EMMC_OK = 0,
    EMMC_ERROR = 1,
    EMMC_BUSY = 2,
    EMMC_TIMEOUT = 3,
    EMMC_NOT_INITIALIZED = 4
} EMMC_StatusTypeDef;

/* Exported constants --------------------------------------------------------*/
#define EMMC_INSTANCE               0U
#define EMMC_BLOCK_SIZE             512U

/* Exported functions --------------------------------------------------------*/

/**
  * @brief  Initialize the eMMC storage
  * @retval EMMC_StatusTypeDef
  */
EMMC_StatusTypeDef EMMC_Init(void);

/**
  * @brief  Deinitialize the eMMC storage
  * @retval EMMC_StatusTypeDef
  */
EMMC_StatusTypeDef EMMC_DeInit(void);

/**
  * @brief  Check if eMMC is initialized
  * @retval 1 if initialized, 0 otherwise
  */
uint8_t EMMC_IsInitialized(void);

/**
  * @brief  Read data blocks from eMMC
  * @param  pData      Pointer to data buffer
  * @param  blockAddr  Block address to read from
  * @param  numBlocks  Number of blocks to read
  * @retval EMMC_StatusTypeDef
  */
EMMC_StatusTypeDef EMMC_ReadBlocks(uint8_t *pData, uint32_t blockAddr, uint32_t numBlocks);

/**
  * @brief  Write data blocks to eMMC
  * @param  pData      Pointer to data buffer
  * @param  blockAddr  Block address to write to
  * @param  numBlocks  Number of blocks to write
  * @retval EMMC_StatusTypeDef
  */
EMMC_StatusTypeDef EMMC_WriteBlocks(const uint8_t *pData, uint32_t blockAddr, uint32_t numBlocks);

/**
  * @brief  Read data blocks from eMMC using DMA
  * @param  pData      Pointer to data buffer
  * @param  blockAddr  Block address to read from
  * @param  numBlocks  Number of blocks to read
  * @retval EMMC_StatusTypeDef
  */
EMMC_StatusTypeDef EMMC_ReadBlocks_DMA(uint8_t *pData, uint32_t blockAddr, uint32_t numBlocks);

/**
  * @brief  Write data blocks to eMMC using DMA
  * @param  pData      Pointer to data buffer
  * @param  blockAddr  Block address to write to
  * @param  numBlocks  Number of blocks to write
  * @retval EMMC_StatusTypeDef
  */
EMMC_StatusTypeDef EMMC_WriteBlocks_DMA(const uint8_t *pData, uint32_t blockAddr, uint32_t numBlocks);

/**
  * @brief  Erase blocks on eMMC
  * @param  startAddr  Start block address
  * @param  endAddr    End block address
  * @retval EMMC_StatusTypeDef
  */
EMMC_StatusTypeDef EMMC_Erase(uint32_t startAddr, uint32_t endAddr);

/**
  * @brief  Get eMMC card information
  * @param  pCardInfo  Pointer to card info structure
  * @retval EMMC_StatusTypeDef
  */
EMMC_StatusTypeDef EMMC_GetCardInfo(EMMC_CardInfoTypeDef *pCardInfo);

/**
  * @brief  Get eMMC card state
  * @retval EMMC_StatusTypeDef
  */
EMMC_StatusTypeDef EMMC_GetCardState(void);

/**
  * @brief  Get total capacity in bytes
  * @retval Total capacity in bytes
  */
uint64_t EMMC_GetCapacity(void);

/* Weak callbacks that can be overridden by user */
void EMMC_ReadCpltCallback(void);
void EMMC_WriteCpltCallback(void);
void EMMC_AbortCallback(void);

#ifdef __cplusplus
}
#endif

#endif /* EMMC_STORAGE_H */
