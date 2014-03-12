#include<avr/io.h>
#include<avr/interrupt.h>
#include"uart.h"

void UART_Init(unsigned long int freq,unsigned long int baudrate)
{
    unsigned int baud;
	baud=(unsigned int)(freq/baudrate/16-1);
    UBRRH=(unsigned char)(baud>>8);
	UBRRL=(unsigned char)(baud);
    UCSRB|=(1<<RXCIE)|(1<<RXEN)|(1<<TXEN);
}

void UART_Transmit(unsigned char txdata)
{
    while(!(UCSRA&(1<<UDRE)));
	UDR=txdata;
}

ISR(USART_RXC_vect)
{
    unsigned char rxdata;
	rxdata=UDR;
	UART_Transmit(rxdata);
}

