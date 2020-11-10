#include <reg52.h>
#include "FPM10A.h"
#include "key.h"
#include "uart.h"
#include "lcd_1602.h"
#include "delay.h"
#include "buzz.h"

sbit SG_PWM = P3^7; //舵机脉冲输出端
//变量
unsigned char PWM_count = 20; //pwm高电平时间   初始为1.5ms   舵机转动90°
unsigned int count = 0;		 //延时中断计数

void SG90INIT() //SG90初始化
{

	TMOD = 0x22; //定时器0 方式2
	TH0 = 156;	 //100us定时
	TL0 = 156;
	EA = 1; //中断开启
	ET0 = 1;
	TR0 = 1; //开始计数
}
void SG90shutdown() //SG90关闭
{
	TR0 = 0;
	// EA = 0; //中断关闭

}
void timer0() interrupt 1 //100us 中断
{
	if (count <= PWM_count) //脉冲高电平时间
	{
		SG_PWM = 1;
	}
	else //脉冲时基-高电平=低电平时间
	{
		SG_PWM = 0;
	}
	count++;
}


// sbit SRD = P3 ^ 7; //继电器 
volatile unsigned char FPM10A_RECEICE_BUFFER[32];
unsigned int finger_id = 0;

//统计数据的全局变量
struct Numbers
{
	char name;
	unsigned int times; // 0~65536
	unsigned int fid_min;
	unsigned int fid_max;
} numbers[] = {
	{'F', 0, 0, 19},
	{'Y', 0, 20, 39},
	{'H', 0, 40, 59},
	{'L', 0, 60, 79},
	{'G', 0, 80, 999},
};

//FINGERPRINT通信协议定义

code unsigned char FPM10A_Get_Device[10] = {0x01, 0x00, 0x07, 0x13, 0x00, 0x00, 0x00, 0x00, 0x00, 0x1b};	   //口令验证
code unsigned char FPM10A_Pack_Head[6] = {0xEF, 0x01, 0xFF, 0xFF, 0xFF, 0xFF};								   //协议包头
code unsigned char FPM10A_Get_Img[6] = {0x01, 0x00, 0x03, 0x01, 0x00, 0x05};								   //获得指纹图像
code unsigned char FPM10A_Get_Templete_Count[6] = {0x01, 0x00, 0x03, 0x1D, 0x00, 0x21};						   //获得模版总数
code unsigned char FPM10A_Search[11] = {0x01, 0x00, 0x08, 0x04, 0x01, 0x00, 0x00, 0x03, 0xE7, 0x00, 0xF8};	   //搜索指纹搜索范围0 - 999,使用BUFFER1中的特征码搜索
code unsigned char FPM10A_Search_0_9[11] = {0x01, 0x00, 0x08, 0x04, 0x01, 0x00, 0x00, 0x00, 0x13, 0x00, 0x21}; //搜索0-9号指纹
code unsigned char FPM10A_Img_To_Buffer1[7] = {0x01, 0x00, 0x04, 0x02, 0x01, 0x00, 0x08};					   //将图像放入到BUFFER1
code unsigned char FPM10A_Img_To_Buffer2[7] = {0x01, 0x00, 0x04, 0x02, 0x02, 0x00, 0x09};					   //将图像放入到BUFFER2
code unsigned char FPM10A_Reg_Model[6] = {0x01, 0x00, 0x03, 0x05, 0x00, 0x09};								   //将BUFFER1跟BUFFER2合成特征模版
code unsigned char FPM10A_Delete_All_Model[6] = {0x01, 0x00, 0x03, 0x0d, 0x00, 0x11};						   //删除指纹模块里所有的模版
volatile unsigned char FPM10A_Save_Finger[9] = {0x01, 0x00, 0x06, 0x06, 0x01, 0x00, 0x0B, 0x00, 0x19};		   //将BUFFER1中的特征码存放到指定的位置
//volatile:系统总是重新从它所在的内存读取数据，即使它前面的指令刚刚从该处读取过数据
/*------------------ FINGERPRINT命令字 --------------------------*/
//发送包头
void FPM10A_Cmd_Send_Pack_Head(void)
{
	int i;
	for (i = 0; i < 6; i++) //包头
	{
		Uart_Send_Byte(FPM10A_Pack_Head[i]);
	}
}
//发送指令
void FPM10A_Cmd_Check(void)
{
	int i = 0;
	FPM10A_Cmd_Send_Pack_Head(); //发送通信协议包头
	for (i = 0; i < 10; i++)
	{
		Uart_Send_Byte(FPM10A_Get_Device[i]);
	}
}
//接收反馈数据缓冲
void FPM10A_Receive_Data(unsigned char ucLength)
{
	unsigned char i;

	for (i = 0; i < ucLength; i++)
		FPM10A_RECEICE_BUFFER[i] = Uart_Receive_Byte();
}

//FINGERPRINT_获得指纹图像命令
void FPM10A_Cmd_Get_Img(void)
{
	unsigned char i;
	FPM10A_Cmd_Send_Pack_Head(); //发送通信协议包头
	for (i = 0; i < 6; i++)		 //发送命令 0x1d
	{
		Uart_Send_Byte(FPM10A_Get_Img[i]);
	}
}
//讲图像转换成特征码存放在Buffer1中
void FINGERPRINT_Cmd_Img_To_Buffer1(void)
{
	unsigned char i;
	FPM10A_Cmd_Send_Pack_Head(); //发送通信协议包头
	for (i = 0; i < 7; i++)		 //发送命令 将图像转换成 特征码 存放在 CHAR_buffer1
	{
		Uart_Send_Byte(FPM10A_Img_To_Buffer1[i]);
	}
}
//将图像转换成特征码存放在Buffer2中
void FINGERPRINT_Cmd_Img_To_Buffer2(void)
{
	unsigned char i;
	for (i = 0; i < 6; i++) //发送包头
	{
		Uart_Send_Byte(FPM10A_Pack_Head[i]);
	}
	for (i = 0; i < 7; i++) //发送命令 将图像转换成 特征码 存放在 CHAR_buffer1
	{
		Uart_Send_Byte(FPM10A_Img_To_Buffer2[i]);
	}
}
//搜索全部用户999枚
void FPM10A_Cmd_Search_Finger(void)
{
	unsigned char i;
	FPM10A_Cmd_Send_Pack_Head(); //发送通信协议包头
	for (i = 0; i < 11; i++)
	{
		Uart_Send_Byte(FPM10A_Search[i]);
	}
}

void FPM10A_Cmd_Reg_Model(void)
{
	unsigned char i;

	FPM10A_Cmd_Send_Pack_Head(); //发送通信协议包头

	for (i = 0; i < 6; i++)
	{
		Uart_Send_Byte(FPM10A_Reg_Model[i]);
	}
}
//删除指纹模块里的所有指纹模版
void FINGERPRINT_Cmd_Delete_All_Model(void)
{
	unsigned char i;
	for (i = 0; i < 6; i++) //包头
		Uart_Send_Byte(FPM10A_Pack_Head[i]);
	for (i = 0; i < 6; i++) //命令合并指纹模版
	{
		Uart_Send_Byte(FPM10A_Delete_All_Model[i]);
	}
}
//保存指纹
void FPM10A_Cmd_Save_Finger(unsigned int storeID)
{
	unsigned long temp = 0;
	unsigned char i;
	FPM10A_Save_Finger[5] = (storeID & 0xFF00) >> 8;
	FPM10A_Save_Finger[6] = (storeID & 0x00FF);
	for (i = 0; i < 7; i++) //计算校验和
		temp = temp + FPM10A_Save_Finger[i];
	FPM10A_Save_Finger[7] = (temp & 0x00FF00) >> 8; //存放校验数据
	FPM10A_Save_Finger[8] = temp & 0x0000FF;
	FPM10A_Cmd_Send_Pack_Head(); //发送通信协议包头
	for (i = 0; i < 9; i++)
		Uart_Send_Byte(FPM10A_Save_Finger[i]); //发送命令 将图像转换成 特征码 存放在 CHAR_buffer1
}
//添加指纹
void FPM10A_Add_Fingerprint()
{
	unsigned char id_show[] = {0, 0, 0};
	char iNumber = 0;
	LCD1602_WriteCMD(0x01); //清屏
	finger_id = 0;

	while (1)
	{
		// 选择用户
		LCD1602_Display(0x80, "  Who are you?  ", 0, 16);
		LCD1602_Display(0xc0, " my name is     ", 0, 16);
		//按返回键直接回到主菜单
		if (KEY_CANCEL == 0)
		{
			while (KEY_CANCEL == 0)
				;
			break;
		}
		//按切换键切换名字
		if (KEY_DOWN == 0)
		{
			while (KEY_DOWN == 0)
				;
			if (++iNumber == 5)
			{
				iNumber = 0;
			}
		}

		//name显示处理
		LCD1602_WriteCMD(0xc0 + 12);
		LCD1602_WriteDAT(numbers[iNumber].name);

		//按确认键开始选择指纹id
		if (KEY_OK == 0)
		{
			while (KEY_OK == 0)
				;
			finger_id = numbers[iNumber].fid_min;
			while (1)
			{
				LCD1602_Display(0x80, " : Add  finger  ", 0, 16);
				LCD1602_Display(0xc0, "    ID is       ", 0, 16);
				LCD1602_WriteCMD(0x80);
				LCD1602_WriteDAT(numbers[iNumber].name);
				//按返回键直接回到上一层
				if (KEY_CANCEL == 0)
				{
					while (KEY_CANCEL == 0)
						;
					break;
				}

				//按切换键指纹iD值加1
				if (KEY_DOWN == 0)
				{
					while (KEY_DOWN == 0)
						;
					if (finger_id++ == numbers[iNumber].fid_max)
					{
						finger_id = numbers[iNumber].fid_min;
					}
				}

				//指纹iD值显示处理
				LCD1602_WriteCMD(0xc0 + 10);
				LCD1602_WriteDAT(finger_id / 100 + 48);
				LCD1602_WriteDAT(finger_id % 100 / 10 + 48);
				LCD1602_WriteDAT(finger_id % 100 % 10 + 48);

				//按确认键开始录入指纹信息
				if (KEY_OK == 0)
				{
					while (KEY_OK == 0)
						;
					LCD1602_Display(0x80, " Please  finger ", 0, 16);
					LCD1602_Display(0xc0, "                ", 0, 16);
					while (KEY_CANCEL == 1)
					{
						//按下返回键退出录入返回fingerID调整状态
						if (KEY_CANCEL == 0)
						{
							while (KEY_CANCEL == 0)
								;
							break;
						}
						FPM10A_Cmd_Get_Img(); //获得指纹图像
						FPM10A_Receive_Data(12);
						//判断接收到的确认码,等于0指纹获取成功
						if (FPM10A_RECEICE_BUFFER[9] == 0)
						{
							Delay_Ms(100);
							FINGERPRINT_Cmd_Img_To_Buffer1();
							FPM10A_Receive_Data(12);
							LCD1602_Display(0x80, "Successful entry", 0, 16);
							Buzz_Times(1);
							Delay_Ms(1000);
							LCD1602_Display(0x80, " Please  finger ", 0, 16);
							LCD1602_Display(0xc0, "     again      ", 0, 16);
							while (1)
							{
								if (KEY_CANCEL == 0)
								{
									while (KEY_CANCEL == 0)
										;
									break;
								}
								FPM10A_Cmd_Get_Img(); //获得指纹图像
								FPM10A_Receive_Data(12);
								//判断接收到的确认码,等于0指纹获取成功
								if (FPM10A_RECEICE_BUFFER[9] == 0)
								{
									Delay_Ms(200);
									LCD1602_Display(0x80, "Successful entry", 0, 16);
									LCD1602_Display(0xc0, "    ID is       ", 0, 16);
									//指纹iD值显示处理
									LCD1602_WriteCMD(0xc0 + 10);
									LCD1602_WriteDAT(finger_id / 100 + 48);
									LCD1602_WriteDAT(finger_id % 100 / 10 + 48);
									LCD1602_WriteDAT(finger_id % 100 % 10 + 48);
									FINGERPRINT_Cmd_Img_To_Buffer2();
									FPM10A_Receive_Data(12);
									FPM10A_Cmd_Reg_Model(); //转换成特征码
									FPM10A_Receive_Data(12);
									FPM10A_Cmd_Save_Finger(finger_id);
									FPM10A_Receive_Data(12);
									Buzz_Times(1);
									Delay_Ms(1000);
									if (finger_id++ == numbers[iNumber].fid_max)
									{
										finger_id = numbers[iNumber].fid_min;
									}
									break;
								}
							}

							break;
						}
					}
				}
			}
		}
	}
}

//搜索指纹
void FPM10A_Find_Fingerprint()
{
	unsigned int find_fingerid = 0;
	unsigned char id_show[] = {0, 0, 0};
	char iNumber = 0;
	int i = 0;
	do
	{
		LCD1602_Display(0x80, "fingerprint lock", 0, 16);
		LCD1602_Display(0xc0, "   running ...  ", 0, 16);
		FPM10A_Cmd_Get_Img(); //获得指纹图像
		FPM10A_Receive_Data(12);
		//判断接收到的确认码,等于0指纹获取成功
		if (FPM10A_RECEICE_BUFFER[9] == 0)
		{
			Delay_Ms(100);
			FINGERPRINT_Cmd_Img_To_Buffer1();
			FPM10A_Receive_Data(12);
			FPM10A_Cmd_Search_Finger();
			FPM10A_Receive_Data(16);
			if (FPM10A_RECEICE_BUFFER[9] == 0) //搜索到
			{
				LCD1602_Display(0x80, " Hello ,      ! ", 0, 16);
				LCD1602_Display(0xc0, "    ID is       ", 0, 16);
				Buzz_Times(1);
				//拼接指纹ID数
				find_fingerid = FPM10A_RECEICE_BUFFER[10] * 256 + FPM10A_RECEICE_BUFFER[11];
				//指纹iD值显示处理
				LCD1602_WriteCMD(0xc0 + 10);
				LCD1602_WriteDAT(find_fingerid / 100 + 48);
				LCD1602_WriteDAT(find_fingerid % 100 / 10 + 48);
				LCD1602_WriteDAT(find_fingerid % 100 % 10 + 48);
				//记录统计
				for (iNumber = 0; iNumber <= 4; iNumber++)
				{
					if (numbers[iNumber].fid_min <= find_fingerid && find_fingerid <= numbers[iNumber].fid_max)
					{
						numbers[iNumber].times++;
						break;
					}
				}
				LCD1602_WriteCMD(0x80 + 10);
				LCD1602_WriteDAT(numbers[iNumber].name);

				//SRD输出操作

				SG90INIT(); //舵机驱动
				PWM_count = 5;
				while (i++ < 6000)
				{
					if (count >= 190)
						count = 0; //设置舵机脉冲时基 20ms
				}
				i = 0;
				SG90shutdown();

				Delay_Ms(1000);

				SG90INIT(); //舵机驱动
				PWM_count = 20;
				while (i++ < 6000)
				{
					if (count >= 190)
						count = 0; //设置舵机脉冲时基 20ms
				}
				i = 0;
				SG90shutdown();
				// SRD = 0;
				// Delay_Ms(3000);
				// SRD = 1;
			}
			else //没有找到
			{
				LCD1602_Display(0x80, " Search  failed ", 0, 16);
				LCD1602_Display(0xc0, "                ", 0, 16);
				Buzz_Times(3);
			}
		}
	} while (KEY_CANCEL == 1);
}
//删除所有存贮的指纹库
void FPM10A_Delete_All_Fingerprint()
{
	unsigned char i = 0;
	LCD1602_Display(0x80, "   empty all    ", 0, 16);
	LCD1602_Display(0xc0, "   Yes or no ?  ", 0, 16);
	do
	{
		if (KEY_OK == 0)
		{
			while (KEY_OK == 0)
				;
			LCD1602_Display(0x80, "   emptying     ", 0, 16);
			LCD1602_Display(0xc0, "                ", 0, 16);
			Delay_Ms(300);
			LCD1602_WriteCMD(0xc0);
			for (i = 0; i < 16; i++)
			{
				LCD1602_WriteDAT(42);
				Delay_Ms(100);
			}
			FINGERPRINT_Cmd_Delete_All_Model();
			FPM10A_Receive_Data(12);
			LCD1602_Display(0x80, "   All empty    ", 0, 16);
			LCD1602_Display(0xc0, "                ", 0, 16);
			Buzz_Times(3);
			break;
		}
	} while (KEY_CANCEL == 1);
}

void Device_Check(void)
{
	unsigned char i = 0;
	FPM10A_RECEICE_BUFFER[9] = 1;			//串口数组第九位可判断是否通信正常
	LCD1602_Display(0xc0, "Loading", 0, 7); //设备加载中界面

	LCD1602_Display(0xc0, "Docking  failure", 0, 16); //液晶先显示对接失败，如果指纹模块插对的话会将其覆盖
	FPM10A_Cmd_Check();								  //单片机向指纹模块发送校对命令
	FPM10A_Receive_Data(12);						  //将串口接收到的数据转存
	if (FPM10A_RECEICE_BUFFER[9] == 0)				  //判断数据低第9位是否接收到0
	{
		LCD1602_Display(0xc0, "Docking  success", 0, 16); //符合成功条件则显示对接成功
	}
}

/***********************************************
函数名称：FPM10A_Statistic
功    能：统计界面主函数
************************************************/
void FPM10A_Statistic()
{
	unsigned char i = 0;
	char function_date = 0; //统计功能内箭头位置
	do
	{
		/**************进入统计功能界面****************/
		LCD1602_Display(0x80, " = statistics = ", 0, 16);
		LCD1602_Display(0xc0, "   View   Delete", 0, 16);
		if (function_date == 0)
		{
			LCD1602_Display(0xc0 + 1, "->", 0, 2);
			LCD1602_Display(0xc0 + 8, "  ", 0, 2);
		}
		else if (function_date == 1)
		{
			LCD1602_Display(0xc0 + 1, "  ", 0, 2);
			LCD1602_Display(0xc0 + 8, "->", 0, 2);
		}

		//确认键
		if (KEY_OK == 0)
		{
			while (KEY_OK == 0)
				; //等待松开按键
			switch (function_date)
			{
			case 0: //查看数据
				FPM10A_Statistic_View();
				break;

			case 1: //清除数据
				FPM10A_Statistic_Delete();
				break;
			}
		}
		//切换键
		if (KEY_DOWN == 0)
		{
			while (KEY_DOWN == 0)
				; //等待松开按键
			if (function_date <= 1)
			{
				function_date++;
				if (function_date == 2)
					function_date = 0;
			}
		}
		Delay_Ms(100); //延时判断100MS检测一次
	} while (KEY_CANCEL == 1);
}

/***********************************************
函数名称：FPM10A_Statistic_View
功    能：查看统计操作界面 
0 F		1 Y
2 H		3 L
4 G (guest)
************************************************/
void FPM10A_Statistic_View()
{
	char iNumber = 0;
	unsigned int i = 0;
	LCD1602_Display(0x80, " S=== View ==== ", 0, 16);
	LCD1602_Display(0xc0, "   loading...   ", 0, 16);

	for (iNumber = 0; iNumber <= 4; iNumber++)
	{
		i += numbers[iNumber].times; //注意！ 这里可能溢出
	}
	//                    "All 065536 times"
	LCD1602_Display(0xc0, "All 0      times", 0, 16);
	LCD1602_WriteCMD(0xc0 + 5);
	LCD1602_WriteDAT(i / 10000 + 48);
	LCD1602_WriteDAT(i % 10000 / 1000 + 48);
	LCD1602_WriteDAT(i % 1000 / 100 + 48);
	LCD1602_WriteDAT(i % 100 / 10 + 48);
	LCD1602_WriteDAT(i % 10 + 48);

	iNumber = 0;
	do
	{
		//                    "G is 65536 times"
		LCD1602_Display(0x80, "  is       times", 0, 16);
		LCD1602_WriteCMD(0x80 + 0);
		LCD1602_WriteDAT(numbers[iNumber].name);
		i = numbers[iNumber].times;
		LCD1602_WriteCMD(0x80 + 5);
		LCD1602_WriteDAT(i / 10000 + 48);
		LCD1602_WriteDAT(i % 10000 / 1000 + 48);
		LCD1602_WriteDAT(i % 1000 / 100 + 48);
		LCD1602_WriteDAT(i % 100 / 10 + 48);
		LCD1602_WriteDAT(i % 10 + 48);

		Delay_Ms(1000);
		if (iNumber++ == 4)
		{
			iNumber = 0;
		};

	} while (KEY_CANCEL == 1);
}

/***********************************************
函数名称：FPM10A_Statistic_Delete()
功    能：清除统计操作界面
************************************************/
void FPM10A_Statistic_Delete()
{
	unsigned char i = 0;
	char iNumber = 0;
	LCD1602_Display(0x80, "S=== Delete === ", 0, 16);
	LCD1602_Display(0xc0, "   Yes or no ?  ", 0, 16);
	do
	{
		if (KEY_OK == 0)
		{
			while (KEY_OK == 0)
				;
			LCD1602_Display(0x80, "S=== Delete === ", 0, 16);
			LCD1602_Display(0xc0, "  deleting ...  ", 0, 16);

			for (iNumber = 0; iNumber <= 4; iNumber++)
			{
				numbers[iNumber].times = 0;
			}

			LCD1602_Display(0x80, "S=== Delete === ", 0, 16);
			LCD1602_Display(0xc0, "   All empty    ", 0, 16);
			Buzz_Times(3);
			break;
		}
	} while (KEY_CANCEL == 1);
}