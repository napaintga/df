/***************************************************************************//**
* \file cy_si_efuse.c
* \version 1.10
*
* \brief
* EFuse configuration for secure image
*
********************************************************************************
* \copyright
* Copyright 2017-2018, Cypress Semiconductor Corporation. All rights reserved.
* You may use this file only in accordance with the license, terms, conditions,
* disclaimers, and limitations in the end user license agreement accompanying
* the software package with which this file was provided.
*******************************************************************************/

#include "cy_si_efuse.h"

#if defined(__cplusplus)
extern "C" {
#endif

#if (CY_SI_EFUSE_AVAILABLE == 1)
    
/* EFuse configuration */
CY_SECTION(".cy_efuse") __USED const cy_stc_efuse_data_t cy_efuseData = 
{
    .RESERVED = CY_SI_EFUSE_RESERVED0,                         /* Reserved bits ignored */
    .DEAD_ACCESS_RESTRICT0 =
    {
        .CM0_DISABLE = CY_SI_EFUSE_STATE_SET,                  /* Disable CM0+ access port */
        .CM4_DISABLE = CY_SI_EFUSE_STATE_SET,                  /* Disable CM4 access port */
        .SYS_DISABLE = CY_SI_EFUSE_STATE_SET,                  /* Disable System access port */
        .SYS_AP_MPU_ENABLE = CY_SI_EFUSE_STATE_SET,            /* Enable the system access port MPU */
        .SFLASH_ALLOWED = CY_SI_EFUSE_SFLASH_ALLOWED_ENTIRE,   /* SYS AP MPU protection of SFlash */
        .MMIO_ALLOWED = CY_SI_EFUSE_MMIO_ALLOWED_ENTIRE,       /* SYS AP MPU protection of MMIO */
    },
    .DEAD_ACCESS_RESTRICT1 =
    {
        .FLASH_ALLOWED = CY_SI_EFUSE_FLASH_ALLOWED_ENTIRE,     /* SYS AP MPU protection of Flash */
        .SRAM_ALLOWED = CY_SI_EFUSE_SRAM_ALLOWED_ENTIRE,       /* SYS AP MPU protection of SRAM */
        .SMIF_XIP_ALLOWED = CY_SI_EFUSE_SMIF_XIP_ALLOWED_ENTIRE, /* SYS AP MPU protection of SMIF XIP */
        .DIRECT_EXECUTE_DISABLE = CY_SI_EFUSE_STATE_SET        /* Disable "direct execute" system call */
    },
    .SECURE_ACCESS_RESTRICT0 = 
    {
        .CM0_DISABLE = CY_SI_EFUSE_STATE_SET,                  /* Disable CM0+ access port */
        .CM4_DISABLE = CY_SI_EFUSE_STATE_SET,                  /* Disable CM4 access port */
        .SYS_DISABLE = CY_SI_EFUSE_STATE_SET,                  /* Disable System access port */
        .SYS_AP_MPU_ENABLE = CY_SI_EFUSE_STATE_SET,            /* Enable the system access port MPU */
        .SFLASH_ALLOWED = CY_SI_EFUSE_SFLASH_ALLOWED_ENTIRE,   /* SYS AP MPU protection of SFlash */
        .MMIO_ALLOWED = CY_SI_EFUSE_MMIO_ALLOWED_ENTIRE,       /* SYS AP MPU protection of MMIO */
    },
    .SECURE_ACCESS_RESTRICT1 = 
    {
        .FLASH_ALLOWED = CY_SI_EFUSE_FLASH_ALLOWED_ENTIRE,     /* SYS AP MPU protection of Flash */
        .SRAM_ALLOWED = CY_SI_EFUSE_SRAM_ALLOWED_ENTIRE,       /* SYS AP MPU protection of SRAM */
        .SMIF_XIP_ALLOWED = CY_SI_EFUSE_SMIF_XIP_ALLOWED_ENTIRE, /* SYS AP MPU protection of SMIF XIP */
        .DIRECT_EXECUTE_DISABLE = CY_SI_EFUSE_STATE_SET        /* Disable "direct execute" system call */
    },
    .LIFECYCLE_STAGE =
    {
        .NORMAL = CY_SI_EFUSE_STATE_IGNORE,            /* Normal lifecycle already set - ignore */
        .SECURE_WITH_DEBUG = CY_SI_EFUSE_STATE_SET,    /* Transition to "Secure with Debug" lifecycle */
        .SECURE = CY_SI_EFUSE_STATE_IGNORE,            /* Cannot be at both "Secure" and "Secure with Debug" - ignore */
        .RMA = CY_SI_EFUSE_STATE_IGNORE,               /* Cypress use only - ignore */
        .RESERVED = CY_SI_EFUSE_LIFECYCLE_RESERVED0    /* Reserved bits ignored */
    },
    .RESERVED1 = CY_SI_EFUSE_RESERVED1,                /* Reserved bits ignored */
    .CUSTOMER_DATA = 
    {
        CY_SI_EFUSE_CUSTOMER_IGNORE512                 /* All user EFUSE data ignored */
    }
};

#endif

#if defined(__cplusplus)
}
#endif

/* [] END OF FILE */
