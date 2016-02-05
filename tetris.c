#include <stdio.h>
#include <stdlib.h>
#include "tetris_utils.h"

#define BOARD_WIDTH        10
#define BOARD_HEIGHT       10

#define STARTING_ROW_POS   (1 - MAX_PIECE_HEIGHT)
#define STARTING_COL_POS   ((BOARD_WIDTH - MAX_PIECE_WIDTH) / 2)

int main(void)
{
	console_t console;
	console_init(&console);
	
	board_t board;
	board_init(&board, BOARD_WIDTH, BOARD_HEIGHT);
	
	piece_init();
	
	while (1) {
		piece_t piece = piece_gen();
		
		int row_pos = STARTING_ROW_POS;
		int col_pos = STARTING_COL_POS;
		
		while (1) {
			piece_show(&board, piece, row_pos, col_pos);
			board_print(&board);
			
			keycode_t input = get_input();
			
			piece_hide(&board, piece, row_pos, col_pos);
			
			input_handle(input, &row_pos, &col_pos);
		}
	}
	
	board_destroy(&board);
	console_destroy(&console);
	return 0;
}
