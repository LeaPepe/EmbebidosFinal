#ifndef _PHASOR_COMMUNICATION_H_
#define _PHASOR_COMMUNICATION_H_

#include <stdint.h>
#include <sapi.h>
#include <ring_buffer.h>
#include "digitalization.h"
#include "utils.h"

// --- DEFINITIONS --- //
#define UART_BUFFER_TX_SIZE 64
#define UART_BUFFER_RX_SIZE 64
#define CMD_BUFFER_SIZE 32

#define UART_COM UART_232
#define UART_DEBUG UART_USB

#define N_SAMPLES_TO_SEND 40

// --- GLOBAL FLAGS --- //
static bool_t bEnableSendParams;
static bool_t bEnableSendSamples;

// --- EXTERN GLOBAL VARIABLES --- //
extern params_t currentParams;

//static bool_t bUpdateRtc;



// -- DATA STRUCTURES --- //

// Serial Communication commands Identifications
typedef enum commandsID {
	CMD_HELLO = 'H',				// send hello to server
	CMD_ACK = 'A',					// Acknowledge
	CMD_RESET_KWH = 'E', 				// Reset energy logging
	CMD_REQ_PARAMS_ON = 'N', 			// Set line parameters sending every second to server
	CMD_REQ_PARAMS_OFF = 'F',			// Clear
	CMD_UPDATE_RTC = 'C',				// Update RTC with CMDID,YEAR,MONTH,MONTHDAY,WEEKDAY,HOUR,MINUTE,SECOND
	CMD_REQ_CYCLE_SAMPLES = 'R',		// Send cycle sample to server
	CMD_SENDING_PARAMS = 'P',			// ID for parameters data line   CMDID,VRMS,IRMS,COSHPHI,KWHCOUNT,KWHRESETTIME
	CMD_SENDING_CYCLE_SAMPLE = 'S', 	// ID for cycle sample send  CMDID,INDEX,TOTALINDICES,V,I,DELTAT
	CHAR_RETURN_CARRY  = '\r',
	CHAR_TERMINATOR = '\n',
	CHAR_END_STRING = '\0'
} commandsID_t;


// All communications variables used
typedef struct{
	// Rx and Tx buffers
	RINGBUFF_T TxRBuffer,RxRBuffer;
	uint8_t TxBuffer[UART_BUFFER_TX_SIZE];
	uint8_t RxBuffer[UART_BUFFER_RX_SIZE];

	// Command buffer to parse
	uint8_t cmdBuffer[CMD_BUFFER_SIZE];
	volatile uint16_t cmdSize;

	// Flags
	bool_t bPendingMsg;
}communicationManager_t;



// --- FUNCTIONS DEFINITIONS --- //

// UART Callbacks
void onTxFree(void* unused);
void onRx(void* unused);
void onRxOverflow();


// Comunication functions
void comunicationManagerConfig();
void handleMessages();
void parseCommand(const uint8_t* cmd,const uint8_t size);
void sendData(const void* data,const uint16_t dataSize);
void sendByte(const uint8_t c);

// FUNCTIONS
void sendSample(const uint16_t count,const uint16_t maxSamples,const float v,const float i);
void sendLineParameters();
void clearEnergy();

#endif /* _PHASOR_COMMUNICATION_H_ */
