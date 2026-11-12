#include <stdint.h>

#include "uart.h"

/* Base address of the QEMU virt PL011 UART0. */
#define UART0_BASE 0x09000000UL

/* Data register: write transmitted bytes here. */
#define UART_DR            (UART0_BASE + 0x00U)

/* Flag register: reports UART transmit and receive status. */
#define UART_FR            (UART0_BASE + 0x18U)

/* Integer part of the baud-rate divisor. */
#define UART_IBRD          (UART0_BASE + 0x24U)

/* Fractional part of the baud-rate divisor. */
#define UART_FBRD          (UART0_BASE + 0x28U)

/* Line-control register: configures framing and word length. */
#define UART_LCRH          (UART0_BASE + 0x2CU)

/* Control register: enables the UART and its transmitter. */
#define UART_CR            (UART0_BASE + 0x30U)

/* TX FIFO full status bit in the flag register. */
#define UART_FR_TXFF       (1U << 5)

/* Word-length field value selecting 8-bit words. */
#define UART_LCRH_WLEN_8   (3U << 5)

/* UART enable bit in the control register. */
#define UART_CR_UARTEN     (1U << 0)

/* Transmitter enable bit in the control register. */
#define UART_CR_TXE        (1U << 8)

static inline uint32_t uart_read_reg(uintptr_t address)
{
    return *(volatile uint32_t *)address;
}

static inline void uart_write_reg(uintptr_t address, uint32_t value)
{
    *(volatile uint32_t *)address = value;
}

/**
 * @brief Initialize the ARM PL011 UART used by the QEMU virt machine.
 *
 * The virt board exposes a memory-mapped PL011 at 0x09000000. The UART is
 * configured for 115200 baud, 8N1 framing, and transmit enabled.
 */
void uart_init(void)
{
    uart_write_reg(UART_CR, 0U);

    /* baud = UART clock / (16 * (IBRD + FBRD / 64)). */
    /* For a 24 MHz clock, IBRD=13 and FBRD=1 give about 115200 baud. */
    uart_write_reg(UART_IBRD, 13U);
    uart_write_reg(UART_FBRD, 1U);

    uart_write_reg(UART_LCRH, UART_LCRH_WLEN_8);
    uart_write_reg(UART_CR, UART_CR_UARTEN | UART_CR_TXE);
}

/**
 * @brief Transmit a single byte over the PL011 UART.
 *
 * @param ch Character to send.
 */
static void uart_putc(char ch)
{
    while ((uart_read_reg(UART_FR) & UART_FR_TXFF) != 0U) {
    }

    uart_write_reg(UART_DR, (uint32_t)(unsigned char)ch);
}

/**
 * @brief Transmit a null-terminated string over the PL011 UART.
 *
 * @param str String to print.
 */
void uart_puts(const char *str)
{
    while (*str != '\0') {
        uart_putc(*str++);
    }
}

/**
 * @brief Print a 32-bit value in hexadecimal form.
 *
 * @param value Number to render with a 0x prefix.
 */
void uart_puthex32(uint32_t value)
{
    static const char digits[] = "0123456789abcdef";
    int shift;

    uart_puts("0x");

    for (shift = 28; shift >= 0; shift -= 4) {
        uart_putc(digits[(value >> shift) & 0xFU]);
    }
}

/**
 * @brief Print a 64-bit value in hexadecimal form.
 *
 * @param value Number to render with a 0x prefix.
 */
void uart_puthex64(uint64_t value)
{
    static const char digits[] = "0123456789abcdef";
    int shift;

    uart_puts("0x");

    for (shift = 60; shift >= 0; shift -= 4) {
        uart_putc(digits[(value >> shift) & 0xFU]);
    }
}
