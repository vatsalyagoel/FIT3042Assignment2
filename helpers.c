#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <string.h>

#include "helpers.h"

/*********************************************************
*   GENERAL DEFINED FUNCTIONS TO HELP WITH EXECUTION
*********************************************************/

static char s_time[32]; /* String to store the current time */

/*********************************************************
* Desc   : Prints the Command usage for the applicaiton
*
* Params : None
*
* Return : None
*********************************************************/
void print_usage()
{
	printf("NAME:\n");
	printf("\tppmplayer - plays a sequence of ppm files readifn from stdin and displays them at a provided framerate.\n");
	printf("USAGE:\n");
	printf("\t./ppmplayer delay [brightness contrast saturation]\n");
	printf("\t\t1. ./ppmplayer - program binary\n");
	printf("\t\t2. delay - time between two frames in ms \n");
	printf("\t\t3. [brightness] - a number bertween 0-100 to specify brightness\n");
	printf("\t\t4. [contrast] - a number bertween 0-100 to specify contrast\n");
	printf("\t\t5. [saturation] - a number between 0-100 to specify saturation)\n");
}

/*********************************************************
* Desc   : Function to set s_time to current time 
*
* Params : None
*
* Return : None
*********************************************************/
void get_current_time()
{
	time_t ltime; /* calendar time */
	struct tm tim;
    
    ltime=time(NULL); /* get current calendar time */
    localtime_r(&ltime, &tim);
    asctime_r(&tim, s_time);

    s_time[strcspn(s_time, "\n")] = 0;
}

/*********************************************************
* Desc   : Function to log data to stdout
*
* Params : *message - message to log
*
* Return : None 
*********************************************************/
void log_debug(char *message)
{
	get_current_time();
	fprintf(log_file, "%s : %s\n", s_time , message);
}

/*********************************************************
* Desc   : Function to log data to stderr
*
* Params : *message - message to log
*
* Return : None
*********************************************************/
void log_error(char *error_message)
{
	get_current_time();
	fprintf(stderr, "%s : %s\n", s_time , error_message);
}

/*********************************************************
* Desc   : Function to verify if a file exists
*
* Params : *filename - Filename to test if exitst
*
* Return : 1 if exists, 0 otherwise
*********************************************************/
int file_exists(char *filename) /* Verifies if the file exists in the system */
{
	if( access( filename, F_OK ) != -1 )
	{
	    return 1;
	} 
	else 
	{
	    return 0;
	}
}


FILE * open_file(char *filename, char *mode)
{
	FILE *file;
	fopen(filename, mode, file);
	if(file == NULL)
	{
		ppm_exit();
	}
	return file;
}

/*********************************************************
* Desc   : Function to safely allocate memory in the heap
*		   calls malloc dut does safety checks 
*
* Params : size    - Size to allocate
*		   *module - Name of module allocating memory for
*
* Return : ptr - Allocated Memory Pointer 
*********************************************************/
void * ppm_malloc(size_t size, char *module) /* Defensive aproach for malloc() */
{
	void *ptr = malloc(size);
	get_current_time();
	fprintf(stdout,"%s : Allocated %d bytes for Module %s\n", s_time, (int)size, module);

	if(ptr != NULL)
	{
		fprintf(stdout, "%s : Successfully Allocated %d bytes for Module %s\n", s_time, (int)size, module);
	}
	else
	{
		fprintf(stderr, "%s : Unable to allocate %d bytes for Module %s\n", s_time, (int)size, module);
		rle_exit();
	}
	return ptr;
}

/*********************************************************
* Desc   : Function to safely deallocate memory in the 
*		   heap allocated by rle_malloc similar to free()
*
* Params : *ptr 		 - pointer to deallocate memory
*		   *pointer_name - The pointer name for logs
*
* Return : None
*********************************************************/
void ppm_free(void *ptr, char *pointer_name) /* Defensive aproach for free() */
{
	free(ptr);
	get_current_time();
	fprintf(stdout, "%s : freeing pointer %s\n", s_time, pointer_name );

	if(ptr != NULL)
	{
		fprintf(stdout, "%s : Successfully freed pointer %s\n", s_time, pointer_name );
	}
	else
	{
		fprintf(stderr, "%s : Error freeing pointer %s\n", s_time, pointer_name );
		rle_exit();
	}
}

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
	}
}


/*********************************************************
* Desc   : To quit the program if something goes wrong
*		   Tells users where they can access all logs
*
* Params : None
*
* Return : None
*********************************************************/
void ppm_exit()
{
	fprintf(stdout, "An Error Occured. The program will exit now.\n");
	fprintf(stdout, "The logs can be accessed at ./logs/error.log\n");
	exit(-1);
}