/* USER CODE BEGIN Header */
/**
	******************************************************************************
	* @file           : main.c
	* @brief          : Main program body
	******************************************************************************
	* @attention
	*
	* Copyright (c) 2025 STMicroelectronics.
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
#include "fatfs.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include <math.h>
#include <string.h>
#include <stdbool.h>
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
SPI_HandleTypeDef hspi2;

TIM_HandleTypeDef htim1;
TIM_HandleTypeDef htim2;

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_SPI2_Init(void);
static void MX_TIM1_Init(void);
static void MX_TIM2_Init(void);
/* USER CODE BEGIN PFP */

/*		main functions		*/
static void preparation();
static WRESULT testMC(SIZESmm papersize);
static WRESULT machine_write(const char* filename,
	const SIZESmm papersize, 
	const char* writemode);

/*		image proccessing		*/
extern Coords* new_start_points(FRESULT* FR_Status, const char* filename, uint16_t* stpts_num);
extern void delete_start_points(Coords* start_points);
extern uint8_t** new_pixel_matrix(void);
extern void delete_pixel_matrix(uint8_t** pixel_matrix);
extern FRESULT read_BMPpart(uint8_t** pixel_matrix,
	const Coords start_point, 
	const char* filename,
	Quality* BMPquality);
extern FRESULT fout_start_points(Coords* start_points,
	const uint16_t stpts_num,
	const char* filename,
	const BYTE writemode);
extern FRESULT fout_pixel_matrix(uint8_t** pixel_matrix,
	const Coords start_points,
	const char* filename, 
	const BYTE writemode);

/*		machine control		*/
extern WRESULT rotate(const M_Angles rot_angles,
	M_Angles* start_angles,
	TIM_HandleTypeDef* htimx);

extern SIZESmm find_sheetsizes(const SIZESmm sheetsize, const char* sizetype);
extern M_Angles find_angles(const SIZESmm sheetsize,
	const M_Angles* start_angles,
	const Quality* BMPquality,
	const Square* SquareforWriting,
	const Coords* start_points, 
	const Coords* pixel_matrix_coords);
extern FRESULT save_start_angles(const M_Angles* start_angles, const char* filename);
extern FRESULT get_start_angles(M_Angles* start_angles, const char* filename);

extern WRESULT pen_up();
extern WRESULT pen_down();
/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */


/* USER CODE END 0 */

/**
	* @brief  The application entry point.
	* @retval int
	*/
int main(void)
{
	/* USER CODE BEGIN 1 */
	
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
	MX_SPI2_Init();
	MX_FATFS_Init();
	MX_TIM1_Init();
	MX_TIM2_Init();
	/* USER CODE BEGIN 2 */
	preparation();
	WRESULT MW_Status = machine_write("eyes.bmp", A4, "");
	// MW_Status = machine_write("8x8.bmp", A4, "continue");
	// delay_us(300000, &htim1);
	// testMC(A4);
	/* USER CODE END 2 */

	/* Infinite loop */
	/* USER CODE BEGIN WHILE */
	while (1)
	{
	/* USER CODE END WHILE */
	
	/* USER CODE BEGIN 3 */
		int b = 0;
		//pen_up();
		//pen_down();

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

	/** Initializes the RCC Oscillators according to the specified parameters
	* in the RCC_OscInitTypeDef structure.
	*/
	RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
	RCC_OscInitStruct.HSEState = RCC_HSE_ON;
	RCC_OscInitStruct.HSEPredivValue = RCC_HSE_PREDIV_DIV1;
	RCC_OscInitStruct.HSIState = RCC_HSI_ON;
	RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
	RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
	RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL9;
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
	* @brief SPI2 Initialization Function
	* @param None
	* @retval None
	*/
static void MX_SPI2_Init(void)
{

	/* USER CODE BEGIN SPI2_Init 0 */

	/* USER CODE END SPI2_Init 0 */

	/* USER CODE BEGIN SPI2_Init 1 */

	/* USER CODE END SPI2_Init 1 */
	/* SPI2 parameter configuration*/
	hspi2.Instance = SPI2;
	hspi2.Init.Mode = SPI_MODE_MASTER;
	hspi2.Init.Direction = SPI_DIRECTION_2LINES;
	hspi2.Init.DataSize = SPI_DATASIZE_8BIT;
	hspi2.Init.CLKPolarity = SPI_POLARITY_LOW;
	hspi2.Init.CLKPhase = SPI_PHASE_1EDGE;
	hspi2.Init.NSS = SPI_NSS_SOFT;
	hspi2.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_8;
	hspi2.Init.FirstBit = SPI_FIRSTBIT_MSB;
	hspi2.Init.TIMode = SPI_TIMODE_DISABLE;
	hspi2.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
	hspi2.Init.CRCPolynomial = 10;
	if (HAL_SPI_Init(&hspi2) != HAL_OK)
	{
	Error_Handler();
	}
	/* USER CODE BEGIN SPI2_Init 2 */

	/* USER CODE END SPI2_Init 2 */

}

/**
	* @brief TIM1 Initialization Function
	* @param None
	* @retval None
	*/
static void MX_TIM1_Init(void)
{

	/* USER CODE BEGIN TIM1_Init 0 */

	/* USER CODE END TIM1_Init 0 */

	TIM_ClockConfigTypeDef sClockSourceConfig = {0};
	TIM_MasterConfigTypeDef sMasterConfig = {0};

	/* USER CODE BEGIN TIM1_Init 1 */

	/* USER CODE END TIM1_Init 1 */
	htim1.Instance = TIM1;
	htim1.Init.Prescaler = 72;
	htim1.Init.CounterMode = TIM_COUNTERMODE_UP;
	htim1.Init.Period = 65535;
	htim1.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
	htim1.Init.RepetitionCounter = 0;
	htim1.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
	if (HAL_TIM_Base_Init(&htim1) != HAL_OK)
	{
	Error_Handler();
	}
	sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
	if (HAL_TIM_ConfigClockSource(&htim1, &sClockSourceConfig) != HAL_OK)
	{
	Error_Handler();
	}
	sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
	sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
	if (HAL_TIMEx_MasterConfigSynchronization(&htim1, &sMasterConfig) != HAL_OK)
	{
	Error_Handler();
	}
	/* USER CODE BEGIN TIM1_Init 2 */

	/* USER CODE END TIM1_Init 2 */

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

	TIM_ClockConfigTypeDef sClockSourceConfig = {0};
	TIM_MasterConfigTypeDef sMasterConfig = {0};
	TIM_OC_InitTypeDef sConfigOC = {0};

	/* USER CODE BEGIN TIM2_Init 1 */

	/* USER CODE END TIM2_Init 1 */
	htim2.Instance = TIM2;
	htim2.Init.Prescaler = 71;
	htim2.Init.CounterMode = TIM_COUNTERMODE_UP;
	htim2.Init.Period = 9999;
	htim2.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
	htim2.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
	if (HAL_TIM_Base_Init(&htim2) != HAL_OK)
	{
	Error_Handler();
	}
	sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
	if (HAL_TIM_ConfigClockSource(&htim2, &sClockSourceConfig) != HAL_OK)
	{
	Error_Handler();
	}
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
	sConfigOC.Pulse = 0;
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
	* @brief GPIO Initialization Function
	* @param None
	* @retval None
	*/
static void MX_GPIO_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStruct = {0};

	/* GPIO Ports Clock Enable */
	__HAL_RCC_GPIOD_CLK_ENABLE();
	__HAL_RCC_GPIOA_CLK_ENABLE();
	__HAL_RCC_GPIOB_CLK_ENABLE();

	/*Configure GPIO pin Output Level */
	HAL_GPIO_WritePin(GPIOB, SD_CS_Pin|SM_PUL_Pin, GPIO_PIN_RESET);

	/*Configure GPIO pin Output Level */
	HAL_GPIO_WritePin(GPIOA, SM_DIR_Pin|SM_ENA_Pin|CM_PUL_Pin|CM_DIR_Pin
							|CM_ENA_Pin, GPIO_PIN_RESET);

	/*Configure GPIO pins : SD_CS_Pin SM_PUL_Pin */
	GPIO_InitStruct.Pin = SD_CS_Pin|SM_PUL_Pin;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
	HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

	/*Configure GPIO pins : SM_DIR_Pin SM_ENA_Pin CM_PUL_Pin CM_DIR_Pin
							CM_ENA_Pin */
	GPIO_InitStruct.Pin = SM_DIR_Pin|SM_ENA_Pin|CM_PUL_Pin|CM_DIR_Pin
							|CM_ENA_Pin;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
	HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

}

/* USER CODE BEGIN 4 */
static void preparation()
{
	HAL_GPIO_WritePin(CM_ENA_GPIO_Port, CM_ENA_Pin, 1);
	HAL_Delay(500);
	HAL_GPIO_WritePin(CM_ENA_GPIO_Port, CM_ENA_Pin, 0);
	
	HAL_GPIO_WritePin(SM_ENA_GPIO_Port, SM_ENA_Pin, 1);
	HAL_Delay(500);
	HAL_GPIO_WritePin(SM_ENA_GPIO_Port, SM_ENA_Pin, 0);

	HAL_TIM_Base_Start(&htim1);
	
	HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_3);
}


static WRESULT testMC(SIZESmm papersize)
{	
	const SIZESmm framesize = 5;
	const Square SquareforWriting = { 
		framesize,
		framesize,
		find_sheetsizes(papersize, "width") - framesize,
		find_sheetsizes(papersize, "height") - framesize
	};

	const int stpts_num = 5;
	Coords start_points[stpts_num]; 
	start_points[0] = (Coords){ 50, 50 };
	start_points[1] = (Coords){ 50, 150 };
	start_points[2] = (Coords){ 150, 150 };
	start_points[3] = (Coords){ 150, 50 };
	start_points[4] = (Coords){ 50, 50 };
	
	Quality BMPquality = { SquareforWriting.max_y, SquareforWriting.max_x };
	Coords pixel_matrix_coords = { 0, 0 };
	M_Angles start_angles = { 0.0, 0.0 };
	
	double R = 80;
	
	while (1)
	{
		for (int k = 0; k < 7; ++k)
		{
			
			for (double i = 0; i < 2 * M_PI; i += 0.2)
			{
				Coords coords = { 2 * 4 * M_PI * k + 10 + i * 4, 70 + 25 * sin(i) };
		
				M_Angles rot_angles = find_angles(papersize,
					&start_angles,
					&BMPquality,
					&SquareforWriting,
					&coords,
					&pixel_matrix_coords);
				if (rot_angles.CM == Angle_Err) return Angle_Err;
			
				WRESULT MC_Status = rotate(rot_angles, &start_angles, &htim1);
				if (MC_Status != MC_OK) return MC_Status;
			}
		}
	}
	
	return MC_OK;
}

static WRESULT machine_write(const char* filename, const SIZESmm papersize, const char* writemode)
{
	/*--------------------INIT--------------------*/
	FATFS FatFs; // for sd card where images stored
	FRESULT FR_Status;
	FR_Status = f_mount(&FatFs, "", 1);

	if (FR_Status != FR_OK) return SD_Err;
	
	const char* image_info = "image_info.txt"; // for output pixel_matrix and start_points
	const char* start_angles_info = "start_angles_info.bin"; // to memorize angles
	
	// divide the image into pieces (buffers) with start points (upper left point of square on sheet) of every piece 
	uint16_t stpts_num = 0;
	Coords* start_points = new_start_points(&FR_Status, filename, &stpts_num);
	if (start_points == NULL) return Mem_Err;
	if (FR_Status != FR_OK) return File_Err;
	
	FR_Status = fout_start_points(start_points, stpts_num, image_info, FA_CREATE_ALWAYS);
	if (FR_Status != FR_OK) return File_Err;
	
	M_Angles start_angles = { 0.0, 0.0 }; 
	if (!strcmp(writemode, "continue")) { // get start angles from file
		FR_Status = get_start_angles(&start_angles, start_angles_info);
		if (FR_Status != FR_OK) return File_Err;
	}
	
	// square on sheet where image will be written
	const SIZESmm framesize = 5; 
	const Square SquareforWriting = { 
		//.min_x = framesize, 
		//.min_y = framesize, 
		//.max_x = find_sheetsizes(A4, "width") - framesize,
		//.max_y = find_sheetsizes(A4, "height") - framesize
		.min_x = framesize,
		.min_y = find_sheetsizes(A4, "height") - framesize - 230,
		.max_x = framesize + 160,
		.max_y = find_sheetsizes(A4, "height") - framesize
	};
	
	/*--------------------WRITING--------------------*/
	for (uint16_t t = 0; t < stpts_num; ++t)
	{
		Quality BMPquality = { 0, 0 };
		uint8_t** pixel_matrix = new_pixel_matrix();
		if (pixel_matrix == NULL) return Mem_Err;
		
		FR_Status = read_BMPpart(pixel_matrix, start_points[t], filename, &BMPquality);
		if (FR_Status != FR_OK) return File_Err;
		
		FR_Status = fout_pixel_matrix(pixel_matrix, start_points[t], image_info, FA_OPEN_ALWAYS);
		if (FR_Status != FR_OK) return File_Err;
		
		WRESULT pen_cond = pen_up(); // pen up before starting
		
		for (uint16_t i = 0; i < BUFheight; ++i)
		{
			for (uint16_t j = 0; j < BUFwidth; ++j)
			{
				if (j == 0 || pixel_matrix[i][j] == WHITE)
				{
					if (pen_cond == Pen_Down) pen_cond = pen_up();
				}
				if (pixel_matrix[i][j] != WHITE)
				{
					Coords pixel_matrix_coords = { j, i };
					M_Angles rot_angles = find_angles(papersize,
						&start_angles,
						&BMPquality,
						&SquareforWriting,
						&start_points[t],
						&pixel_matrix_coords);
					if (rot_angles.CM == Angle_Err) return Angle_Err;
					
					// so long with rotate...
					WRESULT MC_Status = rotate(rot_angles, &start_angles, &htim1);
					if (MC_Status != MC_OK) return MC_Status;
					
					if (pen_cond == Pen_Up) pen_cond = pen_down();
				}
				// if (pen_cond == Pen_Down) pen_cond = pen_up();
			}
		}
		delete_pixel_matrix(pixel_matrix);
	}
	delete_start_points(start_points);
 
	/*--------------------END--------------------*/
	FR_Status = save_start_angles(&start_angles, start_angles_info);
	if (FR_Status != FR_OK) return File_Err;
	
	FR_Status = f_mount(NULL, "", 0);
	if (FR_Status != FR_OK) return SD_Err;
	
	return W_OK;
};
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
