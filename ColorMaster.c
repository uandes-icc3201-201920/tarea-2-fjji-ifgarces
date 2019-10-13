#include "ColorMaster.h"
#include <stdlib.h>
#include <string.h>

void printcolor(int colorcode, const char* what_to_print)
{
	assert((colorcode >= 30) && (colorcode <= 37));
	_head = (char*)malloc(sizeof(char)*20);
	_foot = (char*)malloc(sizeof(char)*20);
	strcpy(_foot, "\033[0m");
	sprintf(_head, "\033[1;%dm", colorcode);     /// format: "\033[<STYLE>;<COLOR>m<MY_TEXT>\033[0m"
	
	printf("%s%s%s\n", _head, what_to_print, _foot);
	free(_head);
	free(_foot);
}