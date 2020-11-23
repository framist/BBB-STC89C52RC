#include <reg52.h>
#include <intrins.h>
#include "buzz.h"
#include "key.h"
#include "uart.h"
#include "delay.h"
#include "FPM10A.h"
#include "lcd_1602.h"
#include "eeprom.h"
void main()
{
	LCD1602_Init();									  //初始化液晶
	LCD1602_Display(0x80, "Drivers Checking", 0, 16); //液晶开机显示界面
	Uart_Init();									  //初始化串口
	Key_Init();										  //初始化按键
	IOT_OPEN = 1;
	Delay_Ms(200);									  //延时500MS，等待指纹模块复位
	Device_Check();									  //校对指纹模块是否接入正确，液晶做出相应的提示
	dataLoad();										  //数据载入
	Delay_Ms(1000);									  //对接成功界面停留一定时间
	while (1)
	{
		/**************进入主功能界面****************/
		LCD1602_Display(0x80, "  RUN     statis", 0, 16); //第一排显示搜索指纹
		LCD1602_Display(0xc0, "  add     delete", 0, 16); //添加和删除指纹
		if (local_date == 0)
		{
			LCD1602_Display(0x80, "=>", 0, 2);
			LCD1602_Display(0xc0, "  ", 0, 2);
			LCD1602_Display(0xc0 + 8, "  ", 0, 2);
			LCD1602_Display(0x80 + 8, "  ", 0, 2);
		}
		else if (local_date == 1)
		{
			LCD1602_Display(0x80, "  ", 0, 2);
			LCD1602_Display(0xc0, "=>", 0, 2);
			LCD1602_Display(0xc0 + 8, "  ", 0, 2);
			LCD1602_Display(0x80 + 8, "  ", 0, 2);
		}
		else if (local_date == 2)
		{
			LCD1602_Display(0x80, "  ", 0, 2);
			LCD1602_Display(0xc0, "  ", 0, 2);
			LCD1602_Display(0xc0 + 8, "=>", 0, 2);
			LCD1602_Display(0x80 + 8, "  ", 0, 2);
		}
		else if (local_date == 3)
		{
			LCD1602_Display(0x80, "  ", 0, 2);
			LCD1602_Display(0xc0, "  ", 0, 2);
			LCD1602_Display(0xc0 + 8, "  ", 0, 2);
			LCD1602_Display(0x80 + 8, "=>", 0, 2);
		}
		//确认键
		if (KEY_OK == 0)
		{
			while (KEY_OK == 0)
				; //等待松开按键
			switch (local_date)
			{
			case 0: //搜索指纹
				FPM10A_Find_Fingerprint();
				break;

			case 1: //添加指纹
				FPM10A_Add_Fingerprint();
				break;

			case 2: //清空指纹
				FPM10A_Delete_All_Fingerprint();
				break;

			case 3: //统计功能
				FPM10A_Statistic();
				break;
			}
		}
		//切换键
		if (KEY_DOWN == 0)
		{
			while (KEY_DOWN == 0)
				; //等待松开按键
			if (local_date <= 3)
			{
				local_date++;
				if (local_date == 4)
					local_date = 0;
			}
		}
		Delay_Ms(100); //延时判断100MS检测一次
	}
}
