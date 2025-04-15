/***************************************************************************//**
* \file transport_spi.c
* \version 2.20
*
* This file provides the source code of the bootloader communication APIs
* for the SCB Component SPI mode.
*
* Note that component name has to be SPI.
*
********************************************************************************
* \copyright
* Copyright 2016-2018, Cypress Semiconductor Corporation.  All rights reserved.
* You may use this file only in accordance with the license, terms, conditions,
* disclaimers, and limitations in the end user license agreement accompanying
* the software package with which this file was provided.
*******************************************************************************/

#include "transport_spi.h"
#include "scb/cy_scb_spi.h"

#if defined(CY_PSOC_CREATOR_USED)
    
    /* USER CONFIGURABLE: Change the header file based on the I2C component instance name */
    #include "SPI.h"
    
    /* USER CONFIGURABLE: Instance name of the I2C component */
    #define CY_BOOTLOAD_SPI_INSTANCE    SPI
                                                 
    #define JOIN_LEVEL2(a, b)           a ## b
    #define JOIN_LEVEL1(a, b)           JOIN_LEVEL2(a, b)
    #define CY_BOOTLOAD_SPI_HW          JOIN_LEVEL1(CY_BOOTLOAD_SPI_INSTANCE, _SCB__HW)
    #define SPI_API(fn)                 JOIN_LEVEL1(CY_BOOTLOAD_SPI_INSTANCE, fn)
#else
    /* USER CONFIGURABLE: The SCB block used for bootloader communication */
    #define CY_BOOTLOAD_SPI_HW          SCB1   
    
    /* USER CONFIGURABLE: The slave select line constant. Update it based on the
     * pin selected for slave select.
     */
    #define CY_SPI_SLAVE_SELECT     CY_SCB_SPI_SLAVE_SELECT0
    
    /** The instance-specific configuration structure.
     * The pointer to this structure should be passed to Cy_SCB_SPI_Init function
     * to initialize component with GUI selected settings.
     */
    static cy_stc_scb_spi_config_t const SPI_config =
    {
        .spiMode  = CY_SCB_SPI_SLAVE,
        .subMode  = CY_SCB_SPI_MOTOROLA,
        .sclkMode = CY_SCB_SPI_CPHA0_CPOL0,

        .oversample = 0UL,

        .rxDataWidth              = 8UL,
        .txDataWidth              = 8UL,
        .enableMsbFirst           = true,
        .enableInputFilter        = false,

        .enableFreeRunSclk        = false,
        .enableMisoLateSample     = false,
        .enableTransferSeperation = false,
        .ssPolarity               = ((((uint32_t) CY_SCB_SPI_ACTIVE_LOW) << CY_SCB_SPI_SLAVE_SELECT0) | \
                                     (((uint32_t) CY_SCB_SPI_ACTIVE_LOW) << CY_SCB_SPI_SLAVE_SELECT1) | \
                                     (((uint32_t) CY_SCB_SPI_ACTIVE_LOW) << CY_SCB_SPI_SLAVE_SELECT2) | \
                                     (((uint32_t) CY_SCB_SPI_ACTIVE_LOW) << CY_SCB_SPI_SLAVE_SELECT3)),

        .enableWakeFromSleep = false,

        .rxFifoTriggerLevel  = 0UL,
        .rxFifoIntEnableMask = 0x0UL,

        .txFifoTriggerLevel  = 0UL,
        .txFifoIntEnableMask = 0x0UL,

        .masterSlaveIntEnableMask = 0x0UL
    };

    /** The instance-specific context structure.
     * It is used while the driver operation for internal configuration and
     * data keeping for the SPI. The user should not modify anything in this 
     * structure.
     */
    static cy_stc_scb_spi_context_t SPI_context;
#endif  /* #if defined(CY_PSOC_CREATOR_USED) */    

/* Byte to byte time interval in microseconds. Slave waits for this amount of
 * time between checking whether more data is being received. Slave starts
 * processing the packet if the number of received bytes remain same between
 * two successive checks. Change this depending on the expected inter-byte delay
 * for a given SPI master. 
 */
#define SPI_SPI_BYTE_TO_BYTE   (32u)

/* Timeout unit in millisecond */
#define SPI_WAIT_1_MS           (1u)

/* Return number of bytes to copy into bootloader buffer */
#define SPI_BYTES_TO_COPY(actBufSize, bufSize) \
                            ( ((uint32)(actBufSize) < (uint32)(bufSize)) ? \
                                ((uint32) (actBufSize)) : ((uint32) (bufSize)) )                            
                            
                                
/*******************************************************************************
* Function Name: SPI_SpiCyBtldrCommStart
****************************************************************************//**
*
*  Starts the SPI component.
*
*******************************************************************************/
void SPI_SpiCyBtldrCommStart(void)
{
    #if defined(CY_PSOC_CREATOR_USED)
        SPI_API(_Start)();
    #else
        /* Configure component */
        (void) Cy_SCB_SPI_Init(CY_BOOTLOAD_SPI_HW, &SPI_config, &SPI_context);

        /* Set active slave select to line 0 */
        Cy_SCB_SPI_SetActiveSlaveSelect(CY_BOOTLOAD_SPI_HW, CY_SPI_SLAVE_SELECT);
        Cy_SCB_SPI_Enable(CY_BOOTLOAD_SPI_HW);
    #endif /* #if defined(CY_PSOC_CREATOR_USED) */
}


/*******************************************************************************
* Function Name: SPI_SpiCyBtldrCommStop
****************************************************************************//**
*
*  Disables the SPI component.
*
*******************************************************************************/
void SPI_SpiCyBtldrCommStop(void)
{
    Cy_SCB_SPI_Disable(CY_BOOTLOAD_SPI_HW, &SPI_context);
    Cy_SCB_SPI_DeInit(CY_BOOTLOAD_SPI_HW);
}


/*******************************************************************************
* Function Name: SPI_SpiCyBtldrCommReset
****************************************************************************//**
*
*  Resets the receive and transmit communication buffers and the slave status.
*
*******************************************************************************/
void SPI_SpiCyBtldrCommReset(void)
{
    Cy_SCB_SPI_ClearTxFifo(CY_BOOTLOAD_SPI_HW);
    Cy_SCB_SPI_ClearRxFifo(CY_BOOTLOAD_SPI_HW);
}


/*******************************************************************************
* Function Name: SPI_SpiCyBtldrCommRead
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
cy_en_bootload_status_t SPI_SpiCyBtldrCommRead(uint8_t pData[], uint32_t size, uint32_t *count, uint32_t timeout)
{
    cy_en_bootload_status_t statusLoc = CY_BOOTLOAD_ERROR_UNKNOWN;
    uint32_t byteCount;

    if ((pData != NULL) && (size > 0u))
    {
        statusLoc = CY_BOOTLOAD_ERROR_TIMEOUT;
        
        /* Wait with timeout 1mS for packet start */
        byteCount = 0u;
 
        do
        {
            /* Check packet start */
            if (0u != Cy_SCB_GetNumInRxFifo(CY_BOOTLOAD_SPI_HW))
            {
                /* Wait for packet end */
                do
                {
                    byteCount = Cy_SCB_GetNumInRxFifo(CY_BOOTLOAD_SPI_HW);
                    CyDelayUs(SPI_SPI_BYTE_TO_BYTE);
                }
                while (byteCount != Cy_SCB_GetNumInRxFifo(CY_BOOTLOAD_SPI_HW));

                /* Disable data reception into RX FIFO */
                CY_BOOTLOAD_SPI_HW->RX_FIFO_CTRL |= SCB_RX_FIFO_CTRL_FREEZE_Msk;

                byteCount = SPI_BYTES_TO_COPY(byteCount, size);
                *count = byteCount;
                
                /* Get data from RX buffer into bootloader buffer */
                (void) Cy_SCB_SPI_ReadArray(CY_BOOTLOAD_SPI_HW, (void*)pData, byteCount);
               
                statusLoc = CY_BOOTLOAD_SUCCESS;
                break;
            }

            CyDelay(SPI_WAIT_1_MS);
            --timeout;
        }
        while (0u != timeout);
    }

    return (statusLoc);
}


/*******************************************************************************
* Function Name: SPI_SpiCyBtldrCommWrite
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
cy_en_bootload_status_t SPI_SpiCyBtldrCommWrite(const uint8_t pData[], uint32_t size, uint32_t *count, uint32_t timeout)
{
    cy_en_bootload_status_t statusLoc = CY_BOOTLOAD_ERROR_UNKNOWN;

    if ((NULL != pData) && (size > 0u))
    {
        statusLoc = CY_BOOTLOAD_ERROR_TIMEOUT;

        Cy_SCB_SPI_ClearTxFifo(CY_BOOTLOAD_SPI_HW);
        
        /* Put data into TX buffer */
        Cy_SCB_SPI_WriteArrayBlocking(CY_BOOTLOAD_SPI_HW, (void *)pData, size);

        /* Wait with timeout 1mS for packet end */
        do
        {
            /* Check for packet end */
            uint32_t numInTxFifo = Cy_SCB_SPI_GetNumInTxFifo(CY_BOOTLOAD_SPI_HW); 
            uint32_t txSrValid = Cy_SCB_GetTxSrValid(CY_BOOTLOAD_SPI_HW);
            if ((!Cy_SCB_SPI_IsBusBusy(CY_BOOTLOAD_SPI_HW)) &&
                (0u == numInTxFifo) &&
                (0u == txSrValid))
            {
                *count = size;
                statusLoc = CY_BOOTLOAD_SUCCESS;
                break;
            }

            CyDelay(SPI_WAIT_1_MS);
            --timeout;
        }
        while (0u != timeout);

        /* Enable data reception into RX FIFO */
        Cy_SCB_SPI_ClearRxFifo(CY_BOOTLOAD_SPI_HW);
        CY_BOOTLOAD_SPI_HW->RX_FIFO_CTRL &= (uint32_t)~SCB_RX_FIFO_CTRL_FREEZE_Msk;
    }

    return (statusLoc);
}


/* [] END OF FILE */
