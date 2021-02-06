#ifndef _PHASOR_UTILS_H_
#define _PHASOR_UTILS_H_

#include <stdint.h>
#include <sapi.h>


// return ascii from int value
char* itoa(int value, char* result, int base);

// LOGGING
#define LOG_INFO(x) uartWriteString(UART_USB,"INFO:"); uartWriteString(UART_USB,x); uartWriteString(UART_USB,"\r\n")
#define LOG_WARNING(x) uartWriteString(UART_USB,"WARNING!:"); uartWriteString(UART_USB,x); uartWriteString(UART_USB,"\r\n")
#define LOG_CRITICAL(x) uartWriteString(UART_USB,"CRITICAL!:"); uartWriteString(UART_USB,x); uartWriteString(UART_USB,"\r\n")


#endif /* _PHASOR_UTILS_H_ */
