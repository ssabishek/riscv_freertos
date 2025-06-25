.extern freertos_risc_v_trap_handler 

.section .init
.global _start

_start:
    # Initialize stack pointer
    la sp, __stack_top

    # Set up trap vector
    la t0, freertos_risc_v_trap_handler
    csrw mtvec, t0

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


