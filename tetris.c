#include "tetris_utils.h"

int main()
{
    struct console console;
    console_init(&console);

    struct board board;
    board_init(&board, 10, 10);

    // This example code draws a horizontal bar 4 squares long.
    board_set(&board, 2, 3, 1);
    board_set(&board, 2, 4, 1);
    board_set(&board, 2, 5, 1);
    board_set(&board, 2, 6, 1);
    board_print(&board);

    // This example takes input from the console and prints a debug message
    // if an arrow was pressed.
    while(1) {
        enum keycode input = get_input();
        if (input == KEY_LEFT) {
            printf("LEFT\n");
        } else if (input == KEY_RIGHT) {
            printf("RIGHT\n");
        } else if (input == KEY_UP) {
            printf("UP\n");
        } else if (input == KEY_DOWN) {
            printf("DOWN\n");
        }
    }

    board_destroy(&board);
    console_destroy(&console);

    return 0;
}
