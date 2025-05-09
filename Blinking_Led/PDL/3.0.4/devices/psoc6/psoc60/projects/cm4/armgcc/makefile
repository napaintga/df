
#
#       !!!! Do NOT edit this makefile with an editor which replace tabs by spaces !!!!    
#
##############################################################################################
# 
# On command line:
#
# make all = Create project
#
# make clean = Clean project files.
#
# To rebuild project do "make clean" and "make all".
##############################################################################################
# Start of default section
#
# Example Linux: TOOLSDIR = /opt/arminarm/bin/
#                ELFTOOL  = ../../../../../../tools/linux/elf/cymcuelftool
TOOLSDIR = "$(ARMGCC_DIR)"
ELFTOOL = "../../../../../../tools/win/elf/cymcuelftool.exe"
TRGT    = $(TOOLSDIR)/arm-none-eabi-
CC      = $(TRGT)gcc
CP      = $(TRGT)objcopy
CPP     = $(TRGT)g++
CXX     = $(TRGT)g++
AS      = $(TRGT)gcc -x assembler-with-cpp
LD      = $(TRGT)gcc
AR      = $(TRGT)ar
OBJCOPY = $(TRGT)objcopy
SZ      = $(TRGT)size
OBJDUMP = $(TRGT)objdump
NM      = $(TRGT)nm
RM      = rm -rf
HEX     = $(CP) -O ihex
BIN     = $(CP) -I elf32-little -O binary
SREC    = $(CP) -O srec

######################################################################################
# Custom options for cortex-m and cortex-r processors 
######################################################################################
CORTEX_M0PLUS_CC_FLAGS  = -mthumb -mcpu=cortex-m0plus
CORTEX_M0PLUS_LIB_PATH  = $(GCC_LIB)armv6-m
CORTEX_M0_CC_FLAGS      = -mthumb -mcpu=cortex-m0
CORTEX_M0_LIB_PATH      = $(GCC_LIB)armv6-m
CORTEX_M1_CC_FLAGS      = -mthumb -mcpu=cortex-m1
CORTEX_M1_LIB_PATH      = $(GCC_LIB)armv6-m
CORTEX_M3_CC_FLAGS      = -mthumb -mcpu=cortex-m3
CORTEX_M3_LIB_PATH      = $(GCC_LIB)armv7-m
CORTEX_M4_NOFP_CC_FLAGS = -mthumb -mcpu=cortex-m4
CORTEX_M4_NOFP_LIB_PATH = $(GCC_LIB)armv7e-m
CORTEX_M4_SWFP_CC_FLAGS = -mthumb -mcpu=cortex-m4 -mfloat-abi=softfp -mfpu=fpv4-sp-d16
CORTEX_M4_SWFP_LIB_PATH = $(GCC_LIB)armv7e-m/softfp
CORTEX_M4_HWFP_CC_FLAGS = -mthumb -mcpu=cortex-m4 -mfloat-abi=hard -mfpu=fpv4-sp-d16
CORTEX_M4_HWFP_LIB_PATH = $(GCC_LIB)armv7e-m/fpu
CORTEX_R4_NOFP_CC_FLAGS = -mthumb -march=armv7-r
CORTEX_R4_NOFP_LIB_PATH = $(GCC_LIB)armv7-r/thumb
CORTEX_R4_SWFP_CC_FLAGS = -mthumb -march=armv7-r -mfloat-abi=softfp -mfloat-abi=softfp -mfpu=vfpv3-d16
CORTEX_R4_SWFP_LIB_PATH = $(GCC_LIB)armv7-r/thumb/softfp
CORTEX_R4_HWFP_CC_FLAGS = -mthumb -march=armv7-r -mfloat-abi=softfp -mfloat-abi=hard -mfpu=vfpv3-d16
CORTEX_R4_HWFP_LIB_PATH = $(GCC_LIB)armv7-r/thumb/fpu
CORTEX_R5_NOFP_CC_FLAGS = -mthumb -march=armv7-r
CORTEX_R5_NOFP_LIB_PATH = $(GCC_LIB)armv7-r/thumb
CORTEX_R5_SWFP_CC_FLAGS = -mthumb -march=armv7-r -mfloat-abi=softfp -mfloat-abi=softfp -mfpu=vfpv3-d16
CORTEX_R5_SWFP_LIB_PATH = $(GCC_LIB)armv7-r/thumb/softfp
CORTEX_R5_HWFP_CC_FLAGS = -mthumb -march=armv7-r -mfloat-abi=softfp -mfloat-abi=hard -mfpu=vfpv3-d16
CORTEX_R5_HWFP_LIB_PATH = $(GCC_LIB)armv7-r/thumb/fpu

##############################################################################################
# Start of user section
#
# Define project name here
PROJECT = psoc60_cm4_template
MCU_CC_FLAGS = $(CORTEX_M4_NOFP_CC_FLAGS)
MCU_LIB_PATH = $(CORTEX_M4_NOFP_LIB_PATH)

# Define linker script file here
LD_SCRIPT = ../../../common/armgcc/linker/cy8c6xx6_cm4.ld


#
# End of user section
##############################################################################################

# directories
INC_DIRS = ../../../../../../cmsis/include\
    ../../../include\
    ../../../../ip\
    ../../../../../../drivers/peripheral\

# List C source files here
SRC_DIRS = ../\
    ../../../../../../drivers/peripheral/flash\
    ../../../../../../drivers/peripheral/ipc\
    ../../../../../../drivers/peripheral/sysint\
    ../../../../../../drivers/peripheral/syslib/gcc\
    ../../../../../../drivers/peripheral/syslib\

C_SRCS =
S_SRCS =
C_OBJS =
S_OBJS =

OUT_DIR = output/release

ifeq ($(strip $(OUT_DIR)), )
	OBJ_FOLDER =
else
	OBJ_FOLDER = $(strip $(OUT_DIR))/
endif

# All source/object files inside SRC_DIRS
C_SRCS := $(foreach dir,$(SRC_DIRS),$(wildcard $(dir)/*.c))
C_SRCS += $(foreach dir,../../../common,$(wildcard $(dir)/system_psoc60_cm4.c))
C_OBJS := $(patsubst %.c,$(OBJ_FOLDER)%.o,$(notdir $(C_SRCS)))

CPP_SRCS := $(foreach dir,$(SRC_DIRS),$(wildcard $(dir)/*.cpp))
CPP_OBJS := $(patsubst %.cpp,$(OBJ_FOLDER)%.o,$(notdir $(CPP_SRCS)))

S_SRCS := $(foreach dir,$(SRC_DIRS),$(wildcard $(dir)/*.S))
S_SRCS += $(foreach dir,../../../common/armgcc/startup,$(wildcard $(dir)/startup_psoc60_cm4.S))
S_OBJS := $(patsubst %.S,$(OBJ_FOLDER)%.o,$(notdir $(S_SRCS)))

VPATH := $(SRC_DIRS)
VPATH += ../../../common/armgcc/startup\
	../../../common

TARGET    = $(PROJECT)
LIB_DIRS  = $(patsubst %,-L%,$(MCU_LIB_PATH))
 
LIBS    = -lg -lstdc++ -lsupc++ -lm -lgcc -lc -lnosys

INC_DIRS_F = -I. $(patsubst %, -I%, $(INC_DIRS))

COMPILER_OPTIONS  = -g -ggdb -Os -Wall -fno-strict-aliasing
COMPILER_OPTIONS += -fno-strict-aliasing  -ffunction-sections -fdata-sections -fno-exceptions -fno-delete-null-pointer-checks
COMPILER_OPTIONS += -fno-hosted 
COMPILER_OPTIONS += -fmessage-length=0 -fno-builtin
COMPILER_OPTIONS += $(MCU_CC_FLAGS) -MMD -MP $(CC_SYMBOLS)
COMPILER_OPTIONS += -D NDEBUG -D CY8C6016BZI_F04

DEPEND_OPTS = -MF $(OBJ_FOLDER)$(@F:.o=.d)

# Flags
CFLAGS = $(COMPILER_OPTIONS) $(DEPEND_OPTS) $(INC_DIRS_F) -std=gnu99 -c

CXXFLAGS = $(COMPILER_OPTIONS)  -fno-rtti $(DEPEND_OPTS) $(INC_DIRS_F) -std=gnu++98 -c

ASFLAGS = $(COMPILER_OPTIONS) $(INC_DIRS_F) -c

LD_OPTIONS = $(MCU_CC_FLAGS) -Os -L $(LIB_DIRS) -T $(LD_SCRIPT) $(INC_DIRS_F)
#use this if %f is used, by default it's commented
#LD_OPTIONS += -u _printf_float -u _scanf_float
LD_OPTIONS += -Wl,--start-group
LD_OPTIONS += -lc -lg -lstdc++ -lsupc++
LD_OPTIONS += -lgcc -lm
LD_OPTIONS += -Wl,--end-group 
LD_OPTIONS += -Wl,-Map=$(OBJ_FOLDER)$(TARGET).map,--cref,--no-warn-mismatch,--gc-sections


#
# makefile rules
#

$(OBJ_FOLDER)%.o : %.c
	@echo 'Compiling C file: $(@F)'
	$(CC) $(CFLAGS) $< -o $@

$(OBJ_FOLDER)%.o : %.cpp
	@echo 'Compiling C++ file: $(@F)'
	$(CXX) $(CXXFLAGS) $< -o $@

$(OBJ_FOLDER)%.o : %.S
	@echo 'Compiling ASM file: $(@F)'
	$(AS) $(ASFLAGS) $< -o $@

%.hex: %.elf
	$(HEX) $< $@
	
%.bin: %.elf
	$(BIN) $< $@
	
%.mhx: %.elf
	$(SREC) $< $@

%.srec: %.elf
	$(SREC) $< $@
	
all: create_outputdir $(OBJ_FOLDER)$(TARGET).elf $(OBJ_FOLDER)$(TARGET).bin $(OBJ_FOLDER)$(TARGET).srec postbuildstep print_info

create_outputdir:
	$(shell mkdir -p $(OBJ_FOLDER) 2>/dev/null)

# Tool invocations
$(OBJ_FOLDER)$(TARGET).elf: $(LD_SCRIPT) $(C_OBJS) $(CPP_OBJS) $(S_OBJS)
	@echo 'Linking target: $@'
	$(LD) $(LD_OPTIONS) $(CPP_OBJS) $(C_OBJS) $(S_OBJS) $(LIBS) -o $(OBJ_FOLDER)$(TARGET).elf

# Other Targets
clean:
	@echo 'Removing entire out directory'
	$(RM) $(TARGET).elf $(TARGET).srec $(TARGET).bin $(TARGET).map $(OBJ_FOLDER)*.* $(OBJ_FOLDER)
	@echo ' '

postbuildstep:
	$(ELFTOOL) --sign "$(OBJ_FOLDER)$(TARGET).elf" --output "$(OBJ_FOLDER)$(TARGET).elf" --hex "$(OBJ_FOLDER)$(TARGET).hex"

print_info:
	@echo 'Printing size'
	$(SZ) --totals $(OBJ_FOLDER)$(TARGET).elf
	$(OBJDUMP) -D $(OBJ_FOLDER)$(TARGET).elf > $(OBJ_FOLDER)$(TARGET).lst
	$(NM) $(OBJ_FOLDER)$(TARGET).elf > $(OBJ_FOLDER)$(TARGET)-symbol-table.txt
	@echo ' '

.PHONY: all clean print_info
	


