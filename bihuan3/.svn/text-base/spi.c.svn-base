#include<avr/io.h>
#include<avr/interrupt.h>
#include"spi.h"
#include"Timer2.h"
#include"userinterface.h"
#include"uart.h"

volatile unsigned char nextstate=1;      //当前状态位置
volatile unsigned char receive_data=0;   //临时存放接收数据   
volatile unsigned char datadirect=0;     //数据传输方向，0为主机到从机，1为从机到主机
//unsigned char evenvertify=0;
volatile unsigned char vertifybit=0;     //校验码，校验方式为异或校验
volatile unsigned char registernum=0;    //寄存器号
volatile unsigned long int data_need=0;  //接收并处理接收的数据
volatile unsigned long int data_send=0;  //存放需要发送的数据
//unsigned char remainframe=0;
volatile unsigned char framenum=0;      //当前发送或接收的数据帧号
volatile unsigned char nframenum=0;     //接收时期望收到数据的帧号
volatile unsigned char mframenum=0;     //接收或发送数据需要用的数据帧数
volatile unsigned char righttrans=0;    //发送数据时需要右移的位数
volatile unsigned char tempsend=0;      //发送数据临时变量
volatile unsigned char frame_num[16]={NUMREC0,NUMREC1,NUMREC2,NUMREC3,NUMREC4,NUMREC5,NUMREC6,NUMREC7,NUMREC8,NUMREC9,NUMREC10,NUMREC11,NUMREC12,NUMREC13,NUMREC14,NUMREC15};  //存放事先约定的各个寄存器的数据帧个数，在头文件里定义
volatile unsigned int WriteLimit=WRITELIMIT;    //寄存器写权限设置 1为没有权限

void SPI_SlaveInit()
{
   nextstate=1;
   DDRB|=(1<<PB6);   //MISO设为输入
   SPCR|=(1<<SPE)|(1<<SPIE)|(1<<SPR1);
   SPSR|=(1<<SPI2X);    //32分频，500kb/s
   SPDR=FIRSTFRAME;
   //SPDR=150;
   //nextstate=9;
}

ISR(SPI_STC_vect)
{
   sei();
   switch(nextstate)
   {
   case 1: State1(); break;
   case 2: State2(); break;
   case 3: State3(); break;
   case 4: State4(); break;
   case 5: State5(); break;
   case 6: State6(); break;
   case 7: State7(); break;
   case 8: State8(); break;
   case 9: Test9(); break;   //测试用函数
   default: HitError(); break;
   }
   Timer2_Start();
}

void Test9()
{
    nextstate=9;
	SPDR=170;
}
void State1()
{
   receive_data=SPDR;
   if((receive_data&UPPERBIT)==STARTQUEUE)
   {
      datadirect=receive_data&DIRECTBIT;           //最后一位为方向
      registernum=((receive_data&LOWERBIT)>>1);    //寄存器号
	  if(registernum>=NUMOFRIG)
	  { 
	     SPDR=FIRSTFRAME;
         nextstate=1;
      }
	  else 
	  if(datadirect==0)   //主机到从机
	  { 
	     if((1<<registernum)&WriteLimit)
		 {
		  SPDR=WRONGCALL;
		  nextstate=8;
         }
		 else
		 {
	     nframenum=1;       //可要可不要
		 mframenum=frame_num[registernum];
		 data_need=0;      //数据缓冲清零,准备接收数据
		 vertifybit=0;     //校验区域清零
	     SPDR=READYFRAME;
		 nextstate=2;
		 }
      }
	  else           //从机到主机
	  {
	     framenum=1;       //可要可不要
		 mframenum=frame_num[registernum];
		 vertifybit=0;
         //data_send=DataSend(registernum);  //提取要发送的数据
		 data_send=125;
         righttrans=(mframenum-1)*5;
         tempsend=((unsigned char)(data_send>>righttrans))&LOWERBIT;
		 vertifybit^=tempsend;
         SPDR=(framenum<<5)|tempsend;
		 nextstate=6;
	  }
	}  
   else
   {
      SPDR=FIRSTFRAME;
	  //SPDR=TESTFRAME6;
	  nextstate=1;
   }
}

void State2()
{
   receive_data=SPDR;
   framenum=(receive_data&UPPERBIT)>>5;
   if(framenum==nframenum)
   {
      SPDR=READYFRAME;
	  //SPDR=TESTFRAME7;
	  vertifybit^=receive_data;
	  if(nframenum<mframenum)
	  {
	   nframenum++;    
	  data_need|=(receive_data&LOWERBIT);
	  data_need<<=5;
	  nextstate=2;
	  }
	  else
	  {
	  data_need|=(receive_data&LOWERBIT);
	  nextstate=3;
	  }	  
   }
   else    //帧号不匹配
   {
      SPDR=FIRSTFRAME;
	  //SPDR=TESTFRAME8;
	  nextstate=1;
    }
}

void State3()
{
	receive_data=SPDR;
	//if(receive_data==FINISHQUEUE)   //无校验
	if(((receive_data&UPPERBIT)==FINISHQUEUE)&&((receive_data&LOWERBIT)==(vertifybit&LOWERBIT)))  //异或校验
	{
	   SPDR=ENDFRAME;
	   nextstate=4;
    }
	else
	{
	   SPDR=FIRSTFRAME;
	   nextstate=1;
    }
}
   
void State4()
{
   
   SPDR=FIRSTFRAME;
   nextstate=1;
   //调用写变量及相关执行程序
   //receiveData=data_need;
   Write_Data(registernum,data_need);
   //UART_Transmit(0xab);
}

void State5()            //错误处理状态
{
   SPDR=FIRSTFRAME;
   //SPDR=TESTFRAME13;
   nextstate=1;
}

void State6()
{
	 receive_data=SPDR;
	 if(receive_data==READYFRAME)
	 {
	    framenum++;
		if(framenum<=mframenum)    //还有数据帧未发送
		{
	       righttrans-=5;
		   tempsend=((unsigned char)(data_send>>righttrans))&LOWERBIT;
		   vertifybit^=tempsend;
           SPDR=(framenum<<5)|tempsend;
		   nextstate=6;
         }
		 else
		 {
		    SPDR=FINISHQUEUE|(vertifybit&LOWERBIT);
			nextstate=7;
         }
      }
	  else {
	  	nextstate = 1;
		SPDR = FIRSTFRAME;
	}
}

void State7()
{
   
    SPDR=FIRSTFRAME;
	nextstate=1;    //恢复到状态1
}

void State8()   
{
    
   SPDR=FIRSTFRAME;
   nextstate=1;
}

void HitError()
{
    SPDR=ERRORFRAME;
	//SPDR=TESTFRAME22;
	nextstate=5;
}

