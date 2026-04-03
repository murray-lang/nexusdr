/*
******************************************************************************
**  Linker script for STM32H745 with QSPI support
**  Strategy: ALL code in FLASH by default
**            Explicitly list what goes to QSPI
******************************************************************************
*/

/* Entry Point */
ENTRY(Reset_Handler)

/* Highest address of the user mode stack */
_estack = ORIGIN(RAM_D1) + LENGTH(RAM_D1);

_Min_Heap_Size = 0x1000;
_Min_Stack_Size = 0x2000;

/* Memories definition */
MEMORY
{
  DTCMRAM (xrw)  : ORIGIN = 0x20000000, LENGTH = 128K /* DMA buffers, 32-byte aligned */
  RAM_D1 (xrw)   : ORIGIN = 0x24000000, LENGTH =  512K
  FLASH  (rx)    : ORIGIN = 0x08000000, LENGTH = 1024K
  
  RAM_D2 (xrw)   : ORIGIN = 0x30000000, LENGTH = 288K
  SHARED_RAM (xrw)   : ORIGIN = 0x38000000, LENGTH = 64K /* SRAM4 - shared between cores */
  ITCMRAM (xrw)  : ORIGIN = 0x00000000, LENGTH = 64K
  QSPI (rx)      : ORIGIN = 0x90000000, LENGTH = 64M
}

/* Define output sections */
SECTIONS
{
  /* The startup code goes first into FLASH */
  .isr_vector :
  {
    . = ALIGN(4);
    KEEP(*(.isr_vector)) /* Startup code */
    . = ALIGN(4);
  } >FLASH

  /* The program code and other data goes into FLASH */
  .text :
  {
    . = ALIGN(4);
    *(.text)           /* .text sections (code) */
    *(.text*)          /* .text* sections (code) */
    *(.glue_7)         /* glue arm to thumb code */
    *(.glue_7t)        /* glue thumb to arm code */
    *(.eh_frame)

    KEEP (*(.init))
    KEEP (*(.fini))

    . = ALIGN(4);
    _etext = .;        /* define a global symbols at end of code */
  } >FLASH

  /* LVGL code in QSPI - explicitly listed */
  .text_qspi :
  {
    . = ALIGN(4);
    *lv*.o(.text .text.*)
    . = ALIGN(4);
  } >QSPI

  /* Constant data goes into FLASH */
  .rodata :
  {
    . = ALIGN(4);
    *(.rodata)         /* .rodata sections (constants, strings, etc.) */
    *(.rodata*)        /* .rodata* sections (constants, strings, etc.) */
    . = ALIGN(4);
  } >FLASH

  .ARM.extab (READONLY) : /* The READONLY keyword is only supported in GCC11 and later, remove it if using GCC10 or earlier. */
  {
    *(.ARM.extab* .gnu.linkonce.armextab.*)
  } >FLASH
  .ARM (READONLY) : /* The READONLY keyword is only supported in GCC11 and later, remove it if using GCC10 or earlier. */
  {
    __exidx_start = .;
    *(.ARM.exidx*)
    __exidx_end = .;
  } >FLASH

  .preinit_array (READONLY) : /* The READONLY keyword is only supported in GCC11 and later, remove it if using GCC10 or earlier. */
  {
    PROVIDE_HIDDEN (__preinit_array_start = .);
    KEEP (*(.preinit_array*))
    PROVIDE_HIDDEN (__preinit_array_end = .);
  } >FLASH

  .init_array (READONLY) : /* The READONLY keyword is only supported in GCC11 and later, remove it if using GCC10 or earlier. */
  {
    PROVIDE_HIDDEN (__init_array_start = .);
    KEEP (*(SORT(.init_array.*)))
    KEEP (*(.init_array*))
    PROVIDE_HIDDEN (__init_array_end = .);
  } >FLASH

  .fini_array (READONLY) : /* The READONLY keyword is only supported in GCC11 and later, remove it if using GCC10 or earlier. */
  {
    PROVIDE_HIDDEN (__fini_array_start = .);
    KEEP (*(SORT(.fini_array.*)))
    KEEP (*(.fini_array*))
    PROVIDE_HIDDEN (__fini_array_end = .);
  } >FLASH

  /* used by the startup to initialize data */
  _sidata = LOADADDR(.data);

  /* DMA buffers section - back in DMA_RAM where SDMMC DMA can access */
  .dma_buffer (NOLOAD) :
  {
    . = ALIGN(32);
    _sdma = .;
    *(.dma_buffer)
    *(.dma_buffer*)
    . = ALIGN(32);
    _edma = .;
  } >DTCMRAM

  /* Initialized data sections goes into RAM, load LMA copy after code */
  _sidata2 = LOADADDR(.data);
  .data :
  {
    . = ALIGN(32);
    _sdata = .;        /* create a global symbol at data start */
    *(.data)           /* .data sections */
    *(.data*)          /* .data* sections */
    *(.RamFunc)
    *(.RamFunc*)

    . = ALIGN(32);
    _edata = .;        /* define a global symbol at data end */
  } >RAM_D1 AT> FLASH

  /* Uninitialized data section */
  . = ALIGN(32);
  .bss (NOLOAD) :
  {
    /* This is used by the startup in order to initialize the .bss section */
    _sbss = .;         /* define a global symbol at bss start */
    __bss_start__ = _sbss;
    *(.bss)
    *(.bss*)
    *(COMMON)

    . = ALIGN(32);
    _ebss = .;         /* define a global symbol at bss end */
    __bss_end__ = _ebss;
  } >RAM_D1

  /* Shared memory section for dual-core access */
.shared_data (NOLOAD) :
{
  . = ALIGN(32);
  _sshared = .;
  *(.shared_data)
  *(.shared_data*)
  . = ALIGN(32);
  _eshared = .;
} >SHARED_RAM

  /* User heap and stack section, used to check that there is enough RAM left */
  ._user_heap_stack (NOLOAD) :
  {
    . = ALIGN(8);
    PROVIDE ( end = . );
    PROVIDE ( _end = . );
    . = . + _Min_Heap_Size;
    . = . + _Min_Stack_Size;
    . = ALIGN(8);
  } >RAM_D1

  /* USB buffers section - in DTCMRAM for non-cacheable access */
  /* TinyUSB uses .RAM_D1 section but we place it in DTCMRAM to avoid cache issues */
  .RAM_D1 (NOLOAD) :
  {
    . = ALIGN(32);
    *(.RAM_D1)
    *(.RAM_D1*)
    . = ALIGN(32);
  } >DTCMRAM

  
  /* Remove information from the standard libraries */
  /DISCARD/ :
  {
    libc.a ( * )
    libm.a ( * )
    libgcc.a ( * )
  }

  .ARM.attributes 0 : { *(.ARM.attributes) }
}
