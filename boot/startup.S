/*
 * Bare-metal AArch64 entry point for the QEMU virt machine.
 * This code runs before C initialization and must establish the
 * minimum execution state required for a freestanding firmware image.
 */
.section .text.boot

.global _start
.type _start, %function

_start:
    /* Preserve QEMU's DTB address passed in x0 across BSS initialization. */
    mov x19, x0

    /* Set the stack pointer to the highest valid RAM address. */
    ldr x30, =__stack_top
    mov sp, x30

    /*
     * Clear the BSS region to zero. This ensures static variables and
     * uninitialized storage start in a known state before main().
     */
    adrp x0, __bss_start
    add x0, x0, :lo12:__bss_start
    adrp x1, __bss_end
    add x1, x1, :lo12:__bss_end

1:
    cmp x0, x1
    b.hs 2f
    str xzr, [x0], #8
    b 1b

2:
    /* Pass the preserved DTB address to the C entry point in x0. */
    mov x0, x19
    bl main

3:
    /* Stay stopped if main() ever returns. */
    wfe
    b 3b

.size _start, . - _start
