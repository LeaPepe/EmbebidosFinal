#define SAMPLE_CMD_SIZE 11
#define PARAMS_CMD_SIZE 14

#define N_SAMPLES_TO_SEND 40
#define MAX_CMD_LENGTH 30
char cmdBuffer[MAX_CMD_LENGTH];
uint16_t cmdBufferSize;

typedef struct{
  // Rx and Tx buffers
  float v[N_SAMPLES_TO_SEND];
  float i[N_SAMPLES_TO_SEND];
  float t[N_SAMPLES_TO_SEND];
  uint16_t count;
}samples_t;

typedef struct{
  float Vrms;
  float Irms;
  float CosPhi;
}params_t;


extern samples_t samples;
extern params_t parameters;
// Serial Communication commands Identifications
typedef enum commandsID {
  CMD_HELLO = 'H',        // send hello to server
  CMD_ACK = 'A',          // Acknowledge
  CMD_RESET_KWH = 'E',        // Reset energy logging
  CMD_REQ_PARAMS_ON = 'N',      // Set line parameters sending every second to server
  CMD_REQ_PARAMS_OFF = 'F',     // Clear
  CMD_UPDATE_RTC = 'C',       // Update RTC with CMDID,YEAR,MONTH,MONTHDAY,WEEKDAY,HOUR,MINUTE,SECOND
  CMD_REQ_CYCLE_SAMPLES = 'R',    // Send cycle sample to server
  CMD_SENDING_PARAMS = 'P',     // ID for parameters data line   CMDID,VRMS,IRMS,COSHPHI,KWHCOUNT,KWHRESETTIME
  CMD_SENDING_CYCLE_SAMPLE = 'S',   // ID for cycle sample send  CMDID,INDEX,TOTALINDICES,V,I,DELTAT
  CHAR_RETURN_CARRY  = '\r',
  CHAR_TERMINATOR = '\n',
  CHAR_END_STRING = '\0'
} commandsID_t;

void requestSamples()
{
  Serial2.print(char(CMD_REQ_CYCLE_SAMPLES));
  Serial2.print("\r\n");
}


void serialAck()
{
  Serial2.print(char(CMD_ACK));
  Serial2.print("\r\n");
}

void serialHello()
{
  Serial2.print(char(CMD_HELLO));
  Serial2.print("\r\n");
}

void handleUart()
{
  while (Serial2.available()) {
    // get char
    char c = (char)Serial2.read();
    // add it to cmd buffer
    cmdBuffer[cmdBufferSize] = c;
    cmdBufferSize++;
    // if terminator, parse the command
    
    if (c == CHAR_RETURN_CARRY){
      c = (char)Serial2.read();
      if (c == CHAR_TERMINATOR){
        parseCmd();
        cmdBufferSize = 0; // clear string
      } else {
        cmdBuffer[cmdBufferSize] = c;
        cmdBufferSize++;
      }
    } 
  }
}

void parseCmd()
{
  if (cmdBufferSize <= 1) return;
  //ToDo:Validate data
  switch (cmdBuffer[0])
  {
    
    case CMD_HELLO:
      Serial.println("CIAA says hello!");
      // send acknowledge
      serialAck();
      break;
    case CMD_ACK:
      Serial.println("CIAA acknowledge!");
      break;
    case CMD_SENDING_CYCLE_SAMPLE:
      parseSampleCmd();
      break;
    case CMD_SENDING_PARAMS:
      parseParamsCmd();
      break;
    default:
      Serial.println("command ID Unknown!");
      Serial.println(cmdBuffer);
      break;
  }
}


void parseSampleCmd()
{
  if (cmdBufferSize < SAMPLE_CMD_SIZE)
  {
    Serial.println("Sample string too short");
    return;
  }
  uint16_t index = *((uint16_t*)(cmdBuffer + 1));
  Serial.print(index);
  samples.v[index] = *((float*)(cmdBuffer + 3));
  samples.i[index] = *((float*)(cmdBuffer + 7));
  docSamples["v"][index] = samples.v[index];
  docSamples["i"][index] = samples.i[index];
  docSamples["t"][index] = index;
  if(index == N_SAMPLES_TO_SEND - 2)
  {
    
    stringSamples = "";
    serializeJson(docSamples, stringSamples);
    Serial.println(stringSamples);
  }
}



void parseParamsCmd()
{
  if (cmdBufferSize < PARAMS_CMD_SIZE)
  {
    Serial.println("Parameter string too short");
    return;
  }
  
  parameters.Vrms = *((float*)(cmdBuffer + 1));
  parameters.Irms = *((float*)(cmdBuffer + 5));
  parameters.CosPhi = *((float*)(cmdBuffer + 9));
  docParams["vrms"] = parameters.Vrms;
  docParams["irms"] = parameters.Irms;
  docParams["cosphi"] = parameters.CosPhi;
  stringParams = "";
  serializeJson(docParams, stringParams);
}
