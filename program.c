/*
Do not modify this file.
Make all of your changes to main.c instead.
*/

#include "program.h"

#include <stdio.h>
#include <stdlib.h>

static int compare_bytes( const void* pa, const void* pb )
{
	int a = *(char*)pa;
	int b = *(char*)pb;

	if (a < b)
	{
		return -1;
	}
	else if (a == b)
	{
		return 0;
	}
	else
	{
		return 1;
	}
}

void access_pattern1( char* data, int length )
{
	for (int k=0; k<length; k++)
	{
		data[k]++;
	}
}

void access_pattern2( char* data, int length )
{
	// TO-DO: Implementar
}

void access_pattern3( char* cdata, int length )
{
	// TO-DO: Implementar
}
