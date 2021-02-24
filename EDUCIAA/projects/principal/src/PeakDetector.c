#include "PeakDetector.h"

// Initialize the peak data structure
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

// inputs sample to the structure and checks for peaks
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

	// Detect peaks.
	// V. If detected, wait until finished
	if(pd->peakSamples[1].v >= pd->peakSamples[0].v &&
			pd->peakSamples[1].v > pd->peakSamples[2].v && pd->bPeakDetectedV == FALSE)
	{
		// ToDo: You can compute frequency here too as time between peaks!
		pd->peakTimeV = Time_get();
		pd->bPeakDetectedV = TRUE;
	}

	// I. Only if V peak was detected
	if(pd->peakSamples[1].i > pd->peakSamples[0].i &&
			pd->peakSamples[1].i > pd->peakSamples[2].i && pd->bPeakDetectedV == TRUE)
	{
		// ToDo: You can compute frequency here too as time between peaks!
		pd->peakTimeI = Time_get();
		pd->bPeakDetectedI = TRUE;
	}

	// if both detected, compute phi
	if (pd->bPeakDetectedV == TRUE && pd->bPeakDetectedI == TRUE)
	{
		float timeBetweenPeaks = Time_getEllapsed(pd->peakTimeV,pd->peakTimeI);
		// ms to deg
		pd->phi = getPhiFromTime(timeBetweenPeaks);

		// add to sum
		pd->sumPhi += pd->phi;
		// add 1 to count
		pd->phiCount++;

		// reset the flags
		pd->bPeakDetectedV = FALSE;
		pd->bPeakDetectedI = FALSE;
	}
}



// translate time to deg, knowing that 20ms is a full cycle
float getPhiFromTime(float t)
{
	float retVal =1.0f;

	// limits -180 < phi < 180
	while(t >= 10000.0f)
	{
		t = t - 20000.0f;
	}

	// time to deg
	return (t*360.0f)/20000.0f;
}

// resets the flags and counts
void PeakDetector_Reset(peakData_t* pd)
{
	// reset the flags
	pd->bPeakDetectedI = FALSE;
	pd->bPeakDetectedV = FALSE;

	// reset the count
	pd->phiCount = 0;
	pd->sumPhi = 0;
	// no need to reset anything else
}

// computes the avg phi
float PeakDetector_getAvgPhi(peakData_t* pd)
{
	return pd->sumPhi / (float)pd->phiCount;
}
