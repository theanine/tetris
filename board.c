#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <termios.h>
#include <unistd.h>
#include <assert.h>
#include <pthread.h>
#include <time.h>
#include "errors.h"
#include "board.h"
#include "piece.h"
#include "console.h"

#define DEBUG 0

#if DEBUG
#define TRACE(...)  do { printf(__VA_ARGS__); } while (0)
#else
#define TRACE(...)  do { } while (0)
#endif

#define INITIAL_LEVEL              1
#define INITIAL_SCORE              0
#define DIFFICULTY_MULTIPLIER      1000000
#define TETRIS_ROW_COUNT           4
#define TETRIS_ROW_SCORE           10
#define DIFFICULTY_ACCELERATION    10     // in pieces

bool board_gameover(board_t* b)
{
	for (int y = 0; y < MAX_PIECE_HEIGHT; y++) {
		for (int x = 0; x < MAX_PIECE_WIDTH; x++) {
			if (piece_get_cell(&b->piece, y, x)) {
				if (piece_collision_check_top(b, y + b->row, x + b->col)) {
					TRACE("%s caused gameover with (%d,%d) at (%d,%d)\n", piece_to_str(b->piece.name), y, x, y + b->row, x + b->col);
					TRACE("Piece was %sconsidered anchored.\n", piece_anchor_check(b) ? "" : "NOT ");
					return true;
				}
			}
		}
	}
	return false;
}

void board_shift(board_t* b, int start_row)
{
	for (int row = start_row; row >= 0; row--) {
		for (int col = 0; col < b->width; col++) {
			color_t val = (row < 1) ? COLOR_NONE : board_get(b, row - 1, col);
			board_set(b, row, col, val);
		}
	}
}

void board_linecheck(board_t* b)
{
	int rows_cleared = 0;
	for (int row = 0; row < b->height; row++) {
		
		bool row_cleared = true;
		
		for (int col = 0; col < b->width; col++)
			if (board_get(b, row, col) == COLOR_NONE)
				row_cleared = false;
		
		if (row_cleared) {
			rows_cleared++;
			for (int col = 0; col < b->width; col++)
				board_set(b, row, col, COLOR_NONE);
			board_shift(b, row);
		}
	}
	
	assert(rows_cleared <= TETRIS_ROW_COUNT);
	if (rows_cleared == TETRIS_ROW_COUNT)
		b->score += TETRIS_ROW_SCORE;
	else
		b->score += rows_cleared;
}

void board_init(board_t* b, int width, int height)
{
	b->level       = INITIAL_LEVEL;
	b->score       = INITIAL_SCORE;
	b->width       = width;
	b->height      = height;
	b->piece_count = 0;
	b->cells       = calloc(height, sizeof(color_t*));
	
	for (int row = 0; row < height; row++)
		b->cells[row] = calloc(width, sizeof(color_t));
}

void board_destroy(board_t* b)
{
	for (int row = 0; row < b->height; row++)
		free(b->cells[row]);
	free(b->cells);
}

void board_set(board_t* b, int row, int col, color_t color)
{
	TRACE("%s(%p, %d, %d, %d)\n", __func__, (void*)b, row, col, color);
	if (row >= 0 && row < b->height)
		if (col >= 0 && col < b->width)
			b->cells[row][col] = color;
}

color_t board_get(board_t* b, int row, int col)
{
	if (col < 0)
		return ERR_OFF_LEFT;
	if (col >= b->width)
		return ERR_OFF_RIGHT;
	if (row >= b->height)
		return ERR_OFF_BOTTOM;
	if (row < 0)
		return ERR_OFF_TOP;
	return b->cells[row][col];
}

color_t board_get_next_piece(board_t* b, int row, int col)
{
	if (b->next_piece.cells[row][col])
		return b->next_piece.color;
	return COLOR_NONE;
}

// drop time is in microseconds
int board_getdroptime(board_t* b)
{
	return DIFFICULTY_MULTIPLIER / b->level;
}

void board_levelup(board_t* b)
{
	b->level++;
}

void board_newpiece(board_t* b)
{
	int start_row_pos = 1 - MAX_PIECE_HEIGHT;
	int start_col_pos = (b->width - MAX_PIECE_WIDTH) / 2;
	
	b->row        = start_row_pos;
	b->col        = start_col_pos;
	
	if (b->piece_count == 0)
		b->next_piece = piece_gen();
	
	b->piece      = b->next_piece;
	b->next_piece = piece_gen();
	
	b->piece_count++;
	if (b->piece_count % DIFFICULTY_ACCELERATION == 0)
		board_levelup(b);
}

void piece_visibility(board_t* b, bool shown)
{
	color_t color = shown ? b->piece.color : COLOR_NONE;
	for (int y = 0; y < MAX_PIECE_HEIGHT; y++)
		for (int x = 0; x < MAX_PIECE_WIDTH; x++)
			if (piece_get_cell(&b->piece, y, x))
				board_set(b, y + b->row, x + b->col, color);
}

void piece_show(board_t* b)
{
	piece_visibility(b, true);
}

void piece_hide(board_t* b)
{
	piece_visibility(b, false);
}

bool piece_collision_check_bottom(board_t* b, int row, int col)
{
	int color = board_get(b, row, col);
	TRACE("%s: %d\n", __func__, color);
	if (color != COLOR_NONE && color > 0)
		return true;
	
	return (color == ERR_OFF_BOTTOM);
}

bool piece_collision_check_top(board_t* b, int row, int col)
{
	int color = board_get(b, row, col);
	return (color == ERR_OFF_TOP);
}

int piece_get_collision(board_t* b, piece_t* p, int row, int col)
{
	TRACE("%s(%d, %d)\n", __func__, row, col);
	
	for (int y = 0; y < MAX_PIECE_HEIGHT; y++) {
		for (int x = 0; x < MAX_PIECE_WIDTH; x++) {
			if (piece_get_cell(p, y, x)) {
				int color = board_get(b, row + y, col + x);
				if (color != COLOR_NONE && color != ERR_OFF_TOP) {
					TRACE("Found a collision at (%d, %d) with color %d\n", row + y, col + x, color);
					return color;
				}
			}
		}
	}
	TRACE("%s: No collisions.\n", __func__);
	return COLOR_NONE;
}

bool piece_collision_check(board_t* b, piece_t* p, int row, int col)
{
	int color = piece_get_collision(b, p, row, col);
	return (color != COLOR_NONE && color != ERR_OFF_TOP);
}

bool piece_anchor_check(board_t* b)
{
	TRACE("%s(%d, %d)\n", __func__, b->row, b->col);
	
	for (int y = MAX_PIECE_HEIGHT-1; y > 0; y--) {
		for (int x = 0; x < MAX_PIECE_WIDTH; x++) {
			if (piece_get_cell(&b->piece, y, x)) {
				if (piece_get_cell(&b->piece, y + 1, x))
					continue;
				if (piece_collision_check_bottom(b, b->row + y + 1, b->col + x)) {
					TRACE("Detected a collision with [%d,%d] at [%d,%d]\n", y, x, b->row + y + 1, b->col + x);
					return true;
				}
			}
		}
	}
	
	TRACE("%s: No collisions.\n", __func__);
	return false;
}
