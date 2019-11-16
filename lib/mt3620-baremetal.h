/* Copyright (c) Microsoft Corporation. All rights reserved.
   Licensed under the MIT License. */

#ifndef MT3620_BAREMETAL_H
#define MT3620_BAREMETAL_H

#include <stdint.h>
#include <stddef.h>

/// <summary>
/// Write the supplied 8-bit value to an address formed from the supplied base
/// address and offset.
/// </summary>
/// <param name="baseAddr">Typically the start of a register bank.</param>
/// <param name="offset">This value is added to the base address to form the target address.
/// It is typically the offset of a register within a bank.</param>
/// <param name="value">8-bit value to write to the target address.</param>
static inline void WriteReg8(uintptr_t baseAddr, size_t offset, uint8_t value)
{
    *(volatile uint8_t *)(baseAddr + offset) = value;
}

/// <summary>
/// Write the supplied 32-bit value to an address formed from the supplied base
/// address and offset.
/// </summary>
/// <param name="baseAddr">Typically the start of a register bank.</param>
/// <param name="offset">This value is added to the base address to form the target address.
/// It is typically the offset of a register within a bank.</param>
/// <param name="value">32-bit value to write to the target address.</param>
static inline void WriteReg32(uintptr_t baseAddr, size_t offset, uint32_t value)
{
    *(volatile uint32_t *)(baseAddr + offset) = value;
}

/// <summary>
/// Read a 32-bit value from an address formed from the supplied base
/// address and offset.
/// </summary>
/// <param name="baseAddr">Typically the start of a register bank.</param>
/// <param name="offset">This value is added to the base address to form the target address.
/// It is typically the offset of a register within a bank.</param>
/// <returns>An unsigned 32-bit value which is read from the target address.</returns>
static inline uint32_t ReadReg32(uintptr_t baseAddr, size_t offset)
{
    return *(volatile uint32_t *)(baseAddr + offset);
}

/// <summary>
/// <para>Read a 32-bit register from the supplied address, clear the supplied bits,
/// and write the new value back to the register.</para>
/// <para>This is not an atomic operation. If the value of the register is liable
/// to change between the read and write operations, the caller should use
/// appropriate locking.</para>
/// </summary>
/// <param name="baseAddr">Typically the start of a register bank.</param>
/// <param name="offset">This value is added to the base address to form the target address.
/// It is typically the offset of a register within a bank.</param>
/// <param name="clearBits">Bits which should be cleared in the final value.</param>
static inline void ClearReg32(uintptr_t baseAddr, size_t offset, uint32_t clearBits)
{
    uint32_t value = ReadReg32(baseAddr, offset);
    value &= ~clearBits;
    WriteReg32(baseAddr, offset, value);
}

/// <summary>
/// <para>Read a 32-bit register from the supplied address, set the supplied bits,
/// and write the new value back to the register.</para>
/// <para>This is not an atomic operation. If the value of the register is liable
/// to change between the read and write operations, the caller should use
/// appropriate locking.</para>
/// </summary>
/// <param name="baseAddr">Typically the start of a register bank.</param>
/// <param name="offset">This value is added to the base address to form the target address.
/// It is typically the offset of a register within a bank.</param>
/// <param name="setBits">Bits which should be cleared in the final value.</param>
static inline void SetReg32(uintptr_t baseAddr, size_t offset, uint32_t setBits)
{
    uint32_t value = ReadReg32(baseAddr, offset);
    value |= setBits;
    WriteReg32(baseAddr, offset, value);
}

#endif /* MT3620_BAREMETAL_H */
