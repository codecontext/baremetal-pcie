#include "drivers/uart/uart.h"

/**
 * @brief Minimal C entry point for the bare-metal firmware.
 *
 * The startup assembly clears BSS and transfers control here.
 *
 * @return This function does not return under normal operation.
 */
int main(void)
{
    uart_init();
    uart_puts("baremetal-pcie: boot complete. Welcome! man\n");

    for (;;) {
    }

    return 0;
}
