/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2022 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "stdio.h"
#include "string.h"
#include "math.h"
#include "lcd.h"
#include "lcd2.h"
#include "GUI.h"
#include "test.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
ADC_HandleTypeDef hadc1;
DMA_HandleTypeDef hdma_adc1;

SPI_HandleTypeDef hspi1;
DMA_HandleTypeDef hdma_spi1_tx;

TIM_HandleTypeDef htim2;
TIM_HandleTypeDef htim3;
TIM_HandleTypeDef htim5;

UART_HandleTypeDef huart2;

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_USART2_UART_Init(void);
static void MX_SPI1_Init(void);
static void MX_DMA_Init(void);
static void MX_ADC1_Init(void);
static void MX_TIM2_Init(void);
static void MX_TIM3_Init(void);
static void MX_TIM5_Init(void);
/* USER CODE BEGIN PFP */
void drawGraph(float *data, int32_t len, float dt);
void showAdcData(uint32_t ndtr, uint32_t offset);
/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
uint32_t adc_dr_dma[ADC_BUF_SIZE] = {0};
float adc_phy[ADC_BUF_SIZE] = {0};
int32_t enable_display_update = TRUE;
int32_t trig_ndtr = 0;
int32_t g_is_trig_stop = FALSE;

#define LCD_BUF_SIZE	(320 * 240 * 2)
//int8_t lcd_buf_black[64000] = {0};
const int8_t lcd_buf_black[LCD_BUF_SIZE] = {0};
/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* USER CODE BEGIN 1 */
  //float data[100];
  uint32_t adc_dr;

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_USART2_UART_Init();
  MX_DMA_Init();
  MX_SPI1_Init();

  MX_ADC1_Init();
  MX_TIM2_Init();
  MX_TIM3_Init();
  MX_TIM5_Init();
  /* USER CODE BEGIN 2 */
  hadc1.Instance->CR1 &=  ~(ADC_CR1_AWDIE);
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  LCD_Init();

#if 0
  float w = 2 * 3.141592 / 0.1;
  for(int i=0; i<100; i++){
	  data[i] = sin(w * 0.001 * i) + 1.5;
  }
  drawGraph(data, 100);
#endif

  //HAL_ADC_Start_DMA(&hadc1, adc_dr_dma, 5);
  HAL_ADC_Start_DMA(&hadc1, adc_dr_dma, ADC_BUF_SIZE);

  //10kHzのPWM  @PCLK2(84MHz)
  HAL_TIM_OC_Start(&htim2, TIM_CHANNEL_3);

  //サンプリングRate(MAX) = PCLK2(84MHz) / 4div / 15サイクル(12bit ADCの変換時間) = 1.4MHz
  //とりあえず200kHzで1ms(200点)サンプリング  @PCLK2(84MHz)
  HAL_TIM_OC_Start(&htim3, TIM_CHANNEL_1);

  //drawGraph(NULL, 0, 0);

  while (1)
  {
	  HAL_Delay(100);
	  //if(g_is_trig_stop == FALSE){
	  	  //htim3.Instance->CR1 &= ~(TIM_CR1_CEN);
		  showAdcData(hdma_adc1.Instance->NDTR, SHOW_SIZE);
	  //}

#if 0
	  //HAL_ADC_Start_DMA(&hadc1, adc_dr_dma, 200);

	  HAL_Delay(1000);
	  //HAL_GPIO_TogglePin(LD2_GPIO_Port, LD2_Pin);

	  for(int i=0; i<ADC_BUF_SIZE; i++){
		  adc_phy[i] = adc_dr_dma[i] * 3.3 / 4096;
	  }
	  drawGraph(adc_phy, ADC_BUF_SIZE, 0.005);

#endif


#if 0
	  HAL_ADC_Start(&hadc1);
	  HAL_ADC_PollForConversion(&hadc1, 1000);
	  adc_dr = HAL_ADC_GetValue(&hadc1);
	  //adc_phy = adc_dr * 3.3 / 4096;
	  printf("adc result: %ld\n", adc_dr);
#endif

#if 0
	  //main_test();
	  	Test_Color();
		Test_FillRec();
		Test_Circle();
		Test_Triangle();
#endif
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Configure the main internal regulator output voltage
  */
  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE2);
  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL.PLLM = 16;
  RCC_OscInitStruct.PLL.PLLN = 336;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV4;
  RCC_OscInitStruct.PLL.PLLQ = 7;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }
  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief ADC1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_ADC1_Init(void)
{

  /* USER CODE BEGIN ADC1_Init 0 */

  /* USER CODE END ADC1_Init 0 */

  ADC_AnalogWDGConfTypeDef AnalogWDGConfig = {0};
  ADC_ChannelConfTypeDef sConfig = {0};

  /* USER CODE BEGIN ADC1_Init 1 */

  /* USER CODE END ADC1_Init 1 */
  /** Configure the global features of the ADC (Clock, Resolution, Data Alignment and number of conversion)
  */
  hadc1.Instance = ADC1;
  hadc1.Init.ClockPrescaler = ADC_CLOCK_SYNC_PCLK_DIV4;
  hadc1.Init.Resolution = ADC_RESOLUTION_12B;
  hadc1.Init.ScanConvMode = DISABLE;
  hadc1.Init.ContinuousConvMode = DISABLE;
  hadc1.Init.DiscontinuousConvMode = DISABLE;
  hadc1.Init.ExternalTrigConvEdge = ADC_EXTERNALTRIGCONVEDGE_RISING;
  hadc1.Init.ExternalTrigConv = ADC_EXTERNALTRIGCONV_T3_CC1;
  hadc1.Init.DataAlign = ADC_DATAALIGN_RIGHT;
  hadc1.Init.NbrOfConversion = 1;
  hadc1.Init.DMAContinuousRequests = ENABLE;
  hadc1.Init.EOCSelection = ADC_EOC_SINGLE_CONV;
  if (HAL_ADC_Init(&hadc1) != HAL_OK)
  {
    Error_Handler();
  }
  /** Configure the analog watchdog
  */
  AnalogWDGConfig.WatchdogMode = ADC_ANALOGWATCHDOG_SINGLE_REG;
  AnalogWDGConfig.HighThreshold = 2048;
  AnalogWDGConfig.LowThreshold = 0;
  AnalogWDGConfig.Channel = ADC_CHANNEL_0;
  AnalogWDGConfig.ITMode = ENABLE;
  if (HAL_ADC_AnalogWDGConfig(&hadc1, &AnalogWDGConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /** Configure for the selected ADC regular channel its corresponding rank in the sequencer and its sample time.
  */
  sConfig.Channel = ADC_CHANNEL_0;
  sConfig.Rank = 1;
  sConfig.SamplingTime = ADC_SAMPLETIME_3CYCLES;
  if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN ADC1_Init 2 */

  /* USER CODE END ADC1_Init 2 */

}

/**
  * @brief SPI1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_SPI1_Init(void)
{

  /* USER CODE BEGIN SPI1_Init 0 */

  /* USER CODE END SPI1_Init 0 */

  /* USER CODE BEGIN SPI1_Init 1 */

  /* USER CODE END SPI1_Init 1 */
  /* SPI1 parameter configuration*/
  hspi1.Instance = SPI1;
  hspi1.Init.Mode = SPI_MODE_MASTER;
  hspi1.Init.Direction = SPI_DIRECTION_2LINES;
  hspi1.Init.DataSize = SPI_DATASIZE_8BIT;
  hspi1.Init.CLKPolarity = SPI_POLARITY_LOW;
  hspi1.Init.CLKPhase = SPI_PHASE_1EDGE;
  hspi1.Init.NSS = SPI_NSS_SOFT;
  hspi1.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_2;
  hspi1.Init.FirstBit = SPI_FIRSTBIT_MSB;
  hspi1.Init.TIMode = SPI_TIMODE_DISABLE;
  hspi1.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
  hspi1.Init.CRCPolynomial = 10;
  if (HAL_SPI_Init(&hspi1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN SPI1_Init 2 */

  /* USER CODE END SPI1_Init 2 */

}

/**
  * @brief TIM2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM2_Init(void)
{

  /* USER CODE BEGIN TIM2_Init 0 */

  /* USER CODE END TIM2_Init 0 */

  TIM_MasterConfigTypeDef sMasterConfig = {0};
  TIM_OC_InitTypeDef sConfigOC = {0};

  /* USER CODE BEGIN TIM2_Init 1 */

  /* USER CODE END TIM2_Init 1 */
  htim2.Instance = TIM2;
  htim2.Init.Prescaler = 0;
  htim2.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim2.Init.Period = 8800;
  htim2.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim2.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_ENABLE;
  if (HAL_TIM_PWM_Init(&htim2) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim2, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sConfigOC.OCMode = TIM_OCMODE_PWM1;
  sConfigOC.Pulse = 4400;
  sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
  sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
  if (HAL_TIM_PWM_ConfigChannel(&htim2, &sConfigOC, TIM_CHANNEL_3) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM2_Init 2 */

  /* USER CODE END TIM2_Init 2 */
  HAL_TIM_MspPostInit(&htim2);

}

/**
  * @brief TIM3 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM3_Init(void)
{

  /* USER CODE BEGIN TIM3_Init 0 */

  /* USER CODE END TIM3_Init 0 */

  TIM_MasterConfigTypeDef sMasterConfig = {0};
  TIM_OC_InitTypeDef sConfigOC = {0};

  /* USER CODE BEGIN TIM3_Init 1 */

  /* USER CODE END TIM3_Init 1 */
  htim3.Instance = TIM3;
  htim3.Init.Prescaler = 0;
  htim3.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim3.Init.Period = 440;
  htim3.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim3.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_ENABLE;
  if (HAL_TIM_PWM_Init(&htim3) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim3, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sConfigOC.OCMode = TIM_OCMODE_PWM1;
  sConfigOC.Pulse = 220;
  sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
  sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
  if (HAL_TIM_PWM_ConfigChannel(&htim3, &sConfigOC, TIM_CHANNEL_1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM3_Init 2 */

  /* USER CODE END TIM3_Init 2 */
  HAL_TIM_MspPostInit(&htim3);

}

/**
  * @brief TIM5 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM5_Init(void)
{

  /* USER CODE BEGIN TIM5_Init 0 */

  /* USER CODE END TIM5_Init 0 */

  TIM_MasterConfigTypeDef sMasterConfig = {0};
  TIM_OC_InitTypeDef sConfigOC = {0};

  /* USER CODE BEGIN TIM5_Init 1 */

  /* USER CODE END TIM5_Init 1 */
  htim5.Instance = TIM5;
  htim5.Init.Prescaler = 0;
  htim5.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim5.Init.Period = 168000;
  htim5.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim5.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_OC_Init(&htim5) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_TIM_OnePulse_Init(&htim5, TIM_OPMODE_SINGLE) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim5, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sConfigOC.OCMode = TIM_OCMODE_ACTIVE;
  sConfigOC.Pulse = 84000;
  sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
  sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
  if (HAL_TIM_OC_ConfigChannel(&htim5, &sConfigOC, TIM_CHANNEL_2) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM5_Init 2 */

  /* USER CODE END TIM5_Init 2 */

}

/**
  * @brief USART2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART2_UART_Init(void)
{

  /* USER CODE BEGIN USART2_Init 0 */

  /* USER CODE END USART2_Init 0 */

  /* USER CODE BEGIN USART2_Init 1 */

  /* USER CODE END USART2_Init 1 */
  huart2.Instance = USART2;
  huart2.Init.BaudRate = 115200;
  huart2.Init.WordLength = UART_WORDLENGTH_8B;
  huart2.Init.StopBits = UART_STOPBITS_1;
  huart2.Init.Parity = UART_PARITY_NONE;
  huart2.Init.Mode = UART_MODE_TX_RX;
  huart2.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart2.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart2) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART2_Init 2 */

  /* USER CODE END USART2_Init 2 */

}

/**
  * Enable DMA controller clock
  */
static void MX_DMA_Init(void)
{

  /* DMA controller clock enable */
  __HAL_RCC_DMA2_CLK_ENABLE();

  /* DMA interrupt init */
  /* DMA2_Stream0_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(DMA2_Stream0_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(DMA2_Stream0_IRQn);
  /* DMA2_Stream3_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(DMA2_Stream3_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(DMA2_Stream3_IRQn);

}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOH_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(LCD_RS_GPIO_Port, LCD_RS_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(LCD_RESET_GPIO_Port, LCD_RESET_Pin, GPIO_PIN_SET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(SPI1_CS_GPIO_Port, SPI1_CS_Pin, GPIO_PIN_SET);

  /*Configure GPIO pin : B1_Pin */
  GPIO_InitStruct.Pin = B1_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_FALLING;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(B1_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pin : LCD_RS_Pin */
  GPIO_InitStruct.Pin = LCD_RS_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(LCD_RS_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pin : LCD_RESET_Pin */
  GPIO_InitStruct.Pin = LCD_RESET_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(LCD_RESET_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pin : SPI1_CS_Pin */
  GPIO_InitStruct.Pin = SPI1_CS_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(SPI1_CS_GPIO_Port, &GPIO_InitStruct);

  /* EXTI interrupt init*/
  HAL_NVIC_SetPriority(EXTI15_10_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(EXTI15_10_IRQn);

}

/* USER CODE BEGIN 4 */
int _write(int file, char *ptr, int len)
{
  int DataIdx;
  for(DataIdx=0; DataIdx<len; DataIdx++)
  {
    ITM_SendChar(*ptr++);
  }
  return len;
}

#define ORIGIN_PX (30)
#define ORIGIN_PY (lcddev.height-30)
#define ZERO_V_PY	(ORIGIN_PY - DIV_PY * 3)
#define MAX_RANGE_PX	(280)
#define MAX_RANGE_PY	(200)
#define DIV_PX	(20)
#define DIV_PY	(20)
#define DIV_VOLT	(0.5)	//V
#define DIV_TIME	(0.1)	//msec
//#define DIV_TIME	(0.01)	//msec
#define PX_PER_TIME	(DIV_PX / DIV_TIME)
#define PY_PER_VOLT	(DIV_PY / DIV_VOLT)

#if 0
void LCD_Clear_black(void)
{
	  volatile unsigned int i, m;

	  LCD_SetWindows(0,0,lcddev.width-1,lcddev.height-1);

	  LCD_CS_CLR;
	  LCD_RS_SET;

	  for(i=0; i<4; i++){
		  HAL_SPI_Transmit_DMA(&hspi1, &lcd_buf_black[i * 38400], LCD_BUF_SIZE / 4);

		  while(1){

			  if(HAL_OK == HAL_DMA_PollForTransfer(&hdma_spi1_tx, HAL_DMA_FULL_TRANSFER, 1000)){
				  hspi1.State = HAL_SPI_STATE_READY;
				  break;
			  }
		  }
	  }

	  LCD_CS_SET;
}
#endif

void drawGraph(float *data, int32_t len, float dt)
{
	int32_t axis_px, axis_py;
	int32_t px, py, old_px, old_py;
	int8_t div_info[128];
	uint16_t color_idx;


	LCD_Clear2(1);

	// Draw graph area window
	//POINT_COLOR = WHITE;
	color_idx = 0;

	LCD_DrawLine2(ORIGIN_PX, ORIGIN_PY - MAX_RANGE_PY , ORIGIN_PX, ORIGIN_PY, color_idx);
	LCD_DrawLine2(ORIGIN_PX + MAX_RANGE_PX, ORIGIN_PY - MAX_RANGE_PY , ORIGIN_PX + MAX_RANGE_PX, ORIGIN_PY, color_idx);
	LCD_DrawLine2(ORIGIN_PX, ORIGIN_PY - MAX_RANGE_PY , ORIGIN_PX + MAX_RANGE_PX, ORIGIN_PY - MAX_RANGE_PY, color_idx);
	LCD_DrawLine2(ORIGIN_PX, ORIGIN_PY , ORIGIN_PX + MAX_RANGE_PX, ORIGIN_PY, color_idx);


	// Draw gridline
	//POINT_COLOR = GRAYBLUE;
	color_idx = 16;

	for(int i=1; i<10; i++){
		LCD_DrawLine2(ORIGIN_PX, ORIGIN_PY - MAX_RANGE_PY + DIV_PY * i, ORIGIN_PX + MAX_RANGE_PX, ORIGIN_PY - MAX_RANGE_PY + DIV_PY * i, color_idx);
	}

	for(int i=1; i<14; i++){
		LCD_DrawLine2(ORIGIN_PX + DIV_PX * i, ORIGIN_PY - MAX_RANGE_PY , ORIGIN_PX + DIV_PX * i, ORIGIN_PY, color_idx);
	}


	sprintf(div_info,"%0.2f V/div   %0.2f ms/div", DIV_VOLT, DIV_TIME);
	//Gui_StrCenter(30, 220, WHITE, BLACK, div_info, 16, 1);

	if(len > 0){
		// Draw data point
		//POINT_COLOR = YELLOW;
		color_idx = 10;

		axis_px = PX_PER_TIME * dt * 0;
		axis_py = PY_PER_VOLT * data[0];
		old_px = ORIGIN_PX + axis_px;
		old_py = ZERO_V_PY - axis_py;

		for(int i=1; i<len; i++){
			axis_px = PX_PER_TIME * dt * i;
			axis_py = PY_PER_VOLT * data[i];
			px = ORIGIN_PX + axis_px;
			py = ZERO_V_PY - axis_py;
			if((px <= ORIGIN_PX + MAX_RANGE_PX) && (ORIGIN_PY - MAX_RANGE_PY <= py) && (py <= ORIGIN_PY)){
				//LCD_DrawPoint(px_x, px_y);
				LCD_DrawLine2(old_px, old_py , px, py, color_idx);
				old_px = px;
				old_py = py;

			}
		}
	}
	else{
		//Gui_StrCenter(30, 120, BLACK, YELLOW, "Running", 16, 1);
	}

	// Draw 0V marker
	LCD_DrawLine2(ORIGIN_PX, ZERO_V_PY, ORIGIN_PX - 8, ZERO_V_PY - 4, color_idx);
	LCD_DrawLine2(ORIGIN_PX - 8, ZERO_V_PY - 4, ORIGIN_PX - 8, ZERO_V_PY + 4, color_idx);
	LCD_DrawLine2(ORIGIN_PX - 8, ZERO_V_PY + 4, ORIGIN_PX, ZERO_V_PY, color_idx);

	LCD_SendBuffer(&hspi1, &hdma_spi1_tx, &htim3);

}

void showAdcData(uint32_t ndtr, uint32_t offset)
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


void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
	printf("B1 pin is pushed (main.c)\n");

	/* Set AWDEN bits */
	hadc1.Instance->CR1 |=  ADC_CR1_AWDIE;

	/* Start timer for ADC trigger */
	htim3.Instance->CR1 |= TIM_CR1_CEN;

	g_is_trig_stop = FALSE;

}

void HAL_ADC_LevelOutOfWindowCallback(ADC_HandleTypeDef* hadc)
{
	trig_ndtr = hdma_adc1.Instance->NDTR;

	// 1ms(200点)後にサンプリングを停止するためのタイマ�???��?��をスター??��?��?
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
	g_is_trig_stop = TRUE;
	//showAdcData(hdma_adc1.Instance->NDTR, SHOW_SIZE);

	//showAdcData(trig_ndtr, 50);

#if 0
	for(int i=0; i<ADC_BUF_SIZE; i++){
		adc_phy[i] = adc_dr_dma[i] * 3.3 / 4096;
	}
	drawGraph(adc_phy, ADC_BUF_SIZE, 0.005);
#endif
}


/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
