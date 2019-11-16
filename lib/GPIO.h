/* Copyright (c) Microsoft Corporation. All rights reserved.
   Licensed under the MIT License. */

#ifndef MT3620_GPIO_H
#define MT3620_GPIO_H

#include <stdbool.h>
#include <stdint.h>

#include "Common.h"

#define ERROR_GPIO_NOT_A_PIN (ERROR_SPECIFIC - 1)

/// <summary>
/// <para>An application must call this function before it configures or uses any of
/// the pins in the block. This function only needs to be called once for each block.</para>
/// <para>**Errors**</para>
/// <para>-ENOENT if the block range contains an unsupported pin.</para>
/// <para>-EEXIST if any of the pins in the block range has already been claimed by
/// a previous call to <see cref="GPIO_AddBlock" />.</para>
/// </summary>
/// <param name="index">Index of the corresponding GPIO block to add.</param>
/// <returns>Zero on success, A negative error code on failure.</returns>
int32_t GPIO_AddBlock(const uint32_t index);

/// <summary>
/// <para>Configure a pin for output. Call <see cref="GPIO_Write" /> to set the
/// state.</para>
/// <para><see cref="GPIO_AddBlock" /> must be called before this function.</para>
/// </summary>
/// <param name="pin">A specific pin.</param>
/// <returns>Zero on success, a standard errno.h code otherwise.</returns>
int32_t GPIO_ConfigurePinForOutput(uint32_t pin);

/// <summary>
/// <para>Configure a pin for input. Call <see cref="GPIO_Write" /> to read the
/// state.</para>
/// <para>This function does not control the pull-up or pull-down resistors.
/// If the pin is connected to a possibly-floating input, the application may
/// want to additionally enable these via the register interface.</para>
/// <para><see cref="GPIO_AddBlock" /> must be called before this function.</para>
/// </summary>
/// <param name="pin">A specific pin.</param>
/// <returns>Zero on success, a standard errno.h code otherwise.</returns>
int32_t GPIO_ConfigurePinForInput(uint32_t pin);

/// <summary>
/// <para>Set the state of a pin which has been configured for output.</para>
/// <para><see cref="GPIO_ConfigurePinForOutput" /> must be called before this
/// function.</para>
/// </summary>
/// <param name="pin">A specific pin.</param>
/// <param name="state">true to drive the pin high; false to drive it low.</param>
/// <returns>Zero on success, a standard errno.h code otherwise.</returns>
int32_t GPIO_Write(uint32_t pin, bool state);

/// <summary>
/// <para>Read the state of a pin which has been configured for input.</para>
/// <para><see cref="GPIO_ConfigurePinForInput" /> must be called before this
/// function.</para>
/// </summary>
/// <param name="pin">A specific pin.</param>
/// <param name="state">On return, contains true means the input is high, and false means
/// low.</param>
/// <returns>Zero on success, a standard errno.h code otherwise.</returns>
int32_t GPIO_Read(uint32_t pin, bool *state);

#endif // #ifndef MT3620_GPIO_H
