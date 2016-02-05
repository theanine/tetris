#ifndef TETRIS_UTILS
#define TETRIS_UTILS

#include <termios.h>

// struct console handles putting the terminal in raw mode for input
typedef struct console {
	struct termios old_t;
} console_t;

// board_t represents the tetris board state
typedef struct board {
	int   width;
	int   height;
	int** cells;
} board_t;

typedef enum keycode {
	KEY_UNKNOWN = -1,
	KEY_UP      = 65,
	KEY_DOWN    = 66,
	KEY_RIGHT   = 67,
	KEY_LEFT    = 68
} keycode_t;

void console_init(console_t* c);
void console_destroy(console_t* c);
void console_clear(void);
keycode_t get_input(void);
void board_init(board_t* b, int width, int height);
void board_destroy(board_t* b);
void board_set(board_t* b, int row, int col, int val);
int board_get(board_t* b, int row, int col);
void board_print(board_t* b);

#endif /* TETRIS_UTILS */
