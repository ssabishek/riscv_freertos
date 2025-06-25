OUTPUT_ARCH(riscv)
ENTRY(_start)

MEMORY
{
  CODE (rx)  : ORIGIN = 0x80000000, LENGTH = 64K  /* For .text (code) */
  DATA (rw)  : ORIGIN = 0x80010000, LENGTH = 64K  /* For .data, .bss, .heap, .stack */
}

SECTIONS
{
  /* Code section (text + rodata) */
  .text : 
  {
    KEEP(*(.init))       /* Startup code */
    *(.text*)            /* All text sections */
    *(.rodata*)          /* Read-only data */
    *(.srodata*)         /* Small read-only data */
  } > CODE

  /* Initialized data section */
  .data : ALIGN(4)
  {
    *(.data*)
    *(.sdata*)           /* Small data */
  } > DATA

  /* Uninitialized data section (zero-initialized) */
  .bss : ALIGN(4)
  {
    __bss_start = .;
    *(.bss*)
    *(.sbss*)
    *(COMMON)
    __bss_end = .;
  } > DATA

  /* Heap section for malloc and FreeRTOS heap_4.c */
  .heap (NOLOAD) : ALIGN(8)
  {
    __heap_start = .;
    . = . + 0x8000;       /* 32 KB heap size */
    __heap_end = .;
  } > DATA

  /* Stack section (placed at top of DATA region) */
  .stack (NOLOAD) : ALIGN(16)
  {
    __stack_top = ORIGIN(DATA) + LENGTH(DATA);
    __stack_bottom = __stack_top - 0x2000; /* 8 KB stack */
  } > DATA
}