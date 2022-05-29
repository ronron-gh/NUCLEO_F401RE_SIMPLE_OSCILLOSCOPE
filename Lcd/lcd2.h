#ifndef __LCD2_H__
#define __LCD2_H__

void LCD_Clear2(u16 color_idx);
void LCD_DrawLine2(u16 x1, u16 y1, u16 x2, u16 y2, u16 color_idx);
void LCD_DrawPoint2(u16 x, u16 y, u16 color_idx);

void LCD_ShowChar2(u16 x, u16 y, u8 num, u8 size, u16 color_idx);
void Show_Str2(u16 x, u16 y, u8 *str, u8 size, u16 color_idx);
void Gui_StrCenter2(u16 x, u16 y, u8 *str, u8 size, u16 color_idx);

void LCD_SendBuffer(SPI_HandleTypeDef *hspi, DMA_HandleTypeDef *hdma, TIM_HandleTypeDef *htim);

#endif // __LCD2_H__
