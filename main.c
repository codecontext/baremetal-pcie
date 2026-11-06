#include "drivers/uart/uart.h"
#include "platform/qemu-virt/fdt.h"

/**
 * @brief Minimal C entry point for the bare-metal firmware.
 *
 * @details Checks the QEMU-provided FDT header and reports the result over UART.
 *
 * @param device_tree_blob Address of the QEMU-provided flattened device tree.
 * @return This function does not return under normal operation.
 */
int main(const void *device_tree_blob)
{
    uart_init();
    uart_puts("Boot complete. Welcome!\n");

    if (fdt_header_is_valid(device_tree_blob)) {
        uart_puts("FDT header: valid\n");
    } else {
        uart_puts("FDT header: invalid\n");
    }

    for (;;) {
    }

    return 0;
}
