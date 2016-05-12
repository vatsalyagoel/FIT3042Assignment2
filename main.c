#include <stdio.h>


#include <SDL.h>

/* Program Headers*/
#include "helpers.h"
#include "ppm.h" 
#include "ppmplayer.h"



int main(int argc, char *argv[])
{
	window = NULL;
	screen_surface = NULL;

	int running = 1;
	float brightness = 50, contrast = 50, saturation = 50;

	FILE *ppm_file;
	PPMImage *ppm_image;

	/* Setup the stream for error and logging*/
	freopen( "./logs/error.log", "w+", stderr );
	log_file = open_file("./logs/debog.log", "w+");
	
	/* Send the streams some text */ 
	log_error("Start of rledecode" );


	if(argc < 2)
	{
		print_usage();
		exit(1);
	}
	if(argc > 2 && argc < 5)
	{
		print_usage();
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

	ppm_file = stdin;

	int start_time;

	if(SDL_Init(SDL_INIT_VIDEO) < 0)
	{
		fprintf(stderr, "SDL could not initialize! SDL_Error: %s\n", SDL_GetError());
	}
	else
	{
		window = SDL_CreateWindow("ppmplayer", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 0,
		0, SDL_WINDOW_SHOWN);
		if(window == NULL)
		{
			fprintf(stderr, "Window could not be created! SDL_Error: %s\n", SDL_GetError());
		}
		else
		{
			char c;
			int count;
			c = fgetc(ppm_file);
			if(c  == 'P')
			{
				ungetc(c, ppm_file);
				ppm_image = read_ppm_data(ppm_file, brightness, contrast, saturation);
			}
			else
			{
				return 1;
			}
			while(running == 1)
			{
				start_time = SDL_GetTicks();
				
				make_window(ppm_image);
				free_ppm(ppm_image);

				count = 0;
				c = fgetc(ppm_file);
				while(c != 'P' && c == -1)
				{
					c = fgetc(ppm_file);
					count++;
					if(count > 4)
					{
						SDL_DestroyWindow(window);
						running = 0;
						return 0;
					}
				}
				if(c  == 'P')
				{
					ungetc(c, ppm_file);
					ppm_image = read_ppm_data(ppm_file, brightness, contrast, saturation);
				}

				int end_time = SDL_GetTicks() - start_time;

				if(end_time < DELAY)
				{
					SDL_Delay(DELAY-(SDL_GetTicks() - start_time));
				}
			}
		}
	}
	return 0;
}