#include "config.h"
#include "misc_utils.h"

UART_HandleTypeDef     UartHandle;

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

PUTCHAR_PROTOTYPE
{
  HAL_UART_Transmit(&UartHandle, (uint8_t *)&ch, 1, 0xFFFFF);
  return ch;
}

void Error_Handler(void)
{
  SAFE_PRINTF("[CM4]:\tError!! \n");
  while(1)
  {
  }
}