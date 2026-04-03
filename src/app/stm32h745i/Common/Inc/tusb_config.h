/*
 * TinyUSB Configuration for STM32H745I-DISCO
 * USB MSC Device for eMMC access via FatFs
 */

#ifndef TUSB_CONFIG_H_
#define TUSB_CONFIG_H_

#ifdef __cplusplus
 extern "C" {
#endif

//--------------------------------------------------------------------
// Board Specific Configuration
//--------------------------------------------------------------------

// RHPort 0 = USB_OTG_FS on STM32H745I-DISCO
#ifndef BOARD_TUD_RHPORT
#define BOARD_TUD_RHPORT      0
#endif

// RHPort max operational speed
#ifndef BOARD_TUD_MAX_SPEED
#define BOARD_TUD_MAX_SPEED   OPT_MODE_FULL_SPEED
#endif

//--------------------------------------------------------------------
// Common Configuration
//--------------------------------------------------------------------

// MCU is STM32H7 (defined via compiler flag CFG_TUSB_MCU=OPT_MCU_STM32H7)
#ifndef CFG_TUSB_MCU
#error CFG_TUSB_MCU must be defined
#endif

// No RTOS for now (bare metal operation)
#ifndef CFG_TUSB_OS
#define CFG_TUSB_OS           OPT_OS_NONE
#endif

// Debug level (0 = none, 1 = errors, 2 = warnings, 3 = all)
#ifndef CFG_TUSB_DEBUG
#define CFG_TUSB_DEBUG        0
#endif

// Enable Device stack
#define CFG_TUD_ENABLED       1

// Default is max speed that hardware controller could support with on-chip PHY
#define CFG_TUD_MAX_SPEED     BOARD_TUD_MAX_SPEED

/* USB DMA on STM32H7 can only access D1 domain SRAM (DTCM not accessible)
 * We need to ensure buffers are placed in AXI SRAM or SRAM1/2/3
 */
#ifndef CFG_TUSB_MEM_SECTION
#define CFG_TUSB_MEM_SECTION  __attribute__((section(".RAM_D1")))
#endif

#ifndef CFG_TUSB_MEM_ALIGN
#define CFG_TUSB_MEM_ALIGN    __attribute__ ((aligned(4)))
#endif

//--------------------------------------------------------------------
// DEVICE CONFIGURATION
//--------------------------------------------------------------------

#ifndef CFG_TUD_ENDPOINT0_SIZE
#define CFG_TUD_ENDPOINT0_SIZE   64
#endif

//------------- CLASS -------------//
// MSC only for now (no CDC serial, no HID)
#define CFG_TUD_CDC              0
#define CFG_TUD_MSC              1
#define CFG_TUD_HID              0
#define CFG_TUD_MIDI             0
#define CFG_TUD_VENDOR           0

// MSC Buffer size - 512 bytes for standard sector size
#define CFG_TUD_MSC_EP_BUFSIZE   512

#ifdef __cplusplus
 }
#endif

#endif /* TUSB_CONFIG_H_ */
