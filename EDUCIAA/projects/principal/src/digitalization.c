#include "digitalization.h"

// in c++ could be a static member in sample_t
extern uint16_t sampleCount; // used when sending samples to server


// Enables ADC
void ADC_Init()
{
	adcConfig( ADC_ENABLE );
}

// Reads analog logic inputs and traduce to real value
void Sample_Read(sample_t* s)
{
	uint16_t logicV = adcRead(CH1);
	uint16_t logicI = adcRead(CH2);
	s->v = getVoltage(logicV);
	s->i = getCurrent(logicI);
}


// clear parameters
void LineParams_Clear(lineParams_t* lp)
{
	lp->Vrms = 0;
	lp->Irms = 0;
	lp->Phi = 0;
}


// Initialize params data structure
void Params_Init(params_t* p)
{
	p->bEnableSend = TRUE;
	p->bRdyToSend = FALSE;
	PeakDetector_Init(&p->peakData);
	LineParams_Clear(&p->current);
}
// computes the inside integral sum and peak detection
void Params_ComputeSample(params_t* p,sample_t* s)
{
	// sum v^2 and i^2
	p->current.Vrms += s->v * s->v;
	p->current.Irms += s->i * s->i;
	// peak detector input sample
	PeakDetector_InputData(&p->peakData, s);
}

// finalize the computation
void Params_ComputeParams(params_t* p)
{
	// calculate rest of params
	p->current.Vrms /= 1000;
	p->current.Irms /= 1000;

	//finishing computing parameters
	p->computed.Vrms = sqrt(p->current.Vrms);
	p->computed.Irms = sqrt(p->current.Irms);
	p->computed.Phi = PeakDetector_getAvgPhi(&p->peakData);

	// raise flag to send
	p->bRdyToSend = TRUE;
}

// Resets parameters
void Params_Reset(params_t* p)
{
	PeakDetector_Reset(&p->peakData);
	LineParams_Clear(&p->current);
}


// Logic to real V
float getVoltage(const uint16_t logicValue)
{
	float offsetValue = logicValue - (ANALOG_MAX_VALUE/2);
	return ANALOG_MAX_VOLTAGE*(offsetValue)/(float)(ANALOG_MAX_VALUE/2);
}

// Logic to real I
float getCurrent(const uint16_t logicValue)
{
	float offsetValue = logicValue - (ANALOG_MAX_VALUE/2);
	return ANALOG_MAX_CURRENT*(offsetValue)/(float)(ANALOG_MAX_VALUE/2) ;
}
