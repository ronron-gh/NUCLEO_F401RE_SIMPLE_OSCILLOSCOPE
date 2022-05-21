

void LCD_Clear2(uint16_t color_idx);
void LCD_DrawLine2(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t color_idx);
void LCD_DrawPoint2(uint16_t x, uint16_t y, uint16_t color_idx);
void LCD_SendBuffer(SPI_HandleTypeDef *hspi, DMA_HandleTypeDef *hdma, TIM_HandleTypeDef *htim);

