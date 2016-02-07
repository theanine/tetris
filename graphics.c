#include <stdio.h>
#include <stdlib.h>
#include "board.h"
#include "console.h"

#define DEBUG 0

#if DEBUG
#define TRACE(...)  do { printf(__VA_ARGS__); } while (0)
#else
#define TRACE(...)  do { } while (0)
#endif

void graphics_update(board_t* b)
{
	console_clear();
	
	printf("Level: %d\n", b->level);
	printf("Score: %d\n", b->score);
	
	printf("┌");
	for (int col = 0; col < b->width; col++)
		printf("─");
	printf("┐");
	
	printf("\n");
	for (int row = 0; row < b->height; row++) {
		printf("│");
		for (int col = 0; col < b->width; col++) {
color_t board_get(board_t* b, int row, int col);
			if (b->cells[row][col] != COLOR_NONE) {
				console_setcolor(b->cells[row][col]);
				printf("█");
				console_setcolor(COLOR_NORMAL);
			} else {
				printf(" ");
			}
		}
		printf("│\n");
	}
	
	printf("└");
	for (int col = 0; col < b->width; col++)
		printf("─");
	printf("┘");
	
	printf("\n");
}
