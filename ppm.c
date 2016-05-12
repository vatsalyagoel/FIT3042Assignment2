#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <unistd.h>

#include <SDL.h>

#include "helpers.h"
#include "ppm.h"
#include "frame_manipulation.h"

PPMImage *read_ppm_data(FILE *ppm, float brightness_factor, float contrast_factor, float saturation_factor)
{
	PPMImage *img;
	char c;
	int row, col ,index = 0;
	img = (PPMImage *)ppm_malloc(sizeof(PPMImage));

	c = fgetc(ppm);
	if(c == 'P' || c == 'p')
	{
		img -> magic_number[0] = c;
		c = fgetc(ppm);
		if(c == '6')
		{
			img -> magic_number[1] = c;
			img -> magic_number[2] = '\0';
		}
		else
		{
			fprintf(stderr, "Image format P%c Not handled\n", c);
			exit(0);
		}
	}
	else
	{
		fprintf(stderr, "Image not in PPM format\n");
		exit(0);
	}

	skip_comment(ppm);
	fscanf(ppm, "%d", &(img -> width));
	skip_comment(ppm);
	fscanf(ppm, "%d", &(img -> height));
	skip_comment(ppm);
	fscanf(ppm, "%d", &(img -> max_rgb_val));
	fgetc(ppm); //skip new line
	img -> pixels = (PPMPixel*)ppm_malloc(img -> width * img -> height * sizeof(PPMPixel));

	for (row = img -> height - 1; row >= 0; row--)
	{
		for (col = 0; col < img -> width; col++)
		{
			img -> pixels[index].rgb = (RGB*)ppm_malloc(sizeof(RGB));

			img -> pixels[index].rgb -> red = (float)fgetc(ppm);
			img -> pixels[index].rgb -> green = (float)fgetc(ppm);
			img -> pixels[index].rgb -> blue = (float)fgetc(ppm);

			img -> pixels[index].hsl = RGBtoHSL(img -> pixels[index].rgb);

			alter_pixel(&(img -> pixels[index]), brightness_factor, contrast_factor, saturation_factor);
			index++;
		}
	}
	return img;
}

void write_ppm(PPMImage *img, char *path)
{
	FILE *ppm = fopen(path, "wb");
	int row, col ,index = 0;
	fprintf(ppm, "%s\n", img -> magic_number);
	fprintf(ppm, "%d %d\n", img -> width, img -> height);
	fprintf(ppm, "255\n");
	for (row = img -> height - 1; row >= 0; row--)
	{
		for (col = 0; col < img -> width; col++)
		{
			fprintf(ppm, "%c", (unsigned char)img -> pixels[index].rgb -> red);
			fprintf(ppm, "%c", (unsigned char)img -> pixels[index].rgb -> green);
			fprintf(ppm, "%c", (unsigned char)img -> pixels[index].rgb -> blue);
			index++;
		}
	}
}

void make_window(void* param)
{
	PPMImage *ppm_image = param;
	SDL_SetWindowSize(window, ppm_image -> width, ppm_image -> height);
	screen_surface = SDL_GetWindowSurface(window);
	int row, col ,index = 0;
	for (row = 0; row < ppm_image -> height; row++)
	{
		for (col = 0; col < ppm_image -> width; col++)
		{
			int *p = screen_surface -> pixels + row * screen_surface -> pitch + col * screen_surface -> format -> BytesPerPixel; 
			int r = ppm_image -> pixels[index].rgb -> red;
			int g = ppm_image -> pixels[index].rgb -> green;
			int b = ppm_image -> pixels[index].rgb -> blue;
			*p = SDL_MapRGB(screen_surface -> format, r, g, b);
			index++;
		}
	}
	SDL_UpdateWindowSurface(window);
}