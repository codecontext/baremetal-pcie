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
	@echo "Current status: infrastructure-only foundation; firmware sources are not"
	@echo "yet implemented in this Phase 0 step."

run:
	@echo "[INFO] Phase 0 foundation only: no firmware has been added yet."
	@echo "[INFO] The first implementation step is repository scaffolding and setup."

debug: run

clean:
	rm -rf $(BUILD_DIR)
