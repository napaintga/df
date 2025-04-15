/***************************************************************************//**
* \file cy_si_config.c
* \version 1.10
*
* \brief
* Configuration structures, globals and routines for Secure Image.
*
********************************************************************************
* \copyright
* Copyright 2017-2018, Cypress Semiconductor Corporation. All rights reserved.
* You may use this file only in accordance with the license, terms, conditions,
* disclaimers, and limitations in the end user license agreement accompanying
* the software package with which this file was provided.
*******************************************************************************/

#include "cy_si_config.h"

#if defined(__cplusplus)
extern "C" {
#endif

cy_stc_crypto_server_context_t cy_si_cryptoServerContext;   /**< Context memory for the Crypto server */


/***************************************
*         Memory Blacklists
***************************************/

/** RAM Blacklist: [beginAddr][endAddr][accessAttr] */
const uint32_t CyRamBlacklist[CY_SI_MEM_RAM_BLACKLIST_SIZE][CY_SI_MEM_ELEM_SIZE] = {
    {CY_SI_SECURE_RAM_BEGIN, CY_SI_SECURE_RAM_END, CY_SI_MEM_DENIED}
    /* Expand as necessary */
};

/** Flash Blacklist: [beginAddr][endAddr][accessAttr] */
const uint32_t CyFlashBlacklist[CY_SI_MEM_FLASH_BLACKLIST_SIZE][CY_SI_MEM_ELEM_SIZE] = {
    {CY_SI_SECURE_FLASH_BEGIN,  CY_SI_SECURE_FLASH_END,          CY_SI_MEM_DENIED},
    {CY_SI_USERAPP_FLASH_BEGIN, CY_SI_USERAPP_FLASH_END,         CY_SI_MEM_R},
    {CY_SFLASH_BASE,            CY_SFLASH_BASE + CY_SFLASH_SIZE, CY_SI_MEM_DENIED},
    {CY_ROM_BASE,               CY_ROM_BASE + CY_ROM_SIZE,       CY_SI_MEM_DENIED}
    /* Expand as necessary */
};


/***************************************
*         SMPU configurations 
***************************************/

/** Slave SMPU config for SFlash region */
static const cy_stc_smpu_cfg_t sflash_prot_cfg_s = {
    .address = (uint32_t *)SFLASH_BASE,
    .regionSize = CY_PROT_SIZE_32KB,
    .subregions = (uint8_t)(CY_PROT_SUBREGION_DIS0), /* Needed for SysLib driver. Allow up to 0x16000FFF */
    .userPermission = CY_PROT_PERM_DISABLED,
    .privPermission = CY_PROT_PERM_DISABLED,
    .secure = true,
    .pcMatch = false,
    .pcMask = (uint16_t)0u /* Only allow PC=0 */
};

/** Slave SMPU config for User-defined secure key region */
static const cy_stc_smpu_cfg_t flashKey_prot_cfg_s = {
    .address = (uint32_t *)&CySecureKeyStorage, /* Note: Alignment needs to match .regionSize */
    .regionSize = CY_PROT_SIZE_1KB, /* 256 Bytes x 4 */
    .subregions = 0u,
    .userPermission = CY_PROT_PERM_DISABLED,
    .privPermission = CY_PROT_PERM_DISABLED,
    .secure = true,
    .pcMatch = false,
    .pcMask = (uint16_t)0u /* Only allow PC=0 */
};

/** Slave SMPU config for secure SRAM region */
static const cy_stc_smpu_cfg_t secureRam_prot_cfg_s = {
    .address = (uint32_t *)CY_SI_SECURE_RAM_BEGIN, /* Note: Alignment needs to match .regionSize */
    .regionSize = CY_PROT_SIZE_8KB, /* Update based on the actual size of secure RAM */
    .subregions = 0u, /* Update based on the actual size of secure RAM */
    .userPermission = CY_PROT_PERM_DISABLED,
    .privPermission = CY_PROT_PERM_DISABLED,
    .secure = true,
    .pcMatch = false,
    .pcMask = (uint16_t)0u /* Only allow PC=0 */
};

/** Common Master SMPU config for all SMPUs */
static const cy_stc_smpu_cfg_t smpu_prot_cfg_m = {
    .userPermission = CY_PROT_PERM_R, /* Allow all to have read access (PC=0 has write access) */
    .privPermission = CY_PROT_PERM_R, /* Allow all to have read access (PC=0 has write access) */
    .secure = false,
    .pcMatch = false,
    .pcMask = (uint16_t)(CY_SI_ALL_PC_MASK & ~CY_PROT_MPU_PC_LIMIT_MASK)
};

/***************************************
*         PROG PPU configurations
***************************************/

/** Common master config for all PROG PPUs */
static const cy_stc_ppu_prog_cfg_t prog_prot_cfg_m = {
    .userPermission = CY_PROT_PERM_R, /* Allow all to have read access (PC=0 has write access) */
    .privPermission = CY_PROT_PERM_R, /* Allow all to have read access (PC=0 has write access) */
    .secure = false,
    .pcMatch = false,
    .pcMask = (uint16_t)(CY_SI_ALL_PC_MASK & ~CY_PROT_MPU_PC_LIMIT_MASK)
};

/***************************************
*         PPU configurations  
***************************************/

/** Slave (SL) PPU config for Crypto MMIO region */
static const cy_stc_ppu_sl_cfg_t crypto_prot_cfg_s = {
    .userPermission = CY_PROT_PERM_DISABLED,
    .privPermission = CY_PROT_PERM_DISABLED,
    .secure = true,
    .pcMatch = false,
    .pcMask = (uint16_t)0u /* Only allow PC=0 */
};

/** Slave (RG) PPU config for MS_CTL MMIO region */
static const cy_stc_ppu_rg_cfg_t msCtl_prot_cfg_s = {
    .userPermission = CY_PROT_PERM_DISABLED,
    .privPermission = CY_PROT_PERM_DISABLED,
    .secure = true,
    .pcMatch = false,
    .pcMask = (uint16_t)0u /* Only allow PC=0 */
};

/** Common master config for all Group PPUs */
static const cy_stc_ppu_gr_cfg_t gr_prot_cfg_m = {
    .userPermission = CY_PROT_PERM_R, /* Allow all to have read access (PC=0 has write access) */
    .privPermission = CY_PROT_PERM_R, /* Allow all to have read access (PC=0 has write access) */
    .secure = false,
    .pcMatch = false,
    .pcMask = (uint16_t)(CY_SI_ALL_PC_MASK & ~CY_PROT_MPU_PC_LIMIT_MASK)
};

/** Common master config for all Region PPUs */
static const cy_stc_ppu_rg_cfg_t rg_prot_cfg_m = {
    .userPermission = CY_PROT_PERM_R, /* Allow all to have read access (PC=0 has write access) */
    .privPermission = CY_PROT_PERM_R, /* Allow all to have read access (PC=0 has write access) */
    .secure = false,
    .pcMatch = false,
    .pcMask = (uint16_t)(CY_SI_ALL_PC_MASK & ~CY_PROT_MPU_PC_LIMIT_MASK)
};

/** Common master config for all Slave PPUs */
static const cy_stc_ppu_sl_cfg_t sl_prot_cfg_m = {
    .userPermission = CY_PROT_PERM_R, /* Allow all to have read access (PC=0 has write access) */
    .privPermission = CY_PROT_PERM_R, /* Allow all to have read access (PC=0 has write access) */
    .secure = false,
    .pcMatch = false,
    .pcMask = (uint16_t)(CY_SI_ALL_PC_MASK & ~CY_PROT_MPU_PC_LIMIT_MASK)
};

/***************************************
*         IPC PPU configurations
***************************************/

/** Slave (RG) PPU config for IPC_INTR_STRUCT MMIO region */
static const cy_stc_ppu_rg_cfg_t ipc_intr_prot_cfg_s = {
    .userPermission = CY_PROT_PERM_RW,
    .privPermission = CY_PROT_PERM_RW,
    .secure = false,
    .pcMatch = false,
    .pcMask = (uint16_t)(CY_PROT_PCMASK1) /* Only allow PC=0 and PC=1 */
};

/** Master (RG) PPU config for IPC_INTR_STRUCT MMIO region */
static const cy_stc_ppu_rg_cfg_t ipc_intr_prot_cfg_m = {
    .userPermission = CY_PROT_PERM_R, /* Allow all to have read access (PC=0 has write access) */
    .privPermission = CY_PROT_PERM_R, /* Allow all to have read access (PC=0 has write access) */
    .secure = false,
    .pcMatch = false,
    .pcMask = (uint16_t)(CY_SI_ALL_PC_MASK & ~CY_PROT_MPU_PC_LIMIT_MASK)
};

/** Slave (RG) PPU config for IPC_STRUCT MMIO region */
static const cy_stc_ppu_rg_cfg_t ipc_prot_cfg_s = {
    .userPermission = CY_PROT_PERM_RW, 
    .privPermission = CY_PROT_PERM_RW,
    .secure = false,
    .pcMatch = false,
    .pcMask = (uint16_t)(CY_PROT_PCMASK1) /* Only allow PC=0 and PC=1 */
};

/** Master (RG) PPU config for IPC_STRUCT MMIO region */
static const cy_stc_ppu_rg_cfg_t ipc_prot_cfg_m = {
    .userPermission = CY_PROT_PERM_R, /* Allow all to have read access (PC=0 has write access) */
    .privPermission = CY_PROT_PERM_R, /* Allow all to have read access (PC=0 has write access) */
    .secure = false,
    .pcMatch = false,
    .pcMask = (uint16_t)(CY_SI_ALL_PC_MASK & ~CY_PROT_MPU_PC_LIMIT_MASK)
};


/*******************************************************************************
* Function Name: Cy_SI_ConfigProt
****************************************************************************//**
*
* \brief Initialize the protection units for the secure image.
*
* This function configures the bus master capabilities and initializes the 
* protection units for the secure image. It enables them and locks out the
* configurations from being reconfigured by PC!=0 bus masters. All invalid
* accesses will generate faults. 
*
* Bus master Configuration:
* - CM0+   : Only allow PC=0       
* - Crypto : Only allow PC=0
* - DW0    : Ignored - DW0 inherits the client's config
* - DW1    : Ignored - DW1 inherits the client's config
* - CM4    : Allow all PC except PC=0
* - TC     : Allow all PC except PC=0
* 
* The function also provides IPC protection unit configurations as reference 
* to be used by the Client software; The IPC_STRUCT and IPC_STRUCT registers 
* need to be shared between us masters and it is the client's responsibility 
* to configure them. The data to be passed through the IPC_STRUCT.DATA register
* also should be protected using an SMPU.
*
* \note The protection unit evaluation is prioritized in a descending order. 
* i.e. Higher index is evaluated before a lower index protection unit.
*
* \note All master protection units are enabled and configured to only allow
* PC=0 bus masters to reconfigure their respective slave protection units. This
* is to prevent unwanted configuration of higher priority protection units,
* which can override the lower priority protection unit settings. Refer to 
* the TRM for more information on the protection unit priorities.
*
* \return 
* Status of protection unit configuration.
*
*******************************************************************************/
CY_NOINLINE cy_en_si_status_t Cy_SI_ConfigProt(void)
{
    cy_en_prot_status_t status;

    /* Configure Bus masters */
    status = Cy_Prot_ConfigBusMaster(CPUSS_MS_ID_CM0, true, true, 0UL); /* Only allow PC=0 */
    status = (status == CY_PROT_SUCCESS) ? Cy_Prot_ConfigBusMaster(CPUSS_MS_ID_CRYPTO, true, false, 0UL) : status;
    status = (status == CY_PROT_SUCCESS) ? Cy_Prot_ConfigBusMaster(CPUSS_MS_ID_CM4, true, false, (CY_SI_ALL_PC_MASK & ~CY_PROT_MS14_PC_LIMIT_MASK)) : status;
    status = (status == CY_PROT_SUCCESS) ? Cy_Prot_ConfigBusMaster(CPUSS_MS_ID_TC, true, false, (CY_SI_ALL_PC_MASK & ~CY_PROT_MS15_PC_LIMIT_MASK)) : status;

    /* Transition the non-secure bus master PC values to 1 */
    status = (status == CY_PROT_SUCCESS) ? Cy_Prot_SetActivePC(CPUSS_MS_ID_CM4, 1UL) : status;
    status = (status == CY_PROT_SUCCESS) ? Cy_Prot_SetActivePC(CPUSS_MS_ID_TC, 1UL) : status;
    
    /************************************************************
    * Configure the slave structs of the protection units.
    ************************************************************/
    
    /************************************************************
    * Note: The following slave structs are reserved and used by
    * SROM and FLASHBOOT to protect key resources. The restrictions
    * are configured to only allow PC=0 bus masters to have access.
    *
    * SMPU 15: Read/write restriction for ROM private stack.
    * SMPU 14: Read/write restriction for ROM region.
    *
    * PROG PPU 15: Write restriction for CPUSS AP_CTL, PROTECTION,
    *              CM0_NMI_CTL, DP_CTL and MBIST_CTL registers.
    * PROG PPU 14: Read/write restriction for CPUSS WOUNDING and
    *              CM0_PC0_HANDLER registers.
    * PROG PPU 13: Write restriction for FlashC FM_CTL.BOOKMARK
    *              register.
    * PROG PPU 12: Read/write restriction for EFUSE region 
    *              (excluding CUSTOMER_DATA).
    * PROG PPU 11: Write restriction for IPC 0, 1 and 2 during
    *              system calls.
    * PROG PPU 10: Read/write restriction for Crypto during
    *              system calls that use crypto operations.
    * PROG PPU 9:  Read/write restriction for FM_CTL registers.
    ************************************************************/
        
    /* SMPU 13 - SFlash */
    status = (status == CY_PROT_SUCCESS) ? Cy_Prot_ConfigSmpuSlaveStruct(PROT_SMPU_SMPU_STRUCT13, &sflash_prot_cfg_s) : status;
    status = (status == CY_PROT_SUCCESS) ? Cy_Prot_EnableSmpuSlaveStruct(PROT_SMPU_SMPU_STRUCT13) : status;

    /* SMPU 12 - Flash key storage */
    status = (status == CY_PROT_SUCCESS) ? Cy_Prot_ConfigSmpuSlaveStruct(PROT_SMPU_SMPU_STRUCT12, &flashKey_prot_cfg_s) : status;
    status = (status == CY_PROT_SUCCESS) ? Cy_Prot_EnableSmpuSlaveStruct(PROT_SMPU_SMPU_STRUCT12) : status;
    
    /* SMPU 11 - Secure RAM */
    status = (status == CY_PROT_SUCCESS) ? Cy_Prot_ConfigSmpuSlaveStruct(PROT_SMPU_SMPU_STRUCT11, &secureRam_prot_cfg_s) : status;
    status = (status == CY_PROT_SUCCESS) ? Cy_Prot_EnableSmpuSlaveStruct(PROT_SMPU_SMPU_STRUCT11) : status;
    
    /* PPU RG - MS_CTL (Bus master control in SMPU) */
    status = (status == CY_PROT_SUCCESS) ? Cy_Prot_ConfigPpuFixedRgSlaveStruct(PERI_GR_PPU_RG_SMPU, &msCtl_prot_cfg_s) : status;
    status = (status == CY_PROT_SUCCESS) ? Cy_Prot_ConfigPpuFixedRgMasterStruct(PERI_GR_PPU_RG_SMPU, &rg_prot_cfg_m) : status;
    status = (status == CY_PROT_SUCCESS) ? Cy_Prot_EnablePpuFixedRgSlaveStruct(PERI_GR_PPU_RG_SMPU) : status;
    status = (status == CY_PROT_SUCCESS) ? Cy_Prot_EnablePpuFixedRgMasterStruct(PERI_GR_PPU_RG_SMPU) : status;

    /* PPU SL - Crypto */
    status = (status == CY_PROT_SUCCESS) ? Cy_Prot_ConfigPpuFixedSlSlaveStruct(PERI_GR_PPU_SL_CRYPTO, &crypto_prot_cfg_s) : status;
    status = (status == CY_PROT_SUCCESS) ? Cy_Prot_ConfigPpuFixedSlMasterStruct(PERI_GR_PPU_SL_CRYPTO, &sl_prot_cfg_m) : status;
    status = (status == CY_PROT_SUCCESS) ? Cy_Prot_EnablePpuFixedSlSlaveStruct(PERI_GR_PPU_SL_CRYPTO) : status;
    status = (status == CY_PROT_SUCCESS) ? Cy_Prot_EnablePpuFixedSlMasterStruct(PERI_GR_PPU_SL_CRYPTO) : status;
    
    /************************************************************
    * Take control of the master structs of protection units
    * that can possibly be used to override the protection 
    * settings or initiate a denial-of-service attack.
    ************************************************************/

    for(uint8_t idx = 0u; idx < 16u; idx++)
    {
        /* Master structs for SMPU 15-0 */
        status = (status == CY_PROT_SUCCESS) ? Cy_Prot_ConfigSmpuMasterStruct((PROT_SMPU_SMPU_STRUCT_Type*) &PROT->SMPU.SMPU_STRUCT[idx], &smpu_prot_cfg_m) : status;
        status = (status == CY_PROT_SUCCESS) ? Cy_Prot_EnableSmpuMasterStruct((PROT_SMPU_SMPU_STRUCT_Type*) &PROT->SMPU.SMPU_STRUCT[idx]) : status;
        
        /* Master structs for PROG PPU 15-0 */
        status = (status == CY_PROT_SUCCESS) ? Cy_Prot_ConfigPpuProgMasterStruct((PERI_PPU_PR_Type*) &PERI->PPU_PR[idx], &prog_prot_cfg_m) : status;
        status = (status == CY_PROT_SUCCESS) ? Cy_Prot_EnablePpuProgMasterStruct((PERI_PPU_PR_Type*) &PERI->PPU_PR[idx]) : status;
        
        if(status != CY_PROT_SUCCESS)
        {
            break;
        }
    }
    
    /* PPU GR - Master structs for MMIO Groups */
    status = (status == CY_PROT_SUCCESS) ? Cy_Prot_ConfigPpuFixedGrMasterStruct(PERI_PPU_GR_MMIO0, &gr_prot_cfg_m) : status;
    status = (status == CY_PROT_SUCCESS) ? Cy_Prot_EnablePpuFixedGrMasterStruct(PERI_PPU_GR_MMIO0) : status;
    status = (status == CY_PROT_SUCCESS) ? Cy_Prot_ConfigPpuFixedGrMasterStruct(PERI_PPU_GR_MMIO1, &gr_prot_cfg_m) : status;
    status = (status == CY_PROT_SUCCESS) ? Cy_Prot_EnablePpuFixedGrMasterStruct(PERI_PPU_GR_MMIO1) : status;
    status = (status == CY_PROT_SUCCESS) ? Cy_Prot_ConfigPpuFixedGrMasterStruct(PERI_PPU_GR_MMIO2, &gr_prot_cfg_m) : status;
    status = (status == CY_PROT_SUCCESS) ? Cy_Prot_EnablePpuFixedGrMasterStruct(PERI_PPU_GR_MMIO2) : status;
    status = (status == CY_PROT_SUCCESS) ? Cy_Prot_ConfigPpuFixedGrMasterStruct(PERI_PPU_GR_MMIO3, &gr_prot_cfg_m) : status;
    status = (status == CY_PROT_SUCCESS) ? Cy_Prot_EnablePpuFixedGrMasterStruct(PERI_PPU_GR_MMIO3) : status;
    status = (status == CY_PROT_SUCCESS) ? Cy_Prot_ConfigPpuFixedGrMasterStruct(PERI_PPU_GR_MMIO4, &gr_prot_cfg_m) : status;
    status = (status == CY_PROT_SUCCESS) ? Cy_Prot_EnablePpuFixedGrMasterStruct(PERI_PPU_GR_MMIO4) : status;
    status = (status == CY_PROT_SUCCESS) ? Cy_Prot_ConfigPpuFixedGrMasterStruct(PERI_PPU_GR_MMIO6, &gr_prot_cfg_m) : status;
    status = (status == CY_PROT_SUCCESS) ? Cy_Prot_EnablePpuFixedGrMasterStruct(PERI_PPU_GR_MMIO6) : status;
    status = (status == CY_PROT_SUCCESS) ? Cy_Prot_ConfigPpuFixedGrMasterStruct(PERI_PPU_GR_MMIO9, &gr_prot_cfg_m) : status;
    status = (status == CY_PROT_SUCCESS) ? Cy_Prot_EnablePpuFixedGrMasterStruct(PERI_PPU_GR_MMIO9) : status;
    status = (status == CY_PROT_SUCCESS) ? Cy_Prot_ConfigPpuFixedGrMasterStruct(PERI_PPU_GR_MMIO10, &gr_prot_cfg_m) : status;
    status = (status == CY_PROT_SUCCESS) ? Cy_Prot_EnablePpuFixedGrMasterStruct(PERI_PPU_GR_MMIO10) : status;
    
    /* PPU RG - Master structs for Bus master PC settings */
    status = (status == CY_PROT_SUCCESS) ? Cy_Prot_ConfigPpuFixedRgMasterStruct(PERI_GR_PPU_RG_MPU_CM0P, &rg_prot_cfg_m) : status;
    status = (status == CY_PROT_SUCCESS) ? Cy_Prot_EnablePpuFixedRgMasterStruct(PERI_GR_PPU_RG_MPU_CM0P) : status;
    status = (status == CY_PROT_SUCCESS) ? Cy_Prot_ConfigPpuFixedRgMasterStruct(PERI_GR_PPU_RG_MPU_CRYPTO, &rg_prot_cfg_m) : status;
    status = (status == CY_PROT_SUCCESS) ? Cy_Prot_EnablePpuFixedRgMasterStruct(PERI_GR_PPU_RG_MPU_CRYPTO) : status;
    status = (status == CY_PROT_SUCCESS) ? Cy_Prot_ConfigPpuFixedRgMasterStruct(PERI_GR_PPU_RG_MPU_CM4, &rg_prot_cfg_m) : status;
    status = (status == CY_PROT_SUCCESS) ? Cy_Prot_EnablePpuFixedRgMasterStruct(PERI_GR_PPU_RG_MPU_CM4) : status;
    status = (status == CY_PROT_SUCCESS) ? Cy_Prot_ConfigPpuFixedRgMasterStruct(PERI_GR_PPU_RG_MPU_TC, &rg_prot_cfg_m) : status;
    status = (status == CY_PROT_SUCCESS) ? Cy_Prot_EnablePpuFixedRgMasterStruct(PERI_GR_PPU_RG_MPU_TC) : status;
    
    /* PPU SL - Master structs for PERI Groups */
    status = (status == CY_PROT_SUCCESS) ? Cy_Prot_ConfigPpuFixedSlMasterStruct(PERI_GR_PPU_SL_PERI_GR1, &sl_prot_cfg_m) : status;
    status = (status == CY_PROT_SUCCESS) ? Cy_Prot_EnablePpuFixedSlMasterStruct(PERI_GR_PPU_SL_PERI_GR1) : status;
    status = (status == CY_PROT_SUCCESS) ? Cy_Prot_ConfigPpuFixedSlMasterStruct(PERI_GR_PPU_SL_PERI_GR2, &sl_prot_cfg_m) : status;
    status = (status == CY_PROT_SUCCESS) ? Cy_Prot_EnablePpuFixedSlMasterStruct(PERI_GR_PPU_SL_PERI_GR2) : status;
    status = (status == CY_PROT_SUCCESS) ? Cy_Prot_ConfigPpuFixedSlMasterStruct(PERI_GR_PPU_SL_PERI_GR3, &sl_prot_cfg_m) : status;
    status = (status == CY_PROT_SUCCESS) ? Cy_Prot_EnablePpuFixedSlMasterStruct(PERI_GR_PPU_SL_PERI_GR3) : status;
    status = (status == CY_PROT_SUCCESS) ? Cy_Prot_ConfigPpuFixedSlMasterStruct(PERI_GR_PPU_SL_PERI_GR4, &sl_prot_cfg_m) : status;
    status = (status == CY_PROT_SUCCESS) ? Cy_Prot_EnablePpuFixedSlMasterStruct(PERI_GR_PPU_SL_PERI_GR4) : status;
    status = (status == CY_PROT_SUCCESS) ? Cy_Prot_ConfigPpuFixedSlMasterStruct(PERI_GR_PPU_SL_PERI_GR6, &sl_prot_cfg_m) : status;
    status = (status == CY_PROT_SUCCESS) ? Cy_Prot_EnablePpuFixedSlMasterStruct(PERI_GR_PPU_SL_PERI_GR6) : status;
    status = (status == CY_PROT_SUCCESS) ? Cy_Prot_ConfigPpuFixedSlMasterStruct(PERI_GR_PPU_SL_PERI_GR9, &sl_prot_cfg_m) : status;
    status = (status == CY_PROT_SUCCESS) ? Cy_Prot_EnablePpuFixedSlMasterStruct(PERI_GR_PPU_SL_PERI_GR9) : status;
    status = (status == CY_PROT_SUCCESS) ? Cy_Prot_ConfigPpuFixedSlMasterStruct(PERI_GR_PPU_SL_PERI_GR10, &sl_prot_cfg_m) : status;
    status = (status == CY_PROT_SUCCESS) ? Cy_Prot_EnablePpuFixedSlMasterStruct(PERI_GR_PPU_SL_PERI_GR10) : status;
    
    /************************************************************
    * Sample IPC protection unit configurations are provided
    * below as reference. These should be used by the Client 
    * software for secure IPC communication. E.g. the structures
    * can be configured to allow only privileged software with
    * PC=2 to write to the IPC registers.
    * 
    * The data pointer passed to the IPC_STRUCT.DATA register 
    * should also be protected using an SMPU. E.g. Use an SMPU
    * to protect the region of memory passed to the IPC such that
    * only the trusted client-side application and the secure
    * image task have access to it. Alternatively, build a
    * mechanism of enabling the SMPU at Client-side at PC=0 and
    * release/disable the SMPU in the secure task when the 
    * operation completes.
    ************************************************************/
    
    /* PPU RG - Syscall (IPC0, IPC1, IPC2) */
    status = (status == CY_PROT_SUCCESS) ? Cy_Prot_ConfigPpuFixedRgSlaveStruct(PERI_GR_PPU_RG_IPC_STRUCT0, &ipc_prot_cfg_s) : status;
    status = (status == CY_PROT_SUCCESS) ? Cy_Prot_ConfigPpuFixedRgMasterStruct(PERI_GR_PPU_RG_IPC_STRUCT0, &ipc_prot_cfg_m) : status;
    status = (status == CY_PROT_SUCCESS) ? Cy_Prot_EnablePpuFixedRgSlaveStruct(PERI_GR_PPU_RG_IPC_STRUCT0) : status;
    status = (status == CY_PROT_SUCCESS) ? Cy_Prot_EnablePpuFixedRgMasterStruct(PERI_GR_PPU_RG_IPC_STRUCT0) : status;
    status = (status == CY_PROT_SUCCESS) ? Cy_Prot_ConfigPpuFixedRgSlaveStruct(PERI_GR_PPU_RG_IPC_STRUCT1, &ipc_prot_cfg_s) : status;
    status = (status == CY_PROT_SUCCESS) ? Cy_Prot_ConfigPpuFixedRgMasterStruct(PERI_GR_PPU_RG_IPC_STRUCT1, &ipc_prot_cfg_m) : status;
    status = (status == CY_PROT_SUCCESS) ? Cy_Prot_EnablePpuFixedRgSlaveStruct(PERI_GR_PPU_RG_IPC_STRUCT1) : status;
    status = (status == CY_PROT_SUCCESS) ? Cy_Prot_EnablePpuFixedRgMasterStruct(PERI_GR_PPU_RG_IPC_STRUCT1) : status;
    status = (status == CY_PROT_SUCCESS) ? Cy_Prot_ConfigPpuFixedRgSlaveStruct(PERI_GR_PPU_RG_IPC_STRUCT2, &ipc_prot_cfg_s) : status;
    status = (status == CY_PROT_SUCCESS) ? Cy_Prot_ConfigPpuFixedRgMasterStruct(PERI_GR_PPU_RG_IPC_STRUCT2, &ipc_prot_cfg_m) : status;
    status = (status == CY_PROT_SUCCESS) ? Cy_Prot_EnablePpuFixedRgSlaveStruct(PERI_GR_PPU_RG_IPC_STRUCT2) : status;
    status = (status == CY_PROT_SUCCESS) ? Cy_Prot_EnablePpuFixedRgMasterStruct(PERI_GR_PPU_RG_IPC_STRUCT2) : status;
        
    /* PPU RG - Syscall (IPC INTR0) */
    status = (status == CY_PROT_SUCCESS) ? Cy_Prot_ConfigPpuFixedRgSlaveStruct(PERI_GR_PPU_RG_IPC_INTR_STRUCT0, &ipc_intr_prot_cfg_s) : status;
    status = (status == CY_PROT_SUCCESS) ? Cy_Prot_ConfigPpuFixedRgMasterStruct(PERI_GR_PPU_RG_IPC_INTR_STRUCT0, &ipc_intr_prot_cfg_m) : status;
    status = (status == CY_PROT_SUCCESS) ? Cy_Prot_EnablePpuFixedRgSlaveStruct(PERI_GR_PPU_RG_IPC_INTR_STRUCT0) : status;
    status = (status == CY_PROT_SUCCESS) ? Cy_Prot_EnablePpuFixedRgMasterStruct(PERI_GR_PPU_RG_IPC_INTR_STRUCT0) : status;
    
    /* PPU RG - Crypto (IPC3) */
    status = (status == CY_PROT_SUCCESS) ? Cy_Prot_ConfigPpuFixedRgSlaveStruct(PERI_GR_PPU_RG_IPC_STRUCT3, &ipc_prot_cfg_s) : status;
    status = (status == CY_PROT_SUCCESS) ? Cy_Prot_ConfigPpuFixedRgMasterStruct(PERI_GR_PPU_RG_IPC_STRUCT3, &ipc_prot_cfg_m) : status;
    status = (status == CY_PROT_SUCCESS) ? Cy_Prot_EnablePpuFixedRgSlaveStruct(PERI_GR_PPU_RG_IPC_STRUCT3) : status;
    status = (status == CY_PROT_SUCCESS) ? Cy_Prot_EnablePpuFixedRgMasterStruct(PERI_GR_PPU_RG_IPC_STRUCT3) : status;
    
    /* PPU RG - Crypto IPC (INTR1, INTR2) */
    status = (status == CY_PROT_SUCCESS) ? Cy_Prot_ConfigPpuFixedRgSlaveStruct(PERI_GR_PPU_RG_IPC_INTR_STRUCT1, &ipc_intr_prot_cfg_s) : status;
    status = (status == CY_PROT_SUCCESS) ? Cy_Prot_ConfigPpuFixedRgMasterStruct(PERI_GR_PPU_RG_IPC_INTR_STRUCT1, &ipc_intr_prot_cfg_m) : status;
    status = (status == CY_PROT_SUCCESS) ? Cy_Prot_EnablePpuFixedRgSlaveStruct(PERI_GR_PPU_RG_IPC_INTR_STRUCT1) : status;
    status = (status == CY_PROT_SUCCESS) ? Cy_Prot_EnablePpuFixedRgMasterStruct(PERI_GR_PPU_RG_IPC_INTR_STRUCT1) : status;
    status = (status == CY_PROT_SUCCESS) ? Cy_Prot_ConfigPpuFixedRgSlaveStruct(PERI_GR_PPU_RG_IPC_INTR_STRUCT2, &ipc_intr_prot_cfg_s) : status;
    status = (status == CY_PROT_SUCCESS) ? Cy_Prot_ConfigPpuFixedRgMasterStruct(PERI_GR_PPU_RG_IPC_INTR_STRUCT2, &ipc_intr_prot_cfg_m) : status;
    status = (status == CY_PROT_SUCCESS) ? Cy_Prot_EnablePpuFixedRgSlaveStruct(PERI_GR_PPU_RG_IPC_INTR_STRUCT2) : status;
    status = (status == CY_PROT_SUCCESS) ? Cy_Prot_EnablePpuFixedRgMasterStruct(PERI_GR_PPU_RG_IPC_INTR_STRUCT2) : status;
    
    return (cy_en_si_status_t)status;
}


/*******************************************************************************
* Function Name: Cy_SI_ValidateImage
****************************************************************************//**
*
* \brief Validates the user image using SHA-256 and RSA-2048. 
*
* This function uses the same validation scheme used by Flash boot to verify
* the authenticity of the user application. SHA-256 is used to generate the user
* application Hash, which is encrypted using RSA-2048 and a private key to 
* generate the digital signature. The validation scheme uses the RSA-2048 
* public key in SFlash to decrypt the digital signature and compare it with the
* SHA-256 Hash of the application object.
*
* \note This function requires that the user application is constructed in
* Cypress application format. It references a validation routine in SFlash,
* which is designed to only work with RSA.
*
* \note If an error is encountered in Flash boot while operating in NORMAL
* or SECURE_with_DEBUG modes, a debugger can be attached and the SRAM address 
* defined by the CY_SI_FLASHBOOT_ERROR_ADDR macro can be used to determine the
* source of the error.
*
* \return
* Status of the image validation.
*  
*******************************************************************************/
CY_NOINLINE cy_en_si_status_t Cy_SI_ValidateImage(uint32_t *validToc)
{
    cy_en_si_status_t status = CY_SI_SUCCESS;
    cy_stc_crypto_rsa_pub_key_t *publicKey = NULL;
    uint32_t tocState = 0UL;
    uint32_t tocAddr = 0UL;
    uint32_t tocAppFormat = 0UL;
    uint32_t appAddr = 0UL;
    uint32_t appSize;
    uint32_t digSigAddr;
    
    /* Check the validity of TOC2 and RTOC2 */
    tocState = Cy_FB_ValidateToc((uint32_t)&SFLASH->TOC2_OBJECT_SIZE);
    if(tocState == CY_SI_TOC_EMPTY)
    {
        status = CY_SI_EMPTY_TOC;
        *validToc = CY_SI_TOC_EMPTY;
    }
    else if(tocState == CY_SI_TOC_INVALID)
    {
        status = CY_SI_INVALID_TOC;
        *validToc = CY_SI_TOC_INVALID;
    }
    else
    {
        /* Choose either TOC2 or RTOC2 data */
        if(tocState == (uint32_t)&SFLASH->TOC2_OBJECT_SIZE)
        {
            publicKey = (cy_stc_crypto_rsa_pub_key_t *)(SFLASH->TOC2_SIGNATURE_VERIF_KEY + 8u);
            tocAddr = (uint32_t)&SFLASH->TOC2_OBJECT_SIZE;
            tocAppFormat = SFLASH->TOC2_SECOND_USER_APP_FORMAT;
            appAddr = SFLASH->TOC2_SECOND_USER_APP_ADDR;
            *validToc = CY_SI_VALID_TOC2;
        }
        else 
        {
            publicKey = (cy_stc_crypto_rsa_pub_key_t *)(SFLASH->RTOC2_SIGNATURE_VERIF_KEY + 8u);
            tocAddr = (uint32_t)&SFLASH->RTOC2_OBJECT_SIZE;
            tocAppFormat = SFLASH->RTOC2_SECOND_USER_APP_FORMAT;
            appAddr = SFLASH->RTOC2_SECOND_USER_APP_ADDR;
            *validToc = CY_SI_VALID_RTOC2;
        }
    }
    
    /* Check that the application is in Cypress format */
    if(status == CY_SI_SUCCESS)
    {
        if (tocAppFormat != CY_SI_APP_FORMAT_CYPRESS)
        {
            status = CY_SI_INVALID_FORMAT;
        }
    }
    
    /* Check the validity of the key format */
    if(status == CY_SI_SUCCESS)
    {
        if (Cy_FB_IsValidKey(tocAddr, publicKey) == 0UL)
        {
            status = CY_SI_INVALID_KEY;
        }
    }
        
    /* Verify the user application */
    if(status == CY_SI_SUCCESS)
    {
        appSize = ((cy_stc_user_appheader_t *)appAddr)->objSize;
        digSigAddr = appAddr + appSize;
        if(Cy_FB_VerifyApplication(appAddr, appSize, digSigAddr, publicKey) == 0UL)
        {
            status = CY_SI_INVALID_IMAGE;
        }
    }
    
    return status;
}


/*******************************************************************************
* Function Name: Cy_SI_ConfigCrypto
****************************************************************************//**
*
* \brief Configures the Crypto server.
*
* This function initializes and starts the Crypto server, which accepts and
* executes all valid Crypto operation requests. These routines are executed
* by the Crypto IP with the aid of CM0+ in the secure domain of the secure
* image.
*
* \note The entire Crypto library is included in the secure image by default.
* If only a subset is required by the client application, choose a smaller
* library to reduce the size of the secure image.
*
* \return
* Status of the crypto server configuration
*  
*******************************************************************************/
CY_NOINLINE cy_en_si_status_t Cy_SI_ConfigCrypto(void)
{
    cy_en_si_status_t status;
    cy_en_crypto_status_t serverStatus;
    
    cy_stc_crypto_config_t si_cryptoConfig;
    (void)memcpy(&si_cryptoConfig, &cryptoConfig, sizeof(cryptoConfig));
    
    si_cryptoConfig.userCompleteCallback = NULL; /* Set by user application */
    si_cryptoConfig.userGetDataHandler = &NvicMux2_IRQHandler;
    si_cryptoConfig.userErrorHandler = &NvicMux31_IRQHandler;
        
    /* Clear the mask and interrupt */
    Cy_IPC_Drv_SetInterruptMask(Cy_IPC_Drv_GetIntrBaseAddr(CY_CRYPTO_IPC_INTR_NOTIFY_NUM), 
                                    CY_SI_NO_IRQ, CY_SI_NO_IRQ);
    Cy_IPC_Drv_ClearInterrupt(Cy_IPC_Drv_GetIntrBaseAddr(CY_CRYPTO_IPC_INTR_NOTIFY_NUM), 
                                    CY_SI_ALL_IRQ, CY_SI_ALL_IRQ);
    NVIC_ClearPendingIRQ(CY_CRYPTO_CM0_NOTIFY_INTR_NR);
    
    serverStatus = Cy_Crypto_Server_Start(&si_cryptoConfig, &cy_si_cryptoServerContext);
    
    status = (cy_en_si_status_t)serverStatus;
    
    return status;
}


/*******************************************************************************
* Function Name: Cy_SI_ConfigSysCall
****************************************************************************//**
*
* \brief Configures the SysCall interrupt.
*
* SysCall IPC requests are by default configured to generate an NMI on the CM0+
* core. This is serviced by code residing in ROM. In order to apply
* pre-processing to the SysCall request in the secure image, the IPC-NMI
* connection must be broken and the interrupt vector reconfigured.
*
* \return
* Status of the SysCall configuration
*  
*******************************************************************************/
CY_NOINLINE cy_en_si_status_t Cy_SI_ConfigSysCall(void)
{
    cy_en_si_status_t status = CY_SI_SUCCESS;
    
    /* SysCall interrupt configuration */
    cy_stc_sysint_t syscall_irq_cfg = {
        .intrSrc = CY_SI_SYSCALL_INTR_NUMBER,
        .cm0pSrc = cpuss_interrupts_ipc_0_IRQn,
        .intrPriority = CY_SI_SYSCALL_INTR_PRIORITY
    };

    /*  Disconnect the NMI from IPC_INTR0 and then reconfigure the interrupt */
    NVIC_DisableIRQ(syscall_irq_cfg.intrSrc);
    Cy_SysInt_SetIntSourceNMI(unconnected_IRQn);
    (void)Cy_SysInt_Init(&syscall_irq_cfg, &NvicMux0_IRQHandler);
    NVIC_EnableIRQ(syscall_irq_cfg.intrSrc);
    
    /* Initialize the mask */
    Cy_IPC_Drv_SetInterruptMask(Cy_IPC_Drv_GetIntrBaseAddr(CY_SI_SYSCALL_INTR_STRUCT), 
                                    CY_SI_NO_IRQ, CY_SI_SYSCALL_INTR_STRUCT_MASK);

    return status;
}


/*******************************************************************************
* Function Name: Cy_SI_GetSecureKeyAddr
****************************************************************************//**
*
* \brief Get Security Key Address given the index. 
*
* This function returns the address of the key specified by the index number. 
* The pointer can be passed to the Crypto engine, which has access to the Secure 
* Key Storage. Direct access (dereference) to the secure key by a non-secure 
* bus master will result in an access fault.
*
* \param keyIndex
* Index of the security key inside the Secure Key Storage area.
*
* - 0      : Public Key in SFlash
* - 1 to N : Index of the key located in user Flash
*
* \return
* The pointer to the secure key indicated by the keyIndex parameter. If
* invalid key request, returns NULL.
*
*******************************************************************************/
CY_NOINLINE uint32_t* Cy_SI_GetSecureKeyAddr(uint32_t keyIndex)
{
    uint32_t* currentKey = NULL;
    
    if(keyIndex == CY_SI_KEYADDR_PUBLIC_KEY)
    {
        currentKey = (uint32_t *)&(SFLASH->PUBLIC_KEY);
    }
    else
    {
        if (keyIndex <= CY_SI_SECURE_KEY_ARRAY_SIZE)
        {
            currentKey = (uint32_t *)&CySecureKeyStorage[(keyIndex - 1UL)][0];
        }
    }

    return currentKey;
}


/*******************************************************************************
* Function Name: Cy_SI_GetVersion
****************************************************************************//**
*
* \brief Returns the version of the Secure Image. 
*
* \return
* Returns the version of Secure Image; Major[31:24], Minor[23:16].
*
*******************************************************************************/
CY_NOINLINE uint32_t Cy_SI_GetVersion(void)
{
    return (CY_SI_APP_VERSION);
}


/*******************************************************************************
* Function Name: Cy_SI_CheckMemRange
****************************************************************************//**
*
* \brief Checks that the referenced memory is not in the protected regions. 
*
* \param beginAddr
* The beginning address used to access the interested memory. For example,
* the first address of an array.
*
* \param endAddr
* The end address used to access the interested memory. For example, the last
* address of an array.
*
* \param checkType
* The type of access to be evaluated by the function call.
*  - CY_SI_MEM_TYPE_FLASH_DST_MASK: The memory access is intended to be used in
*    writing to a Flash row.
*  - CY_SI_MEM_TYPE_FLASH_SRC_MASK: The memory access is intended to be used in
*    reading from Flash.
*  - CY_SI_MEM_TYPE_RAM_MASK: The memory access is intended for accessing RAM.
*
* \return
* Status of the memory check.
*
*******************************************************************************/
cy_en_si_status_t Cy_SI_CheckMemRange(uint32_t beginAddr, uint32_t endAddr, uint32_t checkType)
{
    cy_en_si_status_t status = CY_SI_SUCCESS;
    uint32_t beginSecureAddr;
    uint32_t endSecureAddr;
    uint32_t accessAttr;
    uint8_t idx;

    /* Parameter check */
    if((checkType & (CY_SI_MEM_TYPE_FLASH_SRC_MASK | CY_SI_MEM_TYPE_FLASH_DST_MASK | CY_SI_MEM_TYPE_RAM_MASK) ) == 0UL)
    {
        status = CY_SI_BAD_PARAM;
    }
    
    if(status == CY_SI_SUCCESS)
    {
        /* Address check for when writing to Flash */
        if((checkType & CY_SI_MEM_TYPE_FLASH_DST_MASK) != 0UL)
        { 
            for(idx = 0u; idx < CY_SI_MEM_FLASH_BLACKLIST_SIZE; idx++)
            {               
                /* Check Flash location */
                beginSecureAddr = CyFlashBlacklist[idx][CY_SI_MEM_ADDR_BEGIN];
                endSecureAddr = CyFlashBlacklist[idx][CY_SI_MEM_ADDR_END];
                accessAttr = CyFlashBlacklist[idx][CY_SI_MEM_ADDR_ACCESS] & CY_SI_MEM_W;
                
                /* Calculate the Flash row boundaries */
                beginSecureAddr = (beginSecureAddr - (beginSecureAddr%CY_SI_SYSCALL_ROWSIZE));
                endSecureAddr = ((endSecureAddr - (endSecureAddr%CY_SI_SYSCALL_ROWSIZE)) + CY_SI_SYSCALL_ROWSIZE);

                 /* Check address range and access attributes */
                if(   (((beginAddr <  beginSecureAddr) && (endAddr   >  endSecureAddr))
                    || ((beginAddr >= beginSecureAddr) && (beginAddr <= endSecureAddr))
                    || ((endAddr   >= beginSecureAddr) && (endAddr   <= endSecureAddr)))
                    && (accessAttr == CY_SI_MEM_DENIED))
                {
                    status = CY_SI_INVALID_FLASH_ACCESS;
                    break;
                }
            }
        }
    }    
    if(status == CY_SI_SUCCESS)
    {
        /* Address check for when reading from Flash */
        if((checkType & CY_SI_MEM_TYPE_FLASH_SRC_MASK) != 0UL)
        {
            for(idx = 0u; idx < CY_SI_MEM_FLASH_BLACKLIST_SIZE; idx++)
            {
                /* Check if the address is in a protected Flash row */
                beginSecureAddr = CyFlashBlacklist[idx][CY_SI_MEM_ADDR_BEGIN];
                endSecureAddr = CyFlashBlacklist[idx][CY_SI_MEM_ADDR_END];
                accessAttr = CyFlashBlacklist[idx][CY_SI_MEM_ADDR_ACCESS] & CY_SI_MEM_R;
            
                /* Check address range and access attributes */
                if(   (((beginAddr <  beginSecureAddr) && (endAddr   >  endSecureAddr))
                    || ((beginAddr >= beginSecureAddr) && (beginAddr <= endSecureAddr))
                    || ((endAddr   >= beginSecureAddr) && (endAddr   <= endSecureAddr)))
                    && (accessAttr == CY_SI_MEM_DENIED))
                {
                    status = CY_SI_INVALID_FLASH_ACCESS;
                    break;
                }
            }
        }
    }
    if(status == CY_SI_SUCCESS)
    {
        /* Address check for when reading/writing SRAM */
        if ((checkType & CY_SI_MEM_TYPE_RAM_MASK) != 0UL)
        {
            /* Check access to user defined secure RAM area */
            for(idx = 0u; idx < CY_SI_MEM_RAM_BLACKLIST_SIZE; idx++) 
            {
                beginSecureAddr = CyRamBlacklist[idx][CY_SI_MEM_ADDR_BEGIN];
                endSecureAddr = CyRamBlacklist[idx][CY_SI_MEM_ADDR_END];
                accessAttr = CyFlashBlacklist[idx][CY_SI_MEM_ADDR_ACCESS] & CY_SI_MEM_RW;
            
                /* Check address range and access attributes */
                if(   (((beginAddr <  beginSecureAddr) && (endAddr   >  endSecureAddr))
                    || ((beginAddr >= beginSecureAddr) && (beginAddr <= endSecureAddr))
                    || ((endAddr   >= beginSecureAddr) && (endAddr   <= endSecureAddr)))
                    && (accessAttr == CY_SI_MEM_DENIED))
                {
                    status = CY_SI_INVALID_RAM_ACCESS;
                    break;
                }
            }
        }
    }
    
    return status;
}

#if defined(__cplusplus)
}
#endif

/* [] END OF FILE */
