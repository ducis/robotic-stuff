#include<avr/io.h>
#include"userinterface.h"
//#include"pid.h"

unsigned int data=0;
//unsigned char reg=0;
unsigned char rotatedir=0;
signed int speedset=0;
extern volatile unsigned int currentoutput;
extern volatile uint8_t flag;//中断标志
extern volatile uint8_t locate;//AD转换选择的二极管0~15
extern volatile uint8_t firstrun;
extern volatile uint8_t LED;
extern volatile unsigned char running;


void Write_Data(unsigned char registernum,unsigned long int data_need)
{
	if(registernum==0)
	{
		if (data_need == 1) {
			running = 1;
			flag = 0;
			locate = 0;
			LED = 0;
		}
		else {
			running = 0;
		}
	}
}
volatile unsigned long int DataSend(unsigned int registernum)
{
    volatile unsigned long int temp;
	if (registernum == 1) {
	 	temp = currentoutput;
	 	return temp;
	}
	else
		return 0;
}
