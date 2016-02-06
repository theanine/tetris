#ifndef TETRIS_UTILS
#define TETRIS_UTILS

#include <termios.h>
#include <stdbool.h>

#define MAX_PIECE_HEIGHT  4
#define MAX_PIECE_WIDTH   4

// struct console handles putting the terminal in raw mode for input
typedef struct console {
	struct termios old_t;
} console_t;

// board_t represents the tetris board state
typedef struct board {
	int   width;
	int   height;
	int** cells;
	int   level;
	int   score;
} board_t;

// piece_t represents the pieces
typedef struct piece {
	bool cells[MAX_PIECE_HEIGHT][MAX_PIECE_WIDTH];
} piece_t;

typedef enum piece_name {
	PIECE_STRAIGHT,
	PIECE_SQUARE,
	PIECE_T,
	PIECE_L,
	PIECE_L_REVERSE,
	PIECE_S,
	PIECE_S_REVERSE,
	PIECE_COUNT
} piece_name_t;
	
typedef enum keycode {
	KEY_UNKNOWN = -1,
	KEY_UP      = 65,
	KEY_DOWN    = 66,
	KEY_RIGHT   = 67,
	KEY_LEFT    = 68,
	KEY_ENTER   = '\n',
	KEY_SPACE   = ' ',
} keycode_t;

void piece_init(void);
piece_t piece_gen(void);
void piece_show(board_t* board, piece_t piece, int row, int col);
void piece_hide(board_t* board, piece_t piece, int row, int col);
void console_init(console_t* c);
void console_destroy(console_t* c);
void console_clear(void);
void input_queue(keycode_t key);
keycode_t input_pop(void);
void* input_handler(void* unused);
void input_handle(board_t* b, keycode_t input, piece_t* piece, int* row, int* col);
int input_init(void);
bool collision_check(board_t* b, piece_t piece, int row, int col);
bool anchor_check(board_t* b, piece_t piece, int row, int col);
void board_linecheck(board_t* b);
bool board_gameover(board_t* b, piece_t piece, int row, int col);
void board_init(board_t* b, int width, int height);
void board_destroy(board_t* b);
void board_set(board_t* b, int row, int col, int val);
int board_get(board_t* b, int row, int col);
void board_print(board_t* b);
int board_getdroptime(board_t* b);
void board_levelup(board_t* b);

#endif /* TETRIS_UTILS */
