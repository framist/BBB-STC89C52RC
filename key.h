#ifndef _KEY_H_
#define _KEY_H_
#include<reg52.h>

//定义按键输入端口
sbit KEY_DOWN=P1^4;
sbit KEY_OK=P1^5;
sbit KEY_CANCEL=P1^6;

void Key_Init(void);

#endif
