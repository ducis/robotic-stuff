/**************************************************************************
ȡ����ʽ��
	�Ƹ���������S1���õ�LED����ʱ��ȡ�������
	�Ƹ��𴦣���S2���õ��������ȡ�������

SPIΪ����ģʽ��ʱ������Ϊfck/16
	����������ϣ�������ߣ���ֵΪ1~16
	����Ʋ������ϣ����20
	
	
????���з��ͽ����Ƿ���ָ�ӻ����յ���


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
volatile uint8_t light[16];//������Ҫ����F��
volatile uint8_t Threshold_H[16];//ADת����ֵ
volatile uint8_t Threshold_L[16];
volatile uint16_t AD_TEMP[9][16];//������ֵʱʹ�õ���ʱ�洢�ռ�
volatile uint8_t LED=0;//ѡ����Ҫɨ��Ĺ���������,ÿ�μ���10000B��16��
volatile uint8_t flag=0;//�жϱ�־
volatile uint8_t timeflag=0;
volatile uint16_t ADC_RESULT;//����ADCת��������Ҷ���
volatile uint8_t locate;//ADת��ѡ��Ķ�����0~15
volatile uint8_t times_in_adjust;//������ֵʱ�Ĵ���0~4
volatile uint8_t h_adjust=false;//����ֵ�����ж�
volatile uint8_t l_adjust=false;//����ֵ�����ж�
volatile unsigned char running = 1;
volatile unsigned int currentoutput = 0;

void ShiftReg(uint8_t SR);//ʹ����λ�Ĵ������
void LightLED();//����light[16]��ֵ���
void INIT_AD_STATISTIC( );//��ʼ����ֵ
void ADC_H();
void ADC_L();
void OutputData();
//SIGNAL(SIG_OUTPUT_COMPARE0);//T0ƥ���ж�

int main()
{
	/* ����12kHz PWM�� */
	OCR0=20;
	TCNT0=0;
	TCCR0=_BV(WGM01)|_BV(CS01)|_BV(CS00)|_BV(COM00);//CTC,8��Ƶ��com1:0=1��ƥ��ʱȡ��
	/* �ж����� */
	
	TIMSK|=_BV(OCIE0);//T0ƥ���ж�ʹ��
	/* IO������ */
	DDRA=0xF0;//����λ���������λ���룬����λ��������ѡ����
	DDRA&=~_BV(PA0);//PA0Ϊ����״̬
	PORTA=0x00;//init PORTA pins����ʼ����λ
	DDRB=0x0F;//PB3:0���
	DDRC&=0xFC;
	ADMUX=0x60;//AVCC������˲����ݣ�ѡ������룬��������ADC0,1100000
	ADCSRA|=_BV(ADEN)|_BV(ADPS1)|_BV(ADPS0);////ADCʹ�ܣ�ADC��ʼת��
	/* SPI���� */
	SPI_SlaveInit();
	//DDRB|=(1<<PB5)|(1<<PB7);//����MOSI��SCKΪ���������Ϊ����
	//SPCR=(1<<SPE)|(1<<MSTR)|(1<<SPR0);//ʹ��SPI����ģʽ������ʱ������Ϊfck/16
	/* ��ȡ��ֵ */
	INIT_AD_STATISTIC();
	//��ʼ����
	SREG|=_BV(7);//ȫ���ж�ʹ��
	/* ���� */
	while(1){
		LightLED();
		if(!(PINC&0x01))
			ADC_H();
		if(!(PINC&0x02))
			ADC_L();
	}
}

void ShiftReg(uint8_t SR){//����λ�Ĵ������в��������
	uint8_t i;
	PORTB&=~(_BV(PB1));//���ʱ��
	for(i=0;i<8;i++){//��8λ��SR�Ӹ�λ����λһλһλ��ѹ����λ�Ĵ���
		if(SR&(_BV(7))){//��iλ��1
			PORTB&=~(_BV(PB1));//���ʱ��
			PORTB|=_BV(PB2);//DATA=1
			PORTB|=_BV(PB1);//���������أ�ѹ��һλ����
		}
		else {//��iλ��0
			PORTB&=~(_BV(PB1));//���ʱ��
			PORTB&=~(_BV(PB2));//DATA=0
			PORTB|=_BV(PB1);//���������أ�ѹ��һλ����
		}
		SR<<=1;//����1λ�������һλ
	}
}

void LightLED(){
	uint8_t i,j,SR;//i������ѡ��j������ѡ��SRʹ�ö�����ѡ��һ�������ĵƣ�����λѡ���У�����λѡ����
	for(i=0;i<4;i++){//ÿ��ѭ�����Կ���4յ�ƣ��������۵��ӳٲ����������Ƶ�Ч��
		//��i��
		SR=0xF0;//��λ&����
		SR|=_BV(i);//ѡ���i��
		for(j=0;j<4;j++){//ѡ���i�������ĵ�
			if(light[i*4+j]==1){
				SR&=~_BV(4+j);//�����־λ����������־λΪ0
				}
			else {}
		}
		//ShiftReg(0);//������λ�Ĵ���
		ShiftReg(SR);//ѹ����Ҫ����ĵ�
		_delay_ms(5);//�ӳ٣���������
	}
	//ShiftReg(0);
}

ISR(SIG_OUTPUT_COMPARE0,ISR_NOBLOCK)//T0ƥ���ж�
{
	sei();
	if (running == 0) return;
	if(flag==0){
		PORTB&=~_BV(PB0);//PB0�ڽ��㣬C6��磬�´��ж�ʱ������ģת����C6�ŵ�
		//flag=1;
	}
	if (flag == 1) {
		//PORTB&=~_BV(PB0);//PB0�ڽ��㣬C6��磬�´��ж�ʱ������ģת����C6�ŵ�
	}
	else if (flag == 3) {//�����ɣ�����ģ��ת����֮��C6�ŵ�	
		
		ADCSRA|=_BV(ADSC);////ADCʹ�ܣ�ADC��ʼת��
		while((ADCSRA&(1<<ADSC))!=0){};//�ȴ�ת�����
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
		PORTB|=_BV(PB0);//C6�ŵ�
		if(locate==15) OutputData();
		LED+=16;
		PORTA=LED;//����һ������������ɨ��
		//LED = 9<<4;
		if (locate == 9)
			locate = 9;
		
		/* ��ֵ���� */
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
		//flag=0;//������һ��ѭ��
	}
	++flag;
	if (flag == 4) flag = 0;
}

void INIT_AD_STATISTIC()//��ʼ����ֵ
{
	uint8_t i;
	for(i=0;i<16;i++)
	{
		eeprom_busy_wait();//�ȴ���д���
		Threshold_H[i]=eeprom_read_byte((uint8_t *)(i*2));//��EEPROM�ж�ȡ��ֵ����
		Threshold_L[i]=eeprom_read_byte((uint8_t *)(i*2+1));//��EEPROM�ж�ȡ��ֵ����
	}
}
	
void ADC_H()
{
	cli();
	times_in_adjust=1;
	LED = 0;
	locate = 0;
	flag = 0;
	PORTB|=_BV(PB0);//C6�ŵ�
	_delay_ms(4);
	sei();
	h_adjust=true;
	while(h_adjust)//�ȴ�������ֵ���
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
	PORTB|=_BV(PB0);//C6�ŵ�
	_delay_ms(4);
	sei();
	l_adjust=true;
	while(l_adjust){//�ȴ�������ֵ���
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
	//if(n==0){//����������ϣ����20
		//MiddleLight=31;
		//currentoutput=MiddleLight;
		// while(!(SPSR&(1<<SPIF)));
	//}
	//if(n!=0){//��������ϣ�����м���һյ�ƣ���ֵ1~16
		//MiddleLight=(a/n) << 1;
		//if (n%2 == 0) MiddleLight += 1;
		//currentoutput=MiddleLight;
		// while(!(SPSR&(1<<SPIF)));
	//}
}
