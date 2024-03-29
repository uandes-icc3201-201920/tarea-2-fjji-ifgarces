/*
Do not modify this file.
Make all of your changes to main.c instead.
*/

#include "program.h"

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

int random_num;

static int compare_bytes( const void *pa, const void *pb )
{
	int a = *(char*)pa;
	int b = *(char*)pb;

	if(a<b) {
		return -1;
	} else if(a==b) {
		return 0;
	} else {
		return 1;
	}

}

void access_pattern1( char* data, int length )
{
	/// acceso secuencial
	for (int k = 0; k < length; k++)
	{
		data[k]++;
	}
}

void access_pattern2( char* data, int length )
{
	/// acceso aleatorio
	srand(time(NULL));   // se refresca la semilla para que aumentar aleatoriedad. fuente: https://stackoverflow.com/questions/822323/how-to-generate-a-random-int-in-c
	
	for (int k = 0; k < length; k++)
	{
		random_num = rand()%length;
		data[random_num]--;
	}
}

void access_pattern3( char* data, int length )
{
	/// acceso al revés, o algo.
	//printf("pattern3 does nothing for now.");
	
	for (int k = length-1; k >= 0; k--)
	{
		data[k] = 7;
	}
}