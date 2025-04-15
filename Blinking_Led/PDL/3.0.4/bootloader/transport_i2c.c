/***************************************************************************//**
* \file transport_i2c.c
* \version 2.20
*
* This file provides the source code of the bootloader communication APIs
* for the SCB Component I2C mode.
*
* Note that component name has to be I2C.
*
********************************************************************************
* \copyright
* Copyright 2016-2018, Cypress Semiconductor Corporation.  All rights reserved.
* You may use this file only in accordance with the license, terms, conditions,
* disclaimers, and limitations in the end user license agreement accompanying
* the software package with which this file was provided.
*******************************************************************************/

#include "transport_i2c.h"
#include "scb/cy_scb_i2c.h"
#include "sysint/cy_sysint.h"
#include <string.h>


#if defined(CY_PSOC_CREATOR_USED)
    
    /* USER CONFIGURABLE: Change the header file based on the I2C component instance name */
    #include "I2C.h"
    
    /* USER CONFIGURABLE: Instance name of the I2C component */
    #define CY_BOOTLOAD_I2C_INSTANCE    I2C
    
    #define JOIN_LEVEL2(a, b)           a ## b
    #define JOIN_LEVEL1(a, b)           JOIN_LEVEL2(a, b)
    #define CY_BOOTLOAD_I2C_HW          JOIN_LEVEL1(CY_BOOTLOAD_I2C_INSTANCE, _SCB__HW)
    #define I2C_API(fn)                 JOIN_LEVEL1(CY_BOOTLOAD_I2C_INSTANCE, fn)
#else
    /* USER CONFIGURABLE: The SCB block used for bootloader communication */
    #define CY_BOOTLOAD_I2C_HW          SCB3
    
    /* USER CONFIGURABLE: Interrupt configuration for the I2C block.
     * This configuration is applied for the core which calls 
     * Cy_Bootload_TransportStart(). Configure INTC_NUMBER, PRIORITY, and
     * INTC_MUX in addition for CM0+. 
     */
    #define CY_I2C_SCB_IRQ__INTC_NUMBER                 scb_3_interrupt_IRQn
                                                        
    #if(CY_CPU_CORTEX_M4)                               
        #define CY_I2C_SCB_IRQ__INTC_CORTEXM4_PRIORITY   (7u) /* Interrupt priority for Cortex-M4. Valid range: 0 to 7 */
    #elif(CY_CPU_CORTEX_M0P)                                
        #define CY_I2C_2_SCB_IRQ__INTC_CORTEXM0P_MUX     (0u) /* Interrupt mux to use for Cortex-M0+. Valid range: 0 to 27 */
        #define CY_I2C_SCB_IRQ__INTC_CORTEXM0P_PRIORITY  (3u) /* Interrupt priority for Cortex-M0+. Valid range: 1 to 3 */
    #endif /* #if(CY_CPU_CORTEX_M4) */
    
    /** The instance-specific configuration structure.
     * The pointer to this structure should be passed to Cy_SCB_I2C_Init function
     * to initialize component with GUI selected settings.
     */
    static cy_stc_scb_i2c_config_t const I2C_config =
    {
        .i2cMode    = CY_SCB_I2C_SLAVE,

        .useRxFifo = true,
        .useTxFifo = true,

        .slaveAddress        = 0x8U,
        .slaveAddressMask    = 0xFEU,
        .acceptAddrInFifo    = false,
        .ackGeneralAddr      = false,

        .enableWakeFromSleep = false
    };

    /** The instance-specific context structure.
    * It is used while the driver operation for internal configuration and
    * data keeping for the I2C. The user should not modify anything in this
    * structure.
    */
    static cy_stc_scb_i2c_context_t I2C_context;
    
    /* Interrupt configuration structure */
    #if(CY_CPU_CORTEX_M4)
        static const cy_stc_sysint_t I2C_SCB_IRQ_cfg = {
            .intrSrc = (IRQn_Type)CY_I2C_SCB_IRQ__INTC_NUMBER,
            .intrPriority = CY_I2C_SCB_IRQ__INTC_CORTEXM4_PRIORITY
    };
    #elif(CY_CPU_CORTEX_M0P)
        static const cy_stc_sysint_t I2C_SCB_IRQ_cfg = {
            .intrSrc = (IRQn_Type)CY_I2C_2_SCB_IRQ__INTC_CORTEXM0P_MUX,
            .cm0pSrc = (cy_en_intr_t)CY_I2C_SCB_IRQ__INTC_NUMBER,
            .intrPriority = CY_I2C_SCB_IRQ__INTC_CORTEXM0P_PRIORITY
        };
    #endif /* #if(CY_CPU_CORTEX_M4) */
    
    __STATIC_INLINE void I2C_Interrupt(void)
    {
        Cy_SCB_I2C_Interrupt(CY_BOOTLOAD_I2C_HW, &I2C_context);
    }
    
#endif  /* #if defined(CY_PSOC_CREATOR_USED) */


/* Size of Read/Write buffers for I2C bootloader  */
#define I2C_BTLDR_SIZEOF_TX_BUFFER   (64u)
#define I2C_BTLDR_SIZEOF_RX_BUFFER   (64u)

/* Timeout unit in millisecond */
#define I2C_WAIT_1_MS  (1u)

/* Return number of bytes to copy into bootloader buffer */
#define I2C_BYTES_TO_COPY(actBufSize, bufSize) \
                            ( ((uint32)(actBufSize) < (uint32)(bufSize)) ? \
                                ((uint32) (actBufSize)) : ((uint32) (bufSize)) )

/* Writes to this buffer */
static uint8_t I2C_slaveTxBuf[I2C_BTLDR_SIZEOF_TX_BUFFER];

/* Reads from this buffer */
static uint8_t I2C_slaveRxBuf[I2C_BTLDR_SIZEOF_RX_BUFFER];

/* Flag to release buffer to be read */
static uint32_t I2C_applyBuffer;

/* Callback to insert the response on a read request */
static void I2C_I2CResposeInsert(uint32_t event);

/*******************************************************************************
* Function Name: I2C_I2cCyBtldrCommStart
****************************************************************************//**
*
*  Starts the I2C component.
*
*******************************************************************************/
void I2C_I2cCyBtldrCommStart(void)
{
    #if defined(CY_PSOC_CREATOR_USED)
        I2C_API(_Start)();
    #else
        /* Configure the I2C block */
        (void) Cy_SCB_I2C_Init(CY_BOOTLOAD_I2C_HW, &I2C_config, &I2C_context);
        
        (void) Cy_SysInt_Init(&I2C_SCB_IRQ_cfg, &I2C_Interrupt);
        NVIC_EnableIRQ((IRQn_Type) I2C_SCB_IRQ_cfg.intrSrc);
        
        Cy_SCB_I2C_Enable(CY_BOOTLOAD_I2C_HW);
    #endif /* CY_PSOC_CREATOR_USED */
       
    Cy_SCB_I2C_SlaveConfigReadBuf(CY_BOOTLOAD_I2C_HW, I2C_slaveTxBuf, 0u, &I2C_context);
    Cy_SCB_I2C_SlaveConfigWriteBuf(CY_BOOTLOAD_I2C_HW, I2C_slaveRxBuf, I2C_BTLDR_SIZEOF_RX_BUFFER, &I2C_context);
    Cy_SCB_I2C_RegisterEventCallback(CY_BOOTLOAD_I2C_HW, &I2C_I2CResposeInsert, &I2C_context);
    I2C_applyBuffer = 0u;
}


/*******************************************************************************
* Function Name: I2C_I2cCyBtldrCommStop
****************************************************************************//**
*
*  Disables the I2C component.
*
*******************************************************************************/
void I2C_I2cCyBtldrCommStop(void)
{
    Cy_SCB_I2C_Disable(CY_BOOTLOAD_I2C_HW, &I2C_context);
    Cy_SCB_I2C_DeInit(CY_BOOTLOAD_I2C_HW);
}


/*******************************************************************************
* Function Name: I2C_I2cCyBtldrCommReset
****************************************************************************//**
*
*  Resets the receive and transmit communication buffers and the slave status.
*
*******************************************************************************/
void I2C_I2cCyBtldrCommReset(void)
{
    Cy_SCB_ClearTxFifo(CY_BOOTLOAD_I2C_HW);
    Cy_SCB_ClearRxFifo(CY_BOOTLOAD_I2C_HW);
    
    Cy_SCB_I2C_SlaveConfigReadBuf(CY_BOOTLOAD_I2C_HW, I2C_slaveTxBuf, 0u, &I2C_context);
    Cy_SCB_I2C_SlaveConfigWriteBuf(CY_BOOTLOAD_I2C_HW, I2C_slaveRxBuf, I2C_BTLDR_SIZEOF_RX_BUFFER, &I2C_context);
    
    (void)Cy_SCB_I2C_SlaveClearReadStatus(CY_BOOTLOAD_I2C_HW, &I2C_context);
    (void)Cy_SCB_I2C_SlaveClearWriteStatus(CY_BOOTLOAD_I2C_HW, &I2C_context);
    
    I2C_applyBuffer = 0u;
}


/*******************************************************************************
* Function Name: I2C_I2cCyBtldrCommRead
****************************************************************************//**
*
*  Allows the caller to read data from the bootloader host (the host writes the
*  data). The function handles polling to allow a block of data to be completely
*  received from the host device.
*
*  \param pData: Pointer to storage for the block of data to be read from the
*   bootloader host
*  \param size: Number of bytes to be read.
*  \param count: Pointer to the variable to write the number of bytes actually
*   read.
*  \param timeOut The amount of time (in milliseconds) for which the
*                function should wait before indicating communication
*                time out.
*
*  \return
*   Returns CYRET_SUCCESS if no problem was encountered or returns the value
*   that best describes the problem. For more information refer to the
*   "Return Codes" section of the System Reference Guide.
*
*******************************************************************************/
cy_en_bootload_status_t I2C_I2cCyBtldrCommRead(uint8_t pData[], uint32_t size, uint32_t *count, uint32_t timeout)
{
    cy_en_bootload_status_t status = CY_BOOTLOAD_ERROR_UNKNOWN;

    if ((pData != NULL) && (size > 0u))
    {
        status = CY_BOOTLOAD_ERROR_TIMEOUT;
 
        while (0u != timeout)
        {
            /* Check if host complete write */
            if (0u != (Cy_SCB_I2C_SlaveGetStatus(CY_BOOTLOAD_I2C_HW, &I2C_context) & CY_SCB_I2C_SLAVE_WR_CMPLT))
            {
                /* Get number of received bytes */
                *count = I2C_BYTES_TO_COPY(Cy_SCB_I2C_SlaveGetWriteTransferCount(CY_BOOTLOAD_I2C_HW, &I2C_context), size);

                /* Clear slave status */
                (void)Cy_SCB_I2C_SlaveClearWriteStatus(CY_BOOTLOAD_I2C_HW, &I2C_context);

                /* Copy command into bootloader buffer */
                (void) memcpy((void *) pData, (const void *) I2C_slaveRxBuf, *count);
                
                /* Prepare the slave buffer for next reception */
                Cy_SCB_I2C_SlaveConfigWriteBuf(CY_BOOTLOAD_I2C_HW, I2C_slaveRxBuf, I2C_BTLDR_SIZEOF_RX_BUFFER, &I2C_context);
                status = CY_BOOTLOAD_SUCCESS;
                break;
            }

            CyDelay(I2C_WAIT_1_MS);
            --timeout;
        }
    }

    return (status);
}


/*******************************************************************************
* Function Name: I2C_I2cCyBtldrCommWrite
****************************************************************************//**
*
*  Allows the caller to write data to the bootloader host (the host reads the
*  data). The function does not use timeout and returns after data has been
*  copied into the transmit buffer. The data transmission starts immediately
*  after the first data element is written into the buffer and lasts until all
*  data elements from the buffer are sent.
*
*  \param pData: Pointer to the block of data to be written to the bootloader
*   host.
*  \param size: Number of bytes to be written.
*  \param count: Pointer to the variable to write the number of bytes actually
*   written.
*  \param timeOut: The timeout is not used by this function.
*   The function returns as soon as data is copied into the transmit buffer.
*
*  \return
*   Returns CYRET_SUCCESS if no problem was encountered or returns the value
*   that best describes the problem. For more information refer to the
*   "Return Codes" section of the System Reference Guide.
*
*******************************************************************************/
cy_en_bootload_status_t I2C_I2cCyBtldrCommWrite(const uint8_t pData[], uint32_t size, uint32_t *count, uint32_t timeOut)
{
    cy_en_bootload_status_t status = CY_BOOTLOAD_ERROR_UNKNOWN;
    
    if ((NULL != pData) && (size > 0u))
    {
        /* Copy response into read buffer */
        *count = size;
        (void) memcpy((void *) I2C_slaveTxBuf, (const void *) pData, (uint32_t) size);

        /* Read buffer is ready to be released to host */
        I2C_applyBuffer = (uint32_t) size;

        if (0u != timeOut)
        {
            /* Suppress compiler warning */
        }

        status = CY_BOOTLOAD_SUCCESS;
    }

    return (status);
}


/*******************************************************************************
* Function Name: I2C_I2CResposeInsert
****************************************************************************//**
*
*  Releases the read buffer to be read when a response is copied to the buffer
*  and a new read transaction starts.
*  Closes the read buffer when write transaction is started.
*
* \globalvars
*  I2C_applyBuffer - the flag to release the buffer with a response
*  to be read by the host.
*
*******************************************************************************/
static void I2C_I2CResposeInsert(uint32_t event)
{
    if ((CY_SCB_I2C_SLAVE_READ_EVENT == event) && (0u != I2C_applyBuffer))
    {
        /* Address phase, host reads: release read buffer */
        (void)Cy_SCB_I2C_SlaveClearReadStatus(CY_BOOTLOAD_I2C_HW, &I2C_context);
        Cy_SCB_I2C_SlaveConfigReadBuf(CY_BOOTLOAD_I2C_HW, I2C_slaveTxBuf, I2C_applyBuffer, &I2C_context);
        I2C_applyBuffer  = 0u;
    }
    else if(CY_SCB_I2C_SLAVE_WRITE_EVENT == event)
    {
        /* Address phase, host writes: make read buffer empty so that host will
         * receive only 0xFF (CY_SCB_I2C_DEFAULT_TX) until the bootloader
         * application has a valid response packet.
         */
        Cy_SCB_I2C_SlaveConfigReadBuf(CY_BOOTLOAD_I2C_HW, I2C_slaveTxBuf, 0u, &I2C_context);
    }
    else
    {
        /* No action */
    }
}

/* [] END OF FILE */
