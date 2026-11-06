# Bare-Metal PCIe Driver Development on QEMU

This repository is the starting point for a bare-metal AArch64 firmware project
that explores PCIe host initialization, device enumeration, BAR discovery, and
basic PCI configuration-space and MMIO access on the QEMU virt platform.

## Project purpose

The goal is to build a clean, modular embedded firmware project that runs on a
QEMU-emulated Cortex-A53 system and discovers a PCIe virtio-net endpoint behind
an emulated PCIe root port. The project deliberately starts with the host-side
firmware and PCIe discovery flow. It does not begin with a complete virtio
network stack or DMA-based data path.

## Current status

The project currently builds minimal AArch64 firmware for QEMU virt. Startup
initializes the stack, clears BSS, and passes QEMU's DTB pointer to `main()`.
The firmware initializes the PL011 UART and validates the FDT magic and minimum
header size. DTB property parsing and PCIe support remain future work.

## Repository structure

```text
baremetal-pcie/
├── boot/
├── arch/
│   └── aarch64/
│       ├── mmu/
│       └── gic/
├── drivers/
│   ├── uart/
│   │   ├── uart.c
│   │   └── uart.h
│   ├── pci/
│   └── virtio/
│       └── pci/
├── lib/
│   ├── string/
│   ├── memory/
│   └── printf/
├── platform/
│   └── qemu-virt/
│       ├── fdt.c
│       └── fdt.h
├── tests/
│   └── pci/
├── scripts/
├── docs/
├── .gitignore
├── LICENSE
├── Makefile
├── README.md
└── .git/
```

## Directory purpose

- `boot/`: AArch64 startup assembly, vector tables, linker script, and early boot
  setup.
- `arch/`: Architecture-specific CPU and exception helpers.
- `drivers/`: Hardware-specific drivers, with each driver's public header beside
  its implementation (for example, `drivers/uart/uart.c` and `uart.h`).
- `lib/`: Generic utility code such as memory helpers and minimal formatting.
- `platform/`: QEMU virt platform discovery; currently contains basic FDT header
  validation, with resource parsing still pending.
- `tests/`: Validation and functional checks for each milestone.
- `scripts/`: Local automation for build, run, and debug workflows.
- `docs/`: Architecture notes, setup guides, and PCIe reference material.

## Development environment

This project is designed for the host environment described in the project brief:

- Ubuntu 24.04 LTS
- x86_64 host
- aarch64-linux-gnu-gcc 13.3.0
- QEMU 8.2.2
- GNU Make 4.3
- Git and VS Code

## Build instructions

```bash
cd /home/dask/work/baremetal-pcie
```

Available commands:

- `make`: Incrementally build the firmware, rebuilding only missing or outdated outputs.
- `make help`: List the available Make targets.
- `make clean`: Remove generated files from `build/`.
- `make rebuild`: Clean generated files, then rebuild the firmware from scratch.
- `make run`: Run the firmware in QEMU virt and connect its UART to the terminal.
- `make debug`: Start QEMU paused with a GDB server on port 1234.

The build produces `build/baremetal-pcie.elf` for symbols and debugging and
`build/baremetal-pcie.bin`, the raw image loaded by QEMU.

## Documentation

The initial setup guide is in:

- `docs/development-setup.md`

Additional architecture and PCIe reference documents will be added as each phase
is implemented.

## Planned work

The project roadmap follows the phases described in the project brief:

1. Repository foundation and setup
2. Bare-metal boot infrastructure
3. QEMU platform and DTB initialization
4. PCIe configuration-space access
5. PCIe enumeration and bridge traversal
6. BAR and capability discovery
7. Basic read/write validation and virtio discovery
8. Future networking milestones only after the initial milestone is stable

## Notes

The current milestone is limited to startup, linker layout, and UART console
output. PCIe functionality will be added in small, independently validated
steps.
