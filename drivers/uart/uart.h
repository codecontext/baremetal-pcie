/**
 * @file uart.h
 * @brief UART console interface for the QEMU virt platform.
 */

#ifndef UART_H
#define UART_H

#include <stdint.h>

void uart_init(void);
void uart_puts(const char *str);
void uart_puthex32(uint32_t value);
void uart_puthex64(uint64_t value);

#endif /* UART_H */