#ifndef __PIECE_H__
#define __PIECE_H__

#include <stdbool.h>
#include "console.h"

#define MAX_PIECE_HEIGHT  4
#define MAX_PIECE_WIDTH   4

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

// piece_t represents the pieces
typedef struct piece {
	piece_name_t name;
	color_t color;
	int rotated_degrees;
	bool cells[MAX_PIECE_HEIGHT][MAX_PIECE_WIDTH];
} piece_t;

#include "board.h"

const char* piece_to_str(piece_name_t piece);
void piece_init(void);
piece_t piece_gen(void);
bool piece_get_cell(piece_t* piece, int y, int x);
void piece_show(board_t* board, piece_t piece, int row, int col);
void piece_hide(board_t* board, piece_t piece, int row, int col);
bool piece_anchor_check(board_t* b, piece_t piece, int row, int col);
bool piece_collision_check_top(board_t* b, int row, int col);
bool piece_collision_check(board_t* b, piece_t piece, int row, int col);
void piece_rotate(piece_t* piece);

#endif /* __PIECE_H__ */
