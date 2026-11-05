# Development setup

This document captures the host and toolchain requirements for the
bare-metal PCIe project.

## Host environment

- Ubuntu 24.04 LTS
- x86_64 host architecture
- GNU Make 4.3
- Git
- VS Code

## Required tools

Install the cross-compilation and emulation toolchain:

```bash
sudo apt update
sudo apt install -y build-essential git qemu-system-arm qemu-utils \
    gcc-aarch64-linux-gnu binutils-aarch64-linux-gnu
```

## Verify installed versions

```bash
qemu-system-aarch64 --version
arm-none-eabi-gcc --version || true
aarch64-linux-gnu-gcc --version
make --version
```

## Expected QEMU platform

The project targets the `virt` machine with a Cortex-A53 CPU and the
QEMU-provided PCIe root complex. Initial development keeps the environment
strictly software-based and avoids any physical PCIe device dependency.

## Repository workflow

This repository is intentionally structured around small, reviewable steps.
Each implementation step adds only the code and documentation needed for the
current milestone.

## Build and verification

The repository uses a lightweight Make-based workflow for foundation checks:

```bash
cd /home/dask/work/baremetal-pcie
make help
make clean
```

The current Phase 0 state is intentionally a scaffold only. Firmware sources and
QEMU boot steps will be added after the initial repository foundation is reviewed.
