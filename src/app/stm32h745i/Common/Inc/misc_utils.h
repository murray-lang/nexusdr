#ifndef __MISC_UTILS_H__
#define __MISC_UTILS_H__

#include <stdio.h>
#include <stdint.h>

#define SAFE_PRINTF(fmt, ...) \
  do { \
    LOCK_HSEM(UART_HSEM_ID); \
    printf(fmt, ##__VA_ARGS__); \
    UNLOCK_HSEM(UART_HSEM_ID); \
  } while(0)

extern uint8_t Buffercmp(uint8_t* pBuffer1, uint8_t* pBuffer2, uint32_t BufferLength);

#endif // __MISC_UTILS_H__