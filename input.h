#ifndef __INPUT_H__
#define __INPUT_H__

#include <termios.h>
#include <stdbool.h>
#include "board.h"
#include "piece.h"

typedef enum keycode {
	KEY_UNKNOWN = -1,
	KEY_UP      = 65,
	KEY_DOWN    = 66,
	KEY_RIGHT   = 67,
	KEY_LEFT    = 68,
	KEY_ENTER   = '\n',
	KEY_SPACE   = ' ',
} keycode_t;

void input_queue(keycode_t key);
keycode_t input_pop(void);
void* input_handler(void* unused);
void input_handle(board_t* b, keycode_t input, piece_t* piece, int* row, int* col);
int input_init(void);

#endif /* __INPUT_H__ */
