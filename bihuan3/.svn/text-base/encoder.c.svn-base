#include<avr/io.h>
#include<avr/interrupt.h>
#include"encoder.h"

//signed char Encoder_Status[2]={0,0};
//unsigned char i=0;
signed int pulse=0;
//unsigned char First_Flag=0;

void Encoder_Init()
{
	MCUCR|=(1<<ISC01)|(1<<ISC00);
    GICR|=(1<<INT0);
    pulse=0;  //状态清零
}
ISR(INT0_vect)
{
	if(!(PIND&(1<<PD3)))    //正转
	{pulse++;}
	else
	{pulse--;}      //反转
}
void StateClear()
{
   pulse=0;	 
}

signed int SpeedCal()
{
    return pulse;
}
/*void Encoder_Init()
{
    //MCUCR|=(1<<ISC00)|(1<<ISC01);   //中断0设为上升沿中断
    //GICR|=(1<<INT0);               //中断0使能
	//MCUCR|=(1<<ISC00)|(1<<ISC01)|(1<<ISC10)|(1<<ISC11);    //中断0、1设为上升沿触发
	//GICR|=(1<<INT0)|(1<<INT1);      //中断0、1使能
	//MCUCR|=(1<<ISC00);      //中断0设为沿触发
    //GICR|=(1<<INT0);
	MCUCR|=(1<<ISC00)|(1<<ISC10);
	GICR|=(1<<INT0)|(1<<INT1);
}

//编码器状态空间编号为：1:0+  2:1+   3:0-    4:1-   从小到大速度设为正
ISR(INT0_vect)
{
	if(!(PIND&(1<<PD3)))  //0-状态 3
	Encoder_Status[1]=3;
	else
	Encoder_Status[1]=1;  //0+状态 1
	if(First_Flag==0)
	{
	    Encoder_Status[0]=Encoder_Status[1];
		First_Flag=1;
     }
	 else
	 {
	    if(Encoder_Status[1]==3)
		{
	    pulse+=(Encoder_Status[1]-Encoder_Status[0]);
		Encoder_Status[0]=Encoder_Status[1];
		}
		else   //Encoder_Status[1]=1
		{
		   if(Encoder_Status[0]==2)pulse--;
		   else pulse++;
		   Encoder_Status[0]=Encoder_Status[1];
        }
     }
	 
}

ISR(INT1_vect)
{
    if(!(PIND&(1<<PD2)))   //1-状态 4
    Encoder_Status[1]=4;
	else
	Encoder_Status[1]=2;   //1+状态 2
	if(First_Flag==0)
	{
	    Encoder_Status[0]=Encoder_Status[1];
		First_Flag=1;
     }
	 else
	 {
	    if(Encoder_Status[1]==2)
		{
	    pulse+=(Encoder_Status[1]-Encoder_Status[0]);
		Encoder_Status[0]=Encoder_Status[1];
		}
		else //Encoder_Status[1]=4
		{
		   if(Encoder_Status[0]==3)pulse++;
		   else pulse--;
           Encoder_Status[0]=Encoder_Status[1];
        }
     }
	
}

void StateClear()
{
   First_Flag=0;
   pulse=0;	 
}

signed int SpeedCal()
{
    return pulse;
}*/
