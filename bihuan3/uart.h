#ifndef UART_H
#define UART_H

#define Freq_CPU 16000000
#define BAUDRATE 57600

void UART_Init(unsigned long int freq,unsigned long int baudrate);
void UART_Transmit(unsigned char txdata);

#endif
