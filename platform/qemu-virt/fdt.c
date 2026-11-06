/**
 * @file fdt.c
 * @brief Minimal validation of a flattened device tree header.
 *
 * @details FDT header fields are stored in big-endian byte order. This initial
 * check validates the magic and total-size field before later code traverses
 * the structure block.
 */

#include <stdbool.h>
#include <stdint.h>

#include "fdt.h"

#define FDT_MAGIC 0xD00DFEEDU
#define FDT_HEADER_SIZE 40U

/**
 * @brief Read a 32-bit big-endian value from a byte buffer.
 *
 * @details Combines four consecutive bytes in most-significant-byte-first
 * order, as required for integer fields in the FDT header.
 *
 * @param bytes Pointer to at least four readable bytes.
 * @return The decoded 32-bit value in host integer representation.
 */
static uint32_t fdt_read_be32(const uint8_t *bytes)
{
    return ((uint32_t)bytes[0] << 24) |
           ((uint32_t)bytes[1] << 16) |
           ((uint32_t)bytes[2] << 8) |
           (uint32_t)bytes[3];
}

/**
 * @brief Check the FDT magic and minimum header size.
 *
 * @param blob Address of the FDT blob provided at firmware entry.
 * @return true if the blob begins with a minimally sized FDT header.
 * @return false if the pointer, magic, or header size is invalid.
 */
bool fdt_header_is_valid(const void *blob)
{
    const uint8_t *bytes = blob;

    if (bytes == 0) {
        return false;
    }

    if (fdt_read_be32(bytes) != FDT_MAGIC) {
        return false;
    }

    return fdt_read_be32(bytes + 4U) >= FDT_HEADER_SIZE;
}