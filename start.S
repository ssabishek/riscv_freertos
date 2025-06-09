.section .init
.global _start

_start:
    # Initialize stack pointer
    la sp, __stack_top

    # Clear BSS section
    la t0, __bss_start
    la t1, __bss_end
clear_bss:
    beq t0, t1, bss_done
    sw zero, (t0)
    addi t0, t0, 4
    j clear_bss
bss_done:

    # Call main
    call main

    # Infinite loop after main returns
halt:
    j halt
