#ifndef __PIECE_H__
#define __PIECE_H__

#include <stdbool.h>

#define MAX_PIECE_HEIGHT  4
#define MAX_PIECE_WIDTH   4

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

#include "board.h"

void piece_init(void);
piece_t piece_gen(void);
void piece_show(board_t* board, piece_t piece, int row, int col);
void piece_hide(board_t* board, piece_t piece, int row, int col);
bool piece_anchor_check(board_t* b, piece_t piece, int row, int col);
bool piece_collision_check_top(board_t* b, int row, int col);
bool piece_collision_check(board_t* b, piece_t piece, int row, int col);
void piece_rotate(piece_t* piece);

#endif /* __PIECE_H__ */
