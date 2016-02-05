#ifndef TETRIS_UTILS
#define TETRIS_UTILS

#include <stdio.h>
#include <stdlib.h>
#include <termios.h>
#include <unistd.h>

// struct console handles putting the terminal in raw mode for input
struct console {
    struct termios old_t;
};

enum keycode {
    KEY_UNKNOWN = -1,
    KEY_UP = 65,
    KEY_DOWN = 66,
    KEY_RIGHT = 67,
    KEY_LEFT = 68
};

void console_init(struct console *c) {
    struct termios t;
    tcgetattr(STDIN_FILENO, &t);
    c->old_t = t;
    t.c_lflag &= (~ICANON & ~ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &t);
}

void console_destroy(struct console *c) {
    tcsetattr(STDIN_FILENO, TCSANOW, &c->old_t);
}

enum keycode get_input() {
    // 27 and 29 are the first two characters of an arrow key input
    int c = getchar();
    if (c != 27) {
        return KEY_UNKNOWN;
    }
    c = getchar();
    if (c != 91) {
        return KEY_UNKNOWN;
    }
    return (enum keycode)getchar();
}

// struct board represents the tetris board state
struct board {
    int width;
    int height;
    int **cells;
};

void board_init(struct board *b, int width, int height) {
    b->width = width;
    b->height = height;
    b->cells = calloc(height, sizeof(int*));
    for (int row = 0; row < height; row++) {
        b->cells[row] = calloc(width, sizeof(int*));
    }
}

void board_destroy(struct board *b) {
    for (int row = 0; row < b->height; row++) {
        free(b->cells[row]);
    }
    free(b->cells);
}

void board_set(struct board *b, int row, int col, int val) {
    b->cells[row][col] = val;
}

int board_get(struct board *b, int row, int col) {
    return b->cells[row][col];
}

void board_print(struct board *b) {
    for (int col = 0; col < b->width+2; col++) {
        printf("-");
    }
    printf("\n");
    for (int row = 0; row < b->height; row++) {
        printf("|");
        for (int col = 0; col < b->width; col++) {
            printf("%c", (b->cells[row][col] == 0 ? ' ' : '#'));
        }
        printf("|\n");
    }
    for (int col = 0; col < b->width+2; col++) {
        printf("-");
    }
    printf("\n");
}

#endif //ifndef TETRIS_UTILS
