#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "gapbuffer.h"
#include "ncurses.h"
#include "texteditwindow.h"
#include "infowindow.h"
#include "windowstatemachine.h"

int NUM_LINES, NUM_COLS;

void initncurses()
{
    initscr();
    cbreak();
    noecho();
    nodelay(stdscr, TRUE);
    // curs_set(0);
    keypad(stdscr, TRUE);
    // initColours();
}

int main(int argc, char *argv[])
{
    initncurses();
    getmaxyx(stdscr, NUM_LINES, NUM_COLS);
    
    twInit(NUM_LINES - 1, NUM_COLS, 0, 0);
    if (argc > 1)
    {
        twLoadFile(argv[1]);
    }
    // twDrawLines();
    twDraw();

    iwInit(1, NUM_COLS, NUM_LINES - 1, 0);

    while (!wsm.shouldExit)
    {
        wsmUpdate();
    }

    twDelete();
    iwDelete();
    endwin();
}