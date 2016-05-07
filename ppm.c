#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "ppm.h"

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

HSV *RGBtoHSV(RGB *rgb)
{
	HSV *hsv;
	hsv = (HSV *)malloc(sizeof(HSV));

	float min, max, delta;

	min = MIN(rgb -> red, rgb -> green, rgb -> blue);
	max = MAX(rgb -> red, rgb -> green, rgb -> blue);

	hsv -> value = max;

	delta = max - min;

	if(max != 0)
	{
		hsv -> saturation = delta / max;
	}
	else
	{
		hsv -> saturation = 0;
		hsv -> hue = -1;
		return hsv;
	}

	if( rgb -> red == max )
	{
		hsv -> hue = (rgb -> green - rgb -> blue) / delta;
	}
	else if( rgb -> green == max )
	{
		hsv -> hue = 2 + (rgb -> blue - rgb -> red) / delta;
	}
	else
	{
		hsv -> hue = 4 + (rgb -> red - rgb -> green) / delta;
	}
	hsv -> hue *= 60;

	if(hsv -> hue < 0 )
		hsv -> hue += 360;

	return hsv;
}

void HSVtoRGB (PPMPixel *ppm_pixel)
{
	int i;
	float f, p, q, t;

	if( ppm_pixel -> hsv -> saturation == 0 )
	{
		ppm_pixel -> rgb -> red = ppm_pixel -> hsv -> value;
		ppm_pixel -> rgb -> red = ppm_pixel -> hsv -> value;
		ppm_pixel -> rgb -> red = ppm_pixel -> hsv -> value;
		return;
	}

	ppm_pixel -> hsv -> hue /= 60;
	i = floor(ppm_pixel -> hsv -> hue);
	f = ppm_pixel -> hsv -> hue - i;

	p = ppm_pixel -> hsv -> value * (1 - ppm_pixel -> hsv -> saturation);
	q = ppm_pixel -> hsv -> value * (1 - ppm_pixel -> hsv -> saturation * f);
	t = ppm_pixel -> hsv -> value * (1 - ppm_pixel -> hsv -> saturation * (1 - f));
	switch(i)
	{
		case 0:
			ppm_pixel -> rgb -> red = ppm_pixel -> hsv -> value;
			ppm_pixel -> rgb -> green = t;
			ppm_pixel -> rgb -> blue = p;
			break;
		case 1:
			ppm_pixel -> rgb -> red = q;
			ppm_pixel -> rgb -> green = ppm_pixel -> hsv -> value;
			ppm_pixel -> rgb -> blue = p;
			break;
		case 2:
			ppm_pixel -> rgb -> red = p;
			ppm_pixel -> rgb -> green = ppm_pixel -> hsv -> value;
			ppm_pixel -> rgb -> blue = t;
			break;
		case 3:
			ppm_pixel -> rgb -> red = p;
			ppm_pixel -> rgb -> green = q;
			ppm_pixel -> rgb -> blue = ppm_pixel -> hsv -> value;
			break;
		case 4:
			ppm_pixel -> rgb -> red = t;
			ppm_pixel -> rgb -> green = p;
			ppm_pixel -> rgb -> blue = ppm_pixel -> hsv -> value;
			break;
		default:
			ppm_pixel -> rgb -> red = ppm_pixel -> hsv -> value;
			ppm_pixel -> rgb -> green = p;
			ppm_pixel -> rgb -> blue = q;
			break;
	}
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

			img -> pixels[index].hsv = RGBtoHSV(img -> pixels[index].rgb);
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
			img -> pixels[index].hsv -> saturation = 0;
			HSVtoRGB(&(img -> pixels[index]));
			index++;
		}
	}
}

int main(int argc, char *argv[])
{
	FILE *ppm = fopen("/home/vatsalya/Documents/3042/rlefiles/test/sonic-test-0001.ppm", "r");
	PPMImage *img = read_ppm_data(ppm);
	write_ppm(img, "/home/vatsalya/Documents/3042/rlefiles/test/sonic-test-bright1-0001.ppm");
	change_saturation(img, -1);
	write_ppm(img, "/home/vatsalya/Documents/3042/rlefiles/test/sonic-test-bright2-0001.ppm");

	//PPMPixel *ppm_pixel;
	//ppm_pixel = (PPMPixel*)malloc(sizeof(PPMPixel));
	
	//ppm_pixel -> rgb = (RGB*)malloc(sizeof(RGB));

	//ppm_pixel -> rgb -> red = 0;
	//ppm_pixel -> rgb -> green = 255;
	//ppm_pixel -> rgb -> blue = 0;

	//ppm_pixel -> hsv = RGBtoHSV(ppm_pixel -> rgb);
	//printf("%f %f %f\n", ppm_pixel -> hsv -> hue, ppm_pixel -> hsv -> saturation, ppm_pixel -> hsv -> value);
	//HSVtoRGB(ppm_pixel);
	//printf("%f %f %f\n",ppm_pixel -> rgb -> red, ppm_pixel -> rgb -> green, ppm_pixel -> rgb -> blue );
	//change_brightness(img, -0.9);
	//write_ppm(img, "/home/vatsalya/Documents/3042/rlefiles/test/sonic-test-bright2-0001.ppm");
	return 0;
}