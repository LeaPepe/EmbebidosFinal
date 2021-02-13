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
