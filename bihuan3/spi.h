#ifndef SPI_H
#define SPI_H



#define NUMOFRIG 2    //寄存器数量，最多为16
#define NUMREC0 2    //寄存器0需要发送或接受数据帧的个数，最大为6
#define NUMREC1 2
#define NUMREC2 0
#define NUMREC3 0
#define NUMREC4 0
#define NUMREC5 0
#define NUMREC6 0
#define NUMREC7 0
#define NUMREC8 0
#define NUMREC9 0
#define NUMREC10 0
#define NUMREC11 0
#define NUMREC12 0
#define NUMREC13 0
#define NUMREC14 0
#define NUMREC15 0
#define WRITELIMIT 0b0000000000000010


/*#define TESTFRAME1 1
#define TESTFRAME2 2
#define TESTFRAME3 3
#define TESTFRAME4 4
#define TESTFRAME5 5
#define TESTFRAME6 6
#define TESTFRAME7 7
#define TESTFRAME8 8
#define TESTFRAME9 9 
#define TESTFRAME10 10
#define TESTFRAME11 11
#define TESTFRAME12 12
#define TESTFRAME13 13
#define TESTFRAME14 14
#define TESTFRAME15 15
#define TESTFRAME16 16
#define TESTFRAME17 17
#define TESTFRAME18 18
#define TESTFRAME19 19
#define TESTFRAME20 20
#define TESTFRAME21 21
#define TESTFRAME22 22*/

#define FIRSTFRAME 0b01000000
#define READYFRAME 0b01100000
#define ERRORFRAME 0b10100000
#define ENDFRAME 0b11100000
#define WRONGCALL 0b10000000

#define UPPERBIT 0b11100000
#define LOWERBIT 0b00011111
#define DIRECTBIT 0b00000001

#define STARTQUEUE 0b00000000
#define FINISHQUEUE 0b11100000



void SPI_SlaveInit();
void State1(); 
void State2(); 
void State3();
void State4(); 
void State5(); 
void State6(); 
void State7(); 
void State8(); 
void Test9();   //测试函数
void HitError(); 

//unsigned long int DataSend(unsigned int registernum);

#endif
