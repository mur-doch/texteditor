#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "gapbuffer.h"
#include "ncurses.h"
#include "texteditwindow.h"
#include "infowindow.h"

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

int main()
{
    initncurses();
    getmaxyx(stdscr, NUM_LINES, NUM_COLS);
    
    twInit(NUM_LINES - 1, NUM_COLS, 0, 0);
    twLoadFile("test.txt");
    twDraw();

    iwInit(1, NUM_COLS, NUM_LINES - 1, 0);

    while (!iw.shouldExit)
    {
        iwUpdate();
    }

    twDelete();
    endwin();
}