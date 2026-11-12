/**
 * @file pci.h
 * @brief Minimal PCI configuration-space access helpers for QEMU virt ECAM.
 */

#ifndef QEMU_VIRT_PCI_H
#define QEMU_VIRT_PCI_H

#include <stdbool.h>
#include <stdint.h>

bool pci_ecam_config_read32(uint64_t ecam_base,
                           uint32_t bus,
                           uint32_t device,
                           uint32_t function,
                           uint32_t offset,
                           uint32_t *value);

bool pci_ecam_config_write32(uint64_t ecam_base,
                            uint32_t bus,
                            uint32_t device,
                            uint32_t function,
                            uint32_t offset,
                            uint32_t value);

bool pci_scan_root_bus(uint64_t ecam_base);

#endif /* QEMU_VIRT_PCI_H */
