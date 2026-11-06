/**
 * @file uart.h
 * @brief UART console interface for the QEMU virt platform.
 */

#ifndef UART_H
#define UART_H

void uart_init(void);
void uart_puts(const char *str);

#endif /* UART_H */