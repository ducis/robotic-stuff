/**************************************************************************
取样方式：
	灯该亮处，按S1，得到LED灯亮时的取样结果。
	灯该灭处，按S2，得到背景光的取样结果。

SPI为主机模式，时钟速率为fck/16
	如果灯在线上，输出中线，数值为1~16
	如果灯不在线上，输出20
	
	
????串行发送结束是否是指从机接收到？


**************************************************************************/

#include<avr/io.h>
#include<avr/interrupt.h>
#define F_CPU 8000000UL//8MHz
#include<util/delay.h>
#include<stdint.h>
#include<avr/eeprom.h>
#include "spi.h"
#define true 1
#define false 0
#define ThresholdRatio 2/3
volatile uint8_t light[16];//保存需要亮的F灯
volatile uint8_t Threshold_H[16];//AD转换阈值
volatile uint8_t Threshold_L[16];
volatile uint16_t AD_TEMP[9][16];//调整阈值时使用的临时存储空间
volatile uint8_t LED=0;//选择需要扫描的光敏二极管,每次加上10000B（16）
volatile uint8_t flag=0;//中断标志
volatile uint8_t timeflag=0;
volatile uint16_t ADC_RESULT;//保存ADC转换结果，右对齐
volatile uint8_t locate;//AD转换选择的二极管0~15
volatile uint8_t times_in_adjust;//调整阈值时的次数0~4
volatile uint8_t h_adjust=false;//高阈值调整判断
volatile uint8_t l_adjust=false;//低阈值调整判断
volatile unsigned char running = 1;
volatile unsigned int currentoutput = 0;

void ShiftReg(uint8_t SR);//使用移位寄存器点灯
void LightLED();//根据light[16]的值点灯
void INIT_AD_STATISTIC( );//初始化阈值
void ADC_H();
void ADC_L();
void OutputData();
//SIGNAL(SIG_OUTPUT_COMPARE0);//T0匹配中断

int main()
{
	/* 产生12kHz PWM波 */
	OCR0=20;
	TCNT0=0;
	TCCR0=_BV(WGM01)|_BV(CS01)|_BV(CS00)|_BV(COM00);//CTC,8分频，com1:0=1，匹配时取反
	/* 中断设置 */
	
	TIMSK|=_BV(OCIE0);//T0匹配中断使能
	/* IO口设置 */
	DDRA=0xF0;//高四位输出，低四位输入，高四位用于数据选择器
	DDRA&=~_BV(PA0);//PA0为输入状态
	PORTA=0x00;//init PORTA pins，初始高四位
	DDRB=0x0F;//PB3:0输出
	DDRC&=0xFC;
	ADMUX=0x60;//AVCC，外接滤波电容，选择左对齐，单端输入ADC0,1100000
	ADCSRA|=_BV(ADEN)|_BV(ADPS1)|_BV(ADPS0);////ADC使能，ADC开始转换
	/* SPI设置 */
	SPI_SlaveInit();
	//DDRB|=(1<<PB5)|(1<<PB7);//设置MOSI和SCK为输出，其它为输入
	//SPCR=(1<<SPE)|(1<<MSTR)|(1<<SPR0);//使能SPI主机模式，设置时钟速率为fck/16
	/* 读取阈值 */
	INIT_AD_STATISTIC();
	//开始工作
	SREG|=_BV(7);//全局中断使能
	/* 工作 */
	while(1){
		LightLED();
		if(!(PINC&0x01))
			ADC_H();
		if(!(PINC&0x02))
			ADC_L();
	}
}

void ShiftReg(uint8_t SR){//对移位寄存器进行操作，点灯
	uint8_t i;
	PORTB&=~(_BV(PB1));//清空时钟
	for(i=0;i<8;i++){//将8位的SR从高位到低位一位一位的压入移位寄存器
		if(SR&(_BV(7))){//第i位是1
			PORTB&=~(_BV(PB1));//清空时钟
			PORTB|=_BV(PB2);//DATA=1
			PORTB|=_BV(PB1);//产生上升沿，压入一位数据
		}
		else {//第i位是0
			PORTB&=~(_BV(PB1));//清空时钟
			PORTB&=~(_BV(PB2));//DATA=0
			PORTB|=_BV(PB1);//产生上升沿，压入一位数据
		}
		SR<<=1;//左移1位，获得下一位
	}
}

void LightLED(){
	uint8_t i,j,SR;//i用于行选择，j用于列选择，SR使用二进制选择一行中亮的灯，低四位选择行，高四位选择列
	for(i=0;i<4;i++){//每次循环可以控制4盏灯，利用人眼的延迟产生持续亮灯的效果
		//第i行
		SR=0xF0;//置位&清零
		SR|=_BV(i);//选择第i行
		for(j=0;j<4;j++){//选择第i行中亮的灯
			if(light[i*4+j]==1){
				SR&=~_BV(4+j);//清零标志位，灯亮，标志位为0
				}
			else {}
		}
		//ShiftReg(0);//清零移位寄存器
		ShiftReg(SR);//压入需要输出的灯
		_delay_ms(5);//延迟，产生灯亮
	}
	//ShiftReg(0);
}

ISR(SIG_OUTPUT_COMPARE0,ISR_NOBLOCK)//T0匹配中断
{
	sei();
	if (running == 0) return;
	if(flag==0){
		PORTB&=~_BV(PB0);//PB0口接零，C6充电，下次中断时进行数模转换与C6放电
		//flag=1;
	}
	if (flag == 1) {
		//PORTB&=~_BV(PB0);//PB0口接零，C6充电，下次中断时进行数模转换与C6放电
	}
	else if (flag == 3) {//充电完成，进行模数转换，之后C6放电	
		
		ADCSRA|=_BV(ADSC);////ADC使能，ADC开始转换
		while((ADCSRA&(1<<ADSC))!=0){};//等待转换完毕
		//volatile unsigned int adcval;
		//adcval = ADCH;
		ADC_RESULT=ADCH;
		locate=(LED>>4);
		if(ADC_RESULT>Threshold_H[locate] && light[locate] == 0) {
			light[locate]=1;
			currentoutput |= (1 << locate);
		}
		else if (ADC_RESULT<Threshold_L[locate] && light[locate] == 1) {
			light[locate]=0;
			currentoutput &= ~(1 << locate);
		}
		PORTB|=_BV(PB0);//C6放电
		if(locate==15) OutputData();
		LED+=16;
		PORTA=LED;//向下一个光敏二极管扫描
		//LED = 9<<4;
		if (locate == 9)
			locate = 9;
		
		/* 阈值调整 */
		if(h_adjust){
			AD_TEMP[times_in_adjust][locate]=ADC_RESULT;
			if(locate==15)
				times_in_adjust++;
			if(times_in_adjust==5)
				h_adjust=false;
		}
		else{}
		if(l_adjust){
			AD_TEMP[times_in_adjust][locate]=ADC_RESULT;
			if(locate==15)
				times_in_adjust++;
			if(times_in_adjust==9)
				l_adjust=false;
		}
		else{}
		//flag=0;//进入下一次循环
	}
	++flag;
	if (flag == 4) flag = 0;
}

void INIT_AD_STATISTIC()//初始化阈值
{
	uint8_t i;
	for(i=0;i<16;i++)
	{
		eeprom_busy_wait();//等待读写完成
		Threshold_H[i]=eeprom_read_byte((uint8_t *)(i*2));//从EEPROM中读取阈值数据
		Threshold_L[i]=eeprom_read_byte((uint8_t *)(i*2+1));//从EEPROM中读取阈值数据
	}
}
	
void ADC_H()
{
	cli();
	times_in_adjust=1;
	LED = 0;
	locate = 0;
	flag = 0;
	PORTB|=_BV(PB0);//C6放电
	_delay_ms(4);
	sei();
	h_adjust=true;
	while(h_adjust)//等待调整阈值完成
	{
		LightLED();
	}
	ShiftReg(0x0F);
	_delay_ms(200);
	ShiftReg(0xF0);
	_delay_ms(300);
}

void ADC_L()
{
	cli();
	LED = 0;
	flag = 0;
	PORTB|=_BV(PB0);//C6放电
	_delay_ms(4);
	sei();
	l_adjust=true;
	while(l_adjust){//等待调整阈值完成
		LightLED();
	}
	uint8_t i;
	for(i=0;i<16;i++)
	{
		AD_TEMP[0][i]=(AD_TEMP[1][i]+AD_TEMP[2][i]+AD_TEMP[3][i]+AD_TEMP[4][i])*3/16+(AD_TEMP[5][i]+AD_TEMP[6][i]+AD_TEMP[7][i]+AD_TEMP[8][i])/16;
		eeprom_busy_wait();
		eeprom_write_byte((uint8_t *)(i*2),AD_TEMP[0][i]);
		AD_TEMP[0][i]=(AD_TEMP[1][i]+AD_TEMP[2][i]+AD_TEMP[3][i]+AD_TEMP[4][i])/16+(AD_TEMP[5][i]+AD_TEMP[6][i]+AD_TEMP[7][i]+AD_TEMP[8][i])*3/16;
		eeprom_busy_wait();
		eeprom_write_byte((uint8_t *)(i*2+1),AD_TEMP[0][i]);
	}
	INIT_AD_STATISTIC();//Reload
	ShiftReg(0x0F);
	_delay_ms(200);
	ShiftReg(0xF0);
	_delay_ms(300);
}

void OutputData()
{
	//uint8_t a=0,n=0,MiddleLight;
	//n = 0;
	/*
	for (volatile unsigned char i = 0;i<16;++i){
		if(light[i]==1){
			a+=i;
			++n;
		}
	}
	*/
	//currentoutput=MiddleLight;
	//if(n==0){//如果不在线上，输出20
		//MiddleLight=31;
		//currentoutput=MiddleLight;
		// while(!(SPSR&(1<<SPIF)));
	//}
	//if(n!=0){//如果在线上，输出中间那一盏灯，数值1~16
		//MiddleLight=(a/n) << 1;
		//if (n%2 == 0) MiddleLight += 1;
		//currentoutput=MiddleLight;
		// while(!(SPSR&(1<<SPIF)));
	//}
}
