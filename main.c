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

static const uint32_t Cifar10ImgSize = CFIAR10_WIDTH * CFIAR10_HEIGHT * CFIAR10_DEPTH;
static uint8_t Cifar10ImgBuf[CFIAR10_WIDTH * CFIAR10_HEIGHT * CFIAR10_DEPTH];

int get_top_prediction(q7_t* predictions)
{
	int max_ind = 0;
	int max_val = -128;
	for (int i = 0; i < 10; i++) {
		if (max_val < predictions[i]) {
			max_val = predictions[i];
			max_ind = i;
		}
	}
	return max_ind;
}
const char* cifar10_label[] = { "Plane", "Car", "Bird", "Cat", "Deer", "Dog", "Frog", "Horse", "Ship", "Truck" };
_Noreturn void RTCoreMain(void)
{
	VectorTableInit();
	CPUFreq_Set(197600000);
	
	DebugUARTInit();

	Log_Debug("Exmaple to run NN for Cifar-10\r\n");

#if 1

	BufferHeader *outbound, *inbound;
	uint32_t sharedBufSize = 0;
	if (GetIntercoreBuffers(&outbound, &inbound, &sharedBufSize) == -1) {
		Log_Debug("ERROR: GetIntercoreBuffers failed\r\n");
		while (1);
	}

	Log_Debug("outbound = 0x%X, inbound = 0x%X, sharedBufSize = %d\r\n", outbound, inbound, sharedBufSize);

	const size_t payloadStart = 20;
	const uint32_t maxInterCoreBufSize = 1024;

	uint32_t piece = 0;
	uint32_t recvSize = maxInterCoreBufSize + payloadStart;
	uint8_t  recvBuf[maxInterCoreBufSize + payloadStart];

	while (1) {

		if (DequeueData(outbound, inbound, sharedBufSize, &recvBuf[0], &recvSize) == -1) {
			continue;
		}

		if (piece < 3) {
			memcpy(&Cifar10ImgBuf[piece * maxInterCoreBufSize], &recvBuf[payloadStart], maxInterCoreBufSize);
			piece++;
		}

		q7_t output_data[10];

		if (piece == 3) {
			piece = 0;

			run_nn(&Cifar10ImgBuf[0], output_data);
			int top_ind = get_top_prediction(output_data);
			Log_Debug("%s\r\n", cifar10_label[top_ind]);
			//for (int i = 0; i < 10; i++)
			//{
			//	Log_Debug("%d: %d\r\n", i, output_data[i]);
			//}
			//Log_Debug("start\r\n");
			//for (uint32_t pos = 0; pos < 3072; pos++) {
			//	Log_Debug("%d, ", Cifar10ImgBuf[pos]);
			//}
			//Log_Debug("\r\n");
		}
	}

#endif

}
