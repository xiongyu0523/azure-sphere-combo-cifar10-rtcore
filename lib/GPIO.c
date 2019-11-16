/* Copyright (c) Microsoft Corporation. All rights reserved.
   Licensed under the MIT License. */

#include <stdbool.h>
#include <stdint.h>

#include "GPIO.h"
#include "mt3620/gpio.h"
#include "mt3620/adc.h"

static uint32_t pinToIndex(int pin)
{
    if (pin < GPIO_ADC_BLOCK_START && pin >= 0) {
        return (pin / GPIO_BLOCK_PIN_COUNT);
    }
    else if (pin >= GPIO_ADC_BLOCK_START && pin <= GPIO_ADC_BLOCK_START + GPIO_ADC_BLOCK_PIN_COUNT) {
        return GPIO_ADC_BLOCK_INDEX;
    } else {
        return GPIO_MAX_INDEX + 1;
    }
}

static uint32_t pinToPinMask(int pin, int index)
{
    if (pin < GPIO_ADC_BLOCK_START) {
        return (1U << (pin - (index * GPIO_BLOCK_PIN_COUNT) ) );
    } else {
        return (1U << (pin - (GPIO_ADC_BLOCK_START) ) );
    }
}

static int32_t ConfigurePin(uint32_t pin, bool asInput)
{
    uint32_t index = pinToIndex(pin);
	if (index > GPIO_MAX_INDEX) {
		return ERROR_GPIO_NOT_A_PIN;
	}

    uint32_t pinMask = pinToPinMask(pin, index);

	if (index == GPIO_ADC_BLOCK_INDEX) {
		if (asInput == true) {
			mt3620_adc->gpio_adc_ies_set = pinMask;
		} else {
			mt3620_adc->gpio_adc_oe_set = pinMask;
		}

	} else {
		if (asInput == true) {
			mt3620_gpio[index]->gpio_pwm_grp_ies_set = pinMask;
		} else {
			mt3620_gpio[index]->gpio_pwm_grp_oe_set = pinMask;
		}
	}

    return 0;
}

int32_t GPIO_ConfigurePinForOutput(uint32_t pin)
{
    return ConfigurePin(pin, false);
}

int32_t GPIO_ConfigurePinForInput(uint32_t pin)
{
    return ConfigurePin(pin, true);
}

int32_t GPIO_Write(uint32_t pin, bool state)
{
    uint32_t index = pinToIndex(pin);
	if (index > GPIO_MAX_INDEX) {
		return ERROR_GPIO_NOT_A_PIN;
	}

    uint32_t pinMask = pinToPinMask(pin, index);

	if (index == GPIO_ADC_BLOCK_INDEX) {
		if (state == true) {
            mt3620_adc->gpio_adc_dout_set = pinMask;
		} else {
			mt3620_adc->gpio_adc_dout_reset = pinMask;
		}
	} else {
		if (state == true) {
			mt3620_gpio[index]->gpio_pwm_grp_dout_set = pinMask;
		} else {
			mt3620_gpio[index]->gpio_pwm_grp_dout_reset = pinMask;
		}
}

    return 0;
}

int32_t GPIO_Read(uint32_t pin, bool *state)
{
    uint32_t index = pinToIndex(pin);
	if (index > GPIO_MAX_INDEX) {
		return ERROR_GPIO_NOT_A_PIN;
	}

    uint32_t pinMask = pinToPinMask(pin, index);

	if (index == GPIO_ADC_BLOCK_INDEX) {
		*state = pinMask & mt3620_adc->gpio_adc_din;
	} else {
		*state = pinMask & mt3620_gpio[index]->gpio_pwm_grp_din;
	}

    return 0;
}

int32_t GPIO_AddBlock(const uint32_t index)
{
    //TODO: Add the locking structure behaviour here

    return 0;
}
