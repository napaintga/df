/*******************************************************************************
* File Name: main.c
*
* Version: 1.00
*
* Description:
*  This is the source code for the CE217911 FreeRTOS Basic example project.
*
********************************************************************************
* Copyright 2016-2018, Cypress Semiconductor Corporation. All rights reserved.
* This software is owned by Cypress Semiconductor Corporation and is protected
* by and subject to worldwide patent and copyright laws and treaties.
* Therefore, you may use this software only as provided in the license agreement
* accompanying the software package from which you obtained this software.
* CYPRESS AND ITS SUPPLIERS MAKE NO WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,
* WITH REGARD TO THIS SOFTWARE, INCLUDING, BUT NOT LIMITED TO, NONINFRINGEMENT,
* IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
*******************************************************************************/
/* Driver includes */
#include "gpio/cy_gpio.h"
#include "sysint/cy_sysint.h"
#include "syslib/cy_syslib.h"

/* Kernel includes. */
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"
#include "queue.h"
#include "croutine.h"

/* The number of items the queue can hold.  This is 1 as the receive task
will remove items as they are added, meaning the send task should always find
the queue empty. */
#define mainQUEUE_LENGTH                    ( 1u )

/* Values passed to the two tasks just to check the task parameter
functionality. */
#define mainQUEUE_SEND_PARAMETER            ( 0x1111UL )
#define mainQUEUE_RECEIVE_PARAMETER         ( 0x22UL )

/* Priorities at which the tasks are created. */
#define mainQUEUE_RECEIVE_TASK_PRIORITY     ( tskIDLE_PRIORITY + 2u )
#define mainQUEUE_SEND_TASK_PRIORITY        ( tskIDLE_PRIORITY + 1u )

/* The rate at which data is sent to the queue, specified in milliseconds, and
converted to ticks using the portTICK_PERIOD_MS constant. */
#define mainQUEUE_SEND_FREQUENCY_MS         ( 500u / portTICK_PERIOD_MS )

/* GPIO used for the LED */
#define LED_PORT                            (GPIO_PRT0)
#define LED_SHIFT                           (3u)
#define LED                                 LED_PORT, LED_SHIFT

extern void vTaskStartScheduler(void);

static void prvQueueReceiveTask( void *pvParameters );
static void prvQueueSendTask( void *pvParameters );

/* The queue used by both tasks. */
static QueueHandle_t xQueue = NULL;


/*******************************************************************************
* Function Name: main()
********************************************************************************
* Summary:
*  Main function for the project.
*
* Parameters:
*  None
*
* Return:
*  None
*
* Theory:
*  Configures FreeRTOS and start two tasks.
*
*******************************************************************************/
int main(void)
{
    __enable_irq(); /* Enable global interrupts. */

    Cy_GPIO_Pin_FastInit(LED_PORT, LED_SHIFT, CY_GPIO_DM_STRONG, 0x00u, P0_3_GPIO);
    
    xQueue = xQueueCreate( mainQUEUE_LENGTH, sizeof( unsigned long ) );

    if( xQueue != NULL )
    {
        xTaskCreate( prvQueueReceiveTask,                    /* The function that implements the task. */
                    "Rx",                                    /* The text name assigned to the task - for debug only as it is not used by the kernel. */
                    configMINIMAL_STACK_SIZE,                /* The size of the stack to allocate to the task. */
                    ( void * ) mainQUEUE_RECEIVE_PARAMETER,  /* The parameter passed to the task - just to check the functionality. */
                    mainQUEUE_RECEIVE_TASK_PRIORITY,         /* The priority assigned to the task. */
                    NULL );                                  /* The task handle is not required, so NULL is passed. */

        xTaskCreate( prvQueueSendTask,                       /* The function that implements the task. */
                    "Tx",                                    /* The text name assigned to the task - for debug only as it is not used by the kernel. */
                    configMINIMAL_STACK_SIZE,                /* The size of the stack to allocate to the task. */
                    ( void * ) mainQUEUE_SEND_PARAMETER,     /* The parameter passed to the task - just to check the functionality. */
                    mainQUEUE_SEND_TASK_PRIORITY,            /* The priority assigned to the task. */
                    NULL );                                  /* The task handle is not required, so NULL is passed. */

        /* Start the tasks and timer running. */
        vTaskStartScheduler();
    }

    for(;;)
    {
        /* Should never reach this code. */
        taskDISABLE_INTERRUPTS();
        while (1u == 1u)
        {
        
        }
    }
}


/*******************************************************************************
* Function Name: prvQueueSendTask()
********************************************************************************
* Summary:
*  Main function for the Send Task.
*
* Parameters:
*  pvParameters - task parameters
*
* Return:
*  None
*
* Theory:
*  Calls vTaskDelayUntil and sends the message using xQueue message queue.
*
*******************************************************************************/
static void prvQueueSendTask( void *pvParameters )
{
    TickType_t xNextWakeTime;
    const unsigned long ulValueToSend = 100UL;

    /* Check the task parameter is as expected. */
    configASSERT( ( ( unsigned long ) pvParameters ) == mainQUEUE_SEND_PARAMETER );

    /* Initialize xNextWakeTime - this only needs to be done once. */
    xNextWakeTime = xTaskGetTickCount();

    for( ;; )
    {
        /* Place this task in the blocked state until it is time to run again.
        The block time is specified in ticks, the constant used converts ticks
        to ms.  While in the Blocked state this task will not consume any CPU
        time. */
        vTaskDelayUntil( &xNextWakeTime, mainQUEUE_SEND_FREQUENCY_MS );

        /* Send to the queue - causing the queue receive task to unblock and
        toggle the LED.  0 is used as the block time so the sending operation
        will not block - it shouldn't need to block as the queue should always
        be empty at this point in the code. */
        xQueueSend( xQueue, &ulValueToSend, 0U );
    }
}


/*******************************************************************************
* Function Name: prvQueueReceiveTask()
********************************************************************************
* Summary:
*  Main function for the Receive Task.
*
* Parameters:
*  pvParameters - task parameters
*
* Return:
*  None
*
* Theory:
*  Waits for the message in xQueue message queue and if it does match 100UL
*  inverts LED state.
*
*******************************************************************************/
static void prvQueueReceiveTask( void *pvParameters )
{
    unsigned long ulReceivedValue;

    /* Check the task parameter is as expected. */
    configASSERT( ( ( unsigned long ) pvParameters ) == mainQUEUE_RECEIVE_PARAMETER );

    for( ;; )
    {
        /* Wait until something arrives in the queue - this task will block
        indefinitely provided INCLUDE_vTaskSuspend is set to 1 in
        FreeRTOSConfig.h. */
        xQueueReceive( xQueue, &ulReceivedValue, portMAX_DELAY );

        /*  To get here something must have been received from the queue, but
        is it the expected value?  If it is, toggle the LED. */
        if( ulReceivedValue == 100UL )
        {
            Cy_GPIO_Inv(LED);
            ulReceivedValue = 0U;
        }
    }
}


/*******************************************************************************
* Function Name: vApplicationMallocFailedHook()
********************************************************************************
* Summary:
*  Hook that is called by the FreeRTOS if the heap size is not big enough.
*
*******************************************************************************/
void vApplicationMallocFailedHook( void )
{
    /* The heap space has been exceeded. */

    taskDISABLE_INTERRUPTS();
    
    while (1u == 1u)
    {
        /* Do nothing - this is a placeholder for a breakpoint */
    }
}


/*******************************************************************************
* Function Name: vApplicationStackOverflowHook()
********************************************************************************
* Summary:
*  Hook that is called by the FreeRTOS if the stack size is not big enough.
*
*******************************************************************************/
void vApplicationStackOverflowHook( TaskHandle_t pxTask, signed char *pcTaskName )
{
    /* The stack space has been exceeded for a task */
    
    (void)pxTask;
    (void)pcTaskName;

    taskDISABLE_INTERRUPTS();
    
    while (1u == 1u)
    {
        /* Do nothing - this is a placeholder for a breakpoint */
    }
}


/* [] END OF FILE */
