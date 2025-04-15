/***************************************************************************//**
* \file main_cm0p.c
* \version 1.0
*
* \brief
* Sample CM0+ user application to verify SysCall and Crypto calls to the
* secure image.
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
/*    Application Header        */
/********************************/
#define CY_USERAPP_MAJOR_VERSION    (1UL)           /* App Major version [27:24] */
#define CY_USERAPP_MINOR_VERSION    (0UL)           /* App Minor version [23:16] */
#define CY_USERAPP_VERSION          ((CY_USERAPP_MAJOR_VERSION << 24u) | (CY_USERAPP_MINOR_VERSION << 16u)) /* App version */
#define CY_USERAPP_TYPE             (0x0001UL)      /* User defined application type */
#define CY_USERAPP_ID               (CY_USERAPP_VERSION | CY_USERAPP_TYPE) /* Application ID */
#define CY_USERAPP_CM0P_CPUID       (0xC6000000UL)  /* CM0+ CPUID[15:4] */
#define CY_USERAPP_CM4_CPUID        (0xC2400000UL)  /* CM4 CPUID[15:4] */

#define CY_USERAPP_CM0P_VT_OFFSET   ((uint32_t)(&__Vectors[0]) - CY_SI_USERAPP_FLASH_BEGIN \
                                        - offsetof(cy_stc_user_appheader_t, core0Vt))  /** CM0+ VT offset */
#define CY_USERAPP_CM4_VT_OFFSET    (CY_SI_USERAPP_CM4_FLASH_BEGIN - CY_SI_USERAPP_FLASH_BEGIN \
                                        - offsetof(cy_stc_user_appheader_t, core1Vt)) /** CM4 VT offset */

/** User application header */
CY_SECTION(".cy_app_header") __USED static const cy_stc_user_appheader_t cy_user_appHeader = {
    .objSize        = CY_SI_USERAPP_FLASH_END - CY_SI_USERAPP_FLASH_BEGIN - CY_SI_USERAPP_DIGSIG_SIZE,
    .appId          = CY_USERAPP_ID,                /* Application ID */
    .appAttributes  = 0UL,                          /* Reserved */
    .numCores       = 2UL,                          /* CM0+ and CM4 */
    .core0Vt        = CY_USERAPP_CM0P_VT_OFFSET,    /* CM0+ Vector table offset */
    .core1Vt        = CY_USERAPP_CM4_VT_OFFSET,     /* CM4 Vector table offset */
    .core0Id        = CY_USERAPP_CM0P_CPUID,        /* CM0+ core ID */
    .core1Id        = CY_USERAPP_CM4_CPUID          /* CM4 core ID */
};


/********************************/
/*     Globals and Macros       */
/********************************/
#define TEST_FLASHROW_ADDR     (0x14000000UL)


/********************************/
/*          Functions           */
/********************************/

/*******************************************************************************
* Function Name: main 
****************************************************************************//**
* 
* \brief
*  CM0+ User application main.
*
*******************************************************************************/
int main(void)
{
    /* Optional: Set the MSP to the one defined in the Secure Image */
    __set_MSP(*(uint32_t *)CY_SI_SECURE_VT_ADDRESS); 
    
    /********************************/
    /*          Variables           */
    /********************************/
    uint32_t appVersion;
    uint32_t* pubKeyLoc = NULL;
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
    Cy_SysEnableCM4(CY_CORTEX_M4_APPL_ADDR); 

    /********************************/
    /*      SI function tests       */
    /********************************/
    /* Verify SI app version */
    appVersion = Cy_SI_GetVersion();
    if(appVersion == CY_SI_APP_VERSION)
    {
        /* Verify the location of the public key */
        pubKeyLoc = Cy_SI_GetSecureKeyAddr(CY_SI_KEYADDR_PUBLIC_KEY);
    }

    /***************************************/
    /*   SysCalls test using Flash driver  */
    /***************************************/
    sStatus = Cy_Flash_WriteRow(TEST_FLASHROW_ADDR, outPtr);
    if(sStatus == CY_FLASH_DRV_SUCCESS)
    {
        sStatus = Cy_Flash_CalculateHash((uint32_t *)TEST_FLASHROW_ADDR, 4UL, outPtr);
    }
    if(sStatus == CY_FLASH_DRV_SUCCESS)
    {
        sStatus = Cy_Flash_RowChecksum (TEST_FLASHROW_ADDR, outPtr);
    }
    
    /****************************************/
    /*    Crypto test using Crypto driver   */
    /****************************************/
    cStatus = Cy_Crypto_Init(&cryptoConfig, &client_cryptoContext);
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
        if(pubKeyLoc == (uint32_t *)(SFLASH->PUBLIC_KEY))
        {
            Cy_GPIO_Pin_FastInit(P0_3_PORT, P0_3_NUM, CY_GPIO_DM_STRONG, 0UL, HSIOM_SEL_GPIO);
    
            while(1)
            { 
                Cy_GPIO_Inv(P0_3_PORT, P0_3_NUM); 
                CyDelay(1000UL);
            }
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

/* [] END OF FILE */
