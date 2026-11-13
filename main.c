#include "drivers/uart/uart.h"
#include "platform/qemu-virt/fdt.h"
#include "platform/qemu-virt/pci.h"

/**
 * @brief DTB address supplied by the boot environment.
 *
 * @details Startup stores the pointer passed in x0 here after clearing BSS and
 * before calling main().
 */
const void * volatile boot_device_tree_blob;

/**
 * @brief Minimal C entry point for the bare-metal firmware.
 *
 * @details Reads the DTB pointer saved by startup, validates the FDT header,
 * and uses the platform data for PCIe discovery.
 *
 * @return This function does not return under normal operation.
 */
int main(void)
{
    const void *device_tree_blob = boot_device_tree_blob;
    uint64_t memory_base = 0U;
    uint64_t memory_size = 0U;
    uint64_t pci_ecam_base = 0U;
    uint64_t pci_ecam_size = 0U;

    uart_init();
    uart_puts("\nSystem startup complete. Welcome!\n");

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

        bool scan_ok = pci_scan_root_bus(pci_ecam_base);
        if (scan_ok) {
            uart_puts("PCI scan: completed\n");
        } else {
            uart_puts("PCI scan: failed\n");
        }
    } else {
        uart_puts("FDT PCIe ECAM: unavailable\n");
    }

    for (;;) {
    }

    return 0;
}
