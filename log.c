/*
 * -----------------------------------------------------
 *      __  __  _____  _____    _____
 *     |  \/  ||_   _||  __ \  / ____|
 *     | \  / |  | |  | |__) || (___
 *     | |\/| |  | |  |  ___/  \___ \
 *     | |  | | _| |_ | |      ____) |
 *     |_|  |_||_____||_|     |_____/
 * -----------------------------------------------------
 * Copyright (c) 2025, MIPS All rights reserved.
 * -----------------------------------------------------
 */

#include "log.h"

static log_output_handler_t custom_output_handler = NULL;

__attribute__((weak)) void log_output_default(const char* message, size_t length)
{
    // Use UART to output the message
    uart_puts(message);
}

static void log_output_internal(const char* message, size_t length)
{
    if(custom_output_handler != NULL) {
        /* Use registered custom handler */
        custom_output_handler(message, length);
    } else {
        /* Use default output (can be overridden via weak function) */
        log_output_default(message, length);
    }
}

void log_register_output_handler(log_output_handler_t handler)
{
    custom_output_handler = handler;
}

void log_init(void)
{
    uart_init();
    uart_puts("Logging system initialized.\n");
}

// Helper function to append a string to buffer
static int append_str(char* buffer, size_t* offset, size_t max, const char* str, int precision)
{
    size_t i = 0;
    while (str[i] && (precision < 0 || i < precision) && *offset < max - 1) {
        buffer[(*offset)++] = str[i++];
    }
    return i;
}

// Helper function to append a number (decimal or hex) to buffer
static int append_num(char* buffer, size_t* offset, size_t max, unsigned long num, int base, int is_signed, int width, int zero_pad, int upper)
{
    char temp[32];
    int i = 0;
    unsigned long n = num;

    if (is_signed && (long)num < 0) {
        buffer[(*offset)++] = '-';
        n = -(long)num;
    }

    do {
        int digit = n % base;
        temp[i++] = (digit < 10) ? '0' + digit : (upper ? 'A' : 'a') + digit - 10;
        n /= base;
    } while (n && i < sizeof(temp));

    // Pad with zeros or spaces
    while (i < width && *offset < max - 1) {
        buffer[(*offset)++] = zero_pad ? '0' : ' ';
        width--;
    }

    // Append digits in reverse order
    while (i > 0 && *offset < max - 1) {
        buffer[(*offset)++] = temp[--i];
    }
    return i;
}

void log_print(const char* fmt, ...)
{
    va_list args;
    char buffer[32];
    size_t offset = 0;

    va_start(args, fmt);

    // Process the format string
    while (*fmt && offset < sizeof(buffer) - 1) {
        if (*fmt != '%') {
            buffer[offset++] = *fmt++;
            continue;
        }

        fmt++; // Skip '%'
        int zero_pad = 0;
        int width = 0;

        // Check for zero-padding
        if (*fmt == '0') {
            zero_pad = 1;
            fmt++;
        }

        // Parse width
        while (*fmt >= '0' && *fmt <= '9') {
            width = width * 10 + (*fmt - '0');
            fmt++;
        }

        // Handle precision (e.g., %.2s, %.2d)
        int precision = -1;
        if (*fmt == '.') {
            fmt++;
            precision = 0;
            while (*fmt >= '0' && *fmt <= '9') {
                precision = precision * 10 + (*fmt - '0');
                fmt++;
            }
        }

        // Handle format specifier
        switch (*fmt) {
            case 's': {
                const char* str = va_arg(args, const char*);
                if (!str) str = "(null)";
                append_str(buffer, &offset, sizeof(buffer), str, precision);
                fmt++;
                break;
            }
            case 'd':
            case 'i': {
                int num = va_arg(args, int);
                append_num(buffer, &offset, sizeof(buffer), num, 10, 1, width, zero_pad, 0);
                fmt++;
                break;
            }
            case 'u': {
                unsigned int num = va_arg(args, unsigned int);
                append_num(buffer, &offset, sizeof(buffer), num, 10, 0, width, zero_pad, 0);
                fmt++;
                break;
            }
            case 'x':
            case 'X': {
                unsigned int num = va_arg(args, unsigned int);
                append_num(buffer, &offset, sizeof(buffer), num, 16, 0, width, zero_pad, (*fmt == 'X'));
                fmt++;
                break;
            }
            case 'p': {
                unsigned long num = (unsigned long)va_arg(args, void*);
                append_str(buffer, &offset, sizeof(buffer), "0x", -1);
                append_num(buffer, &offset, sizeof(buffer), num, 16, 0, width, 1, 0);
                fmt++;
                break;
            }
            case 'c': {
                int c = va_arg(args, int);
                if (offset < sizeof(buffer) - 1) {
                    buffer[offset++] = (char)c;
                }
                fmt++;
                break;
            }
            case '%':
                if (offset < sizeof(buffer) - 1) {
                    buffer[offset++] = '%';
                }
                fmt++;
                break;
            default:
                // Unknown specifier, output as-is
                if (offset < sizeof(buffer) - 1) {
                    buffer[offset++] = '%';
                    buffer[offset++] = *fmt++;
                }
                break;
        }
    }

    buffer[offset] = '\0';
    va_end(args);

    // Output the buffer
    if (offset > 0) {
        log_output_internal(buffer, offset);
    }
}