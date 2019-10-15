#include "ColorMaster.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <assert.h>

void printcolor(int colorcode, const char* what_to_print)
{
	assert((colorcode >= 30) && (colorcode <= 37));
	_head = (char*) malloc(sizeof(char)*25);
	_foot = (char*) malloc(sizeof(char)*25);
	strcpy(_foot, "\033[0m");
	sprintf(_head, "\033[1;%dm", colorcode);   // formato: "\033[<STYLE>;<COLOR>m<MY_TEXT>\033[0m"
	
	printf("%s%s%s\n", _head, what_to_print, _foot);
	free(_head);
	free(_foot);
}