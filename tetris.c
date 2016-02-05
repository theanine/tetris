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
	
	bool gameover = false;
	while (!gameover) {
		piece_t piece = piece_gen();
		
		int row = STARTING_ROW_POS;
		int col = STARTING_COL_POS;
		
		bool anchored = false;
		while (!anchored) {
			piece_show(&board, piece, row, col);
			board_print(&board);
			
			keycode_t input = get_input();
			
			piece_hide(&board, piece, row, col);
			
			input_handle(&board, input, &piece, &row, &col);
			
			anchored = anchor_check(&board, piece, row, col);
		}
		
		piece_show(&board, piece, row, col);
		board_linecheck(&board);
		gameover = board_gameover(&board, piece, row, col);
	}
	
	printf("\n***** GAME OVER!! *****\n");
	board_destroy(&board);
	console_destroy(&console);
	return 0;
}
