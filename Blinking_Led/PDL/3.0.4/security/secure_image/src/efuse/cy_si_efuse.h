/***************************************************************************//**
* \file cy_si_efuse.h
* \version 1.10
*
* \brief
* Header file for EFuse configuration.
*
********************************************************************************
* \copyright
* Copyright 2017-2018, Cypress Semiconductor Corporation. All rights reserved.
* You may use this file only in accordance with the license, terms, conditions,
* disclaimers, and limitations in the end user license agreement accompanying
* the software package with which this file was provided.
*******************************************************************************/

#ifndef CY_SI_EFUSE_H
#define CY_SI_EFUSE_H

#include "efuse/cy_efuse.h"

#if defined(__cplusplus)
extern "C" {
#endif

/** \cond INTERNAL */

/***************************************
* WARNING: Enables EFUSE write
***************************************/

#define CY_SI_EFUSE_AVAILABLE   (0) /**< Control macro for including/excluding EFUSE data */

/***************************************
*               Macros
***************************************/

/* Helper macros to populate the efuse data structure */
#define CY_SI_EXPAND_ARG2(arg)      arg, arg
#define CY_SI_EXPAND_ARG3(arg)      arg, arg, arg
#define CY_SI_EXPAND_ARG4(arg)      CY_SI_EXPAND_ARG2(arg),  CY_SI_EXPAND_ARG2(arg)     /**< Expand to 2 */
#define CY_SI_EXPAND_ARG8(arg)      CY_SI_EXPAND_ARG4(arg),  CY_SI_EXPAND_ARG4(arg)     /**< Expand to 4 */
#define CY_SI_EXPAND_ARG16(arg)     CY_SI_EXPAND_ARG8(arg),  CY_SI_EXPAND_ARG8(arg)     /**< Expand to 8 */
#define CY_SI_EXPAND_ARG24(arg)     CY_SI_EXPAND_ARG16(arg), CY_SI_EXPAND_ARG8(arg)     /**< Expand to 16 */
#define CY_SI_EXPAND_ARG32(arg)     CY_SI_EXPAND_ARG16(arg), CY_SI_EXPAND_ARG16(arg)    /**< Expand to 32 */
#define CY_SI_EXPAND_ARG64(arg)     CY_SI_EXPAND_ARG32(arg), CY_SI_EXPAND_ARG32(arg)    /**< Expand to 64 */
#define CY_SI_EXPAND_ARG128(arg)    CY_SI_EXPAND_ARG64(arg), CY_SI_EXPAND_ARG64(arg)    /**< Expand to 128 */
#define CY_SI_EXPAND_ARG256(arg)    CY_SI_EXPAND_ARG128(arg), CY_SI_EXPAND_ARG128(arg)  /**< Expand to 256 */
#define CY_SI_EXPAND_ARG512(arg)    CY_SI_EXPAND_ARG256(arg), CY_SI_EXPAND_ARG256(arg)  /**< Expand to 512 */

/* EFUSE bit action macros */
#define CY_SI_EFUSE_STATE_SET       (0x01U) /**< Tell programmer to set the EFUSE bit */
#define CY_SI_EFUSE_STATE_UNSET     (0x00U) /**< Tell programmer to check that the EFUSE bit is not set */
#define CY_SI_EFUSE_STATE_IGNORE    (0xffU) /**< Tell programmer to ignore the EFUSE bit */

/* EFuse ignore write bits macros */
#define CY_SI_EFUSE_STATE_IGNORE2   CY_SI_EXPAND_ARG2(CY_SI_EFUSE_STATE_IGNORE)      /**< Ignore 2-bits */
#define CY_SI_EFUSE_STATE_IGNORE3   CY_SI_EXPAND_ARG3(CY_SI_EFUSE_STATE_IGNORE)      /**< Ignore 3-bits */
#define CY_SI_EFUSE_STATE_IGNORE4   CY_SI_EXPAND_ARG4(CY_SI_EFUSE_STATE_IGNORE)      /**< Ignore 4-bits */
#define CY_SI_EFUSE_STATE_IGNORE8   CY_SI_EXPAND_ARG8(CY_SI_EFUSE_STATE_IGNORE)      /**< Ignore 8-bits */
#define CY_SI_EFUSE_STATE_IGNORE16  CY_SI_EXPAND_ARG16(CY_SI_EFUSE_STATE_IGNORE)     /**< Ignore 16-bits */
#define CY_SI_EFUSE_STATE_IGNORE32  CY_SI_EXPAND_ARG32(CY_SI_EFUSE_STATE_IGNORE)     /**< Ignore 32-bits */
#define CY_SI_EFUSE_STATE_IGNORE64  CY_SI_EXPAND_ARG64(CY_SI_EFUSE_STATE_IGNORE)     /**< Ignore 64-bits */
#define CY_SI_EFUSE_STATE_IGNORE128 CY_SI_EXPAND_ARG128(CY_SI_EFUSE_STATE_IGNORE)    /**< Ignore 128-bits */
#define CY_SI_EFUSE_STATE_IGNORE256 CY_SI_EXPAND_ARG256(CY_SI_EFUSE_STATE_IGNORE)    /**< Ignore 256-bits */
#define CY_SI_EFUSE_STATE_IGNORE512 CY_SI_EXPAND_ARG512(CY_SI_EFUSE_STATE_IGNORE)    /**< Ignore 512-bits */

/* EFuse macros for reserved bits */
#define CY_SI_EFUSE_RESERVED0 {CY_SI_EFUSE_STATE_IGNORE256, CY_SI_EFUSE_STATE_IGNORE32, \
                               CY_SI_EFUSE_STATE_IGNORE16, CY_SI_EFUSE_STATE_IGNORE8 }  /**< Ignore EFUSE_DATA_Type.RESERVED0 EFUSE bits */
#define CY_SI_EFUSE_RESERVED1 {CY_SI_EFUSE_STATE_IGNORE128, CY_SI_EFUSE_STATE_IGNORE32} /**< Ignore EFUSE_DATA_Type.RESERVED1 EFUSE bits */
#define CY_SI_EFUSE_LIFECYCLE_RESERVED0 {CY_SI_EFUSE_STATE_IGNORE4}                     /**< Ignore EFUSE_LIFECYCLE_STAGE_Type.RESERVED0 EFUSE bits */

/* SFlash area SYS AP MPU protection Enable macros */
#define CY_SI_EFUSE_SFLASH_ALLOWED_ENTIRE  {CY_SI_EFUSE_STATE_UNSET, CY_SI_EFUSE_STATE_UNSET} /**< Allow all SFlash */
#define CY_SI_EFUSE_SFLASH_ALLOWED_1_2     {CY_SI_EFUSE_STATE_SET, CY_SI_EFUSE_STATE_UNSET}   /**< Allow first 1/2 of SFlash */
#define CY_SI_EFUSE_SFLASH_ALLOWED_1_4     {CY_SI_EFUSE_STATE_UNSET, CY_SI_EFUSE_STATE_SET}   /**< Allow first 1/4 of SFlash */
#define CY_SI_EFUSE_SFLASH_ALLOWED_NONE    {CY_SI_EFUSE_STATE_SET, CY_SI_EFUSE_STATE_SET}     /**< Do not allow SFlash */
#define CY_SI_EFUSE_SFLASH_ALLOWED_IGNORE  {CY_SI_EFUSE_STATE_IGNORE2}                        /**< Ignore bits */

/* MMIO area SYS AP MPU protection Enable macros */
#define CY_SI_EFUSE_MMIO_ALLOWED_ENTIRE    {CY_SI_EFUSE_STATE_UNSET, CY_SI_EFUSE_STATE_UNSET} /**< Allow all MMIO */
#define CY_SI_EFUSE_MMIO_ALLOWED_IPC       {CY_SI_EFUSE_STATE_SET, CY_SI_EFUSE_STATE_UNSET}   /**< Allow only IPC */
#define CY_SI_EFUSE_MMIO_ALLOWED_NONE      {CY_SI_EFUSE_STATE_UNSET, CY_SI_EFUSE_STATE_SET}   /**< Do not allow MMIO */
#define CY_SI_EFUSE_MMIO_ALLOWED_IGNORE    {CY_SI_EFUSE_STATE_IGNORE2}                        /**< Ignore bits */

/* Flash area SYS AP MPU protection Enable macros */
#define CY_SI_EFUSE_FLASH_ALLOWED_ENTIRE   {CY_SI_EFUSE_STATE_UNSET, CY_SI_EFUSE_STATE_UNSET, CY_SI_EFUSE_STATE_UNSET}  /**< Allow all Flash */
#define CY_SI_EFUSE_FLASH_ALLOWED_7_8      {CY_SI_EFUSE_STATE_SET, CY_SI_EFUSE_STATE_UNSET, CY_SI_EFUSE_STATE_UNSET}    /**< Allow first 7/8 of Flash */
#define CY_SI_EFUSE_FLASH_ALLOWED_3_4      {CY_SI_EFUSE_STATE_UNSET, CY_SI_EFUSE_STATE_SET, CY_SI_EFUSE_STATE_UNSET}    /**< Allow first 3/4 of Flash */
#define CY_SI_EFUSE_FLASH_ALLOWED_1_2      {CY_SI_EFUSE_STATE_SET, CY_SI_EFUSE_STATE_SET, CY_SI_EFUSE_STATE_UNSET}      /**< Allow first 1/2 of Flash */
#define CY_SI_EFUSE_FLASH_ALLOWED_1_4      {CY_SI_EFUSE_STATE_UNSET, CY_SI_EFUSE_STATE_UNSET, CY_SI_EFUSE_STATE_SET}    /**< Allow first 1/4 of Flash */
#define CY_SI_EFUSE_FLASH_ALLOWED_1_8      {CY_SI_EFUSE_STATE_SET, CY_SI_EFUSE_STATE_UNSET, CY_SI_EFUSE_STATE_SET}      /**< Allow first 1/8 of Flash */
#define CY_SI_EFUSE_FLASH_ALLOWED_1_16     {CY_SI_EFUSE_STATE_UNSET, CY_SI_EFUSE_STATE_SET, CY_SI_EFUSE_STATE_SET}      /**< Allow first 1/16 of Flash */
#define CY_SI_EFUSE_FLASH_ALLOWED_NONE     {CY_SI_EFUSE_STATE_SET, CY_SI_EFUSE_STATE_SET, CY_SI_EFUSE_STATE_SET}        /**< Do not allow Flash */
#define CY_SI_EFUSE_FLASH_ALLOWED_IGNORE   {CY_SI_EFUSE_STATE_IGNORE, CY_SI_EFUSE_STATE_IGNORE,CY_SI_EFUSE_STATE_IGNORE} /**< Ignore bits */

/* SRAM area SYS AP MPU protectionEnable macros */
#define CY_SI_EFUSE_SRAM_ALLOWED_ENTIRE    {CY_SI_EFUSE_STATE_UNSET, CY_SI_EFUSE_STATE_UNSET, CY_SI_EFUSE_STATE_UNSET}  /**< Allow all SRAM */
#define CY_SI_EFUSE_SRAM_ALLOWED_7_8       {CY_SI_EFUSE_STATE_SET, CY_SI_EFUSE_STATE_UNSET, CY_SI_EFUSE_STATE_UNSET}    /**< Allow first 7/8 of SRAM */
#define CY_SI_EFUSE_SRAM_ALLOWED_3_4       {CY_SI_EFUSE_STATE_UNSET, CY_SI_EFUSE_STATE_SET, CY_SI_EFUSE_STATE_UNSET}    /**< Allow first 3/4 of SRAM */
#define CY_SI_EFUSE_SRAM_ALLOWED_1_2       {CY_SI_EFUSE_STATE_SET, CY_SI_EFUSE_STATE_SET, CY_SI_EFUSE_STATE_UNSET}      /**< Allow first 1/2 of SRAM */
#define CY_SI_EFUSE_SRAM_ALLOWED_1_4       {CY_SI_EFUSE_STATE_UNSET, CY_SI_EFUSE_STATE_UNSET, CY_SI_EFUSE_STATE_SET}    /**< Allow first 1/4 of SRAM */
#define CY_SI_EFUSE_SRAM_ALLOWED_1_8       {CY_SI_EFUSE_STATE_SET, CY_SI_EFUSE_STATE_UNSET, CY_SI_EFUSE_STATE_SET}      /**< Allow first 1/8 of SRAM */
#define CY_SI_EFUSE_SRAM_ALLOWED_1_16      {CY_SI_EFUSE_STATE_UNSET, CY_SI_EFUSE_STATE_SET, CY_SI_EFUSE_STATE_SET}      /**< Allow first 1/16 of SRAM */
#define CY_SI_EFUSE_SRAM_ALLOWED_NONE      {CY_SI_EFUSE_STATE_SET, CY_SI_EFUSE_STATE_SET, CY_SI_EFUSE_STATE_SET}        /**< Do not allow SRAM */
#define CY_SI_EFUSE_SRAM_ALLOWED_IGNORE    {CY_SI_EFUSE_STATE_IGNORE, CY_SI_EFUSE_STATE_IGNORE,CY_SI_EFUSE_STATE_IGNORE} /**< Ignore bits */

/* SMIF XIP area SYS AP MPU protection Enable macros */
#define CY_SI_EFUSE_SMIF_XIP_ALLOWED_ENTIRE    CY_SI_EFUSE_STATE_UNSET /**< Allow all SMIF XIP region */
#define CY_SI_EFUSE_SMIF_XIP_ALLOWED_NONE      CY_SI_EFUSE_STATE_SET   /**< Do not allow SMIF XIP region */
#define CY_SI_EFUSE_SMIF_XIP_ALLOWED_IGNORE    CY_SI_EFUSE_STATE_IGNORE /**< Ignore bits */

/* Customer data ignore macros */
#define CY_SI_EFUSE_CUSTOMER_IGNORE8      {{CY_SI_EFUSE_STATE_IGNORE8}}                     /**< Customer data ignore 8-bits */
#define CY_SI_EFUSE_CUSTOMER_IGNORE16     CY_SI_EXPAND_ARG2(CY_SI_EFUSE_CUSTOMER_IGNORE8)   /**< Customer data ignore 16-bits */
#define CY_SI_EFUSE_CUSTOMER_IGNORE32     CY_SI_EXPAND_ARG2(CY_SI_EFUSE_CUSTOMER_IGNORE16)  /**< Customer data ignore 32-bits */
#define CY_SI_EFUSE_CUSTOMER_IGNORE64     CY_SI_EXPAND_ARG2(CY_SI_EFUSE_CUSTOMER_IGNORE32)  /**< Customer data ignore 64-bits */
#define CY_SI_EFUSE_CUSTOMER_IGNORE128    CY_SI_EXPAND_ARG2(CY_SI_EFUSE_CUSTOMER_IGNORE64)  /**< Customer data ignore 128-bits */
#define CY_SI_EFUSE_CUSTOMER_IGNORE256    CY_SI_EXPAND_ARG2(CY_SI_EFUSE_CUSTOMER_IGNORE128) /**< Customer data ignore 256-bits */
#define CY_SI_EFUSE_CUSTOMER_IGNORE512    CY_SI_EXPAND_ARG2(CY_SI_EFUSE_CUSTOMER_IGNORE256) /**< Customer data ignore 512-bits */

/** \endcond */

#if defined(__cplusplus)
}
#endif

#endif /* CY_SI_EFUSE_H */

/* [] END OF FILE */
