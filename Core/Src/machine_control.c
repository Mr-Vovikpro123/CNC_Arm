#include <math.h>
#include <string.h>
#include "fatfs.h"
#include "main.h"

const double step_angle = 1.8;
typedef enum {
	CM_driver_microstep = 16,
	CM_reductor_ratio   = 3,
	CM_min_rotperiod    = 1500,
	CM_max_rotperiod    = 2500,
	CM_max_stepnum      = 200 * CM_driver_microstep
} CentralMotor_Data;
const double CM_real_step_angle = step_angle / (CM_driver_microstep * CM_reductor_ratio);
typedef enum {
	SM_driver_microstep = 32,
	SM_reductor_ratio   = 1,
	SM_min_rotperiod    = 1500,
	SM_max_rotperiod    = 2500,
	SM_max_stepnum      = 200 * SM_driver_microstep
} ShoulderMotor_Data;
const double SM_real_step_angle = step_angle / (SM_driver_microstep * SM_reductor_ratio);
typedef enum { cw = 1, ccw = 0 } rot_dir;


/*-----------------FUNCTIONS FOR ROTATE MOTORS-----------------*/
static double bounded_parabola(double x, Square square) /* return x^2 in square (square.min_x,square.max_x) X (square.min_y,square.max_y) */
{
	/* (x0, y0) - central minimum point */
	double x0 = (double)(square.max_x + square.min_x) / 2;
	double y0 = square.min_y;
	double k = (square.max_y - y0) / pow((square.min_x - x0), 2);
	
	return k*pow((x - x0), 2) + y0;
}

static double mirror_exp(double x, Square square) /* return mirror exp(x) in square (square.min_x,square.max_x) X (square.min_y,square.max_y) */
{
	double x0 = (double)(square.max_x + square.min_x) / 2;
	double y0 = square.min_y - 1;
	double k = log(square.max_y - y0) / (square.min_x - x0);
	
	if (x <= x0) 
		return exp(k*(x - x0)) + y0;
	else 
		return exp(k*((square.min_x + square.max_x - x) - x0)) + y0;
};

void delay_us(const uint32_t us, TIM_HandleTypeDef* htimx) // us - micro seconds
{
	__HAL_TIM_SET_COUNTER(htimx, 0);
	while (__HAL_TIM_GET_COUNTER(htimx) < us) ;
}

void step_pulse(uint16_t pulseperiod_us,
	GPIO_TypeDef* GPIOx,
	uint16_t GPIO_Pin,
	TIM_HandleTypeDef* htimx) // motor one step lasting pulseperiod_us
{
	HAL_GPIO_WritePin(GPIOx, GPIO_Pin, GPIO_PIN_SET);
	delay_us(pulseperiod_us - pulseperiod_us / 2, htimx);
	
	HAL_GPIO_WritePin(GPIOx, GPIO_Pin, GPIO_PIN_RESET);
	delay_us(pulseperiod_us / 2, htimx);
}

WRESULT rotate(const M_Angles rot_angles,
	M_Angles* start_angles,
	TIM_HandleTypeDef* htimx) 
{	
	uint8_t CM_rot_dir = (rot_angles.CM < 0) ? ccw : cw;
	uint16_t CM_steps_num = round(fabs(rot_angles.CM) / CM_real_step_angle);
	double CM_real_rot_angle = CM_steps_num * CM_real_step_angle * (CM_rot_dir * 2 - 1);
	double temp = CM_real_step_angle;
	double test_1 = CM_rot_dir * 2 - 1;
	
	uint8_t SM_rot_dir = (rot_angles.SM < 0) ? ccw : cw;
	uint16_t SM_steps_num = round(fabs(rot_angles.SM) / SM_real_step_angle);
	double SM_real_rot_angle = SM_steps_num * SM_real_step_angle * (SM_rot_dir * 2 - 1);
	
	start_angles->CM += rot_angles.CM;
	start_angles->SM += rot_angles.SM;
	// add error of round() to start_angles: 
	start_angles->CM += CM_real_rot_angle - rot_angles.CM;
	start_angles->SM += SM_real_rot_angle - rot_angles.SM;
	// check angles
	if (start_angles->CM < -180 || start_angles->CM > 0 ||
		start_angles->SM < 0 || start_angles->SM > 180)
		return Angle_Err;
	
	HAL_GPIO_WritePin(CM_DIR_GPIO_Port, CM_DIR_Pin, CM_rot_dir); // set rotation direction
	HAL_GPIO_WritePin(SM_DIR_GPIO_Port, SM_DIR_Pin, SM_rot_dir);

	Square toBoundParabola = { // for safety parabolla (func argument: work_time; func value: pulseperiod)
		.min_x = 0,
		.min_y = max(CM_min_rotperiod, SM_min_rotperiod),
		.max_x = max(CM_steps_num, SM_steps_num),
		.max_y = min(CM_max_rotperiod, SM_max_rotperiod)

	};
	double CM_work_time = 0; // total work time of central motor
	double SM_work_time = 0;	
	uint16_t CM_i = 0, SM_i = 0; // current steps num of each motor
	double CM_R = 1, SM_R = 1; // ratio of num of motor steps
	if (CM_steps_num == 0) CM_work_time = INFINITY; // to only SM working
	else if (SM_steps_num > CM_steps_num) // to get a wide parabolla
		CM_R = (double)SM_steps_num / CM_steps_num;
	
	if (SM_steps_num == 0) SM_work_time = INFINITY;
	else if (CM_steps_num > SM_steps_num)
		SM_R = (double)CM_steps_num / SM_steps_num;
	
	while (SM_i < SM_steps_num || CM_i < CM_steps_num) // rotate
	{
		if (CM_work_time < SM_work_time)
		{
			double CM_pulseperiod = bounded_parabola(CM_i*CM_R, toBoundParabola);
			step_pulse((uint16_t)CM_pulseperiod, CM_PUL_GPIO_Port, CM_PUL_Pin, htimx);
			CM_work_time += (CM_pulseperiod*CM_R);
			CM_i++;
		}
		else
		{
			double SM_pulseperiod = bounded_parabola(SM_i*SM_R, toBoundParabola);
			step_pulse((uint16_t)SM_pulseperiod, SM_PUL_GPIO_Port, SM_PUL_Pin, htimx);
			SM_work_time += (SM_pulseperiod*SM_R); 
			SM_i++;
		}
	}
	
	return MC_OK;
}


/*-----------------FUNCTIONS FOR MOTORS ROTATION ANGLES-----------------*/
/*				CALCULATION findAngles()
				
									sheetSqforWriting.dx
										   <------>
									  sheetX
									  <----->
							 			<---widthsheet-->
				 						----------------- ^
				^					 	|  --------		| |
	^			|					 	|  |.	  |		| | 
	|	sheetSqforWriting.dy	^	    |  / 	  |		| |
	|			|				|	 	|/ |	  |		| |
	|			v				|	   /|  --------		| |
	|		arm_length*sin(beta) |	 /	|			    | |heightsheet
	|							v	O	|				| |
 sheetY						^	|	|				| |
	|							|	|   |				| |
	|							|	|   |				| |
	|		arm_length*sin(alpha)|	|   ----------------- v
	|							|	\	^
	v							v	 O  v CMtosheet_height	
									 <->
								CMtosheet_width
SYMBOLS:
arm_length  - arm length
sheetX - length from CM to pencil on x-projection
sheetY - length from CM to pencil on y-projection
near_pts_distX - distance between adjacent points on x axis
near_pts_distY - distance between adjacent points on y axis
angles.CM  - rotation angle of central motor
angles.SM  - rotation angle of shoulder motor

FORMULAS:
near_pts_distX = (sheetSqforWriting.max_x - sheetSqforWriting.min_x) / BMPquality.width
near_pts_distY = (sheetSqforWriting.max_y - sheetSqforWriting.min_y) / BMPquality.height

sheetX = CMtosheet_width + PapsheetSqforWriting.min_x + 
	(start_points.x + pixel_matrix_coords.x) * near_pts_distX
sheetY = CMtosheet_height + sheetSqforWriting.max_y -
	(start_points.y + pixel_matrix_coords.y) * near_pts_distY

sheetY = arm_length*sin(alpha) + arm_length*sin(beta)
sheetX = arm_length*cos(alpha) + arm_length*cos(beta)

Solve in "Solution_SystemOfEquations_for_findAngles().jpg":
alpha = atan(sheetY / sheetX) + acos((sqrt(pow(sheetX, 2) + pow(sheetY, 2)) / (2*arm_length)));
beta = atan(sheetY / sheetX) - acos((sqrt(pow(sheetX, 2) + pow(sheetY, 2)) / (2*arm_length)));
*/
SIZESmm find_sheetsizes(const SIZESmm sheetsize, const char* sizetype)
{
	uint16_t widthsheet = 0, heightsheet = 0;
	switch (sheetsize) {
	case A4:
		widthsheet = A4w;
		heightsheet = A4h;
		break;
	case A4album:
		widthsheet = A4h; 
		heightsheet = A4w;
		break;
	case A3:
		widthsheet = A3w; 
		heightsheet = A3h;
		break;
	case A3album:
		widthsheet = A3h; 
		heightsheet = A3w;
		break;
	default: return 0;
	}
	if (!strcmp(sizetype, "width")) return widthsheet;
	else if (!strcmp(sizetype, "height")) return heightsheet;
	return 0;
}

M_Angles find_angles(const SIZESmm sheetsize,
	const M_Angles* start_angles,
	const Quality* BMPquality,
	const Square* SquareforWriting,
	const Coords* start_points, 
	const Coords* pixel_matrix_coords)
{
	if (BMPquality->width <= 0 || BMPquality->height <= 0)
		return (M_Angles) { Angle_Err, Angle_Err };
	
	uint16_t widthsheet = find_sheetsizes(sheetsize, "width");
	uint16_t heightsheet = find_sheetsizes(sheetsize, "height");
	
	// distance between adjacent points on x,y axis
	double near_pts_distX = (SquareforWriting->max_x - SquareforWriting->min_x) / BMPquality->width; 
	double near_pts_distY = (SquareforWriting->max_y - SquareforWriting->min_y) / BMPquality->height;

	// length from CM to pencil on x,y-projection
	double sheetX = CMtoSheet_width + SquareforWriting->min_x + 
		(start_points->x + pixel_matrix_coords->x) * near_pts_distX; 
	double sheetY = CMtoSheet_height + SquareforWriting->max_y -
		(start_points->y + pixel_matrix_coords->y) * near_pts_distY;
	
	if (sheetX < CMtoSheet_width || sheetY < CMtoSheet_height || 
		sheetX > CMtoSheet_width + widthsheet ||
		sheetY > CMtoSheet_height + heightsheet) // else hand will go outside of sheet
		return (M_Angles) { Angle_Err, Angle_Err };
	
	double dist_to_pencil = sqrt(pow(sheetX, 2) + pow(sheetY, 2));
	if (dist_to_pencil > 2*arm_length) // else hand won't reach
		return (M_Angles){ Angle_Err, Angle_Err };
	
	// the angle of the motor relative to the angular position (0,0) 
	double alpha = (180 / M_PI)*(atan(sheetY / sheetX) + 
		acos(dist_to_pencil / (2*arm_length)));
	double beta = (180 / M_PI)*(atan(sheetY / sheetX) - 
		acos(dist_to_pencil / (2*arm_length)));
	
	M_Angles rot_angles;
	rot_angles.CM = -alpha - start_angles->CM;
	rot_angles.SM = (alpha - beta) - start_angles->SM;
	
	return rot_angles;
}

FRESULT save_start_angles(const M_Angles* start_angles, const char* filename)
{
	FIL Fil;
	FRESULT FR_Status;
	UINT WWC;
	
	FR_Status = f_open(&Fil, filename, FA_WRITE | FA_CREATE_ALWAYS);
	if (FR_Status != FR_OK) return FR_Status;
	
	f_write(&Fil, start_angles, sizeof(*start_angles), &WWC);
	f_close(&Fil);
	
	return FR_Status;
}

FRESULT get_start_angles(M_Angles* start_angles, const char* filename)
{
	FIL Fil;
	FRESULT FR_Status;
	UINT RWC;
	
	FR_Status = f_open(&Fil, filename, FA_READ);
	if (FR_Status != FR_OK) return FR_Status;
	
	f_read(&Fil, start_angles, sizeof(*start_angles), &RWC);
	f_close(&Fil);
	
	return FR_Status;
}

/*-----------------FUCTIONS FOR SERVO DRIVE-----------------*/
WRESULT pen_down()
{
	HAL_Delay(500);
	TIM2->CCR3 = 500;
	
	return Pen_Down;
}

WRESULT pen_up()
{
	HAL_Delay(500);
	TIM2->CCR3 = 700;
	
	return Pen_Up;
}

/*-----------------FUNCTIONS FOR ROTATE TO START POSITION-----------------*/

