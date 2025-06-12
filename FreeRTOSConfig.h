/*
* SPDX-License-Identifier: BSD-2-Clause
*
* Copyright (c) 2025 MIPS
*
*/

#ifndef FREERTOS_CONFIG_H
#define FREERTOS_CONFIG_H

// TODO : Add includes for below CLINT
// TODO : Definations of CLINT - Should be REMOVED once these drivers is added. 
#define CLINT_ADDR			0x02000000UL
#define CLINT_MSIP			0x0000UL
#define CLINT_MTIMECMP		0x4000UL
#define CLINT_MTIME			0xbff8UL

#define configMTIME_BASE_ADDRESS		( CLINT_ADDR + CLINT_MTIME )
#define configMTIMECMP_BASE_ADDRESS		( CLINT_ADDR + CLINT_MTIMECMP )
#define configISR_STACK_SIZE_WORDS		( 300 )

#define configUSE_PREEMPTION			1
#define configUSE_IDLE_HOOK				0
#define configUSE_TICK_HOOK				0
#define configCPU_CLOCK_HZ				( ( unsigned long ) 25000000 )
#define configTICK_RATE_HZ				( ( TickType_t ) 1000 )
#define configMINIMAL_STACK_SIZE		( ( unsigned short ) 120 )
#define configTOTAL_HEAP_SIZE			( ( size_t ) ( 4 * 1024 ) )
#define configMAX_TASK_NAME_LEN			( 12 )
#define configUSE_16_BIT_TICKS			0
#define configCHECK_FOR_STACK_OVERFLOW	1
#define configUSE_PORT_OPTIMISED_TASK_SELECTION 1
#define configENABLE_TASK_DELETION          1
#define configMAX_PRIORITIES			( 9UL )
#define configSUPPORT_STATIC_ALLOCATION	1

/* Timer related defines. */
#define configUSE_TIMERS				0

#define configUSE_TASK_NOTIFICATIONS	0
#define configUSE_STREAM_BUFFERS        0

/* Set the following definitions to 1 to include the API function, or zero
to exclude the API function. */

#define INCLUDE_xTaskResumeFromISR	    0
#define INCLUDE_vTaskDelete				1

#ifndef __ASSEMBLER__    /* Exclude function prototypes from assembly code to ensure compatibility with portASM. */
    void vAssertCalled( const char *pcFileName, uint32_t ulLine );
    #define configASSERT( x ) if( ( x ) == 0 ) vAssertCalled( __FILE__, __LINE__ );
#endif

#endif /* FREERTOS_CONFIG_H */
