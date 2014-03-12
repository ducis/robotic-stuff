#include<avr/io.h>
#include"pid.h"

signed int KpSpeed=0;
signed int KiSpeed=0;
signed int KiSpeedSet=0;
signed int KdSpeed=0;
signed int SpeedSet=0;
signed int ErrorSpeedLimit0=10;
signed int ErrorSpeedLimit1=-10;
signed int ErrorSpeed[2]={0,0};
signed int DeltaErrorSpeed[2]={0,0};
signed int DeltaControlSpeed=0;
signed long int ControlSpeed=0;

void PID_Init()
{
    KpSpeed=9;
	KiSpeedSet=1;
	KiSpeed=1;
	KdSpeed=0;
	SpeedSet=0;
	ErrorSpeed[1]=ErrorSpeed[0]=0;
	DeltaErrorSpeed[1]=DeltaErrorSpeed[0]=0;
	DeltaControlSpeed=0;
	ControlSpeed=0;
}

    
/*void Set_PID_vect(signed int Kp,signed int Ki,signed int Kd,signed int Speed)
{
    KpSpeed=(signed long int)Kp;
	KiSpeedSet=Ki;
	KiSpeed=(signed long int)Ki;
	KdSpeed=(signed long int)Kd;
	SpeedSet=Speed;
	ErrorSpeed[1]=ErrorSpeed[0]=0;
	DeltaErrorSpeed[1]=DeltaErrorSpeed[0]=0;
	DeltaControlSpeed=0;
	ControlSpeed=0;
}*/
void Set_Speed(signed int speed)
{
    SpeedSet=speed;
}
signed long int PIDSpeed(signed int SpeedNow)
{
  //ErrorSpeedLeft[0]=SpeedLeftSet-SpeedLeft/10;
  ErrorSpeed[0]=SpeedSet-SpeedNow;
  //DeltaErrorSpeedLeft[0]=ErrorSpeedLeft[0]-ErrorSpeedLeft[1];
  DeltaErrorSpeed[0]=ErrorSpeed[0]-ErrorSpeed[1];
  //if(ErrorSpeedLeft[0]>ErrorSpeed0) KiSpeedLeft=0;
  //else KiSpeedLeft=KiSpeedLeftSet;
  //if(ErrorSpeed[0]>ErrorSpeedLimit0)KiSpeed=0;
  //else if(ErrorSpeed[0]<ErrorSpeedLimit1)KiSpeed=0;   //这个积分分离有问题，没有取绝对值
  //else KiSpeed=KiSpeedSet;
  //KiSpeed=0;

  //DeltaControlSpeedLeft=(KpSpeedLeft*DeltaErrorSpeedLeft[0]+KiSpeedLeft*ErrorSpeedLeft[0]+KdSpeedLeft*(DeltaErrorSpeedLeft[0]-DeltaErrorSpeedLeft[1]))/100;
  DeltaControlSpeed=KpSpeed*DeltaErrorSpeed[0]+KiSpeed*ErrorSpeed[0]+KdSpeed*(DeltaErrorSpeed[0]-DeltaErrorSpeed[1]);
  //DeltaControlSpeed=KpSpeed*DeltaErrorSpeed[0];
  //ControlSpeedLeft=ControlSpeedLeft+DeltaControlSpeedLeft;
  ControlSpeed=ControlSpeed+DeltaControlSpeed;
  if(ControlSpeed>10000000) ControlSpeed=10000000;       //数据溢出保护
  if(ControlSpeed<-10000000) ControlSpeed=-10000000;

  //ErrorSpeedLeft[1]=ErrorSpeedLeft[0];
  ErrorSpeed[1]=ErrorSpeed[0];
  //DeltaErrorSpeedLeft[1]=DeltaErrorSpeedLeft[0];
  DeltaErrorSpeed[1]=DeltaErrorSpeed[0];
  return ControlSpeed;
  //ErrorSpeed[0]=(signed long int)(SpeedSet-SpeedNow);
  //return KpSpeed*ErrorSpeed[0];
}

/*signed long int PIDSpeed(signed int SpeedNow)
{
  //ErrorSpeedLeft[0]=SpeedLeftSet-SpeedLeft/10;
  ErrorSpeed[0]=SpeedSet-SpeedNow;
  //DeltaErrorSpeedLeft[0]=ErrorSpeedLeft[0]-ErrorSpeedLeft[1];
  DeltaErrorSpeed[0]=ErrorSpeed[0]-ErrorSpeed[1];
  //if(ErrorSpeedLeft[0]>ErrorSpeed0) KiSpeedLeft=0;
  //else KiSpeedLeft=KiSpeedLeftSet;
  //if(ErrorSpeed[0]>ErrorSpeedLimit)KiSpeed=0;   //这个积分分离有问题，没有取绝对值
  //else KiSpeed=KiSpeedSet;

  //DeltaControlSpeedLeft=(KpSpeedLeft*DeltaErrorSpeedLeft[0]+KiSpeedLeft*ErrorSpeedLeft[0]+KdSpeedLeft*(DeltaErrorSpeedLeft[0]-DeltaErrorSpeedLeft[1]))/100;
  DeltaControlSpeed=KpSpeed*DeltaErrorSpeed[0]+KiSpeed*ErrorSpeed[0]+KdSpeed*(DeltaErrorSpeed[0]-DeltaErrorSpeed[1]);
  //ControlSpeedLeft=ControlSpeedLeft+DeltaControlSpeedLeft;
  ControlSpeed=ControlSpeed+DeltaControlSpeed;

  //ErrorSpeedLeft[1]=ErrorSpeedLeft[0];
  ErrorSpeed[1]=ErrorSpeed[0];
  //DeltaErrorSpeedLeft[1]=DeltaErrorSpeedLeft[0];
  DeltaErrorSpeed[1]=DeltaErrorSpeed[0];
  return ControlSpeed;
}*/
    
