# Makefile for final project
# Targets are 
#   tiva
#   flash-tiva
#   debug-tiva
#   bbg
#   debug-bbg
#   clean 
#
# Originally based on the uCtools project
# found at uctools.github.com showing
# TIVA command line tool example
#
#######################################
# user configuration:
#######################################
# Path for sources
VPATH = src:3p/FreeRTOS/Source:3p/FreeRTOS/Source/portable/GCC/ARM_CM4F:3p/FreeRTOS/Source/portable/MemMang/
# TARGET: name of the output file
TARGET = tiva
# MCU: part number to build for
MCU = TM4C1294XL
# SOURCES: list of input source sources
SOURCES = main.c \
		  startup_gcc.c \
		  tasks.c \
    	  queue.c \
		  tasks.c \
		  list.c \
          heap_2.c \
          timers.c \
          croutine.c \
          event_groups.c \
          port.c

# INCLUDES: list of includes, by default, use Includes directory
INCLUDES = -Iinc -I3p/FreeRTOS/Source/include -I/home/ben/Repos/tivaware -I3p/FreeRTOS/Source/portable/GCC/ARM_CM4F
# BUILDDIR: directory to use for output
BUILDDIR = build
# TIVAWARE_PATH: path to tivaware folder
TIVAWARE_PATH = /home/ben/Repos/tivaware
# BINDIR: directory for binary output
BINDIR = bin

# LD_SCRIPT: linker script
LD_SCRIPT = $(MCU).ld

# define flags
CFLAGS = -g -mthumb -mcpu=cortex-m4 -mfpu=fpv4-sp-d16 -mfloat-abi=hard
CFLAGS +=-Os -ffunction-sections -fdata-sections -MD -std=c99 -Wall
CFLAGS += -pedantic -DPART_$(MCU) -c $(INCLUDES) 
CFLAGS += -DTARGET_IS_TM4C129_RA1 -D__TI_VFP_SUPPORT__
LDFLAGS = -T $(LD_SCRIPT) -Wl,-eResetISR -Wl,--gc-sections -g -mthumb -mcpu=cortex-m4 -mfpu=fpv4-sp-d16 -mfloat-abi=hard -ffunction-sections -fdata-sections -MD -std=c99 -Wall


#######################################

#######################################
# binaries
#######################################
CC = arm-none-eabi-gcc
LD = arm-none-eabi-gcc
OBJCOPY = arm-none-eabi-objcopy
RM      = rm -f
MKDIR	= mkdir -p
MV      = mv
FLASH   = lm4flash
GDB     = arm-none-eabi-gdb
GDB_CMD = -ex 'target extended-remote | openocd -f board/ek-tm4c1294xl.cfg -c "gdb_port pipe; log_output openocd.log"; monitor reset; monitor halt'

#######################################

# list of object files, placed in the build directory regardless of source path
OBJECTS = $(addprefix $(BUILDDIR)/,$(notdir $(SOURCES:.c=.o)))

all: tiva

debug-tiva: flash-tiva
	$(GDB) $(GDB_CMD) $(BINDIR)/$(TARGET).out

flash-tiva: tiva
	$(FLASH) bin/$(TARGET).bin

tiva: $(BUILDDIR)/$(TARGET).bin $(BINDIR)
	$(MV) $(BUILDDIR)/$(TARGET).bin $(BINDIR)/$(TARGET).bin
	$(MV) $(BUILDDIR)/$(TARGET).out $(BINDIR)/$(TARGET).out

$(BUILDDIR)/%.o: %.c | $(BUILDDIR)
	$(CC) -o $@ $^ $(CFLAGS)

$(BUILDDIR)/$(TARGET).out: $(OBJECTS)
	$(LD) -o $@ $^ -L$(TIVAWARE_PATH)/driverlib/gcc -ldriver $(LDFLAGS) 

$(BUILDDIR)/$(TARGET).bin: $(BUILDDIR)/$(TARGET).out
	$(OBJCOPY) -O binary $< $@

# create the output directories
$(BUILDDIR):
	$(MKDIR) $(BUILDDIR)

$(BINDIR):
	$(MKDIR) $(BINDIR)

clean:
	-$(RM) $(BUILDDIR)/*
	-$(RM) $(BINDIR)/*

.PHONY: all clean
