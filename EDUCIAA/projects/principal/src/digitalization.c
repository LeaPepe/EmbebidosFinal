#include "digitalization.h"

params_t currentParams,computedParams;

float getVoltage(uint16_t logicValue)
{
	float offsetValue = logicValue - (ANALOG_MAX_VALUE/2);
	return ANALOG_MAX_VOLTAGE*(offsetValue)/(float)ANALOG_MAX_VALUE ;
}

float getCurrent(uint16_t logicValue)
{
	float offsetValue = logicValue - (ANALOG_MAX_VALUE/2);
	return ANALOG_MAX_VOLTAGE*(offsetValue)/(float)ANALOG_MAX_VALUE ;
}


void setCurrentParams(const float v, const float i, const float cosPhi)
{
	currentParams.Vrms = v;
	currentParams.Irms = i;
	currentParams.CosPhi = cosPhi;
}

void clearCurrentParams()
{
	currentParams.Vrms = 0;
	currentParams.Irms = 0;
	currentParams.CosPhi = 0;
}
