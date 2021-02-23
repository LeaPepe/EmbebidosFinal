#include "main.h"

// --- GLOBAL VARIABLES --- //

static const tick_t timeTick = 1000; // Sample tick Time in us

params_t params;

communicationManager_t ComMngr;

volatile bool_t bEnableSendSamples;

uint16_t sampleCount; // used when sending samples to server



// --- MAIN FUNCTION (entry point) --- //
int main(void)
{
	// Board initialization board.h
	Board_Init();

	// Communication initialization
	ComMngr_Init(&ComMngr);

	// ADC initialization
	ADC_Init();

	// Time initialization
	Time_Init();

	// Tick 1ms initialization
	tickInit(timeTick/1000);
	tickCallbackSet(onTickUpdate,NULL);

	// Initialize flags and other variables
	bEnableSendSamples = FALSE;
	sampleCount = 0;

	// Peak detector data initialization
	Params_Init(&params);

	LOG_INFO("Begining! ... \r\n");

	// --- MAIN LOOP --- //
	while (1) {

		// handle pending command messages (if any)
		ComMngr_HandleMessages(&ComMngr);


		// Send line data to webserver (every 1 second)
		if(params.bEnableSend == TRUE && params.bRdyToSend == TRUE)
		{
			ComMngr_SendParams(&ComMngr,&params);
			params.bRdyToSend = FALSE;
		}
	}
   
   return 0;
}


// --- 1MS TICK CALLBACK --- //
void onTickUpdate(void* UNUSED)
{
	// Tick watchdog time start
	float timeStartTick = Time_get(); // Homemade Watchdog

	sample_t sample;

	// read values
	Sample_Read(&sample);

	// Send raw sample each tick (if requested)
	if (bEnableSendSamples == TRUE){
		if(sampleCount >= N_SAMPLES_TO_SEND){
			sampleCount = 0;
			bEnableSendSamples = FALSE;
		} else {
			ComMngr_SendSample(&ComMngr, &sample, sampleCount);
			sampleCount++;
		}
	}


	// inside the integral: sum of v^2, i^2
	// also peak detector input data
	Params_ComputeSample(&params,&sample);

	// each second
	if(tickRead() % 1000 == 0)
	{
		// outside integral: 1/T
		// also set RdyToSend flag
		Params_ComputeParams(&params);
		// resets the count, stores finished value to send
		Params_Reset(&params);
	}

	// Tick watchdog check
	if(Time_getEllapsed(timeStartTick, Time_get()) >= timeTick)
	{
		LOG_WARNING("Tick callback time is grater than tick time interval");
	}
}

