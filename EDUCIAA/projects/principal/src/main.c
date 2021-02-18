#include "main.h"

// Global variables
static const tick_t tickTimeMS = 1; // Sample tick Time in milliseconds
// flags
static bool_t bRdyToSendParams;



// ---- Main function (entry point) ---- //
int main(void)
{
	// Board initialization board.h
	Board_Init();

	// Communication initialization
	comunicationManagerConfig();

	// ADC enable
	adcConfig( ADC_ENABLE );

	// Time initialization
	cyclesCounterConfig(EDU_CIAA_NXP_CLOCK_SPEED);
	cyclesCounterReset();


	// Tick 1ms initialization
	tickInit(1);
	tickCallbackSet(onTickUpdate,NULL);

	// Initialize some parameters
	uartWriteString(UART_USB,"Begining! ... \r\n");
	bEnableSendParams = TRUE;
	bRdyToSendParams = FALSE;


	// initialize times and needed values
	setCurrentParams(0.5,0.5,0.5);
	sampleCount = 0;
	// Main loop
	while (1) {
		// handle pending command messages (if any)
		handleMessages();

		// Send line data to webserver (every 1 second)
		if(bRdyToSendParams == TRUE && bEnableSendParams == TRUE)
		{
			sendLineParameters();
			bRdyToSendParams = FALSE;
		}
	}
   
   return 0;
}


// -- TICK CALLBACK -- //
void onTickUpdate(void* UNUSED)
{
	// Tick watchdog time start
	float timeStartTick = cyclesCounterToUs(cyclesCounterRead()); // Homemade Watchdog


	// read values
	sample_t sample;
	sample.v = getVoltage(adcRead(CH1));
	sample.i = getCurrent(adcRead(CH2));

	// calculate on the fly params
	currentParams.Vrms += sample.v * sample.v;
	currentParams.Irms += sample.i * sample.i;

	// each second tasks
	if(tickRead() % 1000 == 0)
	{
		// calculate rest of params
		currentParams.Vrms /= 1000;
		currentParams.Irms /= 1000;

		computedParams.Vrms = sqrt(currentParams.Vrms);
		computedParams.Irms = sqrt(currentParams.Irms);
		computedParams.CosPhi = 3;

		// raise flag
		bRdyToSendParams = TRUE;

		// restart values
		currentParams.Vrms = 0.2;
		currentParams.Irms = 0.3;
	}

	// Send samples each tick (if requested)
	if (bEnableSendSamples){
		if(sampleCount >= N_SAMPLES_TO_SEND){
			sampleCount = 0;
			bEnableSendSamples = FALSE;
		} else {
			sendSample(sample,sampleCount);
			sampleCount++;
		}
	}

	// Tick watchdog check
	float timeElapsedTick = cyclesCounterToUs(cyclesCounterRead()) - timeStartTick;
	if(timeElapsedTick >= tickTimeMS*1000)
	{
		LOG_WARNING("Tick time elapsed is grater than tick time interval");
	}
}

