#include "UART.h"
#include "Log_Debug.h"

static UART* DebugUartHandler;

void DebugUARTInit(void)
{
	DebugUartHandler = UART_Open(MT3620_UNIT_UART_DEBUG, 115200, UART_PARITY_NONE, 1, NULL);
}

void _putchar(char character)
{
	UART_Write(DebugUartHandler, &character, 1);
}
