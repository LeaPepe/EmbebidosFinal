#include "digitalization.h"

float getVoltage(uint16_t analogValue){
	return ANALOG_MAX_VOLTAGE*analogValue/ANALOG_MAX_VALUE;
}

float getCurrent(uint16_t analogValue){
	return ANALOG_MAX_CURRENT*analogValue/ANALOG_MAX_VALUE;
}


