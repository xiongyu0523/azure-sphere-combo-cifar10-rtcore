/* Copyright (c) Microsoft Corporation. All rights reserved.
   Licensed under the MIT License. */

#ifndef AZURE_SPHERE_SPIMASTER_H_
#define AZURE_SPHERE_SPIMASTER_H_

#include "Common.h"
#include "Platform.h"
#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>


/// <summary>Errors specific for SPI.</summary>
#define ERROR_SPI_MODE          (ERROR_SPECIFIC - 1)
#define ERROR_SPI_FRAME_FORMAT  (ERROR_SPECIFIC - 2)
#define ERROR_SPI_DATA_BITS     (ERROR_SPECIFIC - 3)
#define ERROR_SPI_BIT_ORDER     (ERROR_SPECIFIC - 4)
#define ERROR_SPI_CS_SELECT     (ERROR_SPECIFIC - 5)
#define ERROR_SPI_TRANSFER_FAIL (ERROR_SPECIFIC - 6)

/// <summary>SPI clock max speed and polarity settings.</summary>
#define CS_LINE0    (0)
#define CS_LINE1    (1)
#define CS_NONE     (7)
#define CPOL0       (0)
#define CPHA0       (0)
#define CPOL1       (1)
#define CPHA1       (1)

/// <summary>Opaque SPI Master handle.</summary>
typedef struct SpiMaster SpiMaster;

/// <summary>SPI transfer entry, used for queueing multiple transfers.</summary>
typedef struct {
    /// <summary>
    /// <para>Pointer to data to be transmitted.</para>
    /// <para>The writeData is a variable for transfering data to the SDOR buffer.</para>
    /// </summary>
    const void *writeData;
    /// <summary>
    /// <para>Pointer to buffer where received data will be written.</para>
    /// <para>The readData is a variable for receiving data from the SDIR buffer.</para>
    /// </summary>
    void *readData;
    /// <summary>
    /// <para>Length of data to be transmitted or received.</para>
    /// </summary>
    uintptr_t length;
} SpiTransfer;

/// <summary>
/// <para>Sets the subordinate device select channel.</para>
/// </summary>
/// <param name="handle">Selects the SPI handle to be configured.</param>
/// <param name="csLine">Selects which subordinate device line that the selected master is connected to</param>
/// <returns>Returns ERROR_NONE on success, or an error code on failure.</returns>
int32_t SpiMaster_Select(SpiMaster *handle, unsigned csLine);

/// <summary>
/// <para>Sets the configuration parameters for the SPI transaction.</para>
/// </summary>
/// <param name="handle">Selects the SPI handle to be configured.</param>
/// <param name="cPol">Selects the SPI clock polarity.</param>
/// <param name="cPha">Selects SPI clock phase.</param>
/// <param name="busSpeed">Selects the closest compatible baud rate below this value.</param>
/// <returns>Returns ERROR_NONE on success, or an error code on failure.</returns>
int32_t SpiMaster_Configure(SpiMaster *handle, bool cpol, bool cpha, uint32_t busSpeed);

/// <summary>
/// <para>Enable or disable DMA acceleration for the SPI device.</para>
/// </summary>
/// <param name="handle">Selects the SPI handle for which DMA will be configured.</param>
/// <param name="enable">Select whether the DMA unit will be enabled or not.</param>
/// <returns>Returns ERROR_NONE on success, or an error code on failure.</returns>
int32_t SpiMaster_ConfigDMA(SpiMaster *handle, bool enable);

/// <summary>
/// <para>Initialises the SPI communication protocol and aquire a
/// handle before using a given SPI interface.</para>
/// </summary>
/// <param name="id">Which SPI interface to initialize and acquire a handle for.</param>
/// <returns>A handle of an SPI interface or NULL on failure.</returns>
SpiMaster *SpiMaster_Open(Platform_Unit unit);

/// <summary>
/// <para>Uninitialises the SPI communication protocol.</para>
/// </summary>
/// <param name="handle">Selects the SPI handle to be uninitialised.</param>
void SpiMaster_Close(SpiMaster *handle);

/// <summary>
/// <para>Executes a sequence of asynchronous SPI transactions
/// (Read, Write or WriteThenRead) on the interface provided.</para>
/// </summary>
/// <param name="handle">Selects the SPI handle to perform the transfer on.</param>
/// <param name="transfer">A pointer to the base of an array of transfers, for more information
/// look at <see cref="SpiTransfer"/>.</param>
/// <param name="count">The total number of transfers in the transfer array.</param>
/// <param name="callback">A pointer to a function which will be called once the transfer is completed.</param>
/// <returns>Returns ERROR_NONE on success, or an error code on failure.</returns>
int32_t SpiMaster_TransferSequentialAsync(
    SpiMaster *handle, SpiTransfer *transfer,
    uint32_t count, void (*callback)(int32_t status, uintptr_t data_count));

/// <summary>
/// <para>Executes a single write operation on the SPI interface provided.</para>
/// </summary>
/// <param name="handle">Selects the SPI handle to perform the transfer on.</param>
/// <param name="data">A pointer to the data to be written.</param>
/// <param name="length">The length of the data to be written.</param>
/// <param name="callback">A pointer to a function which will be called once the transfer is completed.</param>
/// <returns>Returns ERROR_NONE on success, or an error code on failure.</returns>
static inline int32_t SpiMaster_WriteAsync(
    SpiMaster *handle, const void *data,
    uintptr_t length, void (*callback)(int32_t status, uintptr_t dataCount));

/// <summary>
/// <para>Executes a single read operation on the SPI interface provided.</para>
/// </summary>
/// <param name="handle">Selects the SPI handle to perform the transfer on.</param>
/// <param name="data">A pointer to the data to be read.</param>
/// <param name="length">The length of the data to be read.</param>
/// <param name="callback">A pointer to a function which will be called once the transfer is completed.</param>
/// <returns>Returns ERROR_NONE on success, or an error code on failure.</returns>
static inline int32_t SpiMaster_ReadAsync(
    SpiMaster *handle, void *data,
    uintptr_t length, void (*callback)(int32_t status, uintptr_t dataCount));

/// <summary>
/// <para>Executes back-to-back write then read operations on the SPI interface provided.</para>
/// </summary>
/// <param name="handle">Selects the SPI handle to perform the transfer on.</param>
/// <param name="writeData">A pointer to the data to be written</param>
/// <param name="writeLength">The length of the data to be written.</param>
/// <param name = "readData">A pointer to the data to be read.< / param>
/// <param name="readLength">The length of the data to be read.</param>
/// <param name="callback">A pointer to a function which will be called once the transfer is completed.</param>
/// <returns>Returns ERROR_NONE on success, or an error code on failure.</returns>
static inline int32_t SpiMaster_WriteThenReadAsync (
    SpiMaster *handle, const void *writeData,
    uintptr_t writeLength, void *readData, uintptr_t readLength,
    void (*callback)(int32_t status, uintptr_t dataCount));

/// <summary>
/// <para>Executes a sequence of SPI operations on the interface provided.</para>
/// <para>This is a synchronous wrapper around <see cref="SpiMaster_TransferSequentialAsync"/>.</para>
/// </summary>
/// <param name="handle">Selects the SPI handle to perform the transfer on.</param>
/// <param name="transfer">A pointer to the base of an array of transfers, for more information
/// look at <see cref="SpiTransfer"/>.</param>
/// <param name="count">Selects the number of the sequential SPI transactions.</param>
/// <returns>Returns ERROR_NONE on success, or an error code on failure.</returns>
int32_t  SpiMaster_TransferSequentialSync(
    SpiMaster *handle, SpiTransfer *transfer, uint32_t count);

/// <summary>
/// <para>Executes a single write operation on the SPI interface provided.</para>
/// <para>This is a synchronous wrapper around <see cref="SpiMaster_WriteAsync"/>.</para>
/// </summary>
/// <param name="handle">Selects the SPI handle to perform the transfer on.</param>
/// <param name="data">A pointer to the data to be written.</param>
/// <param name="length">The length of the data to be written.</param>
/// <returns>Returns ERROR_NONE if successful, else returns an error code.</returns>
static inline int32_t SpiMaster_WriteSync(
    SpiMaster *handle, const void *data, uintptr_t length);

/// <summary>
/// <para>Executes a single read operation on the SPI interface provided.</para>
/// <para>This is a synchronous wrapper around <see cref="SpiMaster_ReadAsync"/>.</para>
/// </summary>
/// <param name="handle">Selects the SPI handle to perform the transfer on.</param>
/// <param name="data">A pointer to the data to be read.</param>
/// <param name="length">The length of the data to be read.</param>
/// <returns>Returns ERROR_NONE on success, or an error code on failure.</returns>
static inline int32_t SpiMaster_ReadSync(
    SpiMaster *handle, void *data, uintptr_t length);

/// <summary>
/// <para>Executes back-to-back write then read operations on the SPI interface provided.</para>
/// <para>This is a synchronous wrapper around <see cref="SpiMaster_WriteThenReadAsync"/>.</para>
/// </summary>
/// <param name="handle">Selects the SPI handle to perform the transfer on.</param>
/// <param name="writeData">A pointer to the data to be written.</param>
/// <param name="writeLength">The length of the data to be written.</param>
/// <param name="readData">A pointer to the data to be read.</param>
/// <param name="readLength">The length of the data to be read.</param>
/// <returns>Returns ERROR_NONE on success, or an error code on failure.</returns>
static inline int32_t SpiMaster_WriteThenReadSync(
    SpiMaster *handle, const void *writeData,
    uintptr_t writeLength, void *readData, uintptr_t readLength);

static inline int32_t SpiMaster_WriteAsync(
    SpiMaster *handle, const void *data, uintptr_t length,
    void (*callback)(int32_t status, uintptr_t dataCount))
{
    SpiTransfer transfer = {
        .writeData = data,
        .readData  = NULL,
        .length    = length,
    };
    return SpiMaster_TransferSequentialAsync(handle, &transfer, 1, callback);
}

static inline int32_t SpiMaster_ReadAsync(
    SpiMaster *handle, void *data, uintptr_t length,
    void (*callback)(int32_t status, uintptr_t dataCount))
{
    SpiTransfer transfer = {
        .writeData = NULL,
        .readData  = data,
        .length    = length,
    };
    return SpiMaster_TransferSequentialAsync(handle, &transfer, 1, callback);
}

static inline int32_t  SpiMaster_WriteThenReadAsync(
    SpiMaster *handle, const void *writeData, uintptr_t writeLength,
    void *readData, uintptr_t readLength, void (*callback)(int32_t status, uintptr_t dataCount))
{
    SpiTransfer transfer[2] = {
        {.writeData = writeData, .readData = NULL    , .length = writeLength },
        {.writeData = NULL     , .readData = readData, .length = readLength  },
    };
    return SpiMaster_TransferSequentialAsync(handle, transfer, 2, callback);
}

static inline int32_t SpiMaster_WriteSync(SpiMaster *handle, const void *data, uintptr_t length)
{
    SpiTransfer transfer = {
        .writeData = data,
        .readData  = NULL,
        .length    = length,
    };
    return SpiMaster_TransferSequentialSync(handle, &transfer, 1);
}

static inline int32_t SpiMaster_ReadSync(SpiMaster *handle, void *data, uintptr_t length)
{
    SpiTransfer transfer = {
        .writeData = NULL,
        .readData  = data,
        .length    = length,
    };
    return SpiMaster_TransferSequentialSync(handle, &transfer, 1);
}

static inline int32_t  SpiMaster_WriteThenReadSync(
    SpiMaster *handle, const void *writeData, uintptr_t writeLength, void *readData, uintptr_t readLength)
{
    SpiTransfer transfer[2] = {
        { .writeData = writeData, .readData = NULL ,    .length = writeLength },
        { .writeData = NULL ,     .readData = readData, .length = readLength },
    };
    return SpiMaster_TransferSequentialSync(handle, transfer, 2);
}

#endif // #ifndef AZURE_SPHERE_SPIMASTER_H_
