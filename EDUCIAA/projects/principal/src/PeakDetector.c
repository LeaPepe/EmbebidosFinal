#include "PeakDetector.h"

//global data
peakData_t peakData;

void PeakDetector_Init(peakData_t* pd)
{
	// its a max detector, so big negative number to init. ToDo: Init with negative infinite value.
	pd->peakSamples[2].v = -100000.0f;
	pd->peakSamples[2].i = -100000.0f;
	pd->peakSamples[1].v = -100000.0f;
	pd->peakSamples[1].i = -100000.0f;
	pd->peakSamples[0].v = -100000.0f;
	pd->peakSamples[0].i = -100000.0f;

	PeakDetector_Reset(pd);
}

void PeakDetector_InputData(peakData_t* pd,  sample_t const * const sample)
{
	// cycle data
	pd->peakSamples[2].v = pd->peakSamples[1].v;
	pd->peakSamples[2].i = pd->peakSamples[1].i;
	pd->peakSamples[1].v = pd->peakSamples[0].v;
	pd->peakSamples[1].i = pd->peakSamples[0].i;

	// input new data
	pd->peakSamples[0].v = sample->v;
	pd->peakSamples[0].i = sample->i;

	// Detect peaks
	// V
	if(pd->peakSamples[1].v >= pd->peakSamples[0].v &&
			pd->peakSamples[1].v >= pd->peakSamples[2].v)
	{
		// ToDo: You can compute frequency here too as time between peaks!
		pd->peakTimeV = cyclesCounterToUs(cyclesCounterRead()) - 1000.0f;
		pd->bPeakDetectedV = TRUE;
	}
	// I
	if(pd->peakSamples[1].i >= pd->peakSamples[0].i &&
			pd->peakSamples[1].i >= pd->peakSamples[2].i)
	{
		// ToDo: You can compute frequency here too as time between peaks!
		pd->peakTimeI = cyclesCounterToUs(cyclesCounterRead()) - 1000.0f;
		pd->bPeakDetectedI = TRUE;
	}

	// if both detected, compute phi
	if (pd->bPeakDetectedV == TRUE && pd->bPeakDetectedV == TRUE)
	{
		float timeBetweenPeaksUS = pd->peakTimeV - pd->peakTimeI;
		// if no phi data, then avg = phi
		if (pd->phiCount == 0)
		{
			pd->phi = getPhiFromTime(timeBetweenPeaksUS);
			pd->avgPhi = pd->phi;
		// else sum to the avg amount
		} else {
			pd->phi = getPhiFromTime(timeBetweenPeaksUS);
			pd->avgPhi = ((pd->avgPhi * (float)pd->phiCount) + pd->phi)
					/ (float)(pd->phiCount +1);
		}
		pd->phiCount++;

		// reset the flags
		pd->bPeakDetectedI = FALSE;
		pd->bPeakDetectedV = FALSE;
	}
}


float getPhiFromTime(float t)
{
	float retVal = 1;
	// if time is negative, phi is negative
	if(t < 0)
		{
			t *= -1;
			retVal *= -1;
		}
	// if phi > 180, then phi is negative
	if(t > 10000)
	{
		t = t - 10000;
		retVal *= -1;
	}
	return (retVal * t*360.0)/20000.0;
}

void PeakDetector_Reset(peakData_t* pd)
{
	// reset the flags
	pd->bPeakDetectedI = FALSE;
	pd->bPeakDetectedV = FALSE;

	// reset the count
	pd->phiCount = 0;

	// no need to reset anything else
}

float PeakDetector_getAvgPhi(peakData_t* pd)
{
	return pd->avgPhi;
}
