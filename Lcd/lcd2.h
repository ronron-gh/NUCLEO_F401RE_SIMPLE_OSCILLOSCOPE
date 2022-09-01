#ifndef __LCD2_H__
#define __LCD2_H__

#include "main.h"

#define LCD_BUF_SIZE	(320 * 240)
#define LCD_SPLIT_SEND_NUM	(10)
#define LCD_SEND_SIZE	(LCD_BUF_SIZE / LCD_SPLIT_SEND_NUM)


void LCD_Clear2(u16 color_idx);
void LCD_DrawLine2(u16 x1, u16 y1, u16 x2, u16 y2, u16 color_idx);
void LCD_DrawPoint2(u16 x, u16 y, u16 color_idx);
void LCD_DrawRectangle2(u16 x1, u16 y1, u16 x2, u16 y2, u16 color_idx);
void LCD_Fill2(u16 sx,u16 sy,u16 ex,u16 ey,u16 color_idx);

void LCD_ShowChar2(u16 x, u16 y, u8 num, u8 size, u16 color_idx);
void Show_Str2(u16 x, u16 y, u8 *str, u8 size, u16 color_idx);
void Gui_StrCenter2(u16 x, u16 y, u8 *str, u8 size, u16 color_idx);

void LCD_SendBuffer(SPI_HandleTypeDef *hspi, DMA_HandleTypeDef *hdma);

#endif // __LCD2_H__
