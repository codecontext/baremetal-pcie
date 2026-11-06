/**
 * @file fdt.h
 * @brief Basic validation for the QEMU-provided flattened device tree.
 */

#ifndef QEMU_VIRT_FDT_H
#define QEMU_VIRT_FDT_H

#include <stdbool.h>

bool fdt_header_is_valid(const void *blob);

#endif /* QEMU_VIRT_FDT_H */