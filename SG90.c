#include<reg52.h>    
sbit P10=P1^0;
sbit P11=P1^1;
unsigned int scale;   //占空比控制变量
void main(void)       
{ unsigned int n;     //延时循环变量
    TMOD=0x02;        //定时器0，工作模式2（0000 0010），8位定时
    TH0=0x06;         //定时250us（12M晶振）
    TL0=0x06;         //预置值
    TR0=1;            //启动定时器0
    ET0=1;            //启动定时器0中断
    EA=1;             //开启总中断
  while(1)   
  { 
        for(n=0;n<50000;n++);    //延时一段时间
        scale++;                 //占空比控制变量加一
        if(scale==10)  scale=0;  //达到循环的效果
  } 
} 

timer0() interrupt 1             //定时器0中断程序 
{ 
    static  unsigned int  tt ;   //tt用来保存当前时间在一个时钟周期的位置
    tt++;                        //每中断一次，即每经过250us，tt的值自加1
    if(tt==80)                   //中断80次定时20ms，
    { 
        tt=0;                    //使tt=0，开始新的周期，达到循环的效果
//        P10=0;                   //点亮LED
 	P37=0
    } 
    if(scale>=tt)                //如果占空比与中断次数相同时，此时输出高电平
//    P10=1;                       //熄灭LED灯
  	P37=1
    /*
    前面的程序是将占空比设置成了10个等级，scale=1时维持高电平的时间
    是1/10个PWM周期（这里是2.5ms），因为scale<=tt的时间段是低电平点亮LED灯的，
    scale>=tt的时间是熄灭LED灯的，通过改变一个周期内高低电平维持的时间长短
    就可以控制LED灯的亮度或者是电机的转速了，这就是PWM的工作原理
    */
} 