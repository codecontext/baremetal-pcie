/**
 * @file fdt.c
 * @brief Minimal validation and property parsing for a flattened device tree.
 *
 * @details The firmware only needs enough of the FDT format to validate the
 * header and read the memory node's REG property. The DTB uses big-endian cell
 * values and the structure block is walked token by token.
 */

#include <stdbool.h>
#include <stdint.h>

#include "fdt.h"

#define FDT_MAGIC 0xD00DFEEDU
#define FDT_HEADER_SIZE 40U
#define FDT_BEGIN_NODE 0x00000001U
#define FDT_END_NODE 0x00000002U
#define FDT_PROP 0x00000003U
#define FDT_END 0x00000009U

/**
 * @brief Read a 32-bit big-endian value from a byte buffer.
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
 * @brief Read a 64-bit big-endian value from a byte buffer.
 *
 * @param bytes Pointer to at least eight readable bytes.
 * @return The decoded 64-bit value in host integer representation.
 */
static uint64_t fdt_read_be64(const uint8_t *bytes)
{
    return ((uint64_t)fdt_read_be32(bytes) << 32) |
           (uint64_t)fdt_read_be32(bytes + 4U);
}

/**
 * @brief Align a byte count to the next 32-bit boundary.
 *
 * @param value Length in bytes.
 * @return The aligned length in bytes.
 */
static uint32_t fdt_align4(uint32_t value)
{
    return (value + 3U) & ~3U;
}

/**
 * @brief Match the generic "memory" node name used by device-tree memory nodes.
 *
 * @param name Node name to test.
 * @return true when the node name is memory or memory@... .
 */
static bool fdt_name_is_memory(const char *name)
{
    const char *cursor = name;

    if (name == 0) {
        return false;
    }

    if (cursor[0] != 'm' || cursor[1] != 'e' || cursor[2] != 'm' ||
        cursor[3] != 'o' || cursor[4] != 'r' || cursor[5] != 'y') {
        return false;
    }

    return cursor[6] == '\0' || cursor[6] == '@';
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

/**
 * @brief Read the REG property from the memory node of the device tree.
 *
 * @details QEMU virt exposes memory as a single memory@... node with a "reg"
 * property describing the base address and size in big-endian 64-bit cells.
 *
 * @param blob Pointer to the flattened device tree blob.
 * @param base Output base address of the system RAM.
 * @param size Output size of the RAM region in bytes.
 * @return true when the memory node and REG property are successfully parsed.
 */
bool fdt_get_memory_range(const void *blob, uint64_t *base, uint64_t *size)
{
    const uint8_t *bytes = blob;
    const uint8_t *cursor;
    const uint8_t *strings;
    const uint8_t *end;
    uint32_t total_size;
    uint32_t off_dt_struct;
    uint32_t off_dt_strings;
    bool inside_memory_node = false;

    if (bytes == 0 || base == 0 || size == 0) {
        return false;
    }

    if (!fdt_header_is_valid(blob)) {
        return false;
    }

    total_size = fdt_read_be32(bytes + 4U);
    off_dt_struct = fdt_read_be32(bytes + 8U);
    off_dt_strings = fdt_read_be32(bytes + 12U);
    strings = bytes + off_dt_strings;
    end = bytes + total_size;
    cursor = bytes + off_dt_struct;

    while (cursor < end) {
        uint32_t token = fdt_read_be32(cursor);
        cursor += 4U;

        switch (token) {
        case FDT_BEGIN_NODE: {
            const char *name = (const char *)cursor;
            uint32_t name_length = 0U;

            while (cursor[name_length] != '\0') {
                name_length++;
            }

            inside_memory_node = fdt_name_is_memory(name);
            cursor += fdt_align4(name_length + 1U);
            break;
        }

        case FDT_PROP: {
            uint32_t length = fdt_read_be32(cursor);
            uint32_t name_offset = fdt_read_be32(cursor + 4U);
            const char *property_name = (const char *)(strings + name_offset);
            const uint8_t *property_data = cursor + 8U;

            if (inside_memory_node && property_name[0] == 'r' &&
                property_name[1] == 'e' && property_name[2] == 'g' &&
                property_name[3] == '\0') {
                if (length >= 16U) {
                    *base = fdt_read_be64(property_data);
                    *size = fdt_read_be64(property_data + 8U);
                    return true;
                }

                if (length >= 8U) {
                    *base = fdt_read_be32(property_data);
                    *size = fdt_read_be32(property_data + 4U);
                    return true;
                }
            }

            cursor += fdt_align4(8U + length);
            break;
        }

        case FDT_END_NODE:
            inside_memory_node = false;
            break;

        case FDT_END:
            return false;

        default:
            break;
        }
    }

    return false;
}