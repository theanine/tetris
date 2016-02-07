#ifndef __CONSOLE_H__
#define __CONSOLE_H__

#include <termios.h>

// struct console handles putting the terminal in raw mode for input
typedef struct console {
	struct termios old_t;
} console_t;

typedef enum color {
	COLOR_NONE,
	COLOR_NORMAL,
	COLOR_RED,
	COLOR_GREEN,
	COLOR_YELLOW,
	COLOR_BLUE,
	COLOR_MAGENTA,
	COLOR_CYAN,
	COLOR_WHITE,
} color_t;

void console_init(console_t* c);
void console_destroy(console_t* c);
void console_clear(void);
void console_setcolor(color_t);

#endif /* __CONSOLE_H__ */
