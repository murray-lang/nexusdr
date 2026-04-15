#ifndef __CONFIG_H__
#define __CONFIG_H__

/* Includes ------------------------------------------------------------------*/
#include "stm32h7xx_hal.h"
#include "stm32h745i_discovery.h"
#include "ff_gen_drv.h"
#include "mmc_diskio.h"

/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
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


extern UART_HandleTypeDef UartHandle;

/* Exported macro ------------------------------------------------------------*/
#define HSEM_ID_0 (0U) /* HW semaphore 0*/
#define UART_HSEM_ID (2U)

#define LOCK_HSEM(__sem__) while(HAL_HSEM_FastTake(__sem__) != HAL_OK) {}
#define UNLOCK_HSEM(__sem__)  HAL_HSEM_Release(__sem__, 0)

#ifdef __GNUC__
/* With GCC/RAISONANCE, small printf (option LD Linker->Libraries->Small printf
set to 'Yes') calls __io_putchar() */
#define PUTCHAR_PROTOTYPE int __io_putchar(int ch)
#else
#define PUTCHAR_PROTOTYPE int fputc(int ch, FILE *f)
#endif /* __GNUC__ */

#define SDIO1_D2_Pin GPIO_PIN_10
#define SDIO1_D2_GPIO_Port GPIOC
#define SDIO1_D3_Pin GPIO_PIN_11
#define SDIO1_D3_GPIO_Port GPIOC
#define OSC32_OUT_Pin GPIO_PIN_15
#define OSC32_OUT_GPIO_Port GPIOC
#define OSC32_IN_Pin GPIO_PIN_14
#define OSC32_IN_GPIO_Port GPIOC
#define SDIO1_CK_Pin GPIO_PIN_12
#define SDIO1_CK_GPIO_Port GPIOC
#define SDIO1_D5_Pin GPIO_PIN_9
#define SDIO1_D5_GPIO_Port GPIOB
#define SDIO1_D4_Pin GPIO_PIN_8
#define SDIO1_D4_GPIO_Port GPIOB
#define SDIO1_CMD_Pin GPIO_PIN_2
#define SDIO1_CMD_GPIO_Port GPIOD
#define VBUS_FS2_Pin GPIO_PIN_9
#define VBUS_FS2_GPIO_Port GPIOA
#define SDIO1_D0_Pin GPIO_PIN_8
#define SDIO1_D0_GPIO_Port GPIOC
#define SDIO1_D1_Pin GPIO_PIN_9
#define SDIO1_D1_GPIO_Port GPIOC
#define USB_OTG_FS2_P_Pin GPIO_PIN_12
#define USB_OTG_FS2_P_GPIO_Port GPIOA
#define USB_OTG_FS2_N_Pin GPIO_PIN_11
#define USB_OTG_FS2_N_GPIO_Port GPIOA
#define SDIO1_D7_Pin GPIO_PIN_7
#define SDIO1_D7_GPIO_Port GPIOC
#define SDIO1_D6_Pin GPIO_PIN_6
#define SDIO1_D6_GPIO_Port GPIOC
#define OSC_OUT_Pin GPIO_PIN_1
#define OSC_OUT_GPIO_Port GPIOH
#define OSC_IN_Pin GPIO_PIN_0
#define OSC_IN_GPIO_Port GPIOH
#define VCP_TX_Pin GPIO_PIN_10
#define VCP_TX_GPIO_Port GPIOB
#define VCP_RX_Pin GPIO_PIN_11
#define VCP_RX_GPIO_Port GPIOB


/* Exported functions ------------------------------------------------------- */
extern PUTCHAR_PROTOTYPE;
extern void UART_Config(void);
extern void Error_Handler(void);

extern void MX_GPIO_Init(void);
extern void MX_DMA_Init(void);
extern void MX_FMC_Init(void);
extern void MX_SAI2_Init(void);
extern void MX_USART3_UART_Init(void);
extern void MX_DAC1_Init(void);
extern void MX_ADC1_Init(void);
extern void MX_ADC2_Init(void);
extern void MX_TIM6_Init(void);
extern void MX_I2C4_Init(void);
extern void MX_USART6_UART_Init(void);

// extern void USB_Device_Init(void);

#endif // __CONFIG_H__