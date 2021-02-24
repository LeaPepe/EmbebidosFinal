#include "Time.h"


// initialize cycle counter
void Time_Init()
{
	cyclesCounterConfig(EDU_CIAA_NXP_CLOCK_SPEED);
	cyclesCounterReset();
}

// gets current time in us
float Time_get()
{
	return cyclesCounterToUs(cyclesCounterRead());
}

// computes time ellapsed between two times
float Time_getEllapsed(float start,float end)
{
	return end - start;
}
