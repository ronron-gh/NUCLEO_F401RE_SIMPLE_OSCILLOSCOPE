/********************************************************************
 * MSP2807のサンプルプログラム(lcd.c, GUI.c)の改良版。
 * 描画を高速化するために、画面全体(320x240)のRGBデータを一旦RAM上で作成してから、
 * DMAでSPI送信データレジスタに転送する方式とした。
 ********************************************************************/

#include "stm32f4xx_hal.h"
#include "stdio.h"
#include "string.h"
#include "type.h"
#include "lcd.h"
#include "lcd2.h"

#define LCD_BUF_SIZE	(320 * 240)
#define LCD_SPLIT_SEND_NUM	(10)
#define LCD_SEND_SIZE	(LCD_BUF_SIZE / LCD_SPLIT_SEND_NUM)

s8 g_lcd_buf[LCD_BUF_SIZE];
u16 g_lcd_send_buf[LCD_SEND_SIZE];

extern const unsigned char asc2_1206[95][12];	// 実体は"font.h"
extern const unsigned char asc2_1608[95][16];	// 実体は"font.h"

// カラーテーブル（予めエンディアンをスワップしておく）
u16 g_color_table[] = {
		0xFFFF,		// 0: WHITE
		0x0000,		// 1: BLACK
		0x1F00,		// 2: BLUE
		0x1FF8,		// 3: BRED
		0xE0FF,		// 4: GRED
		0xFF07,		// 5: GBLUE
		0x00F8,		// 6: RED
		0x1FF8,		// 7: MAGENTA
		0xE007,		// 8: GREEN
		0xFF7F,		// 9: CYAN
		0xE0FF,		// 10: YELLOW
		0x40BC,		// 11: BROWN
		0x07FC,		// 12: BRRED
		0x3084,		// 13: GRAY
		0xCF01,		// 14: DARKBLUE
		0x7C7D,		// 15: LIGHTBLUE
		0x5854,		// 16: GRAYBLUE
		0x1F84,		// 17: LIGHTGREEN
		0x5BEF,		// 18: LIGHTGRAY
		0x18C6,		// 19: LGRAY
		0x51A6,		// 20: LGRAYBLUE
		0x122B		// 21: LBBLUE
};

/*****************************************************************************
 * @name       :void LCD_Clear2(u16 color_idx)
 * @date       :2022-05-03
 * @function   :Full screen filled LCD screen
 * @parameters :color:Filled color
 * @retvalue   :None
******************************************************************************/
void LCD_Clear2(u16 color_idx)
{
	memset(g_lcd_buf, color_idx, LCD_BUF_SIZE);
}

/*******************************************************************
 * @name       :void LCD_DrawLine2(u16 x1, u16 y1, u16 x2, u16 y2, u16 color_idx)
 * @date       :2022-05-03
 * @function   :Draw a line between two points
 * @parameters :x1:the bebinning x coordinate of the line
                y1:the bebinning y coordinate of the line
				x2:the ending x coordinate of the line
				y2:the ending y coordinate of the line
 * @retvalue   :None
********************************************************************/
void LCD_DrawLine2(u16 x1, u16 y1, u16 x2, u16 y2, u16 color_idx)
{
	u16 t;
	s32 xerr=0,yerr=0,delta_x,delta_y,distance;
	s32 incx,incy,uRow,uCol;

	delta_x=x2-x1;
	delta_y=y2-y1;
	uRow=x1;
	uCol=y1;
	if(delta_x>0)incx=1;
	else if(delta_x==0)incx=0;
	else {incx=-1;delta_x=-delta_x;}
	if(delta_y>0)incy=1;
	else if(delta_y==0)incy=0;
	else{incy=-1;delta_y=-delta_y;}
	if( delta_x>delta_y)distance=delta_x;
	else distance=delta_y;
	for(t=0;t<=distance+1;t++ )
	{
		LCD_DrawPoint2(uRow,uCol,color_idx);
		xerr+=delta_x ;
		yerr+=delta_y ;
		if(xerr>distance)
		{
			xerr-=distance;
			uRow+=incx;
		}
		if(yerr>distance)
		{
			yerr-=distance;
			uCol+=incy;
		}
	}
}

/*****************************************************************************
 * @name       :void LCD_DrawPoint2(u16 x, u16 y, u16 color_idx)
 * @date       :2022-05-03
 * @function   :Write a pixel data at a specified location
 * @parameters :x:the x coordinate of the pixel
                y:the y coordinate of the pixel
 * @retvalue   :None
******************************************************************************/
void LCD_DrawPoint2(u16 x, u16 y, u16 color_idx)
{
	//LCD_SetCursor(x,y);
	//Lcd_WriteData_16Bit(POINT_COLOR);
	g_lcd_buf[y*320 + x] = color_idx;
}


/*****************************************************************************
 * @name       :void LCD_DrawRectangle2(u16 x1, u16 y1, u16 x2, u16 y2)
 * @date       :2018-08-09
 * @function   :Draw a rectangle
 * @parameters :x1:the bebinning x coordinate of the rectangle
                y1:the bebinning y coordinate of the rectangle
				x2:the ending x coordinate of the rectangle
				y2:the ending y coordinate of the rectangle
 * @retvalue   :None
******************************************************************************/
void LCD_DrawRectangle2(u16 x1, u16 y1, u16 x2, u16 y2, u16 color_idx)
{
	LCD_DrawLine2(x1,y1,x2,y1,color_idx);
	LCD_DrawLine2(x1,y1,x1,y2,color_idx);
	LCD_DrawLine2(x1,y2,x2,y2,color_idx);
	LCD_DrawLine2(x2,y1,x2,y2,color_idx);
}

/*******************************************************************
 * @name       :void LCD_Fill2(u16 sx,u16 sy,u16 ex,u16 ey,u16 color)
 * @date       :2018-08-09
 * @function   :fill the specified area
 * @parameters :sx:the bebinning x coordinate of the specified area
                sy:the bebinning y coordinate of the specified area
				ex:the ending x coordinate of the specified area
				ey:the ending y coordinate of the specified area
				color:the filled color value
 * @retvalue   :None
********************************************************************/
void LCD_Fill2(u16 sx,u16 sy,u16 ex,u16 ey,u16 color_idx)
{
	u16 i,j;

	//memset(&g_lcd_buf[sy*320 + sx], color_idx, width * height);
	for(i=sy; i<=ey; i++)
	{
		for(j=sx; j<=ex; j++){
			g_lcd_buf[i*320 + j] = color_idx;
		}
	}

}


/*****************************************************************************
 * @name       :void LCD_ShowChar2(u16 x, u16 y, u8 num, u8 size, u16 color_idx)
 * @date       :2022-05-03
 * @function   :Display a single English character
 * @parameters :x:the bebinning x coordinate of the Character display position
                y:the bebinning y coordinate of the Character display position
								fc:the color value of display character
								bc:the background color of display character
								num:the ascii code of display character(0~94)
								size:the size of display character
								mode:0-no overlying,1-overlying
 * @retvalue   :None
******************************************************************************/
void LCD_ShowChar2(u16 x, u16 y, u8 num, u8 size, u16 color_idx)
{
    u8 temp;
    u8 pos,t;
	//u16 colortemp=POINT_COLOR;

	num=num-' ';//�õ�ƫ�ƺ��ֵ
	//LCD_SetWindows(x,y,x+size/2-1,y+size-1);//���õ���������ʾ����


	for(pos=0;pos<size;pos++)
	{
		if(size==12)temp=asc2_1206[num][pos];//����1206����
		else temp=asc2_1608[num][pos];		 //����1608����
		for(t=0;t<size/2;t++)
		{
			//POINT_COLOR=fc;
			if(temp&0x01)LCD_DrawPoint2(x+t,y+pos,color_idx);//��һ����
			temp>>=1;
		}
	}

	//POINT_COLOR=colortemp;
	//LCD_SetWindows(0,0,lcddev.width-1,lcddev.height-1);//�ָ�����Ϊȫ��
}


/*****************************************************************************
 * @name       :void Show_Str2(u16 x, u16 y, u8 *str, u8 size, u16 color_idx)
 * @date       :2022-05-03
 * @function   :Display Chinese and English strings
 * @parameters :x:the bebinning x coordinate of the Chinese and English strings
                y:the bebinning y coordinate of the Chinese and English strings
								fc:the color value of Chinese and English strings
								bc:the background color of Chinese and English strings
								str:the start address of the Chinese and English strings
								size:the size of Chinese and English strings
								mode:0-no overlying,1-overlying
 * @retvalue   :None
******************************************************************************/
void Show_Str2(u16 x, u16 y, u8 *str, u8 size, u16 color_idx)
{
	u16 x0=x;

    while(*str!=0)//����δ����
    {

		if(x>(lcddev.width-size/2)||y>(lcddev.height-size))
		return;

		if(*str==0x0D)//���з���
		{
			y+=size;
			x=x0;
			str++;
		}
		else
		{
			if(size>16)//�ֿ���û�м���12X24 16X32��Ӣ������,��8X16����
			{
			LCD_ShowChar2(x,y,*str,16,color_idx);
			x+=8; //�ַ�,Ϊȫ�ֵ�һ��
			}
			else
			{
			LCD_ShowChar2(x,y,*str,size,color_idx);
			x+=size/2; //�ַ�,Ϊȫ�ֵ�һ��
			}
		}
		str++;

    }
}



/*****************************************************************************
 * @name       :void Gui_StrCenter2(u16 x, u16 y, u8 *str, u8 size, u16 color_idx)
 * @date       :2022-05-03
 * @function   :Centered display of English strings
 * @parameters :x:the bebinning x coordinate of the English strings
                y:the bebinning y coordinate of the English strings
				str:the start address of the English strings
				size:the size of English strings
				color_idx:the color value of English strings
 * @retvalue   :None
******************************************************************************/
void Gui_StrCenter2(u16 x, u16 y, u8 *str, u8 size, u16 color_idx)
{
	u16 len=strlen((const char *)str);
	u16 x1=(lcddev.width-len*8)/2;
	Show_Str2(x1,y,str,size,color_idx);
}


/*****************************************************************************
 * @name       :void LCD_SendBuffer(SPI_HandleTypeDef *hspi, DMA_HandleTypeDef *hdma)
 * @date       :2022-05-03
 * @function   :Send RGB buffer data to LCD
 * @parameters :hspi:the SPI　handler
                hdma:the DMA handler
 * @retvalue   :None
******************************************************************************/
void LCD_SendBuffer(SPI_HandleTypeDef *hspi, DMA_HandleTypeDef *hdma)
{
	s32 i, p, ps;

	LCD_SetWindows(0,0,lcddev.width-1,lcddev.height-1);

	LCD_CS_CLR;
	LCD_RS_SET;

	for(i=0; i<LCD_SPLIT_SEND_NUM; i++){

		ps = i * LCD_SEND_SIZE;

		for(p=0; p<LCD_SEND_SIZE; p++){
			g_lcd_send_buf[p] = g_color_table[ g_lcd_buf[ps+p] ];
		}

		HAL_SPI_Transmit_DMA(hspi, (uint8_t*)g_lcd_send_buf, LCD_SEND_SIZE * 2);

		while(1){

#if 0	//ポーリングするより前にHAL_DMA_IRQHandler()で割り込みステータスをクリアされてしまうので、hdma->Stateをポーリングする方法に変更
			if(HAL_OK == HAL_DMA_PollForTransfer(hdma, HAL_DMA_FULL_TRANSFER, 1000)){
				hspi1.State = HAL_SPI_STATE_READY;
				break;
			}
#endif

			if(hdma->State == HAL_DMA_STATE_READY){
				hspi1.State = HAL_SPI_STATE_READY;
				break;
			}
		}
	}

	LCD_CS_SET;

}

