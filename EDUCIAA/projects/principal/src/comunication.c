#include "comunication.h"


// Configure all parameters to the communication manager
void comunicationManagerConfig(communicationManager_t* ComMngr){

	// Buffers init
	RingBuffer_Init(&ComMngr->TxRBuffer,ComMngr->TxBuffer, sizeof(uint8_t),UART_BUFFER_TX_SIZE);
	RingBuffer_Init(&ComMngr->RxRBuffer,ComMngr->RxBuffer, sizeof(uint8_t),UART_BUFFER_RX_SIZE);
	ComMngr->cmdSize = CMD_BUFFER_SIZE;

	ComMngr->bPendingMsg = FALSE;

	// UART init
	uartConfig(UART_COM, 115200); // WebServer Comunication
	uartConfig(UART_DEBUG, 115200); // Debug

	uartCallbackSet(UART_COM, UART_RECEIVE,onRxCallback,NULL);

	// Hello to server
	sendByte(CMD_HELLO);
	sendByte(CMD_TERMINATOR);
}

// Callback function when rx is
void onRxCallback(void* unused){
	// Overflow callback (Warning!)
	if(RingBuffer_IsFull(&communicationManager.RxRBuffer) == TRUE)
		onRxBufferOverflow();
	// Instert to RxBuffer
	uint8_t c = uartRxRead(UART_COM);
	RingBuffer_Insert(&communicationManager.RxRBuffer, &c);
	// Pending message enable
	communicationManager.bPendingMsg = TRUE;
}


void onRxBufferOverflow(){
	LOG_WARNING("Rx buffer overflow, message overwritten!");
}

// Callback function when transmitter isn't busy. Clear when txbuffer is empty
void onTxFreeCallback(void* unused){
	if(RingBuffer_IsEmpty(&communicationManager.TxRBuffer) == TRUE)
	{
		// Disable Callback Interrupt
		uartCallbackClr(UART_COM, UART_TRANSMITER_FREE);
	} else {
		while(RingBuffer_IsEmpty(&communicationManager.TxRBuffer) == FALSE && uartTxReady(UART_COM)){
			uint8_t c;
			RingBuffer_Pop(&communicationManager.TxRBuffer,&c);
			uartTxWrite(UART_COM, c);
		}
	}
}

void sendData(const uint8_t* data,const uint16_t dataSize)
{
	for(uint16_t i = 0; i < dataSize;i++){
		RingBuffer_Insert(&communicationManager.TxRBuffer, &data[i]);
	}

	// Enable callback interrupt
	uartCallbackSet(UART_COM, UART_TRANSMITER_FREE,onTxFreeCallback,NULL);
	uartSetPendingInterrupt(UART_COM);
}

void sendByte(const uint8_t c)
{
	RingBuffer_Insert(&communicationManager.TxRBuffer, &c);
	// Enable callback interrupt
	uartCallbackSet(UART_COM, UART_TRANSMITER_FREE,onTxFreeCallback,NULL);
	uartSetPendingInterrupt(UART_COM);
}

void handleMessages()
{
	// for readability
	const bool_t *const pending = &communicationManager.bPendingMsg;
	uint8_t *const cmdBuff = communicationManager.cmdBuffer;
	uint16_t *const cmdSize = &communicationManager.cmdSize;

	// if no messages pending, return
	if (*pending == FALSE) return;

	//While Rx buffer not empty
	while(RingBuffer_IsEmpty(&communicationManager.RxRBuffer) == FALSE)
	{
		uint8_t c;
		RingBuffer_Pop(&communicationManager.RxRBuffer,&c);
		if(c == CMD_TERMINATOR)
		{
			cmdBuff[*cmdSize] = CMD_TERMINATOR;
			(*cmdSize)++;
			parseCommand(cmdBuff,*cmdSize);
			*cmdSize = 0;
		} else {
			if (*cmdSize >= CMD_BUFFER_SIZE){
				LOG_WARNING("CMD buffer overflow");
				break;
			}
			cmdBuff[*cmdSize] = c;
			(*cmdSize)++;
		}
	}
}

void parseCommand(const uint8_t* cmd,const uint8_t size)
{
	if (size == 0) return;
	//ToDo:Validate data
	switch(cmd[0])
	{
	case CMD_HELLO:
		LOG_INFO("Server says hello!");
	case CMD_RESET_KWH:
		clearEnergy();
		break;
	case CMD_REQ_PARAMS_ON:
		bEnableSendParams == TRUE;
		break;
	case CMD_REQ_PARAMS_OFF:
		bEnableSendParams == FALSE;
		break;
	case CMD_UPDATE_RTC:
		//ToDo:Validate data
		rtc.year = (*(uint16_t*)&cmd[1]);
		rtc.month = cmd[3];
		rtc.mday = cmd[4];
		rtc.wday = cmd[5];
		rtc.hour = cmd[6];
		rtc.min = cmd[7];
		rtc.sec = cmd[8];
		bUpdateRtc = TRUE;
	case CMD_REQ_CYCLE_SAMPLES:
		break;
	default:
		LOG_INFO("Unable to Parse command!");
		LOG_INFO(cmd);
		break;
	}
}


void sendLineParameters()
{
	// ID for parameters data line   CMDID,VRMS,IRMS,COSHPHI,KWHCOUNT,KWHRESETTIME
	// debug
	uint8_t uartBuff[10];
	itoa( adcRead(CH1) , uartBuff, 10 );
	uartWriteString(UART_232,uartBuff);
	uartWriteString(UART_232,",");
	itoa( adcRead(CH2) , uartBuff, 10 );
	uartWriteString(UART_232,uartBuff);
	uartWriteString(UART_232,"\r\n");
}


void clearEnergy()
{
	return;
}
