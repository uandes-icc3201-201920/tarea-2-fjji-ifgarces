#ifndef __SUPERCOLOR__
#define __SUPERCOLOR__

//#define BLACK 30, RED 31, GREEN 32, YELLOW 33, BLUE 34, MAGENTA 35, PURPLE 35, CYAN 36, WHITE 37
//int BLACK = 30, RED = 31, GREEN = 32, YELLOW = 33, BLUE = 34, MAGENTA = 35, PURPLE = 35, CYAN = 36, WHITE = 37;

char* _head;
char* _foot;

/// colo colorea texto, no fondo.
void printcolor(int colorcode, const char* text);
void color_start(int colorcode);
void color_end();

#endif