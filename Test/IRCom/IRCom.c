#include <avr/io.h>
#include <stdlib.h>
#include "index.h"





void SendSingleLowestBit(unsigned char toSend){
	PORTD |= _BV(PORTD1);
	{
		static const short waiting[2] = { BIT_PULSE_LEN_MOD_ZERO , BIT_PULSE_LEN_MOD_ONE };
		unsigned short i = waiting[toSend&1];
		for(;i>0;--i){
			TIFR &= ~_BV(OCF2);
			while(!(TIFR & _BV(OCF2)))
				;
		}
	}
	PORTD &= ~_BV(PORTD1);
	{
		unsigned short i = WAIT_AFTER_BIT;
		for(;i>0;--i){
			TIFR &= ~_BV(OCF2);
			while(!(TIFR & _BV(OCF2)))
				;
		}
	}
}

void WaitIRCarrierCycles(unsigned short toWait){
	for(;toWait>0;--toWait){
		TIFR &= ~_BV(OCF2);
		while(!(TIFR & _BV(OCF2)))
			;
	}
}


void SendBitStreamCh(unsigned char toSend, unsigned char n){
	for(;n;--n){
		SendSingleLowestBit(toSend);
		toSend>>=1;
	}
}


void SendLowestBit(unsigned char toSend){
	if( toSend & 1){
		PORTD &= ~_BV(PORTD1);
	}else{
		PORTD |= _BV(PORTD1);
	}
	{
		unsigned short i;
		for(i = BIT_LEN_MODIFIER; i>0; --i){
			TIFR &= ~_BV(OCF2);
			while(!(TIFR & _BV(OCF2)))
				;
		}
	}
}

void SendLowerHalfByte(unsigned char toSend){
	SendLowestBit(0);
	for(toSend = (toSend & ((1<<4)-1)) | 1<<4  ;  toSend>1  ;  toSend>>=1){  //Set toSend's higher half byte to 0001
		//Sending 0 as (0,1) and 1 as (1,0)
		SendLowestBit(toSend);
		SendLowestBit(~toSend);
	}
	SendLowestBit(1);
}

void SendByte(unsigned char toSend){
	SendLowerHalfByte(toSend);
	SendLowerHalfByte(toSend>>4);
}

void FlushRecv(){
	
}

void CheckRecv(){
	if(UCSRA & _BV(RXC)){
		unsigned char c = ~UDR;
		if(c){
			unsigned char b = ( c<SINGLE_PULSE_THRESHOLD ? 1 : 0 );
			CircularBitBuffer_PushBit( &g_recvBitBuffer, b );
		}
	}
}

void CheckRecvRedundent(){
	static unsigned char count = 0;
	static unsigned char cur = 0;
	char sth = 0;
	if(UCSRA & _BV(RXC)){
		unsigned char c = ~UDR;
		if(c){
			unsigned char b = ( c<SINGLE_PULSE_THRESHOLD ? 1 : 0 );
			if(b==cur){
				if(REDUNDENCY_RECV <= count){
					CircularBitBuffer_PushBit( &g_recvBitBuffer, b );
					count = 0;
				}else{
					++count;
				}
			}else{
				cur = b;
				count = 1;
			}
			sth = 1;
//			TCNT1 = 0;
//			TIFR&=~_BV(TOV1);
//			PORTB&=~_BV(1);
		}
	}
	if(sth){
		TCNT1 = 0;
		TIFR&=~_BV(TOV1);
		PORTB&=~_BV(1);
	}else if( (TCNT1>=TIMEOUT) || (TIFR&_BV(TOV1)) ){
		TCNT1 = 0;
		TIFR&=~_BV(TOV1);
		PORTB|=_BV(1);
		CircularBitBuffer_Clear(&g_recvBitBuffer);
		count = 0;
		cur = 0;
		UCSRB&=~_BV(RXEN);
		UCSRB|=_BV(RXEN);
	}
}

void StateProc_Init(){

	DDRB = _BV(DDB0)|_BV(DDB1);
	
	DDRD = _BV(DDD1)|_BV(DDD7);
	PORTD = 0;

	TCCR2 = _BV(WGM21)|_BV(COM20)|_BV(CS20);	
	TCNT2 = 0;
	OCR2 = CARRIER_FREQ_MODIFIER;

	UCSRA = _BV(UDRE);
	UCSRB = _BV(RXEN);
	UCSRC = _BV(URSEL)|_BV(UCSZ1)|_BV(UCSZ0);
	UBRRH = 0;
	UBRRL = 103;//Baud rate == 2400 bps

	TCCR1B = _BV(CS12)|_BV(CS10);
	TCNT1 = 0;

	//GoToState(&StateProc_ThirdTest);
	GoToState(&StateProc_CheckedComIdle);
	//GoToState(&StateProc_NoiseGen);
}
void StateProc_Idle(){
}
void StateProc_FirstTest(){
	if(!(PIND&_BV(PIND3))){
		SendByte(115);
	}
	FlushRecv();
	{
		unsigned long i = 100000;
		while(--i);
	}
}
void StateProc_SecondTest(){
	UCSRB&=~_BV(RXEN);
	if(!(PIND&_BV(PIND6))){ //S2
		SendSingleLowestBit(0);
	}else if(!(PIND&_BV(PIND3))){//S3
		SendSingleLowestBit(1);
	}
	UCSRB|=_BV(RXEN);
	{
		unsigned long i = 3000;
		while(--i){
			if(UCSRA & _BV(RXC)){
				unsigned char c = ~UDR;
				if(c){
					unsigned char mask = _BV(c<SINGLE_PULSE_THRESHOLD);
					if(PINB & mask){
						PORTB &= ~mask;
					}else{
						PORTB |= mask;
					}
				}
			}
		}
	}
}
void StateProc_ThirdTest(){
	unsigned char sth = 0;
	if(!(PIND&_BV(PIND6))){ //S2
		UCSRB&=~_BV(RXEN);
		SendBitStreamCh(5,4);
		UCSRB|=_BV(RXEN);
	}else if(!(PIND&_BV(PIND3))){//S3
		UCSRB&=~_BV(RXEN);
		SendBitStreamCh(3,4);
		UCSRB|=_BV(RXEN);
	}
	CheckRecvRedundent();
	if(CircularBitBuffer_NumBits(&g_recvBitBuffer)>=4){
		unsigned char c=0;
		unsigned char i=0;
		for(;i<4;++i){
			c |= CircularBitBuffer_PopBit(&g_recvBitBuffer)<<i;
		}
		if( c == 5 ){
			unsigned char mask = _BV(0);
			if(PINB & mask){
				PORTB &= ~mask;
			}else{
				PORTB |= mask;
			}
			sth = 1;
		}
	}
/*	if(sth){
		TCNT1 = 0;
		TIFR&=~_BV(TOV1);
		PORTB&=~_BV(1);
	}else if( (TCNT1>=TIMEOUT) || (TIFR&_BV(TOV1)) ){
		TCNT1 = 0;
		TIFR&=~_BV(TOV1);
		PORTB|=_BV(1);
		CircularBitBuffer_Clear(&g_recvBitBuffer);
//			count = 0;
//			cur = 0;
//			UCSRA &= ~_BV(RXC);
	}*/
}

void (*g_currentStateProc)() = NULL;//&StateProc_Init;

void GoToState(void *StateProc){
	g_currentStateProc = StateProc;
}

void StateProc_NoiseGen(){
	static int lim = RAND_MAX/500;
	static int limR = RAND_MAX/2;
	if(ButtonIsDown(S2)){
		--lim;
	}
	if(ButtonIsDown(S3)){
		++lim;
	}
	if(ButtonIsDown(S4)){
		--limR;
	}
	if(ButtonIsDown(S5)){
		++limR;
	}
	if(rand()<lim){
		SendSingleLowestBit(rand()<limR?1:0);
	}
}

int main(){
	CircularBitBuffer_Ctor(&g_recvBitBuffer);
	GoToState(&StateProc_Init);
	while(1){
		(*g_currentStateProc)();
	}
	return 0;
};


