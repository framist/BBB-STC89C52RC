#ifndef __BUZZER_H__
#define __BUZZER_H__

#define uchar unsigned char
#include<reg52.h>

extern char IF_MUSIC_INTERRUPT;
sbit beepIO=P1^0; //定义蜂鸣器端口为p1^0



void T0_int();

/*******延时函数*************/
void delay(uchar p);

/*******蜂鸣器初始化buzzer_init*************/
void Buzzer_Init();
/*******开锁声音buzzer_Unlock sound*************/
void Buzzer_UnlockSound(int count);
/*******访客进入buzzer_Guest sound*************/
void Buzzer_Sound(uchar music[][2]);

void Buzzer_Sound_Music_Guest();

#endif