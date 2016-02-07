#include <stdio.h>
#include <stdlib.h>
#include <termios.h>
#include <unistd.h>
#include <assert.h>
#include <pthread.h>
#include <time.h>
#include "errors.h"
#include "input.h"
#include "board.h"
#include "piece.h"

#define DEBUG 0

#if DEBUG
#define TRACE(...)  do { printf(__VA_ARGS__); } while (0)
#else
#define TRACE(...)  do { } while (0)
#endif

#define INPUT_BUFFER_SIZE   10
keycode_t input_buffer[INPUT_BUFFER_SIZE];
int input_produce_count = 0;
int input_consume_count = 0;
static pthread_t input_thread;

void input_handle(board_t* b, keycode_t input)
{
	TRACE("%s()\n", __func__);
	
	int new_row       = b->row;
	int new_col       = b->col;
	piece_t new_piece = b->piece;
	
	switch (input) {
		case KEY_LEFT:
			new_col--;
			break;
		case KEY_RIGHT:
			new_col++;
			break;
		case KEY_UP:
			piece_rotate(&new_piece);
			int collision = piece_get_collision(b, &new_piece, new_row, new_col);
			if (collision == ERR_OFF_LEFT)
				new_col++;
			if (collision == ERR_OFF_RIGHT)
				new_col--;
			if (collision == ERR_OFF_BOTTOM)
				new_row--;
			break;
		case KEY_DOWN:
			new_row++;
			break;
		case KEY_SPACE:
			while (!piece_collision_check(b, &new_piece, new_row, new_col))
				new_row++;
			new_row--;
			break;
		case KEY_ENTER:
			// nothing for now
		default:
			return;
	}
	
	if (piece_collision_check(b, &new_piece, new_row, new_col))
		return;
	
	b->row   = new_row;
	b->col   = new_col;
	b->piece = new_piece;
}

void input_queue(keycode_t key)
{
	// check for full buffer
	while (input_produce_count - input_consume_count == INPUT_BUFFER_SIZE);
	
	// produce key
	input_buffer[input_produce_count % INPUT_BUFFER_SIZE] = key;
	input_produce_count++;
}

keycode_t input_get(void)
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
	while (input_produce_count - input_consume_count == 0)
		sched_yield();
	
	// consume key
	keycode_t key = input_buffer[input_consume_count % INPUT_BUFFER_SIZE];
	input_consume_count++;
	return key;
}

void* input_handler(void* unused)
{
	while (1) {
		keycode_t input = input_get();
		input_queue(input);
	}
	pthread_exit(0);
}

int input_init(void)
{
	if (pthread_create(&input_thread, NULL, &input_handler, NULL) != 0)
		return ERR_GENERIC;
	return SUCCESS;
}
