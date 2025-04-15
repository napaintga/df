:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
::
:: File Name: user_app0_postbuild.bat
::
:: Version: 1.0
::
:: Description:
::  Postbuild script for the user application image, designed to run after the
::  CM4 application link step. The script performs the following functions.
::  - Run the cymcuelftool application signing process for creating a 
::    digital signature of the user application image.
::  - Combine the user application image with the secure image to create a
::    single elf file and a single hex file.
::
:: This script was designed to work with PSoC Creator 4.2 and PDL 3.0.1. The
:: user_app0 project build settings in PSoC Creator is configured to run 
:: this script at the end of the CM4 build process.
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
::  3. Create a secure image.
::  3. Build the user_app0 project. At the end of the CM4 build process, 
::     this script will be invoked. This will sign the user_app0 image and
::     then merge it with the secure_image.
::  4. Program the merged hex file on to the device.
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
:: %4 = ${Config}

:: Compiler paths in PSoC Creator: ARM_GCC_541, ARM_IAR_Generic, ARM_MDK_Generic, 
set COMPILER_VERSION=%1
:: Project directory
set OUTPUT_DIR=%2
:: Project name of the user app
set USERAPP_PRJ_NAME=%3
:: Build mode (assumed same for secure image)
set BUILD_MODE=%4

:: Secure image project name
set SI_PRJ_NAME=secure_image

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
set PRIVATE_KEY="%~dp0.\keys\rsa_private.txt"
set SI_ELF="..\%SI_PRJ_NAME%.cydsn\CortexM0p\%COMPILER_VERSION%\%BUILD_MODE%\%SI_PRJ_NAME%.elf"
set USERAPP_CM0_ELF=".\CortexM0p\%COMPILER_VERSION%\%BUILD_MODE%\%USERAPP_PRJ_NAME%_link.elf"
set USERAPP_CM4_ELF=".\CortexM4\%COMPILER_VERSION%\%BUILD_MODE%\%USERAPP_PRJ_NAME%_link.elf"
set USERAPP_OUT_ELF="%OUTPUT_DIR%\%USERAPP_PRJ_NAME%.elf"
set USERAPP_HEX="%OUTPUT_DIR%\%USERAPP_PRJ_NAME%.hex"

:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
:: Run commands
:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

:: Merge the user_app0 CM0+ and CM4 images
%MCU_ELFTOOL% --merge %USERAPP_CM4_ELF% %USERAPP_CM0_ELF% --output %USERAPP_OUT_ELF%

:: Populate the image with security policy data
echo CyMcuElfTool uses OpenSSL 1.0.2 or later. Ensure that it is installed before proceeding.
%MCU_ELFTOOL% --sign %USERAPP_OUT_ELF% %HASH_ALGORITHM% --encrypt %ENC_ALGORITHM% --key %PRIVATE_KEY% --output %USERAPP_OUT_ELF% --hex %USERAPP_HEX%

:: Print the elf file content
if not exist %ELF_VIEW% mkdir %ELF_VIEW%
if not "%COMPILER_VENDOR%"=="IAR" (
%OBJDUMP% -s %USERAPP_OUT_ELF% >%ELF_VIEW%\%USERAPP_PRJ_NAME%_objdump.txt
%READELF% -a %USERAPP_OUT_ELF% >%ELF_VIEW%\%USERAPP_PRJ_NAME%_readelf.txt
) else (
%IELFDUMP% --all %USERAPP_OUT_ELF% >%ELF_VIEW%\%USERAPP_PRJ_NAME%_ielfdump.txt
)

:: Merge user_app0 and secure_image and generate the hex file
%MCU_ELFTOOL% --merge %USERAPP_OUT_ELF% %SI_ELF% --output %USERAPP_OUT_ELF% --hex %USERAPP_HEX%

:: Print out the elf file contents
if not "%COMPILER_VENDOR%"=="IAR" (
%OBJDUMP% -s %USERAPP_OUT_ELF% >%ELF_VIEW%\final_image_objdump.txt
%READELF% -a %USERAPP_OUT_ELF% >%ELF_VIEW%\final_image_readelf.txt
) else (
%IELFDUMP% --all %USERAPP_OUT_ELF% >%ELF_VIEW%\final_image_ielfdump.txt
)
