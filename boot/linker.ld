/*
 * Minimal linker script for the QEMU virt bare-metal firmware.
 * The image is linked to the emulated memory region used by the
 * virt machine, and the stack is placed at the top of RAM.
 */
ENTRY(_start)

MEMORY
{
    /* QEMU virt maps RAM to a flat region that is large enough for the firmware. */
    RAM (rwx) : ORIGIN = 0x40000000, LENGTH = 0x08000000
}

SECTIONS
{
    . = ORIGIN(RAM);

    /* Keep the entry code in the first executable section. */
    .text :
    {
        *(.text.boot)
        *(.text*)
    } > RAM

    .rodata :
    {
        *(.rodata*)
    } > RAM

    .data :
    {
        *(.data*)
    } > RAM

    /*
     * BSS is zero-initialized by the assembly startup code before C code runs.
     * The linker defines the start and end symbols used by the boot sequence.
     */
    .bss (NOLOAD):
    {
        __bss_start = .;
        *(.bss*)
        *(COMMON)
        . = ALIGN(8);
        __bss_end = .;
    } > RAM

    . = ALIGN(8);
    __stack_top = ORIGIN(RAM) + LENGTH(RAM) - 16;

    /DISCARD/ :
    {
        *(.comment)
        *(.note*)
    }
}
