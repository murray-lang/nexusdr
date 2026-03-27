/**
  ******************************************************************************
  * @file    ffsystem.c
  * @brief   FatFs system functions (memory allocation for LFN support)
  ******************************************************************************
  * @attention
  *
  * This file provides memory allocation functions required by FatFs when
  * FF_USE_LFN = 3 (heap-based long filename support).
  *
  * These are simple wrappers around standard C malloc/free.
  *
  ******************************************************************************
  */

#include "ff.h"

#if FF_USE_LFN == 3  /* Dynamic memory allocation */

#include <stdlib.h>  /* For malloc, free */

/**
  * @brief  Allocate memory block for FatFs
  * @param  msize: Number of bytes to allocate
  * @retval Pointer to allocated memory, or NULL if failed
  */
void* ff_memalloc(UINT msize)
{
    return malloc(msize);
}

/**
  * @brief  Free memory block
  * @param  mblock: Pointer to memory block to free
  */
void ff_memfree(void* mblock)
{
    free(mblock);
}

#endif  /* FF_USE_LFN == 3 */

/**
  * @brief  Get current time for FatFs timestamps
  * @retval Current time packed into DWORD
  * @note   If you have RTC support, replace this with actual RTC read
  */
DWORD get_fattime(void)
{
    /* Return a fixed timestamp: 2025-01-01 00:00:00 */
    /* Format: bit31:25=Year(0..127 from 1980), 24:21=Month(1..12), 20:16=Day(1..31) */
    /*         bit15:11=Hour(0..23), 10:5=Minute(0..59), 4:0=Second/2(0..29) */

    return ((DWORD)(2025 - 1980) << 25)  /* Year = 2025 */
         | ((DWORD)1 << 21)               /* Month = January */
         | ((DWORD)1 << 16)               /* Day = 1 */
         | ((DWORD)0 << 11)               /* Hour = 0 */
         | ((DWORD)0 << 5)                /* Minute = 0 */
         | ((DWORD)0 << 0);               /* Second = 0 */
}
