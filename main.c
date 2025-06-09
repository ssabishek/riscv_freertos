/*
 * Test case for logging system
 * Tests all format specifiers, log levels, verbose modes, and edge cases
 * Copyright (c) 2025, MIPS All rights reserved.
 */

#include <log.h>

// Define UART0_BASE for QEMU virt machine
#ifndef UART0_BASE
#define UART0_BASE 0x10000000
#endif

// Ensure all log levels are included
#ifndef LOG_MAX_LEVEL
#define LOG_MAX_LEVEL LOG_LEVEL_DEBUG
#endif

// UART driver function declarations
void uart_putc(char c);
void uart_puts(const char *str);

// Comment out to test with LOG_VERBOSE_MODE 0 (simple mode)
//#define LOG_VERBOSE_MODE 1

// Custom output handler for testing
static void custom_handler(const char* message, size_t length)
{
    // Prefix with "CUSTOM: " and use uart_puts
    uart_puts("CUSTOM: ");
    uart_puts(message);
}

int main(void)
{
    log_init(); // Initialize logging system

    // Test 1: Basic log levels with simple format
    LOG_ERROR("Error message: %d\n", -42);
    LOG_WARN("Warning message: %u\n", 1234);
    LOG_INFO("Info message: %s\n", "Hello, RISC-V FreeRTOS!");
    LOG_DEBUG("Debug message: %x\n", 0x1A);

    // Test 2: All format specifiers
    LOG_INFO("All formats: %d, %i, %u, %x, %X, %s, %c, %p, %%\n",
             -42, -42, 1234, 0x1A, 0x1A, "test", 'Z', (void*)0x80000000);

    // Test 3: Format flags (width, zero-padding, precision)
    LOG_INFO("Width: %4d, Zero-pad: %04d, Precision: %.2s, %.2d\n",
             42, 42, "abcd", 3);

    // Test 4: Edge cases
    LOG_INFO("Null string: %s\n", (char*)0); // Should print "(null)"
    LOG_INFO("Large number: %d\n", 2147483647); // Max int32
    LOG_INFO("Zero: %d\n", 0);
    LOG_INFO("Invalid specifier: %q\n", 123); // Should print "%q"

    // Test 5: Long string (approaching buffer limit)
    char long_str[200];
    for (int i = 0; i < 199; i++) long_str[i] = 'A';
    long_str[199] = '\0';
    LOG_INFO("Long string: %.150s\n", long_str); // Limit to 150 chars

    // Test 6: Custom output handler
    log_register_output_handler(custom_handler);
    LOG_INFO("Custom handler test: %d\n", 100);
    log_register_output_handler(NULL); // Reset to default

    // Test 7: Verbose mode (if LOG_VERBOSE_MODE is 1)
    LOG_INFO("Verbose test: %d\n", 42); // Should include file, line, func if verbose

    return 0;
}