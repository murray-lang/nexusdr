/**
  ******************************************************************************
  * @file    FatFs/FatFs_Shared_Device/CM4/Src/main.c
  * @author  MCD Application Team
  * @brief   Main program body for Cortex-M4.
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

/* Includes ------------------------------------------------------------------*/
#include "main.h"

#include <stdio.h>
#include <misc_utils.h>

#include "mpu_config.h"

#ifdef USE_FREERTOS
#ifdef __cplusplus
 extern "C" {
#endif

#include "FreeRTOS.h"
#include "task.h"

#ifdef __cplusplus
 }
#endif
#endif // USE_FREERTOS

/** @addtogroup STM32H7xx_HAL_Examples
  * @{
  */

/** @addtogroup Templates
  * @{
  */

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/


#define CM4_FILE  "cm4.log"

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* DMA buffers - must be 32-byte aligned for cache coherency */
/* Use explicit padding to ensure each buffer is 32-byte aligned */
__attribute__((section(".dma_buffer"), aligned(32))) struct {
  uint8_t workBuffer[512];
  /* workBuffer is 512 bytes = 16*32, next field is auto-aligned */
  uint8_t rtext[512];
  /* rtext is 512 bytes = 16*32, next field is auto-aligned */
  uint8_t wtext[96];  /* Increased from 64 to 96 (3*32) for alignment */
  /* wtext is now 96 bytes = 3*32, total so far = 1120 bytes */
  uint8_t _pad_fatfs[12];  /* Padding so win[] inside FATFS aligns to 32 bytes */
  /* FATFS.win[] is at offset 52 within FATFS, so we need 12 bytes padding (52+12=64, 64%32=0) */
  FATFS MMCFatFs;
  FIL CM4_File __attribute__((aligned(32)));
} dma_buffers;

#define workBuffer (dma_buffers.workBuffer)
#define rtext (dma_buffers.rtext)
#define wtext (dma_buffers.wtext)
#define MMCFatFs (dma_buffers.MMCFatFs)
#define CM4_File (dma_buffers.CM4_File)

char MMCPath[4]; /* SD card logical drive path */

/* Private function prototypes -----------------------------------------------*/
static void FS_FileOperations(void);

static void prvBlinkTask( void *pvParameters );
/* Private functions ---------------------------------------------------------*/


/**
  * @brief  Main program
  * @param  None
  * @retval None
  */
int main(void)
{
  BSP_LED_Init(LED_GREEN);
  BSP_LED_Init(LED_RED);


  HAL_StatusTypeDef halRc = HAL_Init();
  if (halRc != HAL_OK) {
    Error_Handler();
  }
  MPU_Config_Shared();

  /*HW semaphore Clock enable*/
  __HAL_RCC_HSEM_CLK_ENABLE();

  /* Activate HSEM notification for Cortex-M4*/
  HAL_HSEM_ActivateNotification(__HAL_HSEM_SEMID_TO_MASK(HSEM_ID_0));

  /*
    Domain D2 goes to STOP mode (Cortex-M4 in deep-sleep)
  */
  HAL_PWREx_ClearPendingEvent();
  HAL_PWREx_EnterSTOPMode(PWR_MAINREGULATOR_ON, PWR_STOPENTRY_WFE, PWR_D2_DOMAIN);
  //BSP_LED_On(LED_RED);

  /* Clear HSEM flag */
  __HAL_HSEM_CLEAR_FLAG(__HAL_HSEM_SEMID_TO_MASK(HSEM_ID_0));
  SystemCoreClockUpdate();

  UART_Config();  /* USART3 on PB10/PB11 - no conflict with USB on PA11/PA12 */

  MX_GPIO_Init();

  MX_TIM6_Init();
  MX_DMA_Init();
  MX_FMC_Init();
  MX_SAI2_Init();
  MX_I2C4_Init();
  // MX_USART3_UART_Init();
  MX_DAC1_Init();
  MX_ADC1_Init();
  MX_ADC2_Init();
  // MX_USART6_UART_Init();

  // BSP_LED_On(LED_GREEN);

  /*-1- Link the micro SD disk I/O driver */
  // if(FATFS_LinkDriver(&MMC_Driver, MMCPath) == 0)
  // {
  //   SAFE_PRINTF("[CM4]:\tstarting FatFs operation....\r\n");
  //   /* -2- Start the FatFs operation concurrently with CM7 */
  //   // BSP_LED_On(LED_GREEN);
  //   FS_FileOperations();
  //   while(1)
  //   {
  //     BSP_LED_Toggle(LED_GREEN);
  //     HAL_Delay(250);
  //   }
  // }
#ifdef USE_FREERTOS
  BaseType_t rc = xTaskCreate( prvBlinkTask, "Blink", configMINIMAL_STACK_SIZE*5, NULL, tskIDLE_PRIORITY, NULL );
  if (rc == pdPASS) {
    SAFE_PRINTF("[CM4]\txTaskCreate() succeeded\r\n");
  } else {
    SAFE_PRINTF("[CM4]\txTaskCreate() returned: %d", rc);
  }
  vTaskStartScheduler();
#endif

  for (;;) {

  }
}

static void prvBlinkTask( void *pvParameters )
{
  for (;;) {
    vTaskDelay(pdMS_TO_TICKS( 200 ));
    BSP_LED_Toggle(LED_RED);
  }
}

static void FS_FileOperations(void)
{
  FRESULT res;
  uint32_t byteswritten, bytesread;           /* File write/read counts */
  const char *msg = "This is FatFs running on CM4 core";

  /* Initialize wtext buffer */
  for(int i = 0; msg[i] != '\0' && i < sizeof(wtext); i++) {
    wtext[i] = msg[i];
  }

  /* Register the file system object to the FatFs module */
  res = f_mount(&MMCFatFs, (TCHAR const*)MMCPath, 0);
  if( res != FR_OK)
  {
    goto error;
  }

  SAFE_PRINTF("[CM4]:\topening '%s' for writing...\r\n", CM4_FILE);

  /* Create and Open a new text file object with write access */
  res = f_open(&CM4_File, CM4_FILE, FA_CREATE_ALWAYS | FA_WRITE);
  if(res != FR_OK)
  {
    SAFE_PRINTF("[CM4]:\tError %d returned by f_open()\r\n", res);
    goto error;
  }
  // BSP_LED_On(LED_GREEN);
  res = f_write(&CM4_File, wtext, sizeof(wtext), (void *)&byteswritten);

  if((res != FR_OK) || (byteswritten == 0))
  {
    SAFE_PRINTF("[CM4]:\tError %d returned by f_write()\r\n", res);
    f_close(&CM4_File);
    goto error;
  }

  SAFE_PRINTF("[CM4]:\t%s '%lu' bytes written...\r\n", CM4_FILE, byteswritten);

  /* Close the open text file */
  f_close(&CM4_File);

  /* Open the text file object with read access */
  if(f_open(&CM4_File, CM4_FILE, FA_READ) != FR_OK)
  {
    goto error;
  }

  SAFE_PRINTF("[CM4]:\treading '%s' content...\r\n", CM4_FILE);

  /* Read data from the text file */
  res = f_read(&CM4_File, rtext, sizeof(rtext), (void *)&bytesread);

  if((bytesread == 0) || (res != FR_OK))
  {
    SAFE_PRINTF("[CM4]:\tError %d returned by f_read()\r\n", res);
    f_close(&CM4_File);
    goto error;
  }

  /* Close the open text file */
  f_close(&CM4_File);

  /* Compare read data with the expected data */
  if(Buffercmp(rtext, (uint8_t *)wtext,  byteswritten) == 0)
  {
    SAFE_PRINTF("[CM4]:\t%s => '>  %s <'\n", CM4_FILE, rtext);
  }
  else
  {
    goto error;
  }

error:
  Error_Handler();
}

#ifdef  USE_FULL_ASSERT

/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t* file, uint32_t line)
{
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */

  /* Infinite loop */
  while (1)
  {
  }
}
#endif

/**
  * @}
  */

/**
  * @}
  */

