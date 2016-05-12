/*********************************************************
*	STRUCT DEFINITIONS BASED ON THE FORMAT SPECIFIED
*********************************************************/


/*********************************************************
* Name - RGB
* Members - red   - red pixel data
*			blue  - blue pixel data
*			green - green pixel data	
*********************************************************/
#ifndef RGB_H
#define RGB_H
	typedef struct
	{
		float red;
		float green;
		float blue;
	} RGB;
#endif

/*********************************************************
* Name - PPMPixel
* Members - red   - red pixel data
*			blue  - blue pixel data
*			green - green pixel data	
*********************************************************/
#ifndef HSV_H
#define HSV_H
	typedef struct
	{
		float hue;
		float saturation;
		float lightness;
	} HSL;
#endif

/*********************************************************
* Name - PPMPixel
* Members - red   - red pixel data
*			blue  - blue pixel data
*			green - green pixel data	
*********************************************************/
#ifndef PPMPixel_H
#define PPMPixel_H
	typedef struct
	{
		RGB *rgb;
		HSL *hsl;
	} PPMPixel;
#endif

/*********************************************************
* Name - PPMImage
* Members - magic_number - PPM iamge Magic Number Read
*			width  - Width of image
*			height - Height of image
*			max_rgb_val - Maximum value of color
*			pixels - Each Pixel Data
*********************************************************/
#ifndef PROPERTY_H
#define PROPERTY_H
	typedef struct
	{
		char magic_number[16];
		int width, height;
		int max_rgb_val;
		PPMPixel *pixels;
	} PPMImage;
#endif

#define RGB_COMPONENT_COLOR 255
#define HUE_UPPER_LIMIT 360.0
#define PI 3.14159265

PPMImage *read_ppm_data(FILE *ppm, float brightness_factor, float contrast_factor, float saturation_factor);
void alter_pixel(PPMPixel *pixel, float brightness_factor, float contrast_factor, float saturation_factor);

int DELAY;
SDL_Window *window;
SDL_Surface *screen_surface;
int running;