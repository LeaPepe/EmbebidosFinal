#include "Wire.h" // I2C library
#include "MCP4725.h" // MCP library

// DAC ADDR
MCP4725 DAC(0x62);  // 0x60 LOW, 0x61 HIGH

volatile int x;
uint32_t start, stop;


// SETUP
void setup()
{
  Serial.begin(115200);
  Serial.println("Begin");
  DAC.begin(); // Dac init
  test(); // Performance
}



void test()
{
  // START
  Serial.println("\n\nPERFORMANCE");
  Serial.println();

  // SET VALUE i
  start = micros();
  for (int i = 0; i < 1000; i++)
  {
    DAC.setValue(i);
  }
  stop = micros();
  Serial.print("1000x DAC.setValue(i):\t\t");
  Serial.println(stop - start);


  // ReadDAC
  start = micros();
  for (int i = 0; i < 1000; i++)
  {
    x = DAC.readDAC();
  }
  stop = micros();
  Serial.print("1000x DAC.readDAC():\t\t");
  Serial.println(stop - start);

  // WriteDAC no EEPROM
  start = micros();
  for (int i = 0; i < 1000; i++)
  {
    x = DAC.writeDAC(i);
  }
  stop = micros();
  Serial.print("1000x DAC.writeDAC(i):\t\t");
  Serial.println(stop - start);

  // WriteDAC and EEPROM
  start = micros();
  for (int i = 0; i < 10; i++)
  {
    x = DAC.writeDAC(i, true);
  }
  stop = micros();
  Serial.print("10x   DAC.writeDAC(i, true):\t");
  Serial.println(stop - start);

  // Check Ready Status
  start = micros();
  for (int i = 0; i < 1000; i++)
  {
    x = DAC.ready();
  }
  stop = micros();
  Serial.print("1000x DAC.ready():\t\t");
  Serial.println(stop - start);

  // Eprom Write Latency
  while (!DAC.ready());
  DAC.writeDAC(0, true);
  start = micros();
  while (!DAC.ready());
  stop = micros();
  Serial.print("EEPROM write latency:\t\t");
  Serial.println(stop - start);
}

float expectedVoltage, adcVoltage;
int adcValue;

void loop()
{
  //Triangle wave

  for (uint16_t i = 0; i < 4096; i=i+16)
  {
    expectedVoltage = (5.0 / 4096.0) * i;
    DAC.writeDAC(i,true);
    delay(100);
    adcValue = analogRead(A0);
    adcVoltage = (adcValue * 5.0 ) / 1024.0;

    Serial.print("Value: ");
    Serial.print(DAC.readDAC());

    Serial.print("\tExpected Voltage: ");
    Serial.print(expectedVoltage, 3);

    Serial.print("\tADC Value: ");
    Serial.print(adcValue);

    Serial.print("\tADC Voltage: ");
    Serial.println(adcVoltage, 3);
  }
}
