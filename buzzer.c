#include <reg52.h>
#include "FPM10A.h"
#include "key.h"
#include "uart.h"
#include "lcd_1602.h"
#include "delay.h"
#include "buzz.h"
#include "eeprom.h"

#define uchar unsigned char
sbit beepIO=P1^0; //定义蜂鸣器端口为p1^0
uchar m,n; //定义4个八度 每八度12分音律 共48音律


uchar code Music_Guest[][2]= {{0,4}, //定义曲谱数组，前数为音律，后数为音符节拍 ，要换歌改变简谱对应的音律号即可
    {18,12},{22,8},{20,8},{13,8},{0,8},{13,8},{20,8},{22,8},{18,8},
    {0xFF,0xFF}
}; //歌曲结尾标识


void delay(uchar p) //延时函数 无符号字符型变量
{
    uchar i,j; //定义无符号字符型变量J和I
    for(; p>0; p--) // 此处P值即主函数的n值节拍个数
        for(i=181; i>0; i--) //延时181*181个机器周期约35毫秒，即一个1/16节拍
            for(j=181; j>0; j--);
}




void T0_int() interrupt 1
{
    beepIO=!beepIO; //蜂鸣器翻转发声
    TH0=T[m][0];
    TL0=T[m][1];
//音律延时周期次数码表赋给定时寄存器作为计数初始值,每TH0TL0个机器周期触发蜂鸣器端口翻转，演奏出不同音符
}

/*******蜂鸣器初始化buzzer_init*************/
void Buzzer_Init(){
	TMOD&=0xf0;
	TMOD|=0x01;   
    EA=1;
    ET0=1; //开启T0定时16位方式，总中断开启，开启T0外部中断请求
}
/*******开锁声音buzzer_Unlock sound*************/
void Buzzer_UnlockSound(int count){
    int i=0;
	m=48;
	
	
    for(i=0;i<count;i++){
    TR0=1;
    Delay_Ms(150);
    TR0=0;
    }
    
    
	return;
}
/*******访客进入buzzer_Guest sound*************/
void Buzzer_Sound(uchar music[][2]){
		  
		uchar i=0; //定义无符号字符型变量i,初始值为0	
		while(1){
			
		  	m=music[i][0]; //将音律号赋值给m
	        n=music[i][1]; //将节拍号赋值给n
	        if(m==0x00) //如果音律号为0，
	        {
	            TR0=0;
	            delay(n);
	            i++;
	        } //关闭计时器，延迟n拍，将循环数I加1 ，准备读下一个音符
	        else if(m==0xFF) //否则如果音律数为FF
	        {
	            TR0=0;
	            delay(30);
	            i=0;
				return;
	        } //开启节拍延时30个1/16节拍,歌曲停顿2秒,将循环数I置0
	        else if(m==music[i+1][0]) // 否则如果把下一个音律号数给变量m
	        {
	            TR0=1;
	            delay(n);
	            TR0=0;
	            i++;
	        } //定时器0打开延迟n拍，关闭定时器T0，读下一个音符，循环数加1读下一个音律
	        else //音符若不为零
	        {
	            TR0=1;
	            delay(n);
	            i++;
	        }//打开定时器，延时n个1/16拍，循环数I加1 ,准备演奏下一个音符

		}
}
