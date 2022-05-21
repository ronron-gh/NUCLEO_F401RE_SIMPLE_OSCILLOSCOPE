#include "stm32f4xx_hal.h"
#include "stdio.h"
#include "string.h"
#include "lcd.h"
#include "lcd2.h"

#define LCD_BUF_SIZE	(320 * 240)
#define LCD_SPLIT_SEND_NUM	(10)
#define LCD_SEND_SIZE	(LCD_BUF_SIZE / LCD_SPLIT_SEND_NUM)

int8_t g_lcd_buf[LCD_BUF_SIZE];
uint16_t g_lcd_send_buf[LCD_SEND_SIZE];

// カラーテーブル（予めエンディアンをスワップしておく）
uint16_t g_color_table[] = {
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
 * @name       :void LCD_Clear2(uint16_t color_idx)
 * @date       :2018-08-09
 * @function   :Full screen filled LCD screen
 * @parameters :color:Filled color
 * @retvalue   :None
******************************************************************************/
void LCD_Clear2(uint16_t color_idx)
{
	memset(g_lcd_buf, color_idx, LCD_BUF_SIZE);
}

/*******************************************************************
 * @name       :void LCD_DrawLine2(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t color_idx)
 * @date       :2022-05-03
 * @function   :Draw a line between two points
 * @parameters :x1:the bebinning x coordinate of the line
                y1:the bebinning y coordinate of the line
				x2:the ending x coordinate of the line
				y2:the ending y coordinate of the line
 * @retvalue   :None
********************************************************************/
void LCD_DrawLine2(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t color_idx)
{
	uint16_t t;
	int32_t xerr=0,yerr=0,delta_x,delta_y,distance;
	int32_t incx,incy,uRow,uCol;

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
 * @name       :void LCD_DrawPoint2(uint16_t x, uint16_t y, uint16_t color_idx)
 * @date       :2022-05-03
 * @function   :Write a pixel data at a specified location
 * @parameters :x:the x coordinate of the pixel
                y:the y coordinate of the pixel
 * @retvalue   :None
******************************************************************************/
void LCD_DrawPoint2(uint16_t x, uint16_t y, uint16_t color_idx)
{
	//LCD_SetCursor(x,y);
	//Lcd_WriteData_16Bit(POINT_COLOR);
	g_lcd_buf[y*320 + x] = color_idx;
}

void LCD_SendBuffer(SPI_HandleTypeDef *hspi, DMA_HandleTypeDef *hdma, TIM_HandleTypeDef *htim)
{
	int32_t i, p, ps;

	//htim->Instance->CR1 &= ~(TIM_CR1_CEN);	//サンプリングを一時停止


	LCD_SetWindows(0,0,lcddev.width-1,lcddev.height-1);

	LCD_CS_CLR;
	LCD_RS_SET;

	for(i=0; i<LCD_SPLIT_SEND_NUM; i++){

		ps = i * LCD_SEND_SIZE;

		for(p=0; p<LCD_SEND_SIZE; p++){
			g_lcd_send_buf[p] = g_color_table[ g_lcd_buf[ps+p] ];
		}

		//hdma->Instance->CR |= DMA_SxCR_TCIE;	//転送完了割り込みを有効化
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

	//if(g_is_trig_stop == FALSE){
	//	htim->Instance->CR1 |= TIM_CR1_CEN;		//サンプリングを再開
	//}
}

