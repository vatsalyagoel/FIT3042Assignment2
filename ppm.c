#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include <SDL.h>

#include "ppm.h"

int DELAY;

int to_number(char *str_to_num)
{
	int num = atoi(str_to_num);

	if(num == 0 && (str_to_num[0] != '0' || str_to_num == NULL ))
	{
		fprintf(stderr, "%s\n", "Not a Number");
		exit(1);
	}

	return num;
}

float MIN(float a, float b, float c)
{
    float m = a;
    if (m > b) m = b;
    if (m > c) m = c;
    return m;
}
 
float MAX(float a, float b, float c)
{
    float m = a;
    if (m < b) m = b;
    if (m < c) m = c;
    return m;
}

float check_boundary(float val)
{
	if(val < 0)
	{
		return 0;
	}
	if(val > 255)
	{
		return 255;
	}
	return val;
}

void skip_comment(FILE *ppm)
{
	char buf[120]; // assumes at most 120 character comments
	
	while (1) {
	   fscanf(ppm, "%1s", buf);
	   if (buf[0] != '#') { ungetc(buf[0], ppm); return; }
	   else fgets(buf, 120, ppm); /* skip comment line */
	   //printf("Comment: %s\n", buf);
	}
}

HSL *RGBtoHSL(RGB *rgb)
{
	HSL *hsl;
	hsl = (HSL *)malloc(sizeof(HSL));

	float min, max, delta;

	float r1 = rgb -> red / 255.0;
	float g1 = rgb -> green / 255.0;
	float b1 = rgb -> blue / 255.0;

	min = MIN(r1, g1, b1);
	max = MAX(r1, g1, b1);

	delta = max - min;

	hsl -> hue = 0;
	hsl -> saturation = 0;
	hsl -> lightness = 0.5 * (min + max);

	if(delta != 0)
	{
		if( r1 == max )
		{
			hsl -> hue = fmod(((g1 - b1) / delta) , 6.0);
		}
		else if( g1 == max )
		{
			hsl -> hue = 2.0 + (b1 - r1) / delta;
		}
		else
		{
			hsl -> hue = 4.0 + (r1 - g1) / delta;
		}
		hsl -> hue *= 60;
		hsl -> saturation = delta / (1.0 - fabs(2.0 * hsl -> lightness - 1.0 ));
	}
	return hsl;
}

void HSLtoRGB (PPMPixel *ppm_pixel)
{
	double c = 0.0, m = 0.0, x = 0.0;

	c = (1.0 - fabs(2 * ppm_pixel -> hsl -> lightness - 1)) * ppm_pixel -> hsl -> saturation;
	m = 1.0 * (ppm_pixel -> hsl -> lightness - 0.5 * c);
	x = c * (1.0 - fabs(fmod( ppm_pixel -> hsl -> hue / 60.0, 2) - 1.0 ));

	float r1, g1, b1;

	if( ppm_pixel -> hsl -> hue >= 0 && ppm_pixel -> hsl -> hue <= ( HUE_UPPER_LIMIT/6.0))
	{
		r1 = c;
		g1 = x;
		b1 = 0;
	}
	else if( ppm_pixel -> hsl -> hue >= ( HUE_UPPER_LIMIT/ 6.0) && ppm_pixel -> hsl -> hue <= ( HUE_UPPER_LIMIT/ 3.0))
	{
		r1 = x;
		g1 = c;
		b1 = 0;
	}
	else if( ppm_pixel -> hsl -> hue >= ( HUE_UPPER_LIMIT/ 3.0) && ppm_pixel -> hsl -> hue <= ( HUE_UPPER_LIMIT/ 2.0))
	{
		r1 = 0;
		g1 = c;
		b1 = x;
	}
	else if ( ppm_pixel -> hsl -> hue >= ( HUE_UPPER_LIMIT/ 2.0) && ppm_pixel -> hsl -> hue <= (2.0f * HUE_UPPER_LIMIT/ 3.0))
	{
		r1 = 0;
		g1 = x;
		b1 = c;
	}
	else if( ppm_pixel -> hsl -> hue >= (2.0f * HUE_UPPER_LIMIT/ 3.0) && ppm_pixel -> hsl -> hue < (5.0f * HUE_UPPER_LIMIT/ 6.0))
	{
		r1 = x;
		g1 = 0;
		b1 = c;
	}
	else if( ppm_pixel -> hsl -> hue >= (5.0f * HUE_UPPER_LIMIT/ 6.0) && ppm_pixel -> hsl -> hue < HUE_UPPER_LIMIT)
	{
		r1 = c;
		g1 = 0;
		b1 = x;
	}
	else
	{
		r1 = 0;
		g1 = 0;
		b1 = 0;
	}
	ppm_pixel -> rgb -> red = (r1 + m)*255;
	ppm_pixel -> rgb -> green = (g1 + m)*255;
	ppm_pixel -> rgb -> blue = (b1 + m)*255;
}

PPMImage *read_ppm_data(FILE *ppm)
{
	PPMImage *img;
	char c;
	int row, col ,index = 0;

	img = (PPMImage *)malloc(sizeof(PPMImage));

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

	printf("reading a PPM file (raw version)!\n"); 
		
	skip_comment(ppm);
	fscanf(ppm, "%d", &(img -> width));
	skip_comment(ppm);
	fscanf(ppm, "%d", &(img -> height));
	skip_comment(ppm);
	fscanf(ppm, "%d", &(img -> max_rgb_val));
	fgetc(ppm);
	printf("%d rows  %d columns  max value= %d\n", img -> width, img -> height , img -> max_rgb_val);

	img -> pixels = (PPMPixel*)malloc(img -> width * img -> height * sizeof(PPMPixel));

	for (row = img -> height - 1; row >= 0; row--)
	{
		for (col = 0; col < img -> width; col++)
		{
			img -> pixels[index].rgb = (RGB*)malloc(sizeof(RGB));

			img -> pixels[index].rgb -> red = (float)fgetc(ppm);
			img -> pixels[index].rgb -> green = (float)fgetc(ppm);
			img -> pixels[index].rgb -> blue = (float)fgetc(ppm);

			img -> pixels[index].hsl = RGBtoHSL(img -> pixels[index].rgb);
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

void change_saturation(PPMImage *img, float factor)
{
	int row, col ,index = 0;
	for (row = img -> height - 1; row >= 0; row--)
	{
		for (col = 0; col < img -> width; col++)
		{
			if(factor < 0.0)
			{
				img -> pixels[index].hsl -> saturation = img -> pixels[index].hsl -> saturation* (1 + factor);;
			}
			else
			{
				img -> pixels[index].hsl -> saturation = img -> pixels[index].hsl -> saturation + ((1-img -> pixels[index].hsl -> saturation) * factor);
			}
			HSLtoRGB(&(img -> pixels[index]));
			index++;
		}
	}
}

void change_brightness(PPMImage *img, float factor)
{
	int row, col ,index = 0;
	float r1, g1, b1;

	for (row = img -> height - 1; row >= 0; row--)
	{
		for (col = 0; col < img -> width; col++)
		{
			r1 = img -> pixels[index].rgb -> red / 255.0;
			g1 = img -> pixels[index].rgb -> green / 255.0;
			b1 = img -> pixels[index].rgb -> blue / 255.0;
			if(factor < 0.0)
			{
				r1 = r1 * (1 + factor);
				g1 = g1 * (1 + factor);
				b1 = b1 * (1 + factor);
			}
			else
			{
				r1 = r1 + ((1-r1) * factor);
				g1 = g1 + ((1-g1) * factor);
				b1 = b1 + ((1-b1) * factor);
			}

			img -> pixels[index].rgb -> red = r1 * 255;
			img -> pixels[index].rgb -> green = g1 * 255;
			img -> pixels[index].rgb -> blue = b1 * 255;

			index++;
		}
	}
}

void change_contrast(PPMImage *img, float factor)
{
	int row, col ,index = 0;

	for (row = img -> height - 1; row >= 0; row--)
	{
		for (col = 0; col < img -> width; col++)
		{

			img -> pixels[index].rgb -> red = (img -> pixels[index].rgb -> red - 128) * tan((1+factor) * PI/4) + 128;
			img -> pixels[index].rgb -> green = (img -> pixels[index].rgb -> green - 128) * tan((1+factor) * PI/4) + 128;
			img -> pixels[index].rgb -> blue = (img -> pixels[index].rgb -> blue - 128) * tan((1+factor) * PI/4) + 128;

			index++;
		}
	}
}

int main(int argc, char *argv[])
{
	float brightness = 50, contrast = 50, saturation = 50;
	FILE *ppm_file;
	PPMImage *ppm_image;

	if(argc < 2)
	{
		printf("Print Usage Here"); /* TODO */
		exit(1);
	}

	DELAY = to_number(argv[1]);
	brightness = to_number(argv[2]);
	contrast = to_number(argv[3]);
	saturation = to_number(argv[4]);

	if(brightness >100 || brightness < 0)
	{
		brightness = 50;
	}

	if(contrast >100 || contrast < 0)
	{
		contrast = 50;
	}

	if(saturation >100 || saturation < 0)
	{
		saturation = 50;
	}

	brightness = brightness*2/100 -1;
	contrast = contrast*2/100 -1;
	saturation = saturation*2/100 -1;

	ppm_file = fopen("/home/vatsalya/Documents/3042/rlefiles/test/image-0016.ppm", "r");
	ppm_image = read_ppm_data(ppm_file);

	SDL_Window *window = NULL;
	SDL_Surface *screen_surface = NULL;


	if(SDL_Init(SDL_INIT_VIDEO) < 0)
	{
		fprintf(stderr, "SDL could not initialize! SDL_Error: %s\n", SDL_GetError());
	}
	else
	{
		window = SDL_CreateWindow("ppmplayer", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, ppm_image -> width,
			ppm_image -> height, SDL_WINDOW_SHOWN);
	
		if(window == NULL)
		{
			fprintf(stderr, "Window could not be created! SDL_Error: %s\n", SDL_GetError());
		}
		else
		{
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
			// SDL_FillRect(screen_surface, NULL, SDL_MapRGB(screen_surface -> format, 0xff, 0x00, 0xff));
			SDL_UpdateWindowSurface(window);
			SDL_Delay(DELAY*1000);
		}
	}

	return 0;
}