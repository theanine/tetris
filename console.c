#include <stdio.h>
#include <stdlib.h>
#include <termios.h>
#include <unistd.h>
#include <assert.h>
#include <pthread.h>
#include <time.h>
#include "errors.h"
#include "console.h"

#define DEBUG 0

#if DEBUG
#define TRACE(...)  do { printf(__VA_ARGS__); } while (0)
#else
#define TRACE(...)  do { } while (0)
#endif

#define COLOR_STR_NORMAL   "\x1B[0m"
#define COLOR_STR_RED      "\x1B[31m"
#define COLOR_STR_GREEN    "\x1B[32m"
#define COLOR_STR_YELLOW   "\x1B[33m"
#define COLOR_STR_BLUE     "\x1B[34m"
#define COLOR_STR_MAGENTA  "\x1B[35m"
#define COLOR_STR_CYAN     "\x1B[36m"
#define COLOR_STR_WHITE    "\x1B[37m"

void console_init(console_t* c)
{
	struct termios t;
	tcgetattr(STDIN_FILENO, &t);
	c->old_t = t;
	t.c_lflag &= (~ICANON & ~ECHO);
	tcsetattr(STDIN_FILENO, TCSANOW, &t);
	console_setcolor(COLOR_NORMAL);
}

void console_destroy(console_t* c)
{
	tcsetattr(STDIN_FILENO, TCSANOW, &c->old_t);
}

void console_clear(void)
{
	write(STDOUT_FILENO, " \033[1;1H\033[2J", 12);
}

void console_setcolor(color_t color)
{
	const char* color_str = "";
	
	switch (color) {
		case COLOR_NORMAL:  color_str = COLOR_STR_NORMAL;  break;
		case COLOR_RED:     color_str = COLOR_STR_RED;     break;
		case COLOR_GREEN:   color_str = COLOR_STR_GREEN;   break;
		case COLOR_YELLOW:  color_str = COLOR_STR_YELLOW;  break;
		case COLOR_BLUE:    color_str = COLOR_STR_BLUE;    break;
		case COLOR_MAGENTA: color_str = COLOR_STR_MAGENTA; break;
		case COLOR_CYAN:    color_str = COLOR_STR_CYAN;    break;
		case COLOR_WHITE:   color_str = COLOR_STR_WHITE;   break;
		default:                                           break;
	}
	
	printf("%s", color_str);
}
