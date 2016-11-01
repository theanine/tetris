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

#define PREVIEW_ROW_START     1
#define PREVIEW_ROW_END       5
#define BUFFERED_BOX_DRAWING  false

void graphics_print_color(color_t color)
{
	if (color == COLOR_NONE) {
		printf(" ");
	} else {
		console_setcolor(color);
		printf("█");
		console_setcolor(COLOR_NORMAL);
	}
}

void graphics_update(board_t* b)
{
	console_clear();
	
	printf("Level: %d\n", b->level);
	printf("Score: %d\n", b->score);
	printf("Pieces: %d\n", b->piece_count);

	printf("%s", BUFFERED_BOX_DRAWING ? "┌" : " ");
	for (int col = 0; col < b->width; col++)
		printf("%s", BUFFERED_BOX_DRAWING ? "─" : "▁");
	printf("%s", BUFFERED_BOX_DRAWING ? "┐" : "");
	
	printf("\n");
	for (int row = 0; row < b->height; row++) {
		printf("%s", BUFFERED_BOX_DRAWING ? "│" : "▕");
		for (int col = 0; col < b->width; col++) {
			color_t color = board_get(b, row, col);
			graphics_print_color(color);
		}
		printf("%s", BUFFERED_BOX_DRAWING ? "│" : "▏");
		
		if (row == PREVIEW_ROW_START - 1)
			printf("  NEXT");
		
		if (row == PREVIEW_ROW_START)
			printf("%s", BUFFERED_BOX_DRAWING ? " ┌────┐" : " ▁▁▁▁");
		
		if (row > PREVIEW_ROW_START && row < PREVIEW_ROW_END) {
			printf("%s", BUFFERED_BOX_DRAWING ? "│" : "▕");
			for (int i=0; i<MAX_PIECE_WIDTH; i++) {
				color_t color = board_get_next_piece(b, row - PREVIEW_ROW_START - 1, i);
				graphics_print_color(color);
			}
			printf("%s", BUFFERED_BOX_DRAWING ? "│" : "▏");
		}
		
		if (row == PREVIEW_ROW_END)
			printf("%s", BUFFERED_BOX_DRAWING ? " └────┘" : " ▔▔▔▔");
		
		printf("\n");
	}
	
	printf("%s", BUFFERED_BOX_DRAWING ? "└" : " ");
	for (int col = 0; col < b->width; col++)
		printf("%s", BUFFERED_BOX_DRAWING ? "─" : "▔");
	printf("%s", BUFFERED_BOX_DRAWING ? "┘" : "");
	
	printf("\n");
}
