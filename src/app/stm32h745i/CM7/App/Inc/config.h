#ifndef __CONFIG_H__
#define __CONFIG_H__

/* Includes ------------------------------------------------------------------*/
#include <stdbool.h>

#include "stm32h745i_discovery.h"
#include "ff_gen_drv.h"
#include "mmc_diskio.h"


/* Exported types ------------------------------------------------------------*/
/* Exported variables --------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
/* Definition for USARTx clock resources */
#define USARTx                           USART3
#define USARTx_CLK_ENABLE()              __HAL_RCC_USART3_CLK_ENABLE()
#define USARTx_RX_GPIO_CLK_ENABLE()      __HAL_RCC_GPIOB_CLK_ENABLE()
#define USARTx_TX_GPIO_CLK_ENABLE()      __HAL_RCC_GPIOB_CLK_ENABLE()

#define USARTx_FORCE_RESET()             __HAL_RCC_USART3_FORCE_RESET()
#define USARTx_RELEASE_RESET()           __HAL_RCC_USART3_RELEASE_RESET()

/* Definition for USARTx Pins */
#define USARTx_TX_PIN                    GPIO_PIN_10
#define USARTx_TX_GPIO_PORT              GPIOB
#define USARTx_TX_AF                     GPIO_AF7_USART3
#define USARTx_RX_PIN                    GPIO_PIN_11
#define USARTx_RX_GPIO_PORT              GPIOB
#define USARTx_RX_AF                     GPIO_AF7_USART3

/* Definition for USARTx's NVIC IRQ and IRQ Handlers */
#define USARTx_IRQn                      USART3_IRQn
#define USARTx_IRQHandler                USART3_IRQHandler

/* Definition for USARTx peripheral clock configuration */
#define USARTx_PERIPHCLK                 RCC_PERIPHCLK_USART3
#define USARTx_CLKSOURCE                 RCC_USART234578CLKSOURCE_HSI

// #define USARTx                           USART6
// #define USARTx_CLK_ENABLE()              __HAL_RCC_USART6_CLK_ENABLE()
// #define USARTx_RX_GPIO_CLK_ENABLE()      __HAL_RCC_GPIOC_CLK_ENABLE()
// #define USARTx_TX_GPIO_CLK_ENABLE()      __HAL_RCC_GPIOC_CLK_ENABLE()
//
// #define USARTx_FORCE_RESET()             __HAL_RCC_USART6_FORCE_RESET()
// #define USARTx_RELEASE_RESET()           __HAL_RCC_USART6_RELEASE_RESET()
//
// /* Definition for USARTx Pins */
// #define USARTx_TX_PIN                    GPIO_PIN_6
// #define USARTx_TX_GPIO_PORT              GPIOC
// #define USARTx_TX_AF                     GPIO_AF7_USART6
// #define USARTx_RX_PIN                    GPIO_PIN_7
// #define USARTx_RX_GPIO_PORT              GPIOC
// #define USARTx_RX_AF                     GPIO_AF7_USART6
//
// /* Definition for USARTx's NVIC IRQ and IRQ Handlers */
// #define USARTx_IRQn                      USART6_IRQn
// #define USARTx_IRQHandler                USART6_IRQHandler

/* Definition for USARTx peripheral clock configuration */
// #define USARTx_PERIPHCLK                 RCC_PERIPHCLK_USART6
// #define USARTx_CLKSOURCE                 RCC_USART234578CLKSOURCE_HSI

extern UART_HandleTypeDef     UartHandle;

/* Exported macro ------------------------------------------------------------*/

#define HSEM_ID_0 (0U) /* HW semaphore 0*/
#define UART_HSEM_ID (2U) /* HW semaphore 0*/

#define LOCK_HSEM(__sem__)   while(HAL_HSEM_FastTake(__sem__) != HAL_OK) {}
#define UNLOCK_HSEM(__sem__)  HAL_HSEM_Release(__sem__, 0)

#ifdef __GNUC__
/* With GCC/RAISONANCE, small printf (option LD Linker->Libraries->Small printf
set to 'Yes') calls __io_putchar() */
#define PUTCHAR_PROTOTYPE int __io_putchar(int ch)
#else
#define PUTCHAR_PROTOTYPE int fputc(int ch, FILE *f)
#endif /* __GNUC__ */

/* Exported functions ------------------------------------------------------- */
extern void MPU_Config(void);
extern void CPU_CACHE_Enable(void);
extern void SystemClock_Config(void);
extern void Error_Handler(void);
extern void UART_Config(void);
extern void USB_Device_Init(void);

//--------------------------------------------------------------------+
// TinyUSB Device Callbacks
//--------------------------------------------------------------------+
// Invoked when device is mounted
extern void tud_mount_cb(void);

// Invoked when device is unmounted
extern void tud_umount_cb(void);

// Invoked when usb bus is suspended
extern void tud_suspend_cb(bool remote_wakeup_en);

// Invoked when usb bus is resumed
extern void tud_resume_cb(void);
#endif // __CONFIG_H__