#ifndef USERINTERFACE_H
#define USERINTERFACE_H

#define WRITELIMIT 0b0000000000000010	//�Ĵ���дȨ������ 1Ϊû��Ȩ��

#include "spi.h"

/*
#define DIRBIT 0b1000000000
#define SPEEDBIT 0b0111111111
*/
void Write_Data(unsigned char registernum,unsigned long int data_need);
volatile unsigned long int DataSend(unsigned int registernum);

#endif
