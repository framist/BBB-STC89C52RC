#include"key.h"
#include<reg52.h>

void Key_Init(void)
{
    //定义按键输入端口
	KEY_DOWN=1;
	KEY_OK=1;
	KEY_CANCEL=1;
}
