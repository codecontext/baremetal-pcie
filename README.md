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

## Current Phase 0 status

This repository has been initialized with the minimal project foundation only:

- repository layout and directory structure
- project documentation
- build entry points and clean targets
- development environment notes

The actual bare-metal boot firmware and PCIe driver code will be added in later
phases after review and approval.

## Final repository structure

```text
baremetal-pcie/
├── boot/
├── arch/
│   └── aarch64/
│       ├── mmu/
│       └── gic/
├── drivers/
│   ├── uart/
│   ├── pci/
│   └── virtio/
│       └── pci/
├── lib/
│   ├── string/
│   ├── memory/
│   └── printf/
├── platform/
│   └── qemu-virt/
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
- `platform/`: QEMU virt platform discovery and DTB/ACPI style resource mapping.
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
make help
make clean
```

This is intentionally a minimal build scaffold for the repository foundation.
Actual firmware compilation will be added once the bare-metal boot and PCIe code
is ready for implementation.

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

This repository is intentionally kept minimal at this stage. It avoids creating
future source placeholders without a clear implementation path. The next step,
after review, is to add the AArch64 startup and boot infrastructure.
