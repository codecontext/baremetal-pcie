/**
 * @file fdt.h
 * @brief Basic validation and property parsing for the QEMU-provided FDT.
 */

#ifndef QEMU_VIRT_FDT_H
#define QEMU_VIRT_FDT_H

#include <stdbool.h>
#include <stdint.h>

bool fdt_header_is_valid(const void *blob);
bool fdt_get_memory_range(const void *blob, uint64_t *base, uint64_t *size);

#endif /* QEMU_VIRT_FDT_H */