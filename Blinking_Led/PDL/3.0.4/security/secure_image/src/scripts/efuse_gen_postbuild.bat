:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
::
:: File Name: efuse_gen_postbuild.bat
::
:: Version: 1.0
::
:: Description:
::  CM4 Postbuild script for printing out the elf file contents and to delete
::  the CM4 directory. Navigate to the CM0+ directory for the hex file that 
::  should be used for EFUSE programming.
::
::  Note that the EFUSE region can only be programmed using PSoC Programmer.
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

:: Compiler paths in PSoC Creator: ARM_GCC_541
set COMPILER_VERSION=%1
:: Project directory
set OUTPUT_DIR=%2
:: Project name of the secure image
set EFUSE_PRJ_NAME=%3

:: Compiler path in PSoC Creator: ARM_GCC_541
set COMPILER_VERSION=ARM_GCC_541
:: Build path type: Debug, Release
set BUILD_MODE=Debug

set CREATOR_VERSION=4.2
set GNU_VERSION=5.4.1

:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
:: Initialization
:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

:: Supported Compiler vendors: GCC, IAR, ARMCC
if not x%COMPILER_VERSION:GCC=%==x%COMPILER_VERSION% (goto vendor_GCC) else (goto vendor_error)
:vendor_GCC
set COMPILER_VENDOR=GCC
goto done_vendor
:vendor_error
echo Unsupported compiler.
EXIT 1
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
set OBJDUMP="%PROGRAM_FILES_PATH%\PSoC Creator\%CREATOR_VERSION%\PSoC Creator\import\gnu\arm\%GNU_VERSION%\bin\arm-none-eabi-objdump.exe"
set READELF="%PROGRAM_FILES_PATH%\PSoC Creator\%CREATOR_VERSION%\PSoC Creator\import\gnu\arm\%GNU_VERSION%\bin\arm-none-eabi-readelf.exe"

:: File paths
set EFUSE_ELF="%OUTPUT_DIR%\..\..\..\CortexM0p\%COMPILER_VERSION%\%BUILD_MODE%\%EFUSE_PRJ_NAME%.elf"
set CM4_DIR="%OUTPUT_DIR%\..\..\..\CortexM4\"

:: Print out the elf file contents
if not exist %ELF_VIEW% mkdir %ELF_VIEW%
%OBJDUMP% -s %EFUSE_ELF% >%ELF_VIEW%\%EFUSE_PRJ_NAME%_objdump.txt
%READELF% -a %EFUSE_ELF% >%ELF_VIEW%\%EFUSE_PRJ_NAME%_readelf.txt

:: Delete the CM4 directory to ensure there's no confusion
if exist %CM4_DIR% rmdir /s /q %CM4_DIR%