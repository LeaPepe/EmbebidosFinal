#ifndef _PHASOR_COMMUNICATION_H_
#define _PHASOR_COMMUNICATION_H_

#include <stdint.h>
#include <sapi.h>
#include <ring_buffer.h>
#include "utils.h"


#define UART_BUFFER_TX_SIZE 64
#define UART_BUFFER_RX_SIZE 64
#define CMD_BUFFER_SIZE 32

#define UART_COM UART_232
#define UART_DEBUG UART_USB

#define CHAR_CMD_TERMINATOR '\n'

// extern global variables
static rtc_t rtc;
static bool_t updateRtc;


// Serial Communication commands Identifications
typedef enum commandsID {
	CMD_HELLO = 0,				// send hello to server
	CMD_RESET_KWH, 				// Reset energy logging
	CMD_REQ_PARAMS_ON, 			// Set line parameters sending every second to server
	CMD_REQ_PARAMS_OFF,			// Clear
	CMD_UPDATE_RTC,				// Update RTC with CMDID,YEAR,MONTH,MONTHDAY,WEEKDAY,HOUR,MINUTE,SECOND
	CMD_REQ_CYCLE_SAMPLES,		// Send cycle sample to server
	CMD_SENDING_PARAMS,			// ID for parameters data line   CMDID,VRMS,IRMS,COSHPHI,KWHCOUNT,KWHRESETTIME
	CMD_SENDING_CYCLE_SAMPLE, 	// ID for cycle sample send  CMDID,INDEX,TOTALINDICES,V,I,DELTAT
	CMD_TERMINATOR = '\n'
} commandsID_t;

// All communications variables used
typedef struct{
	// Rx and Tx buffers
	RINGBUFF_T TxRBuffer,RxRBuffer;
	uint8_t TxBuffer[UART_BUFFER_TX_SIZE];
	uint8_t RxBuffer[UART_BUFFER_RX_SIZE];

	// Command buffer to parse
	uint8_t cmdBuffer[CMD_BUFFER_SIZE];
	uint16_t cmdSize;

	// Flags
	bool_t bPendingMsg;
}communicationManager_t;

static communicationManager_t communicationManager;

// Comunication functions
void comunicationManagerConfig(communicationManager_t* ComMngr);
void handleMessages();
void parseCommand(const uint8_t* cmd,const uint8_t size);
void sendData(const uint8_t* data,const uint16_t dataSize);
void sendByte(const uint8_t c);

// UART Callbacks
void onTxFreeCallback(void* unused);
void onRxCallback(void* unused);
void onRxBufferOverflow();






// FLAGS
static bool_t bEnableSendParams;
static bool_t bUpdateRtc;


// FUNCTIONS

void sendLineParameters();
void clearEnergy();

#endif /* _PHASOR_COMMUNICATION_H_ */
