#include <stdio.h>
#include <stdlib.h>
#include <termios.h>
#include <unistd.h>
#include "tetris_utils.h"

#define TOTAL_PIECES    1

#define SUCCESS         0
#define ERR_GENERIC    -1
#define ERR_PARAMS     -2

piece_t piece_list[TOTAL_PIECES] = {0};

#define DEBUG 0

#if DEBUG
#define TRACE(...)  do { printf(__VA_ARGS__); } while (0)
#else
#define TRACE(...)  do { } while (0)
#endif

void piece_init(void)
{
	piece_list[PIECE_STRAIGHT].cells[3][0] = 1;
	piece_list[PIECE_STRAIGHT].cells[3][1] = 1;
	piece_list[PIECE_STRAIGHT].cells[3][2] = 1;
	piece_list[PIECE_STRAIGHT].cells[3][3] = 1;
}

piece_t piece_gen(void)
{
	return piece_list[PIECE_STRAIGHT];
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

void console_init(console_t* c)
{
	struct termios t;
	tcgetattr(STDIN_FILENO, &t);
	c->old_t = t;
	t.c_lflag &= (~ICANON & ~ECHO);
	tcsetattr(STDIN_FILENO, TCSANOW, &t);
}

void console_destroy(console_t* c)
{
	tcsetattr(STDIN_FILENO, TCSANOW, &c->old_t);
}

void console_clear(void)
{
	write(STDOUT_FILENO, " \033[1;1H\033[2J", 12);
}

void input_handle(keycode_t input, int* row, int* col)
{
	switch (input) {
		case KEY_LEFT:
			(*col)--;
			break;
		case KEY_RIGHT:
			(*col)++;
			break;
		case KEY_UP:
			// do nothing
			break;
		case KEY_DOWN:
			(*row)++;
			break;
		default:
			// do nothing
			// printf("UNKNOWN\n");
			break;
	}
}

keycode_t get_input(void)
{
	// 27 and 29 are the first two characters of an arrow key input
	int c = getchar();
	if (c != 27)
		return KEY_UNKNOWN;
	
	c = getchar();
	if (c != 91)
		return KEY_UNKNOWN;
	
	return (keycode_t)getchar();
}

bool anchor_check(board_t* b, piece_t piece, int row, int col)
{
	TRACE("%s(%d, %d)\n", __func__, row, col);
	
	// check anchoring with other pieces
	for (int y = MAX_PIECE_HEIGHT-1; y > 0; y--) {
		for (int x = 0; x < MAX_PIECE_WIDTH; x++) {
			if (piece.cells[y][x]) {
				int val = board_get(b, y + row + 1, x + col);
				if (val == ERR_PARAMS || val == true) {
					TRACE("found a collision at (%d, %d)\n", y + row + 1, x + col);
					return true;
				}
			}
		}
	}
	
	return false;
}

void board_init(board_t* b, int width, int height)
{
	b->width = width;
	b->height = height;
	b->cells = calloc(height, sizeof(int*));
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
	if (row < 0 || row >= b->height)
		return ERR_PARAMS;
	if (col < 0 || col >= b->width)
		return ERR_PARAMS;
	return b->cells[row][col];
}

void board_print(board_t* b)
{
	console_clear();
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
