#include <stdio.h>
#include <stdlib.h>
#include <termios.h>
#include <unistd.h>
#include <assert.h>
#include <pthread.h>
#include <time.h>
#include "errors.h"
#include "piece.h"

#define DEBUG 0

#if DEBUG
#define TRACE(...)  do { printf(__VA_ARGS__); } while (0)
#else
#define TRACE(...)  do { } while (0)
#endif

piece_t piece_list[PIECE_COUNT] = {0};

const char* piece_to_str(piece_name_t piece)
{
	switch (piece) {
		case PIECE_STRAIGHT:   return "STRAIGHT";
		case PIECE_SQUARE:     return "SQUARE";
		case PIECE_T:          return "T";
		case PIECE_L:          return "L";
		case PIECE_L_REVERSE:  return "L_REVERSE";
		case PIECE_S:          return "S";
		case PIECE_S_REVERSE:  return "S_REVERSE";
		default:               return "UNKNOWN";
	}
}

void piece_init(void)
{
	// PIECE_STRAIGHT
	piece_list[PIECE_STRAIGHT].name = PIECE_STRAIGHT;
	piece_list[PIECE_STRAIGHT].rotated_degrees = 0;
	piece_list[PIECE_STRAIGHT].cells[2][0] = 1;
	piece_list[PIECE_STRAIGHT].cells[2][1] = 1;
	piece_list[PIECE_STRAIGHT].cells[2][2] = 1;
	piece_list[PIECE_STRAIGHT].cells[2][3] = 1;
	piece_list[PIECE_STRAIGHT].color = COLOR_CYAN;
	
	// PIECE_SQUARE
	piece_list[PIECE_SQUARE].name = PIECE_SQUARE;
	piece_list[PIECE_SQUARE].rotated_degrees = 0;
	piece_list[PIECE_SQUARE].cells[1][1] = 1;
	piece_list[PIECE_SQUARE].cells[1][2] = 1;
	piece_list[PIECE_SQUARE].cells[2][1] = 1;
	piece_list[PIECE_SQUARE].cells[2][2] = 1;
	piece_list[PIECE_SQUARE].color = COLOR_YELLOW;
	
	// PIECE_T
	piece_list[PIECE_T].name = PIECE_T;
	piece_list[PIECE_T].rotated_degrees = 0;
	piece_list[PIECE_T].cells[1][2] = 1;
	piece_list[PIECE_T].cells[2][1] = 1;
	piece_list[PIECE_T].cells[2][2] = 1;
	piece_list[PIECE_T].cells[2][3] = 1;
	piece_list[PIECE_T].color = COLOR_MAGENTA;
	
	// PIECE_L
	piece_list[PIECE_L].name = PIECE_L;
	piece_list[PIECE_L].rotated_degrees = 0;
	piece_list[PIECE_L].cells[0][1] = 1;
	piece_list[PIECE_L].cells[1][1] = 1;
	piece_list[PIECE_L].cells[2][1] = 1;
	piece_list[PIECE_L].cells[2][2] = 1;
	piece_list[PIECE_L].color = COLOR_WHITE;
	
	// PIECE_L_REVERSE
	piece_list[PIECE_L_REVERSE].name = PIECE_L_REVERSE;
	piece_list[PIECE_L_REVERSE].rotated_degrees = 0;
	piece_list[PIECE_L_REVERSE].cells[0][2] = 1;
	piece_list[PIECE_L_REVERSE].cells[1][2] = 1;
	piece_list[PIECE_L_REVERSE].cells[2][2] = 1;
	piece_list[PIECE_L_REVERSE].cells[2][1] = 1;
	piece_list[PIECE_L_REVERSE].color = COLOR_BLUE;
	
	// PIECE_S
	piece_list[PIECE_S].name = PIECE_S;
	piece_list[PIECE_S].rotated_degrees = 0;
	piece_list[PIECE_S].cells[1][2] = 1;
	piece_list[PIECE_S].cells[1][3] = 1;
	piece_list[PIECE_S].cells[2][1] = 1;
	piece_list[PIECE_S].cells[2][2] = 1;
	piece_list[PIECE_S].color = COLOR_GREEN;
	
	// PIECE_S_REVERSE
	piece_list[PIECE_S_REVERSE].name = PIECE_S_REVERSE;
	piece_list[PIECE_S_REVERSE].rotated_degrees = 0;
	piece_list[PIECE_S_REVERSE].cells[1][1] = 1;
	piece_list[PIECE_S_REVERSE].cells[1][2] = 1;
	piece_list[PIECE_S_REVERSE].cells[2][2] = 1;
	piece_list[PIECE_S_REVERSE].cells[2][3] = 1;
	piece_list[PIECE_S_REVERSE].color = COLOR_RED;
	
	srand(time(NULL));
}

piece_t piece_gen(void)
{
	// return piece_list[PIECE_STRAIGHT];
	piece_name_t name = rand() % PIECE_COUNT;
	TRACE("generated a %d\n", name);
	return piece_list[name];
}

bool piece_get_cell(piece_t* piece, int y, int x)
{
	switch (piece->name) {
		
		// square doesn't need rotation
		case PIECE_SQUARE:
			return piece->cells[y][x];
		
		// these pieces only need 180 rotation
		case PIECE_S:
		case PIECE_S_REVERSE:
		case PIECE_STRAIGHT:
			if (piece->rotated_degrees % 180 == 0)
				return piece->cells[y][x];
			return piece->cells[x][MAX_PIECE_HEIGHT-y-1];
		
		// these pieces need all rotations
		case PIECE_T:
		case PIECE_L:
		case PIECE_L_REVERSE:
			if (piece->rotated_degrees == 90)
				return piece->cells[MAX_PIECE_HEIGHT-x-1][y];
			if (piece->rotated_degrees == 180)
				return piece->cells[MAX_PIECE_HEIGHT-y-1][MAX_PIECE_HEIGHT-x-1];
			if (piece->rotated_degrees == 270)
				return piece->cells[x][MAX_PIECE_HEIGHT-y-1];
			return piece->cells[y][x];
		
		default:
			assert(false);
	}
}

void piece_visibility(board_t* board, piece_t piece, int row, int col, bool shown)
{
	color_t color = shown ? piece.color : COLOR_NONE;
	for (int y = 0; y < MAX_PIECE_HEIGHT; y++)
		for (int x = 0; x < MAX_PIECE_WIDTH; x++)
			if (piece_get_cell(&piece, y, x))
				board_set(board, y + row, x + col, color);
}

void piece_show(board_t* board, piece_t piece, int row, int col)
{
	piece_visibility(board, piece, row, col, true);
}

void piece_hide(board_t* board, piece_t piece, int row, int col)
{
	piece_visibility(board, piece, row, col, false);
}

void piece_rotate(piece_t* piece)
{
	piece->rotated_degrees += 90;
	piece->rotated_degrees %= 360;
}

bool piece_collision_check_bottom(board_t* b, int row, int col)
{
	int color = board_get(b, row, col);
	TRACE("%s: %d\n", __func__, color);
	if (color != COLOR_NONE && color > 0)
		return true;
	
	return (color == ERR_OFF_BOTTOM);
}

bool piece_collision_check_top(board_t* b, int row, int col)
{
	int color = board_get(b, row, col);
	return (color == ERR_OFF_TOP);
}

bool piece_collision_check(board_t* b, piece_t piece, int row, int col)
{
	TRACE("%s(%d, %d)\n", __func__, row, col);
	
	for (int y = 0; y < MAX_PIECE_HEIGHT; y++) {
		for (int x = 0; x < MAX_PIECE_WIDTH; x++) {
			if (piece_get_cell(&piece, y, x)) {
				int color = board_get(b, row + y, col + x);
				if (color != COLOR_NONE && color != ERR_OFF_TOP) {
					TRACE("Found a collision at (%d, %d) with color %d\n", row + y, col + x, color);
					return true;
				}
			}
		}
	}
	TRACE("%s: No collisions.\n", __func__);
	return false;
}

bool piece_anchor_check(board_t* b, piece_t piece, int row, int col)
{
	TRACE("%s(%d, %d)\n", __func__, row, col);
	
	for (int y = MAX_PIECE_HEIGHT-1; y > 0; y--) {
		for (int x = 0; x < MAX_PIECE_WIDTH; x++) {
			if (piece_get_cell(&piece, y, x)) {
				if (piece_collision_check_bottom(b, y + row + 1, x + col)) {
					TRACE("Detected a collision with [%d,%d] at [%d,%d]\n", y, x, y + row + 1, x + col);
					return true;
				}
			}
		}
	}
	
	TRACE("%s: No collisions.\n", __func__);
	return false;
}
