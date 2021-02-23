#ifndef _PHASOR_COMMANAGER_H_
#define _PHASOR_COMMANAGER_H_

#include <stdint.h>
#include <sapi.h>
#include <ring_buffer.h>
#include "digitalization.h"
#include "utils.h"

// --- DEFINITIONS --- //
#define UART_BUFFER_TX_SIZE 256
#define UART_BUFFER_RX_SIZE 32
#define CMD_BUFFER_SIZE 16


//UART ports
#define UART_COM UART_232
#define UART_DEBUG UART_USB

//Raw samples to send per request
#define N_SAMPLES_TO_SEND 40

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

// Communication Manager Data Structure
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


// --- GLOBAL FLAGS --- //
extern bool_t bEnableSendParams;
extern volatile bool_t bEnableSendSamples;

// --- EXTERN GLOBAL VARIABLES --- //
extern params_t currentParams,computedParams;






// --- FUNCTIONS DEFINITIONS --- //

// UART Callbacks
void onTxFree(void* unused);
void onRx(void* unused);
void onRxOverflow();

// Comunication functions
void ComMngr_Init(communicationManager_t* cm);
void ComMngr_HandleMessages(communicationManager_t* cm);
void ComMngr_ParseCommand(communicationManager_t* cm, const uint8_t* cmd,const uint8_t size);
void ComMngr_SendData(communicationManager_t* cm,const void* data,const uint16_t dataSize);
void ComMngr_SendByte(communicationManager_t* cm, const uint8_t c);

// FUNCTIONS
void ComMngr_SendSample(communicationManager_t* cm, const sample_t s,const uint16_t count);
void ComMngr_SendLineParams(communicationManager_t* cm);

void clearEnergy();

#endif /* _PHASOR_COMMANAGER_H_ */
