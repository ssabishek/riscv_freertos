.section .init
.global _start

_start:
    # Initialize stack pointer
    la sp, __stack_top

    # Set up trap vector
    la t0, trap_vector
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

.section .text
.align 4
trap_vector:
    # Save some registers to stack
    addi sp, sp, -16
    sw ra, 0(sp)
    sw t0, 4(sp)
    sw t1, 8(sp)
    sw t2, 12(sp)

    # Call C trap handler (to be implemented)
    call trap_handler

    # Restore registers
    lw ra, 0(sp)
    lw t0, 4(sp)
    lw t1, 8(sp)
    lw t2, 12(sp)
    addi sp, sp, 16

    # Return from trap
    mret