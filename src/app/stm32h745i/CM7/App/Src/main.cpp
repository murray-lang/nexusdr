/* USER CODE BEGIN Header */
/**
 ******************************************************************************
 * @file           : main.c
 * @brief          : Main program body
 ******************************************************************************
 * @attention
 *
 * Copyright (c) 2021 STMicroelectronics.
 * All rights reserved.
 *
 * This software is licensed under terms that can be found in the LICENSE file
 * in the root directory of this software component.
 * If no LICENSE file comes with this software, it is provided AS-IS.
 *
 ******************************************************************************
 */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/

// extern "C" {
#include "main.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "stm32h745i_discovery_qspi.h"
#include "stm32h745i_discovery_sdram.h"
#include "lvgl/lvgl.h"
#include "lvgl_port_lcd.h"
#include "lvgl_port_touchpad.h"
#include "lvgl/demos/lv_demos.h"
// }
#include <misc_utils.h>
#include <mpu_config.h>
#include <stdio.h>
#include <stm32h745i_discovery_mmc.h>
#include <string.h>
#include <usb_manager.h>
#include <device/usbd.h>

#include "integer.h"
#include "nlohmann/json.hpp"

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

#ifndef HSEM_ID_0
#define HSEM_ID_0 (0U) /* HW semaphore 0*/
#endif
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

//DMA2D_HandleTypeDef hdma2d;
//
//I2C_HandleTypeDef hi2c4;
//
//LTDC_HandleTypeDef hltdc;
//
//QSPI_HandleTypeDef hqspi;
//
//TIM_HandleTypeDef htim8;
//
//UART_HandleTypeDef huart3;
//
//MDMA_HandleTypeDef hmdma_mdma_channel40_sw_0;
//SDRAM_HandleTypeDef hsdram1;

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
//static void MX_GPIO_Init(void);
//static void MX_FMC_Init(void);
//static void MX_LTDC_Init(void);
//static void MX_QUADSPI_Init(void);
//static void MX_MDMA_Init(void);
//static void MX_I2C4_Init(void);
//static void MX_DMA2D_Init(void);
//static void MX_TIM8_Init(void);
//static void MX_USART3_UART_Init(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

// Redirect printf to ITM
// int _write(int file, char *ptr, int len) {
//   for(int i = 0; i < len; i++) {
//     ITM_SendChar((*ptr++));
//   }
//   return len;
// }

/* USER CODE END 0 */


char MMCPath[4]; /* SD card logical drive path */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  BSP_LED_Init(LED1);
  BSP_LED_Init(LED2);

  MPU_Config();
  CPU_CACHE_Enable();

  /* Boot CM4 core */
  HAL_RCCEx_EnableBootCore(RCC_BOOT_C2);

  /* Immediately take HSEM before CM4 gets to STOP mode */
  __HAL_RCC_HSEM_CLK_ENABLE();
  HAL_HSEM_FastTake(HSEM_ID_0);  // Take it ASAP

  /* Wait until CPU2 boots and enters in stop mode or timeout*/
  int32_t timeout = 0xFFFF;
  while((__HAL_RCC_GET_FLAG(RCC_FLAG_D2CKRDY) != RESET) && (timeout-- > 0));
  if ( timeout < 0 )
  {
    Error_Handler();
  }

  HAL_Init();

  /* Configure the system clock */
  SystemClock_Config();
  SystemCoreClockUpdate();

  __HAL_RCC_HSEM_CLK_ENABLE();

	//CRITICAL: Init QSPI Memory FIRST - application code is in QSPI flash
	BSP_QSPI_Init_t qspi_init;
	qspi_init.InterfaceMode = MT25TL01G_QPI_MODE;
	qspi_init.TransferRate = MT25TL01G_DTR_TRANSFER;
	qspi_init.DualFlashMode = MT25TL01G_DUALFLASH_ENABLE;
	BSP_QSPI_Init(0, &qspi_init);
	BSP_QSPI_EnableMemoryMappedMode(0);

  UART_Config();
  USB_Manager_Init();

  /* Initialize MMC hardware for USB MSC access */
  SAFE_PRINTF("[CM7]:\tInitializing eMMC...\r\n");
  if (BSP_MMC_Init(0) != BSP_ERROR_NONE) {
    SAFE_PRINTF("[CM7]:\tERROR: eMMC initialization failed!\r\n");
    Error_Handler();
  }
  SAFE_PRINTF("[CM7]:\teMMC initialized successfully\r\n");

  SAFE_PRINTF("[CM7]:\tInitializing USB Device...\r\n");
  USB_Device_Init();

  /* Wait for USB enumeration */
  uint32_t enum_start = HAL_GetTick();
  while (HAL_GetTick() - enum_start < 2000) {
    tud_task();
  }
  SAFE_PRINTF("[CM7]:\tUSB enumeration complete\r\n");

  /* Initialize LVGL and LCD */
  SAFE_PRINTF("[CM7]:\tInitializing LVGL and LCD...\r\n");
  lv_init();
  lv_tick_set_cb(HAL_GetTick);
  LCD_init();
  SAFE_PRINTF("[CM7]:\tLCD initialized\r\n");

	touchpad_init();

  if(FATFS_LinkDriver(&MMC_Driver, MMCPath) == 0) {
    SAFE_PRINTF("[CM4]:\tstarting FatFs operation....\r\n");
    /* Request FatFs access (will block if USB is connected) */
    if (USB_Manager_RequestFatFsAccess() != 0)
    {
      SAFE_PRINTF("[CM7]:\tFailed to mount FatFs\r\n");
      Error_Handler();
    }
    SAFE_PRINTF("[CM7]:\tMounted FatFs\r\n");
    FIL configFile;
    FRESULT res = f_open(&configFile, "nexusdr.json", FA_READ);
    SAFE_PRINTF("[CM7]:\tReturned from f_open()\r\n");
    if(res == FR_OK)
    {
      UINT bytesRead;
      uint8_t configJson[1024];
      res = f_read(&configFile, configJson, sizeof(configJson), &bytesRead);
      if((bytesRead > 0) || (res == FR_OK))
      {
        std::string jsonString(reinterpret_cast<const char*>(configJson));
        nlohmann::json config = nlohmann::json::parse(jsonString);
        if (config.contains("testMessage")) {
          SAFE_PRINTF("[CM7]:\tTest message: %s\r\n", config["testMessage"].get<std::string>().c_str());
        } else {
          SAFE_PRINTF("[CM7]:\tNo test message found in config file\r\n");
        }
      } else {
        SAFE_PRINTF("[CM7]:\tError reading config file: %u\r\n", res);
        Error_Handler();
      }

      f_close(&configFile);
    } else {
      SAFE_PRINTF("[CM7]:\tError opening config file: %u\r\n", res);
      Error_Handler();
    }
    USB_Manager_ReleaseFatFsAccess();
  }

	lv_demo_widgets();

	/* Toggle LEDs to show demo loaded */
	BSP_LED_Off(LED1);
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
	while (1)
	{
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */

		tud_task();  // CRITICAL: Process USB events - must be called frequently!
		lv_timer_handler();
	  // HAL_Delay(2);
	}
  /* USER CODE END 3 */
}



///**
//  * @brief DMA2D Initialization Function
//  * @param None
//  * @retval None
//  */
//static void MX_DMA2D_Init(void)
//{
//
//  /* USER CODE BEGIN DMA2D_Init 0 */
//
//  /* USER CODE END DMA2D_Init 0 */
//
//  /* USER CODE BEGIN DMA2D_Init 1 */
//
//  /* USER CODE END DMA2D_Init 1 */
//  hdma2d.Instance = DMA2D;
//  hdma2d.Init.Mode = DMA2D_M2M;
//  hdma2d.Init.ColorMode = DMA2D_OUTPUT_ARGB8888;
//  hdma2d.Init.OutputOffset = 0;
//  hdma2d.LayerCfg[1].InputOffset = 0;
//  hdma2d.LayerCfg[1].InputColorMode = DMA2D_INPUT_ARGB8888;
//  hdma2d.LayerCfg[1].AlphaMode = DMA2D_NO_MODIF_ALPHA;
//  hdma2d.LayerCfg[1].InputAlpha = 0;
//  hdma2d.LayerCfg[1].AlphaInverted = DMA2D_REGULAR_ALPHA;
//  hdma2d.LayerCfg[1].RedBlueSwap = DMA2D_RB_REGULAR;
//  hdma2d.LayerCfg[1].ChromaSubSampling = DMA2D_NO_CSS;
//  if (HAL_DMA2D_Init(&hdma2d) != HAL_OK)
//  {
//    Error_Handler();
//  }
//  if (HAL_DMA2D_ConfigLayer(&hdma2d, 1) != HAL_OK)
//  {
//    Error_Handler();
//  }
//  /* USER CODE BEGIN DMA2D_Init 2 */
//
//  /* USER CODE END DMA2D_Init 2 */
//
//}
//
///**
//  * @brief I2C4 Initialization Function
//  * @param None
//  * @retval None
//  */
//static void MX_I2C4_Init(void)
//{
//
//  /* USER CODE BEGIN I2C4_Init 0 */
//
//  /* USER CODE END I2C4_Init 0 */
//
//  /* USER CODE BEGIN I2C4_Init 1 */
//
//  /* USER CODE END I2C4_Init 1 */
//  hi2c4.Instance = I2C4;
//  hi2c4.Init.Timing = 0x307075B1;
//  hi2c4.Init.OwnAddress1 = 0;
//  hi2c4.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
//  hi2c4.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
//  hi2c4.Init.OwnAddress2 = 0;
//  hi2c4.Init.OwnAddress2Masks = I2C_OA2_NOMASK;
//  hi2c4.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
//  hi2c4.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
//  if (HAL_I2C_Init(&hi2c4) != HAL_OK)
//  {
//    Error_Handler();
//  }
//  /** Configure Analogue filter
//  */
//  if (HAL_I2CEx_ConfigAnalogFilter(&hi2c4, I2C_ANALOGFILTER_ENABLE) != HAL_OK)
//  {
//    Error_Handler();
//  }
//  /** Configure Digital filter
//  */
//  if (HAL_I2CEx_ConfigDigitalFilter(&hi2c4, 0) != HAL_OK)
//  {
//    Error_Handler();
//  }
//  /* USER CODE BEGIN I2C4_Init 2 */
//
//  /* USER CODE END I2C4_Init 2 */
//
//}
//
///**
//  * @brief LTDC Initialization Function
//  * @param None
//  * @retval None
//  */
//static void MX_LTDC_Init(void)
//{
//
//  /* USER CODE BEGIN LTDC_Init 0 */
//
//  /* USER CODE END LTDC_Init 0 */
//
//  LTDC_LayerCfgTypeDef pLayerCfg = {0};
//  LTDC_LayerCfgTypeDef pLayerCfg1 = {0};
//
//  /* USER CODE BEGIN LTDC_Init 1 */
//
//  /* USER CODE END LTDC_Init 1 */
//  hltdc.Instance = LTDC;
//  hltdc.Init.HSPolarity = LTDC_HSPOLARITY_AL;
//  hltdc.Init.VSPolarity = LTDC_VSPOLARITY_AL;
//  hltdc.Init.DEPolarity = LTDC_DEPOLARITY_AL;
//  hltdc.Init.PCPolarity = LTDC_PCPOLARITY_IPC;
//  hltdc.Init.HorizontalSync = 40;
//  hltdc.Init.VerticalSync = 8;
//  hltdc.Init.AccumulatedHBP = 53;
//  hltdc.Init.AccumulatedVBP = 10;
//  hltdc.Init.AccumulatedActiveW = 533;
//  hltdc.Init.AccumulatedActiveH = 282;
//  hltdc.Init.TotalWidth = 565;
//  hltdc.Init.TotalHeigh = 284;
//  hltdc.Init.Backcolor.Blue = 255;
//  hltdc.Init.Backcolor.Green = 255;
//  hltdc.Init.Backcolor.Red = 255;
//  if (HAL_LTDC_Init(&hltdc) != HAL_OK)
//  {
//    Error_Handler();
//  }
//  pLayerCfg.WindowX0 = 0;
//  pLayerCfg.WindowX1 = 480;
//  pLayerCfg.WindowY0 = 0;
//  pLayerCfg.WindowY1 = 272;
//  pLayerCfg.PixelFormat = LTDC_PIXEL_FORMAT_ARGB8888;
//  pLayerCfg.Alpha = 255;
//  pLayerCfg.Alpha0 = 0;
//  pLayerCfg.BlendingFactor1 = LTDC_BLENDING_FACTOR1_PAxCA;
//  pLayerCfg.BlendingFactor2 = LTDC_BLENDING_FACTOR2_PAxCA;
//  pLayerCfg.FBStartAdress = 0;
//  pLayerCfg.ImageWidth = 480;
//  pLayerCfg.ImageHeight = 272;
//  pLayerCfg.Backcolor.Blue = 0;
//  pLayerCfg.Backcolor.Green = 0;
//  pLayerCfg.Backcolor.Red = 0;
//  if (HAL_LTDC_ConfigLayer(&hltdc, &pLayerCfg, 0) != HAL_OK)
//  {
//    Error_Handler();
//  }
//  pLayerCfg1.WindowX0 = 0;
//  pLayerCfg1.WindowX1 = 0;
//  pLayerCfg1.WindowY0 = 0;
//  pLayerCfg1.WindowY1 = 0;
//  pLayerCfg1.PixelFormat = LTDC_PIXEL_FORMAT_ARGB8888;
//  pLayerCfg1.Alpha = 0;
//  pLayerCfg1.Alpha0 = 0;
//  pLayerCfg1.BlendingFactor1 = LTDC_BLENDING_FACTOR1_CA;
//  pLayerCfg1.BlendingFactor2 = LTDC_BLENDING_FACTOR2_CA;
//  pLayerCfg1.FBStartAdress = 0;
//  pLayerCfg1.ImageWidth = 0;
//  pLayerCfg1.ImageHeight = 0;
//  pLayerCfg1.Backcolor.Blue = 0;
//  pLayerCfg1.Backcolor.Green = 0;
//  pLayerCfg1.Backcolor.Red = 0;
//  if (HAL_LTDC_ConfigLayer(&hltdc, &pLayerCfg1, 1) != HAL_OK)
//  {
//    Error_Handler();
//  }
//  /* USER CODE BEGIN LTDC_Init 2 */
//
//  /* USER CODE END LTDC_Init 2 */
//
//}
//
///**
//  * @brief QUADSPI Initialization Function
//  * @param None
//  * @retval None
//  */
//static void MX_QUADSPI_Init(void)
//{
//
//  /* USER CODE BEGIN QUADSPI_Init 0 */
//
//  /* USER CODE END QUADSPI_Init 0 */
//
//  /* USER CODE BEGIN QUADSPI_Init 1 */
//
//  /* USER CODE END QUADSPI_Init 1 */
//  /* QUADSPI parameter configuration*/
//  hqspi.Instance = QUADSPI;
//  hqspi.Init.ClockPrescaler = 3;
//  hqspi.Init.FifoThreshold = 1;
//  hqspi.Init.SampleShifting = QSPI_SAMPLE_SHIFTING_NONE;
//  hqspi.Init.FlashSize = 26;
//  hqspi.Init.ChipSelectHighTime = QSPI_CS_HIGH_TIME_4_CYCLE;
//  hqspi.Init.ClockMode = QSPI_CLOCK_MODE_0;
//  hqspi.Init.DualFlash = QSPI_DUALFLASH_ENABLE;
//  if (HAL_QSPI_Init(&hqspi) != HAL_OK)
//  {
//    Error_Handler();
//  }
//  /* USER CODE BEGIN QUADSPI_Init 2 */
//
//  /* USER CODE END QUADSPI_Init 2 */
//
//}
//
///**
//  * @brief TIM8 Initialization Function
//  * @param None
//  * @retval None
//  */
//static void MX_TIM8_Init(void)
//{
//
//  /* USER CODE BEGIN TIM8_Init 0 */
//
//  /* USER CODE END TIM8_Init 0 */
//
//  TIM_MasterConfigTypeDef sMasterConfig = {0};
//  TIM_OC_InitTypeDef sConfigOC = {0};
//  TIM_BreakDeadTimeConfigTypeDef sBreakDeadTimeConfig = {0};
//
//  /* USER CODE BEGIN TIM8_Init 1 */
//
//  /* USER CODE END TIM8_Init 1 */
//  htim8.Instance = TIM8;
//  htim8.Init.Prescaler = 4;
//  htim8.Init.CounterMode = TIM_COUNTERMODE_UP;
//  htim8.Init.Period = 50000;
//  htim8.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
//  htim8.Init.RepetitionCounter = 0;
//  htim8.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
//  if (HAL_TIM_PWM_Init(&htim8) != HAL_OK)
//  {
//    Error_Handler();
//  }
//  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
//  sMasterConfig.MasterOutputTrigger2 = TIM_TRGO2_RESET;
//  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
//  if (HAL_TIMEx_MasterConfigSynchronization(&htim8, &sMasterConfig) != HAL_OK)
//  {
//    Error_Handler();
//  }
//  sConfigOC.OCMode = TIM_OCMODE_PWM1;
//  sConfigOC.Pulse = 0;
//  sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
//  sConfigOC.OCNPolarity = TIM_OCNPOLARITY_HIGH;
//  sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
//  sConfigOC.OCIdleState = TIM_OCIDLESTATE_RESET;
//  sConfigOC.OCNIdleState = TIM_OCNIDLESTATE_RESET;
//  if (HAL_TIM_PWM_ConfigChannel(&htim8, &sConfigOC, TIM_CHANNEL_3) != HAL_OK)
//  {
//    Error_Handler();
//  }
//  sBreakDeadTimeConfig.OffStateRunMode = TIM_OSSR_DISABLE;
//  sBreakDeadTimeConfig.OffStateIDLEMode = TIM_OSSI_DISABLE;
//  sBreakDeadTimeConfig.LockLevel = TIM_LOCKLEVEL_OFF;
//  sBreakDeadTimeConfig.DeadTime = 0;
//  sBreakDeadTimeConfig.BreakState = TIM_BREAK_DISABLE;
//  sBreakDeadTimeConfig.BreakPolarity = TIM_BREAKPOLARITY_HIGH;
//  sBreakDeadTimeConfig.BreakFilter = 0;
//  sBreakDeadTimeConfig.Break2State = TIM_BREAK2_DISABLE;
//  sBreakDeadTimeConfig.Break2Polarity = TIM_BREAK2POLARITY_HIGH;
//  sBreakDeadTimeConfig.Break2Filter = 0;
//  sBreakDeadTimeConfig.AutomaticOutput = TIM_AUTOMATICOUTPUT_DISABLE;
//  if (HAL_TIMEx_ConfigBreakDeadTime(&htim8, &sBreakDeadTimeConfig) != HAL_OK)
//  {
//    Error_Handler();
//  }
//  /* USER CODE BEGIN TIM8_Init 2 */
//
//  /* USER CODE END TIM8_Init 2 */
//  HAL_TIM_MspPostInit(&htim8);
//
//}
//
///**
//  * @brief USART3 Initialization Function
//  * @param None
//  * @retval None
//  */
//static void MX_USART3_UART_Init(void)
//{
//
//  /* USER CODE BEGIN USART3_Init 0 */
//
//  /* USER CODE END USART3_Init 0 */
//
//  /* USER CODE BEGIN USART3_Init 1 */
//
//  /* USER CODE END USART3_Init 1 */
//  huart3.Instance = USART3;
//  huart3.Init.BaudRate = 115200;
//  huart3.Init.WordLength = UART_WORDLENGTH_8B;
//  huart3.Init.StopBits = UART_STOPBITS_1;
//  huart3.Init.Parity = UART_PARITY_NONE;
//  huart3.Init.Mode = UART_MODE_TX_RX;
//  huart3.Init.HwFlowCtl = UART_HWCONTROL_NONE;
//  huart3.Init.OverSampling = UART_OVERSAMPLING_16;
//  huart3.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
//  huart3.Init.ClockPrescaler = UART_PRESCALER_DIV1;
//  huart3.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;
//  if (HAL_UART_Init(&huart3) != HAL_OK)
//  {
//    Error_Handler();
//  }
//  if (HAL_UARTEx_SetTxFifoThreshold(&huart3, UART_TXFIFO_THRESHOLD_1_8) != HAL_OK)
//  {
//    Error_Handler();
//  }
//  if (HAL_UARTEx_SetRxFifoThreshold(&huart3, UART_RXFIFO_THRESHOLD_1_8) != HAL_OK)
//  {
//    Error_Handler();
//  }
//  if (HAL_UARTEx_DisableFifoMode(&huart3) != HAL_OK)
//  {
//    Error_Handler();
//  }
//  /* USER CODE BEGIN USART3_Init 2 */
//
//  /* USER CODE END USART3_Init 2 */
//
//}
//
///**
//  * Enable MDMA controller clock
//  * Configure MDMA for global transfers
//  *   hmdma_mdma_channel40_sw_0
//  */
//static void MX_MDMA_Init(void)
//{
//
//  /* MDMA controller clock enable */
//  __HAL_RCC_MDMA_CLK_ENABLE();
//  /* Local variables */
//
//  /* Configure MDMA channel MDMA_Channel0 */
//  /* Configure MDMA request hmdma_mdma_channel40_sw_0 on MDMA_Channel0 */
//  hmdma_mdma_channel40_sw_0.Instance = MDMA_Channel0;
//  hmdma_mdma_channel40_sw_0.Init.Request = MDMA_REQUEST_SW;
//  hmdma_mdma_channel40_sw_0.Init.TransferTriggerMode = MDMA_BLOCK_TRANSFER;
//  hmdma_mdma_channel40_sw_0.Init.Priority = MDMA_PRIORITY_HIGH;
//  hmdma_mdma_channel40_sw_0.Init.Endianness = MDMA_LITTLE_ENDIANNESS_PRESERVE;
//  hmdma_mdma_channel40_sw_0.Init.SourceInc = MDMA_SRC_INC_WORD;
//  hmdma_mdma_channel40_sw_0.Init.DestinationInc = MDMA_DEST_INC_WORD;
//  hmdma_mdma_channel40_sw_0.Init.SourceDataSize = MDMA_SRC_DATASIZE_WORD;
//  hmdma_mdma_channel40_sw_0.Init.DestDataSize = MDMA_DEST_DATASIZE_WORD;
//  hmdma_mdma_channel40_sw_0.Init.DataAlignment = MDMA_DATAALIGN_PACKENABLE;
//  hmdma_mdma_channel40_sw_0.Init.BufferTransferLength = 128;
//  hmdma_mdma_channel40_sw_0.Init.SourceBurst = MDMA_SOURCE_BURST_SINGLE;
//  hmdma_mdma_channel40_sw_0.Init.DestBurst = MDMA_DEST_BURST_SINGLE;
//  hmdma_mdma_channel40_sw_0.Init.SourceBlockAddressOffset = 0;
//  hmdma_mdma_channel40_sw_0.Init.DestBlockAddressOffset = 0;
//  if (HAL_MDMA_Init(&hmdma_mdma_channel40_sw_0) != HAL_OK)
//  {
//    Error_Handler();
//  }
//
//  /* MDMA interrupt initialization */
//  /* MDMA_IRQn interrupt configuration */
//  HAL_NVIC_SetPriority(MDMA_IRQn, 0, 0);
//  HAL_NVIC_EnableIRQ(MDMA_IRQn);
//
//}
//
///* FMC initialization function */
//void MX_FMC_Init(void)
//{
//
//  /* USER CODE BEGIN FMC_Init 0 */
//
//  /* USER CODE END FMC_Init 0 */
//
//  FMC_SDRAM_TimingTypeDef SdramTiming = {0};
//
//  /* USER CODE BEGIN FMC_Init 1 */
//
//  /* USER CODE END FMC_Init 1 */
//
//  /** Perform the SDRAM1 memory initialization sequence
//  */
//  hsdram1.Instance = FMC_SDRAM_DEVICE;
//  /* hsdram1.Init */
//  hsdram1.Init.SDBank = FMC_SDRAM_BANK2;
//  hsdram1.Init.ColumnBitsNumber = FMC_SDRAM_COLUMN_BITS_NUM_8;
//  hsdram1.Init.RowBitsNumber = FMC_SDRAM_ROW_BITS_NUM_12;
//  hsdram1.Init.MemoryDataWidth = FMC_SDRAM_MEM_BUS_WIDTH_16;
//  hsdram1.Init.InternalBankNumber = FMC_SDRAM_INTERN_BANKS_NUM_4;
//  hsdram1.Init.CASLatency = FMC_SDRAM_CAS_LATENCY_3;
//  hsdram1.Init.WriteProtection = FMC_SDRAM_WRITE_PROTECTION_DISABLE;
//  hsdram1.Init.SDClockPeriod = FMC_SDRAM_CLOCK_PERIOD_2;
//  hsdram1.Init.ReadBurst = FMC_SDRAM_RBURST_ENABLE;
//  hsdram1.Init.ReadPipeDelay = FMC_SDRAM_RPIPE_DELAY_0;
//  /* SdramTiming */
//  SdramTiming.LoadToActiveDelay = 2;
//  SdramTiming.ExitSelfRefreshDelay = 7;
//  SdramTiming.SelfRefreshTime = 4;
//  SdramTiming.RowCycleDelay = 7;
//  SdramTiming.WriteRecoveryTime = 3;
//  SdramTiming.RPDelay = 2;
//  SdramTiming.RCDDelay = 2;
//
//  if (HAL_SDRAM_Init(&hsdram1, &SdramTiming) != HAL_OK)
//  {
//    Error_Handler( );
//  }
//
//  /* USER CODE BEGIN FMC_Init 2 */
//
//  /* USER CODE END FMC_Init 2 */
//}
//
///**
//  * @brief GPIO Initialization Function
//  * @param None
//  * @retval None
//  */
//static void MX_GPIO_Init(void)
//{
//  GPIO_InitTypeDef GPIO_InitStruct = {0};
//
//  /* GPIO Ports Clock Enable */
//  __HAL_RCC_GPIOK_CLK_ENABLE();
//  __HAL_RCC_GPIOG_CLK_ENABLE();
//  __HAL_RCC_GPIOI_CLK_ENABLE();
//  __HAL_RCC_GPIOE_CLK_ENABLE();
//  __HAL_RCC_GPIOJ_CLK_ENABLE();
//  __HAL_RCC_GPIOC_CLK_ENABLE();
//  __HAL_RCC_GPIOD_CLK_ENABLE();
//  __HAL_RCC_GPIOF_CLK_ENABLE();
//  __HAL_RCC_GPIOH_CLK_ENABLE();
//  __HAL_RCC_GPIOB_CLK_ENABLE();
//
//  /*Configure GPIO pin Output Level */
//  HAL_GPIO_WritePin(LCD_DISPLAY_MODE_GPIO_Port, LCD_DISPLAY_MODE_Pin, GPIO_PIN_SET);
//
//  /*Configure GPIO pin Output Level */
//  HAL_GPIO_WritePin(LD2_GPIO_Port, LD2_Pin, GPIO_PIN_RESET);
//
//  /*Configure GPIO pin Output Level */
//  HAL_GPIO_WritePin(LD1_GPIO_Port, LD1_Pin, GPIO_PIN_RESET);
//
//  /*Configure GPIO pin Output Level */
//  HAL_GPIO_WritePin(LCD_RESET_GPIO_Port, LCD_RESET_Pin, GPIO_PIN_RESET);
//
//  /*Configure GPIO pin : LCD_DISPLAY_MODE_Pin */
//  GPIO_InitStruct.Pin = LCD_DISPLAY_MODE_Pin;
//  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
//  GPIO_InitStruct.Pull = GPIO_NOPULL;
//  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
//  HAL_GPIO_Init(LCD_DISPLAY_MODE_GPIO_Port, &GPIO_InitStruct);
//
//  /*Configure GPIO pin : B1_Pin */
//  GPIO_InitStruct.Pin = B1_Pin;
//  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
//  GPIO_InitStruct.Pull = GPIO_NOPULL;
//  HAL_GPIO_Init(B1_GPIO_Port, &GPIO_InitStruct);
//
//  /*Configure GPIO pin : LD2_Pin */
//  GPIO_InitStruct.Pin = LD2_Pin;
//  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
//  GPIO_InitStruct.Pull = GPIO_NOPULL;
//  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
//  HAL_GPIO_Init(LD2_GPIO_Port, &GPIO_InitStruct);
//
//  /*Configure GPIO pin : LCD_INT_Pin */
//  GPIO_InitStruct.Pin = LCD_INT_Pin;
//  GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING;
//  GPIO_InitStruct.Pull = GPIO_NOPULL;
//  HAL_GPIO_Init(LCD_INT_GPIO_Port, &GPIO_InitStruct);
//
//  /*Configure GPIO pin : LD1_Pin */
//  GPIO_InitStruct.Pin = LD1_Pin;
//  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
//  GPIO_InitStruct.Pull = GPIO_NOPULL;
//  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
//  HAL_GPIO_Init(LD1_GPIO_Port, &GPIO_InitStruct);
//
//  /*Configure GPIO pin : LCD_RESET_Pin */
//  GPIO_InitStruct.Pin = LCD_RESET_Pin;
//  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
//  GPIO_InitStruct.Pull = GPIO_NOPULL;
//  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
//  HAL_GPIO_Init(LCD_RESET_GPIO_Port, &GPIO_InitStruct);
//
//  /* EXTI interrupt init*/
//  HAL_NVIC_SetPriority(EXTI2_IRQn, 0, 0);
//  HAL_NVIC_EnableIRQ(EXTI2_IRQn);
//
//}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */


#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

