#include<avr/io.h>
#include<avr/interrupt.h>
#include"pwm.h"

void PWM_Init()
{
	DDRD|=(1<<PD5)|(1<<PD4);
    TCCR1A|=(1<<WGM11)|(1<<COM1A1)|(1<<COM1B1)|(1<<COM1B0);
    TCCR1B|=(1<<WGM13)|(1<<CS10);
	ICR1=400;
	OCR1A=200;
	OCR1B=200;
}
void Set_PWM(signed long int controldata)
{
    signed int res;
	if(controldata>180)res=180;
	else if(controldata<-180)res=-180;
	else res=(signed int)controldata;
    cli();            //关闭全局中断，防止两个PWM波改变不同步
	OCR1A=res+200;
    OCR1B=OCR1A;
    sei();
}
