#include "comunication.h"

// GLOBAL VARIABLES
//static rtc_t rtc;
//static bool_t updateRtc;

static communicationManager_t comMngr;

// -- COMUNICATION INITIALIZATIONS -- //

// Configures all the parameters to the communication manager
// Tx,Rx Circular buffers, command buffer, UART callbacks
void comunicationManagerConfig(){

	// Buffers init
	RingBuffer_Init(&comMngr.TxRBuffer, comMngr.TxBuffer, sizeof(uint8_t),UART_BUFFER_TX_SIZE);
	RingBuffer_Init(&comMngr.RxRBuffer, comMngr.RxBuffer, sizeof(uint8_t),UART_BUFFER_RX_SIZE);
	comMngr.cmdSize = 0;

	// pending message flag init
	comMngr.bPendingMsg = FALSE;

	// UART init
	uartConfig(UART_COM, 115200);   // WebServer Comunication
	uartConfig(UART_DEBUG, 115200); // Debug

	// Callbacks
	uartCallbackSet(UART_COM, UART_RECEIVE,onRx,NULL);
	uartInterrupt(UART_COM, true);

	// Hello to server
	sendByte(CMD_HELLO);
	sendByte(CHAR_TERMINATOR);
}


// --- UART CALLBACKS --- //

// On receive Callback function
void onRx(void* unused){

	// Overflow callback (Warning!)
	if(RingBuffer_IsFull(&comMngr.RxRBuffer) == TRUE) onRxOverflow();

	// Insert to RxBuffer
	uint8_t c = uartRxRead(UART_COM);
	RingBuffer_Insert(&comMngr.RxRBuffer, &c);

	// Pending message enable
	comMngr.bPendingMsg = TRUE;
}

// Overflow function callback
// In a future, ask for more space
void onRxOverflow(){
	LOG_WARNING("Rx buffer overflow, message overwritten!");
}

// On transmiter free callback function
void onTxFree(void* unused){
	if(RingBuffer_IsEmpty(&comMngr.TxRBuffer) == TRUE)
	{
		// Disable Callback Interrupt
		uartCallbackClr(UART_COM, UART_TRANSMITER_FREE);
	} else {
		while(RingBuffer_IsEmpty(&comMngr.TxRBuffer) == FALSE && uartTxReady(UART_COM)){
			uint8_t c;
			RingBuffer_Pop(&comMngr.TxRBuffer,&c);
			uartTxWrite(UART_COM, c);
		}
	}
}


// --- PUBLIC FUNCTIONS --- //

// Send any kind of data to UART buffer, enables callback
// data, the data container
// dataSize, the size IN BYTES of the data
void sendData(const void* data,const uint16_t dataSize)
{
	// cast to uint8_t buffer array
	uint8_t* dataBuffer = (uint8_t*)data;

	// insert to uart buffer
	for(uint16_t i = 0; i < dataSize;i++){
		RingBuffer_Insert(&comMngr.TxRBuffer, &(dataBuffer[i]));
	}

	// Enable callback interrupt
	uartCallbackSet(UART_COM, UART_TRANSMITER_FREE,onTxFree,NULL);
	uartSetPendingInterrupt(UART_COM);
}

// Send a byte to UART buffer, enables callback
void sendByte(const uint8_t c)
{
	RingBuffer_Insert(&comMngr.TxRBuffer, &c);

	// Enable callback interrupt
	uartCallbackSet(UART_COM, UART_TRANSMITER_FREE,onTxFree,NULL);
	uartSetPendingInterrupt(UART_COM);
}


// Handles the UART Rx buffer
void handleMessages()
{
	uint8_t c;

	// if no messages pending, return
	if (comMngr.bPendingMsg == FALSE) return;

	//While Rx buffer not empty
	while(RingBuffer_IsEmpty(&comMngr.RxRBuffer) == FALSE)
	{

		RingBuffer_Pop(&comMngr.RxRBuffer,&c);

		// if string terminator, parse the command
		if(c == CHAR_TERMINATOR)
		{
			comMngr.cmdBuffer[comMngr.cmdSize] = CHAR_TERMINATOR;
			comMngr.cmdSize++;
			parseCommand(comMngr.cmdBuffer, comMngr.cmdSize);
			comMngr.cmdSize = 0;

		// else append it to the command buffer
		} else {
			if (comMngr.cmdSize >= CMD_BUFFER_SIZE){ // check overflow
				LOG_WARNING("CMD buffer overflow");
				break;
			}
			comMngr.cmdBuffer[comMngr.cmdSize] = c;
			comMngr.cmdSize++;
		}
	}
}


// parses the command line found in the UART buffer
void parseCommand(const uint8_t* cmd,const uint8_t size)
{
	if (size == 0) return;
	//ToDo:Validate data
	switch(cmd[0])
	{
	case CMD_HELLO:
		LOG_INFO("Server says hello!");
		// send acknowledge
		sendByte(CMD_ACK);
		sendData("\r\n",2);
		break;
	case CMD_ACK:
		LOG_INFO("Server acknowledge!");
		break;
	case CMD_RESET_KWH:
		clearEnergy();
		break;
	case CMD_REQ_PARAMS_ON:
		bEnableSendParams = TRUE;
		break;
	case CMD_REQ_PARAMS_OFF:
		bEnableSendParams = FALSE;
		break;
	case CMD_UPDATE_RTC:
		//ToDo:Validate data
		// year is uint16_t
		//rtc.year = (*(uint16_t*)&cmd[1]);
		//rtc.month = cmd[3];
		//rtc.mday = cmd[4];
		//rtc.wday = cmd[5];
		//rtc.hour = cmd[6];
		//rtc.min = cmd[7];
		//rtc.sec = cmd[8];
		//bUpdateRtc = TRUE;
	case CMD_REQ_CYCLE_SAMPLES:
		bEnableSendSamples = TRUE;
		break;

	default:
		LOG_INFO("command ID Unknown!");
		break;
	}
}


// Sends one sample to the webServer
// [commandID, count, maxSample, v, i, \n]
// [S, 12, 128, 220.0, 10.0]
void sendSample(const uint16_t count,const uint16_t maxSamples,const float v,const float i)
{
	// Identifier
	sendByte(CMD_SENDING_CYCLE_SAMPLE);
	// sample number
	sendData(&count,sizeof(uint16_t));
	// max sample number
	sendData(&maxSamples,sizeof(uint16_t));
	// v
	sendData(&v,sizeof(float));
	// i
	sendData(&i,sizeof(float));
	// terminator
	sendByte(CHAR_TERMINATOR);
}

// Send the calculated parameters to the webServer
// [commandID, Vrms, Irms, CosPhi, \n]
// [P, 220.0, 5.0, -7.0]
void sendLineParameters()
{
	// Identifier
	sendByte(CMD_SENDING_PARAMS);
	// Vrms
	sendData(&currentParams.Vrms,sizeof(float));
	// Irms
	sendData(&currentParams.Irms,sizeof(float));
	// Cos Phi
	sendData(&currentParams.CosPhi,sizeof(float));

	// terminator
	sendByte(CHAR_TERMINATOR);
}

// Clears the energy [kWh] counter
void clearEnergy()
{
	return;
}
