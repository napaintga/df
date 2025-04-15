/***************************************************************************//**
* \file transport_i2c.h
* \version 2.20
*
* This file provides constants and parameter values of the bootloader
* communication APIs for the SCB Component.
*
* Note that component name has to be I2C.
*
********************************************************************************
* \copyright
* Copyright 2016-2018, Cypress Semiconductor Corporation. All rights reserved.
* You may use this file only in accordance with the license, terms, conditions,
* disclaimers, and limitations in the end user license agreement accompanying
* the software package with which this file was provided.
*******************************************************************************/

#if !defined(TRANSPORT_I2C_H)
#define TRANSPORT_I2C_H

#include "bootloader/cy_bootload.h"

    
/***************************************
*        Function Prototypes
***************************************/

/* I2C Bootloader physical layer functions */
void I2C_I2cCyBtldrCommStart(void);
void I2C_I2cCyBtldrCommStop (void);
void I2C_I2cCyBtldrCommReset(void);
cy_en_bootload_status_t I2C_I2cCyBtldrCommRead (uint8_t pData[], uint32_t size, uint32_t *count, uint32_t timeout);
cy_en_bootload_status_t I2C_I2cCyBtldrCommWrite(const uint8_t pData[], uint32_t size, uint32_t *count, uint32_t timeOut);


#endif /* !defined(TRANSPORT_I2C_H) */


/* [] END OF FILE */
