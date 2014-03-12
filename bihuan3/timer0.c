#include<avr/io.h>
#include<avr/interrupt.h>
#include"timer0.h"
#include"pwm.h"
#include"uart.h"
//#include"adc.h"
#include"encoder.h"
#include"pid.h"

unsigned int count=0;
//unsigned char j;
signed int Speed_Now=0;
unsigned int Abs_Speed=0;
signed long int Controldata=0;
void Timer0_Init()
{
     TCCR0=0;
	 TCNT0=131;   //2ms
	 //TCNT0=195;   //1ms
	 //TCNT0=220;
	 TIMSK|=(1<<TOIE0);
	 TCCR0|=(1<<CS02);
	 count=0;  //状态清零
}



ISR(TIMER0_OVF_vect)
{
     Speed_Now=SpeedCal();
	 //Speed_Now=48;
     //First_Flag=0;
	 //pulse=0; 
	 StateClear();
     
     TCCR0=0;
	 TCNT0=131;
	 //TCNT0=195;
	 //TCNT0=220;
	 TCCR0|=(1<<CS02);
	 sei();
	 //Set_Speed(32);
     //unsigned int Voltage=0;
	 //unsigned int Current=0;
     Controldata=PIDSpeed(Speed_Now);
     Set_PWM(Controldata);
	 //Set_PWM(30);
	 count++;
	 //UART_Transmit(0xcd);
	 if(count==10)
	 {
	 //unsigned int Current=0;
	 //Voltage=ADC_Conversion(VoltageChannel);
	 //Current=ADC_Conversion(CurrentChannel);
	 //UART_Transmit(0xaf);
	 //UART_Transmit((unsigned char)(Voltage>>8));
	 //UART_Transmit((unsigned char)(Voltage));
	 //UART_Transmit(0xab);
	 //UART_Transmit((unsigned char)(Current>>8));
	 //UART_Transmit((unsigned char)(Current));
	 //UART_Transmit(0xcc);
	 /*UART_Transmit(0xab);
	 UART_Transmit((unsigned char)(Controldata>>8));
	 UART_Transmit((unsigned char)(Controldata));
	 UART_Transmit(0xcc);*/

	 /*if(Speed_Now>=0)
	 {
		   //UART_Transmit(0x00);
		   //UART_Transmit(0xaf);
		   Abs_Speed=(unsigned int)(Speed_Now);
		   UART_Transmit((unsigned char)(Abs_Speed>>8));
		   UART_Transmit((unsigned char)(Abs_Speed));
      }
	  else
	  {
		   //UART_Transmit(0x01);
		   //UART_Transmit(0xbf);
		   Abs_Speed=(unsigned int)(-Speed_Now);
		   UART_Transmit((unsigned char)(Abs_Speed>>8));
		   UART_Transmit((unsigned char)(Abs_Speed));
       }*/
       UART_Transmit((unsigned char)(Speed_Now>>8));
	   UART_Transmit((unsigned char)(Speed_Now));
	  //UART_Transmit(0x50); 
	  count=0;
	  }
}


/*unsigned char j;
void Timer0_Init()
{
    TCCR0=0x00;
	TCNT0=131;         //2ms
	//TCNT0=0;
	TIMSK|=(1<<TOIE0);
	TCCR0|=(1<<CS02);
}

ISR(TIMER0_OVF_vect)
{
    Speed_Now=pulse;
    First_Flag=0;
	pulse=0;                       //这几行代码可能会有问题，不用调用函数，那样会减慢执行速度
    sei();
    TCCR0=0x00;
	TCNT0=131;       //2ms
	//TCNT0=0;
	TCCR0|=(1<<CS02);
	count++;
	if(count==1000)
	{
	    count=0;
		for(j=0;j<12;j++)
		{
		    UART_Transmit(Try_Status[j]);
        }
		if(Speed_Now>=0)
		{
		   //UART_Transmit(0x00);
		   UART_Transmit(0xaf);
		   Abs_Speed=Speed_Now;
		   //UART_Transmit((unsigned char)(Abs_Speed>>8));
		   UART_Transmit((unsigned char)(Abs_Speed));
         }
		 else
		 {
		   //UART_Transmit(0x01);
		   UART_Transmit(0xbf);
		   Abs_Speed=-Speed_Now;
		   //UART_Transmit((unsigned char)(Abs_Speed>>8));
		   UART_Transmit((unsigned char)(Abs_Speed));
          }
	}
}*/
