#include "index.h"

#define STATECAST_NUM  64
#define SYMBOLLENGTH 4
#define SENDSYMBOLWAIT (2*(RECVBITWAIT))
#define RECVBITWAIT (WAIT_AFTER_BIT + BIT_PULSE_LEN_MOD_ZERO + BIT_PULSE_LEN_MOD_ONE)
#define MAXERROR 6

unsigned char g_checkedComToSend;
unsigned char g_checkedComLastReceived;


unsigned char CountBitsDW(unsigned long dw){
	unsigned char r=0;
	while(dw){
		if(dw&1){
			++r;
		}
		dw>>=1;
	}
	return r;
}


unsigned const long g_symbols[1<<SYMBOLLENGTH] = {
/*  2753025324,
  2753025491,
  2752981548,
  2752981715,
  2766722348,
  2766722515,
  2766678572,
  2766678739,
  1528288556,
  1528288723,
  1528244780,
  1528244947,
  1541985580,
  1541985747,
  1541941804,
  1541941971//8 */

  
   954111155,
   916253645,
   447123723,
  1310326899,
   815014167,
  3641068181,
  1472136002,
  4114005095,
  3371221926,
  2246453070,
  3423433394,
  2266617271,
  3075014552,
  3686495895,
   521472158,
    64680612//12
};




BOOL CheckedComTryToStartReceiving(){
	if(UCSRA & _BV(RXC)){
		unsigned char c = ~UDR;
		if(c){
			GoToState( &StateProc_CheckedComReceiving );
			return TRUE;
		}
	}
	return FALSE;
}


void CheckedComStartSending(unsigned char data){
	g_checkedComToSend = data;
	GoToState( &StateProc_CheckedComSending );
}


void StateProc_CheckedComIdle(){
	GoToState(&StateProc_CheckedComTestIdle);
}


void StateProc_CheckedComTestIdle(){
	if(CheckedComTryToStartReceiving()){
	}else if(ButtonIsDown(S2)){
		CheckedComStartSending(0);
	}else if(ButtonIsDown(S3)){
		CheckedComStartSending(1);
	}else if(ButtonIsDown(S4)){
		CheckedComStartSending(2);
	}else if(ButtonIsDown(S5)){
		CheckedComStartSending(3);
	}
}


void StateProc_CheckedComReceiving(){
	static unsigned char lastState=0;
	unsigned long dw = 0;
	unsigned char p = 0;
	unsigned short toWait = RECVBITWAIT;
	while( (UCSRA & _BV(RXC)) || toWait ){
		if(UCSRA & _BV(RXC)){
			unsigned char c = ~UDR;
			if(c){
				if(c<SINGLE_PULSE_THRESHOLD){
					dw|=(1L<<p);
				}
				++p;
				toWait = RECVBITWAIT;
			}
		}else{
			TIFR &= ~_BV(OCF2);
			while(!(TIFR & _BV(OCF2)))
				;
			--toWait;
		}
	}
//	for( p=0; p<(1<<SYMBOLLENGTH); ++p ){
//		if(g_symbols[p] == dw){
//			if(p==lastState){
//				GoToState(StateProc_CheckedComIdle);
//				return;
//			}
//			g_checkedComLastReceived = ((p|(1<<SYMBOLLENGTH)) - lastState) & ((1<<SYMBOLLENGTH)-1);
//			lastState = p;
//			--g_checkedComLastReceived;
//			GoToState(&StateProc_CheckedComReceivingComplete);
//			return;
//		}
//	}
	{
		unsigned char minError = 100;
		unsigned char bestMatch = lastState;
		for( p=0; p<(1<<SYMBOLLENGTH) && minError; ++p ){
			unsigned char n = CountBitsDW(g_symbols[p]^dw);
			if(n<minError){
				bestMatch = p;
				minError = n;
			}else if(n==minError){
				bestMatch = lastState;
			}
		}
		if(bestMatch!=lastState && minError<=MAXERROR){
			g_checkedComLastReceived = ( ((bestMatch|(1<<SYMBOLLENGTH)) - lastState) & ((1<<SYMBOLLENGTH)-1) ) - 1;
			lastState = bestMatch;
			GoToState(&StateProc_CheckedComReceivingComplete);
			return;
		}
	}
	GoToState(StateProc_CheckedComIdle);
}


void StateProc_CheckedComSending(){
	static unsigned char lastState=0;
	lastState = (lastState+g_checkedComToSend+1)&((1<<SYMBOLLENGTH)-1);
	unsigned char i;
	UCSRB&=~_BV(RXEN);
	for(i=0;i<STATECAST_NUM;++i){
		unsigned long dw = g_symbols[lastState];
		SendSingleLowestBit(0);
		while(dw){
			SendSingleLowestBit(dw & 1);
			dw>>=1;
		}
		WaitIRCarrierCycles(SENDSYMBOLWAIT);
	}
	UCSRB|=_BV(RXEN);
	GoToState(StateProc_CheckedComIdle);
}


void StateProc_CheckedComReplying(){
}


void StateProc_CheckedComWaitingForReply(){
}


void StateProc_CheckedComReceivingReply(){
}


void StateProc_CheckedComReceivingComplete(){
	if(g_checkedComLastReceived==0){		
		PORTB |= 1;
	}else if(g_checkedComLastReceived==1){
		PORTB &= ~1;
	}else if(g_checkedComLastReceived==2){
		PORTB |= 2;
	}else if(g_checkedComLastReceived==3){
		PORTB &= ~2;
	}
	GoToState(&StateProc_CheckedComIdle);
}
