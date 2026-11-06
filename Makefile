CROSS_COMPILE ?= aarch64-linux-gnu-
CC := $(CROSS_COMPILE)gcc
OBJCOPY := $(CROSS_COMPILE)objcopy
QEMU ?= qemu-system-aarch64

BUILD_DIR := build
ELF := $(BUILD_DIR)/baremetal-pcie.elf
BIN := $(BUILD_DIR)/baremetal-pcie.bin
.DEFAULT_GOAL := $(BIN)

CFLAGS := -O2 -g -ffreestanding -fno-builtin -Wall -Wextra -Werror -nostdlib

ASM_SRCS := boot/startup.S
C_SRCS := main.c drivers/uart/uart.c
OBJS := $(BUILD_DIR)/boot/startup.o $(BUILD_DIR)/main.o $(BUILD_DIR)/drivers/uart/uart.o

.PHONY: help clean rebuild run debug

help:
	@echo "Bare-Metal PCIe Driver Development on QEMU"
	@echo ""
	@echo "Targets:"
	@echo "  make            Incrementally build the firmware when needed."
	@echo "  make rebuild    Clean and rebuild the firmware from scratch."
	@echo "  make run        Run the startup image in QEMU virt."
	@echo "  make debug      Run with a GDB stub waiting for a debugger."
	@echo "  make clean      Remove build artifacts."
	@echo ""
	@echo "Current status: startup, linker, and UART driver integration."

$(BUILD_DIR):
	@mkdir -p $(BUILD_DIR)
	@mkdir -p $(BUILD_DIR)/boot
	@mkdir -p $(BUILD_DIR)/drivers/uart

$(BUILD_DIR)/main.o: main.c drivers/uart/uart.h | $(BUILD_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD_DIR)/drivers/uart/uart.o: drivers/uart/uart.c drivers/uart/uart.h | $(BUILD_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD_DIR)/boot/startup.o: boot/startup.S | $(BUILD_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(ELF): $(OBJS) boot/linker.ld
	$(CC) -T boot/linker.ld -nostdlib -nostartfiles -static -o $@ $(OBJS)

$(BIN): $(ELF)
	$(OBJCOPY) -O binary $< $@

run: $(BIN)
	$(QEMU) -M virt -cpu cortex-a53 -nographic -monitor none -serial stdio -kernel $(BIN)

debug: $(BIN)
	$(QEMU) -M virt -cpu cortex-a53 -nographic -monitor none -serial stdio -kernel $(BIN) -S -gdb tcp::1234

clean:
	rm -rf $(BUILD_DIR)

rebuild:
	$(MAKE) clean
	$(MAKE)
