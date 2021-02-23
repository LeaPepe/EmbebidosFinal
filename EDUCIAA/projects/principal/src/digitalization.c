#include "digitalization.h"

params_t currentParams,computedParams;

float getVoltage(uint16_t logicValue)
{
	float offsetValue = logicValue - (ANALOG_MAX_VALUE/2);
	return ANALOG_MAX_VOLTAGE*(offsetValue)/(float)(ANALOG_MAX_VALUE/2);
}

float getCurrent(uint16_t logicValue)
{
	float offsetValue = logicValue - (ANALOG_MAX_VALUE/2);
	return ANALOG_MAX_CURRENT*(offsetValue)/(float)(ANALOG_MAX_VALUE/2) ;
}


void setCurrentParams(const float v, const float i, const float Phi)
{
	currentParams.Vrms = v;
	currentParams.Irms = i;
	currentParams.Phi = Phi;
}

void clearCurrentParams()
{
	currentParams.Vrms = 0;
	currentParams.Irms = 0;
	currentParams.Phi = 0;
}
