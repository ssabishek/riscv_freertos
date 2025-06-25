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

/**
 * \file log.h
 * \brief Logging system header file.
 */

#ifndef LOG_H
#define LOG_H

#include <stdarg.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include "uart.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/**
 * \brief Log levels with priority ordering.
 */
#define LOG_LEVEL_OFF   0 /** Logging is disabled */
#define LOG_LEVEL_ERROR 1 /** Critical errors that require immediate attention */
#define LOG_LEVEL_WARN  2 /** Warnings about potential issues or non-critical errors */
#define LOG_LEVEL_INFO  3 /** General informational messages about application state */
#define LOG_LEVEL_DEBUG 4 /** Detailed debug messages for troubleshooting */

/**
 * \brief Function pointer type for custom output handlers.
 */
typedef void (*log_output_handler_t)(const char* message, size_t length);

/* Defines the maximum log level for compile-time logging. Only log messages
    at this level or higher severity will be compiled into the binary.
    Messages below the LOG_MAX_LEVEL will be completely excluded from the binary. */
#ifndef LOG_MAX_LEVEL
#define LOG_MAX_LEVEL LOG_LEVEL_INFO
#endif

/* Define verbose mode - can be controlled at compile time */
#ifndef LOG_VERBOSE_MODE
#define LOG_VERBOSE_MODE 0 /* 0 = simple mode, 1 = verbose mode */
#endif

#define LOG(...) log_print(__VA_ARGS__)

#if LOG_VERBOSE_MODE
#define LOG_FORMAT(level, fmt, ...) \
    log_print("\n[%s] %s:%d:%s() - " fmt, level, __FILE_NAME__, __LINE__, __func__, ##__VA_ARGS__)
#else
#define LOG_FORMAT(level, fmt, ...) log_print("\n[%s] " fmt, level, ##__VA_ARGS__)
#endif

#if LOG_MAX_LEVEL >= LOG_LEVEL_ERROR
#define LOG_ERROR(_fmt, ...) LOG_FORMAT("ERR", _fmt, ##__VA_ARGS__)
#else
#define LOG_ERROR(...) ((void) 0)
#endif

#if LOG_MAX_LEVEL >= LOG_LEVEL_WARN
#define LOG_WARN(_fmt, ...) LOG_FORMAT("WAR", _fmt, ##__VA_ARGS__)
#else
#define LOG_WARN(...) ((void) 0)
#endif

#if LOG_MAX_LEVEL >= LOG_LEVEL_INFO
#define LOG_INFO(_fmt, ...) LOG_FORMAT("INF", _fmt, ##__VA_ARGS__)
#else
#define LOG_INFO(...) ((void) 0)
#endif

#if LOG_MAX_LEVEL >= LOG_LEVEL_DEBUG
#define LOG_DEBUG(_fmt, ...) LOG_FORMAT("DBG", _fmt, ##__VA_ARGS__)
#else
#define LOG_DEBUG(...) ((void) 0)
#endif

/**
 * \brief Initialize the logging system.
 *
 * This function initializes the logging system. It should be called
 * before any logging functions are used.
 */
void log_init(void);

/**
 * \brief Register a custom output handler.
 *
 * \param handler Function pointer to custom output handler
 */
void log_register_output_handler(log_output_handler_t handler);

/**
 * \brief Print a formatted string.
 *
 * \param fmt Format string.
 * \param ... Format arguments.
 */
void log_print(const char* fmt, ...);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* LOG_H */