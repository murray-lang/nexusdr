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
#include <ArduinoJson.h>


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

#include "config/RadioConfig.h"

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

#define LVGL_TASK_PRIORITY			( configMAX_PRIORITIES - 2 )

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

Config::Radio::Fields radioConfig;

char MMCPath[4]; /* SD card logical drive path */

static void prvLvglTask( void *pvParameters );

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
  // Note that the CM4 initialises GPIO, and needs to finish that before this CM7
  // can use the USB
  int32_t timeout = 0xFFFF;
  while((__HAL_RCC_GET_FLAG(RCC_FLAG_D2CKRDY) != RESET) && (timeout-- > 0));

  if ( timeout < 0 )
  {
    Error_Handler();
  }
  HAL_Init();

  /* Configure the system clock */
  SystemClock_Config();
  PeriphCommonClock_Config();
  SystemCoreClockUpdate();
  __HAL_RCC_HSEM_CLK_ENABLE(); // Again after SystemCoreClockUpdate()

  MX_GPIO_Init();

  MX_QUADSPI_Init();
  //CRITICAL: Init QSPI Memory FIRST - application code is in QSPI flash
  // BSP_QSPI_Init_t qspi_init;
  // qspi_init.InterfaceMode = MT25TL01G_QPI_MODE;
  // qspi_init.TransferRate = MT25TL01G_DTR_TRANSFER;
  // qspi_init.DualFlashMode = MT25TL01G_DUALFLASH_ENABLE;
  // BSP_QSPI_Init(0, &qspi_init);
  // BSP_QSPI_EnableMemoryMappedMode(0);

  UART_Config();

  // MX_GPIO_Init();
  MX_DMA_Init();
  MX_FMC_Init();

  // MX_SDMMC1_MMC_Init();
  MX_USB_OTG_FS_USB_Init();
  // UART_Config();

  USB_Manager_Init();
  // MX_SDMMC1_MMC_Init();

  LOCK_HSEM(HSEM_ID_0);
  UNLOCK_HSEM(HSEM_ID_0);// This signals the CM4 to wake up

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

  if(FATFS_LinkDriver(&MMC_Driver, MMCPath) == 0) {
    SAFE_PRINTF("[CM7]:\tstarting FatFs operation....\r\n");
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
        JsonDocument doc;
        DeserializationError error = deserializeJson(doc, reinterpret_cast<const char*>(configJson));
        if (error == DeserializationError::Ok) {
          if (doc["testMessage"]) {
            SAFE_PRINTF("[CM7]:\tTest message: %s\r\n", doc["testMessage"].as<const char*>());
          } else {
            SAFE_PRINTF("[CM7]:\tNo test message found in config file\r\n");
          }
        } else {
          SAFE_PRINTF("Error parsing JSON config: %u", error.code());
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

	/* Toggle LEDs to show demo loaded */
	BSP_LED_Off(LED1);

  // /* AIEC Common configuration: make CPU1 and CPU2 SWI line0
  // sensitive to rising edge : Configured only once */
  // HAL_EXTI_EdgeConfig(EXTI_LINE0 , EXTI_RISING_EDGE);
  // /* USER CODE END 2 */
#ifdef USE_FREERTOS
  BaseType_t rc = xTaskCreate( prvLvglTask, "LVGL", 2048, NULL, LVGL_TASK_PRIORITY, NULL );
  if (rc == pdPASS) {
    SAFE_PRINTF("[CM7]\txTaskCreate() succeeded\r\n");
  } else {
    SAFE_PRINTF("[CM7]\txTaskCreate() returned: %d", rc);
  }
  vTaskStartScheduler();

#endif
  /* Infinite loop (never reached because vTaskStartScheduler() never returns)*/
  /* USER CODE BEGIN WHILE */
	while (1)
	{
	  // HAL_Delay(500);
	  //vTaskDelay(pdMS_TO_TICKS(500));
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
#ifndef USE_FREERTOS
		 tud_task();  // CRITICAL: Process USB events - must be called frequently!
		 lv_timer_handler();
#endif
	}
  /* USER CODE END 3 */
}

// void vApplicationTickHook(void)
// {
//   /* Calls lv_tick_inc with 1ms increment */
//   // lv_tick_inc(1);
//   // BSP_LED_On(LED2);
//   HAL_IncTick();
// }

static void prvLvglTask( void *pvParameters )
{
  SAFE_PRINTF("[CM7]:\tInitializing LVGL and LCD...\r\n");
  lv_init();
  lv_tick_set_cb(HAL_GetTick);
  LCD_init();
  SAFE_PRINTF("[CM7]:\tLCD initialized\r\n");

  touchpad_init();

  lv_demo_widgets();
  for (;;) {
    tud_task();  // Here just for now
    lv_timer_handler();
    // lv_sleep_ms(10);
    // BSP_LED_Toggle(LED2);
  }
}

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

