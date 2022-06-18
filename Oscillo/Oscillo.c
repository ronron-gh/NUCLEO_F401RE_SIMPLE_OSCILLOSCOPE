#include "stdio.h"
#include "string.h"
#include "main.h"
#include "Oscillo.h"
#include "Touch.h"
#include "lcd.h"
#include "Lcd2.h"


uint32_t adc_dr_dma[ADC_BUF_SIZE] = {0};
float adc_phy[ADC_BUF_SIZE] = {0};


void onClickBtnSetting(void);
void onClickBtnRun(void);
void drawGraphView(void);

void onClickBtnDivTime1(void);
void onClickBtnDivTime2(void);
void onClickBtnTrigOn(void);
void onClickBtnTrigOff(void);
void onClickBtnDivVolt1(void);
void onClickBtnDivVolt2(void);
void onClickBtnDivVolt3(void);
void onClickBtnOk(void);
void drawSettingView(void);

/*****************************
 * Graph View Instance
 *****************************/
							// x1, y1, width, height, color,    str,     ,str_size, str_color
struct Label labelTrigOn  = { 25,  5,  70,     20,     1,    "Trig ON ",     16,        0,
							TRUE,
							NULL
							};

							// x1, y1, width, height, color,    str,     ,str_size, str_color
struct Label labelTrigOff  = { 25,  5,  70,     20,     1,    "Trig OFF",     16,        0,
							FALSE,
							NULL
							};

							// x1,  y1, width, height, color,    str,     ,str_size, str_color
struct Label btnRun		   = { 110,  5,  50,     20,     13,    "Run",     14,        1,
							TRUE,
							onClickBtnRun
							};


							// x1,  y1, width, height, color,    str,     ,str_size, str_color
struct Label btnSetting   = {240, 215,  70,     20,     13,  "Settings",     14,        1,
							TRUE,
							onClickBtnSetting
							};


struct View graphView = {
							drawGraphView,
							{
									&labelTrigOn,
									&labelTrigOff,
									&btnRun,
									&btnSetting
							}
						};

/*****************************
 * Setting View Instance
 *****************************/
						   // x1,  y1, width, height, color,    str,       ,str_size, str_color
struct Label labelSettings = { 5,   5,  310,    20,    14,   "Settings",      16,       0,
							TRUE,
							NULL
							};

						  // x1,  y1, width, height, color,    str,       ,str_size, str_color
struct Label labelDivTime = {10,  50,  80,    20,      1,   "ms/div",      16,       0,
							TRUE,
							NULL
							};


						  // x1,  y1, width, height, color,    str,   ,str_size, str_color
struct Label btnDivTime1 = {100,  50,   50,    20,     13,   "0.05",      14,        1,
							TRUE,
							onClickBtnDivTime1
							};

						  // x1,  y1, width, height, color,    str,   ,str_size, str_color
struct Label btnDivTime2 = {170,  50,  50,     20,     13,    "0.1",      14,        1,
							TRUE,
							onClickBtnDivTime2
							};

						  // x1,  y1, width, height, color,    str,       ,str_size, str_color
struct Label labelDivVolt = {10,  100,  80,    20,      1,   "V/div",      16,       0,
							TRUE,
							NULL
							};


						  // x1,  y1, width, height, color,    str,   ,str_size, str_color
struct Label btnDivVolt1 = {100,  100,   50,    20,     13,    "0.5",      14,        1,
							TRUE,
							onClickBtnDivVolt1
							};

						  // x1,  y1, width, height, color,    str,   ,str_size, str_color
struct Label btnDivVolt2 = {170,  100,  50,     20,     13,    "1.0",      14,        1,
							TRUE,
							onClickBtnDivVolt2
							};

						  // x1,  y1, width, height, color,    str,   ,str_size, str_color
struct Label btnDivVolt3 = {240,  100,  50,     20,     13,    "2.0",      14,        1,
							TRUE,
							onClickBtnDivVolt3
							};

						  // x1,  y1, width, height, color,    str,   ,str_size, str_color
struct Label labelTrig   = { 10, 150,  80,    20,      1,   "Trig",      16,       0,
							TRUE,
							NULL
							};


						  // x1,  y1, width, height, color,    str,   ,str_size, str_color
struct Label btnTrigOn =   {100,  150,   50,    20,     13,    "ON",      14,        1,
							TRUE,
							onClickBtnTrigOn
							};

						  // x1,  y1, width, height, color,    str,   ,str_size, str_color
struct Label btnTrigOff =  {170,  150,  50,     20,     13,    "OFF",      14,        1,
							TRUE,
							onClickBtnTrigOff
							};

						  // x1,  y1, width, height, color,    str,   ,str_size, str_color
struct Label btnOk		  = {110, 215,  100,     20,     13,   "O K",      14,        1,
							TRUE,
							onClickBtnOk
							};


struct View settingView = {
								drawSettingView,
								{
										&labelSettings,
										&labelDivTime,
										&btnDivTime1,
										&btnDivTime2,
										&labelDivVolt,
										&btnDivVolt1,
										&btnDivVolt2,
										&btnDivVolt3,
										&labelTrig,
										&btnTrigOn,
										&btnTrigOff,
										&btnOk
								}
						};

/*****************************
 * Current view pointer
 *****************************/
struct View *current_view = &graphView;
struct Label *currentBtnDiv = &btnDivTime2;
struct Label *currentBtnDivVolt = &btnDivVolt2;
struct Label *currentBtnTrig = &btnTrigOn;

/*****************************
 * Common function
 *****************************/
void oscillo_init(void)
{
	  HAL_ADC_Start_DMA(&hadc1, adc_dr_dma, ADC_BUF_SIZE);

	  // Output 10kHz PWM signal from PB10 (CN9/D6) pin as test signal  @PCLK2(84MHz)
	  HAL_TIM_OC_Start(&htim2, TIM_CHANNEL_3);

	  // Start 200kHz timer for sampling  @PCLK2(84MHz)
	  // ( Sampling rate (MAX) = PCLK2(84MHz) / 4div / 15cycle(12bit ADC conversion time) = 1.4MHz )
	  HAL_TIM_OC_Start(&htim3, TIM_CHANNEL_1);

}

static void drawLabel(struct Label *label)
{
	u16 str_x, str_y, len;

	LCD_Fill2(label->x, label->y ,label->x + label->width ,label->y + label->height ,label->color);

	len=strlen((const char *)label->str);
	str_x = label->x + (label->width - len * label->str_size / 2) / 2;
	str_y = label->y + (label->height - label->str_size) / 2;

	Show_Str2(str_x, str_y, label->str, label->str_size, label->str_color);
}

static void drawLabels(struct View *view)
{
	for(int i=0; i<LABEL_LIST_MAX; i++){
		if(view->labelList[i] != NULL){
			if(view->labelList[i]->enable == TRUE){
				drawLabel(view->labelList[i]);
			}
		}
	}
}

void oscillo_draw(struct View *view)
{
	view->drawView();
	drawLabels(view);
	LCD_SendBuffer(&hspi1, &hdma_spi1_tx);
}

/*****************************
 * Graph view function
 *****************************/

#define ORIGIN_PX (30)
#define ORIGIN_PY (lcddev.height-30)
#define ZERO_V_PY	(ORIGIN_PY - DIV_PY * 2)
#define MAX_RANGE_PX	(280)
#define MAX_RANGE_PY	(180)
#define DIV_PX	(20)
#define DIV_PY	(20)
//#define DIV_VOLT	(1.0)	//V
//#define DIV_TIME	(0.1)	//msec
//#define PX_PER_TIME	(DIV_PX / DIV_TIME)
//#define PY_PER_VOLT	(DIV_PY / DIV_VOLT)

double vDIV_VOLT	= 1.0;	// 1DIVあたりの電圧 [V] (可変)
double vDIV_TIME	= 0.1;	// 1DIVあたりの時間 [msec] (可変)
double vPY_PER_VOLT;		// ピクセルあたりの電圧 ( = DIV_PY / vDIV_VOLT )
double vPX_PER_TIME;		// ピクセルあたりの時間 ( = DIV_PX / vDIV_TIME )

static void drawGraph(float *data, int32_t len, float dt)
{
	int32_t axis_px, axis_py;
	int32_t px, py, old_px, old_py;
	int8_t div_info[128];
	uint16_t color_idx;


	LCD_Clear2(1);		// BLACK

	// Draw graph area window
	color_idx = 0;		// WHITE

	LCD_DrawLine2(ORIGIN_PX, ORIGIN_PY - MAX_RANGE_PY , ORIGIN_PX, ORIGIN_PY, color_idx);
	LCD_DrawLine2(ORIGIN_PX + MAX_RANGE_PX, ORIGIN_PY - MAX_RANGE_PY , ORIGIN_PX + MAX_RANGE_PX, ORIGIN_PY, color_idx);
	LCD_DrawLine2(ORIGIN_PX, ORIGIN_PY - MAX_RANGE_PY , ORIGIN_PX + MAX_RANGE_PX, ORIGIN_PY - MAX_RANGE_PY, color_idx);
	LCD_DrawLine2(ORIGIN_PX, ORIGIN_PY , ORIGIN_PX + MAX_RANGE_PX, ORIGIN_PY, color_idx);


	// Draw gridline
	color_idx = 16;		// GRAYBLUE

	for(int i=1; i<9; i++){
		LCD_DrawLine2(ORIGIN_PX, ORIGIN_PY - MAX_RANGE_PY + DIV_PY * i, ORIGIN_PX + MAX_RANGE_PX, ORIGIN_PY - MAX_RANGE_PY + DIV_PY * i, color_idx);
	}

	for(int i=1; i<14; i++){
		LCD_DrawLine2(ORIGIN_PX + DIV_PX * i, ORIGIN_PY - MAX_RANGE_PY , ORIGIN_PX + DIV_PX * i, ORIGIN_PY, color_idx);
	}


	sprintf(div_info,"%0.2f V/div   %0.2f ms/div", vDIV_VOLT, vDIV_TIME);
	color_idx = 0;	// WHITE
	Show_Str2(30,220,div_info,16,color_idx);

	// Draw data point
	color_idx = 10;		// YELLOW

	vPX_PER_TIME = (DIV_PX / vDIV_TIME);
	vPY_PER_VOLT = (DIV_PY / vDIV_VOLT);

	axis_px = vPX_PER_TIME * dt * 0;
	axis_py = vPY_PER_VOLT * data[0];
	old_px = ORIGIN_PX + axis_px;
	old_py = ZERO_V_PY - axis_py;

	for(int i=1; i<len; i++){
		axis_px = vPX_PER_TIME * dt * i;
		axis_py = vPY_PER_VOLT * data[i];
		px = ORIGIN_PX + axis_px;
		py = ZERO_V_PY - axis_py;
		if((px <= ORIGIN_PX + MAX_RANGE_PX) && (ORIGIN_PY - MAX_RANGE_PY <= py) && (py <= ORIGIN_PY)){
			//LCD_DrawPoint(px_x, px_y);
			LCD_DrawLine2(old_px, old_py , px, py, color_idx);
			old_px = px;
			old_py = py;

		}
	}


	// Draw 0V marker
	LCD_DrawLine2(ORIGIN_PX, ZERO_V_PY, ORIGIN_PX - 8, ZERO_V_PY - 4, color_idx);
	LCD_DrawLine2(ORIGIN_PX - 8, ZERO_V_PY - 4, ORIGIN_PX - 8, ZERO_V_PY + 4, color_idx);
	LCD_DrawLine2(ORIGIN_PX - 8, ZERO_V_PY + 4, ORIGIN_PX, ZERO_V_PY, color_idx);

}

static void showAdcData(uint32_t ndtr, uint32_t offset)
{
	int32_t start, end, idx;

	start = ADC_BUF_SIZE - ndtr - offset;

	if(start < 0){
		start = ADC_BUF_SIZE + start;
	}

	end = start + SHOW_SIZE;

	if(end >= ADC_BUF_SIZE){
		end = end - ADC_BUF_SIZE;
	}


	for(int i=0; i<SHOW_SIZE; i++){
		idx = start + i;
		if(idx >= ADC_BUF_SIZE){
			idx = idx - ADC_BUF_SIZE;
		}

		adc_phy[i] = adc_dr_dma[idx] * 3.3 / 4096;
	}
	drawGraph(adc_phy, SHOW_SIZE, 0.005);
}


void drawGraphView(void)
{
    showAdcData(hdma_adc1.Instance->NDTR, SHOW_SIZE);
}

void onClickBtnRun(void)
{

	if(currentBtnTrig == &btnTrigOn){
		/* Set AWDEN bits */
		hadc1.Instance->CR1 |=  ADC_CR1_AWDIE;
	}
	else{
		/* Clear AWDEN bits */
		hadc1.Instance->CR1 &=  ~(ADC_CR1_AWDIE);
	}

	/* Start a timer for ADC trigger */
	htim3.Instance->CR1 |= TIM_CR1_CEN;
}

void onClickBtnSetting(void)
{
	//printf("Settings button is touched\n");
	current_view = &settingView;
}

/*****************************
 * Setting view function
 *****************************/
void drawSettingView(void)
{
	u16 x1,y1,x2,y2;

	LCD_Clear2(1);		// BLACK

	// 選択されているボタンを枠で強調
	// Divあたりの時間
	x1 = currentBtnDiv->x - 3;
	y1 = currentBtnDiv->y - 3;
	x2 = currentBtnDiv->x + currentBtnDiv->width + 3;
	y2 = currentBtnDiv->y + currentBtnDiv->height + 3;
	LCD_DrawRectangle2(x1, y1, x2, y2, 0);

	// Divあたりの電圧
	x1 = currentBtnDivVolt->x - 3;
	y1 = currentBtnDivVolt->y - 3;
	x2 = currentBtnDivVolt->x + currentBtnDivVolt->width + 3;
	y2 = currentBtnDivVolt->y + currentBtnDivVolt->height + 3;
	LCD_DrawRectangle2(x1, y1, x2, y2, 0);

	// Trig ON/OFF
	x1 = currentBtnTrig->x - 3;
	y1 = currentBtnTrig->y - 3;
	x2 = currentBtnTrig->x + currentBtnTrig->width + 3;
	y2 = currentBtnTrig->y + currentBtnTrig->height + 3;
	LCD_DrawRectangle2(x1, y1, x2, y2, 0);


}

void onClickBtnDivTime1(void)
{
	vDIV_TIME = 0.05;
	currentBtnDiv = &btnDivTime1;
}

void onClickBtnDivTime2(void)
{
	vDIV_TIME = 0.1;
	currentBtnDiv = &btnDivTime2;
}

void onClickBtnDivVolt1(void)
{
	vDIV_VOLT = 0.5;
	currentBtnDivVolt = &btnDivVolt1;
}

void onClickBtnDivVolt2(void)
{
	vDIV_VOLT = 1.0;
	currentBtnDivVolt = &btnDivVolt2;
}

void onClickBtnDivVolt3(void)
{
	vDIV_VOLT = 2.0;
	currentBtnDivVolt = &btnDivVolt3;
}

void onClickBtnTrigOn(void)
{
	labelTrigOn.enable = TRUE;
	labelTrigOff.enable = FALSE;
	/* Set AWDEN bits */
	hadc1.Instance->CR1 |=  ADC_CR1_AWDIE;

	currentBtnTrig = &btnTrigOn;
}

void onClickBtnTrigOff(void)
{
	labelTrigOn.enable = FALSE;
	labelTrigOff.enable = TRUE;
	/* Clear AWDEN bits */
	hadc1.Instance->CR1 &=  ~(ADC_CR1_AWDIE);

	currentBtnTrig = &btnTrigOff;
}

void onClickBtnOk(void)
{
	current_view = &graphView;
}

/*****************************
 * Touch event handler
 *****************************/
void oscillo_touchEventHandler(void)
{
	int i;
	struct Label *label;
	tp_dev.scan(0);
	if(tp_dev.sta&TP_PRES_DOWN)
	{
	 	if(tp_dev.x<lcddev.width&&tp_dev.y<lcddev.height)
		{
	 		for(i=0; i<LABEL_LIST_MAX; i++){
	 			label = current_view->labelList[i];
	 			if(label != NULL){
	 				if((label->enable == TRUE) && (label->onClick != NULL)){
		 				if((label->x < tp_dev.x) && (tp_dev.x < label->x + label->width)
		 					&& (label->y < tp_dev.y) && (tp_dev.y < label->y + label->height))
		 				{
		 					label->onClick();

		 				}
	 				}

	 			}
	 			else{
	 				break;
	 			}
	 		}


		}
	}
}


/*****************************
 * HAL ISR
 *****************************/

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
	if(GPIO_Pin == B1_Pin){
		printf("B1 pin is pushed (main.c)\n");

		/* Set AWDEN bits */
		hadc1.Instance->CR1 |=  ADC_CR1_AWDIE;

		/* Start a timer for ADC trigger */
		htim3.Instance->CR1 |= TIM_CR1_CEN;
	}
	else if(GPIO_Pin == T_IRQ_Pin){
		printf("LCD touch interrupt (main.c)\n");
		oscillo_touchEventHandler();
	}

}

void HAL_ADC_LevelOutOfWindowCallback(ADC_HandleTypeDef* hadc)
{
	// Start a timer to stop sampling after 1 millisecond (200 point @200kHz sampling)
	HAL_TIM_OC_Start_IT(&htim5, TIM_CHANNEL_2);
	htim5.Instance->CR1 |= TIM_CR1_CEN;

	/* Clear AWDEN bits */
	hadc->Instance->CR1 &=  ~(ADC_CR1_AWDIE);

	//printf("Analog Watchdog (main.c)\n");
	//printf("DMA NDT: %d\n", hdma_adc1.Instance->NDTR);

}

void HAL_TIM_OC_DelayElapsedCallback(TIM_HandleTypeDef *htim)
{
	htim3.Instance->CR1 &= ~(TIM_CR1_CEN);

}


