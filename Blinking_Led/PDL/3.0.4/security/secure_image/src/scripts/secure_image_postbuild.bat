:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
::
:: File Name: secure_image_postbuild.bat
::
:: Version: 1.0
::
:: Description:
::  Postbuild script for the secure image, designed to run after the
::  CM0+ application link step. The script performs the following functions.
::  - Run the cymcuelftool to export select symbols (as defined in
::    cy_si_export.txt) to be referenced by the user application.
::  - Run the cymcuelftool application signing process for creating a 
::    digital signature of the secure image.
::
:: This script was designed to work with PSoC Creator 4.2 and PDL 3.0.1. The
:: secure_image project build settings in PSoC Creator is configured to run 
:: this script at the end of the CM0+ build process.
::
:: Note:
::  - CyMcuElfTool uses OpenSSL 1.0.2 or later. This package can be obtained
::    in a Windows environment from a CygWin distribution.
::  - ObjDump and ReadElf are used to translate the elf files to human readable
::    text files. ARM GNU distribution in PSoC Creator is used for this purpose.
::  - For IAR, ielfdumparm tool is used to print the elf files. 
::
:: Usage:
::  1. Ensure that the referenced paths and variable are correct for your 
::     build environment. 
::  2. Ensure that all relevant software packages are installed.
::  3. Build the secure_image project. At the end of the CM0+ build process, 
::     this script will be invoked.
::  4. Option a. Program the generated secure image hex file in the /CortexM0p 
::               directory on to the device. Then program the user application
::               hex file separately.
::     Option b. Refer to the user_app0_postbuild.bat to merge the
::               secure_image elf file with the user_app0 elf file. Then
::               program the combined hex file. 
::
:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
:: Copyright 2017-2018, Cypress Semiconductor Corporation. All rights reserved.
:: This software is owned by Cypress Semiconductor Corporation and is protected
:: by and subject to worldwide patent and copyright laws and treaties.
:: Therefore, you may use this software only as provided in the license agreement
:: accompanying the software package from which you obtained this software.
:: CYPRESS AND ITS SUPPLIERS MAKE NO WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,
:: WITH REGARD TO THIS SOFTWARE, INCLUDING, BUT NOT LIMITED TO, NONINFRINGEMENT,
:: IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

:: Parameters from PSoC Creator
:: %1 = ${Platform}
:: %2 = ${OutputDir}
:: %3 = ${ProjectShortName}

:: Compiler paths in PSoC Creator: ARM_GCC_541, ARM_IAR_Generic, ARM_MDK_Generic, 
set COMPILER_VERSION=%1
:: Project directory
set OUTPUT_DIR=%2
:: Project name of the secure image
set SI_PRJ_NAME=%3

:: Environment
set PDL_VERSION=3.0.1
set CREATOR_VERSION=4.2
set GNU_VERSION=5.4.1

:: Encryption Policy
set HASH_ALGORITHM=SHA256
set ENC_ALGORITHM=RSASSA-PKCS

:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
:: Initialization
:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

:: Supported Compiler vendors: GCC, IAR, ARMCC
if not x%COMPILER_VERSION:GCC=%==x%COMPILER_VERSION% (goto vendor_GCC) else (goto vendor_parse2)
:vendor_GCC
set COMPILER_VENDOR=GCC
goto done_vendor
:vendor_parse2
if not x%COMPILER_VERSION:MDK=%==x%COMPILER_VERSION% (goto vendor_MDK) else (goto vendor_IAR)
:vendor_MDK
set COMPILER_VENDOR=ARMCC
goto done_vendor
:vendor_IAR
set COMPILER_VENDOR=IAR
goto done_vendor
:done_vendor
set ELF_VIEW="%~dp0..\..\elf\%COMPILER_VENDOR%"

:: Cypress install path
if exist "%PROGRAMFILES(X86)%" (goto 64BIT) else (goto 32BIT)
:32bit
set PROGRAM_FILES_PATH=C:\Program Files\Cypress
goto done_pc_path
:64bit
set PROGRAM_FILES_PATH=C:\Program Files (x86)\Cypress
goto done_pc_path
:done_pc_path

:: Tool paths
set MCU_ELFTOOL="%PROGRAM_FILES_PATH%\PDL\%PDL_VERSION%\tools\win\elf\cymcuelftool.exe"
set OBJDUMP="%PROGRAM_FILES_PATH%\PSoC Creator\%CREATOR_VERSION%\PSoC Creator\import\gnu\arm\%GNU_VERSION%\bin\arm-none-eabi-objdump.exe"
set READELF="%PROGRAM_FILES_PATH%\PSoC Creator\%CREATOR_VERSION%\PSoC Creator\import\gnu\arm\%GNU_VERSION%\bin\arm-none-eabi-readelf.exe"
set IELFDUMP="C:\Program Files (x86)\IAR Systems\Embedded Workbench 8.0\arm\bin\ielfdumparm.exe"

:: File paths
set EXPORT_SYMBOL_FILE="%~dp0..\cy_si_export_symbols.txt"
set EXPORT_ASM_FILE="%~dp0..\cy_si_export_symbols.s"
set PRIVATE_KEY="%~dp0.\keys\rsa_private.txt"
set SI_IN_ELF="%OUTPUT_DIR%\%SI_PRJ_NAME%_link.elf"
set SI_OUT_ELF="%OUTPUT_DIR%\%SI_PRJ_NAME%.elf"
set SI_HEX="%OUTPUT_DIR%\%SI_PRJ_NAME%.hex"

:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
:: Run commands
:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

:: Generate the exported symbols assembly file
%MCU_ELFTOOL% --codeshare %SI_IN_ELF% %EXPORT_SYMBOL_FILE% %COMPILER_VENDOR% --output %EXPORT_ASM_FILE%

:: Populate the image with digital signature and TOC2 CRC
echo CyMcuElfTool uses OpenSSL 1.0.2 or later. Ensure that it is installed before proceeding.
%MCU_ELFTOOL% --sign %SI_IN_ELF% %HASH_ALGORITHM% --encrypt %ENC_ALGORITHM% --key %PRIVATE_KEY% --output %SI_OUT_ELF% --hex %SI_HEX%

:: Print the elf file content
if not exist %ELF_VIEW% mkdir %ELF_VIEW%
if not "%COMPILER_VENDOR%"=="IAR" (
%OBJDUMP% -s %SI_OUT_ELF% >%ELF_VIEW%\%SI_PRJ_NAME%_objdump.txt
%READELF% -a %SI_OUT_ELF% >%ELF_VIEW%\%SI_PRJ_NAME%_readelf.txt
) else (
%IELFDUMP% --all %SI_OUT_ELF% >%ELF_VIEW%\%SI_PRJ_NAME%_ielfdump.txt
)
