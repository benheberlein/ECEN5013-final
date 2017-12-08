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
VPATH=src
VPATH+=3p/FreeRTOS/Source
VPATH+=3p/FreeRTOS/Source/portable/GCC/ARM_CM4F
VPATH+=3p/FreeRTOS/Source/portable/MemMang/
VPATH+=3p/FreeRTOS-Plus/Source/FreeRTOS-Plus-TCP/portable/BufferManagement
VPATH+=3p/FreeRTOS-Plus/Source/FreeRTOS-Plus-TCP

# TARGET: name of the output file
TARGET = tiva
# MCU: part number to build for
MCU = TM4C1294XL
# SOURCES: list of input source sources
SOURCES = main.c \
		  startup_gcc.c

FREE_RTOS_SRCS = tasks.c \
              queue.c \
              list.c \
              heap_4.c \
              timers.c \
              croutine.c \
              event_groups.c \
              port.c

FREE_RTOS_TCP_SRCS = FreeRTOS_IP.c \
                    FreeRTOS_ARP.c \
                    FreeRTOS_DHCP.c \
                    FreeRTOS_DNS.c \
                    FreeRTOS_Sockets.c \
                    FreeRTOS_TCP_IP.c \
                    FreeRTOS_UDP_IP.c \
                    FreeRTOS_TCP_WIN.c \
                    FreeRTOS_Stream_Buffer.c \
                    NetworkInterface.c \
                    BufferAllocation_2.c \
                    eth.c

SOURCES += $(FREE_RTOS_SRCS) $(FREE_RTOS_TCP_SRCS)

# INCLUDES: list of includes, by default, use Includes directory
INCLUDES = -Iinc \
        -I3p/FreeRTOS/Source/include \
        -I/home/ben/Repos/tivaware \
        -I3p/FreeRTOS/Source/portable/GCC/ARM_CM4F \
        -I3p/FreeRTOS-Plus/Source/FreeRTOS-Plus-TCP/include \
        -I3p/FreeRTOS-Plus/Source/FreeRTOS-Plus-TCP/portable/Compiler/GCC
        
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
CFLAGS += -DTARGET_IS_TM4C129_RA1 -D__TI_VFP_SUPPORT__ -DPART_TM4C1294NCPDT
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
