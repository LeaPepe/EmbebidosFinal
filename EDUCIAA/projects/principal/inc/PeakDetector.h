#ifndef _PHASOR_PEAKDETECTOR_H_
#define _PHASOR_PEAKDETECTOR_H_

#include <sapi.h>

#include "utils.h"
#include "digitalization.h"

// data structure
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


//public
void PeakDetector_Init(peakData_t* pd);
void PeakDetector_InputData(peakData_t* pd,  sample_t const * const sample);
void PeakDetector_Reset(peakData_t* pd);
float PeakDetector_getAvgPhi(peakData_t* pd);

//private
float getPhiFromTime(float t);

#endif
