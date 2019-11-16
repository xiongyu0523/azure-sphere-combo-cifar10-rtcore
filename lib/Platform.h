/* Copyright (c) Microsoft Corporation. All rights reserved.
   Licensed under the MIT License. */

#ifndef PLATFORM_MT3620_H_
#define PLATFORM_MT3620_H_

typedef enum {
    MT3620_UNIT_UART_DEBUG,
    MT3620_UNIT_ISU0,
    MT3620_UNIT_ISU1,
    MT3620_UNIT_ISU2,
    MT3620_UNIT_ISU3,
    MT3620_UNIT_ISU4,
    MT3620_UNIT_ISU5,
    MT3620_UNIT_ADC0,
    MT3620_UNIT_GPT0,
    MT3620_UNIT_GPT1,
    MT3620_UNIT_GPT2,
    MT3620_UNIT_GPT3,
    MT3620_UNIT_GPT4,
} Platform_Unit;

#define MT3620_UNIT_ISU_COUNT 6
#define MT3620_UNIT_ADC_COUNT 1
#define MT3620_UNIT_GPT_COUNT 5

#endif // #ifndef PLATFORM_MT3620_H_
