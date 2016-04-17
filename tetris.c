#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "board.h"
#include "piece.h"
#include "console.h"
#include "input.h"
#include "errors.h"
#include "graphics.h"
#include "timer.h"

#define BOARD_WIDTH      10
#define BOARD_HEIGHT     20
#define MAX_HIGH_SCORES  10

int high_scores[MAX_HIGH_SCORES] = {0};

void leaderboard(int score)
{
	FILE* score_file = fopen("leaderboard.txt", "ab+");
	fread("")
	int bytes = fwrite(buf, sizeof(char), sizeof(buf), score_file);
	assert(bytes == sizeof(buf));
	
}

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
	
	timer_init(&board);
	
	graphics_update(&board);
	
	bool gameover = false;
	while (!gameover) {
		board_newpiece(&board);
		timer_resume();
		
		while (!timer_anchored()) {
			keycode_t input = input_pop();
			
			piece_hide(&board);
			input_handle(&board, input);
			piece_show(&board);
			
			graphics_update(&board);
		}
		
		board_linecheck(&board);
		graphics_update(&board);
		gameover = board_gameover(&board);
	}
	
	printf("\n***** GAME OVER!! *****\n");
	int score = board_get_score(&board);
	
	
	board_destroy(&board);
	console_destroy(&console);
	return 0;
}
