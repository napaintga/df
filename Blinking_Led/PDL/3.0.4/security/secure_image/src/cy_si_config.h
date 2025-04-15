/***************************************************************************//**
* \file cy_si_config.h
* \version 1.10
*
* \brief
* Definitions and function prototypes for Secure Image.
*
********************************************************************************
* \copyright
* Copyright 2017-2018, Cypress Semiconductor Corporation. All rights reserved.
* You may use this file only in accordance with the license, terms, conditions,
* disclaimers, and limitations in the end user license agreement accompanying
* the software package with which this file was provided.
*******************************************************************************/

#ifndef CY_SI_CONFIG_H
#define CY_SI_CONFIG_H

#include <string.h>
#include "prot/cy_prot.h"
#include "ipc/cy_ipc_drv.h"
#include "crypto/cy_crypto.h"
#include "crypto/cy_crypto_server.h"
#include "cy_crypto_config.h"
#include "cy_si_keystorage.h"

#if defined(__cplusplus)
extern "C" {
#endif


/***************************************
*               Macros
***************************************/
/** \addtogroup group_secure_image_macro
* \{
*/
 
/** \defgroup group_secure_image_version_macros Secure Image Version
* Macros to define the secure image version and ID.
* \{
*/
#define CY_SI_VERSION_MAJOR             1UL  /**< Major version */
#define CY_SI_VERSION_MINOR             10UL /**< Minor version */
#define CY_SI_APP_VERSION               ((CY_SI_VERSION_MAJOR << 24u) | (CY_SI_VERSION_MINOR << 16u)) /**< App Version */
#define CY_SI_ID                        CY_PDL_DRV_ID(0x38u)                            /**< Secure Image ID */
#define CY_SI_ID_INFO                   (uint32_t)( CY_SI_ID | CY_PDL_STATUS_INFO )     /**< Secure Image INFO ID */
#define CY_SI_ID_WARNING                (uint32_t)( CY_SI_ID | CY_PDL_STATUS_WARNING)   /**< Secure Image WARNING ID */
#define CY_SI_ID_ERROR                  (uint32_t)( CY_SI_ID | CY_PDL_STATUS_ERROR)     /**< Secure Image ERROR ID */
#define CY_SI_CHECKID(val)              ((uint32_t)(val) & (CY_PDL_MODULE_ID_Msk << CY_PDL_MODULE_ID_Pos)) /**< Check ID macro */
/** \} group_secure_image_version_macros */

/** \defgroup group_secure_image_address_macros Application Addresses 
* Secure Image and User application addresses. These define the addresses used by both the secure image and
* secure boot flow and must match those defined in the linker scripts.
* \{
*/
#define CY_SI_SECURE_FLASH_BEGIN        (0x10000000UL)      /**< Secure Image begin Flash address */
#define CY_SI_SECURE_FLASH_END          (0x10010000UL)      /**< Secure Image end Flash address */
#define CY_SI_SECURE_RAM_BEGIN          (0x08000000UL)      /**< Secure Image begin (secure) SRAM address */
#define CY_SI_SECURE_RAM_END            (0x08001000UL)      /**< Secure Image end (secure) SRAM address */
#define CY_SI_SECURE_VT_ADDRESS         (0x10000400UL)      /**< Secure Image VT adddress */

#define CY_SI_SECURE_DIGSIG_SIZE        (256u)              /**< Size (in Bytes) of the digital signature */
#define CY_SI_USERAPP_FLASH_BEGIN       (0x10020000UL)      /**< User application image begin Flash address */
#define CY_SI_USERAPP_FLASH_END         (0x10090000UL)      /**< User application image begin Flash address */
#define CY_SI_USERAPP_CM4_FLASH_BEGIN   (0x10080000UL)      /**< User application CM4 image begin Flash address */
#define CY_SI_USERAPP_STACK_SIZE        (0x1000UL)          /**< User application stack size (Bytes) */
#define CY_SI_USERAPP_DIGSIG_SIZE       (256u)              /**< Size (in Bytes) of the digital signature */
/** \} group_secure_image_address_macros */

/** \defgroup group_secure_image_blacklist_macros Flash and RAM blacklist sizes 
* Size of the Flash and RAM blacklists.
* \{
*/
#define CY_SI_MEM_RAM_BLACKLIST_SIZE    (1u)                /**< Secure SRAM Blacklist size */
#define CY_SI_MEM_FLASH_BLACKLIST_SIZE  (4u)                /**< Secure Flash Blacklist size */
/** \} group_secure_image_blacklist_macros */

/** \defgroup group_secure_image_blacklist_access_macros Blacklist Access Modifiers
* Access Modifiers for Flash and RAM blacklists.
* \{
*/
#define CY_SI_MEM_DENIED                (0x00UL)            /**< Read and Write access denied */
#define CY_SI_MEM_R                     (0x01UL)            /**< Read access allowed */
#define CY_SI_MEM_W                     (0x02UL)            /**< Write access allowed */
#define CY_SI_MEM_RW                    (0x03UL)            /**< Read and Write access allowed */
/** \} group_secure_image_blacklist_access_macros */

/** \defgroup group_secure_image_syscall_macros SysCall macros
* SysCall interrupt configuration macros.
* \{
*/
#define CY_SI_SYSCALL_INTR_NUMBER       (NvicMux0_IRQn)     /**< SysCall Interrupt NVIC position */
#define CY_SI_SYSCALL_INTR_PRIORITY     (3u)                /**< SysCall Interrupt priority */
#define CY_SI_SYSCALL_INTR_STRUCT       (0u)                /**< SysCall IPC_INTR_STRUCT number */
#define CY_SI_SYSCALL_INTR_STRUCT_MASK  (0x07UL)            /**< SysCall IPC_INTR_STRUCT notify interrupt mask */
/** \} group_secure_image_syscall_macros */

/** \defgroup group_secure_image_flashboot_clock_macros Flash Boot clock selection 
* Clock selection for Flash boot execution.
* \{
*/
#define CY_SI_FLASHBOOT_CLK_25MHZ       (0x00UL)            /**< 25MHz clock selection for Flashboot */
#define CY_SI_FLASHBOOT_CLK_8MHZ        (0x01UL)            /**< 8MHz clock selection for Flashboot */
#define CY_SI_FLASHBOOT_CLK_50MHZ       (0x02UL)            /**< 50MHz clock selection for Flashboot */
/** \} group_secure_image_flashboot_clock_macros */

/** \defgroup group_secure_image_flashboot_wait_macros Flash Boot wait window 
* Debugger wait window selection for Flash boot execution.
* \{
*/
#define CY_SI_FLASHBOOT_WAIT_20MS       (0x00UL)            /**< 20ms debugger wait window for Flashboot */
#define CY_SI_FLASHBOOT_WAIT_10MS       (0x01UL)            /**< 10ms debugger wait window for Flashboot */
#define CY_SI_FLASHBOOT_WAIT_1MS        (0x02UL)            /**< 1ms debugger wait window for Flashboot */
#define CY_SI_FLASHBOOT_WAIT_0MS        (0x03UL)            /**< 0ms debugger wait window for Flashboot */
#define CY_SI_FLASHBOOT_WAIT_100MS      (0x04UL)            /**< 100ms debugger wait window for Flashboot */
/** \} group_secure_image_flashboot_wait_macros */

/** \defgroup group_secure_image_flashboot_validate_macros Flash Boot validation 
* Flash boot validation selection in chip NORMAL mode.
* \{
*/
#define CY_SI_FLASHBOOT_VALIDATE_NO     (0x00UL)            /**< Do not validate app1 in NORMAL mode */
#define CY_SI_FLASHBOOT_VALIDATE_YES    (0x01UL)            /**< Validate app1 in NORMAL mode */
/** \} group_secure_image_flashboot_validate_macros */

/** \defgroup group_secure_image_flashboot_error_macros Flash Boot error codes 
* Error codes returned by Flash boot when it fails to boot. These can be verified by attaching the
* debugger and examining the RAM address pointed to by CY_SI_FLASHBOOT_ERROR_ADDR.
* \{
*/
#define CY_SI_FLASHBOOT_ERROR_ADDR      (CY_SRAM0_BASE + CY_SRAM0_SIZE \
                                        - (CY_SRAM0_SIZE/CPUSS_RAMC0_MACRO_NR)) \ /**< Flashboot error code address */
#define CY_SI_FLASHBOOT_ERROR_NONE                (0UL)             /**< Error code, Indicates no error */
#define CY_SI_FLASHBOOT_ERROR_INVALID_APP_SIGN    (0xF1000100UL)    /**< Error code, App signature validation failed */
#define CY_SI_FLASHBOOT_ERROR_INVALID_TOC         (0xF1000101UL)    /**< Error code, Empty or Invalid TOC    */
#define CY_SI_FLASHBOOT_ERROR_INVALID_KEY         (0xF1000102UL)    /**< Error code, Invalid Public key      */
#define CY_SI_FLASHBOOT_ERROR_UNREACHABLE         (0xF1000103UL)    /**< Error code, Got to unreachable code */
#define CY_SI_FLASHBOOT_ERROR_TOC_DATA_CLOCK      (0xF1000104UL)    /**< Error code, TOC contains invalid CM0+ clock attribute */
#define CY_SI_FLASHBOOT_ERROR_TOC_DATA_DELAY      (0xF1000105UL)    /**< Error code, TOC contains invalid listen window delay  */
#define CY_SI_FLASHBOOT_ERROR_FLL_CONFIG          (0xF1000106UL)    /**< Error code, FLL configuration failed */
#define CY_SI_FLASHBOOT_ERROR_INVALID_APP_DATA    (0xF1000107UL)    /**< Error code, Invalid application structure */
/** \} group_secure_image_flashboot_error_macros */

/** \defgroup group_secure_image_app_format_macros Application format
* Application format selection for secure boot.
* \{
*/
#define CY_SI_APP_FORMAT_BASIC          (0UL)               /**< Basic application format (no header) */
#define CY_SI_APP_FORMAT_CYPRESS        (1UL)               /**< Cypress application format (Cypress header) */
/** \} group_secure_image_app_format_macros */

/** \defgroup group_secure_image_app_type_macros Application type
* Application type selection for secure boot.
* \{
*/
#define CY_SI_APP_ID_FLASHBOOT          (0x8001UL)          /**< Flash boot ID Type */
#define CY_SI_APP_ID_SECUREIMG          (0x8002UL)          /**< Secure image ID Type */
#define CY_SI_APP_ID_BOOTLOADER         (0x8003UL)          /**< Bootloader ID Type */
/** \} group_secure_image_app_type_macros */

/** \} group_secure_image_macro */


/***************************************
*            Constants
***************************************/
/** \cond INTERNAL */

#define CY_SI_TOC_FLAGS_CLOCKS_MASK     (0x00000003UL)      /**< Mask for Flashboot clock selection */
#define CY_SI_TOC_FLAGS_CLOCKS_POS      (0UL)               /**< Bit position of Flashboot clock selection */
#define CY_SI_TOC_FLAGS_DELAY_MASK      (0x0000001CUL)      /**< Mask for Flashboot wait window selection */
#define CY_SI_TOC_FLAGS_DELAY_POS       (2UL)               /**< Bit position of Flashboot wait window selection */
#define CY_SI_TOC_FLAGS_APP_VERIFY_MASK (0x80000000UL)      /**< Mask for Flashboot NORMAL mode app1 validation */
#define CY_SI_TOC_FLAGS_APP_VERIFY_POS  (31UL)              /**< Bit position of Flashboot NORMAL mode app1 validation */

#define CY_SI_IMGVAL_VERIFYAPP_ADDR     ((volatile uint32_t *)0x16002040UL)         /**< Flash boot verify app function address */
#define CY_SI_IMGVAL_VERIFYAPP_REG      (*(uint32_t *)CY_SI_IMGVAL_VERIFYAPP_ADDR)  /**< Flash boot verify app function register */
#define CY_SI_IMGVAL_VALIDKEY_ADDR      ((volatile uint32_t *)0x16002044UL)         /**< Flash boot validate key function address */
#define CY_SI_IMGVAL_VALIDKEY_REG       (*(uint32_t *)CY_SI_IMGVAL_VALIDKEY_ADDR)   /**< Flash boot validate key function register */
#define CY_SI_IMGVAL_CRC16CCITT_ADDR    ((volatile uint32_t *)0x16002048UL)         /**< Flash boot CRC16CCITT function address */
#define CY_SI_IMGVAL_CRC16CCITT_REG     (*(uint32_t *)CY_SI_IMGVAL_CRC16CCITT_ADDR) /**< Flash boot CRC16CCITT function register */
#define CY_SI_IMGVAL_VALIDTOC_ADDR      ((volatile uint32_t *)0x1600204CUL)         /**< Flash boot validate TOC function address */
#define CY_SI_IMGVAL_VALIDTOC_REG       (*(uint32_t *)CY_SI_IMGVAL_VALIDTOC_ADDR)   /**< Flash boot validate TOC function register */

#define CY_SI_SYSCALL_ROM_HANDLER_ADDR  ((volatile uint32_t *)0x00000D20UL)             /**< ROM SysCall Handler address */
#define CY_SI_SYSCALL_ROM_HANDLER_REG   (*(uint32_t *)CY_SI_SYSCALL_ROM_HANDLER_ADDR)   /**< ROM SysCall Handler register */
#define CY_SI_SYSCALL_ROM_DEADSTATE_ADDR ((volatile uint32_t *)0x00000D18UL)             /**< ROM enter dead state function address */
#define CY_SI_SYSCALL_ROM_DEADSTATE_REG  (*(uint32_t *)CY_SI_SYSCALL_ROM_DEADSTATE_ADDR) /**< ROM enter dead state function register */
    
#define CY_SI_SYSCALL_IPC_INTR_MASK     (0x01UL)                    /**< SysCall IPC_INTR_STRUCT single interrupt mask */
#define CY_SI_SYSCALL_RESET             (0u)                        /**< Reset the SysCall IPC_STRUCT index */
#define CY_SI_SYSCALL_MAX               (2u)                        /**< Maximum SysCall IPC_STRUCT index */
#define CY_SI_SYSCALL_ROWSIZE           (CPUSS_FLASHC_PA_SIZE * 4u) /**< Flash row size (Bytes) */
#define CY_SI_SYSCALL_OPCODE_SHIFT      (24u)                       /**< Shift for SysCall opcode */
#define CY_SI_SYSCALL_MEMTYPE_SHIFT     (22u)                       /**< Shift for Checksum memtype */
#define CY_SI_SYSCALL_MEMTYPE_MASK      (0x00C00000UL)              /**< Mask for Checksum memtype */
#define CY_SI_SYSCALL_CHECKSUM_SHIFT    (8u)                        /**< Shift for Checksum row */
#define CY_SI_SYSCALL_CHECKSUM_MASK     (0x001FFF00UL)              /**< Mask for Checksum row */
#define CY_SI_SYSCALL_BLOCKING          (0x00000100UL)              /**< Blocking SysCalls */
#define CY_SI_SYSCALL_MEMTYPE_USER      (0UL)                       /**< Checksum User Flash */
#define CY_SI_SYSCALL_MEMTYPE_EMEEPROM  (1UL)                       /**< Checksum Em_EEPROM Flash */
#define CY_SI_SYSCALL_MEMTYPE_SFLASH    (2UL)                       /**< Checksum SFlash */

#define CY_SI_KEYADDR_PUBLIC_KEY        (0UL)   /**< Public Key index in SFlash */
#define CY_SI_KEYADDR_USER_KEY          (1UL)   /**< User key start index */
    
#define CY_SI_MEM_ADDR_BEGIN            (0u)    /**< Begin memory address location */
#define CY_SI_MEM_ADDR_END              (1u)    /**< End memory address location */
#define CY_SI_MEM_ADDR_ACCESS           (2u)    /**< Memory access location */
#define CY_SI_MEM_ELEM_SIZE             (3u)    /**< Number of elements per blacklist item */
#define CY_SI_MEM_TYPE_FLASH_SRC_SHIFT  (0UL)   /**< Shift for evaluating the Flash address as source */
#define CY_SI_MEM_TYPE_FLASH_SRC_MASK   (1UL << CY_SI_MEM_TYPE_FLASH_SRC_SHIFT) /**< Mask for evaluating the Flash address as source */
#define CY_SI_MEM_TYPE_FLASH_DST_SHIFT  (1UL)   /**< Shift for evaluating the Flash address as destination */
#define CY_SI_MEM_TYPE_FLASH_DST_MASK   (1UL << CY_SI_MEM_TYPE_FLASH_DST_SHIFT) /**< Mask for evaluating the Flash address as destination */
#define CY_SI_MEM_TYPE_RAM_SHIFT        (2UL)   /**< Shift for evaluating the RAM address */
#define CY_SI_MEM_TYPE_RAM_MASK         (1UL << CY_SI_MEM_TYPE_RAM_SHIFT) /**< Mask for evaluating the RAM address */

#define CY_SI_ALL_PC_MASK               (0x00007FFFUL)  /**< Mask for all supported PC values */
#define CY_SI_NO_IRQ                    (0UL)           /**< No interrupts */
#define CY_SI_ALL_IRQ                   (0x0000FFFFUL)  /**< All interrupts in an INTR register */
#define CY_SI_IRQ_MASK                  (0x01UL)        /**< Single interrupt mask */
#define CY_SI_IPC_NOTIFY_SHIFT          (16u)           /**< Shift for IPC notify interrupts */
#define CY_SI_CRC16_CCITT_POLYNOMIAL    (0x10210000UL)  /**< CRC-16 / CCITT-0 Polynomial */
#define CY_SI_CRC_INITSEED              (0xffff0000UL)  /**< CRC initial seed value */
#define CY_SI_TOC2_MAGICNUMBER          (0x01211220UL)  /**< TOC2 identifier */

#define CY_SI_TOC_EMPTY                 (0UL)           /**< Flag to tell that TOC2 and RTOC2 are empty */
#define CY_SI_TOC_INVALID               (1UL)           /**< Flag to tell that TOC2 and RTOC2 are invalid */
#define CY_SI_VALID_TOC2                (2UL)           /**< Flag to tell that TOC2 is valid */
#define CY_SI_VALID_RTOC2               (3UL)           /**< Flag to tell that RTOC2 is valid */

#define CY_SI_NORMAL_LIFECYCLE          (2UL)           /**< Device normal lifecycle */
#define CY_SI_SECURE_LIFECYCLE          (3UL)           /**< Device secure lifecycle */

/***************************************
*     ROM/SFlash function typedefs
***************************************/
typedef uint32_t (*sflash_verifyapp_func_t)(uint32_t param0, uint32_t param1, uint32_t param2,
                                 cy_stc_crypto_rsa_pub_key_t *param3);
typedef uint32_t (*sflash_validkey_func_t)(uint32_t param0, cy_stc_crypto_rsa_pub_key_t *param1);
typedef uint32_t (*sflash_crc16ccitt_func_t)(uint32_t param0, uint32_t param1);
typedef uint32_t (*sflash_validtoc_func_t)(uint32_t param0);
typedef void (*rom_handler_func_t)(void);
typedef void (*rom_deadstate_func_t)(void);

/** \endcond */


/***************************************
*               Structs
***************************************/

/**
* \addtogroup group_secure_image_data_structures
* \{
*/

/** Table of Content structure */
typedef struct{
    volatile uint32_t objSize;       /**< Object size (Bytes) */
    volatile uint32_t magicNum;      /**< TOC ID (magic number) */
    volatile uint32_t userKeyAddr;   /**< Secure key address in user Flash */
    volatile uint32_t smifCfgAddr;   /**< SMIF configuration structure */
    volatile uint32_t appAddr1;      /**< First user application object address */
    volatile uint32_t appFormat1;    /**< First user application format */
    volatile uint32_t appAddr2;      /**< Second user application object address */
    volatile uint32_t appFormat2;    /**< Second user application format */
    volatile uint32_t shashObj;      /**< Number of additional objects to be verified (S-HASH) */
    volatile uint32_t sigKeyAddr;    /**< Signature verification key address */
    volatile uint32_t addObj[116];   /**< Additional objects to include in S-HASH */
    volatile uint32_t tocFlags;      /**< Flags in TOC to control Flash boot options */
    volatile uint32_t crc;           /**< CRC16-CCITT */
}cy_stc_si_toc_t;

/** Secure image application header in Cypress format */
typedef struct{
    volatile uint32_t objSize;       /**< Object size (Bytes) */
    volatile uint32_t appId;         /**< Application ID/version */
    volatile uint32_t appAttributes; /**< Attributes (reserved for future use) */
    volatile uint32_t numCores;      /**< Number of cores */
    volatile uint32_t core0Vt;       /**< (CM0+)VT offset - offset to the vector table from that entry */
    volatile uint32_t core0Id;       /**< CM0+ core ID */
}cy_stc_si_appheader_t;

/** User application header in Cypress format */
typedef struct{
    volatile uint32_t objSize;       /**< Object size (Bytes) */
    volatile uint32_t appId;         /**< Application ID/version */
    volatile uint32_t appAttributes; /**< Attributes (reserved for future use) */
    volatile uint32_t numCores;      /**< Number of cores */
    volatile uint32_t core0Vt;       /**< (CM0+)VT offset - offset to the vector table from that entry */
    volatile uint32_t core1Vt;       /**< (CM4)VT offset - offset to the vector table from that entry */
    volatile uint32_t core0Id;       /**< CM0+ core ID */
    volatile uint32_t core1Id;       /**< CM4 core ID */
}cy_stc_user_appheader_t;

/** SysCall request format */
typedef struct
{
    uint32_t opcode;        /**< SysCall opcode */
    uint32_t arg1;          /**< Generic Argument #1. Functionality depends on the opcode */
    uint32_t arg2;          /**< Generic Argument #2. Functionality depends on the opcode */
    uint32_t arg3;          /**< Generic Argument #3. Functionality depends on the opcode */
}cy_stc_si_syscall_t;

/** \} group_secure_image_data_structures */


/***************************************
*               Globals
***************************************/

/* Protected (secure) variables */
extern cy_stc_crypto_server_context_t  cy_si_cryptoServerContext; /**< Context memory for the Crypto Server */
extern cy_stc_crypto_context_t *processData;                      /**< Crypto server processing pointer */
extern const uint32_t CyRamBlacklist[CY_SI_MEM_RAM_BLACKLIST_SIZE][CY_SI_MEM_ELEM_SIZE]; /**< RAM Blacklist */
extern const uint32_t CyFlashBlacklist[CY_SI_MEM_FLASH_BLACKLIST_SIZE][CY_SI_MEM_ELEM_SIZE]; /**< Flash Blacklist */


/***************************************
*               Enumerations
***************************************/

/**
* \addtogroup group_secure_image_enums
* \{
*/

/**
* Secure Image error codes
*/
typedef enum 
{
    CY_SI_SUCCESS               = 0x00u,                    /**< Operation success */
    CY_SI_BAD_PARAM             = CY_SI_ID_ERROR   | 0x01u, /**< Bad parameter was passed */
    CY_SI_FAILURE               = CY_SI_ID_ERROR   | 0x03u, /**< Generic operation failure */
    CY_SI_SYSCALL_DISALLOWED    = CY_SI_ID_INFO    | 0x04u, /**< Disallowed SysCall command */
    CY_SI_CRYPTO_DISALLOWED     = CY_SI_ID_ERROR   | 0x05u, /**< Disallowed Crypto command */
    CY_SI_EMPTY_TOC             = CY_SI_ID_INFO    | 0x06u, /**< Empty TOC2 and RTOC2 */
    CY_SI_INVALID_FORMAT        = CY_SI_ID_ERROR   | 0x07u, /**< Invalid application format */
    CY_SI_INVALID_TOC           = CY_SI_ID_ERROR   | 0x08u, /**< Invalid TOC */
    CY_SI_INVALID_KEY           = CY_SI_ID_ERROR   | 0x09u, /**< Invalid key format */
    CY_SI_INVALID_IMAGE         = CY_SI_ID_ERROR   | 0x0au, /**< Invalid application */
    CY_SI_INVALID_USERKEY       = CY_SI_ID_ERROR   | 0x0bu, /**< Invalid User key */
    CY_SI_INVALID_FLASH_ACCESS  = CY_SI_ID_WARNING | 0x0cu, /**< Invalid access to secure Flash address */
    CY_SI_INVALID_RAM_ACCESS    = CY_SI_ID_WARNING | 0x0du, /**< Invalid access to secure RAM address */
} cy_en_si_status_t;

/**
* SysCall opcodes
*/
typedef enum 
{
    CY_SI_OPCODE_SILICON_ID             = 0x00u,    /**< Return the Silicon ID */
    CY_SI_OPCODE_BLOW_EFUSE_BIT         = 0x01u,    /**< Blow the addressed EFuse Bit */
    CY_SI_OPCODE_READ_EFUSE_BYTE        = 0x03u,    /**< Read the addressed EFuse Byte */
    CY_SI_OPCODE_LOAD_FLASH_PAGE        = 0x04u,    /**< Load the Flash Page latch */
    CY_SI_OPCODE_WRITE_ROW              = 0x05u,    /**< Write a row of Flash */
    CY_SI_OPCODE_PROGRAM_ROW            = 0x06u,    /**< Program a row of (erased) Flash */
    CY_SI_OPCODE_ERASE_ALL_FLASH        = 0x0Au,    /**< Erase the whole Flash macro */
    CY_SI_OPCODE_CHECKSUM               = 0x0Bu,    /**< Checksum of whole/row Flash */
    CY_SI_OPCODE_COMPUTE_HASH           = 0x0Du,    /**< Compute the Hash of the Flash region */
    CY_SI_OPCODE_ERASE_SECTOR           = 0x14u,    /**< Erase a sector of Flash */
    CY_SI_OPCODE_SOFT_RESET             = 0x1Bu,    /**< Reset the system via CM0+ AIRCR reset bit */
    CY_SI_OPCODE_ERASE_ROW              = 0x1Cu,    /**< Erase a row of Flash */
    CY_SI_OPCODE_ERASE_SUBSECTOR        = 0x1Du,    /**< Erase a sub-sector of Flash */
    CY_SI_OPCODE_READ_UNIQUE_ID         = 0x1Fu,    /**< Return the unique ID of the die */
    CY_SI_OPCODE_FLASHC_VCC_CONFIG      = 0x30u,    /**< Configures the Flasch macro VCC to LP or ULP */
} cy_en_si_syscall_opcode_t;

/** \} group_secure_image_enums */


/***************************************
*        Function Prototypes
***************************************/

/**
* \addtogroup group_secure_image_functions
* \{
*/

/**
* \addtogroup group_secure_image_functions_general
* \{
*/
uint32_t* Cy_SI_GetSecureKeyAddr(uint32_t keyIndex);
uint32_t Cy_SI_GetVersion(void);
/** \} group_secure_image_functions_general */

/**
* \addtogroup group_secure_image_functions_initialization
* \{
*/
cy_en_si_status_t Cy_SI_ValidateImage(uint32_t *validToc);
cy_en_si_status_t Cy_SI_ConfigCrypto(void);
cy_en_si_status_t Cy_SI_ConfigSysCall(void);
cy_en_si_status_t Cy_SI_ConfigProt(void);
cy_en_si_status_t Cy_SI_CheckMemRange(uint32_t beginAddr, uint32_t endAddr, uint32_t checkType);
/** \} group_secure_image_functions_initialization */

/**
* \addtogroup group_secure_image_functions_direct
* \{
*/
__STATIC_INLINE uint32_t Cy_FB_VerifyApplication(uint32_t address, uint32_t length, uint32_t signature,
                                 cy_stc_crypto_rsa_pub_key_t *publicKey);
__STATIC_INLINE uint32_t Cy_FB_IsValidKey(uint32_t tocAddr, cy_stc_crypto_rsa_pub_key_t *publicKey);
__STATIC_INLINE void Cy_ROM_Handler(void);
__STATIC_INLINE void Cy_ROM_EnterDeadState(void);
/** \} group_secure_image_functions_direct */

/**
* \addtogroup group_secure_image_functions_isr
* \{
*/
void NvicMux0_IRQHandler(void);
void NvicMux2_IRQHandler(void);
void NvicMux31_IRQHandler(void);
/** \} group_secure_image_functions_isr */

/**
* \addtogroup group_secure_image_functions_direct
* \{
*/

/*******************************************************************************
* Function Name: Cy_FB_VerifyApplication
****************************************************************************//**
*
* \brief Verifies the secure application digital signature.
*
* This function relies on the assumption that the application digital signature
* was calculated in the following manner:
* 1. SHA-256 hash of the binary application image is calculated.
* 2. The hash (digital digest) is signed using a RSA-1024/2056 <b>private</b> 
*    key to generate the digital signature.
* 3. The digital signature is placed in the application object in Cypress format.
*
* The application verification is performed by performing the following
* operations:
* 1. SHA-256 hash of the binary application image is calculated.
* 2. The application digital signature is decrypted using the RSA-1024/2056
*    <b>public</b> key.
* 3. The hash and the decrypted digital signature are compared. If they are
*    equivalent, the image is valid.
*
* \note This is a direct branch to a function residing in SFlash.
*
* \param address      
* Staring address of the application area to be verified with secure signature.
*
* \param length
* The length of the area to be verified.
* 
* \param signature
* Starting address of the signature inside the application residing in Flash.
* 
* \param publicKey
* Pointer to the public key structure.
*
* \return
* - 1 if the digital secure signature verification succeeds.
* - 0 if the digital secure signature verification of the application fails.
*
*******************************************************************************/
__STATIC_INLINE uint32_t Cy_FB_VerifyApplication(uint32_t address, uint32_t length, 
                                        uint32_t signature, cy_stc_crypto_rsa_pub_key_t *publicKey)
{
    sflash_verifyapp_func_t fp = (sflash_verifyapp_func_t)CY_SI_IMGVAL_VERIFYAPP_REG;
    return ( fp(address, length, signature, publicKey) );
}


/*******************************************************************************
* Function Name: Cy_FB_IsValidKey
****************************************************************************//**
*
* \brief Checks whether the Public Key structure is valid.
*
* The public key structure must be as specified as in cy_si_stc_public_key_t.
* Supported signature schemes are:
*   0x00: RSASSA-PKCS1-v1_5-2048
*   0x01: RSASSA-PKCS1-v1_5-1024
*
* \note This is a direct branch to a function residing in SFlash.
*
* \return
* - 1 if Public Key has a valid format
* - 0 if Public Key has an invalid format
*
*******************************************************************************/
__STATIC_INLINE uint32_t Cy_FB_IsValidKey(uint32_t tocAddr, cy_stc_crypto_rsa_pub_key_t *publicKey)
{
    sflash_validkey_func_t fp = (sflash_validkey_func_t)CY_SI_IMGVAL_VALIDKEY_REG;
    return ( fp(tocAddr, publicKey) );
}


/*******************************************************************************
* Function Name: Cy_FB_Crc16ccitt
****************************************************************************//**
* 
* \brief Computes CRC-16-CCITT with polynomial=0x1021, initial value=0xffff.
*
* \note This function does not work if length is greater than 64KB due to
*       Crypto block hardware limitations.
*
* \param address    Start address of a memory block to calculate CRC for.
* \param length     Length of a memory block to calculate CRC for.
*
* \return
* Calculated CRC-16-CCITT
*
*******************************************************************************/
__STATIC_INLINE uint32_t Cy_FB_Crc16ccitt(uint32_t address, uint32_t length)
{
    sflash_crc16ccitt_func_t fp = (sflash_crc16ccitt_func_t)CY_SI_IMGVAL_CRC16CCITT_REG;
    return ( fp(address, length) );
}


/*******************************************************************************
* Function Name: Cy_FB_ValidateToc
****************************************************************************//**
* 
* \brief Checks if TOC2 and RTOC2 are valid and returns the address of the
* correct TOC. 
*
* This function also performs CRC-16-CCITT calculation of the TOC CRC using the
* Cy_FB_Crc16ccitt() function. High-level view of the operations are as follows.
*
* \code{.unparsed}
* Is TOC_2 empty?
*   YES: Is RTOC_2 empty?
*     YES - return CY_SI_TOC_EMPTY
*     NO: Is RTOC_2 valid?
*       YES - return RTOC_2 address
*       NO  - return 0
*   NO: Is TOC_2 valid?
*     YES - return TOC_2 address
*     NO: Is RTOC_2 valid?
*       YES - return RTOC_2 address
*       NO  - return CY_SI_TOC_INVALID
*
* Empty   + Empty   = Empty
* Invalid + Invalid = Invalid
* Empty   + Invalid = Invalid
* ANY     + Valid   = Valid
*
* \endcode
*
* \param tocAddress     Base address of the second TOC (i.e TOC2_OBJECT_SIZE).
*
* \return
* - CY_SI_TOC_INVALID if TOC is invalid
* - CY_SI_TOC_EMPTY   if TOC is empty
* - Address of the valid TOC, if the TOC is valid
*
*******************************************************************************/
__STATIC_INLINE uint32_t Cy_FB_ValidateToc (uint32_t tocAddress)
{
    sflash_validtoc_func_t fp = (sflash_validtoc_func_t)CY_SI_IMGVAL_VALIDTOC_REG;
    return ( fp(tocAddress) );
}


/*******************************************************************************
* Function Name: Cy_ROM_Handler
****************************************************************************//**
*
* \brief Services and executes the requested SysCall.
*
* \note This is a direct branch to a function residing in ROM.
*
*******************************************************************************/
__STATIC_INLINE void Cy_ROM_Handler(void)
{
    rom_handler_func_t fp = (rom_handler_func_t)CY_SI_SYSCALL_ROM_HANDLER_REG;
    fp();
}


/*******************************************************************************
* Function Name: Cy_ROM_EnterDeadState
****************************************************************************//**
*
* \brief Transitions the device DEAD state when called from SECURE mode.
*
* This function will enter CM0+ deep-sleep when called from NORMAL mode.
*
* \note This is a direct branch to a function residing in ROM.
*
*******************************************************************************/
__STATIC_INLINE void Cy_ROM_EnterDeadState(void)
{
    rom_deadstate_func_t fp = (rom_deadstate_func_t)CY_SI_SYSCALL_ROM_DEADSTATE_REG;
    fp();
}

/** \} group_secure_image_functions_direct */

/** \} group_secure_image_functions */

#if defined(__cplusplus)
}
#endif

#endif /* CY_SI_CONFIG_H */

/* [] END OF FILE */
