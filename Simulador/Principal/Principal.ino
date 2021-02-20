#include <Wire.h>
#include <Adafruit_MCP4725.h>

// Debug options
//#define DEBUG_TIME
//#define DEBUG_VALUES
#define DEBUG_CALCULATIONS


// ONE PHASE PARAMS
const float LINE_FREQ = 50;
const float LINE_FREQ_ANG_US_DEG = 2 * PI * LINE_FREQ / 1000000;

//Global variables
#ifdef DEBUG_TIME
unsigned long startTime, stopTime;
double t;

#endif //debug
int cyclesCount;
// DAC ADDR
Adafruit_MCP4725 DAC1;
Adafruit_MCP4725 DAC2;
// SETUP
void setup()
{
  // Serial
  Serial.begin(115200);
  Serial.println("Begin ...");
  // DAC
  // 0x60 LOW, 0x61 HIGH
  if (!DAC1.begin(0x60)) Serial.println("DAC 0x60 Not Found");
  if (!DAC2.begin(0x61)) Serial.println("DAC 0x61 Not Found");
}

// LOOP
void loop()
{
  // To display cycle time
#ifdef DEBUG_TIME
  startTime = micros();
#endif //DEBUG_TIME

  // Voltage
  float sineV = sin(LINE_FREQ_ANG_US_DEG * micros()); // Sine(2pift)
  int v = (int)(2048.0f + 2000.0f * sineV) ; // to logic value
  DAC2.setVoltage(v, false);

  // Current (analog reads)
  int phaseOffset =  10 * (analogRead(A1) - 512); // A1 for time offset in ms
  float sineA = sin(LINE_FREQ_ANG_US_DEG * (micros() - phaseOffset)); // Sine(2pif(t-offset))
  int i = 2048 + (int)((2000.0f / 1024.0f) * analogRead(A0) * sineA); // A0 for amplitude
  DAC1.setVoltage(i, false);

  // To display cycle time
#ifdef DEBUG_TIME
   stopTime = micros();
   Serial.print("Cycle (ms): ");
   Serial.println((stopTime - startTime) / 1000.0);
#endif //DEBUG_TIME

   // To display real time samples
#ifdef DEBUG_VALUES
   Serial.print(i);
   Serial.print(',');
   Serial.println(v);
#endif //DEBUG_VALUES

  // To display teoric values Vrms,Irms,Phi
#ifdef DEBUG_CALCULATIONS
  if (cyclesCount++ % 5000 == 0) {
    float vrms = ((((float)(4048 - 2048)) * 318.4f) / 2048.0f) * 0.707f;
    float irms = ((((float)((2048 +(2000.0f / 1024.0f) * analogRead(A0)) - 2048)) * 15.0f) / 2048.0f) * 0.707f;
    Serial.print("Ve = ");
    Serial.print(vrms);
    Serial.print(", Ie = ");
    Serial.print(irms);
    Serial.print(", Phi = ");
    float phi = (float)phaseOffset * 360.0f / 20000.0f;
    Serial.println(phi);
  }
#endif //DEBUG_CALCULATIONS
}
