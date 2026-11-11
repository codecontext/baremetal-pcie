#include "drivers/uart/uart.h"
#include "platform/qemu-virt/fdt.h"

/**
 * @brief Minimal C entry point for the bare-metal firmware.
 *
 * @details Validates the QEMU-provided FDT header and reads the memory node so
 * the firmware confirms the platform memory map before later PCIe work.
 *
 * @param device_tree_blob Address of the QEMU-provided flattened device tree.
 * @return This function does not return under normal operation.
 */
int main(const void *device_tree_blob)
{
    uint64_t memory_base = 0U;
    uint64_t memory_size = 0U;
    uint64_t pci_ecam_base = 0U;
    uint64_t pci_ecam_size = 0U;

    uart_init();
    uart_puts("Boot complete. Welcome!\n");

    if (fdt_header_is_valid(device_tree_blob)) {
        uart_puts("FDT header: valid\n");
    } else {
        uart_puts("FDT header: invalid\n");
    }

    if (fdt_get_memory_range(device_tree_blob, &memory_base, &memory_size)) {
        uart_puts("FDT memory: base=");
        uart_puthex64(memory_base);
        uart_puts(", size=");
        uart_puthex64(memory_size);
        uart_puts("\n");
    } else {
        uart_puts("FDT memory: unavailable\n");
    }

    if (fdt_get_pci_ecam_range(device_tree_blob, &pci_ecam_base, &pci_ecam_size)) {
        uart_puts("FDT PCIe ECAM: base=");
        uart_puthex64(pci_ecam_base);
        uart_puts(", size=");
        uart_puthex64(pci_ecam_size);
        uart_puts("\n");
    } else {
        uart_puts("FDT PCIe ECAM: unavailable\n");
    }

    for (;;) {
    }

    return 0;
}
