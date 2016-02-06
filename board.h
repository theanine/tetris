#ifndef __BOARD_H__
#define __BOARD_H__

#include <stdbool.h>

// board_t represents the tetris board state
typedef struct board {
	int   width;
	int   height;
	int** cells;
	int   level;
	int   score;
} board_t;

#include "piece.h"

void board_linecheck(board_t* b);
bool board_gameover(board_t* b, piece_t piece, int row, int col);
void board_init(board_t* b, int width, int height);
void board_destroy(board_t* b);
void board_set(board_t* b, int row, int col, int val);
int board_get(board_t* b, int row, int col);
int board_getdroptime(board_t* b);
void board_levelup(board_t* b);

#endif /* __BOARD_H__ */
