#include <stdio.h>
#include <stdlib.h>
#include <termios.h>
#include <unistd.h>
#include <assert.h>
#include <pthread.h>
#include "tetris_utils.h"
#include "errors.h"

#define TOTAL_PIECES    1

piece_t piece_list[TOTAL_PIECES] = {0};

#define DEBUG 0

#if DEBUG
#define TRACE(...)  do { printf(__VA_ARGS__); } while (0)
#else
#define TRACE(...)  do { } while (0)
#endif

void piece_init(void)
{
	piece_list[PIECE_STRAIGHT].cells[2][0] = 1;
	piece_list[PIECE_STRAIGHT].cells[2][1] = 1;
	piece_list[PIECE_STRAIGHT].cells[2][2] = 1;
	piece_list[PIECE_STRAIGHT].cells[2][3] = 1;
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

void rotate_piece(piece_t* piece)
{
	assert(MAX_PIECE_WIDTH == MAX_PIECE_HEIGHT);
	
	int start_idx = 0;
	int last_idx  = MAX_PIECE_WIDTH - 1;
	
	for (int i=start_idx; i<last_idx; i++) {
		bool tmp = piece->cells[i][last_idx];
		piece->cells[i][last_idx] = piece->cells[start_idx][i]; 
		piece->cells[start_idx][i] = piece->cells[last_idx-i][start_idx];
		piece->cells[last_idx-i][start_idx] = piece->cells[last_idx][last_idx-i]; 
		piece->cells[last_idx][last_idx-i] = tmp;
	}
	
	bool tmp = piece->cells[1][1];
	piece->cells[1][1] = piece->cells[2][1];
	piece->cells[2][1] = piece->cells[2][2];
	piece->cells[2][2] = piece->cells[1][2];
	piece->cells[1][2] = tmp;
	
}

void input_handle(board_t* b, keycode_t input, piece_t* piece, int* row, int* col)
{
	int new_row       = *row;
	int new_col       = *col;
	piece_t new_piece = *piece;
	
	switch (input) {
		case KEY_LEFT:
			new_col--;
			break;
		case KEY_RIGHT:
			new_col++;
			break;
		case KEY_UP:
			rotate_piece(&new_piece);
			break;
		case KEY_DOWN:
			new_row++;
			break;
		case KEY_SPACE:
			while (!collision_check(b, new_piece, new_row, new_col))
				new_row++;
			new_row--;
			break;
		case KEY_ENTER:
			// nothing for now
		default:
			return;
	}
	
	if (collision_check(b, new_piece, new_row, new_col))
		return;
	
	*row   = new_row;
	*col   = new_col;
	*piece = new_piece;
}

#define INPUT_BUFFER_SIZE   10
keycode_t input_buffer[INPUT_BUFFER_SIZE];
int input_produce_count = 0;
int input_consume_count = 0;

void input_queue(keycode_t key)
{
	// check for full buffer
	while (input_produce_count - input_consume_count == INPUT_BUFFER_SIZE);
	
	// produce key
	input_buffer[input_produce_count % INPUT_BUFFER_SIZE] = key;
	input_produce_count++;
}

keycode_t get_input(void)
{
	// 27 and 29 are the first two characters of an arrow key input
	int c = getchar();
	if (c == 27) {
		c = getchar();
		if (c != 91)
			return KEY_UNKNOWN;
		return (keycode_t)getchar();
	}
	
	if (c == '\n' || c == ' ')
		return (keycode_t)c;
	
	return KEY_UNKNOWN;
}

keycode_t input_pop(void)
{
	// check for empty buffer
	if (input_produce_count - input_consume_count == 0)
		return KEY_UNKNOWN;
	
	// consume key
	keycode_t key = input_buffer[input_consume_count % INPUT_BUFFER_SIZE];
	input_consume_count++;
	return key;
}

void* input_handler(void* unused)
{
	while (1) {
		keycode_t input = get_input();
		input_queue(input);
	}
	pthread_exit(0);
}

static pthread_t input_thread;

int input_init(void)
{
	if (pthread_create(&input_thread, NULL, &input_handler, NULL) != 0)
		return ERR_GENERIC;
	return SUCCESS;
}

bool collision_check_bottom(board_t* b, int row, int col)
{
	int val = board_get(b, row, col);
	return (val || val == ERR_OFF_BOTTOM);
}

bool collision_check_top(board_t* b, int row, int col)
{
	int val = board_get(b, row, col);
	return (val == ERR_OFF_TOP);
}

bool collision_check(board_t* b, piece_t piece, int row, int col)
{
	TRACE("%s(%d, %d)\n", __func__, row, col);
	
	for (int y = 0; y < MAX_PIECE_HEIGHT; y++) {
		for (int x = 0; x < MAX_PIECE_WIDTH; x++) {
			if (piece.cells[y][x]) {
				int val = board_get(b, row + y, col + x);
				if (val != 0)
					return true;
			}
		}
	}
	return false;
}

bool anchor_check(board_t* b, piece_t piece, int row, int col)
{
	TRACE("%s(%d, %d)\n", __func__, row, col);
	
	for (int y = MAX_PIECE_HEIGHT-1; y > 0; y--)
		for (int x = 0; x < MAX_PIECE_WIDTH; x++)
			if (piece.cells[y][x])
				if (collision_check_bottom(b, y + row + 1, x + col))
					return true;
	
	return false;
}

bool board_gameover(board_t* b, piece_t piece, int row, int col)
{
	for (int y = 0; y < MAX_PIECE_HEIGHT; y++)
		for (int x = 0; x < MAX_PIECE_WIDTH; x++)
			if (piece.cells[y][x])
				if (collision_check_top(b, y + row, x + col))
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
	b->score  = 0;
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

#define DIFFICULTY_MULTIPLIER    1000000

// drop time is in microseconds
int difficulty_to_droptime(int difficulty)
{
	assert(difficulty == 1);
	
	return (DIFFICULTY_MULTIPLIER / difficulty);
}
