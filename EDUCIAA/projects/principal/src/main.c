
#include "main.h"

// Global variables
static const tick_t tickTimeMS = 1; // Sample tick Time in milliseconds
static uint16_t samplesSentCount;
//static rtc_t rtc; // Real Time Clock
//static communicationManager_t comMngr; // Comm Manager
//static uint16_t tickCounter;

// flags
static bool_t bRdyToSendParams;
//static bool_t bUpdateRtc;


// ---- Main function (entry point) ---- //
int main(void)
{
	// Board initialization board.h
	Board_Init();

	// Communication initialization
	comunicationManagerConfig();

	// ADC enable
	adcConfig( ADC_ENABLE );

	// Real time clock
	// ToDo: Store and fetch data in non-volatile memmory
	//rtc.year = 2021;
	//rtc.month = 2;
	//rtc.mday = 3;
	//rtc.wday = 4;
	//rtc.hour = 13;
	//rtc.min = 36;
	//rtc.sec= 0;

	//rtcInit();

	//rtcWrite( &rtc ); // Establecer fecha y hora

	// cycle counter to get times
	cyclesCounterConfig(EDU_CIAA_NXP_CLOCK_SPEED);
	cyclesCounterReset();

	// Tick Interruption configuration 1ms
	tickInit(1);
	tickCallbackSet(onTickUpdate,NULL);

	uartWriteString(UART_USB,"Begining! ... \r\n");
	bEnableSendParams = TRUE;
	bRdyToSendParams = FALSE;
	// initialize times and needed values
	// V, I, T.
	setCurrentParams(220.0,5.0,5.0);

	// Main loop
	while (1) {
		// Update Rtc if needed
		//if(bUpdateRtc == TRUE) rtcWrite(&rtc);

		// checks if messages pending and parse them
		handleMessages();

		// Send Line params to webserver
		if(bRdyToSendParams == TRUE && bEnableSendParams == TRUE)
		{
			sendLineParameters();
			bRdyToSendParams = FALSE;
		}
	}
   
   return 0;
}



void onTickUpdate(void* UNUSED)
{
	float timeStartTick = cyclesCounterToUs(cyclesCounterRead()); // Homemade Watchdog


	// read values
	uint16_t v = getVoltage(adcRead(CH1));
	uint16_t i = getCurrent(adcRead(CH2));

	// store values (circular buffer?)

	// calculate on the fly params


	// if 1 seg, raise bRdyToSendParams;


	if(tickRead() == 1000)
	{
		// calculate rest of params

		bRdyToSendParams = TRUE;
		tickWrite(0);
	}

	// Send raw samples
	if (bEnableSendSamples){
		if(samplesSentCount >= N_SAMPLES_TO_SEND){
			samplesSentCount = 0;
			bEnableSendSamples = FALSE;
		} else {
			samplesSentCount++;
			sendSample(samplesSentCount,N_SAMPLES_TO_SEND,v,i);
		}
	}

	// Homemade TICK Watchdog
	float timeElapsedTick = cyclesCounterToUs(cyclesCounterRead()) - timeStartTick;
	if(timeElapsedTick >= tickTimeMS*1000)
	{
		LOG_WARNING("Tick time elapsed is grater than tick time interval");
	}
}

