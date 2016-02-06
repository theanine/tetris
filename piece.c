#include <stdio.h>
#include <stdlib.h>
#include <termios.h>
#include <unistd.h>
#include <assert.h>
#include <pthread.h>
#include <time.h>
#include "errors.h"
#include "piece.h"

#define DEBUG 0

#if DEBUG
#define TRACE(...)  do { printf(__VA_ARGS__); } while (0)
#else
#define TRACE(...)  do { } while (0)
#endif

piece_t piece_list[PIECE_COUNT] = {0};

void piece_init(void)
{
	// PIECE_STRAIGHT
	piece_list[PIECE_STRAIGHT].cells[2][0] = 1;
	piece_list[PIECE_STRAIGHT].cells[2][1] = 1;
	piece_list[PIECE_STRAIGHT].cells[2][2] = 1;
	piece_list[PIECE_STRAIGHT].cells[2][3] = 1;
	
	// PIECE_SQUARE
	piece_list[PIECE_SQUARE].cells[1][1] = 1;
	piece_list[PIECE_SQUARE].cells[1][2] = 1;
	piece_list[PIECE_SQUARE].cells[2][1] = 1;
	piece_list[PIECE_SQUARE].cells[2][2] = 1;
	
	// PIECE_T
	piece_list[PIECE_T].cells[2][1] = 1;
	piece_list[PIECE_T].cells[2][2] = 1;
	piece_list[PIECE_T].cells[2][3] = 1;
	piece_list[PIECE_T].cells[1][2] = 1;
	
	// PIECE_L
	piece_list[PIECE_L].cells[0][1] = 1;
	piece_list[PIECE_L].cells[1][1] = 1;
	piece_list[PIECE_L].cells[2][1] = 1;
	piece_list[PIECE_L].cells[2][2] = 1;
	
	// PIECE_L_REVERSE
	piece_list[PIECE_L_REVERSE].cells[0][2] = 1;
	piece_list[PIECE_L_REVERSE].cells[1][2] = 1;
	piece_list[PIECE_L_REVERSE].cells[2][2] = 1;
	piece_list[PIECE_L_REVERSE].cells[2][1] = 1;
	
	// PIECE_S
	piece_list[PIECE_S].cells[1][2] = 1;
	piece_list[PIECE_S].cells[1][3] = 1;
	piece_list[PIECE_S].cells[2][1] = 1;
	piece_list[PIECE_S].cells[2][2] = 1;
	
	// PIECE_S_REVERSE
	piece_list[PIECE_S_REVERSE].cells[1][1] = 1;
	piece_list[PIECE_S_REVERSE].cells[1][2] = 1;
	piece_list[PIECE_S_REVERSE].cells[2][2] = 1;
	piece_list[PIECE_S_REVERSE].cells[2][3] = 1;
	
	srand(time(NULL));
}

piece_t piece_gen(void)
{
	// return piece_list[PIECE_STRAIGHT];
	piece_name_t name = rand() % PIECE_COUNT;
	TRACE("generated a %d\n", name);
	return piece_list[name];
}

void piece_visibility(board_t* board, piece_t piece, int row, int col, bool shown)
{
	for (int y = 0; y < MAX_PIECE_HEIGHT; y++)
		for (int x = 0; x < MAX_PIECE_WIDTH; x++)
			if (piece.cells[y][x])
				board_set(board, y + row, x + col, shown);
}

void piece_show(board_t* board, piece_t piece, int row, int col)
{
	piece_visibility(board, piece, row, col, true);
}

void piece_hide(board_t* board, piece_t piece, int row, int col)
{
	piece_visibility(board, piece, row, col, false);
}

void piece_rotate(piece_t* piece)
{
	assert(MAX_PIECE_WIDTH == MAX_PIECE_HEIGHT);
	
	int start_idx = 0;
	int last_idx  = MAX_PIECE_WIDTH - 1;
	
	for (int i=start_idx; i<last_idx; i++) {
		bool tmp = piece->cells[i][last_idx];
		piece->cells[i][last_idx] = piece->cells[start_idx][i]; 
		piece->cells[start_idx][i] = piece->cells[last_idx-i][start_idx];
		piece->cells[last_idx-i][start_idx] = piece->cells[last_idx][last_idx-i]; 
		piece->cells[last_idx][last_idx-i] = tmp;
	}
	
	bool tmp = piece->cells[1][1];
	piece->cells[1][1] = piece->cells[2][1];
	piece->cells[2][1] = piece->cells[2][2];
	piece->cells[2][2] = piece->cells[1][2];
	piece->cells[1][2] = tmp;
	
}

bool piece_collision_check_bottom(board_t* b, int row, int col)
{
	int val = board_get(b, row, col);
	TRACE("%s: %d\n", __func__, val);
	return (val == 1 || val == ERR_OFF_BOTTOM);
}

bool piece_collision_check_top(board_t* b, int row, int col)
{
	int val = board_get(b, row, col);
	return (val == ERR_OFF_TOP);
}

bool piece_collision_check(board_t* b, piece_t piece, int row, int col)
{
	TRACE("%s(%d, %d)\n", __func__, row, col);
	
	for (int y = 0; y < MAX_PIECE_HEIGHT; y++) {
		for (int x = 0; x < MAX_PIECE_WIDTH; x++) {
			if (piece.cells[y][x]) {
				int val = board_get(b, row + y, col + x);
				if (val != 0 && val != ERR_OFF_TOP)
					return true;
			}
		}
	}
	return false;
}

bool piece_anchor_check(board_t* b, piece_t piece, int row, int col)
{
	TRACE("%s(%d, %d)\n", __func__, row, col);
	
	for (int y = MAX_PIECE_HEIGHT-1; y > 0; y--) {
		for (int x = 0; x < MAX_PIECE_WIDTH; x++) {
			if (piece.cells[y][x]) {
				if (piece_collision_check_bottom(b, y + row + 1, x + col)) {
					TRACE("Detected a collision at [%d,%d] with [%d,%d]\n", y + row + 1, x + col, y, x);
					return true;
				}
			}
		}
	}
	
	TRACE("No collisions.\n");
	return false;
}
