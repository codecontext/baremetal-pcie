CROSS_COMPILE ?= aarch64-linux-gnu-
BUILD_DIR := build
OUTPUT := $(BUILD_DIR)/baremetal-pcie.elf
QEMU ?= qemu-system-aarch64

.PHONY: all help clean run debug

all: help

help:
	@echo "Bare-Metal PCIe Driver Development on QEMU"
	@echo ""
	@echo "Targets:"
	@echo "  make help   Show the available project targets."
	@echo "  make clean  Remove build artifacts."
	@echo "  make run    Placeholder execution target for the bare-metal firmware."
	@echo "  make debug  Placeholder debugging target for QEMU/GDB workflows."
	@echo ""
	@echo "Current status: minimal startup and linker infrastructure only."

run:
	@echo "[INFO] Startup and linker scaffold is in place; UART and PCIe code"
	@echo "[INFO] will be added in later, smaller steps."

debug: run

clean:
	rm -rf $(BUILD_DIR)
