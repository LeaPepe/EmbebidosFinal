#include "PeakDetector.h"


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

void PeakDetector_InputData(peakData_t* pd,  sample_t const * const s)
{
	// cycle data
	pd->peakSamples[2].v = pd->peakSamples[1].v;
	pd->peakSamples[2].i = pd->peakSamples[1].i;
	pd->peakSamples[1].v = pd->peakSamples[0].v;
	pd->peakSamples[1].i = pd->peakSamples[0].i;

	// input new data
	pd->peakSamples[0].v = s->v;
	pd->peakSamples[0].i = s->i;

	// Detect peaks
	// V
	if(pd->peakSamples[1].v >= pd->peakSamples[0].v &&
			pd->peakSamples[1].v > pd->peakSamples[2].v)
	{
		// ToDo: You can compute frequency here too as time between peaks!
		pd->peakTimeV = Time_get();
		pd->bPeakDetectedV = TRUE;
	}

	// I
	if(pd->peakSamples[1].i > pd->peakSamples[0].i &&
			pd->peakSamples[1].i > pd->peakSamples[2].i & pd->bPeakDetectedV == TRUE)
	{
		// ToDo: You can compute frequency here too as time between peaks!
		pd->peakTimeI = Time_get();
		pd->bPeakDetectedI = TRUE;
	}
	// if both detected, compute phi
	if (pd->bPeakDetectedV == TRUE && pd->bPeakDetectedI == TRUE)
	{
		float timeBetweenPeaks = Time_getEllapsed(pd->peakTimeV,pd->peakTimeI);

		pd->phi = getPhiFromTime(timeBetweenPeaks);
		// if no phi data, then avg = phi
		if (pd->phiCount == 0)
		{
			pd->avgPhi = pd->phi;
		// else sum to the avg amount
		} else {
			pd->avgPhi = ((pd->avgPhi * (float)pd->phiCount) + pd->phi)
					/ (float)(pd->phiCount +1);
		}
		pd->phiCount++;

		// reset the flags
		pd->bPeakDetectedV = FALSE;
		pd->bPeakDetectedI = FALSE;
	}
}


float getPhiFromTime(float t)
{
	float retVal =1.0f;
	if(t >= 10000.0f)
	{
		t = t - 10000.0f;
		retVal*=-1;
	}

	retVal *= (t*360.0f)/20000.0f;
	return retVal;
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
