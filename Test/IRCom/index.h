#ifndef _INDEX_H
#define _INDEX_H
#include "Basic.h"
#include "CircularBitBuffer.h"
#include "Buttons.h"



#define CARRIER_FREQ_MODIFIER 104
#define BIT_LEN_MODIFIER 200
#define BIT_PULSE_LEN_MOD_ZERO 300
#define BIT_PULSE_LEN_MOD_ONE 150
#define WAIT_AFTER_BIT 500
#define SINGLE_PULSE_THRESHOLD 12
#define REDUNDENCY_RECV 4
#define REDUNDENCY_SEND 5
#define TIMEOUT 1000

void SendBitStreamCh(unsigned char toSend, unsigned char n);
void SendSingleLowestBit(unsigned char toSend);
void WaitIRCarrierCycles(unsigned short toWait);

void StateProc_NoiseGen();
void StateProc_Init();
void StateProc_Idle();
void StateProc_FirstTest();
void StateProc_SecondTest();
void StateProc_ThirdTest();

void GoToState(void *StateProc);

void StateProc_CheckedComIdle();
void StateProc_CheckedComTestIdle();
void StateProc_CheckedComReceiving();
void StateProc_CheckedComSending();
void StateProc_CheckedComReplying(); 
void StateProc_CheckedComWaitingForReply();
void StateProc_CheckedComReceivingReply();
void StateProc_CheckedComReceivingComplete();


struct CircularBitBuffer g_recvBitBuffer;


BOOL GotBit();


#endif
