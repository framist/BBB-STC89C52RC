#include <reg52.h>
#include <intrins.h>
#include "buzz.h"
#include "key.h"
#include "uart.h"
#include "delay.h"
#include "FPM10A.h"
#include "lcd_1602.h"

void main()
{
	LCD1602_Init();									  //��ʼ��Һ��
	LCD1602_Display(0x80, "Fingerprint Test", 0, 16); //Һ��������ʾ����
	Uart_Init();									  //��ʼ������
	Key_Init();										  //��ʼ������
	Delay_Ms(200);									  //��ʱ500MS���ȴ�ָ��ģ�鸴λ
	Device_Check();									  //У��ָ��ģ���Ƿ������ȷ��Һ��������Ӧ����ʾ
	Delay_Ms(1000);									  //�Խӳɹ�����ͣ��һ��ʱ��
	while (1)
	{
		/**************���������ܽ���****************/
		LCD1602_Display(0x80, "  search finger ", 0, 16); //��һ����ʾ����ָ��
		LCD1602_Display(0xc0, "  Add     delete", 0, 16); //��Ӻ�ɾ��ָ��
		if (local_date == 0)
		{
			LCD1602_Display(0x80, " *", 0, 2);
			LCD1602_Display(0xc0, "  ", 0, 2);
			LCD1602_Display(0xc0 + 8, "  ", 0, 2);
		}
		else if (local_date == 1)
		{
			LCD1602_Display(0x80, "  ", 0, 2);
			LCD1602_Display(0xc0, " *", 0, 2);
			LCD1602_Display(0xc0 + 8, "  ", 0, 2);
		}
		else if (local_date == 2)
		{
			LCD1602_Display(0x80, "  ", 0, 2);
			LCD1602_Display(0xc0, "  ", 0, 2);
			LCD1602_Display(0xc0 + 8, " *", 0, 2);
		}
		//ȷ�ϼ�
		if (KEY_OK == 0)
		{
			while (KEY_OK == 0)
				; //�ȴ��ɿ�����
			switch (local_date)
			{
			case 0: //����ָ��
				FPM10A_Find_Fingerprint();
				break;

			case 1: //���ָ��
				FPM10A_Add_Fingerprint();
				break;

			case 2: //���ָ��
				FPM10A_Delete_All_Fingerprint();
				break;
			}
		}
		//�л���
		if (KEY_DOWN == 0)
		{
			while (KEY_DOWN == 0)
				; //�ȴ��ɿ�����
			if (local_date <= 2)
			{
				local_date++;
				if (local_date == 3)
					local_date = 0;
			}
		}
		Delay_Ms(100); //��ʱ�ж�100MS���һ��
	}
}
