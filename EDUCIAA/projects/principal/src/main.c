#include "main.h"

// Global variables
static const tick_t tickTimeMS = 1; // Sample tick Time in milliseconds

extern peakData_t peakData;
extern communicationManager_t ComMngr;

// flags
bool_t bRdyToSendParams;



// --- MAIN FUNCTION (entry point) --- //
int main(void)
{
	// Board initialization board.h
	Board_Init();

	// Communication initialization
	ComMngr_Init(&ComMngr);

	// ADC enable
	adcConfig( ADC_ENABLE );

	// Time initialization
	cyclesCounterConfig(EDU_CIAA_NXP_CLOCK_SPEED);
	cyclesCounterReset();


	// Tick 1ms initialization
	tickInit(1);
	tickCallbackSet(onTickUpdate,NULL);

	// Initialize some parameters
	uartWriteString(UART_DEBUG,"Begining! ... \r\n");
	bEnableSendParams = TRUE;
	bEnableSendSamples = FALSE;
	bRdyToSendParams = FALSE;

	PeakDetector_Init(&peakData);


	// initialize times and needed values
	clearCurrentParams();
	sampleCount = 0;




	// --- MAIN LOOP --- //
	while (1) {

		// handle pending command messages (if any)
		ComMngr_HandleMessages(&ComMngr);


		// Send line data to webserver (every 1 second)
		if(bRdyToSendParams == TRUE && bEnableSendParams == TRUE)
		{
			ComMngr_SendLineParams(&ComMngr);
			bRdyToSendParams = FALSE;
		}
	}
   
   return 0;
}


// --- 1MS TICK CALLBACK --- //
void onTickUpdate(void* UNUSED)
{
	// Tick watchdog time start
	float timeStartTick = cyclesCounterToUs(cyclesCounterRead()); // Homemade Watchdog


	// read values
	sample_t sample;
	uint16_t logicV = adcRead(CH1);
	uint16_t logicI = adcRead(CH2);
	sample.v = getVoltage(logicV);
	sample.i = getCurrent(logicI);

	// calculate on the fly params
	currentParams.Vrms += sample.v * sample.v;
	currentParams.Irms += sample.i * sample.i;

	PeakDetector_InputData(&peakData,  &sample);

	// each second tasks
	if(tickRead() % 1000 == 0)
	{
		// calculate rest of params
		currentParams.Vrms /= 1000;
		currentParams.Irms /= 1000;

		computedParams.Vrms = sqrt(currentParams.Vrms);
		computedParams.Irms = sqrt(currentParams.Irms);
		computedParams.Phi = PeakDetector_getAvgPhi(&peakData);

		// raise flag to send
		bRdyToSendParams = TRUE;

		// restart values
		PeakDetector_Reset(&peakData);
		currentParams.Vrms = 0.0;
		currentParams.Irms = 0.0;
		currentParams.Phi = 0.0;
	}

	// Send raw samples each tick (if requested)
	if (bEnableSendSamples == TRUE){
		if(sampleCount >= N_SAMPLES_TO_SEND){
			sampleCount = 0;
			bEnableSendSamples = FALSE;
		} else {
			ComMngr_SendSample(&ComMngr, sample, sampleCount);
			sampleCount++;
		}
	}

	// Tick watchdog check
	float timeElapsedTick = cyclesCounterToUs(cyclesCounterRead()) - timeStartTick;

	if(timeElapsedTick >= tickTimeMS*1000)
	{
		LOG_WARNING("Tick callback time is grater than tick time interval");
	}
}

