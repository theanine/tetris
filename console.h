#ifndef __CONSOLE_H__
#define __CONSOLE_H__

#include <termios.h>

// struct console handles putting the terminal in raw mode for input
typedef struct console {
	struct termios old_t;
} console_t;

void console_init(console_t* c);
void console_destroy(console_t* c);
void console_clear(void);

#endif /* __CONSOLE_H__ */
