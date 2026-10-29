#pragma once
#ifndef BMP_INFO_H
#define BMP_INFO_H

#pragma pack(push, 1) // alignment 1 byte

typedef struct // Structure for the BMP file header
{
	unsigned char   ID[2];          // 'BM'
	unsigned int    file_size;      // File size in bytes
	unsigned char   unused[4];      // Reserved
	unsigned int    pixel_offset;   // Offset to image data 
} BMPHEADER;

typedef struct // Structure for the BMP image information
{
	unsigned int    header_size;    // Size of the structure in bytes
	unsigned int    width;          // Image width in pixels
	unsigned int    height;         // Image height in pixels
	unsigned short  color_planes;   // Number of color planes (always 1)
	unsigned short  bits_per_pixel; // Color depth in bits per pixel
	unsigned int    compression;    // Compression type (0 - no compression)
	unsigned int    image_size;     // Size of image data in bytes
	unsigned int    pwidth;         // Horizontal resolution in pixels per meter
	unsigned int    pheight;        // Vertical resolution in pixels per meter
	unsigned int    colors_count;    // Number of colors used
	unsigned int    important_colors;// Number of important colors
} DIBHEADER;

#pragma pack(pop)

#endif