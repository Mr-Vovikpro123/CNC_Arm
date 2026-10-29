#include "main.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "fatfs.h"
#include "bmp_info.h"

Coords* new_start_points(FRESULT* FR_Status, const char* filename, uint16_t* stpts_num)
{
	BMPHEADER BMPheader;
	DIBHEADER BMPinfo;
	
	FIL Fil;
	UINT RWC, WWC;
	*FR_Status = f_open(&Fil, filename, FA_READ);
	if (*FR_Status != FR_OK) return NULL;
	
	f_read(&Fil, &BMPheader, sizeof(BMPHEADER), &RWC);
	f_read(&Fil, &BMPinfo, sizeof(DIBHEADER), &RWC);
	f_close(&Fil);
	
	if (BMPheader.ID[0] != 'B' || BMPheader.ID[1] != 'M')
		return NULL;

	const uint16_t startX_num = (BMPinfo.width - 1) / BUFwidth + 1;
	const uint16_t startY_num = (BMPinfo.height - 1) / BUFheight + 1;
	*stpts_num = startX_num * startY_num;
	Coords* start_points = (Coords*)malloc(*stpts_num * sizeof(Coords));
	if (start_points == NULL) return NULL;

	for (uint16_t t = 0, i = 0; i < startY_num; ++i)
	{
		for (uint16_t j = 0; j < startX_num; ++j, ++t)
		{
			start_points[t].x = j * BUFwidth;
			start_points[t].y = i * BUFheight;
		}
	}
	
	return start_points;
}

void delete_start_points(Coords* start_points)
{
	if (start_points == NULL) return;
	free(start_points);
}

uint8_t** new_pixel_matrix(void)
{
	if (BUFheight * BUFwidth > 10000) return NULL;
	
	uint8_t** pixel_matrix = (uint8_t**)malloc(BUFheight * sizeof(uint8_t*));
	if (pixel_matrix == NULL) return NULL; // Memory allocation failed

	for (uint16_t i = 0; i < BUFheight; ++i)
	{
		pixel_matrix[i] = (uint8_t*)malloc(BUFwidth * sizeof(uint8_t));
		if (pixel_matrix[i] == NULL) // Memory allocation failed for rows
		{
			for (uint16_t j = 0; j < i; ++j)
				free(pixel_matrix[j]);
			free(pixel_matrix);
			return NULL;
		}
		for (uint16_t j = 0; j < BUFwidth; ++j) {
			pixel_matrix[i][j] = WHITE;
		}
	}
	return pixel_matrix;
}

void delete_pixel_matrix(uint8_t** pixel_matrix)
{
	if (pixel_matrix == NULL) return;
	for (uint16_t i = 0; i < BUFheight; ++i)
		free(pixel_matrix[i]);
	free(pixel_matrix);
}

FRESULT read_BMPpart(uint8_t** pixel_matrix,
	const Coords start_point, 
	const char* filename,
	Quality* BMPquality)
{
	if (pixel_matrix == NULL) return FR_INVALID_PARAMETER; // Memory alloc failed

	FIL Fil;
	FRESULT FR_Status;
	UINT RWC, WWC;
	FR_Status = f_open(&Fil, filename, FA_READ);
	if (FR_Status != FR_OK) return FR_Status; // file open failed
	
	BMPHEADER BMPheader;
	DIBHEADER BMPinfo;
	f_read(&Fil, &BMPheader, sizeof(BMPHEADER), &RWC); // fill BMPheader with first bytes in image
	f_read(&Fil, &BMPinfo, sizeof(DIBHEADER), &RWC); // fill BMPinfo
	
	if (BMPheader.ID[0] != 'B' || BMPheader.ID[1] != 'M') { // if image file not .bmp
		f_close(&Fil);
		return FR_INVALID_OBJECT;
	}

	BMPquality->width = BMPinfo.width; // fill for find_angles()
	BMPquality->height = BMPinfo.height;

	const uint8_t bytes_per_pixel = BMPinfo.bits_per_pixel / 8;
	const uint8_t padding_bytes = (4 - (BMPinfo.width * bytes_per_pixel) % 4) % 4; /* If the string size in bytes is not a multiple of 4,
																						'null bytes' are added to the end */
	uint32_t start_byte = (BMPinfo.height - BUFheight - start_point.y) * 
		(BMPinfo.width * bytes_per_pixel + padding_bytes) + (start_point.x * bytes_per_pixel); // byte (pixel) from which image starts read
	
	if (start_byte >= 0) // start_byte<0 means BUFquality is several times greater BMPquality
	{
		for (int i = BUFheight - 1; i >= 0; --i) // cycle works in reverse because the information about the lower pixels comes first 
		{
			FR_Status = f_lseek(&Fil, BMPheader.pixel_offset + start_byte); // pixel_offset is place where pixels data starts
			if (FR_Status != FR_OK) return FR_Status; // cursor not moved
		
			for (int j = 0; j < BUFwidth; ++j)
			{
				if (start_point.x + j < BMPinfo.width && start_point.y + i < BMPinfo.height)
				{
					uint8_t pixel_data[4] = { WHITE, WHITE, WHITE, WHITE };
					f_read(&Fil, pixel_data, bytes_per_pixel * sizeof(pixel_data[0]), &RWC);
					pixel_matrix[i][j] = (pixel_data[0] + pixel_data[1] + pixel_data[2]) / 3;
				}
			}
			start_byte += BMPinfo.width * bytes_per_pixel + padding_bytes; // move cursor on next pixels line
		}
	}
	f_close(&Fil);
	
	return FR_Status;
}


FRESULT fout_start_points(Coords* start_points, const uint16_t stpts_num, const char* filename, const BYTE writemode)
{
	FIL Fil;
	FRESULT FR_Status;
	char W_Buffer[256];
	UINT WWC;
	
	FR_Status = f_open(&Fil, filename, FA_WRITE | writemode);
	if (writemode == FA_OPEN_ALWAYS)
		FR_Status = f_lseek(&Fil, f_size(&Fil));
	if (FR_Status != FR_OK) return FR_Status;
	
	f_puts("start_points (x,y):\n", &Fil);
	
	for (uint16_t i = 0; i < stpts_num; ++i)
	{
		sprintf(W_Buffer, "#%d:\t(%d,%d)\n", i, start_points[i].x, start_points[i].y);
		f_write(&Fil, W_Buffer, strlen(W_Buffer), &WWC);
	}
	f_puts("\n", &Fil);
	f_close(&Fil);
	
	return FR_Status;
}

FRESULT fout_pixel_matrix(uint8_t** pixel_matrix, const Coords start_points, const char* filename, const BYTE writemode)
{
	FIL Fil;
	FRESULT FR_Status;
	char W_Buffer[256];
	UINT WWC;
	
	FR_Status = f_open(&Fil, filename, FA_WRITE | writemode);
	if (writemode == FA_OPEN_ALWAYS)
		FR_Status = f_lseek(&Fil, f_size(&Fil));
	if (FR_Status != FR_OK) return FR_Status;
	
	sprintf(W_Buffer, "pixel_matrix with start_points: (%d,%d)\n", start_points.x, start_points.y);
	f_write(&Fil, W_Buffer, strlen(W_Buffer), &WWC);

	for (uint16_t i = 0; i < BUFheight; ++i)
	{
		for (uint16_t j = 0; j < BUFwidth; ++j)
		{
			sprintf(W_Buffer, "%d\t", pixel_matrix[i][j]); 
			f_write(&Fil, W_Buffer, strlen(W_Buffer), &WWC);
		}
		f_puts("\n", &Fil);
	}
	f_puts("\n", &Fil);
	f_close(&Fil);
	
	return FR_Status;
}