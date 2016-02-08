#ifndef __BOARD_H__
#define __BOARD_H__

#include <stdbool.h>
#include "console.h"
#include "piece.h"

// board_t represents the tetris board state
typedef struct board {
	piece_t    piece;         // current piece
	piece_t    next_piece;
	int        piece_count;
	int        row;           // current row
	int        col;           // current col
	int        width;
	int        height;
	color_t**  cells;
	int        level;
	int        score;
} board_t;

void board_newpiece(board_t* b);
void board_linecheck(board_t* b);
bool board_gameover(board_t* b);
void board_init(board_t* b, int width, int height);
void board_destroy(board_t* b);
void board_set(board_t* b, int row, int col, color_t color);
color_t board_get_next_piece(board_t* b, int row, int col);
color_t board_get(board_t* b, int row, int col);
int board_getdroptime(board_t* b);
void board_levelup(board_t* b);
void piece_show(board_t* b);
void piece_hide(board_t* b);
bool piece_anchor_check(board_t* b);
int piece_get_collision(board_t* b, piece_t* p, int row, int col);
bool piece_collision_check_top(board_t* b, int row, int col);
bool piece_collision_check(board_t* b, piece_t* p, int row, int col);

#endif /* __BOARD_H__ */
