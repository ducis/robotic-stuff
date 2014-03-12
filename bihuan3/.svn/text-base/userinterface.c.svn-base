#include<avr/io.h>
#include"userinterface.h"
#include"pid.h"

unsigned int data=0;
//unsigned char reg=0;
unsigned char rotatedir=0;
signed int speedset=0;


void Write_Data(unsigned char registernum,unsigned long int data_need)
{
     //data=data_need;
	 //reg=registernum;
     if(registernum==0)
	 {
	     rotatedir=(unsigned char)((data_need&DIRBIT)>>9);
		 data=(unsigned int)(data_need&SPEEDBIT);
		 speedset=(signed int)(data/4);
		 if(rotatedir==1) speedset=-speedset;
         Set_Speed(speedset);
	  }
}
unsigned long int DataSend(unsigned int registernum)
{
     unsigned long int temp;
	 temp=(unsigned long int)(registernum);
	 return temp;
}
