#include <reg52.h>
#include "FPM10A.h"
#include "key.h"
#include "uart.h"
#include "lcd_1602.h"
#include "delay.h"
#include "buzz.h"


sbit SRD = P3^7;//�̵���
volatile unsigned char FPM10A_RECEICE_BUFFER[32];
unsigned int finger_id = 0;

//FINGERPRINTͨ��Э�鶨��

code unsigned char FPM10A_Get_Device[10] ={0x01,0x00,0x07,0x13,0x00,0x00,0x00,0x00,0x00,0x1b};//������֤
code unsigned char FPM10A_Pack_Head[6] = {0xEF,0x01,0xFF,0xFF,0xFF,0xFF};  //Э���ͷ
code unsigned char FPM10A_Get_Img[6] = {0x01,0x00,0x03,0x01,0x00,0x05};    //���ָ��ͼ��
code unsigned char FPM10A_Get_Templete_Count[6] ={0x01,0x00,0x03,0x1D,0x00,0x21 }; //���ģ������
code unsigned char FPM10A_Search[11]={0x01,0x00,0x08,0x04,0x01,0x00,0x00,0x03,0xE7,0x00,0xF8}; //����ָ��������Χ0 - 999,ʹ��BUFFER1�е�����������
code unsigned char FPM10A_Search_0_9[11]={0x01,0x00,0x08,0x04,0x01,0x00,0x00,0x00,0x13,0x00,0x21}; //����0-9��ָ��
code unsigned char FPM10A_Img_To_Buffer1[7]={0x01,0x00,0x04,0x02,0x01,0x00,0x08}; //��ͼ����뵽BUFFER1
code unsigned char FPM10A_Img_To_Buffer2[7]={0x01,0x00,0x04,0x02,0x02,0x00,0x09}; //��ͼ����뵽BUFFER2
code unsigned char FPM10A_Reg_Model[6]={0x01,0x00,0x03,0x05,0x00,0x09}; //��BUFFER1��BUFFER2�ϳ�����ģ��
code unsigned char FPM10A_Delete_All_Model[6]={0x01,0x00,0x03,0x0d,0x00,0x11};//ɾ��ָ��ģ�������е�ģ��
volatile unsigned char  FPM10A_Save_Finger[9]={0x01,0x00,0x06,0x06,0x01,0x00,0x0B,0x00,0x19};//��BUFFER1�е��������ŵ�ָ����λ��
//volatile:ϵͳ�������´������ڵ��ڴ��ȡ���ݣ���ʹ��ǰ���ָ��ոմӸô���ȡ������
/*------------------ FINGERPRINT������ --------------------------*/
 //���Ͱ�ͷ
void FPM10A_Cmd_Send_Pack_Head(void)
{
	int i;	
	for(i=0;i<6;i++) //��ͷ
   {
     Uart_Send_Byte(FPM10A_Pack_Head[i]);   
    }		
}
//����ָ��
void FPM10A_Cmd_Check(void)
{
	int i=0;
	FPM10A_Cmd_Send_Pack_Head(); //����ͨ��Э���ͷ
	for(i=0;i<10;i++)
	 {		
		Uart_Send_Byte(FPM10A_Get_Device[i]);
	  }
}
//���շ������ݻ���
void FPM10A_Receive_Data(unsigned char ucLength)
{
  unsigned char i;

  for (i=0;i<ucLength;i++)
     FPM10A_RECEICE_BUFFER[i] = Uart_Receive_Byte();

}

//FINGERPRINT_���ָ��ͼ������
void FPM10A_Cmd_Get_Img(void)
{
    unsigned char i;
    FPM10A_Cmd_Send_Pack_Head(); //����ͨ��Э���ͷ
    for(i=0;i<6;i++) //�������� 0x1d
	{
       Uart_Send_Byte(FPM10A_Get_Img[i]);
	}
}
//��ͼ��ת��������������Buffer1��
void FINGERPRINT_Cmd_Img_To_Buffer1(void)
{
 	unsigned char i;
	FPM10A_Cmd_Send_Pack_Head(); //����ͨ��Э���ͷ      
   	for(i=0;i<7;i++)   //�������� ��ͼ��ת���� ������ ����� CHAR_buffer1
     {
      Uart_Send_Byte(FPM10A_Img_To_Buffer1[i]);
   	  }
}
//��ͼ��ת��������������Buffer2��
void FINGERPRINT_Cmd_Img_To_Buffer2(void)
{
     unsigned char i;
     for(i=0;i<6;i++)    //���Ͱ�ͷ
	 {
    	Uart_Send_Byte(FPM10A_Pack_Head[i]);   
   	 }
     for(i=0;i<7;i++)   //�������� ��ͼ��ת���� ������ ����� CHAR_buffer1
      {
      	Uart_Send_Byte(FPM10A_Img_To_Buffer2[i]);
   	  }
}
//����ȫ���û�999ö
void FPM10A_Cmd_Search_Finger(void)
{
       unsigned char i;	   	    
			 FPM10A_Cmd_Send_Pack_Head(); //����ͨ��Э���ͷ
       for(i=0;i<11;i++)
           {
    	      Uart_Send_Byte(FPM10A_Search[i]);   
   		   }
}

void FPM10A_Cmd_Reg_Model(void)
{
       unsigned char i;	   
	    
			 FPM10A_Cmd_Send_Pack_Head(); //����ͨ��Э���ͷ

       for(i=0;i<6;i++)
           {
    	      Uart_Send_Byte(FPM10A_Reg_Model[i]);   
   		   }


}
//ɾ��ָ��ģ���������ָ��ģ��
void FINGERPRINT_Cmd_Delete_All_Model(void)
{
     unsigned char i;    
    for(i=0;i<6;i++) //��ͷ
      Uart_Send_Byte(FPM10A_Pack_Head[i]);   
    for(i=0;i<6;i++) //����ϲ�ָ��ģ��
	   {
      Uart_Send_Byte(FPM10A_Delete_All_Model[i]);   
		 }	
}
//����ָ��
void FPM10A_Cmd_Save_Finger( unsigned int storeID )
{
       unsigned long temp = 0;
		   unsigned char i;
       FPM10A_Save_Finger[5] =(storeID&0xFF00)>>8;
       FPM10A_Save_Finger[6] = (storeID&0x00FF);
		   for(i=0;i<7;i++)   //����У���
		   	   temp = temp + FPM10A_Save_Finger[i]; 
		   FPM10A_Save_Finger[7]=(temp & 0x00FF00) >> 8; //���У������
		   FPM10A_Save_Finger[8]= temp & 0x0000FF;		   
       FPM10A_Cmd_Send_Pack_Head(); //����ͨ��Э���ͷ	
       for(i=0;i<9;i++)  
      		Uart_Send_Byte(FPM10A_Save_Finger[i]);      //�������� ��ͼ��ת���� ������ ����� CHAR_buffer1
}
//���ָ��
void FPM10A_Add_Fingerprint()
{
	unsigned char id_show[]={0,0,0};
	LCD1602_WriteCMD(0x01); //����
	finger_id=0;  
	while(1)
	{
	LCD1602_Display(0x80,"   Add  finger  ",0,16);
	LCD1602_Display(0xc0,"    ID is       ",0,16);
	//�����ؼ�ֱ�ӻص����˵�
	if(KEY_CANCEL == 0) 
	{
		 while(KEY_CANCEL==0);
		 break;
	}

	//���л���ָ��iDֵ��1
	if(KEY_DOWN == 0)
	{
		while(KEY_DOWN==0);
		if(finger_id == 1000)
		{
			finger_id = 0;
		}
		else
		finger_id = finger_id + 1;
	}

	 //ָ��iDֵ��ʾ���� 
	 LCD1602_WriteCMD(0xc0+10);
	 LCD1602_WriteDAT(finger_id/100+48);
	 LCD1602_WriteDAT(finger_id%100/10+48);
	 LCD1602_WriteDAT(finger_id%100%10+48);

	 //��ȷ�ϼ���ʼ¼��ָ����Ϣ 		 			
	 if(KEY_OK == 0)
	  {	
			  while(KEY_OK==0);
			  LCD1602_Display(0x80," Please  finger ",0,16);
			  LCD1602_Display(0xc0,"                ",0,16);
			while(KEY_CANCEL == 1)
		   {
			  //���·��ؼ��˳�¼�뷵��fingerID����״̬   
				if(KEY_CANCEL == 0) 
				 {
				  while(KEY_CANCEL==0);
				  break;
				  }
				FPM10A_Cmd_Get_Img(); //���ָ��ͼ��
				FPM10A_Receive_Data(12);
				//�жϽ��յ���ȷ����,����0ָ�ƻ�ȡ�ɹ�
				if(FPM10A_RECEICE_BUFFER[9]==0)
				 {
					Delay_Ms(100);
					FINGERPRINT_Cmd_Img_To_Buffer1();
				    FPM10A_Receive_Data(12);
					LCD1602_Display(0x80,"Successful entry",0,16);
					Buzz_Times(1);
					Delay_Ms(1000);
					LCD1602_Display(0x80," Please  finger ",0,16);
			  		LCD1602_Display(0xc0,"                ",0,16);
					while(1)
					{
						if(KEY_CANCEL == 0) 
						{
							while(KEY_CANCEL==0);
							break;
						}
					 FPM10A_Cmd_Get_Img(); //���ָ��ͼ��
					 FPM10A_Receive_Data(12);
					//�жϽ��յ���ȷ����,����0ָ�ƻ�ȡ�ɹ�
					if(FPM10A_RECEICE_BUFFER[9]==0)
					{
						Delay_Ms(200);
						LCD1602_Display(0x80,"Successful entry",0,16);
						LCD1602_Display(0xc0,"    ID is       ",0,16);
						 //ָ��iDֵ��ʾ���� 
						 LCD1602_WriteCMD(0xc0+10);
						 LCD1602_WriteDAT(finger_id/100+48);
						 LCD1602_WriteDAT(finger_id%100/10+48);
						 LCD1602_WriteDAT(finger_id%100%10+48);
						FINGERPRINT_Cmd_Img_To_Buffer2();
				  		FPM10A_Receive_Data(12);
						FPM10A_Cmd_Reg_Model();//ת����������
	         			FPM10A_Receive_Data(12); 
					  	FPM10A_Cmd_Save_Finger(finger_id);                		         
	          			FPM10A_Receive_Data(12);
						Buzz_Times(1);
						Delay_Ms(1000);
						finger_id=finger_id+1;
				    	break;
				  	}
				   }
				   
	        		break;
					}
				}
//			ClrScreen(); //�����ʾ��
			}
		//	Delay_Ms(500);
		}
}

//����ָ��
void FPM10A_Find_Fingerprint()
{
	unsigned int find_fingerid = 0;
	unsigned char id_show[]={0,0,0};
	do
	{
		LCD1602_Display(0x80," Please  finger ",0,16);
		LCD1602_Display(0xc0,"                ",0,16);
		FPM10A_Cmd_Get_Img(); //���ָ��ͼ��
		FPM10A_Receive_Data(12);		
		//�жϽ��յ���ȷ����,����0ָ�ƻ�ȡ�ɹ�
		if(FPM10A_RECEICE_BUFFER[9]==0)
		{			
			Delay_Ms(100);
			FINGERPRINT_Cmd_Img_To_Buffer1();
			FPM10A_Receive_Data(12);		
			FPM10A_Cmd_Search_Finger();
			FPM10A_Receive_Data(16);			
			if(FPM10A_RECEICE_BUFFER[9] == 0) //������  
			{
				 LCD1602_Display(0x80," Search success ",0,16);
				 LCD1602_Display(0xc0,"    ID is       ",0,16);
				 Buzz_Times(1);					
				//ƴ��ָ��ID��
				 find_fingerid = FPM10A_RECEICE_BUFFER[10]*256 + FPM10A_RECEICE_BUFFER[11];					
				 //ָ��iDֵ��ʾ���� 
				 LCD1602_WriteCMD(0xc0+10);
				 LCD1602_WriteDAT(find_fingerid/100+48);
				 LCD1602_WriteDAT(find_fingerid%100/10+48);
				 LCD1602_WriteDAT(find_fingerid%100%10+48);	
				 SRD = 0;
				 Delay_Ms(3000);	
				 SRD = 1;
			  }
				else //û���ҵ�
				{
					LCD1602_Display(0x80," Search  failed ",0,16);
					LCD1602_Display(0xc0,"                ",0,16);
				 	Buzz_Times(3);
				}
			}		
		}while(KEY_CANCEL == 1);
}
//ɾ�����д�����ָ�ƿ�
void FPM10A_Delete_All_Fingerprint()
{
		unsigned char i=0;
				LCD1602_Display(0x80,"   empty all    ",0,16);
				LCD1602_Display(0xc0,"   Yes or no ?  ",0,16); 
		do
		 {
			if(KEY_OK==0)
			{
				while(KEY_OK==0);
				LCD1602_Display(0x80,"   emptying     ",0,16);
				LCD1602_Display(0xc0,"                ",0,16); 
				Delay_Ms(300);
				LCD1602_WriteCMD(0xc0);
				for(i=0;i<16;i++)
				 {
					LCD1602_WriteDAT(42);
					Delay_Ms(100);
				 }
				FINGERPRINT_Cmd_Delete_All_Model();
			  FPM10A_Receive_Data(12); 
				LCD1602_Display(0x80,"   All empty    ",0,16);
				LCD1602_Display(0xc0,"                ",0,16);
				Buzz_Times(3);
				break;
			}
		 }while(KEY_CANCEL==1);
}

void Device_Check(void)
{
		unsigned char i=0;
		FPM10A_RECEICE_BUFFER[9]=1;				           //��������ھ�λ���ж��Ƿ�ͨ������
		LCD1602_Display(0xc0,"Loading",0,7);	           //�豸�����н���							   
		for(i=0;i<8;i++)						           //������ʽ���£�����������
		{
			LCD1602_WriteDAT(42);	                       //42��ӦASIC��� *
			Delay_Ms(200);						           //���ƽ������ٶ�
		}									
		LCD1602_Display(0xc0,"Docking  failure",0,16);      //Һ������ʾ�Խ�ʧ�ܣ����ָ��ģ���ԵĻ��Ὣ�串��	
		FPM10A_Cmd_Check();									//��Ƭ����ָ��ģ�鷢��У������
		FPM10A_Receive_Data(12);							//�����ڽ��յ�������ת��
 		if(FPM10A_RECEICE_BUFFER[9] == 0)					//�ж����ݵ͵�9λ�Ƿ���յ�0
		{
			LCD1602_Display(0xc0,"Docking  success",0,16);	//���ϳɹ���������ʾ�Խӳɹ�
		}
}





