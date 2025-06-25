/*
 * FreeRTOS Software Timer Example
 * Demonstrates auto-reload and one-shot timer functionality with formatted output
 * Features:
 *   - Auto-reload timer prints a counter and stops after 5 iterations
 *   - One-shot timer triggers a single event after a delay
 *   - Detailed console output with timestamps
 * Target: Generic FreeRTOS-compatible platform
 */

#include <stdio.h>
#include "FreeRTOS.h"
#include "task.h"
#include "timers.h"
#include "log.h"

// Timer periods (in milliseconds)
#define AUTO_RELOAD_PERIOD_MS  1000
#define ONE_SHOT_PERIOD_MS     5000
#define AUTO_RELOAD_MAX_COUNT  5

// Timer handles
static TimerHandle_t xAutoReloadTimer = NULL;
static TimerHandle_t xOneShotTimer = NULL;

// Counter for auto-reload timer iterations
static uint32_t ulAutoReloadCount = 0;

/* Auto-reload timer callback */
void vAutoReloadTimerCallback(TimerHandle_t xTimer)
{
    (void)xTimer;
    ulAutoReloadCount++;

    // Print formatted message with timestamp (tick count)
    LOG_INFO("[T=%d] Auto-reload timer: Iteration %d of %d\n",
           xTaskGetTickCount() * portTICK_PERIOD_MS,
           ulAutoReloadCount,
           AUTO_RELOAD_MAX_COUNT);

    // Stop the timer after reaching the maximum count
    if (ulAutoReloadCount >= AUTO_RELOAD_MAX_COUNT)
    {
        if (xTimerStop(xAutoReloadTimer, 0) == pdPASS)
        {
            LOG_INFO("[T=%d] Auto-reload timer: Stopped after %d iterations\n",
                   xTaskGetTickCount() * portTICK_PERIOD_MS,
                   AUTO_RELOAD_MAX_COUNT);
        }
        else
        {
            LOG_INFO("[T=%d] Auto-reload timer: Error stopping timer\n",
                   xTaskGetTickCount() * portTICK_PERIOD_MS);
        }
    }
}

/* One-shot timer callback */
void vOneShotTimerCallback(TimerHandle_t xTimer)
{
    (void)xTimer;
    // Print formatted message with timestamp
    LOG_INFO("[T=%d] One-shot timer: Triggered single event\n",
           xTaskGetTickCount() * portTICK_PERIOD_MS);
}

/* Main task to initialize and start timers */
void vMainTask(void *pvParameters)
{
    (void)pvParameters;

    LOG_INFO("\n=== FreeRTOS Software Timer Example ===\n");
    LOG_INFO("Auto-reload timer runs every %ld ms, stops after %d iterations\n",
           AUTO_RELOAD_PERIOD_MS, AUTO_RELOAD_MAX_COUNT);
    LOG_INFO("One-shot timer triggers once after %ld ms\n\n",
           ONE_SHOT_PERIOD_MS);

    // Create auto-reload timer
    xAutoReloadTimer = xTimerCreate(
        "AutoReloadTimer",                  // Timer name for debugging
        pdMS_TO_TICKS(AUTO_RELOAD_PERIOD_MS), // Timer period in ticks
        pdTRUE,                            // Enable auto-reload
        NULL,                              // Timer ID (not used)
        vAutoReloadTimerCallback           // Callback function
    );

    // Create one-shot timer
    xOneShotTimer = xTimerCreate(
        "OneShotTimer",
        pdMS_TO_TICKS(ONE_SHOT_PERIOD_MS),
        pdFALSE,                           // Disable auto-reload (one-shot)
        NULL,
        vOneShotTimerCallback
    );

    // Validate timer creation
    if (xAutoReloadTimer == NULL || xOneShotTimer == NULL)
    {
        LOG_INFO("Error: Failed to create timers\n");
        for(;;); // Halt on error
    }

    // Start auto-reload timer
    if (xTimerStart(xAutoReloadTimer, 0) != pdPASS)
    {
        LOG_INFO("Error: Failed to start auto-reload timer\n");
        for(;;); // Halt on error
    }

    // Start one-shot timer
    if (xTimerStart(xOneShotTimer, 0) != pdPASS)
    {
        LOG_INFO("Error: Failed to start one-shot timer\n");
        for(;;); // Halt on error
    }

    LOG_INFO("[T=%d] Timers started successfully\n",
           xTaskGetTickCount() * portTICK_PERIOD_MS);

    // Task is no longer needed, delete itself
    vTaskDelete(NULL);
}

/* Application entry point */
void main(void)
{
    log_init();
    // Create the main task
    BaseType_t xResult = xTaskCreate(
        vMainTask,          // Task function
        "MainTask",         // Task name
        configMINIMAL_STACK_SIZE, // Stack size
        NULL,               // Task parameters
        ((tskIDLE_PRIORITY + 1) | portPRIVILEGE_BIT), // Priority
        NULL                // Task handle
    );

    if (xResult != pdPASS)
    {
        LOG_INFO("Error: Failed to create main task\n");
        for(;;); // Halt on error
    }

    // Start the FreeRTOS scheduler
    vTaskStartScheduler();

    // Should never reach here
    for(;;);
}


void vAssertCalled( const char * pcFileName,
                    uint32_t ulLine )
{
    volatile uint32_t ulSetToNonZeroInDebuggerToContinue = 0;

    /* Called if an assertion passed to configASSERT() fails.  See
     * http://www.freertos.org/a00110.html#configASSERT for more information. */

    LOG_INFO("ASSERT! Line %d, file %s\r\n", ( int ) ulLine, pcFileName );

    taskENTER_CRITICAL();
    {
        /* You can step out of this function to debug the assertion by using
         * the debugger to set ulSetToNonZeroInDebuggerToContinue to a non-zero
         * value. */
        while( ulSetToNonZeroInDebuggerToContinue == 0 )
        {
            __asm volatile ( "NOP" );
            __asm volatile ( "NOP" );
        }
    }
    taskEXIT_CRITICAL();
}


void vApplicationStackOverflowHook( TaskHandle_t pxTask,
                                    char * pcTaskName )
{
    ( void ) pcTaskName;
    ( void ) pxTask;

    /* Run time stack overflow checking is performed if
     * configCHECK_FOR_STACK_OVERFLOW is defined to 1 or 2.  This hook
     * function is called if a stack overflow is detected. */
    LOG_INFO("\r\n\r\nStack overflow in %s\r\n", pcTaskName );
    portDISABLE_INTERRUPTS();

    for( ; ; )
    {
    }
}
/*-----------------------------------------------------------*/

/* configUSE_STATIC_ALLOCATION is set to 1, so the application must provide an
 * implementation of vApplicationGetIdleTaskMemory() to provide the memory that is
 * used by the Idle task. */
void vApplicationGetIdleTaskMemory( StaticTask_t ** ppxIdleTaskTCBBuffer,
                                    StackType_t ** ppxIdleTaskStackBuffer,
                                    uint32_t * pulIdleTaskStackSize )
{
/* If the buffers to be provided to the Idle task are declared inside this
 * function then they must be declared static - otherwise they will be allocated on
 * the stack and so not exists after this function exits. */
    static StaticTask_t xIdleTaskTCB;
    static StackType_t uxIdleTaskStack[ configMINIMAL_STACK_SIZE ];

    /* Pass out a pointer to the StaticTask_t structure in which the Idle task's
     * state will be stored. */
    *ppxIdleTaskTCBBuffer = &xIdleTaskTCB;

    /* Pass out the array that will be used as the Idle task's stack. */
    *ppxIdleTaskStackBuffer = uxIdleTaskStack;

    /* Pass out the size of the array pointed to by *ppxIdleTaskStackBuffer.
     * Note that, as the array is necessarily of type StackType_t,
     * configMINIMAL_STACK_SIZE is specified in words, not bytes. */
    *pulIdleTaskStackSize = configMINIMAL_STACK_SIZE;
}

void vApplicationIdleHook( void )
{
}

/* Implementation of vApplicationGetTimerTaskMemory */
void vApplicationGetTimerTaskMemory(StaticTask_t **ppxTimerTaskTCBBuffer,
                                   StackType_t **ppxTimerTaskStackBuffer,
                                   uint32_t *pulTimerTaskStackSize)
{
    /* Static memory for the timer task TCB and stack */
    static StaticTask_t xTimerTaskTCB;
    static StackType_t uxTimerTaskStack[configTIMER_TASK_STACK_DEPTH];

    *ppxTimerTaskTCBBuffer = &xTimerTaskTCB;
    *ppxTimerTaskStackBuffer = uxTimerTaskStack;
    *pulTimerTaskStackSize = configTIMER_TASK_STACK_DEPTH;
}