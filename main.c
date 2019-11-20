#include <stddef.h>
#include <stdbool.h>
#include <stdint.h>

#include "Log_Debug.h"
#include "delay.h"
#include "VectorTable.h"
#include "CPUFreq.h"

#include "mt3620-intercore.h"
#include "nn.h"

#define CFIAR10_WIDTH	32
#define CFIAR10_HEIGHT  32
#define CFIAR10_DEPTH   3

static uint8_t Cifar10ImgBuf[CFIAR10_WIDTH * CFIAR10_HEIGHT * CFIAR10_DEPTH];
static const char* cifar10_label[] = {"Plane", "Car", "Bird", "Cat", "Deer", "Dog", "Frog", "Horse", "Ship", "Truck" };

static uint8_t _get_top_prediction(q7_t* predictions, uint8_t max)
{
	uint8_t index = 0;
	int8_t  max_val = -128;
	for (uint8_t i = 0; i < max; i++) {
		if (max_val < predictions[i]) {
			max_val = predictions[i];
			index = i;
		}
	}
	return index;
}

_Noreturn void RTCoreMain(void)
{
	VectorTableInit();
	CPUFreq_Set(197600000);
	
	DebugUARTInit();
	Log_Debug("Exmaple to run NN inference on RTcore for cifar-10 dataset\r\n");

	BufferHeader *outbound, *inbound;
	uint32_t sharedBufSize = 0;

	if (GetIntercoreBuffers(&outbound, &inbound, &sharedBufSize) == -1) {
		Log_Debug("ERROR: GetIntercoreBuffers failed\r\n");
		while (1);
	}

	const size_t payloadStart = 20;
	const uint32_t maxInterCoreBufSize = 1024;
	uint32_t piece = 0;
	uint32_t recvSize = maxInterCoreBufSize + payloadStart;
	uint8_t recvBuf[maxInterCoreBufSize + payloadStart];
	q7_t output_data[10];
	uint8_t top_index;

	while (1) {

		// waiting for incoming data
		if (DequeueData(outbound, inbound, sharedBufSize, &recvBuf[0], &recvSize) == -1) {
			continue;
		}

		// 3072 = 1024 x 3, as the maximum allowed user payload is 1024, we need split into 3 buffer. (HL and RT core must be synced)
		if (piece < 3) {
			memcpy(&Cifar10ImgBuf[piece * maxInterCoreBufSize], &recvBuf[payloadStart], maxInterCoreBufSize);
			piece++;
		}

		if (piece == 3) {
			piece = 0;

			// input = 32x32x3 RGB data, output = Possibility of each class
			run_nn(&Cifar10ImgBuf[0], output_data);
			top_index = _get_top_prediction(output_data, 10);

			// Send the result back to HL core
			recvBuf[payloadStart] = top_index;
			EnqueueData(inbound, outbound, sharedBufSize, &recvBuf[0], payloadStart + 1);
		}
	}
}
