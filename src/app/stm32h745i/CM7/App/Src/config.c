#include "config.h"

#include "misc_utils.h"
#include "tusb.h"

UART_HandleTypeDef     UartHandle;

PUTCHAR_PROTOTYPE
{
  /* Place your implementation of fputc here */
  /* e.g. write a character to the USART1 and Loop until the end of transmission */
  HAL_UART_Transmit(&UartHandle, (uint8_t *)&ch, 1, 0xFFFFF);

  return ch;
}

void UART_Config(void)
{

  UartHandle.Instance        = USARTx;
  UartHandle.Init.BaudRate   = 115200;
  UartHandle.Init.WordLength = UART_WORDLENGTH_8B;
  UartHandle.Init.StopBits   = UART_STOPBITS_1;
  UartHandle.Init.Parity     = UART_PARITY_NONE;
  UartHandle.Init.HwFlowCtl  = UART_HWCONTROL_NONE;
  UartHandle.Init.Mode       = UART_MODE_TX_RX;
  UartHandle.Init.OverSampling = UART_OVERSAMPLING_16;

  HAL_UART_DeInit(&UartHandle);
  if(HAL_UART_Init(&UartHandle) != HAL_OK)
  {
    /* Initialization Error */
    Error_Handler();
  }
}

/**
* @brief  System Clock Configuration
*         The system Clock is configured as follow :
*            System Clock source            = PLL (HSE)
*            SYSCLK(Hz)                     = 400000000 (Cortex-M7 CPU Clock)
*            HCLK(Hz)                       = 200000000 (Cortex-M4 CPU, Bus matrix Clocks)
*            AHB Prescaler                  = 2
*            D1 APB3 Prescaler              = 2 (APB3 Clock  100MHz)
*            D2 APB1 Prescaler              = 2 (APB1 Clock  100MHz)
*            D2 APB2 Prescaler              = 2 (APB2 Clock  100MHz)
*            D3 APB4 Prescaler              = 2 (APB4 Clock  100MHz)
*            HSE Frequency(Hz)              = 25000000
*            PLL_M                          = 5
*            PLL_N                          = 160
*            PLL_P                          = 2
*            PLL_Q                          = 4
*            PLL_R                          = 2
*            VDD(V)                         = 3.3
*            Flash Latency(WS)              = 4
* @param  None
* @retval None
*/
/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};
  RCC_PeriphCLKInitTypeDef PeriphClkInitStruct = {0};
  HAL_StatusTypeDef ret = HAL_OK;

  /** Supply configuration update enable
  */
  HAL_PWREx_ConfigSupply(PWR_DIRECT_SMPS_SUPPLY);
  /** Configure the main internal regulator output voltage
  */
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE0);

  while(!__HAL_PWR_GET_FLAG(PWR_FLAG_VOSRDY)) {}
  /** Macro to configure the PLL clock source
  */
  __HAL_RCC_PLL_PLLSOURCE_CONFIG(RCC_PLLSOURCE_HSE);
  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_BYPASS;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;

  RCC_OscInitStruct.PLL.PLLM = 5;
  RCC_OscInitStruct.PLL.PLLN = 192;
  RCC_OscInitStruct.PLL.PLLP = 2;
  RCC_OscInitStruct.PLL.PLLQ = 8;
  RCC_OscInitStruct.PLL.PLLR = 4;

  RCC_OscInitStruct.PLL.PLLRGE = RCC_PLL1VCIRANGE_2;
  RCC_OscInitStruct.PLL.PLLVCOSEL = RCC_PLL1VCOWIDE;
  RCC_OscInitStruct.PLL.PLLFRACN = 0;

  // FatFs with eMMC working version
  // RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  // RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  // RCC_OscInitStruct.HSIState = RCC_HSI_OFF;
  // RCC_OscInitStruct.CSIState = RCC_CSI_OFF;
  // RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  // RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  //
  // RCC_OscInitStruct.PLL.PLLM = 5;
  // RCC_OscInitStruct.PLL.PLLN = 160;
  // RCC_OscInitStruct.PLL.PLLFRACN = 0;
  // RCC_OscInitStruct.PLL.PLLP = 2;
  // RCC_OscInitStruct.PLL.PLLQ = 4;  /* USB needs 48MHz: 400MHz / 2 / 4 = 50MHz (close enough) */
  // RCC_OscInitStruct.PLL.PLLR = 2;
  //
  // RCC_OscInitStruct.PLL.PLLRGE = RCC_PLL1VCIRANGE_2;
  // RCC_OscInitStruct.PLL.PLLVCOSEL = RCC_PLL1VCOWIDE;

  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Configure SDMMC peripheral clock
  */
  PeriphClkInitStruct.PeriphClockSelection = RCC_PERIPHCLK_SDMMC;
  PeriphClkInitStruct.SdmmcClockSelection = RCC_SDMMCCLKSOURCE_PLL;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /* Configure PLL3 for USB 48MHz clock */
  /* IMPORTANT: PLL3 is shared with LCD (LTDC) which is configured later */
  /* PLL3 configuration must be compatible with both USB and LCD: */
  /*   - PLL3Q output: 48MHz for USB (this config) */
  /*   - PLL3R output: ~9.6MHz for LTDC (configured in MX_LTDC_ClockConfig) */
  /* PLL3: 25MHz HSE / 5 = 5MHz VCO input, 5MHz * 96 = 480MHz VCO */
  /* This VCO frequency allows both outputs: 480/10=48MHz, 480/50=9.6MHz */
  RCC_PeriphCLKInitTypeDef UsbClkInitStruct = {0};
  UsbClkInitStruct.PeriphClockSelection = RCC_PERIPHCLK_USB;
  UsbClkInitStruct.UsbClockSelection = RCC_USBCLKSOURCE_PLL3;
  UsbClkInitStruct.PLL3.PLL3M = 5;      /* 25MHz / 5 = 5MHz VCO input */
  UsbClkInitStruct.PLL3.PLL3N = 96;     /* 5MHz * 96 = 480MHz VCO output */
  UsbClkInitStruct.PLL3.PLL3P = 2;
  UsbClkInitStruct.PLL3.PLL3Q = 10;     /* 480MHz / 10 = 48MHz for USB FS */
  UsbClkInitStruct.PLL3.PLL3R = 50;      /* 480MHz / 50 = 9.6MHz for LTDC */
  UsbClkInitStruct.PLL3.PLL3RGE = RCC_PLL3VCIRANGE_2;   /* VCO input 2-4 MHz */
  UsbClkInitStruct.PLL3.PLL3VCOSEL = RCC_PLL3VCOWIDE;   /* Wide VCO range (192-960 MHz) */
  UsbClkInitStruct.PLL3.PLL3FRACN = 0;
  ret = HAL_RCCEx_PeriphCLKConfig(&UsbClkInitStruct);
  if(ret != HAL_OK)
  {
    SAFE_PRINTF("[CM7]:\tERROR: PLL3 configuration failed!\r\n");
    Error_Handler();
  }
  else
  {
    SAFE_PRINTF("[CM7]:\tPLL3 configured successfully for USB 48MHz\r\n");
  }

  /* Configure the USART3/USART6 clock source */
  RCC_PeriphCLKInitTypeDef UsartClkInitStruct = {0};
  UsartClkInitStruct.PeriphClockSelection = RCC_PERIPHCLK_USART3 | RCC_PERIPHCLK_USART6;
  UsartClkInitStruct.Usart234578ClockSelection = RCC_USART234578CLKSOURCE_HSI;  // USART3 uses this
  UsartClkInitStruct.Usart16ClockSelection = RCC_USART16CLKSOURCE_HSI;      // USART6 uses this
  ret = HAL_RCCEx_PeriphCLKConfig(&UsartClkInitStruct);
  if(ret != HAL_OK)
  {
    Error_Handler();
  }
  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2
                              |RCC_CLOCKTYPE_D3PCLK1|RCC_CLOCKTYPE_D1PCLK1;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.SYSCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_APB1_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_APB2_DIV2;
  RCC_ClkInitStruct.APB3CLKDivider = RCC_APB3_DIV2;
  RCC_ClkInitStruct.APB4CLKDivider = RCC_APB4_DIV2;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_4) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
* @brief  This function is executed in case of error occurrence.
* @param  None
* @retval None
*/
void Error_Handler(void)
{

  /* User may add here some code to deal with this error */
  SAFE_PRINTF ("[CM7]:\tError!! \r\n");
  // __disable_irq();
  while(1)
  {
  }
}

/**
* @brief  CPU L1-Cache enable.
* @param  None
* @retval None
*/
void CPU_CACHE_Enable(void)
{
  /* Enable I-Cache */
  SCB_EnableICache();

  /* Enable D-Cache */
  SCB_EnableDCache();
}



/**
  * @brief  Configure the MPU attributes
  * @param  None
  * @retval None
  */
void MPU_Config(void)
{
  MPU_Region_InitTypeDef MPU_InitStruct = {0};

  /* Disables the MPU */
  HAL_MPU_Disable();
  /** Initializes and configures the Region and the memory to be protected
  */
  MPU_InitStruct.Enable = MPU_REGION_ENABLE;
  MPU_InitStruct.Number = MPU_REGION_NUMBER0;
  MPU_InitStruct.BaseAddress = 0x0;
  MPU_InitStruct.Size = MPU_REGION_SIZE_4GB;
  MPU_InitStruct.SubRegionDisable = 0x87;
  MPU_InitStruct.TypeExtField = MPU_TEX_LEVEL0;
  MPU_InitStruct.AccessPermission = MPU_REGION_NO_ACCESS;
  MPU_InitStruct.DisableExec = MPU_INSTRUCTION_ACCESS_DISABLE;
  MPU_InitStruct.IsShareable = MPU_ACCESS_SHAREABLE;
  MPU_InitStruct.IsCacheable = MPU_ACCESS_NOT_CACHEABLE;
  MPU_InitStruct.IsBufferable = MPU_ACCESS_NOT_BUFFERABLE;

  HAL_MPU_ConfigRegion(&MPU_InitStruct);
  /** Initializes and configures the Region and the memory to be protected
  */
  MPU_InitStruct.Number = MPU_REGION_NUMBER1;
  MPU_InitStruct.BaseAddress = 0xD0000000;
  MPU_InitStruct.Size = MPU_REGION_SIZE_32MB;
  MPU_InitStruct.SubRegionDisable = 0x0;
  MPU_InitStruct.AccessPermission = MPU_REGION_FULL_ACCESS;
  MPU_InitStruct.DisableExec = MPU_INSTRUCTION_ACCESS_ENABLE;
  MPU_InitStruct.IsShareable = MPU_ACCESS_NOT_SHAREABLE;
  MPU_InitStruct.IsCacheable = MPU_ACCESS_CACHEABLE;

  HAL_MPU_ConfigRegion(&MPU_InitStruct);
  /** Initializes and configures the Region and the memory to be protected
  */
  MPU_InitStruct.Number = MPU_REGION_NUMBER2;
  MPU_InitStruct.BaseAddress = 0x90000000;
  MPU_InitStruct.Size = MPU_REGION_SIZE_128MB;
  MPU_InitStruct.TypeExtField = MPU_TEX_LEVEL1;
  MPU_InitStruct.AccessPermission = MPU_REGION_FULL_ACCESS;
  MPU_InitStruct.DisableExec = MPU_INSTRUCTION_ACCESS_ENABLE;
  MPU_InitStruct.IsShareable = MPU_ACCESS_SHAREABLE;
  MPU_InitStruct.IsCacheable = MPU_ACCESS_CACHEABLE;
  MPU_InitStruct.IsBufferable = MPU_ACCESS_BUFFERABLE;

  HAL_MPU_ConfigRegion(&MPU_InitStruct);
  /* Enables the MPU */
  HAL_MPU_Enable(MPU_PRIVILEGED_DEFAULT);

}

/**
  * @brief  Initializes USB Device as MSC
  * @param  None
  * @retval None
  */
void USB_Device_Init(void)
{
  SAFE_PRINTF("[USB]:\tInitializing USB peripheral hardware...\r\n");

  /* Enable USB OTG FS peripheral clock */
  __HAL_RCC_USB_OTG_FS_CLK_ENABLE();

  /* Enable USB voltage level detector */
  HAL_PWREx_EnableUSBVoltageDetector();

  /* Configure USB FS GPIOs: DM (PA11) and DP (PA12) */
  __HAL_RCC_GPIOA_CLK_ENABLE();

  GPIO_InitTypeDef GPIO_InitStruct = {0};
  GPIO_InitStruct.Pin = GPIO_PIN_11 | GPIO_PIN_12;
  GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
  GPIO_InitStruct.Alternate = GPIO_AF10_OTG1_FS;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /* Enable VBUS sensing (PA9 on STM32H745I-DISCO) */
  GPIO_InitStruct.Pin = GPIO_PIN_9;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /* Check VBUS status */
  GPIO_PinState vbus = HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_9);
  SAFE_PRINTF("[USB]:\tVBUS detected: %s\r\n", vbus == GPIO_PIN_SET ? "YES" : "NO");

  /* Enable USB interrupt in NVIC */
  HAL_NVIC_SetPriority(OTG_FS_IRQn, 6, 0);
  HAL_NVIC_EnableIRQ(OTG_FS_IRQn);

  /* Initialize TinyUSB - rhport 0 = USB_OTG_FS */
  tusb_rhport_init_t dev_init = {
    .role = TUSB_ROLE_DEVICE,
    .speed = TUSB_SPEED_AUTO
  };

  if (!tusb_init(BOARD_TUD_RHPORT, &dev_init)) {
    SAFE_PRINTF("[USB]:\tERROR: tusb_init failed!\r\n");
    return;
  }
  SAFE_PRINTF("[USB]:\tTinyUSB initialized successfully - ready for enumeration\r\n");
}

//--------------------------------------------------------------------+
// TinyUSB Device Callbacks
//--------------------------------------------------------------------+

// Invoked when device is mounted
void tud_mount_cb(void) {
  BSP_LED_On(LED_GREEN);
}

// Invoked when device is unmounted
void tud_umount_cb(void) {
  BSP_LED_Off(LED_GREEN);
}

// Invoked when usb bus is suspended
void tud_suspend_cb(bool remote_wakeup_en) {
  (void) remote_wakeup_en;
}

// Invoked when usb bus is resumed
void tud_resume_cb(void) {
}