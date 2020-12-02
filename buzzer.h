#ifndef __BUZZER_H__
#define __BUZZER_H__

#define uchar unsigned char
#include<reg52.h>

extern char IF_MUSIC_INTERRUPT;
extern uchar m,n;
extern uchar code T[49][2];
extern uchar code music[][2];
extern uchar code Music_Guest[][2];
extern uchar code Music_Guest_short[][2];

sbit beepIO=P1^0; //定义蜂鸣器端口为p1^0



void T0_int();

/*******延时函数*************/
void delay(uchar p);

/*******蜂鸣器初始化buzzer_init*************/
void Buzzer_Init();
/*******开锁声音buzzer_Unlock sound*************/
void Buzzer_UnlockSound(int count);
/*******访客进入buzzer_Guest sound*************/
void Buzzer_Play_Music(uchar music[][2]);



#endif