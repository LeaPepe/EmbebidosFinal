#ifndef _PHASOR_DIGITALIZATION_H_
#define _PHASOR_DIGITALIZATION_H_

#include <stdint.h>
#include <math.h>
#include <sapi.h>
#include "PeakDetector.h"

// --- DEFINITIONS --- //

#define ANALOG_MAX_VOLTAGE 318.4f
#define ANALOG_MAX_CURRENT 15.0f

#define ANALOG_PRESITION 10
#define ANALOG_MAX_VALUE 1024

// in c++ could be a static member in sample_t
extern uint16_t sampleCount; // used when sending samples to server

// -- DATA STRUCTURES --- //


#ifndef _PHASOR_SAMPLE_TYPE_
#define _PHASOR_SAMPLE_TYPE_

typedef struct{
	float v;
	float i;
}sample_t;

#endif

#ifndef _PHASOR_PEAKDATA_TYPE_
#define _PHASOR_PEAKDATA_TYPE_

typedef struct{
	// Data to compute peaks
	sample_t peakSamples[3];
	float peakTimeV,peakTimeI;

	// flags
	bool_t bPeakDetectedV,bPeakDetectedI;

	// the last phi, and avg phi
	float phi,avgPhi;
	uint16_t phiCount;
}peakData_t;

#endif

typedef struct{
	float Vrms;
	float Irms;
	float Phi;
}lineParams_t;


typedef struct{
	lineParams_t current,computed;
	peakData_t peakData;
	bool_t bEnableSend,bRdyToSend;
}params_t;


// --- FUNCTIONS DEFINITIONS --- //

//public
void ADC_Init();

void Sample_Read(sample_t* s);

void LineParams_Clear(lineParams_t* lp);

void Params_Init(params_t* p);
void Params_ComputeSample(params_t* p,sample_t* s);
void Params_ComputeParams(params_t* p);
void Params_Reset(params_t* p);

//private
// get true values from analog values
float getVoltage(uint16_t analogValue);
float getCurrent(uint16_t analogValue);


#endif /* PHASOR_DIGITALIZATION_H_ */
