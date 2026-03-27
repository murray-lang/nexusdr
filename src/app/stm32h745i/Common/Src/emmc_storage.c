/**
  ******************************************************************************
  * @file    emmc_storage.c
  * @brief   Common eMMC storage implementation for STM32H745I dual-core access
  ******************************************************************************
  * @attention
  *
  * This file provides the implementation of common eMMC interface that can be
  * used by both CM4 and CM7 cores on the STM32H745I-DISCO board.
  *
  * Note: The actual hardware initialization (BSP_MMC_Init) should be called
  * from the core that owns the SDMMC peripheral (typically CM7).
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "emmc_storage.h"
#include "stm32h745i_discovery_mmc.h"

/* Override weak MX_MMC_SD_Init to use slower clock to avoid CRC errors */
extern MMC_HandleTypeDef hsd_sdmmc[];

HAL_StatusTypeDef MX_MMC_SD_Init(MMC_HandleTypeDef *hmmc)
{
    HAL_StatusTypeDef ret = HAL_OK;

    hmmc->Instance                 = SDMMC1;
    /* Increase ClockDiv from 2 to 8 to slow down the clock and avoid CRC errors.
     * With SDMMC kernel clock at 200MHz:
     * - ClockDiv=2: 200/(2+2) = 50MHz (was causing HAL_MMC_ERROR_CMD_CRC_FAIL on writes)
     * - ClockDiv=8: 200/(8+2) = 20MHz (more conservative, should be reliable)
     */
    hmmc->Init.ClockDiv            = 8;
    hmmc->Init.ClockPowerSave      = SDMMC_CLOCK_POWER_SAVE_DISABLE;
    hmmc->Init.ClockEdge           = SDMMC_CLOCK_EDGE_RISING;
    hmmc->Init.HardwareFlowControl = SDMMC_HARDWARE_FLOW_CONTROL_DISABLE;
    hmmc->Init.BusWide             = SDMMC_BUS_WIDE_8B;

    /* HAL MMC initialization */
    if (HAL_MMC_Init(hmmc) != HAL_OK) {
        ret = HAL_ERROR;
    }

    return ret;
}

/* Private variables ---------------------------------------------------------*/
static uint8_t emmc_initialized = 0;

/* Private function prototypes -----------------------------------------------*/

/* Exported functions --------------------------------------------------------*/

/**
  * @brief  Initialize the eMMC storage
  * @retval EMMC_StatusTypeDef
  */
EMMC_StatusTypeDef EMMC_Init(void)
{
    int32_t bsp_status;

    if (emmc_initialized) {
        return EMMC_OK;
    }

    /* Initialize the BSP MMC driver */
    bsp_status = BSP_MMC_Init(EMMC_INSTANCE);

    if (bsp_status != BSP_ERROR_NONE) {
        return EMMC_ERROR;
    }

    emmc_initialized = 1;
    return EMMC_OK;
}

/**
  * @brief  Deinitialize the eMMC storage
  * @retval EMMC_StatusTypeDef
  */
EMMC_StatusTypeDef EMMC_DeInit(void)
{
    int32_t bsp_status;

    if (!emmc_initialized) {
        return EMMC_NOT_INITIALIZED;
    }

    bsp_status = BSP_MMC_DeInit(EMMC_INSTANCE);

    if (bsp_status != BSP_ERROR_NONE) {
        return EMMC_ERROR;
    }

    emmc_initialized = 0;
    return EMMC_OK;
}

/**
  * @brief  Check if eMMC is initialized
  * @retval 1 if initialized, 0 otherwise
  */
uint8_t EMMC_IsInitialized(void)
{
    return emmc_initialized;
}

/**
  * @brief  Read data blocks from eMMC
  * @param  pData      Pointer to data buffer
  * @param  blockAddr  Block address to read from
  * @param  numBlocks  Number of blocks to read
  * @retval EMMC_StatusTypeDef
  */
EMMC_StatusTypeDef EMMC_ReadBlocks(uint8_t *pData, uint32_t blockAddr, uint32_t numBlocks)
{
    int32_t bsp_status;
    uint32_t alignedAddr;
    uint32_t alignedSize;
    extern MMC_HandleTypeDef hsd_sdmmc[];

    if (!emmc_initialized) {
        return EMMC_NOT_INITIALIZED;
    }

    if (pData == NULL || numBlocks == 0) {
        return EMMC_ERROR;
    }

    /* Calculate aligned address and size for cache operations
     * STM32H7 D-Cache line size is 32 bytes */
    alignedAddr = (uint32_t)pData & ~31U;
    alignedSize = ((uint32_t)pData - alignedAddr) + (numBlocks * 512);
    alignedSize = (alignedSize + 31) & ~31U;

    /* Invalidate cache before read to avoid reading stale data */
    SCB_InvalidateDCache_by_Addr((uint32_t *)alignedAddr, alignedSize);

    /* Use DMA mode - STM32H7 SDMMC polling mode doesn't work reliably */
    bsp_status = BSP_MMC_ReadBlocks_DMA(EMMC_INSTANCE, (uint32_t *)pData, blockAddr, numBlocks);

    if (bsp_status != BSP_ERROR_NONE) {
        return EMMC_ERROR;
    }

    /* Wait for DMA transfer to complete - DATAEND interrupt sets state to READY */
    uint32_t tickstart = HAL_GetTick();
    uint32_t timeout = 1000; /* 1 second timeout */

    while (HAL_MMC_GetState(&hsd_sdmmc[EMMC_INSTANCE]) != HAL_MMC_STATE_READY) {
        if ((HAL_GetTick() - tickstart) >= timeout) {
            return EMMC_TIMEOUT;
        }
        /* Very small busy-wait to allow interrupts without blocking LVGL */
        for (volatile int i = 0; i < 100; i++);
    }

    /* Check if there was an error during the transfer */
    if (hsd_sdmmc[EMMC_INSTANCE].ErrorCode != HAL_MMC_ERROR_NONE) {
        return EMMC_ERROR;
    }

    /* Invalidate cache again after read to ensure CPU sees fresh data */
    SCB_InvalidateDCache_by_Addr((uint32_t *)alignedAddr, alignedSize);

    return EMMC_OK;
}

/**
  * @brief  Write data blocks to eMMC
  * @param  pData      Pointer to data buffer
  * @param  blockAddr  Block address to write to
  * @param  numBlocks  Number of blocks to write
  * @retval EMMC_StatusTypeDef
  */
EMMC_StatusTypeDef EMMC_WriteBlocks(const uint8_t *pData, uint32_t blockAddr, uint32_t numBlocks)
{
    int32_t bsp_status;
    uint32_t alignedAddr;
    uint32_t alignedSize;
    extern MMC_HandleTypeDef hsd_sdmmc[];

    if (!emmc_initialized) {
        return EMMC_NOT_INITIALIZED;
    }

    if (pData == NULL || numBlocks == 0) {
        return EMMC_ERROR;
    }

    /* Wait for peripheral to be ready before starting a new write.
     * This is critical when FatFs issues back-to-back writes during format.
     * Without this check, BSP_MMC_WriteBlocks_DMA() will return BSP_ERROR_BUSY. */
    uint32_t tickstart = HAL_GetTick();
    uint32_t ready_timeout = 1000; /* 1 second should be plenty for peripheral to settle */

    while (HAL_MMC_GetState(&hsd_sdmmc[EMMC_INSTANCE]) != HAL_MMC_STATE_READY) {
        if ((HAL_GetTick() - tickstart) >= ready_timeout) {
            return EMMC_BUSY;
        }
        /* Very small busy-wait to allow interrupts */
        for (volatile int i = 0; i < 100; i++);
    }

    /* Calculate aligned address and size for cache operations
     * STM32H7 D-Cache line size is 32 bytes */
    alignedAddr = (uint32_t)pData & ~31U;
    alignedSize = ((uint32_t)pData - alignedAddr) + (numBlocks * 512);
    alignedSize = (alignedSize + 31) & ~31U;

    /* Clean cache before write to ensure peripheral reads correct data from RAM */
    SCB_CleanDCache_by_Addr((uint32_t *)alignedAddr, alignedSize);

    /* Use DMA mode - STM32H7 SDMMC polling mode doesn't work reliably */
    bsp_status = BSP_MMC_WriteBlocks_DMA(EMMC_INSTANCE, (uint32_t *)pData, blockAddr, numBlocks);

    if (bsp_status != BSP_ERROR_NONE) {

        /* If CRC error, clear it and retry once */
        if (hsd_sdmmc[EMMC_INSTANCE].ErrorCode == HAL_MMC_ERROR_CMD_CRC_FAIL) {
            /* Clear error state */
            hsd_sdmmc[EMMC_INSTANCE].ErrorCode = HAL_MMC_ERROR_NONE;

            /* Small delay to let peripheral settle */
            for (volatile int i = 0; i < 10000; i++);

            /* Retry the write */
            bsp_status = BSP_MMC_WriteBlocks_DMA(EMMC_INSTANCE, (uint32_t *)pData, blockAddr, numBlocks);

            if (bsp_status == BSP_ERROR_NONE) {
                /* Retry succeeded - continue to wait for completion */
                goto wait_for_completion;
            }
        }

        return EMMC_ERROR;
    }

wait_for_completion:

    /* Wait for DMA transfer to complete - DATAEND interrupt sets state to READY */
    tickstart = HAL_GetTick();  /* Reuse tickstart variable from earlier */
    uint32_t timeout = 5000; /* 5 second timeout for writes (format does many) */

    while (HAL_MMC_GetState(&hsd_sdmmc[EMMC_INSTANCE]) != HAL_MMC_STATE_READY) {
        if ((HAL_GetTick() - tickstart) >= timeout) {
            return EMMC_TIMEOUT;
        }
        /* Very small busy-wait to allow interrupts without blocking LVGL */
        for (volatile int i = 0; i < 100; i++);
    }

    /* Check if there was an error during the transfer */
    if (hsd_sdmmc[EMMC_INSTANCE].ErrorCode != HAL_MMC_ERROR_NONE) {
        return EMMC_ERROR;
    }

    return EMMC_OK;
}

/**
  * @brief  Read data blocks from eMMC using DMA
  * @param  pData      Pointer to data buffer
  * @param  blockAddr  Block address to read from
  * @param  numBlocks  Number of blocks to read
  * @retval EMMC_StatusTypeDef
  */
EMMC_StatusTypeDef EMMC_ReadBlocks_DMA(uint8_t *pData, uint32_t blockAddr, uint32_t numBlocks)
{
    int32_t bsp_status;

    if (!emmc_initialized) {
        return EMMC_NOT_INITIALIZED;
    }

    if (pData == NULL || numBlocks == 0) {
        return EMMC_ERROR;
    }

    bsp_status = BSP_MMC_ReadBlocks_DMA(EMMC_INSTANCE, (uint32_t *)pData, blockAddr, numBlocks);

    if (bsp_status != BSP_ERROR_NONE) {
        return EMMC_ERROR;
    }

    return EMMC_OK;
}

/**
  * @brief  Write data blocks to eMMC using DMA
  * @param  pData      Pointer to data buffer
  * @param  blockAddr  Block address to write to
  * @param  numBlocks  Number of blocks to write
  * @retval EMMC_StatusTypeDef
  */
EMMC_StatusTypeDef EMMC_WriteBlocks_DMA(const uint8_t *pData, uint32_t blockAddr, uint32_t numBlocks)
{
    int32_t bsp_status;

    if (!emmc_initialized) {
        return EMMC_NOT_INITIALIZED;
    }

    if (pData == NULL || numBlocks == 0) {
        return EMMC_ERROR;
    }

    bsp_status = BSP_MMC_WriteBlocks_DMA(EMMC_INSTANCE, (uint32_t *)pData, blockAddr, numBlocks);

    if (bsp_status != BSP_ERROR_NONE) {
        return EMMC_ERROR;
    }

    return EMMC_OK;
}

/**
  * @brief  Erase blocks on eMMC
  * @param  startAddr  Start block address
  * @param  endAddr    End block address
  * @retval EMMC_StatusTypeDef
  */
EMMC_StatusTypeDef EMMC_Erase(uint32_t startAddr, uint32_t endAddr)
{
    int32_t bsp_status;

    if (!emmc_initialized) {
        return EMMC_NOT_INITIALIZED;
    }

    if (startAddr > endAddr) {
        return EMMC_ERROR;
    }

    bsp_status = BSP_MMC_Erase(EMMC_INSTANCE, startAddr, endAddr);

    if (bsp_status != BSP_ERROR_NONE) {
        return EMMC_ERROR;
    }

    return EMMC_OK;
}

/**
  * @brief  Get eMMC card information
  * @param  pCardInfo  Pointer to card info structure
  * @retval EMMC_StatusTypeDef
  */
EMMC_StatusTypeDef EMMC_GetCardInfo(EMMC_CardInfoTypeDef *pCardInfo)
{
    int32_t bsp_status;

    if (!emmc_initialized) {
        return EMMC_NOT_INITIALIZED;
    }

    if (pCardInfo == NULL) {
        return EMMC_ERROR;
    }

    /* EMMC_CardInfoTypeDef is typedef'd to BSP_MMC_CardInfo (which is HAL_MMC_CardInfoTypeDef) */
    /* So we can pass pCardInfo directly without needing to copy fields */
    bsp_status = BSP_MMC_GetCardInfo(EMMC_INSTANCE, (BSP_MMC_CardInfo *)pCardInfo);

    if (bsp_status != BSP_ERROR_NONE) {
        return EMMC_ERROR;
    }

    return EMMC_OK;
}

/**
  * @brief  Get eMMC card state
  * @retval EMMC_StatusTypeDef
  */
EMMC_StatusTypeDef EMMC_GetCardState(void)
{
    int32_t state;

    if (!emmc_initialized) {
        return EMMC_NOT_INITIALIZED;
    }

    state = BSP_MMC_GetCardState(EMMC_INSTANCE);

    if (state == MMC_TRANSFER_OK) {
        return EMMC_OK;
    } else if (state == MMC_TRANSFER_BUSY) {
        return EMMC_BUSY;
    } else {
        return EMMC_ERROR;
    }
}

/**
  * @brief  Get total capacity in bytes
  * @retval Total capacity in bytes
  */
uint64_t EMMC_GetCapacity(void)
{
    EMMC_CardInfoTypeDef card_info;

    if (EMMC_GetCardInfo(&card_info) != EMMC_OK) {
        return 0;
    }

    return ((uint64_t)card_info.BlockNbr * (uint64_t)card_info.BlockSize);
}

/* Weak callback implementations ---------------------------------------------*/

/**
  * @brief  Read complete callback - called from interrupt context
  * @note   This is a weak implementation that can be overridden by user
  */
__weak void EMMC_ReadCpltCallback(void)
{
    /* Cache is invalidated in the main read function after transfer completes */
}

/**
  * @brief  Write complete callback
  * @note   This is a weak implementation that can be overridden by user
  */
__weak void EMMC_WriteCpltCallback(void)
{
    /* User can add specific code here */
}

/**
  * @brief  Abort callback
  * @note   This is a weak implementation that can be overridden by user
  */
__weak void EMMC_AbortCallback(void)
{
    /* User can add specific code here */
}

/* BSP callback implementations ----------------------------------------------*/

/**
  * @brief  BSP MMC read complete callback
  */
void BSP_MMC_ReadCpltCallback(uint32_t Instance)
{
    if (Instance == EMMC_INSTANCE) {
        EMMC_ReadCpltCallback();
    }
}

/**
  * @brief  BSP MMC write complete callback
  */
void BSP_MMC_WriteCpltCallback(uint32_t Instance)
{
    if (Instance == EMMC_INSTANCE) {
        EMMC_WriteCpltCallback();
    }
}

/**
  * @brief  BSP MMC abort callback
  */
void BSP_MMC_AbortCallback(uint32_t Instance)
{
    if (Instance == EMMC_INSTANCE) {
        EMMC_AbortCallback();
    }
}
