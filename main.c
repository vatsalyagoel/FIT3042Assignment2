/*Standard library Headers*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

/* SDL Headers */
#include <SDL.h>

/* PPM Player Headers */
/* #include "helpers.h"
   #include "ppmplayer.h" */

int SCREEN_WIDTH = 640, SCREEN_HEIGHT = 480, DELAY;

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



int main(int argc, char *argv[])
{
	/* int brightness = 50, contrast = 50, saturation = 50; */
	/* FILE *ppm_file; */

	if(argc < 2)
	{
		printf("Print Usage Here"); /* TODO */
		exit(1);
	}

	DELAY = to_number(argv[1]);

	/* printf("%d\n", delay); */

	/* ppm_file = fopen("/home/vatsalya/Documents/3042/Assignment/Assignment 1/ppm/sonic-0001.ppm", "r"); */

	SDL_Window *window = NULL;
	SDL_Surface *screen_surface = NULL;
	/* SDL_Texture *screen_texture = NULL; */

	if(SDL_Init(SDL_INIT_VIDEO) < 0)
	{
		fprintf(stderr, "SDL could not initialize! SDL_Error: %s\n", SDL_GetError());
	}
	else
	{
		window = SDL_CreateWindow("ppmplayer", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH,
			SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
	
		if(window == NULL)
		{
			fprintf(stderr, "Window could not be created! SDL_Error: %s\n", SDL_GetError());
		}
		else
		{
			screen_surface = SDL_GetWindowSurface(window);
			SDL_FillRect(screen_surface, NULL, SDL_MapRGB(screen_surface -> format, 0xff, 0x00, 0xff));
			SDL_UpdateWindowSurface(window);
			sleep(DELAY);
		}
	}

	return 0;

}