#include "digitalization.h"

params_t currentParams;

float getVoltage(uint16_t analogValue)
{
	return ANALOG_MAX_VOLTAGE*analogValue/ANALOG_MAX_VALUE;
}

float getCurrent(uint16_t analogValue)
{
	return ANALOG_MAX_CURRENT*analogValue/ANALOG_MAX_VALUE;
}


void setCurrentParams(const float v, const float i, const float cosPhi)
{
	currentParams.Vrms = v;
	currentParams.Irms = i;
	currentParams.CosPhi = cosPhi;
}
