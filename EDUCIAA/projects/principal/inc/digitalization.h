#ifndef _PHASOR_DIGITALIZATION_H_
#define _PHASOR_DIGITALIZATION_H_

#include <stdint.h>
#include <sapi.h>

// --- DEFINITIONS --- //

#define ANALOG_MAX_VOLTAGE 315.0f
#define ANALOG_MAX_CURRENT 15.0f

#define ANALOG_PRESITION 10
#define ANALOG_MAX_VALUE 1024

// -- DATA STRUCTURES --- //

typedef struct lineParams{
	float Vrms;
	float Irms;
	float CosPhi;
}params_t;

// --- FUNCTIONS DEFINITIONS --- //

// get true values from analog values
float getVoltage(uint16_t analogValue);
float getCurrent(uint16_t analogValue);
void setCurrentParams(const float v, const float i, const float cosPhi);


#endif /* PHASOR_DIGITALIZATION_H_ */
