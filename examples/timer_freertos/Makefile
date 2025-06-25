CROSS=/home/abishekss/tools/riscv/bin/riscv32-unknown-elf-
CC=$(CROSS)gcc
AS=$(CROSS)as
LD=$(CROSS)ld
OBJCOPY=$(CROSS)objcopy
OBJDUMP=$(CROSS)objdump

PROGRAM=hello_freertos
TARGET=$(BUILD_DIR)/$(PROGRAM).elf

ROOT_PATH=../..
DRIVER_PATH=$(ROOT_PATH)/drivers
FREERTOS_PATH=$(ROOT_PATH)/FreeRTOS-Kernel

FILES := \
	main.c \
	port.c \
	tasks.c \
	queue.c \
	list.c \
	timers.c \
	heap_4.c \
	log.c \
	uart.c \

ASMFILES := \
	start.S \
	portASM.S \

FILES_PATH := \
	$(FREERTOS_PATH)/portable/GCC/RISC-V/ \
	$(FREERTOS_PATH)/portable/MemMang \
	$(FREERTOS_PATH)/ \
	$(DRIVER_PATH)/ \

INCLUDES=-I$(FREERTOS_PATH)/include \
	-I$(FREERTOS_PATH)/portable/GCC/RISC-V \
	-I$(FREERTOS_PATH)/portable/GCC/RISC-V/chip_specific_extensions/RISCV_no_extensions \
	-I$(DRIVER_PATH)/ \
	-I. \

CFLAGS=-march=rv32imafd -mabi=ilp32d -O0 -g -Wall
ASMFLAGS=-march=rv32imafd -mabi=ilp32d -g
LDFLAGS=-march=rv32imafd -mabi=ilp32d -Tlinker.ld -nostartfiles

BUILD_DIR=build
OBJ_DIR=build/obj/

# Object and dependency files
OBJS := $(FILES:%.c=%.obj)
OBJS += $(ASMFILES:%.S=%.obj)
DEPS := $(FILES:%.c=%.d)

# Virtual paths for source and object files
vpath %.obj $(OBJ_DIR)
vpath %.c $(FILES_PATH)
vpath %.S $(FILES_PATH)

# Compilation rule for C files
$(OBJ_DIR)/%.obj %.obj: %.c
	@echo Compiling: $(LIBNAME): $<
	$(CC) -c $(CFLAGS) $(INCLUDES) $(DEFINES) -MMD -MT $@ -o $(OBJ_DIR)/$@ $<

# Compilation rule for assembly files
$(OBJ_DIR)/%.obj %.obj: %.S
	@echo Compiling: $(LIBNAME): $<
	$(CC) -c -x assembler-with-cpp $(CFLAGS) $(INCLUDES) $(DEFINES) -o $(OBJ_DIR)/$@ $<

# Default target
all: $(PROGRAM)

# Library creation
$(PROGRAM): $(OBJS) | $(BUILD_DIR)
	@echo Linking: $(PROGRAM)
	$(CC) $(LDFLAGS) -o $(BUILD_DIR)/$(PROGRAM).elf $(addprefix $(OBJ_DIR), $(OBJS)) 
	$(OBJCOPY) -O binary $(BUILD_DIR)/$(PROGRAM).elf $(BUILD_DIR)/$(PROGRAM).bin
	$(OBJDUMP) -S -D $(BUILD_DIR)/$(PROGRAM).elf > $(BUILD_DIR)/$(PROGRAM).lst
	@echo "Build complete."
	@echo .


clean:
	rm -rf $(BUILD_DIR)

run: $(BINARY)
	/home/abishekss/tools/qemu/build/qemu-system-riscv32 -machine virt -nographic -bios none -kernel $(TARGET)

debug: $(BINARY)
	/home/abishekss/tools/qemu/build/qemu-system-riscv32 -machine virt -nographic -bios none -kernel $(TARGET) -s -S

gdb:
	@echo "Starting GDB..."
	/home/abishekss/tools/riscv/bin/riscv32-unknown-elf-gdb $(TARGET) -ex "target remote localhost:1234" -ex "break _start" -ex "continue"
	@echo "GDB session ended."

.PHONY: all clean run debug gdb

$(OBJS): | $(OBJ_DIR)

$(LIB_DIR) $(OBJ_DIR):
	mkdir -p $@

-include $(addprefix $(OBJDIR)/, $(DEPS))