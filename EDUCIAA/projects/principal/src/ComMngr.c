#include "ComMngr.h"

extern params_t params;

// -- COMUNICATION INITIALIZATIONS -- //

// Configures all the parameters to the communication manager
// Tx,Rx Circular buffers, command buffer, UART callbacks
void ComMngr_Init(communicationManager_t* cm){

	// Buffers init
	RingBuffer_Init(&cm->TxRBuffer, cm->TxBuffer, sizeof(uint8_t),UART_BUFFER_TX_SIZE);
	RingBuffer_Init(&cm->RxRBuffer, cm->RxBuffer, sizeof(uint8_t),UART_BUFFER_RX_SIZE);
	cm->cmdSize = 0;

	// pending message flag init
	cm->bPendingMsg = FALSE;

	// UART init
	uartConfig(UART_COM, 115200);   // WebServer Comunication
	uartConfig(UART_DEBUG, 115200); // Debug

	// Callbacks
	uartCallbackSet(UART_COM, UART_RECEIVE,onRx,NULL);
	uartInterrupt(UART_COM, true);

	// Hello to server
	ComMngr_SendByte(cm ,CMD_HELLO);
	ComMngr_SendByte(cm ,CHAR_RETURN_CARRY);
	ComMngr_SendByte(cm ,CHAR_TERMINATOR);
}


// --- UART CALLBACKS --- //

// On receive Callback function
void onRx(void* unused){

	// Overflow callback (Warning!)
	if(RingBuffer_IsFull(&ComMngr.RxRBuffer) == TRUE) onRxOverflow();

	// Insert to RxBuffer
	uint8_t c = uartRxRead(UART_COM);
	RingBuffer_Insert(&ComMngr.RxRBuffer, &c);

	// Pending message enable
	ComMngr.bPendingMsg = TRUE;
}


// Overflow function callback
// In a future, ask for more space maybe
void onRxOverflow(){
	LOG_WARNING("Rx buffer overflow, message overwritten!");
}


// On transmiter free callback function
void onTxFree(void* unused){
	if(RingBuffer_IsEmpty(&ComMngr.TxRBuffer) == TRUE)
	{
		// Disable Callback Interrupt
		uartCallbackClr(UART_COM, UART_TRANSMITER_FREE);
	} else {
		while(RingBuffer_IsEmpty(&ComMngr.TxRBuffer) == FALSE && uartTxReady(UART_COM)){
			uint8_t c;
			RingBuffer_Pop(&ComMngr.TxRBuffer,&c);
			uartTxWrite(UART_COM, c);
		}
	}
}


// --- PUBLIC FUNCTIONS --- //

// Send any kind of data to UART buffer, enables Tx callback
// @param data		: ptr to data
// @param dataSize	: size of data in Bytes
void ComMngr_SendData(communicationManager_t* cm, const void* data, const uint16_t dataSize)
{
	// cast to uint8_t buffer array
	uint8_t* dataBuffer = (uint8_t*)data;

	// insert to uart buffer
	for(uint16_t i = 0; i < dataSize;i++){
		RingBuffer_Insert(&cm->TxRBuffer, dataBuffer++);
	}

	// Enable callback interrupt
	uartCallbackSet(UART_COM, UART_TRANSMITER_FREE,onTxFree,NULL);
	uartSetPendingInterrupt(UART_COM);
}


// Send single byte to UART buffer, enables Tx callback
void ComMngr_SendByte(communicationManager_t* cm, const uint8_t c)
{
	// insert byte to UART buffer
	RingBuffer_Insert(&cm->TxRBuffer, &c);

	// Enable callback interrupt
	uartCallbackSet(UART_COM, UART_TRANSMITER_FREE,onTxFree,NULL);
	uartSetPendingInterrupt(UART_COM);
}


// Checks for pending messages on the UART Rx buffer and parses the message to command
void ComMngr_HandleMessages(communicationManager_t* cm)
{
	uint8_t c;

	// if no messages pending, return
	if (cm->bPendingMsg == FALSE) return;

	//While Rx buffer not empty
	while(RingBuffer_IsEmpty(&cm->RxRBuffer) == FALSE)
	{
		// get byte
		RingBuffer_Pop(&cm->RxRBuffer,&c);

		// if \n: parse the command
		if(c == CHAR_TERMINATOR)
		{
			// append terminator
			cm->cmdBuffer[cm->cmdSize] = CHAR_TERMINATOR;
			cm->cmdSize++;
			// parse command
			ComMngr_ParseCommand(cm,cm->cmdBuffer,cm->cmdSize);
			cm->cmdSize = 0;
		// else append char to the command buffer
		} else {
			// check overflow
			if (cm->cmdSize >= CMD_BUFFER_SIZE){
				LOG_WARNING("CMD buffer overflow");
				break;
			}
			// append char
			cm->cmdBuffer[cm->cmdSize] = c;
			cm->cmdSize++;
		}
	}
}


// Parses the full command line
void ComMngr_ParseCommand(communicationManager_t* cm, const uint8_t* cmd,const uint8_t size)
{
	if (size == 0) return;
	//ToDo:Validate data
	switch(cmd[0])
	{
	case CMD_HELLO:
		LOG_INFO("Server says hello!");
		// send acknowledge
		ComMngr_SendByte(cm, CMD_ACK);
		ComMngr_SendByte(cm, CHAR_RETURN_CARRY);
		ComMngr_SendByte(cm, CHAR_TERMINATOR);
		break;
	case CMD_ACK:
		LOG_INFO("Server acknowledge!");
		break;
	case CMD_RESET_KWH:
		clearEnergy();
		break;
	case CMD_REQ_PARAMS_ON:
		params.bEnableSend = TRUE;
		break;
	case CMD_REQ_PARAMS_OFF:
		params.bEnableSend = FALSE;
		break;
	case CMD_UPDATE_RTC:
		//ToDo
		break;
	case CMD_REQ_CYCLE_SAMPLES:
		bEnableSendSamples = TRUE;
		break;
	default:
		LOG_INFO("command ID Unknown!");
		break;
	}
}


// Sends one sample to the webServer
// [commandID, sampleCount, v, i, \n]
// Eg.:[S, 12, 128, 220.0, 10.0]
void ComMngr_SendSample(communicationManager_t* cm,  sample_t const *const s,const uint16_t nSample)
{
	// Identifier
	ComMngr_SendByte(cm,CMD_SENDING_CYCLE_SAMPLE);
	// sample number
	ComMngr_SendData(cm,&(nSample),sizeof(uint16_t));
	// v
	ComMngr_SendData(cm,&s->v,sizeof(float));
	// i
	ComMngr_SendData(cm,&s->i,sizeof(float));
	// terminator
	ComMngr_SendByte(cm, CHAR_RETURN_CARRY);
	ComMngr_SendByte(cm, CHAR_TERMINATOR);
}


// Sends the calculated parameters to the webServer each second
// [commandID, Vrms, Irms, Phi, \n]
// Eg.:[P, 220.0, 5.0, -7.0]
void ComMngr_SendParams(communicationManager_t* cm, params_t const *const p)
{
	// Identifier
	ComMngr_SendByte(cm, CMD_SENDING_PARAMS);
	// Vrms
	ComMngr_SendData(cm, &p->computed.Vrms, sizeof(float));
	// Irms
	ComMngr_SendData(cm, &p->computed.Irms, sizeof(float));
	// Cos Phi
	ComMngr_SendData(cm, &p->computed.Phi, sizeof(float));
	// terminator
	ComMngr_SendByte(cm, CHAR_RETURN_CARRY);
	ComMngr_SendByte(cm, CHAR_TERMINATOR);
}

// Clears the energy [kWh] counter
void clearEnergy()
{
	//ToDo
	return;
}
