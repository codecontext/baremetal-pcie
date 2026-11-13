/**
 * @file pci.c
 * @brief Minimal ECAM-based PCI discovery for the QEMU virt machine.
 *
 * @details This firmware only needs to initialize the root port exposed by the
 * virt machine and scan the first downstream bus for a small set of known QEMU
 * devices. The goal is to keep the PCI logic minimal and focused on the real
 * POC target rather than full general-purpose enumeration.
 */

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "drivers/uart/uart.h"
#include "pci.h"

#define PCI_VENDOR_ID_OFFSET         0x00U
#define PCI_BRIDGE_CONTROL_OFFSET    0x18U
#define PCI_INVALID_VENDOR_ID        0xFFFFU
#define PCI_MAX_BUS                  2U
#define PCI_MAX_DEVICE               32U
#define PCI_MAX_FUNCTION             1U

struct pci_device_name {
    uint16_t vendor_id;
    uint16_t device_id;
    const char *name;
};

static const struct pci_device_name pci_device_names[] = {
    { 0x1B36U, 0x0008U, "qemu-host-bridge" },    // QEMU virt host bridge
    { 0x1B36U, 0x000CU, "pcie-root-port" },      // QEMU virt root port
    { 0x1234U, 0x11E8U, "edu" },                 // QEMU virt EDU device
    { 0x1AF4U, 0x1041U, "virtio-net-pci" },      // QEMU virt virtio-net device
};

/**
 * @brief Find the display name for a known PCI device.
 *
 * @param vendor_id PCI vendor identifier.
 * @param device_id PCI device identifier.
 * @return The matching device name, or "unknown" if the IDs are not listed.
 */
static const char *pci_lookup_device_name(uint16_t vendor_id, uint16_t device_id)
{
    size_t index;

    // Search the known device list for a matching vendor and device ID.
    for (index = 0U; index < sizeof(pci_device_names) / sizeof(pci_device_names[0]); index++) {
        if (pci_device_names[index].vendor_id == vendor_id &&
            pci_device_names[index].device_id == device_id) {
            return pci_device_names[index].name;
        }
    }

    return "unknown";
}

/**
 * @brief Configure bus numbering on the QEMU PCIe root port.
 *
 * @details Assigns bus 1 as the secondary bus and verifies the configuration
 * by reading the bus-number register back.
 *
 * @param ecam_base Base address of the ECAM window.
 * @return true if the root-port register was written and read back correctly.
 */
static bool pci_program_root_port(uint64_t ecam_base)
{
    uint32_t bridge_config;
    uint32_t bridge_config_readback;

    if (!pci_ecam_config_read32(ecam_base, 0U, 1U, 0U,
                                PCI_BRIDGE_CONTROL_OFFSET, &bridge_config)) {
        return false;
    }

    
    bridge_config = (bridge_config & 0xFF000000U) |
                    (1U << 16) |
                    (1U << 8);

    if (!pci_ecam_config_write32(ecam_base, 0U, 1U, 0U,
                                 PCI_BRIDGE_CONTROL_OFFSET, bridge_config) ||
        !pci_ecam_config_read32(ecam_base, 0U, 1U, 0U,
                                PCI_BRIDGE_CONTROL_OFFSET,
                                &bridge_config_readback)) {
        return false;
    }

    return bridge_config_readback == bridge_config;
}

/**
 * @brief Compute the ECAM address for a given PCI function.
 *
 * @param ecam_base Base address of the ECAM window.
 * @param bus Bus number in the root complex.
 * @param device Device number within the bus.
 * @param function Function number within the device.
 * @param offset Register offset within the config space header.
 * @return The CPU address for the selected configuration-space register.
 */
static uintptr_t pci_ecam_address(uint64_t ecam_base,
                                 uint32_t bus,
                                 uint32_t device,
                                 uint32_t function,
                                 uint32_t offset)
{
    uint64_t bus_offset;

    bus_offset = ((uint64_t)bus << 20) |
                 ((uint64_t)device << 15) |
                 ((uint64_t)function << 12) |
                 ((uint64_t)offset & 0xFFFU);

    return (uintptr_t)(ecam_base + bus_offset);
}

/**
 * @brief Read a 32-bit PCI configuration-space register from ECAM.
 *
 * @param ecam_base Base address of the ECAM window.
 * @param bus Bus number to access.
 * @param device Device number to access.
 * @param function Function number to access.
 * @param offset Register offset within the selected function header.
 * @param value Output register value.
 * @return true if the arguments pass validation and the register is read.
 */
bool pci_ecam_config_read32(uint64_t ecam_base,
                           uint32_t bus,
                           uint32_t device,
                           uint32_t function,
                           uint32_t offset,
                           uint32_t *value)
{
    uintptr_t address;

    if (value == (uint32_t *)0) {
        return false;
    }

    if (ecam_base == 0U) {
        return false;
    }

    if (bus >= PCI_MAX_BUS || device >= PCI_MAX_DEVICE ||
        function >= PCI_MAX_FUNCTION || (offset & 0x3U) != 0U) {
        return false;
    }

    address = pci_ecam_address(ecam_base, bus, device, function, offset);
    *value = *(volatile uint32_t *)address;
    return true;
}

/**
 * @brief Write a 32-bit PCI configuration-space register through ECAM.
 *
 * @param ecam_base Base address of the ECAM window.
 * @param bus Bus number to access.
 * @param device Device number to access.
 * @param function Function number to access.
 * @param offset Aligned register offset within the selected function header.
 * @param value Value to write to the register.
 * @return true if the arguments pass validation and the register is written.
 */
bool pci_ecam_config_write32(uint64_t ecam_base,
                            uint32_t bus,
                            uint32_t device,
                            uint32_t function,
                            uint32_t offset,
                            uint32_t value)
{
    uintptr_t address;

    if (ecam_base == 0U) {
        return false;
    }

    if (bus >= PCI_MAX_BUS || device >= PCI_MAX_DEVICE ||
        function >= PCI_MAX_FUNCTION || (offset & 0x3U) != 0U) {
        return false;
    }

    address = pci_ecam_address(ecam_base, bus, device, function, offset);
    *(volatile uint32_t *)address = value;
    return true;
}

/**
 * @brief Scan the root bus for a visible PCI device.
 *
 * @details Scan bus 0 for the host bridge and root port, then bus 1 for devices
 * behind the configured root port. Only function 0 is checked for each slot.
 *
 * @param ecam_base Base address of the ECAM window.
 * @return true if root-port setup and all configuration reads complete.
 */
bool pci_scan_root_bus(uint64_t ecam_base)
{
    uint32_t bus;
    uint32_t device;
    uint32_t function = 0U;
    uint32_t id_reg;
    uint16_t vendor_id;
    uint16_t device_id;
    bool found_device = false;

    if (!pci_program_root_port(ecam_base)) {
        uart_puts("root port init failed\n");
        return false;
    }

    for (bus = 0U; bus < PCI_MAX_BUS; bus++) {
        uart_puts("\nScanning bus ");
        uart_putuint32(bus);
        uart_puts("\n");
        for (device = 0U; device < PCI_MAX_DEVICE; device++) {
            if (!pci_ecam_config_read32(ecam_base, bus, device, function,
                                        PCI_VENDOR_ID_OFFSET, &id_reg)) {
                uart_puts("ECAM read failed\n");
                return false;
            }

            vendor_id = (uint16_t)(id_reg & 0xFFFFU);
            device_id = (uint16_t)((id_reg >> 16) & 0xFFFFU);

            if (vendor_id == PCI_INVALID_VENDOR_ID) {
                continue;
            }

            uart_puts("Found bus=");
            uart_putuint32(bus);
            uart_puts(" dev=");
            uart_putuint32(device);
            uart_puts(" func=");
            uart_putuint32(function);
            uart_puts(" vend=");
            uart_puthex32(vendor_id);
            uart_puts(" dev_id=");
            uart_puthex32(device_id);
            uart_puts(" name=");
            uart_puts(pci_lookup_device_name(vendor_id, device_id));
            uart_puts("\n");
            found_device = true;
        }
    }

    if (!found_device) {
        uart_puts("no devices on buses 0-1\n");
    }

    return true;
}
