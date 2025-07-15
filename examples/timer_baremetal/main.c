/*
   Baremetal main program with timer interrupt.
   SPDX-License-Identifier: Unlicense

   https://five-embeddev.com/

   Tested with sifive-hifive-revb, but should not have any
   dependencies to any particular implementation.
   
*/

#include <stdint.h>
#include <stdbool.h>

// RISC-V CSR definitions and access classes
#include "riscv_csr.h"
#include "riscv_interrupts.h"
#include "timer.h"
#include "log.h"

// Global to hold current timestamp
static volatile uint64_t timestamp = 0;

static volatile bool global_bool_keep_running = true;

int main(void) {
    log_init();
    LOG_INFO("Baremetal timer example started.\n");
    // Global interrupt disable
    csr_clr_bits_mstatus(MSTATUS_MIE_BIT_MASK);
    csr_write_mie(0);

    // Setup timer for 1 second interval
    timestamp = mtimer_get_raw_time();
    mtimer_set_raw_time_cmp(MTIMER_SECONDS_TO_CLOCKS(1));

    // Enable MIE.MTI
    csr_set_bits_mie(MIE_MTI_BIT_MASK);

    // Global interrupt enable 
    csr_set_bits_mstatus(MSTATUS_MIE_BIT_MASK);

    // Busy loop
    do {
        __asm__ volatile ("wfi");  
    } while (global_bool_keep_running);

    // Global interrupt disable
    csr_clr_bits_mstatus(MSTATUS_MIE_BIT_MASK);
    
    return 0;
}

#pragma GCC push_options
// Force the alignment for mtvec.BASE. A 'C' extension program could be aligned to to bytes.
#pragma GCC optimize ("align-functions=4")
void trap_handler(void)  {
    uint_xlen_t this_cause = csr_read_mcause();
    if (this_cause &  MCAUSE_INTERRUPT_BIT_MASK) {
        this_cause &= 0xFF;
        // Known exceptions
        switch (this_cause) {
        case RISCV_INT_POS_MTI :
            LOG_INFO("Timer interrupt at %u\n", timestamp);
            // Timer exception, keep up the one second tick.
            mtimer_set_raw_time_cmp(MTIMER_SECONDS_TO_CLOCKS(1));
            timestamp = mtimer_get_raw_time();
            break;
        }
    }
}
#pragma GCC pop_options


