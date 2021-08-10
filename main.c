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
    // twLoadFile("test.txt");
    twDraw();

    iwInit(1, NUM_COLS, NUM_LINES - 1, 0);

    while (!tw.shouldExit)
    {
        if (!tw.waitingForFilename)
        {
            twUpdate();
        }
        else
        {
            iwUpdate();
            if (iw.shouldExit)
            {
                tw.waitingForFilename = 0;
                iw.shouldExit = 0;
            }
            else if (iw.filenameReady)
            {
                char *filename = gbGetString(iw.gb);
                twReceiveFilename(filename);
                iw.filenameReady = 0;
            }
        }
    }

    twDelete();
    iwDelete();
    endwin();
}