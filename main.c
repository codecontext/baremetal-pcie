/**
 * @brief Minimal C entry point for the bare-metal firmware.
 *
 * The startup assembly clears BSS and transfers control here. This file is
 * intentionally minimal while the project remains in the initial boot-only
 * milestone.
 *
 * @return This function never returns under normal operation.
 */
int main(void)
{
    for (;;) {
    }

    return 0;
}
