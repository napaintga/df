/***************************************************************************//**
* \file main_cm4.c
* \version 1.0
*
* \brief
* Sample CM4 user application used to verify SysCall and Crypto calls to
* the secure image.
*
********************************************************************************
* \copyright
* Copyright 2017-2018, Cypress Semiconductor Corporation. All rights reserved.
* You may use this file only in accordance with the license, terms, conditions,
* disclaimers, and limitations in the end user license agreement accompanying
* the software package with which this file was provided.
*******************************************************************************/
 
#include "project.h" 
#include "cy_crypto_config.h"
#include "../../src/cy_si_config.h"


/********************************/
/*    Application Signature     */
/********************************/
CY_SECTION(".cy_app_signature") __USED CY_ALIGN(4) const uint8_t cy_user_appSignature[256];


/********************************/
/*     Globals and Macros       */
/********************************/
#define TEST_FLASHROW_ADDR      (0x14000000UL)


/*******************************************************************************
* Function Name: main
****************************************************************************//**
* 
* \brief
*  CM4 User application main.
*
*******************************************************************************/
int main(void)
{
    /********************************/
    /*          Variables           */
    /********************************/
    uint32_t outPtr[4] = {0x01UL, 0x02UL, 0x03UL, 0x04UL};
    uint32_t trngAns;

    static cy_stc_crypto_context_t  client_cryptoContext;
    cy_stc_crypto_context_trng_t    trngContext;
    cy_en_crypto_status_t           cStatus = CY_CRYPTO_SUCCESS;
    cy_en_flashdrv_status_t         sStatus = CY_FLASH_DRV_SUCCESS;
    
    /********************************/
    /*      Begin Application       */
    /********************************/
    __enable_irq();
 
    /***************************************/
    /*   SysCalls test using Flash driver  */
    /***************************************/
    sStatus = Cy_Flash_WriteRow(TEST_FLASHROW_ADDR, outPtr);
    if(sStatus == CY_FLASH_DRV_SUCCESS)
    {
        sStatus = Cy_Flash_CalculateHash((uint32_t *)TEST_FLASHROW_ADDR, 4UL, outPtr);

        /* CalculateHash requires PC=0 in SysCall. Expected to fail for CM4 (PC=1) */
        if(sStatus != CY_FLASH_DRV_SUCCESS)
        {
            sStatus = Cy_Flash_RowChecksum (TEST_FLASHROW_ADDR, outPtr);
        }
        /* Cy_Flash_RowChecksum requires PC=0 in SysCall. Expected to fail for CM4 (PC=1) */
        if(sStatus != CY_FLASH_DRV_SUCCESS)
        {
            sStatus = CY_FLASH_DRV_SUCCESS;
        }
        else
        {
            sStatus = CY_FLASH_DRV_ERR_UNC;
        }
    }
    
    /****************************************/
    /*    Crypto test using Crypto driver   */
    /****************************************/
    cStatus = Cy_Crypto_Init(&cryptoConfig, &client_cryptoContext);
    if(cStatus == CY_CRYPTO_SUCCESS)
    {
        cStatus = Cy_Crypto_Sync(true);
    }
    if(cStatus == CY_CRYPTO_SUCCESS)
    {
        cStatus = Cy_Crypto_Enable();
    }
    if(cStatus == CY_CRYPTO_SUCCESS)
    {
        cStatus = Cy_Crypto_Trng_Generate(33UL, 33UL, 12UL, &trngAns, &trngContext);
    }
    if(cStatus == CY_CRYPTO_SUCCESS)
    {
        cStatus = Cy_Crypto_Sync(true);
    }
    if(cStatus == CY_CRYPTO_SUCCESS)
    {
        cStatus = Cy_Crypto_Trng_Generate(20UL, 50UL, 8UL, &trngAns, &trngContext);
    }
    if(cStatus == CY_CRYPTO_SUCCESS)
    {
        cStatus = Cy_Crypto_Sync(true);
    }
    
    /* Check Test states and start the blinky LED */
    if((cStatus == CY_CRYPTO_SUCCESS) && (sStatus == CY_FLASH_DRV_SUCCESS))
    {
        Cy_GPIO_Pin_FastInit(P1_1_PORT, P1_1_NUM, CY_GPIO_DM_STRONG, 0UL, HSIOM_SEL_GPIO);
        while(1)
        { 
            Cy_GPIO_Inv(P1_1_PORT, P1_1_NUM); 
            CyDelay(500UL);
        }
    }
    else
    {
        /* Check if the return code came from Secure Image */
        cy_en_si_status_t cStatusSi;
        cy_en_si_status_t sStatusSi;
        
        cStatusSi = (CY_SI_CHECKID(cStatus) == CY_SI_ID) ? (cy_en_si_status_t)cStatus : CY_SI_SUCCESS;
        sStatusSi = (sStatus == CY_FLASH_DRV_ERR_UNC) ? (cy_en_si_status_t)Cy_Flash_GetExternalStatus() : CY_SI_SUCCESS;
        
        if((cStatusSi != CY_SI_SUCCESS) && (sStatusSi != CY_SI_SUCCESS))
        {
            /* Error came from Crypto task or SysCall task */
        }
        else
        {
            /* Error came from Crypto core or ROM */
        }
    }
    
    for(;;){}
}

#if defined (__ARMCC_VERSION)
/*******************************************************************************
* Function Name: Cy_SI_Symbols
****************************************************************************//**
*
* Defines the symbols to be used by the cymcuelftool to generate the user 
* application digital signature.
*
*******************************************************************************/
__asm void Cy_SI_Symbols(void)
{
    EXPORT __cy_app_verify_start
    EXPORT __cy_app_verify_length    

__cy_app_verify_start     EQU __cpp(CY_SI_USERAPP_FLASH_BEGIN)
__cy_app_verify_length    EQU __cpp(CY_SI_USERAPP_FLASH_END - CY_SI_USERAPP_FLASH_BEGIN - CY_SI_USERAPP_DIGSIG_SIZE)
}
#endif /* defined (__ARMCC_VERSION) */

/* [] END OF FILE */
