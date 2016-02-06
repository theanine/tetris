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

#define INITIAL_LEVEL            1
#define INITIAL_SCORE            0
#define DIFFICULTY_MULTIPLIER    1000000

bool board_gameover(board_t* b, piece_t piece, int row, int col)
{
	for (int y = 0; y < MAX_PIECE_HEIGHT; y++)
		for (int x = 0; x < MAX_PIECE_WIDTH; x++)
			if (piece.cells[y][x])
				if (piece_collision_check_top(b, y + row, x + col))
					return true;
	return false;
}

void board_shift(board_t* b, int start_row)
{
	for (int row = start_row; row >= 0; row--) {
		for (int col = 0; col < b->width; col++) {
			int val = (row < 1) ? 0 : board_get(b, row - 1, col);
			board_set(b, row, col, val);
		}
	}
}

void board_linecheck(board_t* b)
{
	for (int row = 0; row < b->height; row++) {
		
		bool row_cleared = true;
		
		for (int col = 0; col < b->width; col++)
			if (board_get(b, row, col) == 0)
				row_cleared = false;
		
		if (row_cleared) {
			for (int col = 0; col < b->width; col++)
				board_set(b, row, col, 0);
			board_shift(b, row);
			b->score++;
		}
	}
}

void board_init(board_t* b, int width, int height)
{
	b->level  = INITIAL_LEVEL;
	b->score  = INITIAL_SCORE;
	b->width  = width;
	b->height = height;
	b->cells  = calloc(height, sizeof(int*));
	for (int row = 0; row < height; row++)
		b->cells[row] = calloc(width, sizeof(int*));
}

void board_destroy(board_t* b)
{
	for (int row = 0; row < b->height; row++)
		free(b->cells[row]);
	free(b->cells);
}

void board_set(board_t* b, int row, int col, int val)
{
	TRACE("%s(%p, %d, %d, %d)\n", __func__, (void*)b, row, col, val);
	if (row >= 0 && row < b->height)
		if (col >= 0 && col < b->width)
			b->cells[row][col] = val;
}

int board_get(board_t* b, int row, int col)
{
	TRACE("%s(%p, %d, %d)\n", __func__, (void*)b, row, col);
	if (row < 0)
		return ERR_OFF_TOP;
	if (row >= b->height)
		return ERR_OFF_BOTTOM;
	if (col < 0)
		return ERR_OFF_LEFT;
	if (col >= b->width)
		return ERR_OFF_RIGHT;
	return b->cells[row][col];
}

void board_print(board_t* b)
{
	console_clear();
	printf("Level: %d\n", b->level);
	printf("Score: %d\n", b->score);
	for (int col = 0; col < b->width+2; col++)
		printf("-");
	
	printf("\n");
	for (int row = 0; row < b->height; row++) {
		printf("|");
		for (int col = 0; col < b->width; col++) {
			printf("%c", (b->cells[row][col] == 0 ? ' ' : '#'));
		}
		printf("|\n");
	}
	
	for (int col = 0; col < b->width+2; col++)
		printf("-");
	
	printf("\n");
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
