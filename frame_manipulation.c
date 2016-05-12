#include <stdio.h>


#include <SDL.h>

/* Program Headers*/
#include "helpers.h"
#include "ppm.h" 
#include "frame_manipulation.h"

HSL *RGBtoHSL(RGB *rgb)
{
	HSL *hsl;
	hsl = (HSL *)ppm_malloc(sizeof(HSL), "RGBtoHSL");

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

void alter_pixel(PPMPixel *pixel, float brightness_factor, float contrast_factor, float saturation_factor)
{
	float r1, g1, b1;

	if(saturation_factor < 0.0)
	{
		pixel -> hsl -> saturation = pixel -> hsl -> saturation* (1 + saturation_factor);;
	}
	else
	{
		pixel -> hsl -> saturation = pixel -> hsl -> saturation + ((1-pixel -> hsl -> saturation) * saturation_factor);
	
	}
	HSLtoRGB(pixel);

	//brighness
	r1 = pixel -> rgb -> red / 255.0;
	g1 = pixel -> rgb -> green / 255.0;
	b1 = pixel -> rgb -> blue / 255.0;
	if(brightness_factor < 0.0)
	{
		r1 = r1 * (1 + brightness_factor);
		g1 = g1 * (1 + brightness_factor);
		b1 = b1 * (1 + brightness_factor);
	}
	else
	{
		r1 = r1 + ((1-r1) * brightness_factor);
		g1 = g1 + ((1-g1) * brightness_factor);
		b1 = b1 + ((1-b1) * brightness_factor);
	}

	pixel -> rgb -> red = r1 * 255;
	pixel -> rgb -> green = g1 * 255;
	pixel -> rgb -> blue = b1 * 255;

	//contrast
	pixel -> rgb -> red = (pixel -> rgb -> red - 128) * tan((1+contrast_factor) * PI/4) + 128;
	pixel -> rgb -> green = (pixel -> rgb -> green - 128) * tan((1+contrast_factor) * PI/4) + 128;
	pixel -> rgb -> blue = (pixel -> rgb -> blue - 128) * tan((1+contrast_factor) * PI/4) + 128;

	//sync RGB and HSL
	pixel -> hsl = RGBtoHSL(pixel -> rgb);
}