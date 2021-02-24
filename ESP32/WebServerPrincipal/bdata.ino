#define N_SAMPLES_TO_SEND 40

samples_t samples;
params_t parameters;

// Samples initialization
void initSamples()
{
  for(uint8_t i = 0; i < N_SAMPLES_TO_SEND; i++)
  {
    samples.t[i] = (float)i / 1000.0f;
  }
}
