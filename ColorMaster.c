#include "ColorMaster.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>

/// forma de uso:
///		> printcolor(RED, "texto en rojo.\n")
void printcolor(int colorcode, const char* text)
{
	assert((colorcode >= 30) && (colorcode <= 37));
	_head = (char*) malloc(sizeof(char)*25);
	_foot = (char*) malloc(sizeof(char)*25);
	strcpy(_foot, "\033[0m");
	sprintf(_head, "\033[1;%dm", colorcode);  // formato: "\033[<STYLE>;<COLOR>m<MY_TEXT>\033[0m"
	
	printf("%s%s%s", _head, text, _foot);
	free(_head);
	free(_foot);
}



/// forma de uso:
///		> color_start(RED); printf("mensaje con printf en %s", "rojo"); color_end()
void color_start(int colorcode)
{
	assert((colorcode >= 30) && (colorcode <= 37));
	_head = (char*) malloc(sizeof(char)*25);
	sprintf(_head, "\033[1;%dm", colorcode);
	printf("%s", _head);
	free(_head);
}

void color_end()
{
	printf("\033[0m");
}