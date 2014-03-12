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
    pulse=0;  //״̬����
}
ISR(INT0_vect)
{
	if(!(PIND&(1<<PD3)))    //��ת
	{pulse++;}
	else
	{pulse--;}      //��ת
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
    //MCUCR|=(1<<ISC00)|(1<<ISC01);   //�ж�0��Ϊ�������ж�
    //GICR|=(1<<INT0);               //�ж�0ʹ��
	//MCUCR|=(1<<ISC00)|(1<<ISC01)|(1<<ISC10)|(1<<ISC11);    //�ж�0��1��Ϊ�����ش���
	//GICR|=(1<<INT0)|(1<<INT1);      //�ж�0��1ʹ��
	//MCUCR|=(1<<ISC00);      //�ж�0��Ϊ�ش���
    //GICR|=(1<<INT0);
	MCUCR|=(1<<ISC00)|(1<<ISC10);
	GICR|=(1<<INT0)|(1<<INT1);
}

//������״̬�ռ���Ϊ��1:0+  2:1+   3:0-    4:1-   ��С�����ٶ���Ϊ��
ISR(INT0_vect)
{
	if(!(PIND&(1<<PD3)))  //0-״̬ 3
	Encoder_Status[1]=3;
	else
	Encoder_Status[1]=1;  //0+״̬ 1
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
    if(!(PIND&(1<<PD2)))   //1-״̬ 4
    Encoder_Status[1]=4;
	else
	Encoder_Status[1]=2;   //1+״̬ 2
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
