/***************************************************************************//**
* \file cy_si_main.c
* \version 1.10
*
* \brief
* Secure Image CM0+ application main.
*
* This application has dependencies to secure image and PDL source files.
* Refer to their respective documentation for additional information.
*
********************************************************************************
* \copyright
* Copyright 2017-2018, Cypress Semiconductor Corporation. All rights reserved.
* You may use this file only in accordance with the license, terms, conditions,
* disclaimers, and limitations in the end user license agreement accompanying
* the software package with which this file was provided.
*******************************************************************************/

/**
* \mainpage Cypress Secure Image
*
* Secure Image (SI) is a reference design for building a secure system in 
* user Flash and RAM. Its main objectives are as follows:
*
* - Demonstrate secure boot construction and how to preserve the chain of trust
*   from ROM boot to Flash boot and finally to the first and second secure 
*   applications.
* - Secure System Calls (SysCall), which are special routines that execute out
*   of ROM and can only be communicated via IPC requests.
* - Secure Crypto execution, which access the Crypto engine to perform Crypto
*   routines via IPC requests.
* - Protection Unit configuration for establishing a secure system.
*
* To support these objectives, the secure image implements a security policy 
* that adheres to the following requirements:
*
* - Protection Context (PC)=0 is the secure mode of operation. Only the designated
*   secure core (CM0+) core and the Crypto core are allowed to be in PC=0 state.
* - The designated secure core (CM0+) and the Crypto core bus masters are the
*   only entities that have full PC=0 bus accesses.
* - All requests for PC=0 operations by non-secure bus masters (such as the 
*   CM4 core or TC) are performed by initiating an IPC request to the secure
*   processor (CM0+). The secure core then performs the requested operation in
*   a secure environment and releases the IPC upon completion.
* - System Calls (SysCall), which execute out of ROM are performed in PC=0 
*   environment.
* - All Crypto operations are performed in PC=0 environment. Critical MMIO 
*   registers and memory regions (RO and R/W) are protected using the 
*   appropriate protection units. These are configured to only allow PC=0 
*   accesses to those resources.
* - Boundary checking is performed on the PC=0 operations according to a user
*   defined blacklist. This allows requests to be filtered and allow PC=0 
*   operations to be carried out only on the allowed regions.
* - IPC registers and referenced memory locations are intentionally left
*   unprotected. These must be accessible to all bus masters and the strategy
*   for protecting these is up to the client application.
*
* \section group_secure_image_configuration Design Considerations
* 
* This design is integrated into a PSoC Creator project called "secure_image",
* which is provided in the same directory hierarchy as this source file. The 
* "secure_Image" project workspace also has a "user_app0" project, which is
* a sample user application that is validated by the secure image. The two
* applications are combined into a single hex file. Follow these instructions
* to get started:
*
* 1. Copy the entire "security" directory from the PDL installation to 
*    a local working directory. Open the PSoC Creator workspace located 
*    in the example directory.
* 2. Build "secure_image" project in PSoC Creator. At the end of the CM0+
*    build process, a postbuild script called "secure_image_postbuild.bat"
*    will execute (refer to the project build settings). This process signs
*    the CM0+ application with the digital signature and exports the 
*    shared symbols for "user_app0".
* 3. Build "user_app0" project in PSoC Creator. At the end of the CM4
*    build process, a postbuild script called "user_app0_postbuild.bat"
*    will execute (refer to the project build settings). This process signs
*    the combined CM0+ and CM4 application and then merges it with the
*    CM0+ "secure_image". The final image and hex file are located in the
*    "user_app0" CortexM4 directory.
* 4. Program the "user_app0" hex file on to the PSoC 6 device.
* 5. Observe the two LEDs toggling (P0.3 and P1.1). This shows that both 
*    the CM0+ and CM4 cores have successfully booted and passed the 
*    application unit tests.
*
* Note: If reverting the device to skip the secure boot process, remember to
* also erase the SFlash region. This can be done using PSoC Programmer.
*
* Note: If using a miniprog/kitprog through PSoC Creator, attaching the debugger
* before the image validation (Cy_SI_ValidateImage()) completes will result 
* in disconnection due to an SWD time-out event. You may work around this issue
* by either using a different debug probe or by running PSoC Creator via 
* CMD with the -swdretry flag set. 
*
* \subsection group_secure_image_build Build Environment
*
* The secure image and the accompanying sample user application use custom
* linker scripts. These are designed to work with the default startup files
* provided by the PDL. For more information on the contents of these files, 
* refer to the \ref page_secure_image_linker_scripts page. When revising 
* these, also make the necessary changes in the addresses referenced in
* cy_si_config.h.
*
* The design uses cymcuelftool.exe along with a couple of batch scripts
* (secure_image_postbuild.bat and user_app0_postbuild.bat). These are used 
* to export secure image symbols, populate the security regions in the
* elf files and to perform elf file merges. Review the referenced paths in the
* batch scripts and verify that the correct tools are installed (objdump,
* readelf, OpenSSL v1.0.2) before proceeding.
*
* Note that the secure image also contains a script (rsa_keygen.bat) for 
* generating RSA-2048 public and private keys using OpenSSL. These can be found
* in the same directory as the postbuild batch scripts.
*
* \subsection group_secure_image_symbols Symbol Export
*
* Symbols used in the secure image are exported out so that they can be
* referenced by user application(s). The user application must also be
* able to use the secure image as a library of secure routines. Two steps
* are taken to ensure this.
* 
* 1. The linkers for the supported toolchains (GCC, ARMCC, IAR) are configured
*    to keep symbols from the following entities: cy_crypto*, cy_si*.
*    This allows all of the Crypto library and secure image specific code and
*    symbols to be kept in the final image.
* 2. The export symbols are defined in cy_si_export_symbols.txt, which allow
*    those listed to be exported out as assembly defines. This is accomplished
*    by using the code sharing facility in the cymcuelftool. Refer to the
*    accompanying batch scripts for the implementation.
*
* \subsection group_secure_image_gccbuild GCC Considerations
*
* The GCC compiler in the "secure_image" PSoC Creator project is configured
* to generate function sections "-ffunction-sections". The GCC linker is 
* configured to remove unused sections by using "-Wl,–gc-sections". The 
* "secure_image" needs to export a number of symbols to be referenced in the
* "user_app0" project, however certain symbols get optimized out despite 
* proper efforts in the linker script to keep them. To work around this problem, 
* Cy_SI_GetVersion(), Cy_SI_GetSecureKeyAddr(), cy_toc2 and cy_rtoc2 are deliberately
* referenced in an unreachable code. If your build environment differs, you may 
* remove this code. Verify that both cy_toc2 and cy_rtoc2 symbols are kept in the
* generated elf files and that Cy_SI_GetVersion() and Cy_SI_GetSecureKeyAddr symbols
* are kept in the exported assembly file.
*
* \subsection group_secure_image_mdkbuild ARMCC (MDK) Considerations
*
* The ARMCC compiler for both "secure_image" and "user_app0" are configured
* to suppress the 1296 warning "--diag_suppress 1296". This warning is
* unavoidable as pointer to non-pointer casts are necessary in many places
* in this design (e.g protection unit configuration and IPC data).
* The ARMCC linker is also configured to suppress the warnings regarding
* unused sections in the linker scripts "--diag_suppress=L6314W,L6329W".
* This suppression can be removed if your project does not require the
* use of those unused sections.
*
* \section group_secure_image_MISRA MISRA-C Compliance
* <table class="doxtable">
*   <tr>
*     <th>MISRA rule</th>
*     <th>Rule Class (Required/Advisory)</th>
*     <th>Rule Description</th>
*     <th>Description of Deviation(s)</th>
*   </tr>
*   <tr>
*     <td>8.7</td>
*     <td>R</td>
*     <td>Objects shall be defined at block scope if they are only accessed from within a single function.</td>
*     <td>Defined at file scope to highlight that protection unit configurations are in Flash.</td>
*   </tr>
*   <tr>
*     <td>8.8</td>
*     <td>R</td>
*     <td>An external object or function shall be declared in one and only one file.</td>
*     <td>Function and variable declarations are within cy_si_main.c as it does not need an 
*         accompanying header file to be used in other source files.</td>
*   </tr>
*   <tr>
*     <td>10.3</td>
*     <td>R</td>
*     <td>The value of a complex expression of integer type shall only be cast to a type of the 
*         same signedness that is no wider than the underlying type of the expression.</td>
*     <td>Necessary for extracting the opcode from the memory pointed to by the IPC DATA register.</td>
*   </tr>
*   <tr>
*     <td>11.4</td>
*     <td>A</td>
*     <td>A cast should not be performed between a pointer to object type and a different pointer to object type.</td>
*     <td>Base addresses of protected memory needed for protection unit configurations.</td>
*   </tr>
*   <tr>
*     <td>11.5</td>
*     <td>R</td>
*     <td>Dangerous pointer cast results in loss of volatile qualification.</td>
*     <td>Required to directly execute ROM and SFlash functions.</td>
*   </tr>
*   <tr>
*     <td>19.10</td>
*     <td>R</td>
*     <td>In the definition of a function-like macro each instance of a parameter 
          shall be enclosed in parentheses unless it is used as the operand of # or ##.</td>
*     <td>Used in helper macros for populating the EFUSE data struct.</td>
*   </tr>
*   <tr>
*     <td>20.6</td>
*     <td>R</td>
*     <td>The macro offsetof, in library <stddef.h>, shall not be used.</td>
*     <td>Used to populate the public key struct elements.</td>
*   </tr>
* </table>
*
* \section group_secure_image_changelog Changelog
* <table class="doxtable">
*   <tr><th>Version</th><th>Changes</th><th>Reason for Change</th></tr>
*   <tr>
*     <td rowspan="3">1.10</td>
*     <td>Added information on protection units used by the SROM and FLASHBOOT.</td>
*     <td>Certain protection units are pre-configured at boot and should not be used by the application </td>
*   </tr>
*   <tr>
*     <td>Updated EFUSE documentation and added info regarding SWD through miniprog/kitprog.</td>
*     <td>Improved documentation</td>
*   </tr>
*   <tr>
*     <td>Removed irrelevant system call opcodes.</td>
*     <td>Unnecessary information</td>
*   </tr>
*   <tr>
*     <td rowspan="2">1.0.1</td>
*     <td>Updated user_app0 Flash row checksum parameter and RWW disable macro
*         in user_app0 project build settings.</td>
*     <td>Flash driver updates</td>
*   </tr>
*   <tr>
*     <td>Improved documentation</td>
*     <td></td>
*   </tr>
*   <tr>
*     <td>1.0</td>
*     <td>Initial version</td>
*     <td></td>
*   </tr>
* </table>
*
* \defgroup group_secure_image_macro Macros
* \defgroup group_secure_image_functions Functions
* \{
*   \defgroup group_secure_image_functions_general          General Secure Image Functions
*   \defgroup group_secure_image_functions_initialization   Initialization Functions
*   \defgroup group_secure_image_functions_direct           Direct Branch SFlash/ROM Functions
*   \defgroup group_secure_image_functions_isr              Interrupt Service Routines
*   \defgroup group_secure_image_functions_tasks            Secure Image tasks
* \}
* \defgroup group_secure_image_data_structures Data Structures
* \defgroup group_secure_image_enums Enumerated Types
*
*
********************************************************************************
* \page page_secure_image_implementation Implementation Overview
* \{
********************************************************************************
*
* The anatomy of the secure image implementation consists of the following parts.
*
* \section group_secure_image_secureboot Secure Boot
*
* The secure boot process entails preserving the chain of trust from the root
* of trust (Cypress ROM code) to the user application. When the device is
* transitioned to SECURE lifecycle, the ROM boot will verify the validity
* of Flash boot. The Flash boot code will then validate the first user
* application (secure_image) defined in TOC2. The secure image is then used
* to validate the second user application (user_app0) in TOC2. 
*
* The validation of both first and second user applications use the
* same encryption/decryption scheme (RSASSA-2048). Note that the security method
* for validating the second image does not need to be RSASSA-2048. It is only
* required for the first user application. 
*
* \section group_secure_image_syscall System Calls
*
* System calls are special routines that execute out of ROM. These requests must
* be made via IPC notifications to the CM0+ core. A secure device may need to limit
* the allowed requests and therefore a system call parsing routine is included
* in the secure image. The system call routine is structured to execute in an ISR
* but it can also be readily moved out to an RTOS task. Few notes on the implementation:
*
* 1. System calls that may potentially be dangerous in a secure device are disallowed.
* 2. System calls that are Cypress internal or irrelevant to the application are disallowed.
* 3. IPC_STRUCT.DATA is checked against a user-defined blacklist to determine whether the
*    request is acceptable. Additionally some requests check against the client's 
*    protection context value.
*
* \subsection group_secure_image_flash Notes on Flash Erase/Program/Write
*
* The secure image does not support non-blocking Flash erase, program and write requests.
* These requests rely on complex NMI connections executed out of ROM. However note that
* in the secure image, the CM0+ is interruptible during the "blocking" operation as it
* is not executed out of the NMI. Therefore it is not truly blocking. The "user_app0"
* project's build settings include a preprocessor macro named CY_FLASH_RWW_DRV_SUPPORT_DISABLED
* to disable the non-blocking Flash write.
*
* To use Flash operations with the System Call routine in the secure image, there are 
* two methods of initiating a blocking operation:
*
* 1. Initiate a blocking erase/program/write operation from the CM4 core. This will block
*    the CM4 and the CM0+ cores, but the CM0+ will be interruptible. If this is the desired
*    mode of operation, then the ISR servicing the CM0+ interrupt should be executed out
*    of SRAM. Reads and execution from Flash is not allowed during the Flash erase/program/write
*    operations.
* 2. Initiate a blocking erase/program/write operation from the CM0+ core. This will only
*    block the CM0+ core but it will still be interruptible. As with method#1, any
*    ISR servicing an interrupt during this operation must execute out of SRAM. Additionally
*    the CM4 should not read or execute out of Flash during this time. Recommendation is to
*    use an IPC to signal the CM4 of the CM0+ core's intention to perform a Flash 
*    modification operation. This can allow the CM4 to transition execution to SRAM or take
*    other appropriate measures to not read or execute out of Flash.
*
* For additional information on the Flash erase, program and write operations, refer to the
* Flash driver documentation and the TRM. 
*
* If the non-blocking Flash operations are critical for the application, the System Call routine
* in the secure image can be performed in another proxy layer. For example another IPC channel
* can be used  by the CM4 to request the CM0+ to perform system calls. This request can be
* examined by the CM0+ core (as in the System Call routine in the secure image) and then the
* request can be initiated by the CM0+ core by calling the appropriate API functions (e.g. Flash
* driver functions). In this system, the IPC channel that would have been directly used by 
* the CM4 to perform Flash writes would be protected to only allow PC=0 to write to them. 
* Hence the CM4 would not have direct access to System Calls.
*
* \section group_secure_image_crypto Crypto Server
*
* The Crypto server and the entire Crypto library are included in the secure image.
* This provides a way for non-secure bus masters to request the secure image to perform
* Crypto operations in a secure environment via IPC notifications. The crypto routine
* is structured to execute in an ISR but it can also be readily moved out to an RTOS task.
* Note that the IPC_STRUCT.DATA is examined against a blacklist to determine if the request
* is acceptable.
*
* \section group_secure_image_prot Protection Units
*
* Protection unit settings are implemented to support the security policy described
* in the general overview section. Few notes on the implementation:
*
* 1. Only CM0+ and the Crypto cores are allowed to operate at PC=0. These should never
*    transition to PC!=0 values. DW0 and DW1 bus masters inherit the PC value of the
*    bus master that configured them. It is not recommended to use the CM0+ to configure
*    these as they would then operate at PC=0.
* 2. SMPU slaves are configured to protect sensitive memory regions and only allow
*    PC=0 bus masters to access them. All invalid accesses will generate bus faults.
* 3. PPU slaves are configured to protect sensitive peripheral MMIO registers. 
*    Depending on the use-case, these are either protected to only allow PC=0 bus
*    masters to access them or give read-only privileges to PC!=0 bus masters.
* 4. All SMPU and Programmable PPU master structs are enabled to only allow PC=0 bus
*    masters to reconfigure them. This is to prevent unconfigured slave structs to be
*    used for denial-of-service attacks by compromised down-stream applications.
* 5. Master structs of PPUs that have higher priority than the configured PPUs are
*    enabled to only allow PC=0 bus masters to reconfigure them. This is to prevent
*    the slave structs from being reconfigured by compromised applications, which 
*    can potentially lead to protection settings override.
* 6. Relevant IPC-related protection units are configured to allow all accesses.
*    These are meant to serve as a pointer to what is required by the end user.
*    These should be reconfigured to suit the IPC protection strategy that suits
*    the user-defined security application.
* 7. The following slave structs are reserved and used by SROM and FLASHBOOT to 
*    protect key resources. The restrictions are configured to only allow PC=0 
*    bus masters to have access.
*
*    - SMPU 15: Read/write restriction for ROM private stack.
*    - SMPU 14: Read/write restriction for ROM region.
*    - PROG PPU 15: Write restriction for CPUSS AP_CTL, PROTECTION,
*                   CM0_NMI_CTL, DP_CTL and MBIST_CTL registers.
*    - PROG PPU 14: Read/write restriction for CPUSS WOUNDING and
*                   CM0_PC0_HANDLER registers.
*    - PROG PPU 13: Write restriction for FlashC FM_CTL.BOOKMARK
*                   register.
*    - PROG PPU 12: Read/write restriction for EFUSE region 
*                   (excluding CUSTOMER_DATA).
*    - PROG PPU 11: Write restriction for IPC 0, 1 and 2 during
*                   system calls.
*    - PROG PPU 10: Read/write restriction for Crypto during
*                   system calls that use crypto operations.
*    - PROG PPU 9:  Read/write restriction for FM_CTL registers.
*
* \}
*
********************************************************************************
* \page page_secure_image_ipc Inter-Processor Communication (IPC)
* \{
********************************************************************************
*
* The IPC_STRUCT, IPC_INTR_STRUCT and the data region referenced in 
* IPC_STRUCT.DATA are intentionally left unprotected by the secure image as
* the protection scheme is entirely application specific. These regions need to
* be accessible by bus masters with specific access settings and hence the
* strategy in protecting the IPC channel and ensuring data integrity need to be
* driven from the client software. As a guide, there are two general methods 
* that can be used for ensuring secure access of these registers and memory 
* regions.
*
* \section group_secure_image_ipc1 IPC Protection Method 1
*
* The client application has a framework responsible for setting the PC of 
* the non-secure bus master (e.g. CM4) to a PC!=0 value. For example, a system
* can be constructed using a combination of protection units and a mechanism
* to allow only privileged code to make changes to the PC value. Then the 
* privileged code can protect the IPC_STRUCT, IPC_INTR_STRUCT and IPC data to
* allow only a certain PC value to access them (e.g. PC=2). Once this 
* configuration is in place, IPC requests can only be made by bus masters
* that have the same access settings (e.g. PC=2, privileged). PC=0 bus masters
* (e.g. CM0+) have full access to all resources and can read/write to those
* IPC registers and memory regions. Once the channel is released, the 
* chosen process (PC=2) on the user application can optionally disable the
* protection unit to allow access by other processes. 
* The advantage of this method is that it is relatively simple and the user
* application has full control over the register and memory protection
* settings. The disadvantage is that if the PC switching is compromised
* in the software, or if another master can be configured to run at the 
* chosen PC value for IPC access, then the security is compromised.
*
* \section group_secure_image_ipc2 IPC Protection Method 2
*
* The client user application can enable protection units to protect the
* IPC_STRUCT, IPC_INTR_STRUCT and IPC_STRUCT.DATA regions at PC=0. This 
* would effectively lock out any bus master from accessing (allow reads but
* disallow writes) the content. The only entity allowed to access it then
* would be the PC=0 task in the Secure image. 
* The advantage of this method is that no bus master can access the registers
* and memory once the protection units are enabled. The disadvantage is that
* in the case of IPC_STRUCT and IPC_INTR_STRUCT registers, the protection 
* units must be enabled after initiating the IPC request and would lead to 
* coherency problems in a multi-bus master system. For example there is a 
* chance that the IPC_STRUCT can be hijacked during the time between the IPC
* request and the protection unit enable. The protection unit must also be 
* enabled in a critical section with the interrupts disabled. 
* However, protecting the memory region accessed by IPC_STRUCT.DATA does not
* have this problem and may be a more robust method than option 1. The secure
* task can check whether the protection unit was enabled before executing 
* the routine and then disable the protection unit after releasing the IPC. 
* This way, the data integrity is guaranteed. 
*
* \section group_secure_image_ipc_guard Additional Guard
*
* Protecting the integrity of the IPC is an important design consideration as
* it is a conduit for passing commands to the secure (PC=0) core.
* Another effort to ensure this is for secure tasks to be updated to provide 
* non-repudiation by checking for the validity of the bus master access that 
* acquired the IPC lock (cross-check that IPC_STRUCT.ACQUIRE is as expected)
* and that any protection units protecting the IPC data are properly configured
* and enabled.
*
* In order to ensure that the client application is passing the acceptable
* parameters along with the issued commands, the contents of the IPC_STRUCT.DATA
* are examined and checked against a blacklist. This blacklist is used to
* confirm that memory addresses referenced in these parameters are not in the
* disallowed (secure) memory. E.g. a Crypto memory copy operation should not be
* able to move secure key in Flash to a RAM buffer. However this system relies
* on the validity of the IPC data and that it is unchanged during the secure
* task execution. Therefore the client software must protect it when issuing
* the command.
*
* \}
*
********************************************************************************
* \page page_secure_image_linker_scripts Linker Scripts
* \{
********************************************************************************
* 
* Custom linker scripts are used for the CM0+ secure image and the sample
* user application CM0+ and CM4 images. These are modified versions of the
* default linker scripts provided by the PDL. Linker scripts for the three
* supported toolchains GCC (.ld), ARMCC (.scat) and IAR (.icf) are provided. 
*
* \section group_secure_image_link_secure Secure Image Linker Script
*
* The specifics of the secure image (CM0+ only) are as follows:
*
* - Starting Flash address of the secure application is located at the start of
*   the Flash region 0x10000000. The application object is 0x10000 Bytes.
* - The public key (sflash_public_key), TOC2 (sflash_toc2) and RTOC2 (sflash_rtoc2)
*   are populated in the secure image.
* - Secure image RAM is located at the start of the SRAM region 0x08000000 and its
*   size is 0x2000 Bytes.
* - The secure image application header (cy_app_header) is located at the start
*   of the application Flash (0x10000000) and the application signature 
*   (cy_app_signature) is located at the end of the application Flash (0x1000FF00).
* - The main stack is located an the last 4K of RAM (0x08001000 - 0x08001FFF).
*
* \section group_secure_image_link_user User App Linker Script
*
* The user application has custom linkers scripts for both the CM0+ core and
* the CM4 core. The specifics of the CM0+ linker script are as follows:
*
* - The CM0+ application Flash is located at 0x10020000 - 0x1002FFFF.
* - The CM0+ RAM is located at 0x08010000 and is 0x10000 Bytes.
* - The application object (CM0+ and CM4) header is located at the beginning
*   of the image Flash.
* - The application digital signature is defined in the CM4 linker script as it
*   needs to be located at the end of the application object as defined in the
*   application header.
*
* The specifics of the CM4 linker script are as follows:
*
* - The CM4 application Flash is located at 0x10080000 - 0x1008FFFF.
* - The CM4 RAM is located at 0x08020000 and is 0x10000 Bytes.
* - The application object (CM0+ and CM4) digital signature is located at the 
*   end of the image Flash.
* - The application digital signature is generated by defining two symbols:
*   __cy_app_verify_start (0x10020000) and __cy_app_verify_length (0x0006ff00).
*   This instructs cymcuelftool to begin the signature calculation from the
*   location defined at __cy_app_verify_start up to the address offset defined
*   by __cy_app_verify_length.
*
* \section group_secure_image_elf Using cymcuelftool 
* 
* The cymcuelftool is used in this design for three purposes:
*
* - Generate the assembly file containing the export symbols using the --codeshare
*   (-R) command.
* - Populate the footer/signature region with the application digital signature.
*   This is accomplished by using the --sign (-S) command.
* - Merge the (secure image CM0+, user app CM0+ and CM4) applications. This is 
*   accomplished by using the --merge (-M) command.
*
* The cymcuelftool relies on correct symbols and sections to be defined in the
* generated elf file in order to be able to correctly execute the --sign command.
*
* - The section "cy_app_header" must be defined.
* - The "cy_app_signature" symbol must be defined to generate the digital signature.
* - The symbols "__cy_app_verify_start" and "__cy_app_verify_length" must be used
*   to define the start and length of the memory region to calculate the digital signature.
*
* \}
*
********************************************************************************
* \page page_secure_image_efuse EFUSE Configuration
* \{
********************************************************************************
*
* Sample EFUSE configuration is included in this SDK and is also provided as a
* PSoC Creator project, "efuse_gen". This is meant to be a CM0+ only application
* that does not have any programmable regions except for a special region meant
* for the EFUSE data. The source file contains the following macro to control the
* inclusion/exclusion of EFUSE data.
* 
* - CY_SI_EFUSE_AVAILABLE: Set this to 1 if the EFUSE is ready to be integrated and
*   the application is ready to transition the device to "SECURE with Debug" mode. 
*   This macro is 0 by default as a safety net to prevent inadvertent programming.
*
* Note that programming EFUSE regions must be performed using PSoC Programmer 
* and cannot be done using PSoC Creator. Refer to the EFUSE programming steps
* in the TRM for detailed information. In summary, follow these steps to write into
* the EFUSE region.
*
* 1. Ensure to first program the device with the secure_image and user_app0
*    applications.
* 2. Open the efuse_gen project in PSoC Creator and build it. Then close PSoC Creator.
* 3. Using PSoC Programmer, navigate to the ./example/efuse_gen.cydsn/CortexM0p
*    directory and find efuse_gen.hex. 
* 4. In PSoC Programmer options, enable the "Chip Lock" option and disable the 
*    "Erase Flash" option.
* 5. Ensure that the program voltage is at 2.5V.
* 6. Press the Program button and click OK when the warning dialogs appear.
* 7. Observe a message from PSoC Programmer saying that the EFUSE region has
*    been successfully programmed.
* 8. Attach a debugger and observe that CPUSS->PROTECTION reads 0x03 (i.e.
*    SECURE lifecycle but debug enabled - aka "SECURE with Debug").
*
********************************************************************************
* \page page_secure_image_modify Modification Guidelines
* \{
********************************************************************************
*
* There are several considerations that should be adhered to when modifying the 
* secure image reference design.
*
* - Establish the build environment and check that the design operates
*   successfully in it before attempting to modify the design.
* - Ensure that the linker script and addresses used/referenced in the design
*   are synchronized and build up the design in stages.
* - Flash boot validation will fail for various errors in your design. Attach
*   a debugger and examine the value located at the RAM location pointed to by
*   CY_SI_FLASHBOOT_ERROR_ADDR. The \ref group_secure_image_flashboot_error_macros
*   will show the source of the error in the validation process. 
* - Have a method or a hex file that resets the SFlash regions in order to return
*   to the default state. This can be accomplished by using PSoC Programmer's
*   Erase all Flash command and specifying the SFlash region in the memory
*   types window.
* - The CM0+ core is the secure core and should always operate at PC=0. All
*   CM0+ applications should then be considered as an extension of the security
*   policy and has full access to all protected resources.
* - The CM4 to CM0+ communication via IPC should be considered as a security 
*   vulnerability and adequate steps need to be taken to close it. See
*   \ref page_secure_image_ipc.
*
* \}
*
*/

#include "syslib/cy_syslib.h"
#include "ipc/cy_ipc_drv.h"
#include "crypto/cy_crypto.h"
#include "cy_si_config.h"


/***************************************
*     TOC2 and RTOC2
***************************************/

/** Flashboot parameters */
#define CY_SI_FLASHBOOT_FLAGS ((CY_SI_FLASHBOOT_VALIDATE_YES << CY_SI_TOC_FLAGS_APP_VERIFY_POS) \
                                | (CY_SI_FLASHBOOT_WAIT_20MS << CY_SI_TOC_FLAGS_DELAY_POS) \
                                | (CY_SI_FLASHBOOT_CLK_25MHZ << CY_SI_TOC_FLAGS_CLOCKS_POS))

/** TOC2 in SFlash */
CY_SECTION(".cy_toc_part2") __USED static const cy_stc_si_toc_t cy_toc2 =
{
    .objSize     = sizeof(cy_stc_si_toc_t) - sizeof(uint32_t),  /**< Object Size (Bytes) excluding CRC */
    .magicNum    = CY_SI_TOC2_MAGICNUMBER,                      /**< TOC2 ID (magic number) */
    .userKeyAddr = (uint32_t)&CySecureKeyStorage,               /**< User key storage address */
    .smifCfgAddr = 0UL,                                         /**< SMIF config list pointer */
    .appAddr1    = CY_SI_SECURE_FLASH_BEGIN,                    /**< App1 (secure image) start address */
    .appFormat1  = CY_SI_APP_FORMAT_CYPRESS,                    /**< App1 Format */
    .appAddr2    = CY_SI_USERAPP_FLASH_BEGIN,                   /**< App2 (user app) start address */
    .appFormat2  = CY_SI_APP_FORMAT_CYPRESS,                    /**< App2 Format */
    .shashObj    = 1UL,                                         /**< Include public key in the SECURE HASH */
    .sigKeyAddr  = (uint32_t)&SFLASH->PUBLIC_KEY,               /**< Address of signature verification key */
    .tocFlags    = CY_SI_FLASHBOOT_FLAGS,                       /**< Flashboot flags stored in TOC2 */
    .crc         = 0UL                                          /**< CRC populated by cymcuelftool */
};

/** RTOC2 in SFlash */
CY_SECTION(".cy_rtoc_part2") __USED static const cy_stc_si_toc_t cy_rtoc2 =
{
    .objSize     = sizeof(cy_stc_si_toc_t) - sizeof(uint32_t),  /**< Object Size (Bytes) excluding CRC */
    .magicNum    = CY_SI_TOC2_MAGICNUMBER,                      /**< TOC2 ID (magic number) */
    .userKeyAddr = (uint32_t)&CySecureKeyStorage,               /**< User key storage address */
    .smifCfgAddr = 0UL,                                         /**< SMIF config list pointer */
    .appAddr1    = CY_SI_SECURE_FLASH_BEGIN,                    /**< App1 (secure image) start address */
    .appFormat1  = CY_SI_APP_FORMAT_CYPRESS,                    /**< App1 Format */
    .appAddr2    = CY_SI_USERAPP_FLASH_BEGIN,                   /**< App2 (user app) start address */
    .appFormat2  = CY_SI_APP_FORMAT_CYPRESS,                    /**< App2 Format */
    .shashObj    = 1UL,                                         /**< Include public key in the SECURE HASH */
    .sigKeyAddr  = (uint32_t)&SFLASH->PUBLIC_KEY,               /**< Address of signature verification key */
    .tocFlags    = CY_SI_FLASHBOOT_FLAGS,                       /**< Flashboot flags stored in TOC2 */
    .crc         = 0UL                                          /**< CRC populated by cymcuelftool */
};


/***************************************
*   Application header and signature
***************************************/

#define CY_SI_VT_OFFSET     ((uint32_t)(&__Vectors[0]) - CY_SI_SECURE_FLASH_BEGIN \
                                - offsetof(cy_stc_si_appheader_t, core0Vt)) /**< CM0+ VT Offset */
#define CY_SI_CPUID         (0xC6000000UL)          /**< CM0+ ARM CPUID[15:4] Reg shifted to [31:20] */
#define CY_SI_CORE_IDX      (0UL)                   /**< Index ID of the CM0+ core */

/** Secure Application header */
CY_SECTION(".cy_app_header") __USED 
static const cy_stc_si_appheader_t cy_si_appHeader = {
    .objSize        = CY_SI_SECURE_FLASH_END - CY_SI_SECURE_FLASH_BEGIN - CY_SI_SECURE_DIGSIG_SIZE,
    .appId          = (CY_SI_APP_VERSION | CY_SI_APP_ID_SECUREIMG),
    .appAttributes  = 0UL,                          /* Reserved */
    .numCores       = 1UL,                          /* Only CM0+ */
    .core0Vt        = CY_SI_VT_OFFSET,              /* CM0+ VT offset */
    .core0Id        = CY_SI_CPUID | CY_SI_CORE_IDX, /* CM0+ core ID */
};

/** Secure Image Digital signature (Populated by cymcuelftool) */
CY_SECTION(".cy_app_signature") __USED CY_ALIGN(4) 
static const uint8_t cy_si_appSignature[CY_SI_SECURE_DIGSIG_SIZE] = {0u};


/***************************************
*           Globals
***************************************/
static uint32_t syscall_intrPending = 0UL;              /**< SysCall notify interrupt tracking variable */
static uint32_t crypto_intrPending = 0UL;               /**< Crypto notify interrupt tracking variable */
static cy_en_si_status_t errorStatus = CY_SI_SUCCESS;   /**< Secure image error status */


/***************************************
*           Functions
***************************************/

/**
* \addtogroup group_secure_image_functions_tasks
* \{
*/
void Cy_SI_ErrorTask(void);
void Cy_SI_SysCallTask(void);
void Cy_SI_CryptoTask(void);
/** \} group_secure_image_functions_tasks */

/*******************************************************************************
* Function Name: Cy_SI_UserApp
****************************************************************************//**
* 
* \brief Sets the stack pointer and jumps to the reset vector of the User App.
*
* \param stackPtr 
* Stack pointer of the application.
*
* \param rstAddr
* Address of the reset vector.
*
*******************************************************************************/
static void Cy_SI_UserApp(uint32_t stackPtr, uint32_t rstAddr);

#if defined (__ARMCC_VERSION)
    static __asm void Cy_SI_UserApp(uint32_t stackPtr, uint32_t rstAddr)
    {
        MSR MSP, R0
        BX  R1
        ALIGN
    }
#elif defined(__GNUC__) 
    static void Cy_SI_UserApp(uint32_t stackPtr, uint32_t rstAddr)
    {
        __asm volatile("    MSR msp, %[sp]\n"
                       "    BX %[address] \n" : : [sp]"r"(stackPtr), [address]"r"(rstAddr) : "sp", "memory");
    }
#elif defined(__ICCARM__)
    #pragma optimize=no_inline
    static void Cy_SI_UserApp(uint32_t stackPtr, uint32_t rstAddr)
    {
        __asm volatile("MSR msp, R0\n"
                       "BX  R1     \n" );
    }
#endif  /* (__ARMCC_VERSION) */


/*******************************************************************************
* Function Name: NvicMux0_IRQHandler
****************************************************************************//**
*
* \brief IPC channel notify ISR for SysCall.
*
* Clears the IPC interrupt and updates the syscall_intrPending global variable.
* Cy_SI_SysCallTask() is called at the end of the routine, which services the
* SysCall request.
*
*******************************************************************************/
void NvicMux0_IRQHandler(void)
{
    uint8_t  idx = 0u;
    uint32_t intrReg;

    intrReg = Cy_IPC_Drv_GetInterruptStatusMasked(Cy_IPC_Drv_GetIntrBaseAddr(CY_SI_SYSCALL_INTR_STRUCT))
                >> CY_SI_IPC_NOTIFY_SHIFT;
    
    while (idx <= CY_SI_SYSCALL_MAX)
    {
        if((intrReg & CY_SI_SYSCALL_IPC_INTR_MASK) != CY_SI_NO_IRQ)
        {   
            syscall_intrPending |= (CY_SI_IRQ_MASK << idx); 
            /* Clear the interrupt */
            Cy_IPC_Drv_ClearInterrupt(Cy_IPC_Drv_GetIntrBaseAddr(CY_SI_SYSCALL_INTR_STRUCT), 
                                        CY_SI_NO_IRQ, (CY_SI_IRQ_MASK << idx));
        }
        intrReg = intrReg >> 1u;
        idx++;
    }
    
    /* Note: This function call can be removed and alternatively be invoked in an RTOS task */
    if(syscall_intrPending != 0UL)
    {
        Cy_SI_SysCallTask();
    }
}


/*******************************************************************************
* Function Name: NvicMux2_IRQHandler
****************************************************************************//**
*
* \brief IPC channel notify ISR for Crypto.
*
* Clears the IPC interrupt and updates the crypto_intrPending global variable.
* Cy_SI_CryptoTask() is called at the end of the routine, which services the
* Crypto request.
*
*******************************************************************************/
void NvicMux2_IRQHandler(void)
{
    uint32_t intrReg;
    
    intrReg = Cy_IPC_Drv_GetInterruptStatusMasked(Cy_IPC_Drv_GetIntrBaseAddr(CY_CRYPTO_IPC_INTR_NOTIFY_NUM))
                >> CY_SI_IPC_NOTIFY_SHIFT;

    if (intrReg != CY_SI_NO_IRQ)
    {
        Cy_IPC_Drv_ClearInterrupt(Cy_IPC_Drv_GetIntrBaseAddr(CY_CRYPTO_IPC_INTR_NOTIFY_NUM),
                                CY_SI_NO_IRQ, intrReg);
        crypto_intrPending |= CY_SI_IRQ_MASK;
    }
    
    /* Note: This function call can be removed and alternatively be invoked in an RTOS task */
    if(crypto_intrPending != 0UL)
    {
        Cy_SI_CryptoTask();
    }
}


/*******************************************************************************
* Function Name: NvicMux31_IRQHandler
****************************************************************************//**
*
* \brief Crypto core error ISR.
*
* Retrieves the Crypto error cause and then invokes Cy_Crypto_Server_ErrorHandler()
* to clear the interrupt.
*
*******************************************************************************/
void NvicMux31_IRQHandler(void)
{
    cy_stc_crypto_hw_error_t* cryptoErrorCause = NULL;
    
    (void)Cy_Crypto_GetErrorStatus(cryptoErrorCause);
    
    /* Insert Crypto error handling routine */
    
    /* Clear the interrupt source and reset the Crypto server state machine */
    Cy_Crypto_Server_ErrorHandler();
}


/*******************************************************************************
* Function Name: Cy_SI_ErrorTask
****************************************************************************//**
*
* \brief Catch-all unrecoverable error handler task.
*
* Note that invalid bus-accesses related to PC!=0 access go the the hardfault
* handler. A fault structure (FAULT_STRUCT0, FAULT_STRUCT1) can optionally be
* used to determine the source of the offending access.
*
*******************************************************************************/
CY_NOINLINE void Cy_SI_ErrorTask(void)
{   
    while(1)
    {
        /* Check if the lifecycle is in SECURE mode */
        if(CY_SI_SECURE_LIFECYCLE == CPUSS->PROTECTION)
        {
            /* Error handling task should not be interrupted */
            __disable_irq();
            
            /* Perform specific error handling - Expand as necessary */
            switch(errorStatus)
            {
                case CY_SI_INVALID_FORMAT:
                {
                    /* Secure boot is compromised - Transition to dead state */
                    Cy_ROM_EnterDeadState();
                    break;
                }
                case CY_SI_INVALID_TOC:
                {
                    /* Secure boot is compromised - Transition to dead state */
                    Cy_ROM_EnterDeadState();
                    break;
                }
                case CY_SI_INVALID_KEY:
                {
                    /* Secure boot is compromised - Transition to dead state */
                    Cy_ROM_EnterDeadState();
                    break;
                }
                case CY_SI_INVALID_IMAGE:
                {
                    /* Secure boot is compromised - Transition to dead state */
                    Cy_ROM_EnterDeadState();
                    break;
                }
                default:
                {
                    Cy_SysLib_Halt((uint32_t)errorStatus);
                    break;
                }
            }
        }
    }
}


/*******************************************************************************
* Function Name: Cy_SI_SysCallTask
****************************************************************************//**
*
* \brief Interprets the SysCall requests and applies the pre-processing and 
* boundary checks. If successful, the ROM Handler is invoked to perform the 
* appropriate syscall routines.
*
* The ROM Handler reads the INTR_MASKED register to determine if there's a 
* SysCall request. Requests must be serialized to account for the opcodes that
* exit out of PC=0 from the ROM. The SysCall interrupt is temporarily disabled 
* to allow requests from other IPC_STRUCTs to continue being registered, while
* at the same time allowing the serialization to work as intended.
* 
* \note 
* Pre-processing and checks are not applied to the Test Controller (TC)/DAP
* as a number of opcodes are needed for the debugger during NORMAL mode access. 
* The TC is also the only mechanism to transition the device to SECURE Mode. 
* Once the transition is complete, the TC will no longer be available for use 
* and hence all SysCalls through it will be locked out.
*
* \note 
* It is recommended that the IPC_STRUCT, IPC_INTR_STRUCT and the memory 
* region pointed to in the IPC_STRUCT.DATA register be protected using
* protection units.
*
* \note
* The following opcodes exit out of PC=0 when the requests come from PC!=0
* bus masters. They are either disallowed altogether or restricted to PC=0
* bus masters.
* - CY_SI_OPCODE_READ_EFUSE_BYTE
* - CY_SI_OPCODE_CHECKSUM
* - CY_SI_OPCODE_COMPUTE_HASH
* - CY_SI_OPCODE_CONFIG_REGION_BULK
* - CY_SI_OPCODE_DIRECT_EXECUTE
*
*******************************************************************************/
CY_NOINLINE void Cy_SI_SysCallTask(void)
{   
    cy_en_si_syscall_opcode_t opcodeReq;
    cy_en_si_status_t errorSysCall;
    cy_stc_si_syscall_t *sysCallData;
    uint32_t ipcIntrReg;
    uint32_t ipcIntrMask;
    uint32_t idx;
    bool     allowSysCall;
    
    idx = CY_SI_SYSCALL_RESET;
    ipcIntrMask = Cy_IPC_Drv_GetInterruptMask(Cy_IPC_Drv_GetIntrBaseAddr(CY_SI_SYSCALL_INTR_STRUCT))
                        >> CY_SI_IPC_NOTIFY_SHIFT;
    
    while(syscall_intrPending != CY_SI_NO_IRQ)
    {
        ipcIntrReg = syscall_intrPending;
        ipcIntrReg = ipcIntrReg >> idx;
        
        /* Only enable 1 request as each need to start running at PC=0 */
        if((ipcIntrReg & CY_SI_SYSCALL_IPC_INTR_MASK) != CY_SI_NO_IRQ)
        {   
            allowSysCall = true;
            errorSysCall = CY_SI_SUCCESS;

            /* Preprocess the SysCall request and apply boundary checks (ignored for TC/DAP) */
            if(idx != CY_SI_SYSCALL_MAX)
            {                
                /* Load the message pointer into the internal working RAM (sysCallData) */
                (void)Cy_IPC_Drv_ReadMsgPtr(Cy_IPC_Drv_GetIpcBaseAddress(idx), (void**)&sysCallData);
                
                /* Get opcode based on index */
                opcodeReq = (cy_en_si_syscall_opcode_t)(sysCallData->opcode >> CY_SI_SYSCALL_OPCODE_SHIFT);
            
                /* Formating and validation check */
                switch(opcodeReq)
                {
                    case CY_SI_OPCODE_SILICON_ID:
                    {
                        break;
                    }
                    case CY_SI_OPCODE_BLOW_EFUSE_BIT:
                    {
                        /* Do not allow */
                        errorSysCall = CY_SI_SYSCALL_DISALLOWED;
                        allowSysCall = false;
                        break;
                    }
                    case CY_SI_OPCODE_READ_EFUSE_BYTE:
                    {
                        /* Do not allow */
                        errorSysCall = CY_SI_SYSCALL_DISALLOWED;
                        allowSysCall = false;
                        break;
                    }
                    case CY_SI_OPCODE_LOAD_FLASH_PAGE:
                    {
                        /* Check that the source address is not in protected RAM region */
                        errorSysCall = Cy_SI_CheckMemRange(
                                        sysCallData->arg2,
                                        sysCallData->arg2 + CY_SI_SYSCALL_ROWSIZE,
                                        CY_SI_MEM_TYPE_RAM_MASK);
                        allowSysCall = (errorSysCall != CY_SI_SUCCESS) ? false : true;
                        break;
                    }
                    case CY_SI_OPCODE_WRITE_ROW:
                    {
                        /* Disallow non-blocking calls */
                        if(0UL == (sysCallData->opcode & CY_SI_SYSCALL_BLOCKING))
                        {
                            errorSysCall = CY_SI_SYSCALL_DISALLOWED;
                            allowSysCall = false;
                            break;
                        }
                        
                        /* Check that the destination address is not in protected Flash regions */
                        errorSysCall = Cy_SI_CheckMemRange(
                                        sysCallData->arg2,
                                        sysCallData->arg2,
                                        CY_SI_MEM_TYPE_FLASH_DST_MASK);
                        allowSysCall = (errorSysCall != CY_SI_SUCCESS) ? false : true;
                        if (allowSysCall == false) 
                        {
                            break;
                        }
                        
                        /* Check that the source address is not in protected Flash/RAM region */
                        errorSysCall = Cy_SI_CheckMemRange(
                                        sysCallData->arg3,
                                        sysCallData->arg3 + CY_SI_SYSCALL_ROWSIZE,
                                        CY_SI_MEM_TYPE_FLASH_SRC_MASK | CY_SI_MEM_TYPE_RAM_MASK);
                        allowSysCall = (errorSysCall != CY_SI_SUCCESS) ? false : true;
                        break;
                    }
                    case CY_SI_OPCODE_PROGRAM_ROW:
                    {
                        /* Disallow non-blocking calls */
                        if(0UL == (sysCallData->opcode & CY_SI_SYSCALL_BLOCKING))
                        {
                            errorSysCall = CY_SI_SYSCALL_DISALLOWED;
                            allowSysCall = false;
                            break;
                        }
                        
                        /* Check that the destination address is not in protected Flash regions */
                        errorSysCall = Cy_SI_CheckMemRange(
                                        sysCallData->arg2,
                                        sysCallData->arg2,
                                        CY_SI_MEM_TYPE_FLASH_DST_MASK);
                        allowSysCall = (errorSysCall != CY_SI_SUCCESS) ? false : true;
                        if (allowSysCall == false) 
                        {
                            break;
                        }
                        
                        /* Check that the source address is not in protected Flash/RAM region */
                        errorSysCall = Cy_SI_CheckMemRange(
                                        sysCallData->arg3,
                                        sysCallData->arg3 + CY_SI_SYSCALL_ROWSIZE,
                                        CY_SI_MEM_TYPE_FLASH_SRC_MASK | CY_SI_MEM_TYPE_RAM_MASK);
                        allowSysCall = (errorSysCall != CY_SI_SUCCESS) ? false : true;
                        break;
                    }
                    case CY_SI_OPCODE_ERASE_ALL_FLASH:
                    {
                        /* Dangerous - Do not allow */
                        errorSysCall = CY_SI_SYSCALL_DISALLOWED;
                        allowSysCall = false;
                        break;
                    }
                    case CY_SI_OPCODE_CHECKSUM:
                    {
                        /* Only allow PC=0 to access this command */
                        if(_FLD2VAL(IPC_STRUCT_ACQUIRE_PC, ((IPC_STRUCT_Type*) &IPC->STRUCT[idx])->ACQUIRE) == 0UL)
                        {
                            uint32_t rowType = (uint32_t)((sysCallData->opcode & CY_SI_SYSCALL_MEMTYPE_MASK) 
                                                >> CY_SI_SYSCALL_MEMTYPE_SHIFT);
                            uint32_t rowBase = (rowType == CY_SI_SYSCALL_MEMTYPE_USER) ? CY_FLASH_BASE :
                                               ((rowType == CY_SI_SYSCALL_MEMTYPE_EMEEPROM) ? CY_EM_EEPROM_BASE :
                                                CY_SFLASH_BASE);
                            uint32_t rowAddr = rowBase + ((uint32_t)((sysCallData->opcode & CY_SI_SYSCALL_CHECKSUM_MASK) 
                                                >> CY_SI_SYSCALL_CHECKSUM_SHIFT) * CY_SI_SYSCALL_ROWSIZE);
                            
                            /* Check that the source address is not in protected Flash region */
                            errorSysCall = Cy_SI_CheckMemRange(
                                            rowAddr,
                                            rowAddr + CY_SI_SYSCALL_ROWSIZE,
                                            CY_SI_MEM_TYPE_FLASH_SRC_MASK);
                            allowSysCall = (errorSysCall != CY_SI_SUCCESS) ? false : true;
                        }
                        else
                        {
                            errorSysCall = CY_SI_SYSCALL_DISALLOWED;
                            allowSysCall = false;
                        }
                        break;
                    }
                    case CY_SI_OPCODE_COMPUTE_HASH:
                    {
                        /* Only allow PC=0 to access this command */
                        if(_FLD2VAL(IPC_STRUCT_ACQUIRE_PC, ((IPC_STRUCT_Type*) &IPC->STRUCT[idx])->ACQUIRE) == 0UL)
                        {
                            /* Check that the source address is not in protected Flash/RAM region */
                            errorSysCall = Cy_SI_CheckMemRange(
                                            sysCallData->arg1,
                                            sysCallData->arg1 + sysCallData->arg2,
                                            CY_SI_MEM_TYPE_FLASH_SRC_MASK | CY_SI_MEM_TYPE_RAM_MASK);
                            allowSysCall = (errorSysCall != CY_SI_SUCCESS) ? false : true;
                        }
                        else
                        {
                            errorSysCall = CY_SI_SYSCALL_DISALLOWED;
                            allowSysCall = false;
                        }
                        break;
                    }
                    case CY_SI_OPCODE_ERASE_SECTOR:
                    {
                        /* Do not allow */
                        errorSysCall = CY_SI_SYSCALL_DISALLOWED;
                        allowSysCall = false;
                        break;
                    }
                    case CY_SI_OPCODE_SOFT_RESET:
                    {
                        /* Do not allow */
                        errorSysCall = CY_SI_SYSCALL_DISALLOWED;
                        allowSysCall = false;
                        break;
                    }
                    case CY_SI_OPCODE_ERASE_ROW:
                    {
                        /* Disallow non-blocking calls */
                        if(0UL == (sysCallData->opcode & CY_SI_SYSCALL_BLOCKING))
                        {
                            errorSysCall = CY_SI_SYSCALL_DISALLOWED;
                            allowSysCall = false;
                            break;
                        }
                        
                        /* Check that the destination address is not in protected Flash regions */
                        errorSysCall = Cy_SI_CheckMemRange(
                                        sysCallData->arg1,
                                        sysCallData->arg1,
                                        CY_SI_MEM_TYPE_FLASH_DST_MASK);
                        allowSysCall = (errorSysCall != CY_SI_SUCCESS) ? false : true;
                        break;
                    }
                    case CY_SI_OPCODE_ERASE_SUBSECTOR:
                    {
                        /* Do not allow */
                        errorSysCall = CY_SI_SYSCALL_DISALLOWED;
                        allowSysCall = false;
                        break;
                    }
                    case CY_SI_OPCODE_READ_UNIQUE_ID:
                    {
                        break;
                    }
                    case CY_SI_OPCODE_FLASHC_VCC_CONFIG:
                    {
                        break;
                    }
                    default:
                    {
                        errorSysCall = CY_SI_SYSCALL_DISALLOWED;
                        allowSysCall = false;
                        break;
                    }
                }
            }
            
            /* Begin Critical section (necessary if not executing out of ISR) */
            NVIC_DisableIRQ(CY_SI_SYSCALL_INTR_NUMBER);
            
            if(allowSysCall)
            {
                /* Manually set the interrupt register */
                Cy_IPC_Drv_SetInterruptMask(Cy_IPC_Drv_GetIntrBaseAddr(CY_SI_SYSCALL_INTR_STRUCT), 
                                            CY_SI_NO_IRQ, (CY_SI_SYSCALL_IPC_INTR_MASK << idx));
                Cy_IPC_Drv_SetInterrupt(Cy_IPC_Drv_GetIntrBaseAddr(CY_SI_SYSCALL_INTR_STRUCT), 
                                            CY_SI_NO_IRQ, (CY_SI_IRQ_MASK << idx));
                
                /* Directly branch into the ROM Handler */
                Cy_ROM_Handler();
                
                /* Restore the notification mask */
                Cy_IPC_Drv_SetInterruptMask(Cy_IPC_Drv_GetIntrBaseAddr(CY_SI_SYSCALL_INTR_STRUCT), 
                                            CY_SI_NO_IRQ, ipcIntrMask);
            }
            else
            {   
                /* Enter the error code and release the channel */
                sysCallData->opcode = (uint32_t)errorSysCall;
                (void)Cy_IPC_Drv_LockRelease(Cy_IPC_Drv_GetIpcBaseAddress(idx), CY_SI_NO_IRQ);
            }
            
            /* Update the interrupt state */
            syscall_intrPending &= (uint32_t)(~(CY_SI_IRQ_MASK << idx));
            
            /* End critical section */
            NVIC_EnableIRQ(CY_SI_SYSCALL_INTR_NUMBER);
        }
        idx = (idx == (CY_SI_SYSCALL_MAX)) ? CY_SI_SYSCALL_RESET : (idx + 1u);
    }
}


/*******************************************************************************
* Function Name: Cy_SI_CryptoTask
****************************************************************************//**
*
* \brief Interprets the Crypto requests and applies pre-processing and checks. 
* If successful, the request is passed to the Crypto server processing to 
* execute the routine.
*
* Note: Crypto has access to all device resources and hence can be used to move
* sensitive data to unprotected regions. Therefore extensive address checking
* should be applied to ensure that the security is not compromised. It is also
* recommended that the IPC_STRUCT, IPC_INTR_STRUCT and the memory region 
* pointed to in the IPC_STRUCT.DATA register be protected using protection
* units.
*
*******************************************************************************/
CY_NOINLINE void Cy_SI_CryptoTask(void)
{
    cy_en_si_status_t errorCrypto;
    bool allowCrypto;
    
    /* Tracking variables for memory range check */
    static uint32_t aesKeyLength = 0UL;
    static uint32_t shaDigestLength = 0UL;
    
    if (crypto_intrPending != CY_SI_NO_IRQ)
    {
        /* Load the message pointer into the internal working RAM (processData) */
        (void)Cy_IPC_Drv_ReadMsgPtr(Cy_IPC_Drv_GetIpcBaseAddress(CY_IPC_CHAN_CRYPTO), (void**)&processData);

        allowCrypto = true;
        errorCrypto = CY_SI_SUCCESS;
        
        /* Check that the pointers are not referring to protected Flash/RAM region. Keys can be ignored */
        switch(processData->instr)
        {
            case CY_CRYPTO_INSTR_ENABLE:
            {
                break;
            }
            case CY_CRYPTO_INSTR_DISABLE:
            {
                break;
            }
            case CY_CRYPTO_INSTR_PRNG_INIT:
            {
                /* used params: lfsr32InitState,
                                lfsr31InitState,
                                lfsr29InitState */
                break;
            }
            case CY_CRYPTO_INSTR_PRNG:
            {
                /* used params: max,
                                *prngNum */
                errorCrypto = Cy_SI_CheckMemRange(
                                (uint32_t)(((cy_stc_crypto_context_prng_t *)processData->xdata)->prngNum),
                                (uint32_t)(((cy_stc_crypto_context_prng_t *)processData->xdata)->prngNum),
                                CY_SI_MEM_TYPE_RAM_MASK);
                allowCrypto = (errorCrypto != CY_SI_SUCCESS) ? false : true;
                break;
            }
            case CY_CRYPTO_INSTR_TRNG:
            {
                /* used params: GAROPol,
                                FIROPol,
                                max,
                                *trngNum */
                errorCrypto = Cy_SI_CheckMemRange(
                                (uint32_t)(((cy_stc_crypto_context_trng_t *)processData->xdata)->trngNum),
                                (uint32_t)(((cy_stc_crypto_context_trng_t *)processData->xdata)->trngNum),
                                CY_SI_MEM_TYPE_RAM_MASK);
                allowCrypto = (errorCrypto != CY_SI_SUCCESS) ? false : true;
                break;
            }
            case CY_CRYPTO_INSTR_AES_INIT:
            {
                /* used params: aesState->*key,
                                aesState->keyLength */
                
                /* Store key size information for validating other AES commands */
                aesKeyLength = (uint32_t)(((cy_stc_crypto_context_aes_t *)processData->xdata)->aesState.keyLength);
                break;
            }
            case CY_CRYPTO_INSTR_AES_ECB:
            {
                /* used params: dirMode,
                                *dst,
                                *src */
                errorCrypto = Cy_SI_CheckMemRange(
                                (uint32_t)(((cy_stc_crypto_context_aes_t *)processData->xdata)->dst),
                                (uint32_t)(((cy_stc_crypto_context_aes_t *)processData->xdata)->dst)
                                    + (uint32_t)CY_CRYPTO_AES_BLOCK_SIZE,
                                CY_SI_MEM_TYPE_FLASH_SRC_MASK | CY_SI_MEM_TYPE_RAM_MASK);
                allowCrypto = (errorCrypto != CY_SI_SUCCESS) ? false : true;
                if (allowCrypto == false) 
                {
                    break;
                }
                errorCrypto = Cy_SI_CheckMemRange(
                                (uint32_t)(((cy_stc_crypto_context_aes_t *)processData->xdata)->src),
                                (uint32_t)(((cy_stc_crypto_context_aes_t *)processData->xdata)->src)
                                    + (uint32_t)CY_CRYPTO_AES_BLOCK_SIZE,
                                CY_SI_MEM_TYPE_FLASH_SRC_MASK | CY_SI_MEM_TYPE_RAM_MASK);
                allowCrypto = (errorCrypto != CY_SI_SUCCESS) ? false : true;
                break;
            }
            case CY_CRYPTO_INSTR_AES_CBC:
            {
                /* used params: dirMode,
                                srcSize,
                                *ivPtr,
                                *dst,
                                *src */
                errorCrypto = Cy_SI_CheckMemRange(
                                (uint32_t)(((cy_stc_crypto_context_aes_t *)processData->xdata)->ivPtr),
                                (uint32_t)(((cy_stc_crypto_context_aes_t *)processData->xdata)->ivPtr)
                                    + aesKeyLength,
                                CY_SI_MEM_TYPE_FLASH_SRC_MASK | CY_SI_MEM_TYPE_RAM_MASK);
                allowCrypto = (errorCrypto != CY_SI_SUCCESS) ? false : true;
                if (allowCrypto == false) 
                {
                    break;
                }
                errorCrypto = Cy_SI_CheckMemRange(
                                (uint32_t)(((cy_stc_crypto_context_aes_t *)processData->xdata)->dst),
                                (uint32_t)(((cy_stc_crypto_context_aes_t *)processData->xdata)->dst)
                                    + (uint32_t)(((cy_stc_crypto_context_aes_t *)processData->xdata)->srcSize),
                                CY_SI_MEM_TYPE_FLASH_SRC_MASK | CY_SI_MEM_TYPE_RAM_MASK);
                allowCrypto = (errorCrypto != CY_SI_SUCCESS) ? false : true;
                if (allowCrypto == false) 
                {
                    break;
                }
                errorCrypto = Cy_SI_CheckMemRange(
                                (uint32_t)(((cy_stc_crypto_context_aes_t *)processData->xdata)->src),
                                (uint32_t)(((cy_stc_crypto_context_aes_t *)processData->xdata)->src)
                                    + (uint32_t)(((cy_stc_crypto_context_aes_t *)processData->xdata)->srcSize),
                                CY_SI_MEM_TYPE_FLASH_SRC_MASK | CY_SI_MEM_TYPE_RAM_MASK);
                allowCrypto = (errorCrypto != CY_SI_SUCCESS) ? false : true;
                break;
            }
            case CY_CRYPTO_INSTR_AES_CFB:
            {
                /* used params: dirMode,
                                srcSize,
                                *ivPtr,
                                *dst,
                                *src */
                errorCrypto = Cy_SI_CheckMemRange(
                                (uint32_t)(((cy_stc_crypto_context_aes_t *)processData->xdata)->ivPtr),
                                (uint32_t)(((cy_stc_crypto_context_aes_t *)processData->xdata)->ivPtr)
                                    + aesKeyLength,
                                CY_SI_MEM_TYPE_FLASH_SRC_MASK | CY_SI_MEM_TYPE_RAM_MASK);
                allowCrypto = (errorCrypto != CY_SI_SUCCESS) ? false : true;
                if (allowCrypto == false) 
                {
                    break;
                }
                errorCrypto = Cy_SI_CheckMemRange(
                                (uint32_t)(((cy_stc_crypto_context_aes_t *)processData->xdata)->dst),
                                (uint32_t)(((cy_stc_crypto_context_aes_t *)processData->xdata)->dst)
                                    + (uint32_t)(((cy_stc_crypto_context_aes_t *)processData->xdata)->srcSize),
                                CY_SI_MEM_TYPE_FLASH_SRC_MASK | CY_SI_MEM_TYPE_RAM_MASK);
                allowCrypto = (errorCrypto != CY_SI_SUCCESS) ? false : true;
                if (allowCrypto == false) 
                {
                    break;
                }
                errorCrypto = Cy_SI_CheckMemRange(
                                (uint32_t)(((cy_stc_crypto_context_aes_t *)processData->xdata)->src),
                                (uint32_t)(((cy_stc_crypto_context_aes_t *)processData->xdata)->src)
                                    + (uint32_t)(((cy_stc_crypto_context_aes_t *)processData->xdata)->srcSize),
                                CY_SI_MEM_TYPE_FLASH_SRC_MASK | CY_SI_MEM_TYPE_RAM_MASK);
                allowCrypto = (errorCrypto != CY_SI_SUCCESS) ? false : true;
                break;
            }
            case CY_CRYPTO_INSTR_AES_CTR:
            {
                /* used params:     dirMode,
                                    srcSize,
                                    *srcOffset,
                                    *ivPtr,
                                    *streamBlock,
                                    *dst,
                                    *src */
                errorCrypto = Cy_SI_CheckMemRange(
                                (uint32_t)(((cy_stc_crypto_context_aes_t *)processData->xdata)->srcOffset),
                                (uint32_t)(((cy_stc_crypto_context_aes_t *)processData->xdata)->srcOffset),
                                CY_SI_MEM_TYPE_FLASH_SRC_MASK | CY_SI_MEM_TYPE_RAM_MASK);
                allowCrypto = (errorCrypto != CY_SI_SUCCESS) ? false : true;
                if (allowCrypto == false) 
                {
                    break;
                }
                errorCrypto = Cy_SI_CheckMemRange(
                                (uint32_t)(((cy_stc_crypto_context_aes_t *)processData->xdata)->ivPtr),
                                (uint32_t)(((cy_stc_crypto_context_aes_t *)processData->xdata)->ivPtr)
                                    + aesKeyLength,
                                CY_SI_MEM_TYPE_FLASH_SRC_MASK | CY_SI_MEM_TYPE_RAM_MASK);
                allowCrypto = (errorCrypto != CY_SI_SUCCESS) ? false : true;
                if (allowCrypto == false) 
                {
                    break;
                }
                errorCrypto = Cy_SI_CheckMemRange(
                                (uint32_t)(((cy_stc_crypto_context_aes_t *)processData->xdata)->streamBlock),
                                (uint32_t)(((cy_stc_crypto_context_aes_t *)processData->xdata)->streamBlock)
                                    + (uint32_t)CY_CRYPTO_AES_BLOCK_SIZE,
                                CY_SI_MEM_TYPE_FLASH_SRC_MASK | CY_SI_MEM_TYPE_RAM_MASK);
                allowCrypto = (errorCrypto != CY_SI_SUCCESS) ? false : true;
                if (allowCrypto == false) 
                {
                    break;
                }
                errorCrypto = Cy_SI_CheckMemRange(
                                (uint32_t)(((cy_stc_crypto_context_aes_t *)processData->xdata)->dst),
                                (uint32_t)(((cy_stc_crypto_context_aes_t *)processData->xdata)->dst)
                                    + (uint32_t)(((cy_stc_crypto_context_aes_t *)processData->xdata)->srcSize),
                                CY_SI_MEM_TYPE_FLASH_SRC_MASK | CY_SI_MEM_TYPE_RAM_MASK);
                allowCrypto = (errorCrypto != CY_SI_SUCCESS) ? false : true;
                if (allowCrypto == false) 
                {
                    break;
                }
                errorCrypto = Cy_SI_CheckMemRange(
                                (uint32_t)(((cy_stc_crypto_context_aes_t *)processData->xdata)->src),
                                (uint32_t)(((cy_stc_crypto_context_aes_t *)processData->xdata)->src)
                                    + (uint32_t)(((cy_stc_crypto_context_aes_t *)processData->xdata)->srcSize),
                                CY_SI_MEM_TYPE_FLASH_SRC_MASK | CY_SI_MEM_TYPE_RAM_MASK);
                allowCrypto = (errorCrypto != CY_SI_SUCCESS) ? false : true;
                break;
            }
            case CY_CRYPTO_INSTR_CMAC:
            {
                /* used params:     srcSize,
                                    *dst,
                                    *src,
                                    *key,
                                    keyLength */
                errorCrypto = Cy_SI_CheckMemRange(
                                (uint32_t)(((cy_stc_crypto_context_aes_t *)processData->xdata)->dst),
                                (uint32_t)(((cy_stc_crypto_context_aes_t *)processData->xdata)->dst)
                                    + (uint32_t)CY_CRYPTO_AES_BLOCK_SIZE,
                                CY_SI_MEM_TYPE_FLASH_SRC_MASK | CY_SI_MEM_TYPE_RAM_MASK);
                allowCrypto = (errorCrypto != CY_SI_SUCCESS) ? false : true;
                if (allowCrypto == false) 
                {
                    break;
                }
                errorCrypto = Cy_SI_CheckMemRange(
                                (uint32_t)(((cy_stc_crypto_context_aes_t *)processData->xdata)->src),
                                (uint32_t)(((cy_stc_crypto_context_aes_t *)processData->xdata)->src)
                                    + (uint32_t)(((cy_stc_crypto_context_aes_t *)processData->xdata)->srcSize),
                                CY_SI_MEM_TYPE_FLASH_SRC_MASK | CY_SI_MEM_TYPE_RAM_MASK);
                allowCrypto = (errorCrypto != CY_SI_SUCCESS) ? false : true;
                break;
            }
            case CY_CRYPTO_INSTR_SHA:
            {
                /* used params:     *message,
                                    messageSize,
                                    *dst,
                                    mode */
                
                /* Determine the digest length */
                switch (((cy_stc_crypto_context_sha_t *)processData->xdata)->mode)
                {
                    case CY_CRYPTO_MODE_SHA1:
                    {
                        shaDigestLength = (uint32_t)CY_CRYPTO_SHA1_DIGEST_SIZE;
                        break;
                    }
                    case CY_CRYPTO_MODE_SHA224:
                    {
                        shaDigestLength = (uint32_t)CY_CRYPTO_SHA224_DIGEST_SIZE;
                        break;
                    }
                    case CY_CRYPTO_MODE_SHA256:
                    {
                        shaDigestLength = (uint32_t)CY_CRYPTO_SHA256_DIGEST_SIZE;
                        break;
                    }
                    case CY_CRYPTO_MODE_SHA384:
                    {
                        shaDigestLength = (uint32_t)CY_CRYPTO_SHA384_DIGEST_SIZE;
                        break;
                    }
                    case CY_CRYPTO_MODE_SHA512:
                    {
                        shaDigestLength = (uint32_t)CY_CRYPTO_SHA512_DIGEST_SIZE;
                        break;
                    }
                    case CY_CRYPTO_MODE_SHA512_256:
                    {
                        shaDigestLength = (uint32_t)CY_CRYPTO_SHA512_224_DIGEST_SIZE;
                        break;
                    }
                    case CY_CRYPTO_MODE_SHA512_224:
                    {
                        shaDigestLength = (uint32_t)CY_CRYPTO_SHA512_256_DIGEST_SIZE;
                        break;
                    }
                    default:
                    {
                        errorCrypto = CY_SI_BAD_PARAM;
                        break;
                    }
                }
                allowCrypto = (errorCrypto != CY_SI_SUCCESS) ? false : true;
                if (allowCrypto == false) 
                {
                    break;
                }
                errorCrypto = Cy_SI_CheckMemRange(
                                (uint32_t)(((cy_stc_crypto_context_sha_t *)processData->xdata)->message),
                                (uint32_t)(((cy_stc_crypto_context_sha_t *)processData->xdata)->message)
                                   + (uint32_t)(((cy_stc_crypto_context_sha_t *)processData->xdata)->messageSize),
                                CY_SI_MEM_TYPE_FLASH_SRC_MASK | CY_SI_MEM_TYPE_RAM_MASK);
                allowCrypto = (errorCrypto != CY_SI_SUCCESS) ? false : true;
                if (allowCrypto == false) 
                {
                    break;
                }
                errorCrypto = Cy_SI_CheckMemRange(
                                (uint32_t)(((cy_stc_crypto_context_sha_t *)processData->xdata)->dst),
                                (uint32_t)(((cy_stc_crypto_context_sha_t *)processData->xdata)->dst)
                                    + shaDigestLength,
                                CY_SI_MEM_TYPE_FLASH_SRC_MASK | CY_SI_MEM_TYPE_RAM_MASK);
                allowCrypto = (errorCrypto != CY_SI_SUCCESS) ? false : true;
                break;
            }
            case CY_CRYPTO_INSTR_HMAC:
            {
                /* used params:     *message,
                                    messageSize,
                                    *dst,
                                    mode,
                                    *key,
                                    keyLength */
                
                /* Determine the digest length */
                switch (((cy_stc_crypto_context_sha_t *)processData->xdata)->mode)
                {
                    case CY_CRYPTO_MODE_SHA1:
                    {
                        shaDigestLength = (uint32_t)CY_CRYPTO_SHA1_DIGEST_SIZE;
                        break;
                    }
                    case CY_CRYPTO_MODE_SHA224:
                    {
                        shaDigestLength = (uint32_t)CY_CRYPTO_SHA224_DIGEST_SIZE;
                        break;
                    }
                    case CY_CRYPTO_MODE_SHA256:
                    {
                        shaDigestLength = (uint32_t)CY_CRYPTO_SHA256_DIGEST_SIZE;
                        break;
                    }
                    case CY_CRYPTO_MODE_SHA384:
                    {
                        shaDigestLength = (uint32_t)CY_CRYPTO_SHA384_DIGEST_SIZE;
                        break;
                    }
                    case CY_CRYPTO_MODE_SHA512:
                    {
                        shaDigestLength = (uint32_t)CY_CRYPTO_SHA512_DIGEST_SIZE;
                        break;
                    }
                    case CY_CRYPTO_MODE_SHA512_256:
                    {
                        shaDigestLength = (uint32_t)CY_CRYPTO_SHA512_224_DIGEST_SIZE;
                        break;
                    }
                    case CY_CRYPTO_MODE_SHA512_224:
                    {
                        shaDigestLength = (uint32_t)CY_CRYPTO_SHA512_256_DIGEST_SIZE;
                        break;
                    }
                    default:
                    {
                        errorCrypto = CY_SI_BAD_PARAM;
                        break;
                    }
                }
                allowCrypto = (errorCrypto != CY_SI_SUCCESS) ? false : true;
                if (allowCrypto == false) 
                {
                    break;
                }
                errorCrypto = Cy_SI_CheckMemRange(
                                (uint32_t)(((cy_stc_crypto_context_sha_t *)processData->xdata)->message),
                                (uint32_t)(((cy_stc_crypto_context_sha_t *)processData->xdata)->message)
                                   + (uint32_t)(((cy_stc_crypto_context_sha_t *)processData->xdata)->messageSize),
                                CY_SI_MEM_TYPE_FLASH_SRC_MASK | CY_SI_MEM_TYPE_RAM_MASK);
                allowCrypto = (errorCrypto != CY_SI_SUCCESS) ? false : true;
                if (allowCrypto == false) 
                {
                    break;
                }
                errorCrypto = Cy_SI_CheckMemRange(
                                (uint32_t)(((cy_stc_crypto_context_sha_t *)processData->xdata)->dst),
                                (uint32_t)(((cy_stc_crypto_context_sha_t *)processData->xdata)->dst)
                                    + shaDigestLength,
                                CY_SI_MEM_TYPE_FLASH_SRC_MASK | CY_SI_MEM_TYPE_RAM_MASK);
                allowCrypto = (errorCrypto != CY_SI_SUCCESS) ? false : true;
                break;
            }
            case CY_CRYPTO_INSTR_MEM_CPY:
            {
                /* used params: *dst,
                                *src0,
                                dataSize */
                errorCrypto = Cy_SI_CheckMemRange(
                                (uint32_t)(((cy_stc_crypto_context_str_t *)processData->xdata)->src0),
                                (uint32_t)(((cy_stc_crypto_context_str_t *)processData->xdata)->src0)
                                    + (uint32_t)(((cy_stc_crypto_context_str_t *)processData->xdata)->dataSize),
                                CY_SI_MEM_TYPE_FLASH_SRC_MASK | CY_SI_MEM_TYPE_RAM_MASK);
                allowCrypto = (errorCrypto != CY_SI_SUCCESS) ? false : true;
                if (allowCrypto == false) 
                {
                    break;
                }
                errorCrypto = Cy_SI_CheckMemRange(
                                (uint32_t)(((cy_stc_crypto_context_str_t *)processData->xdata)->dst),
                                (uint32_t)(((cy_stc_crypto_context_str_t *)processData->xdata)->dst)
                                    + (uint32_t)(((cy_stc_crypto_context_str_t *)processData->xdata)->dataSize),
                                CY_SI_MEM_TYPE_FLASH_SRC_MASK | CY_SI_MEM_TYPE_RAM_MASK);
                allowCrypto = (errorCrypto != CY_SI_SUCCESS) ? false : true;
                break;
            }
            case CY_CRYPTO_INSTR_MEM_SET:
            {
                /* used params:     *dst,
                                    data,
                                    dataSize */
                errorCrypto = Cy_SI_CheckMemRange(
                                (uint32_t)(((cy_stc_crypto_context_str_t *)processData->xdata)->dst),
                                (uint32_t)(((cy_stc_crypto_context_str_t *)processData->xdata)->dst)
                                    + (uint32_t)(((cy_stc_crypto_context_str_t *)processData->xdata)->dataSize),
                                CY_SI_MEM_TYPE_FLASH_SRC_MASK | CY_SI_MEM_TYPE_RAM_MASK);
                allowCrypto = (errorCrypto != CY_SI_SUCCESS) ? false : true;
                break;
            }
            case CY_CRYPTO_INSTR_MEM_CMP:
            {
                /* used params:     *src0,
                                    *src1,
                                    dataSize,
                                    *dst */
                errorCrypto = Cy_SI_CheckMemRange(
                                (uint32_t)(((cy_stc_crypto_context_str_t *)processData->xdata)->src0),
                                (uint32_t)(((cy_stc_crypto_context_str_t *)processData->xdata)->src0)
                                    + (uint32_t)(((cy_stc_crypto_context_str_t *)processData->xdata)->dataSize),
                                CY_SI_MEM_TYPE_FLASH_SRC_MASK | CY_SI_MEM_TYPE_RAM_MASK);
                allowCrypto = (errorCrypto != CY_SI_SUCCESS) ? false : true;
                if (allowCrypto == false) 
                {
                    break;
                }
                errorCrypto = Cy_SI_CheckMemRange(
                                (uint32_t)(((cy_stc_crypto_context_str_t *)processData->xdata)->src1),
                                (uint32_t)(((cy_stc_crypto_context_str_t *)processData->xdata)->src1)
                                    + (uint32_t)(((cy_stc_crypto_context_str_t *)processData->xdata)->dataSize),
                                CY_SI_MEM_TYPE_FLASH_SRC_MASK | CY_SI_MEM_TYPE_RAM_MASK);
                allowCrypto = (errorCrypto != CY_SI_SUCCESS) ? false : true;
                if (allowCrypto == false) 
                {
                    break;
                }
                errorCrypto = Cy_SI_CheckMemRange(
                                (uint32_t)(((cy_stc_crypto_context_str_t *)processData->xdata)->dst),
                                (uint32_t)(((cy_stc_crypto_context_str_t *)processData->xdata)->dst), /* uint32_t */
                                CY_SI_MEM_TYPE_FLASH_SRC_MASK | CY_SI_MEM_TYPE_RAM_MASK);
                allowCrypto = (errorCrypto != CY_SI_SUCCESS) ? false : true;
                break;
            }
            case CY_CRYPTO_INSTR_MEM_XOR:
            {
                /* used params:     *src0,
                                    *src1,
                                    dataSize,
                                    *dst */
                errorCrypto = Cy_SI_CheckMemRange(
                                (uint32_t)(((cy_stc_crypto_context_str_t *)processData->xdata)->src0),
                                (uint32_t)(((cy_stc_crypto_context_str_t *)processData->xdata)->src0)
                                    + (uint32_t)(((cy_stc_crypto_context_str_t *)processData->xdata)->dataSize),
                                CY_SI_MEM_TYPE_FLASH_SRC_MASK | CY_SI_MEM_TYPE_RAM_MASK);
                allowCrypto = (errorCrypto != CY_SI_SUCCESS) ? false : true;
                if (allowCrypto == false) 
                {
                    break;
                }
                errorCrypto = Cy_SI_CheckMemRange(
                                (uint32_t)(((cy_stc_crypto_context_str_t *)processData->xdata)->src1),
                                (uint32_t)(((cy_stc_crypto_context_str_t *)processData->xdata)->src1)
                                    + (uint32_t)(((cy_stc_crypto_context_str_t *)processData->xdata)->dataSize),
                                CY_SI_MEM_TYPE_FLASH_SRC_MASK | CY_SI_MEM_TYPE_RAM_MASK);
                allowCrypto = (errorCrypto != CY_SI_SUCCESS) ? false : true;
                if (allowCrypto == false) 
                {
                    break;
                }
                errorCrypto = Cy_SI_CheckMemRange(
                                (uint32_t)(((cy_stc_crypto_context_str_t *)processData->xdata)->dst),
                                (uint32_t)(((cy_stc_crypto_context_str_t *)processData->xdata)->dst)
                                    + (uint32_t)(((cy_stc_crypto_context_str_t *)processData->xdata)->dataSize),
                                CY_SI_MEM_TYPE_FLASH_SRC_MASK | CY_SI_MEM_TYPE_RAM_MASK);
                allowCrypto = (errorCrypto != CY_SI_SUCCESS) ? false : true;
                break;
            }
            case CY_CRYPTO_INSTR_CRC_INIT:
            {
                break;
            }
            case CY_CRYPTO_INSTR_CRC:
            {
                /* used params:     lfsrInitState,
                                    *srcData,
                                    dataSize,
                                    *crc */
                errorCrypto = Cy_SI_CheckMemRange(
                                (uint32_t)(((cy_stc_crypto_context_crc_t *)processData->xdata)->srcData),
                                (uint32_t)(((cy_stc_crypto_context_crc_t *)processData->xdata)->srcData)
                                    + (uint32_t)(((cy_stc_crypto_context_str_t *)processData->xdata)->dataSize),
                                CY_SI_MEM_TYPE_FLASH_SRC_MASK | CY_SI_MEM_TYPE_RAM_MASK);
                allowCrypto = (errorCrypto != CY_SI_SUCCESS) ? false : true;
                if (allowCrypto == false) 
                {
                    break;
                }
                errorCrypto = Cy_SI_CheckMemRange(
                                (uint32_t)(((cy_stc_crypto_context_crc_t *)processData->xdata)->crc),
                                (uint32_t)(((cy_stc_crypto_context_crc_t *)processData->xdata)->crc), /* uint32_t */
                                CY_SI_MEM_TYPE_FLASH_SRC_MASK | CY_SI_MEM_TYPE_RAM_MASK);
                allowCrypto = (errorCrypto != CY_SI_SUCCESS) ? false : true;
                break;
            }
            case CY_CRYPTO_INSTR_DES:
            {
                /* used params:     dirMode,
                                    *key,
                                    *dst,
                                    *src */
                errorCrypto = Cy_SI_CheckMemRange(
                                (uint32_t)(((cy_stc_crypto_context_des_t *)processData->xdata)->dst),
                                (uint32_t)(((cy_stc_crypto_context_des_t *)processData->xdata)->dst)
                                    + (uint32_t)CY_CRYPTO_DES_KEY_SIZE,
                                CY_SI_MEM_TYPE_FLASH_SRC_MASK | CY_SI_MEM_TYPE_RAM_MASK);
                allowCrypto = (errorCrypto != CY_SI_SUCCESS) ? false : true;
                if (allowCrypto == false) 
                {
                    break;
                }
                errorCrypto = Cy_SI_CheckMemRange(
                                (uint32_t)(((cy_stc_crypto_context_des_t *)processData->xdata)->src),
                                (uint32_t)(((cy_stc_crypto_context_des_t *)processData->xdata)->src)
                                    + (uint32_t)CY_CRYPTO_DES_KEY_SIZE,
                                CY_SI_MEM_TYPE_FLASH_SRC_MASK | CY_SI_MEM_TYPE_RAM_MASK);
                allowCrypto = (errorCrypto != CY_SI_SUCCESS) ? false : true;
                break;
            }
            case CY_CRYPTO_INSTR_3DES:
            {
                /* used params:     dirMode,
                                    *key,
                                    *dst,
                                    *src */
                errorCrypto = Cy_SI_CheckMemRange(
                                (uint32_t)(((cy_stc_crypto_context_des_t *)processData->xdata)->dst),
                                (uint32_t)(((cy_stc_crypto_context_des_t *)processData->xdata)->dst)
                                    + (uint32_t)CY_CRYPTO_TDES_KEY_SIZE,
                                CY_SI_MEM_TYPE_FLASH_SRC_MASK | CY_SI_MEM_TYPE_RAM_MASK);
                allowCrypto = (errorCrypto != CY_SI_SUCCESS) ? false : true;
                if (allowCrypto == false) 
                {
                    break;
                }
                errorCrypto = Cy_SI_CheckMemRange(
                                (uint32_t)(((cy_stc_crypto_context_des_t *)processData->xdata)->src),
                                (uint32_t)(((cy_stc_crypto_context_des_t *)processData->xdata)->src)
                                    + (uint32_t)CY_CRYPTO_TDES_KEY_SIZE,
                                CY_SI_MEM_TYPE_FLASH_SRC_MASK | CY_SI_MEM_TYPE_RAM_MASK);
                allowCrypto = (errorCrypto != CY_SI_SUCCESS) ? false : true;
                break;
            }
            case CY_CRYPTO_INSTR_RSA_PROC:
            {
                /* used params:     *key,
                                    *message,
                                    messageSize,
                                    *result */
                
                /* Key structure contains pointers to calculated coefficients so these pointers must 
                   either be NULL or contain correct accessible addresses. */
                if((uint32_t)(((cy_stc_crypto_context_rsa_t *)processData->xdata)->key->barretCoefPtr) != 0UL)
                {
                    errorCrypto = Cy_SI_CheckMemRange(
                                    (uint32_t)(((cy_stc_crypto_context_rsa_t *)processData->xdata)->key->barretCoefPtr),
                                    (uint32_t)(((cy_stc_crypto_context_rsa_t *)processData->xdata)->key->barretCoefPtr)
                                        + (uint32_t)(((cy_stc_crypto_context_rsa_t *)processData->xdata)->key->moduloLength) + 4u,
                                    CY_SI_MEM_TYPE_RAM_MASK);
                }
                allowCrypto = (errorCrypto != CY_SI_SUCCESS) ? false : true;
                if (allowCrypto == false) 
                {
                    break;
                }
                if((uint32_t)(((cy_stc_crypto_context_rsa_t *)processData->xdata)->key->inverseModuloPtr) != 0UL)
                {
                    errorCrypto = Cy_SI_CheckMemRange(
                                    (uint32_t)(((cy_stc_crypto_context_rsa_t *)processData->xdata)->key->inverseModuloPtr),
                                    (uint32_t)(((cy_stc_crypto_context_rsa_t *)processData->xdata)->key->inverseModuloPtr)
                                        + (uint32_t)(((cy_stc_crypto_context_rsa_t *)processData->xdata)->key->moduloLength),
                                    CY_SI_MEM_TYPE_RAM_MASK);
                }
                allowCrypto = (errorCrypto != CY_SI_SUCCESS) ? false : true;
                if (allowCrypto == false) 
                {
                    break;
                }
                if((uint32_t)(((cy_stc_crypto_context_rsa_t *)processData->xdata)->key->rBarPtr) != 0UL)
                {
                    errorCrypto = Cy_SI_CheckMemRange(
                                    (uint32_t)(((cy_stc_crypto_context_rsa_t *)processData->xdata)->key->rBarPtr),
                                    (uint32_t)(((cy_stc_crypto_context_rsa_t *)processData->xdata)->key->rBarPtr)
                                        + (uint32_t)(((cy_stc_crypto_context_rsa_t *)processData->xdata)->key->moduloLength),
                                    CY_SI_MEM_TYPE_RAM_MASK);
                }
                allowCrypto = (errorCrypto != CY_SI_SUCCESS) ? false : true;
                if (allowCrypto == false) 
                {
                    break;
                }       
                errorCrypto = Cy_SI_CheckMemRange(
                                (uint32_t)(((cy_stc_crypto_context_rsa_t *)processData->xdata)->message),
                                (uint32_t)(((cy_stc_crypto_context_rsa_t *)processData->xdata)->message)
                                    + (uint32_t)(((cy_stc_crypto_context_rsa_t *)processData->xdata)->messageSize),
                                CY_SI_MEM_TYPE_FLASH_SRC_MASK | CY_SI_MEM_TYPE_RAM_MASK);
                allowCrypto = (errorCrypto != CY_SI_SUCCESS) ? false : true;
                if (allowCrypto == false) 
                {
                    break;
                }
                errorCrypto = Cy_SI_CheckMemRange(
                                (uint32_t)(((cy_stc_crypto_context_rsa_t *)processData->xdata)->result),
                                (uint32_t)(((cy_stc_crypto_context_rsa_t *)processData->xdata)->result), /* ??? */
                                CY_SI_MEM_TYPE_FLASH_SRC_MASK | CY_SI_MEM_TYPE_RAM_MASK);
                allowCrypto = (errorCrypto != CY_SI_SUCCESS) ? false : true;
                break;
            }
            case CY_CRYPTO_INSTR_RSA_COEF:
            {
                /* used params:    key */
                
                /* Key structure contains pointers to calculated coefficients so these pointers must 
                   contain correct accessible addresses. */
                if((uint32_t)(((cy_stc_crypto_context_rsa_t *)processData->xdata)->key->barretCoefPtr) != 0UL)
                {
                    errorCrypto = Cy_SI_CheckMemRange(
                                    (uint32_t)(((cy_stc_crypto_context_rsa_t *)processData->xdata)->key->barretCoefPtr),
                                    (uint32_t)(((cy_stc_crypto_context_rsa_t *)processData->xdata)->key->barretCoefPtr)
                                        + (uint32_t)(((cy_stc_crypto_context_rsa_t *)processData->xdata)->key->moduloLength) + 4u,
                                    CY_SI_MEM_TYPE_RAM_MASK);
                }
                allowCrypto = (errorCrypto != CY_SI_SUCCESS) ? false : true;
                if (allowCrypto == false) 
                {
                    break;
                }
                if((uint32_t)(((cy_stc_crypto_context_rsa_t *)processData->xdata)->key->inverseModuloPtr) != 0UL)
                {
                    errorCrypto = Cy_SI_CheckMemRange(
                                    (uint32_t)(((cy_stc_crypto_context_rsa_t *)processData->xdata)->key->inverseModuloPtr),
                                    (uint32_t)(((cy_stc_crypto_context_rsa_t *)processData->xdata)->key->inverseModuloPtr)
                                        + (uint32_t)(((cy_stc_crypto_context_rsa_t *)processData->xdata)->key->moduloLength),
                                    CY_SI_MEM_TYPE_RAM_MASK);
                }
                allowCrypto = (errorCrypto != CY_SI_SUCCESS) ? false : true;
                if (allowCrypto == false) 
                {
                    break;
                }
                if((uint32_t)(((cy_stc_crypto_context_rsa_t *)processData->xdata)->key->rBarPtr) != 0UL)
                {
                    errorCrypto = Cy_SI_CheckMemRange(
                                    (uint32_t)(((cy_stc_crypto_context_rsa_t *)processData->xdata)->key->rBarPtr),
                                    (uint32_t)(((cy_stc_crypto_context_rsa_t *)processData->xdata)->key->rBarPtr)
                                        + (uint32_t)(((cy_stc_crypto_context_rsa_t *)processData->xdata)->key->moduloLength),
                                    CY_SI_MEM_TYPE_RAM_MASK);
                }
                allowCrypto = (errorCrypto != CY_SI_SUCCESS) ? false : true;
                break;
            }
            case CY_CRYPTO_INSTR_RSA_VER:
            {
                /* used params:     *verResult,
                                    digestType,
                                    *hash,
                                    *decryptedSignature,
                                    decryptedSignatureLength */
                
                /* Determine the digest length */
                switch (((cy_stc_crypto_context_rsa_ver_t *)processData->xdata)->digestType)
                {
                    case CY_CRYPTO_MODE_SHA1:
                    {
                        shaDigestLength = (uint32_t)CY_CRYPTO_SHA1_DIGEST_SIZE;
                        break;
                    }
                    case CY_CRYPTO_MODE_SHA224:
                    {
                        shaDigestLength = (uint32_t)CY_CRYPTO_SHA224_DIGEST_SIZE;
                        break;
                    }
                    case CY_CRYPTO_MODE_SHA256:
                    {
                        shaDigestLength = (uint32_t)CY_CRYPTO_SHA256_DIGEST_SIZE;
                        break;
                    }
                    case CY_CRYPTO_MODE_SHA384:
                    {
                        shaDigestLength = (uint32_t)CY_CRYPTO_SHA384_DIGEST_SIZE;
                        break;
                    }
                    case CY_CRYPTO_MODE_SHA512:
                    {
                        shaDigestLength = (uint32_t)CY_CRYPTO_SHA512_DIGEST_SIZE;
                        break;
                    }
                    case CY_CRYPTO_MODE_SHA512_256:
                    {
                        shaDigestLength = (uint32_t)CY_CRYPTO_SHA512_224_DIGEST_SIZE;
                        break;
                    }
                    case CY_CRYPTO_MODE_SHA512_224:
                    {
                        shaDigestLength = (uint32_t)CY_CRYPTO_SHA512_256_DIGEST_SIZE;
                        break;
                    }
                    default:
                    {
                        errorCrypto = CY_SI_BAD_PARAM;
                        break;
                    }
                }
                allowCrypto = (errorCrypto != CY_SI_SUCCESS) ? false : true;
                if (allowCrypto == false) 
                {
                    break;
                }
                errorCrypto = Cy_SI_CheckMemRange(
                                (uint32_t)(((cy_stc_crypto_context_rsa_ver_t *)processData->xdata)->verResult),
                                (uint32_t)(((cy_stc_crypto_context_rsa_ver_t *)processData->xdata)->verResult),
                                CY_SI_MEM_TYPE_FLASH_SRC_MASK | CY_SI_MEM_TYPE_RAM_MASK);
                allowCrypto = (errorCrypto != CY_SI_SUCCESS) ? false : true;
                if (allowCrypto == false) 
                {
                    break;
                }
                errorCrypto = Cy_SI_CheckMemRange(
                                (uint32_t)(((cy_stc_crypto_context_rsa_ver_t *)processData->xdata)->hash),
                                (uint32_t)(((cy_stc_crypto_context_rsa_ver_t *)processData->xdata)->hash)
                                    + shaDigestLength,
                                CY_SI_MEM_TYPE_FLASH_SRC_MASK | CY_SI_MEM_TYPE_RAM_MASK);
                allowCrypto = (errorCrypto != CY_SI_SUCCESS) ? false : true;
                if (allowCrypto == false) 
                {
                    break;
                }
                errorCrypto = Cy_SI_CheckMemRange(
                                (uint32_t)(((cy_stc_crypto_context_rsa_ver_t *)processData->xdata)->decryptedSignature),
                                (uint32_t)(((cy_stc_crypto_context_rsa_ver_t *)processData->xdata)->decryptedSignature)
                                    + (uint32_t)(((cy_stc_crypto_context_rsa_ver_t *)processData->xdata)->decryptedSignatureLength),
                                CY_SI_MEM_TYPE_FLASH_SRC_MASK | CY_SI_MEM_TYPE_RAM_MASK);
                allowCrypto = (errorCrypto != CY_SI_SUCCESS) ? false : true;
                break;
            }
            case CY_CRYPTO_INSTR_SRV_INFO:
            {
                break;
            }
            default:
            {
                errorCrypto = CY_SI_BAD_PARAM;
                allowCrypto = false;
                break;
            }
        }
        
        /* Begin Critical section (necessary if not executing out of ISR) */
        NVIC_DisableIRQ(CY_CRYPTO_CM0_NOTIFY_INTR_NR);
        
        if(allowCrypto)
        {
            /* Process the command and release the IPC channel */
            Cy_Crypto_Server_Process();
        }
        else
        {
            /* Return error code and release the channel */
            processData->resp = (cy_en_crypto_status_t)errorCrypto;
            if (NULL != processData->userCompleteCallback)
            {
                (void)Cy_IPC_Drv_LockRelease(Cy_IPC_Drv_GetIpcBaseAddress(CY_IPC_CHAN_CRYPTO), 
                                                (1uL << processData->releaseNotifierChannel));
            }
            else
            {
                (void)Cy_IPC_Drv_LockRelease(Cy_IPC_Drv_GetIpcBaseAddress(CY_IPC_CHAN_CRYPTO), CY_SI_NO_IRQ);
            }
        }
        
        /* Update the interrupt state */
        crypto_intrPending &= (uint32_t)(~CY_SI_IRQ_MASK);
        
        /* End Critical section */
        NVIC_EnableIRQ(CY_CRYPTO_CM0_NOTIFY_INTR_NR);
    }
}

   
/*******************************************************************************
* Function Name: main
****************************************************************************//**
*
* \brief Secure Image CM0+ main().
*
* The main function performs the following steps:
*  - Validate the user application image using SHA-256 and RSA-2048.
*  - Initialize the Crypto Server.
*  - Initialize the SysCall interrupt.
*  - Initialize the Protection Units.
*  - Jump to validated application image.
*
*******************************************************************************/
int main(void)
{   
    uint32_t validToc;
    uint32_t vtBase;
    uint32_t vtOffset;
    uint32_t userAppAddr[2];
    
    __enable_irq();
    
    /* Image #2 (User App) Validation */
    errorStatus = Cy_SI_ValidateImage(&validToc);
    if (errorStatus != CY_SI_SUCCESS)
    {
        /* Secure boot failed */
        Cy_SI_ErrorTask();
    }
    else
    {
        if(validToc == CY_SI_VALID_TOC2)
        {
            /* Retrieve the vector table of user app */
            vtBase = (uint32_t)(&((cy_stc_user_appheader_t *)SFLASH->TOC2_SECOND_USER_APP_ADDR)->core0Vt);
            vtOffset = ((cy_stc_user_appheader_t *)SFLASH->TOC2_SECOND_USER_APP_ADDR)->core0Vt;
        }
        else if(validToc == CY_SI_VALID_RTOC2)
        {
            /* Retrieve the vector table of user app */
            vtBase = (uint32_t)(&((cy_stc_user_appheader_t *)SFLASH->RTOC2_SECOND_USER_APP_ADDR)->core0Vt);
            vtOffset = ((cy_stc_user_appheader_t *)SFLASH->RTOC2_SECOND_USER_APP_ADDR)->core0Vt;
        }
        else
        {
            /* Corrupted/Invalid TOC */
            errorStatus = CY_SI_FAILURE;
            Cy_SI_ErrorTask();
        }
        userAppAddr[0] = ((uint32_t *)(vtBase + vtOffset))[0]; /* User App MSP */
        userAppAddr[1] = ((uint32_t *)(vtBase + vtOffset))[1]; /* User App Reset Vector */
    }

    /* Crypto Server(App) configuration */
    errorStatus = Cy_SI_ConfigCrypto();
    if(errorStatus != CY_SI_SUCCESS)
    {
        Cy_SI_ErrorTask();
    }
    
    /* SysCall configuration */
    errorStatus = Cy_SI_ConfigSysCall();
    if(errorStatus != CY_SI_SUCCESS)
    {
        Cy_SI_ErrorTask();
    }
    
    /* Protection Units configuration */
    errorStatus = Cy_SI_ConfigProt();
    if(errorStatus != CY_SI_SUCCESS)
    {
        Cy_SI_ErrorTask();
    }
    
    /* Start the User application */
    Cy_SI_UserApp(userAppAddr[0], userAppAddr[1]);
    
    #if defined (__GNUC__)
        /**************************************************
        * Note: this code will never execute. This section
        * is used as a workaround for keeping the symbols
        * that need to be exported from the secure image.
        * It can safely be removed to suit the application
        * build environment. Refer to the "Build
        * Environment" section for more details.
        **************************************************/
        uint32_t userKeyLoc;
        userKeyLoc = Cy_SI_GetVersion();
        userKeyLoc = (uint32_t)Cy_SI_GetSecureKeyAddr(userKeyLoc);
        userKeyLoc = cy_toc2.userKeyAddr;
        userKeyLoc = cy_rtoc2.userKeyAddr;
    #endif
    
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

__cy_app_verify_start     EQU __cpp(CY_SI_SECURE_FLASH_BEGIN)
__cy_app_verify_length    EQU __cpp(CY_SI_SECURE_FLASH_END - CY_SI_SECURE_FLASH_BEGIN - CY_SI_SECURE_DIGSIG_SIZE)
}
#endif /* defined (__ARMCC_VERSION) */

/* [] END OF FILE */
