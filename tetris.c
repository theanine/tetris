#include <stdio.h>
#include <stdlib.h>
#include "tetris_utils.h"

#define BOARD_WIDTH    10
#define BOARD_HEIGHT   10

int main(void)
{
	console_t console;
	console_init(&console);
	
	board_t board;
	board_init(&board, BOARD_WIDTH, BOARD_HEIGHT);
	
	piece_init();
	
	// // This example code draws a horizontal bar 4 squares long.
	// board_set(&board, 2, 3, 1);
	// board_set(&board, 2, 4, 1);
	// board_set(&board, 2, 5, 1);
	// board_set(&board, 2, 6, 1);
	
	while (1) {
		board_print(&board);
		
		piece_t piece = piece_gen();
		
		int row_pos = 0;
		int col_pos = 0;
		for (int row = 0; row < MAX_PIECE_HEIGHT; row++)
			for (int col = 0; col < MAX_PIECE_WIDTH; col++)
				if (piece.cells[row][col])
					board_set(&board, row_pos + row, col_pos + col, 1);
		
		keycode_t input = get_input();
		console_clear();
		switch (input) {
			case KEY_LEFT:
				printf("LEFT\n");
				break;
			case KEY_RIGHT:
				printf("RIGHT\n");
				break;
			case KEY_UP:
				printf("UP\n");
				break;
			case KEY_DOWN:
				printf("DOWN\n");
				break;
			default:
				printf("UNKNOWN\n");
				break;
		}
	}
	
	board_destroy(&board);
	console_destroy(&console);
	return 0;
}
