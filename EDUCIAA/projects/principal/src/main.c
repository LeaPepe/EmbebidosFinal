
#include "main.h"


// Global variables
static const tick_t tickTimeMS = 1;
static rtc_t rtc;
static communicationManager_t communicationManager;
static uint16_t tickCounter;
// flags
static bool_t bEnableSendParams;
static bool_t bRdyToSendParams;
static bool_t bUpdateRtc;


// ---- Main function (entry point) ---- //
int main(void)
{
	// Board initialization board.h
	Board_Init();

	// Communication initialization
	comunicationManagerConfig(&communicationManager);

	// ADC enable
	adcConfig( ADC_ENABLE );

	// Real time clock
	// ToDo: Store and fetch data in non-volatile memmory
	rtc.year = 2021;
	rtc.month = 2;
	rtc.mday = 3;
	rtc.wday = 4;
	rtc.hour = 13;
	rtc.min = 36;
	rtc.sec= 0;

	rtcInit();

	rtcWrite( &rtc ); // Establecer fecha y hora

	// cycle counter to get times
	cyclesCounterConfig(EDU_CIAA_NXP_CLOCK_SPEED);
	cyclesCounterReset();

	// Tick Interruption configuration 1ms
	tickInit(1);
	tickCallbackSet(onTickUpdate,NULL);

	bEnableSendParams = FALSE;
	bRdyToSendParams = FALSE;
	// initialize times and needed values
	// V, I, T.

	// Main loop
	while (1) {

		// Resets cycle counter
		cyclesCounterReset();

		// Update Rtc if needed
		if(bUpdateRtc == TRUE) rtcWrite(&rtc);

		// checks if messages pending and parse them
		handleMessages();

		// Send Line params to webserver
		if(bRdyToSendParams == TRUE && bEnableSendParams == TRUE)
		{
			sendLineParameters();
			bRdyToSendParams = FALSE;
		}

		// Homemade Watchdog
		float timeElapsedLoop = cyclesCounterToUs(cyclesCounterRead());
		if(timeElapsedLoop >= tickTimeMS*1000)
		{
			LOG_WARNING("Loop time elapsed is grater than tick time interval");
		}
	}
   
   return 0;
}

void onTickUpdate(void* UNUSED)
{
	float timeStartEvent = cyclesCounterToUs(cyclesCounterRead()); // Homemade Watchdog


	// read values

	// store values (circular buffer?)

	// calculate on the fly params


	// if 1 seg, raise bRdyToSendParams;











	if(tickRead() == 500)
	{
		// calculate rest of params

		bRdyToSendParams == TRUE;
		tickWrite(0);
	}

	// Homemade TICK Watchdog
	float timeFinishEvent = cyclesCounterToUs(cyclesCounterRead());
	float timeElapsedEvent = timeFinishEvent - timeStartEvent;
	if(timeElapsedEvent >= tickTimeMS*1000)
	{
		LOG_WARNING("Tick time elapsed is grater than tick time interval");
	}
}

