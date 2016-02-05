#include <stdio.h>
#include <stdlib.h>
#include <termios.h>
#include <unistd.h>
#include "tetris_utils.h"

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
	b->cells[row][col] = val;
}

int board_get(board_t* b, int row, int col)
{
	return b->cells[row][col];
}

void board_print(board_t* b)
{
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
