#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "board.h"
#include "piece.h"
#include "console.h"
#include "input.h"
#include "errors.h"

#define BOARD_WIDTH                10
#define BOARD_HEIGHT               20
#define STARTING_ROW_POS           (1 - MAX_PIECE_HEIGHT)
#define STARTING_COL_POS           ((BOARD_WIDTH - MAX_PIECE_WIDTH) / 2)
#define INPUT_CHECK_FREQ           20  // fraction of drop time
#define DIFFICULTY_ACCELERATION    10  // in pieces

int main(void)
{
	console_t console;
	console_init(&console);
	
	board_t board;
	board_init(&board, BOARD_WIDTH, BOARD_HEIGHT);
	
	piece_init();
	
	if (input_init() != SUCCESS) {
		printf("ERROR: failed to initialize input thread!\n");
		exit(1);
	}
	
	int droptime = board_getdroptime(&board);
	int time_passed = 0;
	int piece_count = 0;
	
	bool gameover = false;
	while (!gameover) {
		piece_t piece = piece_gen();
		piece_count++;
		if (piece_count % DIFFICULTY_ACCELERATION == 0) {
			board_levelup(&board);
			droptime = board_getdroptime(&board);
		}
		
		int row = STARTING_ROW_POS;
		int col = STARTING_COL_POS;
		
		bool anchored = false;
		while (!anchored) {
			piece_show(&board, piece, row, col);
			board_print(&board);
			
			keycode_t input = input_pop();
			
			piece_hide(&board, piece, row, col);
			
			input_handle(&board, input, &piece, &row, &col);
			
			if (time_passed >= droptime) {
				input_handle(&board, KEY_DOWN, &piece, &row, &col);
				time_passed -= droptime;
			}
			
			anchored = piece_anchor_check(&board, piece, row, col);
			
			int wait = droptime / INPUT_CHECK_FREQ;
			time_passed += wait;
			usleep(wait);
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
