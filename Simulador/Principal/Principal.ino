#include <Wire.h>
#include <Adafruit_MCP4725.h>
#define DEBUG_TIME
//#define DEBUG_VALUES
//#define DEBUG_CALCULATIONS


// ONE PHASE PARAMS
const float LINE_FREQ = 10;
const float LINE_FREQ_ANG_US_DEG = 2 * PI * LINE_FREQ / 1000000;

//Global variables
#ifdef DEBUG
unsigned long startTime, stopTime;
double t;
#endif //debug

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
#ifdef DEBUG_TIME
  startTime = micros();
#endif //DEBUG_TIME
  float sine = sin(LINE_FREQ_ANG_US_DEG * micros());
  int v = 2048 + 2000 * sine ;
  DAC2.setVoltage(v, false);
  int i = 2048 + (int)((2000.0 / 1024.0) * analogRead(A0)) * sin(LINE_FREQ_ANG_US_DEG * (micros() - 50 * (analogRead(A1) - 512)));
  DAC1.setVoltage(i, false);

#ifdef DEBUG_TIME
  stopTime = micros();
  Serial.print("Cycle (ms): ");
  Serial.println((stopTime - startTime) / 1000.0);
#endif //DEBUG_TIME

#ifdef DEBUG_VALUES
  Serial.print(i);
  Serial.print(',');
  Serial.println(v);
#endif //DEBUG_VALUES

#ifdef DEBUG_CALCULATIONS
//ToDo: Vrms,Irms,CosPhi calculations
  Serial.print(i);
  Serial.print(',');
  Serial.println(v);
#endif //DEBUG_CALCULATIONS
}


uint8_t isinTable8[] = {
  0, 4, 9, 13, 18, 22, 27, 31, 35, 40, 44,
  49, 53, 57, 62, 66, 70, 75, 79, 83, 87,
  91, 96, 100, 104, 108, 112, 116, 120, 124, 128,

  131, 135, 139, 143, 146, 150, 153, 157, 160, 164,
  167, 171, 174, 177, 180, 183, 186, 190, 192, 195,
  198, 201, 204, 206, 209, 211, 214, 216, 219, 221,

  223, 225, 227, 229, 231, 233, 235, 236, 238, 240,
  241, 243, 244, 245, 246, 247, 248, 249, 250, 251,
  252, 253, 253, 254, 254, 254, 255, 255, 255, 255,
};

int isin(int x)
{
  boolean pos = true;  // positive - keeps an eye on the sign.
  uint8_t idx;
  // remove next 6 lines for fastestl!
  if (x < 0)
  {
    x = -x;
    pos = !pos;
  }
  if (x >= 360) x %= 360;
  if (x > 180)
  {
    idx = x - 180;
    pos = !pos;
  }
  else idx = x;
  if (idx > 90) idx = 180 - idx;
  if (pos) return isinTable8[idx] / 2 ;
  return -(isinTable8[idx] / 2);
}
