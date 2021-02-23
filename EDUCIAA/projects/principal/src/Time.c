#include "Time.h"

void Time_Init()
{
	cyclesCounterConfig(EDU_CIAA_NXP_CLOCK_SPEED);
	cyclesCounterReset();
}

float Time_get()
{
	return cyclesCounterToUs(cyclesCounterRead());
}

float Time_getEllapsed(float start,float end)
{
	return end - start;
}
